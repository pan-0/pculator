/*
  PCulator: A portable, open-source x86 PC emulator.
  Copyright (C)2025 Mike Chambers

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> //for rand()
#include <string.h>
#include "config.h"
#include "debuglog.h"
#include "cpu/cpu.h"
#include "machine.h"
#include "chipset/i8259.h"
#include "chipset/i8253.h"
#include "chipset/i8237.h"
#include "chipset/i8255.h"
#include "ports.h"
#include "modules/video/sdlconsole.h"
#include "modules/video/cga.h"
#include "modules/video/vga.h"

//#define FAKE_PCI // !! HACK ALERT !!

/*uint8_t(*ports_cbReadB[PORTS_COUNT])(void* udata, uint32_t portnum);
uint16_t (*ports_cbReadW[PORTS_COUNT])(void* udata, uint32_t portnum);
uint32_t (*ports_cbReadL[PORTS_COUNT])(void* udata, uint32_t portnum);
void (*ports_cbWriteB[PORTS_COUNT])(void* udata, uint32_t portnum, uint8_t value);
void (*ports_cbWriteW[PORTS_COUNT])(void* udata, uint32_t portnum, uint16_t value);
void (*ports_cbWriteL[PORTS_COUNT])(void* udata, uint32_t portnum, uint32_t value);
void* ports_udata[PORTS_COUNT];*/

struct ports_s {
	uint32_t start;
	uint32_t size;
	uint8_t(*readcb)(void* udata, uint32_t addr);
	uint16_t(*readcbW)(void* udata, uint32_t addr);
	uint32_t(*readcbL)(void* udata, uint32_t addr);
	void (*writecb)(void* udata, uint32_t addr, uint8_t value);
	void (*writecbW)(void* udata, uint32_t addr, uint16_t value);
	void (*writecbL)(void* udata, uint32_t addr, uint32_t value);
	void* udata;
	int used;
} ports[64];

int lastportmap = -1;

extern MACHINE_t machine;

extern int showops;

FUNC_INLINE int getportmap(uint32_t addr32) {
	int i;
	for (i = lastportmap; i >= 0; i--) {
		if (ports[i].used) {
			if ((addr32 >= ports[i].start) && (addr32 < (ports[i].start + ports[i].size))) {
				return i;
			}
		}
	}
	return -1;
}

uint32_t pci_config_address;
uint8_t pci_config_space[256 * 32]; // 256 devices * 32-byte headers

uint8_t piix4_config[64] = {
	// 0x00: Vendor ID (Intel) + Device ID (PIIX4 IDE)
	0x86, 0x80,             // Vendor ID = 0x8086
	0x11, 0x71,             // Device ID = 0x7111 (PIIX4 IDE)

	// 0x04: Command/Status
	0x05, 0x00,             // Command = I/O space + Bus Master enabled (optional)
	0x00, 0x00,             // Status (not needed for Linux probe)

	// 0x08: Revision + Class Code
	0x01,                   // Revision
	0x80,                   // ProgIF = 0x80 (legacy mode IDE)
	0x01,                   // Subclass = 0x01 (IDE)
	0x01,                   // Base Class = 0x01 (Mass Storage)

	// 0x0C: Cache line size, latency, header type, BIST
	0x00, 0x00, 0x00, 0x00,

	// 0x10: BAR0 = Primary I/O: 0x1F0 (with LSB=1 for I/O)
	0xF1, 0x01, 0x00, 0x00,

	// 0x14: BAR1 = Primary Control: 0x3F6
	0xF5, 0x03, 0x00, 0x00,

	// 0x18: BAR2 = Secondary I/O: 0x170
	0x71, 0x01, 0x00, 0x00,

	// 0x1C: BAR3 = Secondary Control: 0x376
	0x75, 0x03, 0x00, 0x00,

	// 0x20<96>0x23: BAR4/5 (not used)
	0x00, 0x00, 0x00, 0x00,

	// 0x24<96>0x33: Reserved/unused
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// 0x34<96>0x3B: Reserved/unused
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// 0x3C: IRQ line & pin
	0x0E, 0x01,             // IRQ line = 14, pin = INTA#

	// 0x3E<96>0x3F: Min Gnt / Max Lat (optional)
	0x00, 0x00
};

// Handle writes to 0xCF8 (PCI config address)
void pci_write_0xcf8(uint32_t value) {
	printf("PCI write %08X\n", value);
	pci_config_address = value;
}

uint32_t pci_read_0xcf8() {
	printf("PCI CF8 read\n");
	return pci_config_address;
}

// Handle reads/writes to 0xCFC (PCI config data)
uint32_t pci_read_0xcfc() {
	uint8_t bus = (pci_config_address >> 16) & 0xFF;
	uint8_t device = (pci_config_address >> 11) & 0x1F;
	uint8_t func = (pci_config_address >> 8) & 0x07;
	uint8_t offset = pci_config_address & 0xFC;
	int idx = ((bus << 8) | (device << 3) | func);
	printf("PCI read CFC, cfg idx = %d\n", idx);
	return *(uint32_t*)&pci_config_space[idx * 256 + offset];
}

void port_write(CPU_t* cpu, uint16_t portnum, uint8_t value) {
	int map;
#ifdef DEBUG_PORTS
	debug_log(DEBUG_DETAIL, "port_write @ %03X <- %02X\r\n", portnum, value);
#endif
	//portnum &= 0x0FFF;

	map = getportmap(portnum);

	/*if (portnum == 0x80) {
		debug_log(DEBUG_DETAIL, "Diagnostic port out: %02X\r\n", value);
		//if (value == 0xA) showops = 1;
	}
	else*/ if (portnum == 0x92) {
		cpu->a20_gate = (value & 2) ? 1 : 0;
		return;
	}
	/*if (ports_cbWriteB[portnum] != NULL) {
		(*ports_cbWriteB[portnum])(ports_udata[portnum], portnum, value);
		return;
	}*/

	if (map != -1) {
		if (ports[map].writecb != NULL) {
			(*ports[map].writecb)(ports[map].udata, portnum, value);
			return;
		}
	}

}

void port_writew(CPU_t* cpu, uint16_t portnum, uint16_t value) {
	int map;
	//portnum &= 0x0FFF;
	map = getportmap(portnum);

	if (portnum == 0x80) {
		debug_log(DEBUG_DETAIL, "Diagnostic port out: %04X\r\n", value);
	}
	/*if (ports_cbWriteW[portnum] != NULL) {
		(*ports_cbWriteW[portnum])(ports_udata[portnum], portnum, value);
		return;
	}*/
	if (map != -1) {
		if (ports[map].writecbW != NULL) {
			(*ports[map].writecbW)(ports[map].udata, portnum, value);
			return;
		}
	}

	port_write(cpu, portnum, (uint8_t)value);
	port_write(cpu, portnum + 1, (uint8_t)(value >> 8));
}

void port_writel(CPU_t* cpu, uint16_t portnum, uint32_t value) {
	int map;
	//portnum &= 0x0FFF;
	map = getportmap(portnum);

	if (portnum == 0x80) {
		debug_log(DEBUG_DETAIL, "Diagnostic port out: %08X\r\n", value);
	}
	/*if (ports_cbWriteL[portnum] != NULL) {
		(*ports_cbWriteL[portnum])(ports_udata[portnum], portnum, value);
		return;
	}*/

	if (map != -1) {
		if (ports[map].writecbL != NULL) {
			(*ports[map].writecbL)(ports[map].udata, portnum, value);
			return;
		}
	}


#ifdef FAKE_PCI
	if (portnum == 0xCF8) {
		pci_write_0xcf8(value);
		return;
	}
#endif

	port_write(cpu, portnum, (uint8_t)value);
	port_write(cpu, portnum + 1, (uint8_t)(value >> 8));
	port_write(cpu, portnum + 2, (uint8_t)(value >> 16));
	port_write(cpu, portnum + 3, (uint8_t)(value >> 24));
}

uint8_t port_read(CPU_t* cpu, uint16_t portnum) {
	int map;
#ifdef DEBUG_PORTS
	if ((portnum != 0x3BA) && (portnum != 0x3DA)) debug_log(DEBUG_DETAIL, "port_read @ %03X\r\n", portnum);
#endif
	if (showops) {
		if ((portnum != 0x3BA) && (portnum != 0x3DA)) debug_log(DEBUG_DETAIL, "port_read @ %03X\r\n", portnum);
	}
	//portnum &= 0x0FFF;
	map = getportmap(portnum);

#ifdef FAKE_PCI
	if (portnum == 0x92) {
		return cpu->a20_gate ? 2 : 0;
	}
	if (portnum == 0xCFC) {
		return pci_read_0xcfc() & 0xFF;
	}
	if (portnum == 0xCFD) {
		return pci_read_0xcfc() >> 8;
	}
	if (portnum == 0xCFE) {
		return pci_read_0xcfc() >> 16;
	}
	if (portnum == 0xCFF) {
		return pci_read_0xcfc() >> 24;
	}
#endif

	/*if (ports_cbReadB[portnum] != NULL) {
		return (*ports_cbReadB[portnum])(ports_udata[portnum], portnum);
	}*/
	if (map != -1) {
		if (ports[map].readcb != NULL) {
			return (*ports[map].readcb)(ports[map].udata, portnum);
		}
	}

	return 0xFF;
}

uint16_t port_readw(CPU_t* cpu, uint16_t portnum) {
	int map;
	uint16_t ret;
	//portnum &= 0x0FFF;
	map = getportmap(portnum);

	/*if (ports_cbReadW[portnum] != NULL) {
		return (*ports_cbReadW[portnum])(ports_udata[portnum], portnum);
	}*/
	if (map != -1) {
		if (ports[map].readcbW != NULL) {
			return (*ports[map].readcbW)(ports[map].udata, portnum);
		}
	}

#ifdef FAKE_PCI
	if (portnum == 0xCFC) {
		return pci_read_0xcfc() & 0xFFFF;
	}
	if (portnum == 0xCFE) {
		return pci_read_0xcfc() >> 16;
	}
#endif

	ret = port_read(cpu, portnum);
	ret |= (uint16_t)port_read(cpu, portnum + 1) << 8;
	return ret;
}

uint32_t port_readl(CPU_t* cpu, uint16_t portnum) {
	int map;
	uint32_t ret;
	//portnum &= 0x0FFF;
	map = getportmap(portnum);

	/*if (ports_cbReadL[portnum] != NULL) {
		return (*ports_cbReadL[portnum])(ports_udata[portnum], portnum);
	}*/
	if (map != -1) {
		if (ports[map].readcbL != NULL) {
			return (*ports[map].readcbL)(ports[map].udata, portnum);
		}
	}


#ifdef FAKE_PCI
	if (portnum == 0xCF8) {
		return pci_read_0xcf8();
	}
	else if (portnum == 0xCFC) {
		return pci_read_0xcfc();
	}
#endif

	ret = port_read(cpu, portnum);
	ret |= (uint32_t)port_read(cpu, portnum + 1) << 8;
	ret |= (uint32_t)port_read(cpu, portnum + 2) << 16;
	ret |= (uint32_t)port_read(cpu, portnum + 3) << 24;
	return ret;
}

void ports_cbRegister(uint32_t start, uint32_t count, uint8_t (*readb)(void*, uint32_t), uint16_t (*readw)(void*, uint32_t), void (*writeb)(void*, uint32_t, uint8_t), void (*writew)(void*, uint32_t, uint16_t), void* udata) {
/*	uint32_t i;
	for (i = 0; i < count; i++) {
		if ((start + i) >= PORTS_COUNT) {
			break;
		}
		ports_cbReadB[start + i] = readb;
		ports_cbReadW[start + i] = readw;
		ports_cbWriteB[start + i] = writeb;
		ports_cbWriteW[start + i] = writew;
		ports_udata[start + i] = udata;
	}*/

	uint8_t i;
	uint32_t j;
	for (i = 0; i < 64; i++) {
		if (ports[i].used == 0) break;
	}
	if (i == 64) {
		debug_log(DEBUG_ERROR, "[PORTS] Out of port map structs!\n");
		exit(0);
	}
	ports[i].readcb = readb;
	ports[i].writecb = writeb;
	ports[i].readcbW = readw;
	ports[i].writecbW = writew;
	ports[i].readcbL = NULL;
	ports[i].writecbL = NULL;
	ports[i].start = start;
	ports[i].size = count;
	ports[i].udata = udata;
	ports[i].used = 1;
	lastportmap = i;

	/*for (int j = 0; j < 64; j++) {
		if (!ports[j].used) continue;

		uint16_t start_a = ports[j].start;
		uint16_t end_a = start_a + ports[j].size - 1;
		uint16_t start_b = start;
		uint16_t end_b = start + count - 1;

		if ((start_b <= end_a) && (end_b >= start_a)) {
			printf("[PORTS WARNING] Port range %04X-%04X overlaps with existing %04X-%04X\n",
				start_b, end_b, start_a, end_a);
		}
	}*/

}

void ports_init() {
	uint32_t i;
	/*for (i = 0; i < PORTS_COUNT; i++) {
		ports_cbReadB[i] = NULL;
		ports_cbReadW[i] = NULL;
		ports_cbReadL[i] = NULL;
		ports_cbWriteB[i] = NULL;
		ports_cbWriteW[i] = NULL;
		ports_cbWriteL[i] = NULL;
		ports_udata[i] = NULL;
	}*/
	for (i = 0; i < 64; i++) {
		ports[i].readcb = NULL;
		ports[i].writecb = NULL;
		ports[i].readcbW = NULL;
		ports[i].writecbW = NULL;
		ports[i].readcbL = NULL;
		ports[i].writecbL = NULL;
		ports[i].used = 0;
	}

	//memcpy(pci_config_space, piix4_config, 64);

/*	pci_config_space[0] = 0x86;
	pci_config_space[1] = 0x80;
	pci_config_space[2] = 0x11;
	pci_config_space[3] = 0x71;
	pci_config_space[4] = 0x05;
	pci_config_space[8] = 0x81;
	pci_config_space[9] = 0x80;
	pci_config_space[0xA] = 0x01;
	pci_config_space[0xB] = 0x01;

	// BAR0 = 0x000001F1 (I/O)
	pci_config_space[0x10] = 0xF1;
	pci_config_space[0x11] = 0x01;
	pci_config_space[0x12] = 0x00;
	pci_config_space[0x13] = 0x00;

	// BAR1 = 0x000003F5
	pci_config_space[0x14] = 0xF5;
	pci_config_space[0x15] = 0x03;
	pci_config_space[0x16] = 0x00;
	pci_config_space[0x17] = 0x00;

	// BAR2 = 0x00000171
	pci_config_space[0x18] = 0x71;
	pci_config_space[0x19] = 0x01;
	pci_config_space[0x1A] = 0x00;
	pci_config_space[0x1B] = 0x00;

	// BAR3 = 0x00000375
	pci_config_space[0x1C] = 0x75;
	pci_config_space[0x1D] = 0x03;
	pci_config_space[0x1E] = 0x00;
	pci_config_space[0x1F] = 0x00;

	pci_config_space[0x3C] = 0x0E; // IRQ line = 14
	pci_config_space[0x3D] = 0x01; // IRQ pin = INTA#
	*/
}
