/*
 * apiBusCan.h
 *
 *  Created on: Aug 20, 2013
 *      Author: borja
 */

#ifndef APIBusCAN_H_
#define APIBusCAN_H_

#include "conectorBusCan.h"
#include "../conector_SGBD/sgbd_baseConector.h"

enum apiCommand{
 	api_Init=0x90,
 	api_End=0x91,
	api_reset=0x92,
	api_Test=0x93,
 	api_SetCFG=0x94,
 	api_GetNodeCFG=0x95,
 	api_SetNodeCFG=0x96,
 	api_AllowAccess=0x97,
 	api_DenyAccess=0x98,
	api_SetText=0x99,
	api_ClearText=0x9A,
	api_SendMessage=0x9B,
 	api_Start=0x9C,
 	api_Stop=0x9D,
 	api_Scan=0x9E,
 	api_SetMode=0x9F,
 	api_ChangeHost=0xA0
};
typedef map<string,string> itemCmdList;
class apiBusCan : public conectorBusCan{
public: // Atributos
	int TestMode;
	sgbd_baseConector mysql;

private:
	int LedPeriod;
	int BuzzerAllowPeriod,BuzzerDenyPeriod;
	int MessagePeriod,RelayPeriod;
protected:
	virtual void responseAction(string);
	
	
	void setText(string msg,string node);
	void sendMsg(string msg,string node);
	void writeText(string msg,string node);
	void writeLine(int line,string text,string node);
	
	void setApiCmd();	
	void ActiveResponseAction(string cmd,string node,string args);
	void TestResponseAction(string cmd,string node,string args);


	void scan(string args);

	void runProcedure(string clause,string node);
	void allowAccess(string node,string msg);
	void denyAccess(string node,string msg);

	void alarm(int seconds,string node);
public:
	void execCmdList(list<itemCmdList> cmd);
	apiBusCan();
	virtual ~apiBusCan();
	
	virtual bool exec(string command,string node="",string args="");	
	void start();
	
	int getFD();
	void setFD(int fd);
};

#endif /* APIBusCAN_H_ */
