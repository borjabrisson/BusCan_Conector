/*
 * sgbd_baseConector.cpp
 *
 *  Created on: 27/12/2012
 *      Author: Borja
 */

#include "sgbd_baseConector.h"
/*
 sgbd_baseConector::sgbd_baseConector() {}

 sgbd_baseConector::~sgbd_baseConector() {}
 */

sgbd_baseConector::sgbd_baseConector() {
	this->username = SGBD_USER_DEFAULT;
	this->pass = SGBD_PASS_DEFAULT;
	this->host = SGBD_HOST_DEFAULT;
	this->db = SGBD_DB_DEFAULT;
	this->port = MYSQL_PORT;

}

int sgbd_baseConector::conect(string db) {
	if (!(this->myData = mysql_init(0))) {
		// Imposible crear el objeto myData
		push_Error(
				"\nsgbd_baseConector::Conect.\n Imposible crear el objeto myData:: "
						+ (string) mysql_error(this->myData));
		this->msgSP = mysql_error(this->myData);
		return 1;
	}
	if (db == "")
		db = this->db;
	if (!mysql_real_connect(this->myData, this->host.c_str(),
			this->username.c_str(), this->pass.c_str(), db.c_str(), this->port,
			NULL, 0)) {
		// No se puede estabñecer la conexión con el servidor.
		push_Error(
				"\nsgbd_baseConector::Conect.\n No se ha podido crear la conexion:\n Error:"
						+ (string) mysql_error(this->myData));
		this->msgSP = mysql_error(this->myData);
		this->desconect();
		return 1;
	}
	return 0;
}

int sgbd_baseConector::desconect() {
	if (this->myData != NULL) {
		mysql_close(this->myData);
		this->myData = NULL;
		return 0;
	}
	push_Error(
			"\nsgbd_baseConector::desconect.Se está intentando cerrar una conexión no establecida.");
	return 2;
}

int sgbd_baseConector::query(string db, string clause,
		list<field_type> &record) {
	if (this->conect(db)) return 0;
	if (mysql_query(this->myData, clause.c_str())) {
		// Error al realizar la consulta:
		push_Error(
				"sgbd_baseConector::query. Error en la consulta. Error:"
						+ (string) mysql_error(this->myData));
		push_Error(
				"sgbd_baseConector::query. Error en la consulta. consulta:"
						+ clause);
		this->msgSP = mysql_error(this->myData);
//              cout << "ERROR: " << mysql_error(this->myData) << endl;
		this->desconect();
		return 2;
	}
	MYSQL_RES *res;
	if ((res = mysql_use_result(this->myData))) {
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(res))) {
			record.push_back(this->get_row(row, res));
		}
		mysql_free_result(res);
	}
	this->desconect();
	return record.size();
}

string sgbd_baseConector::msgLastProcedure() {
	return this->msgSP;
}

map<string,statment> sgbd_baseConector::getResultStatment(){
	return this->resultStatment;
}

void sgbd_baseConector::setHost(string host){
	this->host = host;
}

int sgbd_baseConector::procedure(string db, string clause) {
	if (this->conect(db)) return 0;
	mysql_autocommit(this->myData, 0);

	clause = "call " + clause;
	if (mysql_query(this->myData, clause.c_str())) {
		// Error al realizar la consulta:
		push_Error(
				"SGBD:Procedure. ERROR: " + (string) mysql_error(this->myData));
		this->msgSP = mysql_error(this->myData);
//              this->rollback();
		this->desconect();
		return 0; //2
	}
	map<string, string> data = this->resultSP();
	int ret = 1;
	if (data.count("error") != 0) {
		if (data["error"] == "0")
			this->commit();
		else {
			this->rollback();
			ret = 0;
		}
	} else {
		push_Error(
				"SGBD:Procedure.No se encuentra el campo error en la respuesta del procedimiento.");
		this->rollback();
		ret = 0;
	}

	this->setMessage(data);

	this->desconect();
	return ret;
}

void sgbd_baseConector::setMessage(map<string, string> data) {
	if ((data.count("msg") != 0) || (data.count("message") != 0)) {
		if (data.count("msg") != 0)
			this->msgSP = data["msg"];
		if (data.count("message") != 0)
			this->msgSP = data["message"];
	} else
		this->msgSP = "";
}

map<string, string> sgbd_baseConector::resultSP() {
	MYSQL_RES *res;
	statment statmentContainer;
	field_type data;
	MYSQL_ROW row;
	resultStatment.clear();
	while (mysql_more_results(this->myData)) { // La librería nos obliga a realizar un while para poder liberar los recursos y cerrar la conexión. ###
		res = mysql_store_result(this->myData);
		if ((statmentContainer.size() != 0) && (data.count("resultId") >0)){
			if(resultStatment.count(data["resultId"])>0) {// Ya existe un stament con este nombre.
				resultStatment[data["resultId"]].splice(resultStatment[data["resultId"]].end(),statmentContainer);
				cout << "Tamaño del statment Compuesto "<<data["command"]<<" = "<<resultStatment[data["resultId"]].size()<< endl;
			}
			else{
				resultStatment[data["resultId"]] = statmentContainer;
			}
		}

		while ((row = mysql_fetch_row(res))) {
			data = this->get_row(row, res);
			statmentContainer.push_back(data);
		}

		/*for(field_type::iterator it= data.begin();it != data.end();it++){
			cout << (*it).first << "::  "<<(*it).second << endl;
		}*/
		mysql_free_result(res);
		mysql_next_result(this->myData);
	}
	statmentContainer.pop_back();
	if (statmentContainer.size() != 0)resultStatment[data["resultId"]] = statmentContainer; //###|
	return data;
}

map<string, string> sgbd_baseConector::get_row(MYSQL_ROW &row, MYSQL_RES *res) {
	map<string, string> data;
	MYSQL_FIELD* fields = mysql_fetch_fields(res);
	int nFields = mysql_num_fields(res);
	string value;

	push_Debug("numero de campos " + nFields);
	for (int ind_field = 0; ind_field < nFields; ind_field++) {
		push_Debug("SGB:resultSP: Campo: " + (string) fields[ind_field].name
				 + " Valor: " + (string) row[ind_field]);

		if (row[ind_field] == NULL)	value="NULL";
		else value =row[ind_field];

		data.insert(pair<string, string>(fields[ind_field].name, value));
	}
	return data;
}

bool sgbd_baseConector::commit() {
	if (!mysql_commit(this->myData))
		return true;
	push_Error("SGBD:Commit. Error" + (string) mysql_error(this->myData));
	this->msgSP = mysql_error(this->myData);
	return false;
}

bool sgbd_baseConector::rollback() {
	if (!mysql_rollback(this->myData))
		return true;
	push_Error("SGBD:RollBack. Error" + (string) mysql_error(this->myData));
	this->msgSP = mysql_error(this->myData);
	return false;
}
