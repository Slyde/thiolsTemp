/*
 * bleParser.c
 *
 *  Created on: Nov 6, 2016
 *      Author: sylvain
 */
#include "bleParser.h"
#include <stdbool.h>
#include <math.h>

static bdaddr_t *pMACFilterTable = NULL;
static uint16_t MACFilterTableLen = 0;
static bleParser_advServiceEvtHandler_t pValidEvtHandler = NULL;

static bool isValidAddr(bdaddr_t *addr)
{
	bdaddr_t *p = pMACFilterTable;
	int i;

	if (!p)
		return false;

	for (i=0; i<MACFilterTableLen; i++) {
		if (memcmp(p, addr, sizeof(bdaddr_t)) == 0) {
			return true;
		}
		p++;
	}

	return false;
}

static void parseAdvData(le_advertising_info* info, int len)
{
	uint8_t *p = info->data;
	uint8_t tmpLen;
	uint8_t tmpType;
	uint16_t uuid;

	len = info->length; // only use length of data

	while (len > 0) {
		tmpLen = p[0];
		tmpType = p[1];

		if (tmpType == 0x16) {
			uuid = p[3];
			uuid <<= 8;
			uuid += p[2];

			pValidEvtHandler(&info->bdaddr, uuid, &p[4], tmpLen-3);
		}

		p += tmpLen+1;
		len -= tmpLen+1;
	}
}


void bleParser_init(const bdaddr_t *_pMACFilterTable, const uint16_t _MACFilterTableLen, bleParser_advServiceEvtHandler_t pEvtHandler)
{
	pMACFilterTable = (bdaddr_t *)_pMACFilterTable;
	MACFilterTableLen = _MACFilterTableLen;
	pValidEvtHandler = pEvtHandler;
}

void bleParser_advEvtHandler(le_advertising_info* info, int len)
{
	if (isValidAddr(&info->bdaddr) && pValidEvtHandler)
		parseAdvData(info, len);
}

float bleParser_decodeTemperatureService(uint8_t* pData)
{
	uint32_t mantissa = pData[2];
	mantissa <<= 16;
	mantissa += pData[1];
	mantissa <<= 8;
	mantissa += pData[0];

	return pow10((int8_t)pData[3]) * mantissa;
}

uint8_t bleParser_decodeBatteryService(uint8_t* pData)
{
	return *pData;
}

uint16_t bleParser_getMACFiltertable(bdaddr_t** pRetMACFiltertable)
{
	*pRetMACFiltertable = pMACFilterTable;
	return MACFilterTableLen;
}



