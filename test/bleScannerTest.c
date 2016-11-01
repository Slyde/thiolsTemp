#include "unity_fixture.h"
#include "bleScanner.h"

#include "mocks/Mockhci_lib.h"
#include "mocks/FakeSocket.h"

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

}

TEST_SETUP(bleScanner)
{
	Mockhci_lib_Init();
}

TEST_TEAR_DOWN(bleScanner)
{
	Mockhci_lib_Verify();
	Mockhci_lib_Destroy();
}


void check_device_ID_and_handle(int device_id, int device_handle)
{
	int id = bleScanner_getDeviceID();
	int handle = bleScanner_getDeviceHandle();
	TEST_ASSERT_EQUAL_INT(device_id, id);
	TEST_ASSERT_EQUAL_INT(device_handle, handle);
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

	FakeSocket_setSockoptRetValue(0);

	ret = bleScanner_init();
	TEST_ASSERT_EQUAL_INT(0, ret);

	ret = bleScanner_startScan();
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

	ret = bleScanner_startScan();
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

	FakeSocket_setSockoptRetValue(0);

	ret = bleScanner_init();
	TEST_ASSERT_EQUAL_INT(0, ret);

	ret = bleScanner_startScan();
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

	ret = bleScanner_startScan();
	TEST_ASSERT_EQUAL_INT(-3, ret);
}

