/*
 * bleScanner.h
 *
 *  Created on: Nov 1, 2016
 *      Author: sylvain
 */

#ifndef BLESCANNER_H_
#define BLESCANNER_H_

// TODO
// init stack and start scanning
// parse incoming events and call the event callback with pre-parsed datas

int bleScanner_init(void);
int bleScanner_startScan(void);

// read incoming msgs
// call a callback if valid msg is receive
// callback must receive a parameter of type struct le_advertising_info
// int bleScanner_parseMsg(void);
// thread and loop must be declared in application
// warning... set socket as non blocking !

// implement close connection functions


int bleScanner_getDeviceID(void);
int bleScanner_getDeviceHandle(void);

#endif /* BLESCANNER_H_ */
