
# g++ -I/usr/include/mysql -O0 -g3 -Wall -DBIG_JOINS=1  -fno-strict-aliasing  -g -c -fmessage-length=0 -MMD -MP -MF"sgbd_baseConector.d" -MT"sgbd_baseConector.d" -L '/usr/lib/i386-linux-gnu' -lmysqlclient -lpthread -lz -lm -lrt -ldl -o "sgbd_baseConector.o" sgbd_baseConector.cpp

g++ `mysql_config --cflags --libs` -O0 -g3 -Wall  -c -fmessage-length=0 -MMD -MP -MF"sgbd_baseConector.d" -MT"sgbd_baseConector.d" -o "sgbd_baseConector.o" conector_SGBD/sgbd_baseConector.cpp


objBD=' ./sgbd_baseConector.o'
g++ main.cpp -o conector.out  $objBD `mysql_config --cflags --libs` 

rm *.o *.d