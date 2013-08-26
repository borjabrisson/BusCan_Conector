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

int main( int argc, char *argv[] )
{
	apiBusCan conector;
	serverIP server;
	string cmd, node,args;
	
	conector.Open();
	// 	conector.Open("/dev/ttyS0");

	server.setBusCanFD(conector.getFD());
	conector.launchListener();
	server.start();
	while(true){
		cout << "comando: ";	cin >> cmd;
		if (cmd == "exit") break;
		cout << "Nodo: ";		cin >> node;
		cout << "ARGS: ";		cin >> args;
		conector.exec(cmd,node,args);
	}
  //  Set_Configure_Port(fd,OldConf);     // Restituyo la antigua configuración del puerto.
	server.free();
	conector.Close();          // Cierro el puerto serie.
    
	printf("\nHasta la proxima\n");
	return 0;
}