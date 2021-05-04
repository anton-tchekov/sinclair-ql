10 OPEN #3, ser2i
20 BAUD 2400
30 PRINT #3, "Hello World!";
40 PRINT #3, CHR$(13)
50 REPeat serialin
60 INPUT #3, A$
70 PRINT #3, A$
80 PRINT A$
90 END REPeat serialin
100 CLOSE #3
