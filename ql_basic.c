#include <stdio.h>
#include <stdlib.h>
#include "serial.h"

/*
	Usage on Sinclair QL
		Command: "BAUD 2400 : LOAD ser2z"
		Cable on serial port 2
		ASCII 26 -> end of file

	Usage on Host
		sudo ./loadbasic "/dev/ttyUSB0" "filename.bas"
*/

int main(int argc, char *argv[])
{
	FILE *fp;
	int fd;
	long fsize;
	char *buf;
	char end[2] = {26, 0};

	if(argc != 3)
	{
		fprintf(stderr, "Usage ./ql-basic \"port\" \"file\"\n");
		return 0;
	}

	if((fd = serial_open(argv[1], 2400, 7)) == -1)
	{
		fprintf(stderr, "Couldn't open serial port \"%s\".\n", argv[1]);
		return 1;
	}

	if((fp = fopen(argv[2], "r")) == NULL)
	{
		printf("Error opening file: \"%s\".\n", argv[2]);
		return 1;
	}

	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	rewind(fp);

	buf = malloc(fsize + 1);
	fsize = fread(buf, fsize, 1, fp);

	write(fd, buf, fsize);
	write(fd, end, 1);
	tcdrain(fd);

	fclose(fp);
	close(fd);
	return 0;
}
