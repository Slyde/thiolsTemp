/*
 * FakeSocket.c
 *
 *  Created on: Nov 1, 2016
 *      Author: sylvain
 */
#include <sys/socket.h>

static int sockopt_ret = -1;

void FakeSocket_setSockoptRetValue(int ret)
{
	sockopt_ret = ret;
}


int setsockopt (int __fd, int __level, int __optname,
		       const void *__optval, socklen_t __optlen)
{
	return sockopt_ret;
}
