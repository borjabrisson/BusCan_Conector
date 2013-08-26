/*
 * conectorBusCan.cpp
 *
 *  Created on: Aug 20, 2013
 *      Author: borja
 */

#include "conectorBusCan.h"

conectorBusCan::conectorBusCan() {
	// TODO Auto-generated constructor stub
	setCommand();
	setNameCommand();
	this->listener=false;
	this->temp =0;
}

string conectorBusCan::partialHex(int dec) {
	string numeroHex = "";
	stringstream stream;
	if (dec < 16) {
		switch (dec) {
		case 10:
			numeroHex = "A";
			break;
		case 11:
			numeroHex = "B";
			break;
		case 12:
			numeroHex = "C";
			break;
		case 13:
			numeroHex = "D";
			break;
		case 14:
			numeroHex = "E";
			break;
		case 15:
			numeroHex = "F";
			break;
		default:
			stream << dec;
			numeroHex = stream.str();
			break;
		}
		return numeroHex;
	}
	return "error";
}

string conectorBusCan::HexToStr(int dec) {
	int cociente = 16, residuo = 0;
	string numeroHex = "", numeroHex1 = "";

	while (dec >= 16) {
		cociente = dec / 16;
		residuo = dec % 16;
		dec = cociente;
		numeroHex1 = partialHex(residuo);
		numeroHex = numeroHex1 + numeroHex;
		dec = cociente;
	}
	numeroHex1 = partialHex(dec);
	numeroHex = numeroHex1 + numeroHex;
	if (numeroHex.length() == 1)
		numeroHex = "0" + numeroHex;
	return numeroHex;
}

conectorBusCan::~conectorBusCan() {
	// TODO Auto-generated destructor stub
}

void conectorBusCan::setCommand() {
	commandsID.insert(pair<string, string>("00", "TestCtrLink"));
	commandsID.insert(pair<string, string>("01", "Reset"));
	commandsID.insert(pair<string, string>("02", "GetFirmware"));
	commandsID.insert(pair<string, string>("51", "CloseRelay"));
	commandsID.insert(pair<string, string>("52", "SwitchRelay"));
	commandsID.insert(pair<string, string>("0B", "WrDisplay"));
	commandsID.insert(pair<string, string>("0C", "WrInmDisplay"));
	commandsID.insert(pair<string, string>("0A", "ClrDisplay"));
	commandsID.insert(pair<string, string>("0D", "SaveAndRestoreDisplay"));
	commandsID.insert(pair<string, string>("5A", "ActiveLed/Buzzer"));
	commandsID.insert(pair<string, string>("5B", "SwitchLed"));
	commandsID.insert(pair<string, string>("64", "OutputPort"));
	commandsID.insert(pair<string, string>("59", "EjectCard"));
	commandsID.insert(pair<string, string>("60", "CaptureCard"));
	commandsID.insert(pair<string, string>("42", "SetCFG"));
	commandsID.insert(pair<string, string>("43", "GetCFG"));
	commandsID.insert(pair<string, string>("53", "TxDigitalInput"));
}

void conectorBusCan::setNameCommand() {
	map<string, string>::iterator it;
	for (it = commandsID.begin(); it != commandsID.end(); it++) {
		commandsName[(*it).second] = (*it).first;
	}
	commandsName.insert(pair<string, string>("TestNodeLink", "00"));
	commandsName.insert(pair<string, string>("SetupNCanMax", "01"));
}

string conectorBusCan::calculateCRC(string command) {
	int size = command.size(), amount = 0;
	for (int i = 0; i < size; i++) {
		amount += (int) command[i];
	}
	amount = amount % 256;
	return this->HexToStr(amount);
}

int conectorBusCan::StrToInt(string id) {
	if (id[0] > '9')
		id[0] -= 7;
	if (id[1] > '9')
		id[1] -= 7;
	return ((id[0] - 48) * 16) + (id[1] - 48);
}

bool conectorBusCan::buildEmptyCmd(string cmd, string node) {
	string msg = node + cmd + "00";
	msg += this->calculateCRC(msg);
	return this->sendMessage(msg);
}

bool conectorBusCan::buildArgsCmd(string cmd, string node, string args,int minArgs, int maxArgs) {
	string msg = node + cmd;
	int size = args.size() / 2;
	if ((size >= minArgs) && (size <= maxArgs)) {
		stringstream stream;
		stream << size;
		msg += "0"+stream.str(); //+(string)(char)(48+size);
		msg += args + this->calculateCRC(msg + args);
		return this->sendMessage(msg);
	}
	return false;
}

bool conectorBusCan::sendMessage(string msg) {
	string out = '\x02' + msg + '\x03';
	cout << "Enviamos al bus: " << out << endl;
	
	this->conector.Write_Port(out);
	return true;
}

bool conectorBusCan::exec(string command, string node, string args) {
	string id = commandsName[command];
	switch (StrToInt(id)) {
	case cm_TestLink:
		if (command == "TestCtrLink")	node = "00";
		this->buildEmptyCmd(id, node);
		break;
	case cm_Reset:
		this->buildEmptyCmd(id, node);
		break;
	case cm_GetFirmware:
		this->buildEmptyCmd(id, node);
		break;
	case cm_CloseRelay:
		this->buildArgsCmd(id, node, args, 2, 2);
		break;
	case cm_SwitchRelay:
		this->buildArgsCmd(id, node, args, 2, 2);
		break;
	case cm_WrDisplay:
		this->buildArgsCmd(id, node, args, 1, 6);
		break;
	case cm_WrInmDisplay:
		this->buildArgsCmd(id, node, args, 1, 6);
		break;
	case cm_ClrDisplay:
		this->buildEmptyCmd(id, node);
		break;
	case cm_SaveAndRestoreDisplay:
		this->buildArgsCmd(id, node, args, 1, 1);
		break;
	case cm_ActivateLed:
		this->buildArgsCmd(id, node, args, 2, 2);
		break;
	case cm_SwitchLed:
		this->buildArgsCmd(id, node, args, 2, 2);
		break;
	case cm_OutputPort:
		this->buildArgsCmd(id, node, args, 0, 5);
		break;
	case cm_EjectCard:
		this->buildEmptyCmd(id, node);
		break;
	case cm_CaptureCard:
		this->buildArgsCmd(id, node, args, 6, 6);
		break;
	case cm_SetCFG:
		this->buildArgsCmd(id, node, args, 6, 6);
		break;
	case cm_GetCFG:
		this->buildEmptyCmd(id, node);
		break;
	case cm_TxDigitalInput:
		this->buildEmptyCmd(id, node);
		break;
	}
	return false;
}

void conectorBusCan::Open(string port){
	this->conector.Open_Port(port);
	this->conector.Configure_Port(B9600,"8N1"); 
}
	
void conectorBusCan::runListener(){
	string input;
	sendResponse block;
	while(this->listener){
		if (this->conector.WaitForBlock() != 0){
			this->conector.Gets_Port(input);
			if (!checkCRC(input))continue; // Se tiene que decidir que hacer. Esperamos a la siguiente, o hacemos algo diferente.¿?
			block.obj= this;			block.msg = input;
			pthread_create(new pthread_t, NULL, &conectorBusCan::Thread_HandleResponse,  (void *)&block);
			input ="";
		}
	}
}

void *conectorBusCan::Handle_Thread(void *thread) {
	((conectorBusCan *) thread)->runListener();
	return NULL;
}

void *conectorBusCan::Thread_HandleResponse(void *response){
	sendResponse *msg = response;
	msg->obj->responseAction(msg->msg);
	return NULL;
}

void conectorBusCan::launchListener(){
	if (!this->listener){
		this->listener=true;
		cout << "entramos en Create_Thread_Port" << endl;
		pthread_create(new pthread_t, NULL, &conectorBusCan::Handle_Thread,  (void *)this);
	}
}

void conectorBusCan::Close(){
	this->conector.Close_Port();
}

string conectorBusCan::getCMD(string msg){
	 return msg.substr(3,2);
}
string conectorBusCan::getNode(string msg){
	return msg.substr(1,2);
}
string conectorBusCan::getArgs(string msg){
	if (msg.length()> 7)return msg.substr(7,(msg.length()-10));
	return "";
}

bool conectorBusCan::checkCRC(string msg){
	string crc, msgCRC = msg.substr(msg.length()-3,2);
	crc = msg.substr(1,msg.length()-4);
	crc = this->calculateCRC(crc);
	return(crc == msgCRC);
}

void conectorBusCan::responseAction(string msg){
	string cmd,node,args;
	cmd = this->getCMD(msg);	node = this->getNode(msg);		args = this->getArgs(msg);
	
	switch (StrToInt(cmd)){
		case cm_OnFcnKey:
				cout << "Tecla respondida: "<< (char)StrToInt(args)<<endl;
				exec("ClrDisplay",node);
				exec("WrInmDisplay",node,"00"+args);
			break;
		case cm_OnDigitalInput:
			break;
		case cm_OnTrack:
				cout << "Track yeee"<< endl;
				exec("ClrDisplay",node);
				exec("WrInmDisplay",node,"003031323334");
			break;
		case res_TestLink : case res_Reset : case res_GetFirmware : case res_SaveAndRestoreDisplay : 
		case res_OutputPort : case res_GetCFG :
				cout << "Se ha recibido una respuesta a un evento previo "+cmd << endl;
				cout << msg << endl;
			break;
	}
	
}


string conectorBusCan::StrToDbHex(string text){
	string out="";
	for(unsigned int i =0;i< text.size();i++){
		out += this->HexToStr(text[i]);
	}
	cout << "StrToDbHex valor de entrada:: "<<text<<"  valor devuelto"<< out <<endl;
	return out;
}

// CFG previa: 02 B2 06 F0E00A0A6300 D2

// A poner CFG: C08014146300

