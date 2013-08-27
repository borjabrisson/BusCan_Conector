CC := g++ -std=c++03
BASEFLAGS=  -lpthread ${MySQLFLAGS}
CPPFLAGS = -O0 -g3 -Wall -c -fmessage-length=0  -MMD -MP
MySQLFLAGS = `mysql_config --cflags --libs`

all	: listen serverIP
# 	${CC} server.cpp  -fpermissive ${BASEFLAGS} -o server.out ./hijo.o 
# 	${CC} main.cpp  -fpermissive ${BASEFLAGS} -o server.out ./hijo.o 
	${CC} main.cpp -o hw.out "serie.o" "busCan.o" "api.o" "requestHandler.o" "serverIP.o" -fpermissive ${BASEFLAGS} 
	rm *.o *.d
serverIP: listen
	${CC} -fpermissive ${BASEFLAGS}  ${CPPFLAGS} -MF"serverIP.d" -MT"serverIP.d" -o "serverIP.o" serverIP.cpp
listen	: sgbd apiBusCan
	${CC} ${BASEFLAGS} ${CPPFLAGS} -MF"requestHandler.d" -MT"requestHandler.d" -o "requestHandler.o" "sgbd_baseConector.o" listen/requestHandler.cpp
sgbd	:
	${CC} ${MySQLFLAGS} ${CPPFLAGS} -MF"sgbd_baseConector.d" -MT"sgbd_baseConector.d" -o "sgbd_baseConector.o" conector_SGBD/sgbd_baseConector.cpp
conectorSerial	: 
	${CC} -fpermissive ${BASEFLAGS} ${CPPFLAGS} -MF"serie.d" -MT"serie.d" -o "serie.o" BusCan/conectorSerial.cpp
conectorBusCan : conectorSerial
	${CC} -fpermissive ${BASEFLAGS} ${CPPFLAGS} -MF"busCan.d" -MT"busCan.d" -o "busCan.o"  BusCan/conectorBusCan.cpp
apiBusCan : conectorBusCan
	${CC} -fpermissive ${BASEFLAGS} ${CPPFLAGS} -MF"api.d" -MT"api.d" -o "api.o"  BusCan/apiBusCan.cpp	
clean	:
	rm *.o *.d
no-temporal:
	rm *.cpp~ *.h~ */*.cpp~ */*.h~
