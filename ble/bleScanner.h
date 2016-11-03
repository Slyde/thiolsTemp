/*
 * bleScanner.h
 *
 *  Created on: Nov 1, 2016
 *      Author: sylvain
 */

#ifndef BLESCANNER_H_
#define BLESCANNER_H_

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

// TODO
// init stack and start scanning
// parse incoming events and call the event callback with pre-parsed datas

typedef void (*bleScanner_evtCb_t)(le_advertising_info* info, int len);

int bleScanner_init(void);
int bleScanner_startScan(bleScanner_evtCb_t evtCb);
int bleScanner_parseMsgs(void);

// read incoming msgs
// call a callback if valid msg is receive
// callback must receive a parameter of type struct le_advertising_info
// warning... set socket as non blocking !

// implement close connection functions


int bleScanner_getDeviceID(void);
int bleScanner_getDeviceHandle(void);

#endif /* BLESCANNER_H_ */
