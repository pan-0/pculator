#ifdef _WIN32
#	define xfseek _fseeki64
#	define xftell _ftelli64
#else
#	/* NOTE: `-D_FILE_OFFSET_BITS=64` must be passed to the compiler! */
#	if !defined _FILE_OFFSET_BITS || _FILE_OFFSET_BITS != 64
#		error "fseek64 not supported"
#	endif
#	define xfseek fseek
#	define xftell ftell
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../../ports.h"
#include "../../chipset/i8259.h"
#include "../../debuglog.h"
#include "../../timing.h"
#include "ata.h"

ATA_t ata;
char ata_swap[20];

void ata_delayed_irq(void* dummy) {
	if (!ata.delay_irq) return;
	if (ata.disk[ata.select].interrupt) {
		i8259_doirq(ata.i8259, 6);
	}
	ata.delay_irq = 0;
	ata.dscflag = ATA_STATUS_DSC;
	timing_timerDisable(ata.timernum);
}

void ata_reset_cb(void* dummy) {
	ata.inreset = 0;
	timing_timerDisable(ata.resettimer);
}

void ata_irq() {
	ata.delay_irq = 1;
	timing_timerEnable(ata.timernum);
}

void ata_swap_string(const char* str) {
	int i;
	size_t len = strlen(str);
	memcpy(ata_swap, str, len);
	memset(ata_swap + len, '\0', sizeof(ata_swap) - len);
	for (i = 0; i < 20; i += 2) {
		char tmp;
		tmp = ata_swap[i + 1];
		ata_swap[i + 1] = ata_swap[i];
		ata_swap[i] = tmp;
	}
}

uint8_t ata_gen_status() {
	//if (ata.disk[ata.select].diskfile == NULL) return 0; // ATA_STATUS_DRDY | ATA_STATUS_DSC;
	if (ata.disk[ata.select].error == 4) {
		return ATA_STATUS_DRDY | ATA_STATUS_DSC | ATA_STATUS_ERR;
	}
	/*if (ata.error == 4) {
		return ATA_STATUS_ERR;
	}*/
	if (ata.delay_irq || ata.inreset) {
		return ATA_STATUS_BUSY;
	}
	if (ata.disk[ata.select].iswriting || ata.disk[ata.select].isreading) {
		//if (ata.readssincecommand < 1) return ATA_STATUS_BUSY;
		if (ata.delay_irq) return ATA_STATUS_BUSY;
		return ATA_STATUS_DRQ | ATA_STATUS_DRDY | ata.dscflag;
	}
	//if ((ata.curreadsect <= ata.targetsect)) return ATA_STATUS_DRQ | ATA_STATUS_DRDY;
	return ATA_STATUS_DRDY | ATA_STATUS_DSC;
}

void ata_read_disk() {
	uint32_t curlba;
	if (ata.disk[ata.select].lbamode) {
		xfseek(ata.disk[ata.select].diskfile, ata.disk[ata.select].regs.lba++ * 512LU, SEEK_SET);
		fread(ata.disk[ata.select].buffer, 1, 512, ata.disk[ata.select].diskfile);
		ata.disk[ata.select].error = 0;
		ata.disk[ata.select].buffer_pos = 0;
		ata.disk[ata.select].curreadsect++;
	}
	else {
		curlba = ((ata.disk[ata.select].curcyl * (uint32_t)ata.disk[ata.select].heads + ata.disk[ata.select].curhead) * (uint32_t)ata.disk[ata.select].spt + (ata.disk[ata.select].cursect - 1));
		xfseek(ata.disk[ata.select].diskfile, curlba * 512LU, SEEK_SET);
		fread(ata.disk[ata.select].buffer, 1, 512, ata.disk[ata.select].diskfile);
		ata.disk[ata.select].error = 0;
		ata.disk[ata.select].buffer_pos = 0;
		ata.disk[ata.select].curreadsect++;
		ata.disk[ata.select].regs.lba = (ata.disk[ata.select].regs.lba & ~63) | ata.disk[ata.select].cursect;
		ata.disk[ata.select].regs.lba = (ata.disk[ata.select].regs.lba & 0xFF0000FF) | (ata.disk[ata.select].curcyl << 8);
		ata.disk[ata.select].regs.lba = (ata.disk[ata.select].regs.lba & 0xF0FFFFFF) | (ata.disk[ata.select].curhead << 24);
		ata.disk[ata.select].cursect++;
		if (ata.disk[ata.select].cursect > ata.disk[ata.select].spt) {
			ata.disk[ata.select].cursect = 1;
			ata.disk[ata.select].curhead++;
			if (ata.disk[ata.select].curhead == ata.disk[ata.select].heads) {
				ata.disk[ata.select].curhead = 0;
				ata.disk[ata.select].curcyl++;
			}
		}
	}
	ata.dscflag = 0;
	if (ata.disk[ata.select].interrupt) {
		//i8259_doirq(ata.i8259, 6); //6 is IRQ 14 on this slave PIC
		ata_irq();
	}
}

void ata_write_disk() {
	uint32_t curlba;
	uint16_t i;
	if (ata.disk[ata.select].lbamode) {
		xfseek(ata.disk[ata.select].diskfile, ata.disk[ata.select].regs.lba++ * 512LU, SEEK_SET);
		fwrite(ata.disk[ata.select].buffer, 1, 512, ata.disk[ata.select].diskfile);
		ata.disk[ata.select].error = 0;
		ata.disk[ata.select].buffer_pos = 0;
		ata.disk[ata.select].curreadsect++;
	}
	else {
		curlba = ((ata.disk[ata.select].curcyl * (uint32_t)ata.disk[ata.select].heads + ata.disk[ata.select].curhead) * (uint32_t)ata.disk[ata.select].spt + (ata.disk[ata.select].cursect - 1));
		xfseek(ata.disk[ata.select].diskfile, curlba * 512LU, SEEK_SET);
		fwrite(ata.disk[ata.select].buffer, 1, 512, ata.disk[ata.select].diskfile);
		ata.disk[ata.select].error = 0;
		ata.disk[ata.select].buffer_pos = 0;
		ata.disk[ata.select].curreadsect++;
		ata.disk[ata.select].regs.lba = (ata.disk[ata.select].regs.lba & ~63) | ata.disk[ata.select].cursect;
		ata.disk[ata.select].regs.lba = (ata.disk[ata.select].regs.lba & 0xFF0000FF) | (ata.disk[ata.select].curcyl << 8);
		ata.disk[ata.select].regs.lba = (ata.disk[ata.select].regs.lba & 0xF0FFFFFF) | (ata.disk[ata.select].curhead << 24);
		ata.disk[ata.select].cursect++;
		if (ata.disk[ata.select].cursect > ata.disk[ata.select].spt) {
			ata.disk[ata.select].cursect = 1;
			ata.disk[ata.select].curhead++;
			if (ata.disk[ata.select].curhead == ata.disk[ata.select].heads) {
				ata.disk[ata.select].curhead = 0;
				ata.disk[ata.select].curcyl++;
			}
		}
	}
	ata.dscflag = 1;
	if (ata.disk[ata.select].interrupt) {
		//i8259_doirq(ata.i8259, 6); //6 is IRQ 14 on this slave PIC
		ata_irq();
	}
}

extern int showops;
void ata_command_process() {
	//printf("[ATA] Command: %02X\n", ata.disk[ata.select].command);
	ata.disk[ata.select].error = 0;
	ata.disk[ata.select].iswriting = 0;
	ata.disk[ata.select].isreading = 0;
	ata.readssincecommand = 0;
	ata.disk[ata.select].lastcmd = ata.disk[ata.select].command;
	switch (ata.disk[ata.select].command) {
	case ATA_CMD_IDENTIFY:
		//printf("ATA identify, select disk %u\n", ata.select);
		if (ata.disk[ata.select].diskfile != NULL) {
			//FILE* idf;
			//idf = fopen("c:\\abandon\\identify.bin", "rb");
			memset(ata.disk[ata.select].buffer, 0, 512);
			ata.disk[ata.select].iswriting = 0;
			ata.disk[ata.select].isreading = 1;
			ata.disk[ata.select].buffer_pos = 0;
			ata.disk[ata.select].curreadsect = 1;
			ata.disk[ata.select].targetsect = 1;
			ata.disk[ata.select].error = 0;
			ata.disk[ata.select].buffer[0] = 0x40; //fixed disk
			ata.disk[ata.select].buffer[2] = ata.disk[ata.select].cylinders & 0xFF;
			ata.disk[ata.select].buffer[3] = ata.disk[ata.select].cylinders >> 8;
			ata.disk[ata.select].buffer[6] = ata.disk[ata.select].heads;
			ata.disk[ata.select].buffer[12] = ata.disk[ata.select].spt;
			ata_swap_string("12345678  ");
			memcpy(&ata.disk[ata.select].buffer[20], ata_swap, 10); //Serial number
			ata_swap_string("1.0 ");
			memcpy(&ata.disk[ata.select].buffer[46], ata_swap, 4); //Firmware revision
			ata_swap_string("PCulator virt disk  ");
			memcpy(&ata.disk[ata.select].buffer[54], ata_swap, 20); //Model
			//ata.buffer[94] = 0x01;
			ata.disk[ata.select].buffer[96] = 0; // 0x80;
			ata.disk[ata.select].buffer[99] = 0x02; //LBA supported (make it 0x03 for LBA and DMA) TODO: Should be index 99?
			//ata.buffer[100] = 0x02;
			//ata.buffer[102] = 63; //sectors low
			ata.disk[ata.select].buffer[120] = ata.disk[ata.select].sectors;
			ata.disk[ata.select].buffer[121] = ata.disk[ata.select].sectors >> 8;
			ata.disk[ata.select].buffer[122] = ata.disk[ata.select].sectors >> 16;
			ata.disk[ata.select].buffer[123] = ata.disk[ata.select].sectors >> 24;
			//fread(ata.buffer, 1, 512, idf);
			//fclose(idf);
			/*if (ata.disk[ata.select].interrupt) {
				//i8259_doirq(ata.i8259, 6); //Slave PIC, so 6 = IRQ 14
				ata_irq();
			}*/
		}
		else {
			ata.disk[ata.select].error = 4;
		}
		if (ata.disk[ata.select].interrupt) {
			i8259_doirq(ata.i8259, 6);
		}
		break;

	case ATA_CMD_DIAGNOSTIC:
		ata.disk[ata.select].error = 0; //no error
		break;

	case ATA_CMD_DEVICE_RESET:
		ata.disk[ata.select].error = 4;
		ata.disk[0].isreading = 0;
		ata.disk[0].iswriting = 0;
		ata.disk[0].regs.lba = 0;
		ata.disk[1].isreading = 0;
		ata.disk[1].iswriting = 0;
		ata.disk[1].regs.lba = 0;
		ata.select = 0;
		i8259_clearirq(ata.i8259, 6);
		timing_timerDisable(ata.timernum);
		ata.delay_irq = 0;
		//ata.inreset = 1;
		//timing_timerEnable(ata.resettimer);
		if (ata.disk[ata.select].interrupt) {
			i8259_doirq(ata.i8259, 6);
		}
		break;

	/*case ATA_CMD_READ_MULTIPLE:
	{
		uint32_t sector, cyl, head;
		printf("[ATA] Read multiple sectors (%u)\n", ata.regs.sectors);
		ata.error = 0;
		ata.curreadsect = 0;
		ata.targetsect = (ata.regs.sectors == 0) ? 256 : ata.regs.sectors;
		if (ata.lbamode) {
			ata.savelba = ata.regs.lba;
		}
		else {
			sector = ata.regs.lba & 63; // 0xFF;
			cyl = (ata.regs.lba >> 8) & 0xFFFF;
			head = (ata.regs.lba >> 24) & 0x0F;
			ata.savelba = (cyl * (uint32_t)ata.disk[ata.select].heads + head) * (uint32_t)ata.disk[ata.select].spt + (sector - 1);
			//printf("CHS: %u, %u, %u (LBA %lu)\n", cyl, head, sector, lba);
		}
		ata_read_disk();
		break;
	}*/

	case ATA_CMD_INITIALIZE_PARAMS:
		ata.disk[ata.select].error = 0;
		ata.disk[ata.select].spt = ata.disk[ata.select].regs.sectors & 63;
		ata.disk[ata.select].heads = ((ata.disk[ata.select].regs.lba >> 24) & 0x0F) + 1;
		if (ata.disk[ata.select].interrupt) {
			//i8259_doirq(ata.i8259, 6);
			ata_irq();
		}
		break;

	case 0x41:
	case 0x70:
	case 0xEF:
	case 0x40: //READ VERIFY
	case ATA_CMD_IDLE_IMMEDIATE:
		ata.disk[ata.select].error = 0;
		if (ata.disk[ata.select].interrupt) {
			//i8259_doirq(ata.i8259, 6);
			ata_irq();
		}
		break;

	case ATA_CMD_RECALIBRATE:
		ata.disk[ata.select].error = 0;
		if (ata.disk[ata.select].interrupt) {
			//i8259_doirq(ata.i8259, 6);
			ata_irq();
		}
		break;

	case ATA_CMD_READ_SECTORS:
	case 0x21:
	{
		uint32_t sector, cyl, head;
		//printf("[ATA] Read sectors disk %u (%u)\n", ata.select, ata.regs.sectors);
		ata.disk[ata.select].error = 0;
		ata.disk[ata.select].iswriting = 0;
		ata.disk[ata.select].isreading = 1;
		ata.disk[ata.select].curreadsect = 0;
		ata.disk[ata.select].targetsect = (ata.disk[ata.select].regs.sectors == 0) ? 256 : ata.disk[ata.select].regs.sectors;
		if (ata.disk[ata.select].lbamode) {
			//printf("LBA: %lu\n", ata.disk[ata.select].regs.lba);
			ata.savelba = ata.disk[ata.select].regs.lba & 0xFFFFFF;
		}
		else {
			sector = ata.disk[ata.select].regs.lba & 63; // 0xFF;
			cyl = (ata.disk[ata.select].regs.lba >> 8) & 0xFFFF;
			head = (ata.disk[ata.select].regs.lba >> 24) & 0x0F;
			ata.disk[ata.select].curcyl = (ata.disk[ata.select].regs.lba >> 8) & 0xFFFF;
			ata.disk[ata.select].curhead = (ata.disk[ata.select].regs.lba >> 24) & 0xFF;
			ata.disk[ata.select].cursect = ata.disk[ata.select].regs.lba & 63;
			ata.savelba = (cyl * (uint32_t)ata.disk[ata.select].heads + head) * (uint32_t)ata.disk[ata.select].spt + (sector - 1);
			//printf("CHS: %u, %u, %u (LBA %lu)\n", cyl, head, sector, ata.savelba);
		}
		ata_read_disk();
		break;
	}

	case ATA_CMD_WRITE_SECTORS:
	case 0x31:
	{
		uint32_t sector, cyl, head;
		//printf("[ATA] Write sectors disk %u (%u)\n", ata.select, ata.regs.sectors);
		ata.disk[ata.select].error = 0;
		ata.disk[ata.select].iswriting = 1;
		ata.disk[ata.select].isreading = 0;
		ata.disk[ata.select].curreadsect = 0;
		ata.disk[ata.select].buffer_pos = 0;
		ata.disk[ata.select].targetsect = (ata.disk[ata.select].regs.sectors == 0) ? 256 : ata.disk[ata.select].regs.sectors;
		if (ata.disk[ata.select].lbamode) {
			//printf("LBA: %lu\n", ata.regs.lba);
			ata.savelba = ata.disk[ata.select].regs.lba;
		}
		else {
			sector = ata.disk[ata.select].regs.lba & 63; // 0xFF;
			cyl = (ata.disk[ata.select].regs.lba >> 8) & 0xFFFF;
			head = (ata.disk[ata.select].regs.lba >> 24) & 0x0F;
			ata.disk[ata.select].curcyl = (ata.disk[ata.select].regs.lba >> 8) & 0xFFFF;
			ata.disk[ata.select].curhead = (ata.disk[ata.select].regs.lba >> 24) & 0xFF;
			ata.disk[ata.select].cursect = ata.disk[ata.select].regs.lba & 63;
			ata.savelba = (cyl * (uint32_t)ata.disk[ata.select].heads + head) * (uint32_t)ata.disk[ata.select].spt + (sector - 1);
			//printf("CHS: %u, %u, %u (LBA %lu)\n", cyl, head, sector, ata.savelba);
		}
		break;
	}

	default:
		printf("[ATA] Unimplemented command: 0x%02X\n", ata.disk[ata.select].command);
	}
}

uint8_t ata_read_port(void* dummy, uint32_t portnum) {
	uint8_t ret = 0;
	switch (portnum) {
	case ATA_PORT_DATA:
		if (ata.disk[ata.select].buffer_pos >= 512) return 0;
		//ret = ata.buffer[(ata.buffer_pos & 0xFFFE) | ((ata.buffer_pos & 1) ^ 1)];
		ret = ata.disk[ata.select].buffer[ata.disk[ata.select].buffer_pos];
		ata.disk[ata.select].buffer_pos++;
		if (ata.disk[ata.select].buffer_pos >= 512) {
			if (ata.disk[ata.select].curreadsect >= ata.disk[ata.select].targetsect) {
				ata.disk[ata.select].isreading = 0;
			} else {
				ata_read_disk();
			}
		}
		break;

	case ATA_PORT_ERROR:
		ret = ata.disk[ata.select].error;
		break;

	case ATA_PORT_SECTORS:
		ret = ata.disk[ata.select].regs.sectors;
		break;

	case ATA_PORT_LBA_LOW:
		ret = ata.disk[ata.select].regs.lba & 0xFF;
		break;

	case ATA_PORT_LBA_MID:
		ret = (ata.disk[ata.select].regs.lba >> 8) & 0xFF;
		break;

	case ATA_PORT_LBA_HIGH:
		ret = (ata.disk[ata.select].regs.lba >> 16) & 0xFF;
		break;

	case ATA_PORT_DRIVE:
		ret = 0xA0 | ((ata.disk[ata.select].regs.lba >> 24) & 0x0F) | (ata.select << 4) | (ata.disk[ata.select].lbamode << 6);
		break;

	case ATA_PORT_STATUS:
		ret = ata_gen_status();
		i8259_clearirq(ata.i8259, 6);
		timing_timerDisable(ata.timernum);
		ata.delay_irq = 0;
		break;

	case ATA_PORT_ALTERNATE:
		ret = ata_gen_status();
		break;
	}

	//printf("[ATA] Read port 0x%X = 0x%02X\n", portnum, ret);
	return ret;
}

void ata_write_port(void* dummy, uint32_t portnum, uint8_t value) {
	//printf("[ATA] Write port 0x%X <- 0x%02X\n", portnum, value);
	switch (portnum) {
	case ATA_PORT_DATA:
		printf("ATA 8-bit data write\n");
		break;

	case ATA_PORT_FEATURES:
		ata.disk[ata.select].regs.features = value;
		break;

	case ATA_PORT_SECTORS:
		ata.disk[ata.select].regs.sectors = value;
		break;

	case ATA_PORT_LBA_LOW:
		ata.disk[ata.select].regs.lba = (ata.disk[ata.select].regs.lba & 0xFFFFFF00) | value;
		break;

	case ATA_PORT_LBA_MID:
		ata.disk[ata.select].regs.lba = (ata.disk[ata.select].regs.lba & 0xFFFF00FF) | ((uint32_t)value << 8);
		break;

	case ATA_PORT_LBA_HIGH:
		ata.disk[ata.select].regs.lba = (ata.disk[ata.select].regs.lba & 0xFF00FFFF) | ((uint32_t)value << 16);
		break;

	case ATA_PORT_DRIVE:
		ata.select = (value >> 4) & 1; //master or slave
		ata.disk[ata.select].lbamode = (value >> 6) & 1;
		ata.disk[ata.select].regs.lba = (ata.disk[ata.select].regs.lba & 0x00FFFFFF) | ((uint32_t)(value & 0x0F) << 24);
		//printf("Select %s disk\n", ata.select ? "slave" : "master");
		break;

	case ATA_PORT_COMMAND:
		ata.disk[ata.select].command = value;
		ata_command_process();
		break;

	case ATA_PORT_ALTERNATE:
		ata.disk[ata.select].interrupt = ((value >> 1) & 1) ^ 1;
		ata.disk[ata.select].error = 1;
		//showops = 1;
		//printf("[ATA] Interrupts for %s %s\n", ata.select ? "slave" : "master", ata.disk[ata.select].interrupt ? "enabled" : "disabled");
		break;
	}
}

uint16_t ata_read_data(void* dummy, uint32_t portnum) {
	uint16_t ret;
	//printf("[ATA] Read data from 16-bit data port\n");
	if (ata.disk[ata.select].buffer_pos >= 512 || !ata.disk[ata.select].isreading) return 0;
	ret = (uint16_t)ata.disk[ata.select].buffer[ata.disk[ata.select].buffer_pos] | ((uint16_t)ata.disk[ata.select].buffer[ata.disk[ata.select].buffer_pos + 1] << 8);
	ata.disk[ata.select].buffer_pos += 2;
	if (ata.disk[ata.select].buffer_pos >= 512) {
		//printf("[ATA] completed 512 byte read\n");
		if (ata.disk[ata.select].interrupt) {
			//i8259_doirq(ata.i8259, 6);
			ata_irq();
		}
		if (ata.disk[ata.select].curreadsect >= ata.disk[ata.select].targetsect) {
			ata.disk[ata.select].isreading = 0;
		}
		else {
			ata_read_disk();
		}
	}
	return ret;
}

void ata_write_data(void* dummy, uint32_t portnum, uint16_t value) {
	//printf("[ATA] Write data to 16-bit data port\n");
	if (ata.disk[ata.select].buffer_pos >= 512 || !ata.disk[ata.select].iswriting) return;
	ata.disk[ata.select].buffer[ata.disk[ata.select].buffer_pos] = value & 0xFF;
	ata.disk[ata.select].buffer[ata.disk[ata.select].buffer_pos + 1] = value >> 8;
	ata.disk[ata.select].buffer_pos += 2;
	if (ata.disk[ata.select].buffer_pos >= 512) {
		ata_write_disk();
		if (ata.disk[ata.select].curreadsect >= ata.disk[ata.select].targetsect) {
			ata.disk[ata.select].iswriting = 0;
		}
	}
}

int ata_insert_disk(int select, char* filename) {
	uint32_t chs_total;
	ata.disk[select].diskfile = fopen(filename, "r+b");
	if (ata.disk[select].diskfile == NULL) return 0;
	xfseek(ata.disk[select].diskfile, 0, SEEK_END);
	ata.disk[select].sectors = xftell(ata.disk[select].diskfile) / 512UL;
	ata.disk[select].spt = 63;
	ata.disk[select].heads = 16;
	ata.disk[select].cylinders = ata.disk[select].sectors / (16L * 63L);
	chs_total = ata.disk[select].cylinders * ata.disk[select].heads * ata.disk[select].spt;
	if (chs_total > ata.disk[select].sectors) {
		ata.disk[select].cylinders--; //keep CHS inside LBA boundary
	}
	debug_log(DEBUG_INFO, "[ATA] Inserted disk on %s channel: %s\n", select ? "slave" : "master", filename);
}

void ata_init(I8259_t* i8259) {
	ata.disk[ata.select].buffer_pos = 512;
	ata.i8259 = i8259;
	ata.delay_irq = 0;

	ata.timernum = timing_addTimer(ata_delayed_irq, NULL, 1500, 0);
	ata.resettimer = timing_addTimer(ata_reset_cb, NULL, 4, 0);

	ports_cbRegister(0x1F0, 1, ata_read_port, ata_read_data, ata_write_port, ata_write_data, NULL);
	ports_cbRegister(0x1F1, 7, ata_read_port, NULL, ata_write_port, NULL, NULL);
	ports_cbRegister(0x3F6, 1, ata_read_port, NULL, ata_write_port, NULL, NULL);
}
