all:
	gcc ql_basic.c serial.c -o ql-basic -Wall -Wextra -O2

clean:
	rm -f ql-basic
