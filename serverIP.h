/*
 * serverIP.h
 *
 *  Created on: Aug 13, 2013
 *      Author: borja
 */

#ifndef SERVERIP_H_
#define SERVERIP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <map>
#include <iostream>
#include <string>
#include <list>
#include <set>

#include "configuration/configuration.h"
#include "listen/requestHandler.h"

using namespace std;

class serverIP {
protected:
	bool runState;
	string HOST;
	int PORT;
	int sockfd;
	struct sockaddr_in serv_addr;
	requestHandler conector;

protected: // functions

	void connect();
	void desconnect();
	void lunchHandle(int newsockfd);

	void listener();
	static void* handleRun(void * pp);
	
public:
	serverIP();
	serverIP(void*&);
	// Constructor copia:
	serverIP(const serverIP &obj);
	~serverIP();

	void run();
	void free();
	
	void start();

	void setBusCanFD(int fd){
		conector.setBusCanFD(fd);
	}
	bool isRun() const {
		return this->runState;
	}
};

#endif /* SERVERIP_H_ */
