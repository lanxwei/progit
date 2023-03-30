INCLUDE_NAME=all_h/

PATHNAME=all_so/

libname=mydll

FILENAME=client

server_host_ip=59.172.145.8

server_host_name=studio.iot-yun.club

port=7777

time=30

all:
	gcc ${FILENAME}.c -I ${INCLUDE_NAME} -L ${PATHNAME} -l${libname} -lsqlite3

L1:
	./a.out -H ${server_host_ip} -p ${port} -t ${time}

L2:all
	./a.out -H ${server_host_name} -p ${port} -t ${time} -n 1

L3:
	./a.out -H 127.0.0.1 -p 7777
