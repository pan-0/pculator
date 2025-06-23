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

/*
	Generic RTC interface for XT systems, works with TIMER.COM version 1.2
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "config.h"
#include "ports.h"
#include "timing.h"
#include "chipset/i8259.h"
#include "debuglog.h"
#include "xtime.h"

uint8_t cmosrtc_index = 0;
uint8_t cmosrtc_nvram[128];
uint8_t cmosrtc_ext_index = 0;
uint8_t cmosrtc_ext_nvram[128];
I8259_t* cmosrtc_i8259 = NULL;
uint32_t cmosrtc_timer;
FILE* cmosfile = NULL;

uint8_t cmosrtc_bcd(uint8_t value) {
	return (value % 10) | (((value / 10) % 10) << 4);
}

double cmosrtc_rate_hz(uint8_t value) {
	uint8_t rate_bits = value & 0x0F;
	if (rate_bits < 1 || rate_bits > 15) return 0; // Invalid or reserved
	return (double)(32768u >> (rate_bits - 1));
	//return 32768.0 / pow(2.0, rate_bits - 1);
}

void cmosrtc_tick(void* dummy) {
	xtime tdata;

	xt_get(&tdata);

	if (cmosrtc_nvram[0xB] & 0x40) {
		cmosrtc_nvram[0xC] = 0xC0;
		i8259_doirq(cmosrtc_i8259, 0); //IRQ 8 through slave PIC
	}

	if (cmosrtc_nvram[0xB] & 0x80) {
		return;
	}

	cmosrtc_nvram[0] = cmosrtc_bcd(xt_sec(&tdata) % 60);
	cmosrtc_nvram[2] = cmosrtc_bcd(xt_min(&tdata));
	cmosrtc_nvram[4] = cmosrtc_bcd(xt_hour(&tdata));
	cmosrtc_nvram[6] = cmosrtc_bcd(xt_wday(&tdata));
	cmosrtc_nvram[7] = cmosrtc_bcd(xt_mday(&tdata));
	cmosrtc_nvram[8] = cmosrtc_bcd(xt_mon(&tdata) + 1);
	cmosrtc_nvram[9] = cmosrtc_bcd((xt_year(&tdata) + 1900) % 100);
	//cmosrtc_nvram[32] = cmosrtc_bcd((xt_year(&tdata) + 1900) / 100);
}

uint8_t cmosrtc_read(void* dummy, uint16_t addr) {
	xtime tdata;
	uint8_t ret = 0xFF;

	xt_get(&tdata);

	if (addr == 0x71) {
		ret = cmosrtc_nvram[cmosrtc_index & 0x7F];
		if (cmosrtc_index == 0x0C) {
			cmosrtc_nvram[0xC] = 0; //clear status register C on read
		}
		else if (cmosrtc_index == 0x0A) {
			ret = cmosrtc_nvram[0xA] & 0x7F;
			if (xt_msec(&tdata) < 10) {
				ret |= 0x80;
			}
		}
		else if (cmosrtc_index == 0x0D) {
			if (cmosrtc_nvram[0x0D] == 0) {
				cmosrtc_nvram[0x0D] = ret = 0x80;
			}
		}
		//else if (cmosrtc_index == 0x3D) {
			//ret = 0x02;
		//}
		else if (cmosrtc_index == 0x3F) {
			ret = cmosrtc_ext_nvram[cmosrtc_nvram[0x3D] & 0x7F];
		}
		//printf("[CMOS] Read CMOS register 0x%02X -> 0x%02X\n", cmosrtc_index, ret);
	}

	return ret;
}

void cmosrtc_write(void* dummy, uint16_t addr, uint8_t value) {
	switch (addr) {
	case 0x3F:
		cmosrtc_ext_nvram[cmosrtc_nvram[0x3D] & 0x7F] = value;
		break;
	case 0x70:
		cmosrtc_index = value & 0x7F;
		break;
	case 0x71:
		//printf("[CMOS] Write CMOS register 0x%02X <- 0x%02X\n", cmosrtc_index, value);
		cmosrtc_nvram[cmosrtc_index] = value;
		switch (cmosrtc_index) {
		case 0x0A:
			timing_updateIntervalFreq(cmosrtc_timer, cmosrtc_rate_hz(value));
			//printf("Updated CMOS periodic rate to %f Hz\n", cmosrtc_rate_hz(value));
			break;
		}
		if (cmosfile != NULL) {
			fseek(cmosfile, 0, SEEK_SET);
			fwrite(cmosrtc_nvram, 1, 128, cmosfile);
		}
		break;
	}
}

void cmosrtc_init(char* cmosfilename, I8259_t* i8259) {
	debug_log(DEBUG_INFO, "[CMOS] Initializing CMOS + real time clock\r\n");
	memset(cmosrtc_nvram, 0, sizeof(cmosrtc_nvram));
	ports_cbRegister(0x70, 2, (void*)cmosrtc_read, NULL, (void*)cmosrtc_write, NULL, NULL);
	cmosfile = fopen(cmosfilename, "r+b");
	if (cmosfile != NULL) {
		fread(cmosrtc_nvram, 1, 128, cmosfile);
	}
	else {
		cmosfile = fopen(cmosfilename, "wb");
		if (cmosfile != NULL) {
			fwrite(cmosrtc_nvram, 1, 128, cmosfile);
		}
		else {
			debug_log(DEBUG_INFO, "[CMOS] WARNING: Unable to open file cmos.bin for write. CMOS data will not be preserved!\n");
		}
	}
	cmosrtc_i8259 = i8259;
	cmosrtc_timer = timing_addTimer(cmosrtc_tick, NULL, 64, TIMING_ENABLED);
}
