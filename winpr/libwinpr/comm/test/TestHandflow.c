/**
 * WinPR: Windows Portable Runtime
 * Serial Communication API
 *
 * Copyright 2014 Hewlett-Packard Development Company, L.P.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <sys/stat.h>

#ifndef _WIN32
#include <termios.h>
#endif

#include <winpr/comm.h>
#include <winpr/crt.h>

#include "../comm.h"

static BOOL test_SerialSys(HANDLE hComm)
{
	// TMP: TODO:
	return TRUE;
}

int TestHandflow(int argc, char* argv[])
{
	struct stat statbuf = { 0 };
	BOOL result = 0;
	HANDLE hComm = NULL;

	if (stat("/dev/ttyS0", &statbuf) < 0)
	{
		fprintf(stderr, "/dev/ttyS0 not available, making the test to succeed though\n");
		return EXIT_SUCCESS;
	}

	result = DefineCommDevice("COM1", "/dev/ttyS0");
	if (!result)
	{
		fprintf(stderr, "DefineCommDevice failure: 0x%x\n", GetLastError());
		return EXIT_FAILURE;
	}

	hComm = CreateFile("COM1", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hComm == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "CreateFileA failure: 0x%x\n", GetLastError());
		return EXIT_FAILURE;
	}

	_comm_setServerSerialDriver(hComm, SerialDriverSerialSys);
	if (!test_SerialSys(hComm))
	{
		fprintf(stderr, "test_SerCxSys failure\n");
		return EXIT_FAILURE;
	}

	/* _comm_setServerSerialDriver(hComm, SerialDriverSerCxSys); */
	/* if (!test_SerCxSys(hComm)) */
	/* { */
	/* 	fprintf(stderr, "test_SerCxSys failure\n"); */
	/* 	return EXIT_FAILURE; */
	/* } */

	/* _comm_setServerSerialDriver(hComm, SerialDriverSerCx2Sys); */
	/* if (!test_SerCx2Sys(hComm)) */
	/* { */
	/* 	fprintf(stderr, "test_SerCxSys failure\n"); */
	/* 	return EXIT_FAILURE; */
	/* } */

	if (!CloseHandle(hComm))
	{
		fprintf(stderr, "CloseHandle failure, GetLastError()=%08x\n", GetLastError());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
