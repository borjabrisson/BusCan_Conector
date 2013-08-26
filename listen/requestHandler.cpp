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

void* requestHandler::print(void *id){
	long sock = (long)id;
    int n;
    char buffer[256];

    bzero(buffer,256);

    n = read(sock,buffer,255);
    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    }
    printf("Here is the message(%d): %s\n",sock,buffer);
    n = write(sock,"I got your message",18);
    if (n < 0) 
    {
        perror("ERROR writing to socket");
        exit(1);
    }
    close(sock);
}

void* requestHandler::threadHandle(void *obj){
	((requestHandler *) obj)->listener();
}

void requestHandler::listener(){
    int n,posSeparator,aux;
    char buffer[256];
	string cmd,node,args,msg, ErrorMsg="Error",OkMsg="OK";

    bzero(buffer,256);

    n = read(sockfd,buffer,255);
    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    }
        
	msg = buffer;
	posSeparator = msg.find("::");
	cmd = msg.substr(0,posSeparator);	
	
	aux = posSeparator;			posSeparator = msg.find("::",aux+1);

	node = msg.substr(aux+2,posSeparator-(aux+2));
	args = msg.substr(posSeparator+2);
	
	buscan.exec(cmd,node,args);
	
    printf("Here is the message(%d): %s\n",sockfd,buffer);
    n = write(sockfd,OkMsg.c_str(),OkMsg.length());
    if (n < 0) 
    {
        perror("ERROR writing to socket");
        exit(1);
    }
    close(sockfd);
	
}

void requestHandler::run(int ID){
	int rc;
	pthread_t thread;
	//rc = pthread_create(&thread, NULL,print, (void *)ID);
	this->sockfd = ID;
	rc =  pthread_create(&thread, NULL, &requestHandler::threadHandle, (void *)this);
  if (rc){
	 cout << "Error:unable to create thread," << rc << endl;
	 exit(-1);
  }
  //pthread_exit(NULL);
}
/*
void requestHandler::run(int ID)
{
   pthread_t threads[NUM_THREADS];
   int rc;
   int i;
   for( i=0; i < NUM_THREADS; i++ ){
      cout << "Hijo() : creating thread, " << i << endl;
      rc = pthread_create(&threads[i], NULL,
    		  &requestHandler::print, (void *)i);
      if (rc){
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
   }
   //pthread_exit(NULL);
}*/

