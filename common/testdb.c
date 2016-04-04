#include "libdb.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
	DB_Handle_t handle;

	memset(&handle, 0, sizeof(handle));

	db_open(&handle);

	db_checkNodeAddr(&handle, "B0:48:64:98:31:C5");
	db_checkNodeAddr(&handle, "B0:48:64:98:31:C2");

//	db_addPoint(&handle, "abcdef", 123.567, 12, -87);

	db_close(&handle);


	return 0;
}

