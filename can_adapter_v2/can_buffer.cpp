#include "can_buffer.h"

CAN_Buffer::CAN_Buffer() {
    circular_buffer[8].can_id = 0;
    circular_buffer[8].can_dlc = CAN_MAX_DLC;
    
    for (uint8_t i = 0; i < CAN_MAX_DLC; i++) {
        circular_buffer[8].data[i] = 0;

    }
}

void CAN_Buffer::add(uint32_t id, uint8_t data[CAN_MAX_DLC]) {
    can_frame frame;

    frame.can_id = id;
    frame.can_dlc = CAN_MAX_DLC;
    
    for (uint8_t i = 0; i < CAN_MAX_DLC; i++) {
        frame.data[i] = data[i];

    }

    add(frame);

}

void CAN_Buffer::add(can_frame frame) {
    if (full()) { return; }

    circular_buffer[write_index] = frame;

    buffer_length++;
    write_index++;

    if (write_index == MAX_BUFFER_LEN) { write_index = 0; }

}

can_frame* CAN_Buffer::get() {
    noInterrupts();

    if (empty()) { return &circular_buffer[8]; }

    uint8_t out_index = read_index;

    buffer_length--;
    read_index++;

    if (read_index == MAX_BUFFER_LEN) { read_index = 0; }

    interrupts();
    return &circular_buffer[out_index];

}

uint8_t CAN_Buffer::get_read_index() { return read_index; }

uint8_t CAN_Buffer::get_write_index() { return write_index; }

uint8_t CAN_Buffer::size() { return buffer_length; }

bool CAN_Buffer::empty() { return buffer_length == 0; }

bool CAN_Buffer::full() { return buffer_length == MAX_BUFFER_LEN; }