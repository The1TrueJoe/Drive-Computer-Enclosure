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

// Arduino
#include "Arduino.h"

// CAN
#include "mcp2515.h"
MCP2515* can_trans;

// CAN Pins
#define CAN_CS 10
#define CAN_INT 2

//  Message Buffer
struct can_frame can_msg_in;
struct can_frame can_msg_out;

// CAN Info
uint32_t m_can_id = 0x000;
uint8_t m_can_dlc = 8;

// LEDs
#define TX_LED 6
#define RX_LED 5

/**
 * @brief Main Arduino Setup
 * 
 */

void setup() {
    // LEDs
    pinMode(TX_LED, OUTPUT);
    pinMode(RX_LED, OUTPUT);

    // Init serial port
    Serial.begin(115200);

    // Init
    can_trans = new MCP2515(CAN_CS);

    // Reset and set
    can_trans -> reset();
    can_trans -> setBitrate(CAN_125KBPS);
    can_trans -> setNormalMode();

    // Setup Interupts
    attachInterrupt(digitalPinToInterrupt(CAN_INT), canLoop, FALLING);

}

/**
 * @brief Main Arduino Loop
 * 
 */

void loop() {
    if (Serial.available() > 0) {
        String message = Serial.readString();
        
        if (message.indexOf("CMD-Send: ") != -1) {
            message.replace("CMD-Send: ", "");

            // Send Message
            digitalWrite(TX_LED, HIGH);
            adapterSendMessage(message);
            digitalWrite(TX_LED, LOW);

        }
    }
}

/** @brief CAN Message Handling (Runs on interupt) */
void canLoop() {
    // Get message
    if (getCANMessage()) { printReceivedCANMessage(); }

}

/**
 * @brief Get the CAN message if the id is correct
 * 
 * @return true If message is valid and id's match
 * @return false If message is invalid or id's do not match
 */

bool getCANMessage() {
    if (can_trans -> readMessage(&can_msg_in) == MCP2515::ERROR_OK) {
        if (can_msg_in.can_id == m_can_id) {
            // Print Message
            digitalWrite(RX_LED, HIGH);
            printReceivedCANMessage();
            digitalWrite(RX_LED, LOW);

            return true;

        } 
    }

    return false;

}

/** @brief Print out the received can frame*/
void printReceivedCANMessage() {
    // Start log
    Serial.print("CAN-RX: (" + String(can_msg_in.can_id) + ") ");

    // Print data
    for (int i = 0; i < can_msg_in.can_dlc; i++) {
        Serial.print(String(can_msg_in.data[i]) + " ");

    }

    // New Line
    Serial.println();

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
    const char* str_id = drive_com_msg.substring(id_begin_index, id_end_index - 1).c_str();
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

/**
 * @brief Send a message of the can bus
 * 
 * @param id ID of the CAN device to send message to
 * @param m_data Data to send to the CAN device
 */

void sendCANMessage(uint32_t id, uint8_t m_data[8]) {
    // Assign ID
    can_msg_out.can_id = id;

    // Assign dlc
    can_msg_out.can_dlc = m_can_dlc;

    // Map data
    for (int i = 0; i < m_can_dlc; i++) {
        can_msg_out.data[i] = m_data[i];

    }

    // Send message
    can_trans -> sendMessage(&can_msg_out);

}