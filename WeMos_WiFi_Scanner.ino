#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SSD1306.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
 
// SSID and password of the networks to check
// Add as network as desired
String ssids[3] = {"yourSSID1","yourSSID2","yourSSID3"};
String passwords[3] = {"pwdSSID1","pwdSSID2","pwdSSID3"};
 
// Test server IP
const char* host = "xxx.xxx.xxx.xxx";
// Server Port
const int port = 8080;
 
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);
 
// Init objects
WiFiClient wifi;
HTTPClient http;
 
// Connect to  WiFi 
void setup_wifi(String ssid, String password) {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  // Need to convert String to char*
  WiFi.begin(ssid.c_str(), password.c_str());
  // Connecting to WiFi network
  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    count++;
    delay(500);
    Serial.print(".");
    if ( count > 10 ) { return; }
  }
 
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}
 
void setup() {
  pinMode(A0, INPUT);  // Set A0 converter as input to measure the battery tension
  Serial.begin(115200);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialize with the I2C addr 0x3C (for the 64x48)
  display.display(); 
}
 
float getLevel(){
  float raw = analogRead(A0);
  int level = map(raw, 140, 227, 0, 100);     //With a 1M5 resistor
  Serial.print("A0 "); Serial.println(raw);
  
  if ( level < 0 ) { level = 0; }
  if ( level > 100 ) { level = 100; }
  Serial.print("Level: "); Serial.println(level);
  return level;
}
 
float getVoltage(){
  float raw = analogRead(A0);                      
  float volt = map(raw, 140, 227, 338, 511);  // With a 1M5 resistor
  volt = volt / 100;
  Serial.print("A0 "); Serial.println(raw);
  Serial.print("Voltage "); Serial.println(volt);
  return volt;
}
 
void loop() {
  byte available_networks = WiFi.scanNetworks();
  if ( available_networks > 0 ) {
    for (int network = 0; network < available_networks; network++) {
      //delay(1000);
      int level = getLevel();
      long rssi = WiFi.RSSI(network);
      int bars = getBarsSignal(rssi);
      Serial.print("RSSI:");
      Serial.println(rssi);
  
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(32,8);
 
      // Display network name, signal Strength and battery level
      display.print("SSID "); display.println(WiFi.SSID(network));
      display.setCursor(32,16);
      display.print("RSSI:"); display.println(rssi); 
      
      
      // Display signal quality bars
      for (int b=0; b <= bars; b++) {
        display.fillRect(70 + (b*4),33 - (b*4),3,b*4,WHITE); 
        // display.fillRect(10 + (b*5),48 - (b*5),3,b*5,WHITE); 
      }
      
      display.display();
      delay(2000);
 
      // Check if connexion is correct with the server
      for ( int i = 0 ; i < 3 ; i++ ) {
        String _ssid = ssids[i];
        String _pwd = passwords[i];
        String _currentSSID = WiFi.SSID(network);
        if ( _currentSSID == _ssid) { 
          checkServerConnexion(_ssid,_pwd);
        }
      }    
    }  
  } else {
    displayMessage("No Signal !", 1000, 1);
  }
}
 
void checkServerConnexion(String ssid, String password){
  Serial.println("Checking server connexion...");
  displayMessage("Checking server connexion...", 250, 1);
  setup_wifi(ssid, password);
  String _status = ssid;
  http.setTimeout(1000);
  http.begin(host,port, "/checkconnexion");
  int httpCode = http.GET();
  Serial.print("HTTP Code "); Serial.println(httpCode);
  if (httpCode) {
    if (httpCode == 200) {
      Serial.println("Connexion OK");
      _status += " OK";
      displayMessage(_status, 2000,2);
    } else {
      Serial.println("Connexion OK");
      _status += " KO!";
      displayMessage(_status, 2000, 2);
    }
  } else {
    Serial.println("Connexion OK");
    _status += " OK!";
    displayMessage(_status, 2000, 2);
  }
  Serial.println("closing connection");
  http.end();
}
 
void displayMessage(String message, int duration, int Size){
  display.clearDisplay();
  display.setTextSize(Size);
  display.setTextColor(WHITE);
  display.setCursor(32,8);
  display.println(message);
  display.display();
  delay(duration);
}
 
int getBarsSignal(long rssi){
  // 5. High quality: 90% ~= -55db
  // 4. Good quality: 75% ~= -65db
  // 3. Medium quality: 50% ~= -75db
  // 2. Low quality: 30% ~= -85db
  // 1. Unusable quality: 8% ~= -96db
  // 0. No signal
  int bars;
  
  if (rssi > -55) { 
    bars = 4;
  } else if (rssi < -55 & rssi > -65) {
    bars = 3;
  } else if (rssi < -65 & rssi > -75) {
    bars = 2;
  } else if (rssi < -75 & rssi > -85) {
    bars = 1;
  } else if (rssi < -85 & rssi > -96) {
    bars = 0;
   }
  return bars;
}
