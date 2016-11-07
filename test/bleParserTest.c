/*
 * bleParserTest.c
 *
 *  Created on: Nov 6, 2016
 *      Author: sylvain
 */

/* sample data packet
 *
 * ----------------------
C5:31:98:64:48:B0
  infos:
    len : 37 (26)
    type : 3
    data : 02 01 04 09 09 30 42 38 34 31 33 35 43 07 16 09 18 8f 07 00 fe 04 16 0f 18 1f

    02 : length
    	01 : type = flags
    	04 : value
    09 : length
    	09 : type = local name
     	30 42 38 34 31 33 35 43 : data
    07 : length
    	16 : type = service data 16bits UUID
    	09 18 8f 07 00 fe : data
    		0x1809 : thermometer temp = 0x00078f * 10^0xfe = 1935 * 10^-2 = 19.35
    04 : length
    	16 : type = service data 16bits UUID
    	0f 18 1f : data
    		0x180f = battery service
    		0x1f = 31%

 *
 *
 */

/** todo list
 *
 * MAC filter
 * 	pass a constant list at init
 *
 * service parser
 *   pass a table at init with service UUIDs and callbacks
 *
 * service decoders
 *   set of decoder functions to handle specific UUIDs
 *
 */

#include <stdbool.h>
#include "unity_fixture.h"
#include "bleParser.h"

TEST_GROUP(bleParser);

TEST_GROUP_RUNNER(bleParser)
{
    RUN_TEST_CASE(bleParser, Init);
    RUN_TEST_CASE(bleParser, advEvent_goodAddress);
    RUN_TEST_CASE(bleParser, advEvent_otherAddress);
    RUN_TEST_CASE(bleParser, decodeTemperatureService);
    RUN_TEST_CASE(bleParser, decodeBatteryService);

}

bdaddr_t *pTestAddr[3];
void* pTestData[3];
uint16_t testUuid[3];
int testLen[3];
int evtCount=0;

void validEvtHandler_test(bdaddr_t* addr, uint16_t uuid, void* pData, int len)
{
	pTestAddr[evtCount] = addr;
	pTestData[evtCount] = pData;
	testLen[evtCount] = len;
	testUuid[evtCount] = uuid;
	evtCount++;
}

void check_validEvtHandler_test(int evtID, bdaddr_t* addr, uint16_t uuid, void* pData, int len)
{
	TEST_ASSERT_EQUAL_PTR(addr, pTestAddr[evtID]);
	TEST_ASSERT_EQUAL_PTR(pData, pTestData[evtID]);
	TEST_ASSERT_EQUAL_INT(len, testLen[evtID]);
	TEST_ASSERT_EQUAL_UINT16(uuid, testUuid[evtID]);
}



TEST_SETUP(bleParser)
{
	validEvtHandler_test(NULL, 0, NULL, 0);
	evtCount = 0;
}

TEST_TEAR_DOWN(bleParser)
{

}

TEST(bleParser, Init)
{
	bdaddr_t dummytable[] = {
		{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}},
	};

	bdaddr_t *pTest;
	uint16_t lenTest;

	bleParser_init(dummytable, 1, validEvtHandler_test);

	lenTest = bleParser_getMACFiltertable(&pTest);

	TEST_ASSERT_EQUAL_PTR(dummytable, pTest);
	TEST_ASSERT_EQUAL_UINT16(1, lenTest);
}


TEST(bleParser, advEvent_goodAddress)
{
	bdaddr_t dummytable[] = {
		{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}},
	};

	/*37 (26)
	    type : 3
	    data : 02 01 04 09 09 30 42 38 34 31 33 35 43 07 16 09 18 8f 07 00 fe 04 16 0f 18 1f
	    */
/*	le_advertising_info info = {
		0x00,
		0x00,
		{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}},
		26,
		{0x02, 0x01, 0x04, 0x09, 0x09, 0x30, 0x42, 0x38, 0x34, 0x31, 0x33, 0x35, 0x43, 0x07, 0x16, 0x09, 0x18, 0x8f, 0x07, 0x00, 0xfe, 0x04, 0x16, 0x0f, 0x18, 0x1f}
	};
*/
	 uint8_t info[] = {
			 0x00,
			 0x00,
			 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
			 26,
			 // data
			 0x02,0x01,0x04,
			 0x09,0x09,0x30,0x42,0x38,0x34,0x31,0x33,0x35,0x43,
			 0x07,0x16,0x09,0x18,0x8f,0x07,0x00,0xfe,
			 0x04,0x16,0x0f,0x18,0x1f
	 };

	bleParser_init(dummytable, 1, validEvtHandler_test);

	bleParser_advEvtHandler((le_advertising_info*)info, sizeof(info));

	check_validEvtHandler_test(0, (bdaddr_t*)&info[2], 0x1809, &info[26], 4);
	check_validEvtHandler_test(1, (bdaddr_t*)&info[2], 0x180f, &info[34], 1);
	TEST_ASSERT_EQUAL_INT(2, evtCount);
}

TEST(bleParser, advEvent_otherAddress)
{
	bdaddr_t dummytable[] = {
		{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}},
	};

	 uint8_t info[] = {
			 0x00,
			 0x00,
			 0x01, 0x02, 0x03, 0x04, 0x05, 0x02,
			 26,
			 0x02,0x01,0x04,0x09,0x09,0x30,0x42,0x38,0x34,0x31,0x33,0x35,0x43,0x07,0x16,0x09,0x18,0x8f,0x07,0x00,0xfe,0x04,0x16,0x0f,0x18,0x1f
	 };

	bleParser_init(dummytable, 1, validEvtHandler_test);

	bleParser_advEvtHandler((le_advertising_info*)info, sizeof(info));

	TEST_ASSERT_EQUAL_INT(0, evtCount);
}

TEST(bleParser, decodeTemperatureService)
{
	uint8_t data1[] = {0x8f,0x07,0x00,0xfe}; // 0x00078f * 10^0xfe = 19.35
	uint8_t data2[] = {0xd3,0x07,0x00,0xfd}; // 0x0007d3 * 10^0xfd = 2.003
	uint8_t data3[] = {0x2a,0x06,0x00,0xff}; // 0x00062a * 10^0xff = 157.8

	float res1, res2, res3;

	res1 = bleParser_decodeTemperatureService(data1);
	res2 = bleParser_decodeTemperatureService(data2);
	res3 = bleParser_decodeTemperatureService(data3);

	TEST_ASSERT_EQUAL_FLOAT(19.35, res1);
	TEST_ASSERT_EQUAL_FLOAT(2.003, res2);
	TEST_ASSERT_EQUAL_FLOAT(157.8, res3);
}

TEST(bleParser, decodeBatteryService)
{
	uint8_t data1[] = {0};
	uint8_t data2[] = {15};
	uint8_t data3[] = {54};
	uint8_t data4[] = {100};

	uint8_t res1, res2, res3, res4;

	res1 = bleParser_decodeBatteryService(data1);
	res2 = bleParser_decodeBatteryService(data2);
	res3 = bleParser_decodeBatteryService(data3);
	res4 = bleParser_decodeBatteryService(data4);

	TEST_ASSERT_EQUAL_UINT8(  0, res1);
	TEST_ASSERT_EQUAL_UINT8( 15, res2);
	TEST_ASSERT_EQUAL_UINT8( 54, res3);
	TEST_ASSERT_EQUAL_UINT8(100, res4);
}
