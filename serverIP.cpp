/*
 * serverIP.cpp
 *
 *  Created on: Aug 13, 2013
 *      Author: borja
 */

#include "serverIP.h"
#include <errno.h>

/*			Public functions		*/

serverIP::serverIP() {
	// TODO Auto-generated constructor stub
	this->PORT = PORT_TCP;
	this->runState = true;
	this->HOST  = "localhost";
	this->sockfd = -1;

}

serverIP::serverIP(const serverIP &obj){
	this->PORT = obj.PORT;
	this->runState = obj.runState;
	this->HOST  = obj.HOST;
	this->sockfd = obj.sockfd;
}

serverIP::~serverIP() {
	// TODO Auto-generated destructor stub
}

void serverIP::start(){
	this->connect();
	this->run();
}

void serverIP::run() {
	push_Debug("serverIP::run. Inicio");
	int rc;
	pthread_t thread;
	rc = pthread_create(&thread, NULL, &serverIP::handleRun, (void *)this);
	if (rc) {
		cout << "Error:unable to create thread," << rc << endl;
		exit(-1);
	}
	push_Debug("serverIP::run Fin");
}

void serverIP::free() {
	this->runState = false;
	this->desconnect();
	pthread_exit(NULL);
}

/*							Protected functions								*/

void serverIP::connect() {
	/* Creamos el objeto socket y lo asociamos a un descriptor de fichero del proceso. */
	this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}
	/* Inicializamos la estructura del socket */
	bzero((char *) &this->serv_addr, sizeof(this->serv_addr));

	this->serv_addr.sin_family = AF_INET;
	this->serv_addr.sin_addr.s_addr = INADDR_ANY;
	this->serv_addr.sin_port = htons(this->PORT);

	/* enlazamos el socketo a la dirección indicada utilizando la estructura previa.*/
	if (bind(this->sockfd, (struct sockaddr *) &this->serv_addr,
			sizeof(this->serv_addr)) < 0) {
		cout<<"ERROR on binding: "<< strerror( errno )<< endl;
		exit(1);
	}
	push_Debug("Fin del connect.serverIP");
}

void serverIP::desconnect() {
	shutdown(this->sockfd,SHUT_RDWR); // Cerramos toda conexión de envío o recibimiento de datos.
	//http://linux.die.net/man/3/shutdown
}

void serverIP::lunchHandle(int newsockfd) {
	this->conector.run(newsockfd);
}

void* serverIP::handleRun(void *pp) {
	push_Debug("serverIP::handleRun Inicio");
	((serverIP *) pp)->listener();
	push_Debug("serverIP::handleRun FIN");
	return NULL;
}

void serverIP::listener() {
	int newsockfd, clilen,numRequest = 1;
	struct sockaddr_in cli_addr;
	listen(this->sockfd, 5); //http://linux.die.net/man/2/listen. Por lo que entiendo, el 5 es el número de conexión permitidas como máximo
	clilen = sizeof(cli_addr);

	while (this->runState) {
		newsockfd = accept(this->sockfd, (struct sockaddr *) &cli_addr,&clilen); // Esperamos hasta que se establesca la siguiente conexión.
																				 // Obtenemos el descriptor de esta conexión
		if (newsockfd < 0) {
			perror("ERROR on accept");
			exit(1);
		}
		printf("%d tiene el %d\n", numRequest, newsockfd);
		this->lunchHandle(newsockfd); // Lanzamos un hijo manejador indicandole el descriptor a utilizar
		numRequest++;
	}
	pthread_exit(NULL);
}

