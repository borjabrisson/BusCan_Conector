/*
 * requestHandler.h
 *
 *  Created on: Aug 12, 2013
 *      Author: borja
 */

#ifndef requestHandler_H_
#define requestHandler_H_

#include "../BusCan/apiBusCan.h"
#include "../configuration/configuration.h"

#define NUM_THREADS     5

//#define RqtHandlerTest 1

class requestHandler {
protected:
	int sockfd;
	apiBusCan buscan;

protected:
	static void* threadHandle(void *id); // función estática que nos hace de puente para ejecutar el método de la clase deseado.
	void listener(); // se comunicará con el cliente para obtener el comando y argumentos necesarios.
	void showRead(); // Leerá los datos enviados y responderá con un mensaje. Luego mostrará por pantalla el mensaje recibido.
public:
	requestHandler();
	virtual ~requestHandler();
	void run(int socketID); // lanzamos un hijo manejador indicando el identificador del socket.
	void setBusCanFD(int fd){
		buscan.setFD(fd);
	}
};

#endif /* requestHandler_H_ */
