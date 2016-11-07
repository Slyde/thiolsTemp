/*
 * bleParser.h
 *
 *  Created on: Nov 6, 2016
 *      Author: sylvain
 */

#ifndef BLEPARSER_H_
#define BLEPARSER_H_

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

#define UUID_TEMPERATURE 0x1809
#define UUID_BATTERY     0x180f

typedef void (*bleParser_advServiceEvtHandler_t)(bdaddr_t* pAddr, uint16_t uuid, void* pData, int len);


void bleParser_init(const bdaddr_t *_pMACFilterTable, const uint16_t _MACFilterTableLen, bleParser_advServiceEvtHandler_t pEvtHandler);
void bleParser_advEvtHandler(le_advertising_info* info, int len);

float bleParser_decodeTemperatureService(uint8_t* pData);
uint8_t bleParser_decodeBatteryService(uint8_t* pData);

uint16_t bleParser_getMACFiltertable(bdaddr_t** pRetMACFiltertable);

#endif /* BLEPARSER_H_ */
