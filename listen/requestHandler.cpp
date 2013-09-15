/*
 * requestHandler.cpp
 *
 *  Created on: Aug 12, 2013
 *      Author: borja
 */

#include "requestHandler.h"

requestHandler::requestHandler() {
	// TODO Auto-generated constructor stub

}

requestHandler::~requestHandler() {
	// TODO Auto-generated destructor stub
}

void requestHandler::showRead(){
	long sock = sockfd;
    int n;
    char buffer[256];

    bzero(buffer,256);
    n = read(sock,buffer,255); // leemos el mensaje recibido.
    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    }
    printf("Here is the message(%d): %s\n",sock,buffer);
    n = write(sock,"I got your message",18); // enviamos nuestra respuesta.
    if (n < 0) 
    {
        perror("ERROR writing to socket");
        exit(1);
    }
    close(sock);
}

void* requestHandler::threadHandle(void *obj){
	// Si compilamos en modo Test se ejecutará ShowRead, si no, lanzaremos el listener para capturar los comandos
#if defined(RqtHandlerTest)
	((requestHandler *) obj)->showRead();
#else
    ((requestHandler *) obj)->listener();
#endif

}

void requestHandler::listener(){
    int n,posSeparator,aux;
    char buffer[256];
	string cmd,node,args,msg, ErrorMsg="Error",OkMsg="OK";

	bzero(buffer,256);
    n = read(sockfd,buffer,255); // Leemos el mensaje recibido.
    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    }
        
	msg = buffer;
	if (msg[msg.length()-1]== '\n'){ // limpiamos el mensaje de caracteres inválidos (\n,\r,\t)
			if (msg[msg.length()-2]== '\r') msg.erase(msg.length()-2,2);
			else msg.erase(msg.length()-1,1);
	}
	posSeparator = msg.find("::");
	// Ahora separamos el mensaje (CMD::NODE::ARGS) en sus 3 componentes.
	cmd = msg.substr(0,posSeparator);	
	
	aux = posSeparator;			posSeparator = msg.find("::",aux+1);

	node = msg.substr(aux+2,posSeparator-(aux+2));
	args = msg.substr(posSeparator+2);
	
	// Lanzamos el comando enviado.
	buscan.exec(cmd,node,args);

//  printf("Here is the message(%d): %s\n",sockfd,buffer);
    push_Debug("requestHandler::listener. Msg: "+(string)buffer);
    n = write(sockfd,OkMsg.c_str(),OkMsg.length()); //Enviamos nuestra respuesta.
    if (n < 0) 
    {
        perror("ERROR writing to socket");
        exit(1);
    }
    close(sockfd);
	
}

void requestHandler::run(int socketID){
	int rc;
	pthread_t thread;
	this->sockfd = socketID;
	rc =  pthread_create(&thread, NULL, &requestHandler::threadHandle, (void *)this);
	if (rc){
		cout << "Error:unable to create thread," << rc << endl;
		exit(-1);
	}
  //pthread_exit(NULL);
}

