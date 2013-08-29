#include "serverIP.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "listen/requestHandler.h"
#include "configuration/configuration.h"

#include <sys/types.h> 

#include "BusCan/conectorSerial.h"
#include "BusCan/conectorBusCan.h"
#include "BusCan/apiBusCan.h"

#include <iostream>

#include <map>
#include <iostream>
#include <string>
#include <list>
#include <set>
using namespace std;
#define ENABLE_SERIAL_PORT_EVENT

apiBusCan conector;
serverIP server;
void pruebaAlarma() {
	conector.exec("Reset", "02", "2");
	for (int i = 0; i < 10; i++) {
		conector.exec("ActiveLed/Buzzer", "02", "0202");
		sleep(1);
	}
}

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
	//apiBusCan conector;
	//serverIP server;
	string cmd, node, args;

	conector.Open();
	// 	conector.Open("/dev/ttyS0");

	server.setBusCanFD(conector.getFD());
	conector.launchListener();
	server.start();
	conector.start();

	//pruebaCMDList();
	//pruebaAlarma();

	list<field_type> record;

	list<field_type>::iterator it;
	for (it = record.begin(); it != record.end(); it++) {
		cout << (*it)["Tables_in_amupark"]<< endl;
	}

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
	//  Set_Configure_Port(fd,OldConf);     // Restituyo la antigua configuración del puerto.
	server.free();
	conector.Close(); // Cierro el puerto serie.

	printf("\nHasta la proxima\n");
	return 0;
}
