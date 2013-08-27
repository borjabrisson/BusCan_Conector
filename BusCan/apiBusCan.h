/*
 * apiBusCan.h
 *
 *  Created on: Aug 20, 2013
 *      Author: borja
 */

#ifndef APIBusCAN_H_
#define APIBusCAN_H_

#include "conectorBusCan.h"

enum apiCommand{
// 	api_Init=,
// 	api_End=,
	api_reset=0x93,
	api_Test=0x94,
// 	api_SetCFG=,
// 	api_GetNodeCFG=,
// 	api_SetNodeCFG=,
// 	api_AllowAccess=,
// 	api_DenyAccess=,
	api_SetText=0x90,
	api_ClearText=0x91,
	api_SendMessage=0x92,
// 	api_Start=,
// 	api_Stop=,
 	api_Scan=0x99
};

class apiBusCan : public conectorBusCan{
private: // Atributos
	int TestMode;
protected:
	virtual void responseAction(string);
	
	
	void setText(string msg,string node);
	void sendMsg(string msg,string node);
	void writeText(string msg,string node);
	void writeLine(int line,string text,string node);
	
	void setApiCmd();	
	void ActiveResponseAction(string cmd,string node,string args);
	void TestResponseAction(string cmd,string node,string args);

public:
	apiBusCan();
	virtual ~apiBusCan();
	
	virtual bool exec(string command,string node="",string args="");	
	
	int getFD();
	void setFD(int fd);
};

#endif /* APIBusCAN_H_ */
