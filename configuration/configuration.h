#ifndef CONFIG_H_
#define CONFIG_H_

// Clases y tipos STD utilizados.
#include <iostream>
#include <map>
#include <string>
#include <list>
#include <set>
using namespace std;

// Clases Linux (C++) necesarias para la gestión de threads y sockets.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <cstdlib>
#include <pthread.h>


#define ERROR_OUT 
//#define DEBUG_OUT

// Funciones Inline utilizadas para gestionar los estados de debug y error sin la necesidad de modificar el codigo.

inline void push_Error(string msg){
	#ifdef ERROR_OUT
		cout << "ERROR: "<< msg << endl;
	#endif
}

inline void push_Debug(string msg){
	#ifdef DEBUG_OUT
		cout << "DEBUG: "<<msg << endl;
	#endif
}


typedef map<string,string> field_type;


/*###############################    MACROS DE CONFIGURACIÓN	#####################################*/
#define PORT_TCP 5005
#define HOST_IP "127.0.0.1"

#define SGBD_USER_DEFAULT "buscan"
#define SGBD_PASS_DEFAULT "1234"
#define SGBD_HOST_DEFAULT "192.168.35.220"
#define SGBD_DB_DEFAULT "amupark"


#endif /* CONFIG_H_ */
