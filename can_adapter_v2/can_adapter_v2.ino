#include <mcp2515.h>
#include "can_buffer.h"

// LEDs
#define PC_TX_LED 8
#define PC_RX_LED 7
#define BUS_TX_LED 6
#define BUS_RX_LED 5

// CAN Pins
#define CAN_CS 10
#define CAN_INT 2

// Adapter
MCP2515 can_trans(CAN_CS);

// UART Processing
#define MSG_BYTE_LEN 12
uint8_t uart_input_buffer[MSG_BYTE_LEN];

union {float f; unsigned char b[8];} uart_id_comp;

CAN_Buffer bus_in_buff;

CAN_Buffer bus_out_buff;

void setup() {
    // LEDs
    pinMode(BUS_TX_LED, OUTPUT);
    pinMode(BUS_RX_LED, OUTPUT);
    pinMode(PC_TX_LED, OUTPUT);
    pinMode(PC_RX_LED, OUTPUT);

    // LEDs, HIGH
    digitalWrite(BUS_TX_LED, HIGH);
    digitalWrite(BUS_RX_LED, HIGH);
    digitalWrite(PC_TX_LED, HIGH);
    digitalWrite(PC_RX_LED, HIGH);

    // Serial
    Serial.begin(115200);

    // Reset and set
    can_trans.reset();
    can_trans.setBitrate(CAN_125KBPS);
    can_trans.setNormalMode();

    // LEDs, LOW
    delay(200);
    digitalWrite(BUS_TX_LED, LOW);
    digitalWrite(BUS_RX_LED, LOW);
    digitalWrite(PC_TX_LED, LOW);
    digitalWrite(PC_RX_LED, LOW);

    // Setup Interupts
    attachInterrupt(digitalPinToInterrupt(CAN_INT), bus_rx, FALLING);

}

void run() {
    if (Serial.available() > 0) {
        digitalWrite(PC_RX_LED, HIGH);
        noInterrupts();

        Serial.readBytesUntil('\n', uart_input_buffer, MSG_BYTE_LEN);

        can_frame pc_rx_frame;

        for (int i = 0; i < 4; i++) { uart_id_comp.b[i] = uart_input_buffer[i]; }
        pc_rx_frame.can_id = uart_id_comp.f;
        pc_rx_frame.can_dlc = CAN_MAX_DLC;

        for (int i = 4; i < MSG_BYTE_LEN; i++) { pc_rx_frame.data[i-4] = uart_input_buffer[i]; }

        bus_out_buff.add(pc_rx_frame);

        digitalWrite(PC_RX_LED, LOW);
        interrupts();

    }

    if (!bus_out_buff.empty()) {
        digitalWrite(BUS_TX_LED, HIGH);

        while(!bus_out_buff.empty()) {
            can_trans.sendMessage(bus_out_buff.get());

        }

        digitalWrite(BUS_TX_LED, LOW);

    }

    if (!bus_in_buff.empty()) {
        digitalWrite(PC_TX_LED, HIGH);

        while(!bus_in_buff.empty()) {
            print_msg(bus_in_buff.get());

        }

        digitalWrite(PC_TX_LED, LOW);

    }
}

void bus_rx() {
    can_frame bus_rx_frame;

    if (can_trans.readMessage(&bus_rx_frame) == MCP2515::ERROR_OK) {
        digitalWrite(BUS_RX_LED, HIGH);
        bus_in_buff.add(bus_rx_frame);
        digitalWrite(BUS_RX_LED, LOW);

    }
}

void print_msg(can_frame* frame) {
    // Start log
    Serial.print("<(" + String(&frame.can_id) + ") ");

    // Print data
    for (int i = 0; i < &frame.can_dlc; i++) {
        Serial.print(String(&frame.data[i]) + " ");

    }

    // New Line
    Serial.println();
}