/*
 * FakeIoctl.c
 *
 *  Created on: Nov 2, 2016
 *      Author: sylvain
 */

static int ret = -1;

void FakeIoctl_setReturn(int _ret)
{
	ret = _ret;
}

int ioctl (int __fd, unsigned long int __request, ...)
{
	return ret;
}
