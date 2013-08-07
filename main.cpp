#include "conector_SGBD/sgbd_baseConector.h"

int main(){
	sgbd_baseConector conector;
	
	conector.conect();
	conector.desconect();
	return 0;
}