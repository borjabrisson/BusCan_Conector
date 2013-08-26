/*
 * conectorBusCan.h
 *
 *  Created on: Aug 20, 2013
 *      Author: borja
 */

#ifndef CONECTORBUSCAN_H_
#define CONECTORBUSCAN_H_

#include <map>
#include <iostream>
#include <string>
#include <list>
#include <set>

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <math.h>  

using namespace std;

#include "conectorSerial.h"
#define ENABLE_SERIAL_PORT_EVENT

enum codePCCommand{
	cm_TestLink = 0,
	cm_Reset =1,
	cm_GetFirmware=2,
	cm_CloseRelay=0x51,
	cm_SwitchRelay=0x52,
	cm_WrDisplay=0x0B,
	cm_WrInmDisplay=0x0C,
	cm_ClrDisplay=0x0A,
	cm_SaveAndRestoreDisplay=0x0D,
	cm_ActivateLed=0x5A,
	cm_SwitchLed=0x5B,
	cm_OutputPort=0x64,
	cm_EjectCard=0x59,
	cm_CaptureCard=0x60,
	cm_SetCFG=0x42,
	cm_GetCFG=0x43,
	cm_TxDigitalInput=0x53
};

enum codeNodeCommand{
	cm_OnTrack=0xCB,
	cm_OnDigitalInput=0xD2,
	cm_OnFcnKey=0x8D
};

enum codeResponseCommand{ // Nos indica los valores de OPC de las respuestas de comandos previamente lanzados
	res_TestLink=0x80,
	res_Reset =0x81,
	res_GetFirmware=0x82,
	res_SaveAndRestoreDisplay=0x8E,
	res_OutputPort=0xC0,
	res_GetCFG=0xB2
};


class conectorBusCan {
private:
	typedef struct sendResponse_struct{
	conectorBusCan* obj;
	string msg;
	} sendResponse;
	
protected: // atributos
	map<string,string> commandsID;
	map<string,string> commandsName;

	conectorSerial conector;
	bool listener;
	int temp;

protected: // Ejecución de comandos.
	bool sendMessage(string msg);
	bool buildEmptyCmd(string cmd,string node);
	bool buildArgsCmd(string cmd,string node,string args,int minArgs,int maxArgs);

	void runListener();
	
	static void *Thread_HandleResponse(void *response);
	static void *Handle_Thread(void *hPort);
	
	string getCMD(string msg);
	string getNode(string msg);
	string getArgs(string msg);
	
protected:
	string partialHex(int dec);
	string HexToStr(int dec);

	void setCommand();
	void setNameCommand();
	int StrToInt(string id);
	bool checkCRC(string msg);	
	
	string calculateCRC(string command);
	virtual void responseAction(string);
	
	string StrToDbHex(string text);
	
public:
	conectorBusCan();
	virtual ~conectorBusCan();
	
	
	virtual bool exec(string command,string node="",string args="");
	void Open(string port="/dev/ttyUSB0");
	
	void launchListener();
	void Close();
};

#endif /* CONECTORBUSCAN_H_ */
