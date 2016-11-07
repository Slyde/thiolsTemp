/*
 ============================================================================
 Name        : thiolsTempTest.c
 Author      : SD
 Version     :
 Copyright   :
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <bluetooth/bluetooth.h>
#include <signal.h>
#include <stdbool.h>

#include "bleScanner.h"
#include "bleParser.h"


const bdaddr_t MACFilter[] = {
	{{0xB0, 0x48, 0x64, 0x98, 0x31, 0xC5}}
};

static bool done=false;

void bleScanner_evtCb(le_advertising_info* info, int len)
{
	char str[50];
	int i;

	memset(str, 0, sizeof(str));
	ba2str(&info->bdaddr, str);
	printf("\r\n----------------------");
	printf("\r\n%s", str);

	printf("\r\n  infos:");
	printf("\r\n    len : %d (%d)", len, info->length);
	printf("\r\n    type : %d ", info->evt_type);
	printf("\r\n    data : ");
	for (i=0; i<info->length; i++)
		printf("%02x ", info->data[i]);

	printf("\r\n");

}

void advServiceEvtHandler(bdaddr_t* pAddr, uint16_t uuid, void* pData, int len)
{
	char str[50];

	memset(str, 0, sizeof(str));
	ba2str(pAddr, str);
	printf("\r\n----------------------");
	printf("\r\n%s", str);

	printf("\r\n  infos:");
	printf("\r\n    uuid : %04x", uuid);

	if (uuid == UUID_TEMPERATURE) {
		float temp = bleParser_decodeTemperatureService(pData);
		printf("\r\n    temp : %f", temp);
	} else if (uuid == UUID_BATTERY) {
		uint8_t bat = bleParser_decodeBatteryService(pData);
		printf("\r\n    bat : %u", bat);
	}

	printf("\r\n");
}

static void sigint_handler(int sig)
{
	done = true;
}

int main(int argc, const char* argv[])
{
	struct sigaction sa;

	int ret;

	printf("\r\nInit...");
	ret = bleScanner_init();
	printf("%d", ret);

	bleParser_init(MACFilter, 1, advServiceEvtHandler);

	printf("\r\nScan...");
	bleScanner_startScan(bleParser_advEvtHandler);
	printf("%d", ret);
	fflush(stdout);

	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_NOCLDSTOP;
	sa.sa_handler = sigint_handler;
	sigaction(SIGINT, &sa, NULL);

	while(!done) {
		bleScanner_parseMsgs();
	}

	printf("Done\r\n");


}

