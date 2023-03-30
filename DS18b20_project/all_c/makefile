INCLUDE_NAME=all_h/

PATHNAME=all_so/

libname=mydll

test=

L1:
	gcc DS18b20.c server_connect.c databack.c -shared -fPIC -o lib${libname}.so
	mv lib${libname}.so ../all_so

L2:
	gcc ${test} -shared -fPIC -o libtest.so
	mv lib${libname}.so ../all_so
