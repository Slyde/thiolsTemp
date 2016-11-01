#include "unity_fixture.h"
#include "bleScanner.h"

#include "mocks/Mockhci_lib.h"

TEST_GROUP(bleScanner);

TEST_GROUP_RUNNER(bleScanner)
{
    RUN_TEST_CASE(bleScanner, Init);
    RUN_TEST_CASE(bleScanner, InitNoHCIDevice);
    RUN_TEST_CASE(bleScanner, InitCheckDeviceId0);
    RUN_TEST_CASE(bleScanner, InitCheckDeviceId1);
    RUN_TEST_CASE(bleScanner, InitCheckDeviceIdError);

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

TEST(bleScanner, Init)
{
	int ret;

	hci_get_route_ExpectAndReturn(NULL, 0);

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

TEST(bleScanner, InitCheckDeviceId0)
{
	int ret;
	int deviceID;

	// first test ID 0
	hci_get_route_ExpectAndReturn(NULL, 0);
	ret = bleScanner_init();
	deviceID = bleScanner_getDeviceID();
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_INT(0, deviceID);
}

TEST(bleScanner, InitCheckDeviceId1)
{
	int ret;
	int deviceID;

	// then test ID 1
	hci_get_route_ExpectAndReturn(NULL, 1);
	ret = bleScanner_init();
	deviceID = bleScanner_getDeviceID();
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_INT(1, deviceID);
}

TEST(bleScanner, InitCheckDeviceIdError)
{
	int ret;
	int deviceID;

	// then test error
	hci_get_route_ExpectAndReturn(NULL, -10);
	ret = bleScanner_init();
	deviceID = bleScanner_getDeviceID();
	TEST_ASSERT_EQUAL_INT(-1, ret);
	TEST_ASSERT_EQUAL_INT(-1, deviceID);
}

