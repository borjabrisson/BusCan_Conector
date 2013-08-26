/*
 * conectorSerial.h
 *
 *  Created on: Aug 20, 2013
 *      Author: borja
 */

#ifndef CONECTORSERIAL_H_
#define CONECTORSERIAL_H_

#include <map>
#include <iostream>
#include <string>
#include <list>
#include <set>
using namespace std;

#include <stdio.h>   /* Standard input/output definitions. */
#include <string.h>  /* String function definitions. */
#include <unistd.h>  /* UNIX standard function definitions. */
#include <fcntl.h>   /* File control definitions. */
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <poll.h>

#include <termios.h> /* POSIX terminal control definitions. */

#ifdef ENABLE_SERIAL_PORT_EVENT
#include <pthread.h>
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef BOOL
#define BOOL  int
#endif

#define INVALID_HANDLE_VALUE -1
#define NONE      0
#define RTSCTS    1
#define HARD      1
#define XONXOFF   2
#define SOFT      2

typedef struct termios DCB;
typedef int HANDLE;

class conectorSerial {
protected:
	HANDLE fd;
	BOOL ERROR_CONFIGURE_PORT;
public:
	conectorSerial();
	virtual ~conectorSerial();

	int Kbhit_Port();
	HANDLE Open_Port(string COMx);

	DCB Get_Configure_Port();
	DCB Configure_Port(unsigned int BaudRate, char CharParity[]);
	int Set_Configure_Port(DCB newtio);

	long Write_Port(string Data);
	long Read_Port(string &Data, int SizeData);
	long Gets_Port(string &Data, int SizeData=-1);
	long Getc_Port(char *Data);
	int Close_Port();

	int Set_Hands_Haking(int FlowControl);
	int Set_RThreshold( int n);
	int Set_BaudRate( unsigned int BaudRate);
	int Set_Time(unsigned int Time); //t =Time*0.1 s);

	int IO_Blocking(int Modo);
	int Clean_Buffer();
	int Setup_Buffer( unsigned long InQueue, unsigned long OutQueue);

// Handle notify thread
	virtual void SERIAL_PORT_EVENT();
	void Notify_Event();
	static void *Handle_Thread(void *hPort);
	pthread_t Create_Thread_Port();
	
	int WaitForBlock(int time=-1);
	
	
	HANDLE getFD();
	void setFD(HANDLE fd);
};

#endif /* CONECTORSERIAL_H_ */
