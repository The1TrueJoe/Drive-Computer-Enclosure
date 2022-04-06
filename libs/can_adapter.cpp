#include "can_adapter.h"

// --------- CAN

/** @brief Setup the CAN transceiver */
void setupCAN(int CS_PIN) {
    // Get address from eeprom
    Serial.println("CAN Transceiver: Loading CAN Address");

    // Log init
    Serial.println("CAN Transceiver: Init Starting");

    // Init
    mcp2515 = new MCP2515(CS_PIN);

    // Reset and set
    mcp2515.reset();
    mcp2515.setBitrate(CAN_125KBPS);
    mcp2515.setNormalMode();

    // Log done
    Serial.println("CAN Transceiver: Done");

}

/**
 * @brief Get the CAN message if the id is correct
 * 
 * @return true If message is valid and id's match
 * @return false If message is invalid or id's do not match
 */

bool getCANMessage() {
    if (mcp2515.readMessage(&can_msg_in) == MCP2515::ERROR_OK) {
        if (can_msg_in.can_id == m_can_id) {
            printReceivedCANMessage();
            return true;

        } 
    }

    return false;

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

    // Start log
    Serial.print("CAN-TX: (" + String(can_msg_out.can_id) + ") ");

    // Print data
    for (int i = 0; i < can_msg_out.can_dlc; i++) {
        Serial.print(String(can_msg_out.data[i]) + " ");

    }

    // New Line
    Serial.println();

    // Send message
    mcp2515.sendMessage(&can_msg_out);

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
 * @brief Set the new can address in eeprom
 * 
 * @param new_can_addr New can address
 */

void setCANAddress(uint32_t new_can_addr) {  m_can_id = new_can_addr; }

/**
 * @brief Check the condition and use to prepare a CAN message
 * 
 * @param condition Condition to check
 * 
 * @return uint8_t (0x01 - True) or (0x02 - False)
 */

uint8_t getCANBoolean(bool condition) { return condition ? 0x01 : 0x02; }

/** @brief Convert an 8 bit integer into a standard integer  */
int convertToInt(uint8_t incoming_int) {
    int new_int = incoming_int << 8;

    if (new_int < 0) { new_int = new_int * -1; }
    if (new_int > 255) { new_int = 255; }

    return new_int;

}

/** @brief Convert two 8 bit integers into a standard integer  */
int convertToInt(uint8_t int_1, uint8_t int_2) {
    int new_int = (int_1 << 8) | int_2;

    if (new_int < 0) { new_int = new_int * -1; }
    if (new_int > 255) { new_int = 255; }

    return new_int;
    
}