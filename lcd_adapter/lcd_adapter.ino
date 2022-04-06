/**
 * @file lcd_adapter.ino
 * 
 * @author Joseph Telaak
 * 
 * @brief LCD Adapter for the GSSM Auto Golf Cart Drive Computer
 * 
 * @version 0.1
 * @date 2022-04-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */


// Lib
#include "Adafruit_LiquidCrystal.h"

// LCD Pins
#define Register_Select 7
#define Enable 6
#define D4 5
#define D5 4
#define D6 3
#define D7 2

// LCS
Adafruit_LiquidCrystal lcd(Register_Select, Enable, D4, D5, D6, D7);

/**
 * @brief Main Arduino Setup
 * 
 */

void setup() {
    // Init serial port
    Serial.begin(115200);

    // Setup the Display
    lcd.begin(16, 2);

    // Startup
    lcd.print("GSSM AutoCart");
    lcd.setCursor(0, 1);
    lcd.print("Adapter Start");


}

/**
 * @brief Main Arduino Loop
 * 
 */

void loop() {
    if (Serial.available() > 0) {
        String message = Serial.readString();
        
        if (drive_com_msg.indexOf("Display: ") != -1) {
            display(drive_com_msg.replace("Display: ", ""));

        }
    }
}

/**
 * @brief Print a message to the drive computer LCD
 * 
 * @param drive_com_msg message to display ("L{line num}: {message}")
 */

void display(String drive_com_msg) {
    if (drive_com_msg.indexOf("L1: ") != -1) {
        drive_com_msg = drive_com_msg.replace("L1: ", "");
        lcd.setCursor(0, 0);
        lcd.print(drive_com_msg);

    } else if (drive_com_msg.indexOf("L2: ") != -1) {
        drive_com_msg = drive_com_msg.replace("L2: ", "");
        lcd.setCursor(0, 1);
        lcd.print(drive_com_msg);

    } else if (drive_com_msg.indexOf("Clear") != -1) {
        lcd.setCursor(0, 0);
        lcd.print("");
        lcd.setCursor(0, 1);
        lcd.print("");

    } else if (drive_com_msg.indexOf("Off") != -1) {
        lcd.noDisplay();

    } else if (drive_com_msg.indexOf("On") != -1) {
        lcd.display();

    }
}