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


static bool done=false;

void bleScanner_evtCb(le_advertising_info* info, int len)
{
	char str[50];
	memset(str, 0, sizeof(str));
	ba2str(&info->bdaddr, str);
	printf("\r\naddr : %s (%d)", str, len);
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

	printf("\r\nScan...");
	bleScanner_startScan(bleScanner_evtCb);
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

