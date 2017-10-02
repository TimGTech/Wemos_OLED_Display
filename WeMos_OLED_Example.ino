//  OLED example for Wemos D1 Pro Mini with OLED shield in Hackerbox #023
//  Free code to modify and distribute as needed.  Found at http://www.esp8266learning.com/wemos-oled-shield-example.php
//  Modified 10/1/17 by TimGTech

#include <Adafruit_SSD1306.h>

// Initialize display using Afafruit library on GPIO 0.
Adafruit_SSD1306 display(0);
 
void setup()   {

// by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  // init done

  delay (2000);
    display.display();
 
}

  
void loop() {

  //  You can but the above code down here to run in a loop where you might display data that changes such as Temp or WiFi signal.

  // Clear the buffer.
  display.clearDisplay();
 
  // Text to display on the screen.
  
  display.setTextSize(1);  // Recommend leaving this at 1.  Setting to 2 is way too big for this little screen unless you are doing one line of scrolleing text.
  display.setTextColor(WHITE);  // These only do white.
  display.setCursor(32,8); // This is the upper most left pixel on the OLED shield in Hackerbox #023
  display.println("HACKERBOX");
  display.setCursor(32,16); // This starts the 2nd line of text.  Remember, spacing on OLED screens are by pixels and not by characters.
  display.println("#023");
  display.display();
}
