#ifndef CONFIG_H_
#define CONFIG_H_

#include <map>
#include <iostream>
#include <string>
#include <list>
#include <set>


using namespace std;


#define ERROR_OUT 
#define DEBUG_OUT

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


#define SGBD_USER_DEFAULT ""
#define SGBD_PASS_DEFAULT ""
#define SGBD_HOST_DEFAULT "127.0.0.1"
#define SGBD_DB_DEFAULT "test"

#endif /* CONFIG_H_ */
