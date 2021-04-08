# sinclair-ql
Transfers BASIC code to a Sinclair QL over serial port

Usage on Sinclair QL
* Command: `BAUD 2400 : LOAD ser2z`
* Cable on serial port 2
* ASCII 26 -> end of file

Usage on Host
* `sudo ./loadbasic "/dev/ttyUSB0" "filename.bas"`
