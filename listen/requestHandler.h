/*
 * requestHandler.h
 *
 *  Created on: Aug 12, 2013
 *      Author: borja
 */

#ifndef requestHandler_H_
#define requestHandler_H_

#include <iostream>
#include <cstdlib>
#include <pthread.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "../BusCan/apiBusCan.h"

using namespace std;

#define NUM_THREADS     5

class requestHandler {
protected:
	int sockfd;
	apiBusCan buscan;
protected:
	static void* print(void *id);
	
	static void* threadHandle(void *id);
	void listener();
public:
	requestHandler();
	virtual ~requestHandler();
	void run(int ID);
	
	void setBusCanFD(int fd){
		buscan.setFD(fd);
	}
};

#endif /* requestHandler_H_ */
