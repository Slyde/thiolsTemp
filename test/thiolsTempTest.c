/*
 ============================================================================
 Name        : thiolsTempTest.c
 Author      : SD
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "unity_fixture.h"


static void runAllTests(void)
{
    RUN_TEST_GROUP(bleScanner);
}

int main(int argc, const char* argv[])
{
    return UnityMain(argc, argv, runAllTests);
}

