#include "serverIP.h"
#include "listen/requestHandler.h"
#include "configuration/configuration.h"

#include "BusCan/conectorSerial.h"
#include "BusCan/conectorBusCan.h"
#include "BusCan/apiBusCan.h"

// Es necesario para poder escuchar el Bus Serie sin bloquearse (lanza el hilo). Proviene del conector Serie usado como base.
#define ENABLE_SERIAL_PORT_EVENT

// Estos objetos se crean a nivel global para poder hacer distitnas funciones de pruebas, y así modular mejor el código.
// fíjate que con este uso ya podemos acceder a ellos sin ningún problema.

	apiBusCan conector;
	serverIP server;

// Es la función inicial que contruí para la temporización de la alarma.
void pruebaAlarma() {
	conector.exec("Reset", "02", "2");
	for (int i = 0; i < 10; i++) {
		conector.exec("ActiveLed/Buzzer", "02", "0202");
		sleep(1);
	}
}

// Prueba incial donde ejecutamos una lista de comandos.
void pruebaCMDList() {
	map<string, string> linea;
	list<itemCmdList> lista;

	linea["command"] = "setText";
	linea["node"] = "02";
	linea["args"] = "2";
	lista.push_back(linea);

	linea["command"] = "setText";
	linea["args"] = "Pruebitaaaaa";
	lista.push_back(linea);

	linea["command"] = "sendMessage";
	linea["args"] = "Borramee papii";
	lista.push_back(linea);
	conector.execCmdList(lista);
}

int main(int argc, char *argv[]) {
	string cmd, node, args;

	conector.Open(); // Por defecto se utuliza el puerto ttyUSB0
	// 	conector.Open("/dev/ttyS0");

	server.setBusCanFD(conector.getFD());

	conector.launchListener(); // Se comenzará a escuchar todas las peticiones provenientes por el bus serie.
	server.start(); // el servidor IP comenzará a escuchar todas las peticiones por el puerto (5050 por defecto)

	//conector.start();
	// Si queremos que el sistema comienze arrancado debemos quitar el comentario. si no empezará en modo Tester.
	while (true) {
		cout << "comando: ";
		cin >> cmd;
		if (cmd == "exit")
			break;
		if(cmd == "cmd"){
			conector.mysql.procedure("amupark", "OnStart()");
				conector.execCmdList(conector.mysql.getResultStatment()["commands"]);
			continue;
		}
		cout << "Nodo: ";
		cin >> node;
		cout << "ARGS: ";
		cin >> args;
		conector.exec(cmd, node, args);
	}

	server.free();    // Dejamos de atender peticiones TCP/IP
	conector.Close(); // Cierro el puerto serie.

	printf("\nHasta la proxima\n");
	return 0;
}
