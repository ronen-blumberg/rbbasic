10 REM String Functions Demo
20 REM Demonstrates string manipulation in RB BASIC
30 CLS
40 PRINT "RB BASIC String Functions"
50 PRINT "========================="
60 PRINT
70 TEXT$ = "Ronen Blumberg BASIC"
80 PRINT "Original string: "; TEXT$
90 PRINT
100 PRINT "String Functions:"
110 PRINT "-----------------"
120 PRINT "LEN(TEXT$) = "; LEN(TEXT$)
130 PRINT "LEFT$(TEXT$, 5) = "; LEFT$(TEXT$, 5)
140 PRINT "RIGHT$(TEXT$, 5) = "; RIGHT$(TEXT$, 5)
150 PRINT "MID$(TEXT$, 7, 9) = "; MID$(TEXT$, 7, 9)
160 PRINT
170 PRINT "Case Conversion:"
180 PRINT "----------------"
190 PRINT "UCASE$(TEXT$) = "; UCASE$(TEXT$)
200 PRINT "LCASE$(TEXT$) = "; LCASE$(TEXT$)
210 PRINT
220 PRINT "Search:"
230 PRINT "-------"
240 SEARCH$ = "BASIC"
250 POS = INSTR(TEXT$, SEARCH$)
260 IF POS > 0 THEN
270   PRINT "'"; SEARCH$; "' found at position"; POS
280 ELSE
290   PRINT "'"; SEARCH$; "' not found"
300 END IF
310 PRINT
320 PRINT "ASCII Codes:"
330 PRINT "------------"
340 PRINT "ASC('A') = "; ASC("A")
350 PRINT "ASC('Z') = "; ASC("Z")
360 PRINT "ASC('0') = "; ASC("0")
370 PRINT "CHR$(65) = "; CHR$(65)
380 PRINT "CHR$(90) = "; CHR$(90)
390 PRINT
400 PRINT "String Building:"
410 PRINT "----------------"
420 FIRST$ = "Hello"
430 SECOND$ = "World"
440 COMBINED$ = FIRST$ + " " + SECOND$ + "!"
450 PRINT COMBINED$
460 PRINT
470 PRINT "SPACE$(10) creates: '"; SPACE$(10); "' (10 spaces)"
480 PRINT "STRING$(5, '*') creates: "; STRING$(5, "*")
490 PRINT
500 PRINT "Number/String Conversion:"
510 PRINT "-------------------------"
520 NUM = 42
530 NUMSTR$ = STR$(NUM)
540 PRINT "STR$(42) = '"; NUMSTR$; "'"
550 STRNUM$ = "123"
560 VALUE = VAL(STRNUM$)
570 PRINT "VAL('123') = "; VALUE
580 PRINT
590 PRINT "Interactive Demo:"
600 PRINT "----------------"
610 INPUT "Enter your name"; NAME$
620 PRINT "Hello, "; NAME$; "!"
630 PRINT "Your name has"; LEN(NAME$); "letters."
640 IF LEN(NAME$) > 0 THEN
650   PRINT "First letter: "; LEFT$(NAME$, 1)
660   PRINT "Last letter: "; RIGHT$(NAME$, 1)
670 END IF
680 PRINT
690 PRINT "Press any key to exit..."
700 A$ = INPUT$(1)
710 END
