#include "unity_fixture.h"
#include "bleScanner.h"
#include "errno.h"

#include "mocks/Mockhci_lib.h"
#include "mocks/FakeSocket.h"
#include "mocks/FakeIoctl.h"
#include "mocks/FakeRead.h"

TEST_GROUP(bleScanner);

TEST_GROUP_RUNNER(bleScanner)
{
    RUN_TEST_CASE(bleScanner, Init);
    RUN_TEST_CASE(bleScanner, InitNoHCIDevice);
    RUN_TEST_CASE(bleScanner, InitDeviceId0);
    RUN_TEST_CASE(bleScanner, InitDeviceId1);
    RUN_TEST_CASE(bleScanner, InitDeviceIdError);
    RUN_TEST_CASE(bleScanner, InitDeviceOpenError);
    RUN_TEST_CASE(bleScanner, StartScan);
    RUN_TEST_CASE(bleScanner, StartScanParametersError);
    RUN_TEST_CASE(bleScanner, StartScanScanEnableError);
    RUN_TEST_CASE(bleScanner, StartScanSockoptError);
    RUN_TEST_CASE(bleScanner, StartScanIoctlError);
    RUN_TEST_CASE(bleScanner, ParseMsgsNoData);
    RUN_TEST_CASE(bleScanner, ParseMsgsValidData);
    RUN_TEST_CASE(bleScanner, ParseMsgsInValidSubevent);
 //   RUN_TEST_CASE(bleScanner, ParseMsgsInvalidData);

}


void check_device_ID_and_handle(int device_id, int device_handle)
{
	int id = bleScanner_getDeviceID();
	int handle = bleScanner_getDeviceHandle();
	TEST_ASSERT_EQUAL_INT(device_id, id);
	TEST_ASSERT_EQUAL_INT(device_handle, handle);
}

static char hciEvent[HCI_MAX_FRAME_SIZE];
static int hciEventLen = 0;
void createMetaEvent(const char* addr, int len, int subEvent)
{
	char* ptr;
	evt_le_meta_event *meta;
	le_advertising_info *info;

	// char 0 is ???
	hciEvent[0] = 1;
	// char 1..2 is HCI event header
	hciEvent[1] = hciEvent[2] = 0xAB;

	ptr = hciEvent + (1 + HCI_EVENT_HDR_SIZE);

	meta = (void *) ptr;

	meta->subevent = subEvent;

	info = (le_advertising_info *) (meta->data + 1);

	str2ba(addr, &info->bdaddr);
	hciEventLen = len;
}

void destroyMetaEvent(void)
{
	memset(hciEvent, 0, sizeof(hciEvent));
}

static le_advertising_info* evtCbTest_info;
static int evtCbTest_len;
void bleScanner_evtCb_test(le_advertising_info* info, int len)
{
	evtCbTest_info = info;
	evtCbTest_len = len;
}


TEST_SETUP(bleScanner)
{
	Mockhci_lib_Init();
	FakeSocket_setSockoptRetValue(0);
	FakeIoctl_setReturn(0);
	evtCbTest_info = NULL;
	evtCbTest_len = -1;
}

TEST_TEAR_DOWN(bleScanner)
{
	Mockhci_lib_Verify();
	Mockhci_lib_Destroy();
}

TEST(bleScanner, Init)
{
	int ret;

	hci_get_route_ExpectAndReturn(NULL, 0);
	hci_open_dev_ExpectAndReturn(0, 1);

	ret = bleScanner_init();

	TEST_ASSERT_EQUAL_INT(0, ret);
}

TEST(bleScanner, InitNoHCIDevice)
{
	int ret;

	hci_get_route_ExpectAndReturn(NULL, -10);

	ret = bleScanner_init();

	TEST_ASSERT_EQUAL_INT(-1, ret);
}

TEST(bleScanner, InitDeviceId0)
{
	int ret;

	// first test ID 0
	hci_get_route_ExpectAndReturn(NULL, 0);
	hci_open_dev_ExpectAndReturn(0, 1);
	ret = bleScanner_init();
	TEST_ASSERT_EQUAL_INT(0, ret);
	check_device_ID_and_handle(0, 1);
}

TEST(bleScanner, InitDeviceId1)
{
	int ret;

	// then test ID 1
	hci_get_route_ExpectAndReturn(NULL, 1);
	hci_open_dev_ExpectAndReturn(1, 2);
	ret = bleScanner_init();
	TEST_ASSERT_EQUAL_INT(0, ret);
	check_device_ID_and_handle(1,2);
}

TEST(bleScanner, InitDeviceIdError)
{
	int ret;

	// then test error
	hci_get_route_ExpectAndReturn(NULL, -10);
	ret = bleScanner_init();
	TEST_ASSERT_EQUAL_INT(-1, ret);
	check_device_ID_and_handle(-1,-1);

}

TEST(bleScanner, InitDeviceOpenError)
{
	int ret;

	// then test error
	hci_get_route_ExpectAndReturn(NULL, 0);
	hci_open_dev_ExpectAndReturn(0, -10);
	ret = bleScanner_init();
	TEST_ASSERT_EQUAL_INT(-2, ret);
	check_device_ID_and_handle(-1,-1);
}

TEST(bleScanner, StartScan)
{
	int ret;
	int devid = 0;
	int devhandle = 1;

	hci_get_route_ExpectAndReturn(NULL, devid);
	hci_open_dev_ExpectAndReturn(devid, devhandle);


	hci_le_set_scan_parameters_ExpectAndReturn(devhandle, 0x01, htobs(0x0010),
			htobs(0x0010), 0x00, 0x00, 10000, 0);

	hci_le_set_scan_enable_ExpectAndReturn(devhandle, 0x01, 0, 10000, 0);

	ret = bleScanner_init();
	TEST_ASSERT_EQUAL_INT(0, ret);

	ret = bleScanner_startScan(NULL);
	TEST_ASSERT_EQUAL_INT(0, ret);

}

TEST(bleScanner, StartScanParametersError)
{
	int ret;
	int devid = 1;
	int devhandle = 2; // use other devid and handle

	hci_get_route_ExpectAndReturn(NULL, devid);
	hci_open_dev_ExpectAndReturn(devid, devhandle);


	hci_le_set_scan_parameters_ExpectAndReturn(devhandle, 0x01, htobs(0x0010),
			htobs(0x0010), 0x00, 0x00, 10000, -1);

	ret = bleScanner_init();
	TEST_ASSERT_EQUAL_INT(0, ret);

	ret = bleScanner_startScan(NULL);
	TEST_ASSERT_EQUAL_INT(-1, ret);
}

TEST(bleScanner, StartScanScanEnableError)
{
	int ret;
	int devid = 5;
	int devhandle = 12; // use other devid and handle

	hci_get_route_ExpectAndReturn(NULL, devid);
	hci_open_dev_ExpectAndReturn(devid, devhandle);

	hci_le_set_scan_parameters_ExpectAndReturn(devhandle, 0x01, htobs(0x0010),
			htobs(0x0010), 0x00, 0x00, 10000, 0);

	hci_le_set_scan_enable_ExpectAndReturn(devhandle, 0x01, 0, 10000, -1);

	ret = bleScanner_init();
	TEST_ASSERT_EQUAL_INT(0, ret);

	ret = bleScanner_startScan(NULL);
	TEST_ASSERT_EQUAL_INT(-2, ret);
}

TEST(bleScanner, StartScanSockoptError)
{
	int ret;
	int devid = 3;
	int devhandle = 7; // use other devid and handle

	hci_get_route_ExpectAndReturn(NULL, devid);
	hci_open_dev_ExpectAndReturn(devid, devhandle);

	hci_le_set_scan_parameters_ExpectAndReturn(devhandle, 0x01, htobs(0x0010),
			htobs(0x0010), 0x00, 0x00, 10000, 0);

	hci_le_set_scan_enable_ExpectAndReturn(devhandle, 0x01, 0, 10000, 0);

	FakeSocket_setSockoptRetValue(-1);

	ret = bleScanner_init();
	TEST_ASSERT_EQUAL_INT(0, ret);

	ret = bleScanner_startScan(NULL);
	TEST_ASSERT_EQUAL_INT(-3, ret);
}

TEST(bleScanner, StartScanIoctlError)
{
	int ret;
	int devid = 8;
	int devhandle = 12; // use other devid and handle

	hci_get_route_ExpectAndReturn(NULL, devid);
	hci_open_dev_ExpectAndReturn(devid, devhandle);

	FakeIoctl_setReturn(-1);

	ret = bleScanner_init();
	TEST_ASSERT_EQUAL_INT(-3, ret);
}

TEST(bleScanner, ParseMsgsNoData)
{
	int ret;

	FakeRead_setReturn("", 0);

	ret = bleScanner_parseMsgs();

	TEST_ASSERT_EQUAL_INT(0, ret);
}

TEST(bleScanner, ParseMsgsValidData)
{
	int ret;
	int devid = 4;
	int devhandle = 2; // use other devid and handle

	createMetaEvent("12:34:56:78:9A:BC", 25, EVT_LE_ADVERTISING_REPORT);

	hci_get_route_ExpectAndReturn(NULL, devid);
	hci_open_dev_ExpectAndReturn(devid, devhandle);


	hci_le_set_scan_parameters_ExpectAndReturn(devhandle, 0x01, htobs(0x0010),
			htobs(0x0010), 0x00, 0x00, 10000, 0);

	hci_le_set_scan_enable_ExpectAndReturn(devhandle, 0x01, 0, 10000, 0);

	FakeRead_setReturn(hciEvent, hciEventLen);
	FakeSocket_setSockoptRetValue(0);

	ret = bleScanner_init();
	TEST_ASSERT_EQUAL_INT(0, ret);

	ret = bleScanner_startScan(bleScanner_evtCb_test);
	TEST_ASSERT_EQUAL_INT(0, ret);

	ret = bleScanner_parseMsgs();
	TEST_ASSERT_EQUAL_INT(hciEventLen-1-HCI_EVENT_HDR_SIZE-EVT_LE_META_EVENT_SIZE, ret);
	TEST_ASSERT_EQUAL_INT(hciEventLen-1-HCI_EVENT_HDR_SIZE-EVT_LE_META_EVENT_SIZE, evtCbTest_len);
	TEST_ASSERT_EQUAL_MEMORY(&hciEvent[1+1+HCI_EVENT_HDR_SIZE+EVT_LE_META_EVENT_SIZE], evtCbTest_info, evtCbTest_len);
}

TEST(bleScanner, ParseMsgsInValidSubevent)
{
	int ret;
	int devid = 1;
	int devhandle = 1; // use other devid and handle

	createMetaEvent("12:34:56:78:9A:BC", 25, EVT_LE_ADVERTISING_REPORT+1);

	hci_get_route_ExpectAndReturn(NULL, devid);
	hci_open_dev_ExpectAndReturn(devid, devhandle);


	hci_le_set_scan_parameters_ExpectAndReturn(devhandle, 0x01, htobs(0x0010),
			htobs(0x0010), 0x00, 0x00, 10000, 0);

	hci_le_set_scan_enable_ExpectAndReturn(devhandle, 0x01, 0, 10000, 0);

	FakeRead_setReturn(hciEvent, hciEventLen);
	FakeSocket_setSockoptRetValue(0);

	ret = bleScanner_init();
	TEST_ASSERT_EQUAL_INT(0, ret);

	ret = bleScanner_startScan(bleScanner_evtCb_test);
	TEST_ASSERT_EQUAL_INT(0, ret);

	ret = bleScanner_parseMsgs();
	TEST_ASSERT_TRUE(ret>0);
	TEST_ASSERT_EQUAL_INT(-1, evtCbTest_len);
	TEST_ASSERT_NULL(evtCbTest_info);
}


