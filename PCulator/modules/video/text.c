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
#include <stddef.h>
#ifdef _WIN32
#include <process.h>
#include <Windows.h>
#else
#include <pthread.h>
#endif
#include "text.h"
#include "../../config.h"
#include "../../timing.h"
#include "../../utility.h"
#include "../../ports.h"
#include "../../memory.h"
#include "sdlconsole.h"
#include "../../debuglog.h"

uint16_t text_cursorloc = 0;
uint8_t text_indexreg = 0, text_datareg[256], text_regs[16];
uint8_t* text_RAM = NULL;
#ifdef _WIN32
HANDLE text_handle;
#endif

int text_init() {
	int x, y;

	debug_log(DEBUG_INFO, "[TEXT] Initializing text mode CGA video device\r\n");

	text_RAM = (uint8_t*)malloc(16384);
	if (text_RAM == NULL) {
		debug_log(DEBUG_ERROR, "[TEXT] Failed to allocate video memory\r\n");
		return -1;
	}

#ifdef _WIN32
	text_handle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

	timing_addTimer(text_scanlineCallback, NULL, 62800, TIMING_ENABLED);
	ports_cbRegister(0x3D0, 16, (void*)text_readport, NULL, (void*)text_writeport, NULL, NULL);
	memory_mapCallbackRegister(0xB8000, 0x4000, (void*)text_readmemory, (void*)text_writememory, NULL);

	return 0;
}

void text_writeport(void* dummy, uint16_t port, uint8_t value) {
#ifdef DEBUG_CGA
	debug_log(DEBUG_DETAIL, "Write CGA port: %02X -> %03X (indexreg = %02X)\r\n", value, port, cga_indexreg);
#endif
	switch (port) {
	case 0x3D4:
		text_indexreg = value;
		break;
	case 0x3D5:
		text_datareg[text_indexreg] = value;
		break;
	case 0x3DA:
		break;
	default:
		text_regs[port - 0x3D0] = value;
	}
}

uint8_t text_readport(void* dummy, uint16_t port) {
#ifdef DEBUG_CGA
	debug_log(DEBUG_DETAIL, "Read CGA port: %03X (indexreg = %02X)\r\n", port, cga_indexreg);
#endif
	switch (port) {
	case 0x3D4:
		return text_indexreg;
	case 0x3D5:
		return text_datareg[text_indexreg];
	case 0x3DA:
		return text_regs[0xA]; //rand() & 0xF;
	}
	return text_regs[port - 0x3D0]; //0xFF;
}

void text_writememory(void* dummy, uint32_t addr, uint8_t value) {
#ifdef _WIN32
	COORD pos;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

	addr -= 0xB8000;
	if (addr >= 16384) return;

	text_RAM[addr] = value;
	if (addr >= 4000) return;
	if ((addr & 1) == 0) {
#ifdef _WIN32
		pos.Y = (addr / 160);
		pos.X = (addr % 160) >> 1;
		SetConsoleCursorPosition(text_handle, pos);
#endif
		printf("%c", value);
		fflush(stdout);
	}
	else {

	}
}

void text_scanlineCallback(void* dummy) {
	/*
		NOTE: We are only doing very approximate CGA timing. Breaking the horizontal scan into
		four parts and setting the display inactive bit on 3DAh on the last quarter of it. Being
		more precise shouldn't be necessary and will take much more host CPU time.

		TODO: Look into whether this is true? So far, things are working fine.
	*/
	static uint16_t scanline = 0, hpart = 0;

	text_regs[0xA] = 6; //light pen bits always high
	text_regs[0xA] |= (hpart == 3) ? 1 : 0;
	text_regs[0xA] |= (scanline >= 224) ? 8 : 0;

	hpart++;
	if (hpart == 4) {
		/*if (scanline < 200) {
			cga_update(0, (scanline<<1), 639, (scanline<<1)+1);
		}*/
		hpart = 0;
		scanline++;
	}
	if (scanline == 256) {
		scanline = 0;
	}
}

uint8_t text_readmemory(void* dummy, uint32_t addr) {
	addr -= 0xB8000;
	if (addr >= 16384) return 0xFF;

	return text_RAM[addr];
}
