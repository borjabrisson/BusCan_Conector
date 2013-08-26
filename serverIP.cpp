/*
 * serverIP.cpp
 *
 *  Created on: Aug 13, 2013
 *      Author: borja
 */

#include "serverIP.h"
#include <errno.h>

serverIP::serverIP() {
	// TODO Auto-generated constructor stub
	this->PORT = PORT_TCP;
	this->runState = true;
	this->HOST  = "localhost";
	this->sockfd = -1;

}

// serverIP::serverIP(void*&){
// 	
// }

serverIP::serverIP(const serverIP &obj){
	this->PORT = obj.PORT;
	this->runState = obj.runState;
	this->HOST  = obj.HOST;
	this->sockfd = obj.sockfd;
}

serverIP::~serverIP() {
	// TODO Auto-generated destructor stub
}

void serverIP::connect() {
//     int newsockfd, clilen;
//     char buffer[256];
//     int  n;

	/* First call to socket() function */
	this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}
	/* Initialize socket structure */
	bzero((char *) &this->serv_addr, sizeof(this->serv_addr));

	this->serv_addr.sin_family = AF_INET;
	this->serv_addr.sin_addr.s_addr = INADDR_ANY;
	this->serv_addr.sin_port = htons(this->PORT);

	/* Now bind the host address using bind() call.*/
	if (bind(this->sockfd, (struct sockaddr *) &this->serv_addr,
			sizeof(this->serv_addr)) < 0) {
		cout<<"ERROR on binding: "<< strerror( errno )<< endl;
		exit(1);
	}
	cout << "Fin del connect" << endl;
}

void serverIP::desconnect() {
// 	close(this->sockfd);
	shutdown(this->sockfd,2);
}
void serverIP::lunchHandle(int newsockfd) {
	this->conector.run(newsockfd);
}

void* serverIP::handleRun(void *pp) {
	cout << "hundleRun" << endl;
// 	serverIP obj = (serverIP ) pp;
// 	obj->listener();
	((serverIP *) pp)->listener();
	cout << "fin handleRun" << endl;
}

void serverIP::listener() {
	int newsockfd, clilen;
	struct sockaddr_in cli_addr;
	listen(this->sockfd, 5);
	clilen = sizeof(cli_addr);
	int i = 1;
	while (this->runState) {
		newsockfd = accept(this->sockfd, (struct sockaddr *) &cli_addr,&clilen);
		if (newsockfd < 0) {
			perror("ERROR on accept");
			exit(1);
		}
		printf("%d tiene el %d\n", i, newsockfd);
		this->lunchHandle(newsockfd);
		i++;
		cout << "AAAAA" << endl;
	}
	pthread_exit(NULL);
}
/*			Public functions		*/

void serverIP::start(){
	this->connect();
	this->run();
}

void serverIP::run() {
	cout << "inicio run" << endl;
	int rc;
	pthread_t thread;
	rc = pthread_create(&thread, NULL, &serverIP::handleRun, (void *)this);
	if (rc) {
		cout << "Error:unable to create thread," << rc << endl;
		exit(-1);
	}
	cout << "fin run" << endl;
}

void serverIP::free() {
	
	this->runState = false;
	
	this->desconnect();
	pthread_exit(NULL);
}

