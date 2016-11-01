/*
 * bleScanner.c
 *
 *  Created on: Nov 1, 2016
 *      Author: sylvain
 */

#include <stdlib.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

static int device_id = -1;
static int device_handle = -1;

static int open_default_hci_device(void)
{
	int tmpDeviceId;
	int tmpDeviceHandle;

	tmpDeviceId = hci_get_route(NULL);

	if (tmpDeviceId < 0)
		return -1;

	tmpDeviceHandle = hci_open_dev(tmpDeviceId);

	if (tmpDeviceHandle < 0)
		return -2;

	device_id = tmpDeviceId;
	device_handle = tmpDeviceHandle;

	return 0;
}


int bleScanner_init(void)
{
	int ret;

	device_id = -1;
	device_handle = -1;

	ret = open_default_hci_device();

	return ret;
}

int bleScanner_getDeviceID(void)
{
	return device_id;
}

int bleScanner_getDeviceHandle(void)
{
	return device_handle;
}
