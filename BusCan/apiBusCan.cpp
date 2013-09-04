/*
 * apiBusCan.cpp
 *
 *  Created on: Aug 20, 2013
 *      Author: borja
 */

#include "apiBusCan.h"

apiBusCan::apiBusCan() {
	this->TestMode = 1;
	LedPeriod = 10;
	BuzzerAllowPeriod=10;
	BuzzerDenyPeriod=10;
	MessagePeriod=25;
	RelayPeriod=3;
	this->setApiCmd();
}

apiBusCan::~apiBusCan() {
	// TODO Auto-generated destructor stub
}
void apiBusCan::execCmdList(list<itemCmdList> cmd) {
	list<itemCmdList>::iterator it;
	for (it = cmd.begin(); it != cmd.end(); it++) {
		cout << (*it)["command"] << "," << HexToStr(atoi((*it)["node"].c_str()))
				<< "," << (*it)["args"] << endl;
		exec((*it)["command"], HexToStr(atoi((*it)["node"].c_str())),
				(*it)["args"]);
	}
}

void apiBusCan::scan(string args) {
	int comma = args.find(",");
	int init = 1, max = 256;
	if (comma != -1) {
		init = atoi(args.substr(0, comma).c_str());
		max = atoi(args.substr(comma + 1).c_str());
	}
	for (int i = init; i < max; i++) {
		conectorBusCan::exec("Reset", this->HexToStr(i));
	}
}

void apiBusCan::start() {
	this->TestMode = 0;
	this->runProcedure("OnStart()","01");
}
void apiBusCan::allowAccess(string node,string msg){
	this->exec("sendMessage",node,msg);
	this->exec("ActiveLed/Buzzer",node,"00"+this->HexToStr(LedPeriod));
	this->exec("ActiveLed/Buzzer",node,"02"+this->HexToStr(BuzzerAllowPeriod));
	this->exec("CloseRelay",node,"00"+this->HexToStr(RelayPeriod));

}

void apiBusCan::denyAccess(string node,string msg){
	this->exec("sendMessage",node,msg);
	this->exec("ActiveLed/Buzzer",node,"01"+this->HexToStr(LedPeriod));
	this->alarm(3,node);
	//this->exec("ActiveLed/Buzzer",node,"02"+this->HexToStr(BuzzerDenyPeriod));
}
void apiBusCan::alarm(int seconds,string node){
	for (int i = 0; i < seconds; i++) {
			exec("ActiveLed/Buzzer", node, "0202");
			sleep(1);
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
		sendMsg(args, node);
		break;
	case api_Scan: {
		this->scan(args);
		break;
	}
	case api_Start:
		this->start();
		break;
	case api_SetMode:
		if (args == "Active")	this->start();
		else
			this->TestMode=1;
		break;
	case api_ChangeHost:
		this->mysql.setHost(args);
		break;
	case api_AllowAccess:
			this->allowAccess(node,args);
		break;
	case api_DenyAccess:
		this->denyAccess(node,args);
		break;
	case api_Init:	case api_End:	case api_SetCFG:	case api_GetNodeCFG:
	case api_SetNodeCFG:
	case api_Stop:
		cout << "Funcion temporalmente no implementada: " << command << endl;
		break;
	default:
		return conectorBusCan::exec(command, node, args);
		break;
	}
	return false;
}

void apiBusCan::setApiCmd() {
	map<string, string>::iterator it;
	map<string, string> cmd;
	cmd.insert(pair<string, string>("90", "init"));
	cmd.insert(pair<string, string>("91", "end"));
	cmd.insert(pair<string, string>("92", "reset"));
	cmd.insert(pair<string, string>("93", "test"));

	cmd.insert(pair<string, string>("94", "setCFG"));
	cmd.insert(pair<string, string>("95", "getNodeCFG"));
	cmd.insert(pair<string, string>("96", "setNodeCFG"));
	cmd.insert(pair<string, string>("97", "allowAccess"));
	cmd.insert(pair<string, string>("98", "denyAccess"));
	cmd.insert(pair<string, string>("99", "setText"));

	cmd.insert(pair<string, string>("9A", "clearText"));
	cmd.insert(pair<string, string>("9B", "sendMessage"));
	cmd.insert(pair<string, string>("9C", "start"));
	cmd.insert(pair<string, string>("9D", "stop"));
	cmd.insert(pair<string, string>("9E", "scan"));
 	cmd.insert(pair<string, string>("9F", "setMode"));
 	cmd.insert(pair<string, string>("A0", "changeHost"));

	for (it = cmd.begin(); it != cmd.end(); it++) {
		commandsID[(*it).first] = (*it).second;
		commandsName[(*it).second] = (*it).first;
	}
}

void apiBusCan::responseAction(string msg) {
	string cmd, node, args;
	cmd = this->getCMD(msg);
	node = this->getNode(msg);
	args = this->getArgs(msg);

cout << "Recibimos:: "<< msg << endl;
	if (this->TestMode)
		this->TestResponseAction(cmd, node, args);
	else
		this->ActiveResponseAction(cmd, node, args);
}

void apiBusCan::setText(string msg, string node) {
	writeText(msg, node);
}

void apiBusCan::sendMsg(string msg, string node) {
	exec("SaveAndRestoreDisplay", node, HexToStr(MessagePeriod));
	writeText(msg, node);
}

void apiBusCan::writeLine(int line, string text, string node) {
	int offset = 0, size = text.length();
	string buffer = "";
	int nblock = 0;
	if (size == 0)
		return;
	if (line == 2)
		offset = 64;
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

void apiBusCan::TestResponseAction(string cmd, string node, string args) {
	string msg;
	char car;
	switch (StrToInt(cmd)) {
	case cm_OnFcnKey:
		exec("ClrDisplay", node);
		this->writeLine(1,
				"Md Tester Nodo:" + this->HexToStr(this->StrToInt(node), 10),
				node);
		car = StrToInt(args);
		msg = (string) "Tecla pulsada: " + car;
		this->writeLine(2, msg, node);
		if (car == 'A')
			this->start();
		break;
	case cm_OnDigitalInput:
		break;
	case cm_OnTrack:
		exec("ClrDisplay", node);
		exec("EjectCard",node);
		this->writeLine(1,
				"Md Tester Nodo:" + this->HexToStr(this->StrToInt(node), 10),
				node);
		cout << "valor de tarjeta " << args << " ## " << translateCardCode(args)
				<< endl;
		if (args[0] == '*')
			args = args.substr(1);
		this->writeLine(2, args, node);
		break;
	case res_SaveAndRestoreDisplay:
		cout << "res_SaveAndRestoreDisplay " << args << endl;
		break;
	case res_TestLink:
	case res_Reset:
	case res_GetFirmware:
		/*case res_SaveAndRestoreDisplay:*/
	case res_OutputPort:
	case res_GetCFG:
		exec("ClrDisplay", node);
		this->writeLine(1,
				"Md Tester Nodo:" + this->HexToStr(this->StrToInt(node), 10),
				node);
		cout << "Se ha recibido una respuesta a un evento previo " + cmd
				<< endl;
		this->writeLine(2, "Esperando Accion", node);
		break;
	}
}

void apiBusCan::ActiveResponseAction(string cmd, string node, string args) {
	switch (StrToInt(cmd)) {
	case cm_OnFcnKey: {
		char key = (char) StrToInt(args);
		node = HexToStr(StrToInt(node),10);
		string call = (string) "OnFcnKey(" + node + ",'" + key + "')";
		this->runProcedure(call,node);
	}
		break;
	case cm_OnDigitalInput:
		break;
	case cm_OnTrack: {
		if (args[0] == '*')	args = args.substr(1);

		exec("EjectCard", node);
		node = HexToStr(StrToInt(node),10);
		string call = (string) "OnCard(" + node + ",'" + args + "')";
		this->runProcedure(call,node);
	}
		break;
	/*case res_TestLink:{
		string call = (string) "OnResponde(" + "2" + ",'testCtrLink','')";
		this->runProcedure(call,node);
	}
		break;
	case res_Reset:{
		string call = (string) "OnResponde(" + "2" + ",'reset','')";
		this->runProcedure(call,node);
	}
		break;
	case res_GetFirmware:{
		string call = (string) "OnResponde(" + "2" + ",'getFirmware','"+args+"')";
		this->runProcedure(call,node);
	}
		break;
	case res_GetCFG:{
			string call = (string) "OnResponde(" + "2" + ",'getCFG','"+args+"')";
			this->runProcedure(call,node);
		}
			break;*/
case res_TestLink:
case res_Reset:
case res_GetFirmware:
case res_GetCFG:
	case res_SaveAndRestoreDisplay:
	case res_OutputPort:
		cout << "Se ha recibido una respuesta a un evento previo " + cmd
				<< endl;
		break;
	}
}

void apiBusCan::runProcedure(string clause,string node) {
	if (this->mysql.procedure(SGBD_DB_DEFAULT, clause)) {
		this->execCmdList(this->mysql.getResultStatment()["commands"]);
	} else {
		exec("sendMessage", node, "Error de conexion");
	}
}
