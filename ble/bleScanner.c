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

int bleScanner_startScan(void)
{

	if (hci_le_set_scan_parameters(device_handle, 0x01, htobs(0x0010),
			htobs(0x0010), 0x00, 0x00, 10000) < 0)
		return -1;

	if (hci_le_set_scan_enable(device_handle, 0x01, 0, 10000) < 0)
		return -2;

	struct hci_filter new_filter;

	hci_filter_clear(&new_filter);
	hci_filter_set_ptype(HCI_EVENT_PKT, &new_filter);
	hci_filter_set_event(EVT_LE_META_EVENT, &new_filter);

	if (setsockopt(device_handle, SOL_HCI, HCI_FILTER, &new_filter, sizeof(new_filter)) < 0)
		return -3;

	return 0;
}

int bleScanner_getDeviceID(void)
{
	return device_id;
}

int bleScanner_getDeviceHandle(void)
{
	return device_handle;
}
