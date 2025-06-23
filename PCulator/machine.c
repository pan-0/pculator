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
	Machine definitions.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "config.h"
#include "debuglog.h"
#include "cpu/cpu.h"
#include "chipset/i8042.h"
#include "chipset/i8259.h"
#include "chipset/i8253.h"
#include "chipset/i8237.h"
#include "chipset/i8255.h"
#include "chipset/uart.h"
#include "chipset/rabbit.h"
#include "chipset/opti495.h"
#include "chipset/opti5x7.h"
#include "modules/audio/pcspeaker.h"
#include "modules/audio/opl2.h"
#include "modules/audio/blaster.h"
#include "modules/disk/biosdisk.h"
#include "modules/disk/fdc.h"
#include "modules/disk/ata.h"
#include "modules/disk/pcem-ide.h"
#include "modules/input/mouse.h"
#include "modules/input/input.h"
#ifdef USE_NE2000
#include "modules/io/ne2000.h"
#include "modules/io/pcap-win32.h"
#endif
#include "modules/io/tcpmodem.h"
#include "modules/video/cga.h"
#include "modules/video/vga.h"
#include "modules/video/text.h"
#include "rtc.h"
#include "cmosrtc.h"
#include "memory.h"
#include "utility.h"
#include "timing.h"
#include "modules/video/sdlconsole.h"
#include "machine.h"

/*
	ID string, full description, init function, default video, speed in MHz (-1 = unlimited), default hardware flags
*/
const MACHINEDEF_t machine_defs[] = {
	{ "ami386", "AMI 386", machine_init_asus_386, "cmos/ami386.bin", VIDEO_CARD_VGA, -1, MACHINE_HW_BLASTER | MACHINE_HW_UART1_MOUSE | MACHINE_HW_DISK_HLE | MACHINE_HW_RTC},
	{ "award495", "OPTi 495 Award 486 clone", machine_init_opti495, "cmos/award495.bin", VIDEO_CARD_VGA, -1, MACHINE_HW_BLASTER | MACHINE_HW_UART1_MOUSE | MACHINE_HW_DISK_HLE | MACHINE_HW_RTC},
	{ "seabios", "SeaBIOS (from QEMU)", machine_init_opti495, "cmos/seabios.bin", VIDEO_CARD_VGA, -1, MACHINE_HW_BLASTER | MACHINE_HW_UART1_MOUSE | MACHINE_HW_DISK_HLE | MACHINE_HW_RTC},
	{ "award486", "Award 4.50G 486 clone", machine_init_asus_386, "cmos/award486.bin", VIDEO_CARD_VGA, -1, MACHINE_HW_BLASTER | MACHINE_HW_UART1_MOUSE | MACHINE_HW_DISK_HLE | MACHINE_HW_RTC},
	{ "mrbios486", "OPTi 495 MR-BIOS 486", machine_init_opti495, "cmos/mrbios486.bin", VIDEO_CARD_VGA, -1, MACHINE_HW_BLASTER | MACHINE_HW_UART1_MOUSE | MACHINE_HW_DISK_HLE | MACHINE_HW_RTC},
	{ "4saw2", "Soyo 4SAW2", machine_init_asus_386, "cmos/4saw2.bin", VIDEO_CARD_VGA, -1, MACHINE_HW_BLASTER | MACHINE_HW_UART1_MOUSE | MACHINE_HW_DISK_HLE | MACHINE_HW_RTC},
	{ "hot543", "Shuttle HOT-543", machine_init_opti5x7, "cmos/hot543.bin", VIDEO_CARD_VGA, -1, MACHINE_HW_BLASTER | MACHINE_HW_UART1_MOUSE | MACHINE_HW_DISK_HLE | MACHINE_HW_RTC},
	{ "sp97xv", "Asus SP97-XV", machine_init_opti5x7, "cmos/sp97xv.bin", VIDEO_CARD_VGA, -1, MACHINE_HW_BLASTER | MACHINE_HW_UART1_MOUSE | MACHINE_HW_DISK_HLE | MACHINE_HW_RTC},
	{ "p5sp4", "ASUS PCI/I-P5SP4", machine_init_opti495, "cmos/p5sp4.bin", VIDEO_CARD_VGA, -1, MACHINE_HW_BLASTER | MACHINE_HW_UART1_MOUSE | MACHINE_HW_DISK_HLE | MACHINE_HW_RTC},
	{ NULL }
};

const MACHINEMEM_t machine_mem[][10] = {
	//AMI 386
	{
		{ MACHINE_MEM_RAM, 0x00000, 0xA0000, MACHINE_ROM_ISNOTROM, NULL },
		{ MACHINE_MEM_ROM, 0xF0000, 0x10000, MACHINE_ROM_REQUIRED, "roms/machine/ami386/ami386.bin" },
		{ MACHINE_MEM_RAM, 0x100000, 0xF00000, MACHINE_ROM_ISNOTROM, NULL }, // 16 MB RAM
		{ MACHINE_MEM_ENDLIST, 0, 0, 0, NULL }
	},

	//OPTi 495 Award 486 clone
	{
		{ MACHINE_MEM_RAM, 0x00000, 0xA0000, MACHINE_ROM_ISNOTROM, NULL },
		//{ MACHINE_MEM_ROM, 0xD0000, 0x02000, MACHINE_ROM_REQUIRED, "roms/disk/ide_386.bin" },
		//{ MACHINE_MEM_ROM, 0xE0000, 0x10000, MACHINE_ROM_REQUIRED, "C:\\plpbt-5.0.13\\plpbt64k.bin" },
		{ MACHINE_MEM_ROM, 0xF0000, 0x10000, MACHINE_ROM_REQUIRED, "roms/machine/award495/opt495s.awa" },
		{ MACHINE_MEM_RAM, 0x100000, 0x3F00000, MACHINE_ROM_ISNOTROM, NULL }, // 64 MB RAM
		//{ MACHINE_MEM_RAM, 0x100000, 0xFF00000, MACHINE_ROM_ISNOTROM, NULL }, // 256 MB RAM
		{ MACHINE_MEM_ENDLIST, 0, 0, 0, NULL }
	},

	//SeaBIOS
	{
		{ MACHINE_MEM_RAM, 0x00000, 0xA0000, MACHINE_ROM_ISNOTROM, NULL },
		//{ MACHINE_MEM_ROM, 0xD0000, 0x02000, MACHINE_ROM_REQUIRED, "roms/disk/ide_386.bin" },
		{ MACHINE_MEM_ROM, 0xE0000, 0x20000, MACHINE_ROM_REQUIRED, "roms/machine/seabios/bios.bin" },
		{ MACHINE_MEM_RAM, 0x100000, 0x3F00000, MACHINE_ROM_ISNOTROM, NULL }, // 64 MB RAM
		//{ MACHINE_MEM_RAM, 0x100000, 0xFF00000, MACHINE_ROM_ISNOTROM, NULL }, // 256 MB RAM
		{ MACHINE_MEM_ENDLIST, 0, 0, 0, NULL }
	},

	//Award 486
	{
		{ MACHINE_MEM_RAM, 0x00000, 0xA0000, MACHINE_ROM_ISNOTROM, NULL },
		//{ MACHINE_MEM_ROM, 0xD0000, 0x02000, MACHINE_ROM_REQUIRED, "roms/disk/ide_386.bin" },
		//{ MACHINE_MEM_ROM, 0xD0000, 0x10000, MACHINE_ROM_REQUIRED, "C:\\plpbt-5.0.13\\plpbt64k.bin" },
		{ MACHINE_MEM_ROM, 0xF0000, 0x10000, MACHINE_ROM_REQUIRED, "roms/machine/award486/ah4-02-5eed236c7c5ab670872178.bin" },
		//{ MACHINE_MEM_RAM, 0x100000, 0xF00000, MACHINE_ROM_ISNOTROM, NULL }, // 16 MB RAM
		//{ MACHINE_MEM_RAM, 0x100000, 0x3F00000, MACHINE_ROM_ISNOTROM, NULL }, // 64 MB RAM
		{ MACHINE_MEM_RAM, 0x100000, 0x7F00000, MACHINE_ROM_ISNOTROM, NULL }, // 128 MB RAM
		//{ MACHINE_MEM_RAM, 0x100000, 0xFF00000, MACHINE_ROM_ISNOTROM, NULL }, // 256 MB RAM
		{ MACHINE_MEM_ENDLIST, 0, 0, 0, NULL }
	},

	//OPTi 495 MR-BIOS 486
	{
		{ MACHINE_MEM_RAM, 0x00000, 0xA0000, MACHINE_ROM_ISNOTROM, NULL },
		{ MACHINE_MEM_ROM, 0xF0000, 0x10000, MACHINE_ROM_REQUIRED, "roms/machine/mrbios486/opt495sx.mr" },
		{ MACHINE_MEM_RAM, 0x100000, 0xF00000, MACHINE_ROM_ISNOTROM, NULL }, // 16 MB RAM
		{ MACHINE_MEM_ENDLIST, 0, 0, 0, NULL }
	},
			
	//Soyo 4SAW2
	{
		{ MACHINE_MEM_RAM, 0x00000, 0xA0000, MACHINE_ROM_ISNOTROM, NULL },
		{ MACHINE_MEM_ROM, 0xE0000, 0x20000, MACHINE_ROM_REQUIRED, "roms/machine/4saw2/4saw0911.bin" },
		{ MACHINE_MEM_RAM, 0x100000, 0xF00000, MACHINE_ROM_ISNOTROM, NULL }, // 16 MB RAM
		{ MACHINE_MEM_ENDLIST, 0, 0, 0, NULL }
	},

	//Shuttle HOT-543
	{
		{ MACHINE_MEM_RAM, 0x00000, 0xA0000, MACHINE_ROM_ISNOTROM, NULL },
		{ MACHINE_MEM_ROM, 0xE0000, 0x20000, MACHINE_ROM_REQUIRED, "roms/machine/hot543/543_R21.BIN" },
		{ MACHINE_MEM_RAM, 0x100000, 0xF00000, MACHINE_ROM_ISNOTROM, NULL }, // 16 MB RAM
		{ MACHINE_MEM_ENDLIST, 0, 0, 0, NULL }
	},

	//Asus SP97-XV
	{
		{ MACHINE_MEM_RAM, 0x00000, 0xA0000, MACHINE_ROM_ISNOTROM, NULL },
		{ MACHINE_MEM_ROM, 0xE0000, 0x20000, MACHINE_ROM_REQUIRED, "roms/machine/sp97xv/0109XVJ2.BIN" },
		{ MACHINE_MEM_RAM, 0x100000, 0xF00000, MACHINE_ROM_ISNOTROM, NULL }, // 16 MB RAM
		{ MACHINE_MEM_ENDLIST, 0, 0, 0, NULL }
	},

	//ASUS PCI/I-P5SP4
	{
		{ MACHINE_MEM_RAM, 0x00000, 0xA0000, MACHINE_ROM_ISNOTROM, NULL },
		{ MACHINE_MEM_ROM, 0xE0000, 0x20000, MACHINE_ROM_REQUIRED, "roms/machine/p5sp4/0106.001" },
		//{ MACHINE_MEM_RAM, 0x100000, 0xF00000, MACHINE_ROM_ISNOTROM, NULL }, // 16 MB RAM
		//{ MACHINE_MEM_RAM, 0x100000, 0x3F00000, MACHINE_ROM_ISNOTROM, NULL }, // 64 MB RAM
		{ MACHINE_MEM_RAM, 0x100000, 0xFF00000, MACHINE_ROM_ISNOTROM, NULL }, // 256 MB RAM
		{ MACHINE_MEM_ENDLIST, 0, 0, 0, NULL }
	},
};

uint8_t mac[6] = { 0xac, 0xde, 0x48, 0x88, 0xbb, 0xab };

extern i8042_t kbc;

int machine_init_generic_xt(MACHINE_t* machine) {
	if (machine == NULL) return -1;

	i8259_init(&machine->i8259, 0x20, NULL);
	i8253_init(&machine->i8253, &machine->i8259, &machine->pcspeaker);
	i8237_init(&machine->i8237, &machine->CPU);
	i8255_init(&machine->i8255, &machine->KeyState, &machine->pcspeaker);
	pcspeaker_init(&machine->pcspeaker);
	i8042_init(&machine->CPU, &machine->i8259);
	kbc.config = 0x40;
	kbc.keyboard_enabled = 1;
	sdlconsole_changeScancodes(2);
	machine->CPU.a20_gate = 1;

	//check machine HW flags and init devices accordingly
	if ((machine->hwflags & MACHINE_HW_BLASTER) && !(machine->hwflags & MACHINE_HW_SKIP_BLASTER)) {
		blaster_init(&machine->blaster, &machine->i8237, &machine->i8259, 0x220, 1, 5);
		OPL3_init(&machine->OPL3);
		machine->mixBlaster = 1;
		machine->mixOPL = 1;
	}
	else if ((machine->hwflags & MACHINE_HW_OPL) && !(machine->hwflags & MACHINE_HW_SKIP_OPL)) { //else if because some games won't detect an SB without seeing the OPL, so if SB enabled then OPL already is
		//opl2_init(&machine->OPL2);
		OPL3_init(&machine->OPL3);
		machine->mixOPL = 1;
	}
	if ((machine->hwflags & MACHINE_HW_RTC) && !(machine->hwflags & MACHINE_HW_SKIP_RTC)) {
		rtc_init(&machine->CPU);
	}

	if ((machine->hwflags & MACHINE_HW_UART0_NONE) && !(machine->hwflags & MACHINE_HW_SKIP_UART0)) {
		uart_init(&machine->UART[0], &machine->i8259, 0x3F8, 4, NULL, NULL, NULL, NULL);
	}
	else if ((machine->hwflags & MACHINE_HW_UART0_MOUSE) && !(machine->hwflags & MACHINE_HW_SKIP_UART0)) {
		uart_init(&machine->UART[0], &machine->i8259, 0x3F8, 4, NULL, NULL, (void*)mouse_togglereset, NULL);
		mouse_init(&machine->UART[0]);
		timing_addTimer(mouse_rxpoll, NULL, baudrate / 9, TIMING_ENABLED);
	}
#ifdef ENABLE_TCP_MODEM
	else if ((machine->hwflags & MACHINE_HW_UART0_TCPMODEM) && !(machine->hwflags & MACHINE_HW_SKIP_UART0)) {
		uart_init(&machine->UART[0], &machine->i8259, 0x3F8, 4, (void*)tcpmodem_tx, &machine->tcpmodem[0], NULL, NULL);
		tcpmodem_init(&machine->tcpmodem[0], &machine->UART[0], 23);
		timing_addTimer(tcpmodem_rxpoll, &machine->tcpmodem[0], baudrate / 9, TIMING_ENABLED);
	}
#endif

	if ((machine->hwflags & MACHINE_HW_UART1_NONE) && !(machine->hwflags & MACHINE_HW_SKIP_UART1)) {
		uart_init(&machine->UART[1], &machine->i8259, 0x2F8, 3, NULL, NULL, NULL, NULL);
	}
	else if ((machine->hwflags & MACHINE_HW_UART1_MOUSE) && !(machine->hwflags & MACHINE_HW_SKIP_UART1)) {
		uart_init(&machine->UART[1], &machine->i8259, 0x2F8, 3, NULL, NULL, (void*)mouse_togglereset, NULL);
		mouse_init(&machine->UART[1]);
		timing_addTimer(mouse_rxpoll, NULL, baudrate / 9, TIMING_ENABLED);
	}
#ifdef ENABLE_TCP_MODEM
	else if ((machine->hwflags & MACHINE_HW_UART1_TCPMODEM) && !(machine->hwflags & MACHINE_HW_SKIP_UART1)) {
		uart_init(&machine->UART[1], &machine->i8259, 0x2F8, 3, (void*)tcpmodem_tx, &machine->tcpmodem[1], NULL, NULL);
		tcpmodem_init(&machine->tcpmodem[1], &machine->UART[1], 23);
		timing_addTimer(tcpmodem_rxpoll, &machine->tcpmodem[1], baudrate / 9, TIMING_ENABLED);
	}
#endif

#ifdef USE_NE2000
	if (machine->hwflags & MACHINE_HW_NE2000) {
		ne2000_init(&machine->ne2000, &machine->i8259, 0x300, 2, (uint8_t*)&mac);
		if (machine->pcap_if > -1) {
			if (pcap_xinit(&machine->ne2000, machine->pcap_if)) {
				return -1;
			}
		}
	}
#endif

	cpu_reset(&machine->CPU);
#ifndef USE_DISK_HLE
	fdc_init(&fdc, &machine->CPU, &i8259, &i8237);
	fdc_insert(&fdc, 0, "dos622.img");
#else
	if (machine->int13emu) {
		biosdisk_init(&machine->CPU);
	}
	else {
		ata_init(&machine->i8259_slave);
	}
#endif

	switch (videocard) {
	case VIDEO_CARD_CGA:
		if (cga_init()) return -1;
		break;
	case VIDEO_CARD_TEXT:
		if (text_init()) return -1;
		break;
	case VIDEO_CARD_VGA:
		if (vga_init()) return -1;
		break;
	}

	return 0;
}

int machine_init_asus_386(MACHINE_t* machine) {
	if (machine == NULL) return -1;

	showops = 0;
	sdlconsole_changeScancodes(2); //use AT-style scancodes set 2

	i8259_init(&machine->i8259, 0x20, NULL);
	i8259_init(&machine->i8259_slave, 0xA0, &machine->i8259);
	i8253_init(&machine->i8253, &machine->i8259, &machine->pcspeaker);
	i8237_init(&machine->i8237, &machine->CPU);
	i8255_init(&machine->i8255, &machine->KeyState, &machine->pcspeaker);
	pcspeaker_init(&machine->pcspeaker);
	i8042_init(&machine->CPU, &machine->i8259);
	rabbit_init();

	//check machine HW flags and init devices accordingly
	if ((machine->hwflags & MACHINE_HW_BLASTER) && !(machine->hwflags & MACHINE_HW_SKIP_BLASTER)) {
		blaster_init(&machine->blaster, &machine->i8237, &machine->i8259, 0x220, 1, 5);
		OPL3_init(&machine->OPL3);
		machine->mixBlaster = 1;
		machine->mixOPL = 1;
	}
	else if ((machine->hwflags & MACHINE_HW_OPL) && !(machine->hwflags & MACHINE_HW_SKIP_OPL)) { //else if because some games won't detect an SB without seeing the OPL, so if SB enabled then OPL already is
		//opl2_init(&machine->OPL2);
		OPL3_init(&machine->OPL3);
		machine->mixOPL = 1;
	}
	//if ((machine->hwflags & MACHINE_HW_RTC) && !(machine->hwflags & MACHINE_HW_SKIP_RTC)) {
	//	cmosrtc_init(machine->);
	//}

	if ((machine->hwflags & MACHINE_HW_UART0_NONE) && !(machine->hwflags & MACHINE_HW_SKIP_UART0)) {
		uart_init(&machine->UART[0], &machine->i8259, 0x3F8, 4, NULL, NULL, NULL, NULL);
	}
	else if ((machine->hwflags & MACHINE_HW_UART0_MOUSE) && !(machine->hwflags & MACHINE_HW_SKIP_UART0)) {
		uart_init(&machine->UART[0], &machine->i8259, 0x3F8, 4, NULL, NULL, (void*)mouse_togglereset, NULL);
		mouse_init(&machine->UART[0]);
		timing_addTimer(mouse_rxpoll, NULL, baudrate / 9, TIMING_ENABLED);
	}
#ifdef ENABLE_TCP_MODEM
	else if ((machine->hwflags & MACHINE_HW_UART0_TCPMODEM) && !(machine->hwflags & MACHINE_HW_SKIP_UART0)) {
		uart_init(&machine->UART[0], &machine->i8259, 0x3F8, 4, (void*)tcpmodem_tx, &machine->tcpmodem[0], NULL, NULL);
		tcpmodem_init(&machine->tcpmodem[0], &machine->UART[0], 23);
		timing_addTimer(tcpmodem_rxpoll, &machine->tcpmodem[0], baudrate / 9, TIMING_ENABLED);
	}
#endif

	if ((machine->hwflags & MACHINE_HW_UART1_NONE) && !(machine->hwflags & MACHINE_HW_SKIP_UART1)) {
		uart_init(&machine->UART[1], &machine->i8259, 0x2F8, 3, NULL, NULL, NULL, NULL);
	}
	else if ((machine->hwflags & MACHINE_HW_UART1_MOUSE) && !(machine->hwflags & MACHINE_HW_SKIP_UART1)) {
		uart_init(&machine->UART[1], &machine->i8259, 0x2F8, 3, NULL, NULL, (void*)mouse_togglereset, NULL);
		mouse_init(&machine->UART[1]);
		timing_addTimer(mouse_rxpoll, NULL, baudrate / 9, TIMING_ENABLED);
	}
#ifdef ENABLE_TCP_MODEM
	else if ((machine->hwflags & MACHINE_HW_UART1_TCPMODEM) && !(machine->hwflags & MACHINE_HW_SKIP_UART1)) {
		uart_init(&machine->UART[1], &machine->i8259, 0x2F8, 3, (void*)tcpmodem_tx, &machine->tcpmodem[1], NULL, NULL);
		tcpmodem_init(&machine->tcpmodem[1], &machine->UART[1], 23);
		timing_addTimer(tcpmodem_rxpoll, &machine->tcpmodem[1], baudrate / 9, TIMING_ENABLED);
	}
#endif

#ifdef USE_NE2000
	if (machine->hwflags & MACHINE_HW_NE2000) {
		ne2000_init(&machine->ne2000, &machine->i8259, 0x300, 2, (uint8_t*)&mac);
		if (machine->pcap_if > -1) {
			if (pcap_xinit(&machine->ne2000, machine->pcap_if)) {
				return -1;
			}
		}
	}
#endif

	cpu_reset(&machine->CPU);
#ifndef USE_DISK_HLE
	fdc_init(&fdc, &machine->CPU, &i8259, &i8237);
	fdc_insert(&fdc, 0, "dos622.img");
#else
	if (machine->int13emu) {
		biosdisk_init(&machine->CPU);
	}
	else {
#ifndef USE_PCEM_IDE
		ata_init(&machine->i8259_slave);
#endif
	}
#endif

	switch (videocard) {
	case VIDEO_CARD_CGA:
		if (cga_init()) return -1;
		break;
	case VIDEO_CARD_TEXT:
		if (text_init()) return -1;
		break;
	case VIDEO_CARD_VGA:
		if (vga_init()) return -1;
		break;
	}

	return 0;
}

int machine_init_opti495(MACHINE_t* machine) {
	if (machine == NULL) return -1;

	showops = 0;
	sdlconsole_changeScancodes(2); //use AT-style scancodes set 2

	i8259_init(&machine->i8259, 0x20, NULL);
	i8259_init(&machine->i8259_slave, 0xA0, &machine->i8259);
	i8253_init(&machine->i8253, &machine->i8259, &machine->pcspeaker);
	i8237_init(&machine->i8237, &machine->CPU);
	i8255_init(&machine->i8255, &machine->KeyState, &machine->pcspeaker);
	pcspeaker_init(&machine->pcspeaker);
	i8042_init(&machine->CPU, &machine->i8259);
	opti495_init();

	//check machine HW flags and init devices accordingly
	if ((machine->hwflags & MACHINE_HW_BLASTER) && !(machine->hwflags & MACHINE_HW_SKIP_BLASTER)) {
		blaster_init(&machine->blaster, &machine->i8237, &machine->i8259, 0x220, 1, 5);
		OPL3_init(&machine->OPL3);
		machine->mixBlaster = 1;
		machine->mixOPL = 1;
	}
	else if ((machine->hwflags & MACHINE_HW_OPL) && !(machine->hwflags & MACHINE_HW_SKIP_OPL)) { //else if because some games won't detect an SB without seeing the OPL, so if SB enabled then OPL already is
		//opl2_init(&machine->OPL2);
		OPL3_init(&machine->OPL3);
		machine->mixOPL = 1;
	}
	//if ((machine->hwflags & MACHINE_HW_RTC) && !(machine->hwflags & MACHINE_HW_SKIP_RTC)) {
	//}

	if ((machine->hwflags & MACHINE_HW_UART0_NONE) && !(machine->hwflags & MACHINE_HW_SKIP_UART0)) {
		uart_init(&machine->UART[0], &machine->i8259, 0x3F8, 4, NULL, NULL, NULL, NULL);
	}
	else if ((machine->hwflags & MACHINE_HW_UART0_MOUSE) && !(machine->hwflags & MACHINE_HW_SKIP_UART0)) {
		uart_init(&machine->UART[0], &machine->i8259, 0x3F8, 4, NULL, NULL, (void*)mouse_togglereset, NULL);
		mouse_init(&machine->UART[0]);
		timing_addTimer(mouse_rxpoll, NULL, baudrate / 9, TIMING_ENABLED);
	}
#ifdef ENABLE_TCP_MODEM
	else if ((machine->hwflags & MACHINE_HW_UART0_TCPMODEM) && !(machine->hwflags & MACHINE_HW_SKIP_UART0)) {
		uart_init(&machine->UART[0], &machine->i8259, 0x3F8, 4, (void*)tcpmodem_tx, &machine->tcpmodem[0], NULL, NULL);
		tcpmodem_init(&machine->tcpmodem[0], &machine->UART[0], 23);
		timing_addTimer(tcpmodem_rxpoll, &machine->tcpmodem[0], baudrate / 9, TIMING_ENABLED);
	}
#endif

	if ((machine->hwflags & MACHINE_HW_UART1_NONE) && !(machine->hwflags & MACHINE_HW_SKIP_UART1)) {
		uart_init(&machine->UART[1], &machine->i8259, 0x2F8, 3, NULL, NULL, NULL, NULL);
	}
	else if ((machine->hwflags & MACHINE_HW_UART1_MOUSE) && !(machine->hwflags & MACHINE_HW_SKIP_UART1)) {
		uart_init(&machine->UART[1], &machine->i8259, 0x2F8, 3, NULL, NULL, (void*)mouse_togglereset, NULL);
		mouse_init(&machine->UART[1]);
		timing_addTimer(mouse_rxpoll, NULL, baudrate / 9, TIMING_ENABLED);
	}
#ifdef ENABLE_TCP_MODEM
	else if ((machine->hwflags & MACHINE_HW_UART1_TCPMODEM) && !(machine->hwflags & MACHINE_HW_SKIP_UART1)) {
		uart_init(&machine->UART[1], &machine->i8259, 0x2F8, 3, (void*)tcpmodem_tx, &machine->tcpmodem[1], NULL, NULL);
		tcpmodem_init(&machine->tcpmodem[1], &machine->UART[1], 23);
		timing_addTimer(tcpmodem_rxpoll, &machine->tcpmodem[1], baudrate / 9, TIMING_ENABLED);
	}
#endif

#ifdef USE_NE2000
	if (machine->hwflags & MACHINE_HW_NE2000) {
		ne2000_init(&machine->ne2000, &machine->i8259, 0x300, 7, (uint8_t*)&mac);
		if (machine->pcap_if > -1) {
			if (pcap_xinit(&machine->ne2000, machine->pcap_if)) {
				return -1;
			}
		}
	}
#endif

	cpu_reset(&machine->CPU);
#ifndef USE_DISK_HLE
	fdc_init(&fdc, &machine->CPU, &i8259, &i8237);
	fdc_insert(&fdc, 0, "dos622.img");
#else
	if (machine->int13emu) {
		biosdisk_init(&machine->CPU);
	}
	else {
#ifndef USE_PCEM_IDE
		ata_init(&machine->i8259_slave);
#endif
	}
#endif

	switch (videocard) {
	case VIDEO_CARD_CGA:
		if (cga_init()) return -1;
		break;
	case VIDEO_CARD_TEXT:
		if (text_init()) return -1;
		break;
	case VIDEO_CARD_VGA:
		if (vga_init()) return -1;
		break;
	}

	return 0;
}

int machine_init_opti5x7(MACHINE_t* machine) {
	if (machine == NULL) return -1;

	showops = 0;
	sdlconsole_changeScancodes(2); //use AT-style scancodes set 2

	i8259_init(&machine->i8259, 0x20, NULL);
	i8259_init(&machine->i8259_slave, 0xA0, &machine->i8259);
	i8253_init(&machine->i8253, &machine->i8259, &machine->pcspeaker);
	i8237_init(&machine->i8237, &machine->CPU);
	i8255_init(&machine->i8255, &machine->KeyState, &machine->pcspeaker);
	pcspeaker_init(&machine->pcspeaker);
	i8042_init(&machine->CPU, &machine->i8259);
	opti5x7_init();

	//check machine HW flags and init devices accordingly
	if ((machine->hwflags & MACHINE_HW_BLASTER) && !(machine->hwflags & MACHINE_HW_SKIP_BLASTER)) {
		blaster_init(&machine->blaster, &machine->i8237, &machine->i8259, 0x220, 1, 5);
		OPL3_init(&machine->OPL3);
		machine->mixBlaster = 1;
		machine->mixOPL = 1;
	}
	else if ((machine->hwflags & MACHINE_HW_OPL) && !(machine->hwflags & MACHINE_HW_SKIP_OPL)) { //else if because some games won't detect an SB without seeing the OPL, so if SB enabled then OPL already is
		//opl2_init(&machine->OPL2);
		OPL3_init(&machine->OPL3);
		machine->mixOPL = 1;
	}
	//if ((machine->hwflags & MACHINE_HW_RTC) && !(machine->hwflags & MACHINE_HW_SKIP_RTC)) {
	//}

	if ((machine->hwflags & MACHINE_HW_UART0_NONE) && !(machine->hwflags & MACHINE_HW_SKIP_UART0)) {
		uart_init(&machine->UART[0], &machine->i8259, 0x3F8, 4, NULL, NULL, NULL, NULL);
	}
	else if ((machine->hwflags & MACHINE_HW_UART0_MOUSE) && !(machine->hwflags & MACHINE_HW_SKIP_UART0)) {
		uart_init(&machine->UART[0], &machine->i8259, 0x3F8, 4, NULL, NULL, (void*)mouse_togglereset, NULL);
		mouse_init(&machine->UART[0]);
		timing_addTimer(mouse_rxpoll, NULL, baudrate / 9, TIMING_ENABLED);
	}
#ifdef ENABLE_TCP_MODEM
	else if ((machine->hwflags & MACHINE_HW_UART0_TCPMODEM) && !(machine->hwflags & MACHINE_HW_SKIP_UART0)) {
		uart_init(&machine->UART[0], &machine->i8259, 0x3F8, 4, (void*)tcpmodem_tx, &machine->tcpmodem[0], NULL, NULL);
		tcpmodem_init(&machine->tcpmodem[0], &machine->UART[0], 23);
		timing_addTimer(tcpmodem_rxpoll, &machine->tcpmodem[0], baudrate / 9, TIMING_ENABLED);
	}
#endif

	if ((machine->hwflags & MACHINE_HW_UART1_NONE) && !(machine->hwflags & MACHINE_HW_SKIP_UART1)) {
		uart_init(&machine->UART[1], &machine->i8259, 0x2F8, 3, NULL, NULL, NULL, NULL);
	}
	else if ((machine->hwflags & MACHINE_HW_UART1_MOUSE) && !(machine->hwflags & MACHINE_HW_SKIP_UART1)) {
		uart_init(&machine->UART[1], &machine->i8259, 0x2F8, 3, NULL, NULL, (void*)mouse_togglereset, NULL);
		mouse_init(&machine->UART[1]);
		timing_addTimer(mouse_rxpoll, NULL, baudrate / 9, TIMING_ENABLED);
	}
#ifdef ENABLE_TCP_MODEM
	else if ((machine->hwflags & MACHINE_HW_UART1_TCPMODEM) && !(machine->hwflags & MACHINE_HW_SKIP_UART1)) {
		uart_init(&machine->UART[1], &machine->i8259, 0x2F8, 3, (void*)tcpmodem_tx, &machine->tcpmodem[1], NULL, NULL);
		tcpmodem_init(&machine->tcpmodem[1], &machine->UART[1], 23);
		timing_addTimer(tcpmodem_rxpoll, &machine->tcpmodem[1], baudrate / 9, TIMING_ENABLED);
	}
#endif

#ifdef USE_NE2000
	if (machine->hwflags & MACHINE_HW_NE2000) {
		ne2000_init(&machine->ne2000, &machine->i8259, 0x300, 7, (uint8_t*)&mac);
		if (machine->pcap_if > -1) {
			if (pcap_xinit(&machine->ne2000, machine->pcap_if)) {
				return -1;
			}
		}
	}
#endif

	cpu_reset(&machine->CPU);
#ifndef USE_DISK_HLE
	fdc_init(&fdc, &machine->CPU, &i8259, &i8237);
	fdc_insert(&fdc, 0, "dos622.img");
#else
	if (machine->int13emu) {
		biosdisk_init(&machine->CPU);
	}
	else {
		ata_init(&machine->i8259_slave);
	}
#endif

	switch (videocard) {
	case VIDEO_CARD_CGA:
		if (cga_init()) return -1;
		break;
	case VIDEO_CARD_TEXT:
		if (text_init()) return -1;
		break;
	case VIDEO_CARD_VGA:
		if (vga_init()) return -1;
		break;
	}

	return 0;
}

int machine_init(MACHINE_t* machine, char* id) {
	int num = 0, match = 0, i = 0;

	do {
		if (machine_defs[num].id == NULL) {
			debug_log(DEBUG_ERROR, "[MACHINE] ERROR: Machine definition not found: %s\r\n", id);
			return -1;
		}

		if (_stricmp(id, machine_defs[num].id) == 0) {
			match = 1;
		}
		else {
			num++;
		}
	} while (!match);

	debug_log(DEBUG_INFO, "[MACHINE] Initializing machine: \"%s\" (%s)\r\n", machine_defs[num].description, machine_defs[num].id);

	//Initialize machine memory map
	while(1) {
		static uint8_t* temp;
		if (machine_mem[num][i].memtype == MACHINE_MEM_ENDLIST) {
			break;
		}
		if (machine_mem[num][i].memtype != MACHINE_MEM_ROM_INTERLEAVED_HIGH) { // you have to put the low one right before high in the machine list...
			temp = (uint8_t*)malloc((size_t)machine_mem[num][i].size);
		}
		if ((temp == NULL) &&
			((machine_mem[num][i].required == MACHINE_ROM_REQUIRED) || (machine_mem[num][i].required == MACHINE_ROM_ISNOTROM))) {
			debug_log(DEBUG_ERROR, "[MACHINE] ERROR: Unable to allocate %lu bytes of memory\r\n", machine_mem[num][i].size);
			return -1;
		}
		if (machine_mem[num][i].memtype == MACHINE_MEM_RAM) {
			memory_mapRegister(machine_mem[num][i].start, machine_mem[num][i].size, temp, temp);
		} else if (machine_mem[num][i].memtype == MACHINE_MEM_ROM) {
			int ret;
			ret = utility_loadFile(temp, machine_mem[num][i].size, machine_mem[num][i].filename);
			if ((machine_mem[num][i].required == MACHINE_ROM_REQUIRED) && ret) {
				debug_log(DEBUG_ERROR, "[MACHINE] Could not open file, or size is less than expected: %s\r\n", machine_mem[num][i].filename);
				free(temp);
				return -1;
			}
			memory_mapRegister(machine_mem[num][i].start, machine_mem[num][i].size, temp, NULL);
		} else if (machine_mem[num][i].memtype == MACHINE_MEM_ROM_INTERLEAVED_LOW) {
			int ret;
			uint32_t j;
			uint8_t* temp2;
			temp2 = (uint8_t*)malloc((size_t)machine_mem[num][i].size >> 1);
			ret = utility_loadFile(temp2, machine_mem[num][i].size >> 1, machine_mem[num][i].filename);
			if ((machine_mem[num][i].required == MACHINE_ROM_REQUIRED) && ret) {
				debug_log(DEBUG_ERROR, "[MACHINE] Could not open file, or size is less than expected: %s\r\n", machine_mem[num][i].filename);
				free(temp2);
				return -1;
			}
			for (j = 0; j < (machine_mem[num][i].size >> 1); j++) {
				temp[j << 1] = temp2[j];
			}
			free(temp2);
		}
		else if (machine_mem[num][i].memtype == MACHINE_MEM_ROM_INTERLEAVED_HIGH) {
			int ret;
			uint32_t j;
			uint8_t* temp2;
			temp2 = (uint8_t*)malloc((size_t)machine_mem[num][i].size >> 1);
			ret = utility_loadFile(temp2, machine_mem[num][i].size >> 1, machine_mem[num][i].filename);
			if ((machine_mem[num][i].required == MACHINE_ROM_REQUIRED) && ret) {
				debug_log(DEBUG_ERROR, "[MACHINE] Could not open file, or size is less than expected: %s\r\n", machine_mem[num][i].filename);
				free(temp2);
				return -1;
			}
			for (j = 0; j < (machine_mem[num][i].size >> 1); j++) {
				temp[(j << 1) + 1] = temp2[j];
			}
			free(temp2);
			memory_mapRegister(machine_mem[num][i].start, machine_mem[num][i].size, temp, NULL);
		}
		i++;
	}

	if (machine_defs[num].cmosfile != NULL) {
		cmosrtc_init(machine_defs[num].cmosfile, &machine->i8259_slave);
	}

	machine->hwflags |= machine_defs[num].hwflags;

	if (videocard == 0xFF) {
		videocard = machine_defs[num].video;
	}

	if (speedarg > 0) {
		speed = speedarg;
	} else if (speedarg < 0) {
		speed = -1;
	} else {
		speed = machine_defs[num].speed;
	}

	if ((*machine_defs[num].init)(machine)) { //call machine-specific init routine
		return -1;
	}

	return num;
}

void machine_list() {
	int machine = 0;

	printf("Valid " STR_TITLE " machines:\r\n");

	while(machine_defs[machine].id != NULL) {
		printf("%s: \"%s\"\r\n", machine_defs[machine].id, machine_defs[machine].description);
		machine++;
	}
}
