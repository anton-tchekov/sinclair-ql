#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>

int serial_open(char *port, int baud, int bits);

#endif /* __SERIAL_H__ */
