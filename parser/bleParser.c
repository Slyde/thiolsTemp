/** Bluetooth low energy frame parser
 * Copyright 2016 Sylvain Décastel
 * 
 * Typical usage : hcidump -i hci0 -R |./bleParser
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "../common/common.h"
#include "../common/libdb.h"

#define ASCII_FRAME_START_CHAR '>'

#define BYTE(b) 0xFF&b


typedef struct {
	char macAddr[20];
	float temperature;
	int battery;
	int rssi;
}Point_t;


int convertASCII2Bin(char* pFrame, uint16_t len)
{
	char tmpVal;
	char* src = pFrame;
	char* dest = pFrame; // re-use frame buffer for result
	bool MSB = true;

	// found first char
	while(len>0 && *src != ASCII_FRAME_START_CHAR) {
		len--;
		src++;
	}
	if (len == 0) {
		printf("convert:: error len == 0\r\n");
	}

	// convert all chars to bytes

	while(len-- > 0) { 
		src++;
		if (*src >= '0' && *src <= '9')
			tmpVal = *src - '0';
		else if (*src >= 'A' && *src <= 'F')
			tmpVal = 10 + *src -'A';
		else if (*src > 'a' && *src <= 'f')
			tmpVal = 10 + *src - 'a';
		else
			continue;

		if (MSB)
			*dest = tmpVal << 4;
		else
			*dest++ += tmpVal;
		MSB = !MSB;
	}

//	printf("convert::Nb of bytes converted : %u\r\n", dest-pFrame);
//	printf("convert::array : ");
//	src = pFrame;

//	while(src<dest) {
//		printf("%02.2X ", *src);
//		src++;
//	}
//	printf("\r\n\n");	

	return dest-pFrame;
}

float IEEE11073_to_float(uint32_t mantissa, char exponent)
{
	return (float) (mantissa * pow(10.0, (double)exponent));
}


int decodeFields(char* pBuffer, uint16_t len, Point_t *pPoint)
{
	char startBytes[] = {0x04,0x3E,0x26,0x02,0x01,0x03,0x01};
	int nbServices;
	char *p;
	char mac[6];	

	// we are looking for a specific length
	if (len != 41)
		return -1;

	// check first bytes
	if (memcmp(pBuffer,startBytes, sizeof(startBytes)) != 0)
		return -2;


	memset(pPoint, 0, sizeof(Point_t));


	// get the number of services
	nbServices = pBuffer[5];
	printf("Nb services : %u\r\n", nbServices);

	// decode the MAC address
	memcpy(mac, &pBuffer[7], sizeof(mac));
	sprintf(pPoint->macAddr, "%02X:%02X:%02X:%02X:%02X:%02X",BYTE(mac[0]),BYTE(mac[1]),BYTE(mac[2]),BYTE(mac[3]),BYTE(mac[4]),BYTE(mac[5]));

	// advertisement type not checked

	// decode the services
	p = &pBuffer[17];
	while(nbServices-- > 0) {
		int nbBytes = p[0];
		unsigned char type = p[1];
		uint16_t service = (p[2]<<8)+p[3];
		char tmpBuffer[80];

		printf("Nb bytes : %u | Type : 0x%02X | Service %04X\r\n", nbBytes, type, service);
		
		switch(service) {
		case 0x4546:
			memset(tmpBuffer, 0, sizeof(tmpBuffer));
			memcpy(tmpBuffer, &p[4], nbBytes-4);
			printf("Name : %s\r\n", tmpBuffer);
		break;

		case 0x0918:{
			char exponent = p[7];
			uint32_t mantissa = ((uint32_t)p[6]<<16) & 0x00FF0000;
			mantissa += ((uint32_t)p[5]<<8) & 0x0000FF00;
			mantissa += (p[4]) & 0x000000FF;
			pPoint->temperature = IEEE11073_to_float(mantissa, exponent);
			printf("Temperature :(0x%06X)  (%u)^(%d) =  %f\r\n",mantissa, mantissa, exponent, pPoint->temperature);
		}break;

		case 0x0F18:
			printf("Battery level : %u\r\n", p[4]);
			pPoint->battery = p[4];
		break;

		}

		// point to next service
		p += nbBytes + 1;
	}

	// decode the RSSI
	{
		pPoint->rssi = -1 & pBuffer[len-1];
		printf("RSSI : (%02X) %d\r\n", pPoint->rssi, pPoint->rssi); 
	}

	return 0;
}


int main(void)
{
	char buffer[255];
	ssize_t ret;
	ssize_t n;
	DB_Handle_t db;
	Point_t point;

	printf("\r\nBLE parser V0.2 started");

	printf("\r\nWaiting for incoming data");

	setvbuf(stdin, NULL, _IONBF, 0);

	memset(&db, 0, sizeof(db));

	if (db_open(&db)) {
		printf("\r\n\nFailed to open db");
		return -1;
	}

	do {
		ret = read(fileno(stdin), buffer, sizeof(buffer));
		buffer[ret] = '\0';
		if (ret < 100)
			continue;
		printf("\r\nbuffer (%u) : %s\r\n", ret, buffer);

		n = convertASCII2Bin(buffer, ret);
		decodeFields(buffer, n, &point);

		if (db_checkNodeAddr(&db, point.macAddr)) {
			db_addPoint(&db, point.macAddr, point.temperature, point.battery, point.rssi);
		}

		fflush(stdout);
	}while(ret >= 0);

	db_close(&db);

        return(0);
}


