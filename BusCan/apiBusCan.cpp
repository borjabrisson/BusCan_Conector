/*
 * apiBusCan.cpp
 *
 *  Created on: Aug 20, 2013
 *      Author: borja
 */

#include "apiBusCan.h"

apiBusCan::apiBusCan() {
	this->TestMode=1;
	this->setApiCmd();
}

apiBusCan::~apiBusCan() {
	// TODO Auto-generated destructor stub
}
void apiBusCan::execCmdList(list<itemCmdList> cmd){
	list<itemCmdList>::iterator it;
	for(it=cmd.begin();it!= cmd.end();it++){
		exec((*it)["command"],(*it)["node"],(*it)["args"]);
	}
}

bool apiBusCan::exec(string command, string node, string args) {
	string id = commandsName[command];
	switch (StrToInt(id)) {
	case api_reset:
		return conectorBusCan::exec("Reset", node);
	case api_Test:
		return conectorBusCan::exec("TestNodeLink", node);
	case api_SetText:
		setText(args, node);
		break;
	case api_ClearText:
		return conectorBusCan::exec("ClrDisplay", node);
		break;
	case api_SendMessage:
		sendMsg(args,node);
		break;
	case api_Scan:{
		int comma = args.find(",");
		int init=1, max=256;
		if (comma != -1){
			init = atoi(args.substr(0,comma).c_str());
			max = atoi(args.substr(comma+1).c_str());
		}
		for(int i=init;i<max;i++){
			conectorBusCan::exec("Reset",this->HexToStr(i));
		}
		break;
	}
	default:
		cout << "Entra por el default de apiBusCan::exec" << endl;
		return conectorBusCan::exec(command, node, args);
		break;
	}
	return false;
}

void apiBusCan::setApiCmd() {
	map<string, string>::iterator it;
	map<string, string> cmd;
	cmd.insert(pair<string, string>("90", "setText"));
	cmd.insert(pair<string, string>("91", "clearText"));
	cmd.insert(pair<string, string>("92", "sendMessage"));
	cmd.insert(pair<string, string>("93", "reset"));
	cmd.insert(pair<string, string>("94", "test"));
	cmd.insert(pair<string, string>("99", "scan"));

	for (it = cmd.begin(); it != cmd.end(); it++) {
		commandsID[(*it).first] = (*it).second;
		commandsName[(*it).second] = (*it).first;
	}
}

void apiBusCan::responseAction(string msg) {
	string cmd, node, args;
	cmd = this->getCMD(msg);	node = this->getNode(msg);	args = this->getArgs(msg);

	if (this->TestMode)this->TestResponseAction(cmd,node,args);
	else this->ActiveResponseAction(cmd,node,args);
}

void apiBusCan::setText(string msg, string node) {
	writeText(msg,node);
}

void apiBusCan::sendMsg(string msg,string node){
	exec("SaveAndRestoreDisplay",node,HexToStr(25));
	writeText(msg,node);
}

void apiBusCan::writeLine(int line, string text, string node) {
	int offset = 0, size = text.length();
	string buffer = "";
	int nblock = 0;
	if (size == 0) return;
	if (line == 2) offset = 64;
	if (size > 20) {
		text.substr(0, 20);
		size = 20;
	}
	nblock = ceil(size / 5.) - 1;
	for (int i = 0; i < nblock; i++) {
		exec("WrDisplay", node,
				HexToStr(offset + (i * 5)) + StrToDbHex(text.substr(i * 5, 5)));
	}
	exec("WrInmDisplay", node,
			HexToStr(offset + (nblock * 5))
					+ StrToDbHex(text.substr(nblock * 5)));
}

void apiBusCan::writeText(string msg, string node) {
	string first, second;
	exec("ClrDisplay", node);
	if (msg.length() > 20) {
		first = msg.substr(0, 20);
		second = msg.substr(20);
	} else
		first = msg;
	writeLine(1, first, node);
	writeLine(2, second, node);
}

int apiBusCan::getFD() {
	return this->conector.getFD();
}
void apiBusCan::setFD(int fd) {
	this->conector.setFD(fd);
}

void apiBusCan::TestResponseAction(string cmd,string node,string args){
	string msg;
	char car;

	switch (StrToInt(cmd)) {
		case cm_OnFcnKey:
			exec("ClrDisplay", node);
				this->writeLine(1,"Md Tester Nodo:"+this->HexToStr(this->StrToInt(node),10),node);
			car =  StrToInt(args);
			msg = (string)"Tecla pulsada: "+ car;
			this->writeLine(2,msg,node);
			if (car == 'A') this->TestMode=0;
			break;
		case cm_OnDigitalInput:
			break;
		case cm_OnTrack:
			exec("ClrDisplay", node);
				this->writeLine(1,"Md Tester Nodo:"+this->HexToStr(this->StrToInt(node),10),node);
			cout << "valor de tarjeta "<< args<<" ## "<<translateCardCode(args)<<endl;
			if(args[0] == '*') args = args.substr(1);
			this->writeLine(2,args,node);
			break;
		case res_SaveAndRestoreDisplay:
			cout << "res_SaveAndRestoreDisplay "<< args << endl;
			break;
		case res_TestLink:	case res_Reset:	case res_GetFirmware:
		/*case res_SaveAndRestoreDisplay:*/	case res_OutputPort: case res_GetCFG:
			exec("ClrDisplay", node);
				this->writeLine(1,"Md Tester Nodo:"+this->HexToStr(this->StrToInt(node),10),node);
			cout << "Se ha recibido una respuesta a un evento previo " + cmd<< endl;
			this->writeLine(2,"Esperando Accion",node);
			break;
	}
}

void apiBusCan::ActiveResponseAction(string cmd,string node,string args){
	switch (StrToInt(cmd)) {
		case cm_OnFcnKey:
			cout << "Tecla respondida: " << (char) StrToInt(args) << endl;
			exec("ClrDisplay", node);
			exec("WrInmDisplay", node, "00" + args);
			break;
		case cm_OnDigitalInput:
			break;
		case cm_OnTrack:
			cout << "Track yeee" << endl;
			exec("ClrDisplay", node);
			exec("WrInmDisplay", node, "003031323334");
			break;
		case res_TestLink:
		case res_Reset:
		case res_GetFirmware:
		case res_SaveAndRestoreDisplay:
		case res_OutputPort:
		case res_GetCFG:
			cout << "Se ha recibido una respuesta a un evento previo " + cmd << endl;
			break;
		}
}
