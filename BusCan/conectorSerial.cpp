/*
 * conectorSerial.cpp
 *
 *  Created on: Aug 20, 2013
 *      Author: borja
 */

#include "conectorSerial.h"

conectorSerial::conectorSerial() {
	// TODO Auto-generated constructor stub

}

conectorSerial::~conectorSerial() {
	// TODO Auto-generated destructor stub
}

HANDLE conectorSerial::Open_Port(string COMx) {
	this->fd = open(COMx.c_str(), O_RDWR | O_NOCTTY); //| O_NDELAY);
	if (this->fd < 0) {
		printf("open_port:fd=%d: No se puede abrir %s\n", fd, COMx.c_str());
		this->fd = INVALID_HANDLE_VALUE;
		return INVALID_HANDLE_VALUE;
	}
	printf("open_port:fd=%d: Abierto puerto %s\n", fd, COMx.c_str());
	tcflush(this->fd, TCIOFLUSH);
	return this->fd;
}

DCB conectorSerial::Get_Configure_Port() {
	struct termios oldtio;
	if (tcgetattr(this->fd, &oldtio) != 0) /* almacenamos la configuración actual del puerto */
	{
		printf("Error pidiendo la configuración de puerto serie.\n");
		this->ERROR_CONFIGURE_PORT = TRUE;
		return oldtio;
	}
	this->ERROR_CONFIGURE_PORT = FALSE;
	return oldtio;
}

DCB conectorSerial::Configure_Port(unsigned int BaudRate,char CharParity[]) {
	DCB newtio;
	bzero(&newtio, sizeof(newtio)); //limpiamos struct para recibir los
									//nuevos parámetros del puerto.
	//tcflush(fd, TCIOFLUSH);

	//CLOCAL  : conexion local, sin control de modem.
	//CREAD   : activa recepcion de caracteres.
	newtio.c_cflag = CLOCAL | CREAD;

	cfsetispeed(&newtio, BaudRate);
	cfsetospeed(&newtio, BaudRate);

	if (strncmp(CharParity, "8N1", 3) == 0) //CS8     : 8n1 (8bit,no paridad,1 bit de parada)
			{
		newtio.c_cflag &= ~PARENB;
		newtio.c_cflag &= ~CSTOPB;
		newtio.c_cflag &= ~CSIZE;
		newtio.c_cflag |= CS8;
	}
	if (strncmp(CharParity, "8E1", 3) == 0) {
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		newtio.c_cflag &= ~CSTOPB;
		newtio.c_cflag &= ~CSIZE;
		newtio.c_cflag |= CS8;
	}
	if (strncmp(CharParity, "7E1", 3) == 0) {
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		newtio.c_cflag &= ~CSTOPB;
		newtio.c_cflag &= ~CSIZE;
		newtio.c_cflag |= CS7;
	}
	if (strncmp(CharParity, "7O1", 3) == 0) {
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
		newtio.c_cflag &= ~CSTOPB;
		newtio.c_cflag &= ~CSIZE;
		newtio.c_cflag |= CS7;
	}
	if (strncmp(CharParity, "7S1", 3) == 0) {
		newtio.c_cflag &= ~PARENB;
		newtio.c_cflag &= ~CSTOPB;
		newtio.c_cflag &= ~CSIZE;
		newtio.c_cflag |= CS8;
	}

	// IGNPAR  : ignora los bytes con error de paridad.
	// ICRNL   : mapea CR a NL (en otro caso una entrada CR del otro ordenador
	// no terminaría la entrada) en otro caso hace un dispositivo en bruto
	// (sin otro proceso de entrada).
	newtio.c_iflag = 0;
	//newtio.c_iflag = IGNPAR;
	//newtio.c_iflag |= ICRNL;

	//Salida en bruto.
	newtio.c_oflag = 0;

	//ICANON  : activa entrada canónica(Modo texto).
	//desactiva todas las funcionalidades del eco, y no envía señales al
	//programa llamador.
	//newtio.c_lflag = ICANON;
	newtio.c_lflag = 0;

	// inicializa todos los caracteres de control.
	// Los valores por defecto se pueden encontrar en /usr/include/termios.h,
	// y vienen dadas en los comentarios, pero no los necesitamos aquí.

	newtio.c_cc[VTIME] = 0; /* temporizador entre caracter, no usado */
	newtio.c_cc[VMIN] = 1; /* bloquea lectura hasta llegada de un caracter */

	if (tcsetattr(this->fd, TCSANOW, &newtio) != 0) {
		printf("ERROR: No se pudo poner la configuración del puerto serie\n");
		ERROR_CONFIGURE_PORT = TRUE;
		return newtio;
	}

	return newtio;
}

int conectorSerial::Set_Configure_Port(DCB newtio) {
	// ahora limpiamos el buffer de entrada y salida del modem y activamos
	// la configuración del puerto
	//tcflush(fd, TCIOFLUSH);

	if (tcsetattr(this->fd, TCSANOW, &newtio) != 0) {
		printf("ERROR: No se pudo poner configuración del puerto serie\n");
		ERROR_CONFIGURE_PORT = TRUE;
		return FALSE;
	}
	ERROR_CONFIGURE_PORT = FALSE;

	return TRUE;
}

long conectorSerial::Write_Port(string Data) {
	return write(this->fd, Data.c_str(), Data.size());
}

long conectorSerial::Read_Port(string &Data, int SizeData) {
	struct termios newtio;
	struct timeval inic, fin, temp;
	float tiempo, t;
	int bytes;
	int ESTADO;
	int TEMPO;

	if (tcgetattr(this->fd, &newtio) != 0)
		return -1;
	else {
		ESTADO = newtio.c_cc[VMIN];
		TEMPO = newtio.c_cc[VTIME];
		if ((ESTADO == 0) && (TEMPO == 0)) {
			return read(this->fd, &Data, SizeData);
		} else {
			if (TEMPO == 0) {
				do {
					ioctl(this->fd, FIONREAD, &bytes);
				} while (bytes < SizeData);
				return read(this->fd, &Data, SizeData);
			} else {
				gettimeofday(&inic, NULL);
				tiempo = newtio.c_cc[VTIME];
				do {
					gettimeofday(&fin, NULL);
					temp.tv_sec = fin.tv_sec - inic.tv_sec;
					temp.tv_usec = fin.tv_usec - inic.tv_usec;
					t = ((temp.tv_usec / 1000.0) + temp.tv_sec * 1000.0)
							/ 100.0;
				} while ((t < tiempo * SizeData) && (Kbhit_Port() < SizeData));

				if (Kbhit_Port() != 0)
					return read(this->fd, &Data, SizeData);
				else
					return 0;
			}
		}
	}
}

long conectorSerial::Gets_Port(string &Data, int SizeData) { // si la string no está limpia se concatena junto a la información existente
	struct termios newtio;
	struct timeval inic, fin, temp;
	char item;
	float tiempo, t;
	long i;
	bool infinite = false;
	if (tcgetattr(this->fd, &newtio) != 0) return -1;
	if (SizeData == -1){
		SizeData = 1;
		infinite = true;
	}
	for (i = 0; i < SizeData; i++) {
		gettimeofday(&inic, NULL);
		tiempo = (float) newtio.c_cc[VTIME];
		do {
			gettimeofday(&fin, NULL);
			temp.tv_sec = fin.tv_sec - inic.tv_sec;
			temp.tv_usec = fin.tv_usec - inic.tv_usec;
			t = ((temp.tv_usec / 1000.0) + temp.tv_sec * 1000.0) / 100.0;
		} while ((t < tiempo) && (Kbhit_Port() == 0));

		item = 0;
		if ((Kbhit_Port() != 0) || (tiempo == 0))
			read(this->fd, &item, 1);

		Data += item;
		if (((Data[i] == 13) || (Data[i] == 3) || (Data[i] == 10)|| (Data[i] == 0)) && (i != 0)) {
			i = SizeData;
			break;
		}

		if (infinite) SizeData++;
	}
	return i;
}

long conectorSerial::Getc_Port(char* Data) {
	struct termios newtio;
	struct timeval inic, fin, temp;
	float tiempo, t;

	if (tcgetattr(this->fd, &newtio) != 0)
		return -1;

	gettimeofday(&inic, NULL);
	tiempo = (float) newtio.c_cc[VTIME];
	do {
		gettimeofday(&fin, NULL);
		temp.tv_sec = fin.tv_sec - inic.tv_sec;
		temp.tv_usec = fin.tv_usec - inic.tv_usec;
		t = ((temp.tv_usec / 1000.0) + temp.tv_sec * 1000.0) / 100.0;
	} while ((t < tiempo) && (Kbhit_Port() == 0));

	if ((Kbhit_Port() != 0) || (tiempo == 0))
		return read(this->fd, Data, 1);
	else
		return 0;
}

int conectorSerial::Kbhit_Port() {
	int bytes;
	ioctl(this->fd, FIONREAD, &bytes);
	return bytes;
}

int conectorSerial::Close_Port() {
	if (this->fd != INVALID_HANDLE_VALUE) { // Close the communication port.
		// Ahora limpiamos el buffer de entrada y salida del puerto y activamos
		// la configuración del puerto.
		//tcflush(fd, TCIOFLUSH);
		if (close(this->fd) != 0) {
			printf("Error cerrando el puerto serie\n");
			return FALSE;
		} else {
			this->fd = INVALID_HANDLE_VALUE;
			return TRUE;
		}
	}
	return FALSE;
}

int conectorSerial::Set_Hands_Haking(int FlowControl) {
	struct termios newtio;
	tcgetattr(this->fd, &newtio); /* almacenamos la configuración actual del puerto */
	switch (FlowControl) {
	case 0: //NONE
	{
		newtio.c_cflag &= ~CRTSCTS;
		newtio.c_iflag &= ~(IXON | IXOFF | IXANY);
		newtio.c_cc[VSTART] = 0; /* Ctrl-q */
		newtio.c_cc[VSTOP] = 0; /* Ctrl-s */
		break;
	}
	case 1: //RTS/CTS - HARD
	{
		newtio.c_cflag |= CRTSCTS;
		newtio.c_iflag &= ~(IXON | IXOFF | IXANY);
		newtio.c_cc[VSTART] = 0; /* Ctrl-q */
		newtio.c_cc[VSTOP] = 0; /* Ctrl-s */
		break;
	}
	case 2: //XON/XOFF - SOFT
	{
		newtio.c_cflag &= ~CRTSCTS;
		newtio.c_iflag |= (IXON | IXOFF); //| IXANY);
		newtio.c_cc[VSTART] = 17; /* Ctrl-q */
		newtio.c_cc[VSTOP] = 19; /* Ctrl-s */
		break;
	}
	}
	tcsetattr(this->fd, TCSANOW, &newtio);
	return 0;
}

int conectorSerial::Set_BaudRate(unsigned int BaudRate) {
	struct termios newtio;

	if (tcgetattr(this->fd, &newtio) != 0) {
		printf("Error obteniendo configuración del puerto\n");
		return FALSE;
	}

	cfsetispeed(&newtio, BaudRate);
	cfsetospeed(&newtio, BaudRate);

	if (tcsetattr(this->fd, TCSANOW, &newtio) != 0) {
		printf("Error configurando el BaudRate\n");
		return FALSE;
	}

	return TRUE;
}

int conectorSerial::Set_Time(unsigned int Time) {
	struct termios newtio;
	/* almacenamos la configuracion actual del puerto */
	if (tcgetattr(this->fd, &newtio) != 0) {
		printf("Error obteniendo configuración time-out actual\n");
		return FALSE;
	}

	newtio.c_cc[VTIME] = Time;/*temporizador entre caracter*/
	newtio.c_cc[VMIN] = 1; /*bloquea lectura hasta llegada de un
	 caracter  */

	if (tcsetattr(this->fd, TCSANOW, &newtio) != 0) {
		printf("Error estableciendo nueva configuración time-out\n");
		return FALSE;
	}

	return TRUE;
}

int conectorSerial::IO_Blocking(int Modo) {
	struct termios newtio;
	/* almacenamos la configuracion actual del puerto */
	if (tcgetattr(this->fd, &newtio) != 0) {
		printf("Error obteniendo configuración time-out actual\n");
		return FALSE;
	}

	if (Modo == FALSE) {
		newtio.c_cc[VTIME] = 0; /* Temporizador entre caracter.*/
		newtio.c_cc[VMIN] = 0; /* No bloquea lectura hasta llegada de un caracter. */
	}
	if (Modo == TRUE) {
		newtio.c_cc[VTIME] = 0; /* Temporizador entre caracter.*/
		newtio.c_cc[VMIN] = 1; /* Bloquea lectura hasta llegada de un caracter. */
	}

	if (tcsetattr(this->fd, TCSANOW, &newtio) != 0) {
		printf(
				"Error estableciendo nueva configuración bloqueante/no-bloqueante.\n");
		return FALSE;
	}

	return TRUE;
}

int conectorSerial::Clean_Buffer() {
	if (tcflush(this->fd, TCIOFLUSH) != 0) {
		printf("Error Limpiando el Buffer  de entrada y salida.\n");
		return FALSE;
	}
	return TRUE;
}

void conectorSerial::SERIAL_PORT_EVENT() {
    string input="";
       //Getc_Port(*hPort,c);
     this->Gets_Port(input);
     printf("[%d]=%s \n",this->fd,input.c_str());
     input.clear();
}

int conectorSerial::WaitForBlock(int time){
	struct pollfd fds[1];
	int ret=-1;

	fds[0].fd = this->fd;
    fds[0].events = POLLRDNORM | POLLIN;
	ret = poll(fds, 1, time);
	if (ret < 0){
		perror("Se ha producido un error");
		exit(2);
	}
	if (ret >0){
		return 1;
	}
	return 0;
}

void conectorSerial::Notify_Event(){
	do {
		if (this->WaitForBlock() != 0)this->SERIAL_PORT_EVENT();
	} while (TRUE);
}
void *conectorSerial::Handle_Thread(void *hPort) {
	((conectorSerial *) hPort)->Notify_Event();

	return NULL;
}

pthread_t conectorSerial::Create_Thread_Port() {
	cout << "entramos en Create_Thread_Port" << endl;
	pthread_t idHilo;
	pthread_create(&idHilo, NULL, &conectorSerial::Handle_Thread,  (void *)this);
	return idHilo;
}


HANDLE conectorSerial::getFD(){
	return this->fd;
}

void conectorSerial::setFD(HANDLE fd){
	this->fd = fd;
}
