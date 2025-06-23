#define XCHOP
#ifdef XCHOP
#ifndef I8042_H
#define I8042_H

#include <stdint.h>
#include <stdbool.h>
#include "../cpu/cpu.h"
#include "i8259.h"

typedef struct {
    uint8_t data_buffer[8]; // Data buffer for port 0x60
    uint8_t buflen;         // Current buffer size
    uint8_t status;         // Status register (port 0x64)
    uint8_t config;         // Configuration byte
    uint8_t command;        // Last command received (port 0x64)
    bool reset_requested;   // Flag to signal CPU reset
    bool self_test_done;    // Self-test completion flag
    bool keyboard_enabled;
    CPU_t* cpu;
    I8259_t* i8259;
} i8042_t;

void i8042_buffer_key_data(const uint8_t* data, uint8_t len, uint8_t doirq);
void i8042_init(CPU_t* cpu, I8259_t* i8259);

#endif
#endif

#ifndef I8042_H
#define I8042_H

#include <stdint.h>
#include <stdbool.h>
#include "../cpu/cpu.h"

typedef struct {
    uint8_t data_buffer;   // Data buffer for port 0x60
    uint8_t status;        // Status register (port 0x64)
    uint8_t command;       // Last command received (port 0x64)
    bool reset_requested;  // Flag to signal CPU reset
    bool self_test_done;   // Self-test completion flag
    bool keyboard_enabled;
    CPU_t* cpu;
} i8042_t;

void i8042_init(CPU_t* cpu);

#endif
