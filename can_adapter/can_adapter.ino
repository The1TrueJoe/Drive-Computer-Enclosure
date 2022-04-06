/**
 * @file can_adapter.ino
 * 
 * @author Joseph Telaak
 * 
 * @brief CAN Adapter for the GSSM Auto Golf Cart Drive Computer
 * 
 * @version 0.1
 * @date 2022-04-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */

// CAN
#include "can_adapter.h"
#define CAN_CS 10
#define CAN_INT 2

/**
 * @brief Main Arduino Setup
 * 
 */

void setup() {
    // Init serial port
    Serial.begin(115200);

    // Setup the can bus transceiver
    setupCAN();

    // Setup Interupts
    attachInterupt(digitalPinToInterrupt(CAN_INT), canLoop, FALLING);

}

/**
 * @brief Main Arduino Loop
 * 
 */

void loop() {
    if (Serial.available() > 0) {
        String message = Serial.readString();
        
        if (drive_com_msg.indexOf("CMD-Send: ") != -1) {
            adapterSendMessage(drive_com_msg.replace("CMD-Send: ", ""));

        }
    }
}

/** @brief CAN Message Handling (Runs on interupt) */
void canLoop() {
    // Get message
    if (!getCANMessage()) { return: }

}

/**
 * @brief Sends a message through the CAN Adapter
 * 
 * @param drive_com_msg Message to send (8x bytes separated by spaces)
 */

void adapterSendMessage(String drive_com_msg) {
    // Get the ID indexes
    int id_begin_index = drive_com_msg.indexOf("(");
    int id_end_index = drive_com_msg.indexOf(")");

    // Check ID
    if (id_begin_index == -1 || id_end_index == -1) {
        Serial.println("Err: CAN message is missing ID");
        return;

    }

    // Get the ID
    char* str_id = drive_com_msg.substring(id_begin_index, id_end_index - 1).c_str();
    char* ptr;
    uint32_t set_id = strtoul(str_id, &ptr, 16);

    // Clear ID
    drive_com_msg.replace(drive_com_msg.substring(0, id_end_index + 2), "");

    // Get data
    uint8_t data[8];
    int count = 0;
    
    while (drive_com_msg.length() > 0) {
        int index = drive_com_msg.indexOf(' ');
        
        if (index == -1) {
            data[count++] = atoi(drive_com_msg.c_str());
          
        } else {
            data[count++] = atoi(drive_com_msg.substring(0, index).c_str());
            drive_com_msg = drive_com_msg.substring(index + 1);
            
        }
    }

    // Send message
    sendCANMessage(set_id, data);

}