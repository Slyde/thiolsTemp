/*
 * FakeRead.c
 *
 *  Created on: Nov 2, 2016
 *      Author: sylvain
 */

#include <unistd.h>

static int ret = -1;
static char* buf = NULL;

void FakeRead_setReturn(char *_buf, int _ret)
{
	ret = _ret;
	buf = _buf;
}


ssize_t read (int __fd, void *__buf, size_t __nbytes)
{
	if (__nbytes > ret)
		__nbytes = ret;

	if (ret > 0 && __buf && buf)
		memcpy(__buf, buf, __nbytes);

	return __nbytes;
}
