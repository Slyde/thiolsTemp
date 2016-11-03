/*
 * bleScanner.c
 *
 *  Created on: Nov 1, 2016
 *      Author: sylvain
 */

#include "bleScanner.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

static int device_id = -1;
static int device_handle = -1;
static bleScanner_evtCb_t evtCb = NULL;

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

	int on = 1;
	if(ioctl(tmpDeviceHandle, FIONBIO, (char *)&on) < 0)
		return -3;

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

int bleScanner_startScan(bleScanner_evtCb_t _evtCb)
{
	evtCb = _evtCb;

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

int bleScanner_parseMsgs(void)
{
	static char buf[HCI_MAX_FRAME_SIZE];
	int len;
	evt_le_meta_event *meta;
	le_advertising_info *info;

	len = read(device_handle, buf, sizeof(buf));

	if (len > 0) {

		meta =(void *) ( buf + (1 + HCI_EVENT_HDR_SIZE));
		len -= (1 + HCI_EVENT_HDR_SIZE);

		if (meta->subevent == EVT_LE_ADVERTISING_REPORT) {
			len -= EVT_LE_META_EVENT_SIZE;

			/* Ignoring multiple reports */
			info = (le_advertising_info *) (meta->data + EVT_LE_META_EVENT_SIZE);

			if (evtCb)
				evtCb(info, len);
		}
	}

	return len;
}

int bleScanner_getDeviceID(void)
{
	return device_id;
}

int bleScanner_getDeviceHandle(void)
{
	return device_handle;
}
