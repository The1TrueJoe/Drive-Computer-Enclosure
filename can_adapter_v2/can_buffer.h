//  LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <Arduino.h>
#include "can.h"

#define MAX_BUFFER_LEN 8

class CAN_Buffer {
    private:
        can_frame circular_buffer[MAX_BUFFER_LEN+1] = { 0 };

        uint8_t buffer_length = 0;
        uint8_t read_index = 0;
        uint8_t write_index = 0;

    public:
        CAN_Buffer();

        void add(uint32_t id, uint8_t data[CAN_MAX_DLC]);
        void add(can_frame frame);
        can_frame* get();

        uint8_t get_read_index();
        uint8_t get_write_index();
        uint8_t size();

        bool full();
        bool empty();

};