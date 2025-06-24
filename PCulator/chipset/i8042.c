#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>  /* memcpy(), memmmove() */
#include "i8042.h"
#include "../config.h"
#include "../debuglog.h"
#include "../ports.h"
i8042_t kbc;

void i8042_buffer_key_data(const uint8_t* data, uint8_t len, uint8_t doirq) {
    kbc.keyboard_enabled = 1; //hack
    if (!kbc.keyboard_enabled || len == 0 || len > sizeof(kbc.data_buffer)) return;

    memcpy(kbc.data_buffer, data, len);
    kbc.buflen = len;
    if (doirq) i8259_doirq(kbc.i8259, 1);

    /*printf("Buffer keys: ");
    for (int i = 0; i < len; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");*/

#ifdef DEBUG_KBC
    debug_log(DEBUG_DETAIL, "[KBC] Buffered key data, length: %d\n", len);
#endif
}

// **Port 0x60 Read (Keyboard Data Register)**
uint8_t i8042_read_0x60() {
    static uint8_t ret = 0;

    if (kbc.buflen > 0) {
        ret = kbc.data_buffer[0];
        memmove(kbc.data_buffer, &kbc.data_buffer[1], sizeof(kbc.data_buffer) - 1);
        kbc.buflen--;
    }
    if (kbc.buflen > 0) i8259_doirq(kbc.i8259, 1);

#ifdef DEBUG_KBC
    debug_log(DEBUG_DETAIL, "[KBC] Read 0x60 = 0x%02X\n", ret);
#endif
    return ret;
}

// **Port 0x60 Write**
void i8042_write_0x60(uint8_t value) {
    kbc.status |= 2;
    //if (kbc.command == 0) {
    //    value = 0xFA;  // ACK first
    //    i8042_buffer_key_data(&value, 1, 0);
    //}
    switch (kbc.command) {
    case 0x00:
        switch (value) {
        case 0xFF: // Reset command
        {
                uint8_t ack = 0xFA;
                i8042_buffer_key_data(&ack, 1, 1); // Send ACK
                // Simulate reset response
                uint8_t selftest = 0xAA;
                i8042_buffer_key_data(&selftest, 1, 1);
                //kbc.cpu->a20_gate = 0; // Optional: Reset A20
                // Don't actually reset CPU here, NT expects it to continue
            }
            break;
        default:
            // Handle other keyboard commands as needed
            {
                uint8_t ack = 0xFA;
                i8042_buffer_key_data(&ack, 1, 1);
                //printf("[Keyboard] Unrecognized command: 0x%02X\n", value);
            }
            break;
        }
        break;

    case 0x60: //Write config byte
        kbc.config = value;
        /*printf("KBC config:\n"
            "     First port interrupt: %s\n"
            "    Second port interrupt: %s\n"
            "              System flag: %s\n"
            "           Should be zero: %u\n"
            "         First port clock: %s\n"
            "        Second port clock: %s\n"
            "           XT translation: %s\n"
            "             Must be zero: %u\n",
            (value & 0x01) ? "Yes" : "No",
            (value & 0x02) ? "Yes" : "No",
            (value & 0x04) ? "Yes" : "No",
            (value & 0x08) >> 3,
            (value & 0x10) ? "Yes" : "No",
            (value & 0x20) ? "Yes" : "No",
            (value & 0x40) ? "Yes" : "No",
            (value & 0x80) >> 7);*/
            break;

    case 0xD1: // Handle A20 gate
        kbc.cpu->a20_gate = (value & 0x02) ? 1 : 0;
#ifdef DEBUG_KBC
        debug_log(DEBUG_DETAIL, "[KBC] A20 gate %s\n", kbc.cpu->a20_gate ? "ENABLED" : "DISABLED");
#endif
        break;

    case 0xFF: //reset
        value = 0xFA;  // ACK first
        kbc.status &= ~4;
        i8042_buffer_key_data(&value, 1, 0);
        break;

    default:
        //printf("Unhandled KBC command: %02X\n", kbc.command);
        break;
    }
    kbc.command = 0;
#ifdef DEBUG_KBC
    debug_log(DEBUG_DETAIL, "[KBC] Write 0x60 = 0x%02X (to keyboard)\n", value);
#endif
}

// **Port 0x64 Read (Status Register)**
uint8_t i8042_read_0x64() {
    uint8_t status = kbc.status | 16;
    if (kbc.buflen > 0) status |= 1;
    kbc.status &= ~2;
#ifdef DEBUG_KBC
    debug_log(DEBUG_DETAIL, "[KBC] Read 0x64 (status) = 0x%02X\n", status);
#endif
    return status;
}

// **Port 0x64 Write (Command Register)**
void i8042_write_0x64(uint8_t value) {
    kbc.status |= 2;
    //printf("[KBC] Write 0x64 = 0x%02X (Command Register)\n", value);
#ifdef DEBUG_KBC
    debug_log(DEBUG_DETAIL, "[KBC] Write 0x64 = 0x%02X (Command Register)\n", value);
#endif
    switch (value) {
    case 0x20: // Read Controller configuration byte
        value = kbc.config;
        i8042_buffer_key_data(&value, 1, 0);
        break;

    case 0x60: // Write Controller Configuration Byte
        kbc.command = 0x60;
#ifdef DEBUG_KBC
        debug_log(DEBUG_DETAIL, "[KBC] Awaiting configuration byte (write to 0x60 next).\n");
#endif
        break;

    case 0xA1: // AMI get version
        value = 'N';
        i8042_buffer_key_data(&value, 1, 0);
        break;

    case 0xAA: // Controller self-test
        kbc.status |= 4; //system flag
        value = 0x55; // Self-test OK
        i8042_buffer_key_data(&value, 1, 0);
#ifdef DEBUG_KBC
        debug_log(DEBUG_DETAIL, "[KBC] Self-test completed (0x55 OK).\n");
#endif
        break;

    case 0xAB: // Keyboard interface test
        value = 0x00; // 0x00 = Test passed
        i8042_buffer_key_data(&value, 1, 0);
#ifdef DEBUG_KBC
        debug_log(DEBUG_DETAIL, "[KBC] Keyboard interface test OK.\n");
#endif
        break;

    case 0xAD: // Disable keyboard
        kbc.keyboard_enabled = false;
#ifdef DEBUG_KBC
        debug_log(DEBUG_DETAIL, "[KBC] Keyboard disabled.\n");
#endif
        break;

    case 0xAE: // Enable keyboard
        kbc.keyboard_enabled = true;
        value = 0xFA;  // ACK for enabling
        i8042_buffer_key_data(&value, 1, 0);
#ifdef DEBUG_KBC
        debug_log(DEBUG_DETAIL, "[KBC] Keyboard enabled.\n");
#endif
        break;

    case 0xC0: // Read in port
        value = 0x80 | /* Keyboard not inhibited */
            0x20; /* Manufacturing jumper for POST loop not installed */
        i8042_buffer_key_data(&value, 1, 0);
        break;

    case 0xD1: // Write Output Port (A20 Control)
        kbc.command = value;
#ifdef DEBUG_KBC
        debug_log(DEBUG_DETAIL, "[KBC] Awaiting A20 gate write (next write to 0x60).\n");
#endif
        break;

    default:
        kbc.command = 0;
        //value = 0xFA;  // ACK
        //i8042_buffer_key_data(&value, 1, 0);
        //printf("[KBC] Unknown command 0x%02X\n", value);
#ifdef DEBUG_KBC
        debug_log(DEBUG_DETAIL, "[KBC] Unknown command 0x%02X\n", value);
#endif
        break;
    }
}

// **Main I/O Handlers**
uint8_t i8042_readport(void* dummy, uint32_t portnum) {
    switch (portnum) {
    case 0x60: return i8042_read_0x60();
    case 0x64: return i8042_read_0x64();
    default:
#ifdef DEBUG_KBC
        debug_log(DEBUG_DETAIL, "[KBC] Invalid read from port 0x%X\n", portnum);
#endif
        return 0xFF;
    }
}

void i8042_writeport(void* dummy, uint32_t portnum, uint8_t value) {
    switch (portnum) {
    case 0x60:
        i8042_write_0x60(value);
        break;
    case 0x64:
        i8042_write_0x64(value);
        break;
    default:
#ifdef DEBUG_KBC
        debug_log(DEBUG_DETAIL, "[KBC] Invalid write to port 0x%X = 0x%02X\n", portnum, value);
#endif
        break;
    }
}

void i8042_init(CPU_t* cpu, I8259_t* i8259) {
    kbc.status = 0x00;       // Default status
    //kbc.data_buffer[0] = 0x00;  // Clear data buffer
    kbc.buflen = 0;
    kbc.cpu = cpu;
    kbc.cpu->a20_gate = 0; // A20 disabled at boot
    kbc.reset_requested = false;
    kbc.self_test_done = false;
    kbc.i8259 = i8259;
    ports_cbRegister(0x60, 1, (void*)i8042_readport, NULL, (void*)i8042_writeport, NULL, NULL);
    ports_cbRegister(0x64, 1, (void*)i8042_readport, NULL, (void*)i8042_writeport, NULL, NULL);
    debug_log(DEBUG_INFO, "[KBC] Initialized AT-style keyboard controller\n");
}
