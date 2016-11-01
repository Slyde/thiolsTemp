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

int bleScanner_getDeviceID(void);


#endif /* BLESCANNER_H_ */
