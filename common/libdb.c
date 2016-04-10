#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include "common.h"
#include "libdb.h"
#include "time.h"



static int insert_callback(void *NotUsed, int argc, char **argv, char **azColName);
static int select_callback(void *data, int argc, char **argv, char **azColName);


int db_open(DB_Handle_t *pHandle)
{
	int err;

	// check first if DB is already opened
	if (pHandle->pDBHandle != NULL) {
		printf("DB::Error pHandle is not NULL\r\n");
		return -1;
	}

	err = sqlite3_open(DB_FILE, &pHandle->pDBHandle);

	if (err) {
		printf("DB::Error openeing database (%d)\r\n", err);
		printf("DB::DB file : %s\r\n", DB_FILE);
		printf("DB::sqlite3 error message : \r\n%s\r\n", sqlite3_errmsg(pHandle->pDBHandle));
	} else {
		printf("DB::Database opened\r\n");
	}


	// initialize semaphore
	sem_init(&pHandle->callbackMutex,0,0);
	
	return -err;
}

void db_close(DB_Handle_t *pHandle)
{
	int err;

	err = sqlite3_close(pHandle->pDBHandle);

	if (err) {
		printf("DB::Error openeing databasei (%d)\r\n", err);
		printf("DB::sqlite3 error message : \r\n%s\r\n", sqlite3_errmsg(pHandle->pDBHandle));
	} else {
		printf("DB::Database closed\r\n");
	}	
	
	return;
}

void db_backup(bool force)
{
	printf("DB:Error backup not implemented yet\r\n");
}


bool db_checkNodeAddr(DB_Handle_t *pHandle, char* macAddr)
{
	int rc;
	char *sql;
	char *zErrMsg = 0;
	bool ret = false;

	// create the request buffer
        sql = malloc(500);
        if (sql == NULL) {
                printf("DB::Error addPoint cannot allocate buffer\r\n");
		return -1;
        }

	sprintf(sql, "SELECT id FROM sensors WHERE addr='%s';", macAddr);	

	memset(&pHandle->validNodeAddr, 0, sizeof(pHandle->validNodeAddr));

	rc = sqlite3_exec(pHandle->pDBHandle, sql, select_callback, pHandle, &zErrMsg);

	if( rc != SQLITE_OK ){
                printf("DB::Error checkNodeAddr : Request : %s\r\n", sql);
                printf("DB::Error checkNodeAddr : SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);

                printf("DB::Error checkNodeAddr : sqlite error : %s\r\n", sqlite3_errmsg(pHandle->pDBHandle));
                printf("DB::Error pHandle = %p\r\n", pHandle->pDBHandle);
        } else {
		if (strlen(pHandle->validNodeAddr) > 0) {
			printf("DB::checkNodeAddr found : %s !!\r\n", pHandle->validNodeAddr);
			ret = true;
		} else {
			printf("DB::checkNodeAddr %s not found !!\r\n", macAddr);
		}
	}

	free(sql);
	
	return ret;
}

int db_addPoint(DB_Handle_t *pHandle, char* macAddr, float temperature, int battery, int rssi)
{
	int rc;
	char *sql;
	unsigned char sensor;
	char *zErrMsg = 0;
	unsigned int t;


	// create the request buffer	
	sql = malloc(500);
	if (sql == NULL) {
		printf("DB::Error addPoint cannot allocate buffer\r\n");
		return -1;
	}


	// find the sensor (todo)
	sensor = 0;

	// get the timestamp
	t = time(NULL);

	// create the request
	sprintf(sql, "INSERT INTO points (sensor, time, temperature, battery, rssi) VALUES (%u, %u, %f, %u, %d);", sensor, t, temperature, battery, rssi);

	// execute the request
	rc = sqlite3_exec(pHandle->pDBHandle, sql, insert_callback, &pHandle, &zErrMsg);

	if( rc != SQLITE_OK ){
		printf("DB::Error addPoint : Request : %s\r\n", sql);
		printf("DB::Error addPoint : SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);

		printf("DB::Error addPoint : sqlite error : %s\r\n", sqlite3_errmsg(pHandle->pDBHandle));
		printf("DB::Error pHandle = %p\r\n", pHandle->pDBHandle);
	}else{
		printf("DB::Records created successfully\n");
	}


	free(sql);
}

int db_getLastTemp(DB_Handle_t *pHandle, float* pTemp, time_t *pTime)
{

}

int db_getAvgTemp(DB_Handle_t *pHandle, float* pAvgTemp)
{

}


int db_getCurrentSetPoint(DB_Handle_t *pHandle, float* pTemp)
{

}



// callbacks

static int insert_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

static int select_callback(void *data, int argc, char **argv, char **azColName)
{
   	int i;
	DB_Handle_t *pHandle = data;

	fprintf(stderr, "%s: ", (const char*)data);
	for(i=0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");

	// check if node addr is found
	if (pHandle != NULL && argc > 0) {
		printf("Node found\r\n");
		strcpy(pHandle->validNodeAddr, argv[0]);
	}

	return 0;
}

