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
	pcap interface for Win32
*/

#include "../../config.h"

#ifdef USE_NE2000

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>  /* exit() */
#ifdef _WIN32
#include <process.h>
#else
#include <pthread.h>
pthread_t pcap_dispatchThreadID;
#endif
#include <pcap.h>
#include "../../debuglog.h"
#include "../../utility.h"
#include "../../timing.h"
#include "ne2000.h"
#include "pcap-win32.h"

pcap_t* pcap_adhandle;

NE2000_t* pcap_ne2000 = NULL;

int pcap_active = 0;

uint32_t pcap_timer;
void pcap_check_packets(void* dummy);

void pcap_listdevs() {
	pcap_if_t* alldevs;
	pcap_if_t* d;
	int i = 0;
	char errbuf[PCAP_ERRBUF_SIZE];

	/* Retrieve the device list from the local machine */
#ifdef _WIN32
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL /* auth is not needed */, &alldevs, errbuf) == -1)
#else
	if (pcap_findalldevs(&alldevs, errbuf) == -1)
#endif
	{
		fprintf(stderr, "Error in pcap_findalldevs_ex: %s\n", errbuf);
		exit(1);
	}

	/* Print the list */
	for (d = alldevs; d != NULL; d = d->next)
	{
		printf("%d. %s", ++i, d->name);
		if (d->description)
			printf(" (%s)\n", d->description);
		else
			printf(" (No description available)\n");
	}

	if (i == 0)
	{
		printf("\nNo interfaces found! Make sure pcap is installed.\n");
		return;
	}

	/* We don't need any more the device list. Free it */
	pcap_freealldevs(alldevs);
}

int pcap_xinit(NE2000_t* ne2000, int dev) {
	pcap_if_t* alldevs;
	pcap_if_t* d;
	int i = 0;
	char errbuf[PCAP_ERRBUF_SIZE];

#ifdef _WIN32
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1) {
#else
	if (pcap_findalldevs(&alldevs, errbuf) == -1) {
#endif
		debug_log(DEBUG_ERROR, "Error in pcap_findalldevs: %s\n", errbuf);
		return -1;
	}

	for (d = alldevs, i = 0; i < dev - 1; d = d->next, i++);

	debug_log(DEBUG_INFO, "[PCAP-WIN32] Initializing pcap library using device: \"%s\"\r\n", d->description ? d->description : "No description available");

#ifdef _WIN32
	if ((pcap_adhandle = pcap_open(d->name, 65536, PCAP_OPENFLAG_PROMISCUOUS, 1000, NULL, errbuf)) == NULL) {
#else
	if ((pcap_adhandle = pcap_open_live(d->name, 65535, 1, -1, NULL)) == NULL) {
#endif
		debug_log(DEBUG_ERROR, "\nUnable to open the adapter. %s is not supported by pcap\n", d->name);
		pcap_freealldevs(alldevs);
		return -1;
	}
	pcap_setnonblock(pcap_adhandle, 1, errbuf);
	pcap_active = 1;

	pcap_freealldevs(alldevs);

	pcap_ne2000 = ne2000;

	pcap_timer = timing_addTimer(pcap_check_packets, NULL, 1000, TIMING_ENABLED);


/*
#ifdef _WIN32
	_beginthread((void*)pcap_dispatchThread, 0, NULL);
#else
	pthread_create(&pcap_dispatchThreadID, NULL, pcap_dispatchThread, NULL);
#endif
*/
	return 0;
}

void pcap_dispatchThread() {
	//pcap_loop(pcap_adhandle, 0, pcap_rx_handler, NULL);
	while (running) {
		pcap_dispatch(pcap_adhandle, 1, pcap_rx_handler, NULL);
		//if (pcap_havePacket == 0) {
			utility_sleep(10);
		//}
	}
}

struct pcap_packet_s {
	u_char data[4096];
	int len;
} pcap_buffer[128];

int pcap_bfrcount = 0;

void pcap_rx_buf(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data) {
	(void)(param); //unused variable

	if (pcap_bfrcount == 128) pcap_bfrcount = 0; //if we've overflowed, just dump all the packets in memory and start over
	pcap_buffer[pcap_bfrcount].len = header->caplen;
	memcpy(pcap_buffer[pcap_bfrcount].data, pkt_data, header->caplen > 4096 ? 4096 : header->caplen);
	pcap_bfrcount++;
}

void pcap_check_packets(void* dummy) {
	int i;

	if (!pcap_active) return;

	while (1) {
		if (!pcap_dispatch(pcap_adhandle, 1, pcap_rx_buf, NULL)) break; // pcap_rx_handler, NULL);
	}

	if (pcap_bfrcount == 0) return;
	ne2000_rx_frame(pcap_ne2000, pcap_buffer[0].data, pcap_buffer[0].len);

	for (i = 1; i < pcap_bfrcount; i++) {
		memcpy(pcap_buffer[i - 1].data, pcap_buffer[i].data, 4096);
		pcap_buffer[i - 1].len = pcap_buffer[i].len;
	}
	pcap_bfrcount--;
}

void pcap_rx_handler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data) {
	(void)(param); //unused variable

	//if (!pcap_ne2000->IMR.rx_inte || pcap_ne2000->ISR.pkt_rx) {
	//	return;
	//}

	ne2000_rx_frame(pcap_ne2000, (void*)pkt_data, header->caplen);
}

void pcap_txPacket(u_char* data, int len) {
	pcap_sendpacket(pcap_adhandle, data, len);
}

#endif
