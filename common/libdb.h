

#ifndef LIB_DB_H
#define LIB_DB_H

#include <sqlite3.h> 

typdef sqlite3 DB_Handle_t;


int db_open(DB_Handle_t *pHandle);
void db_close(DB_Handle_t *pHandle);
void db_backup(bool force);

bool db_checkNodeAddr(DB_Handle_t *pHandle, char* macAddr);
int db_addPoint(DB_Handle_t *pHandle, char* macAddr, float value);


int db_getLastTemp(DB_Handle_t *pHandle, float* pTemp, time_t *pTime);
int db_getAvgTemp(DB_Handle_t *pHandle, float* pAvgTemp);

int db_getCurrentSetPoint(DB_Handle_t *pHandle, float* pTemp);



#endif

