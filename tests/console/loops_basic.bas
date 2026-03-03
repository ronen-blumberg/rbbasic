REM Basic Loops Test
REM Tests FOR, WHILE, DO loops without user input

PRINT "=== Loop Tests ==="
PRINT ""

REM FOR loop
PRINT "FOR 1 to 5:";
FOR i = 1 TO 5
    PRINT i;
NEXT i
PRINT ""

REM FOR with STEP
PRINT "FOR 10 to 2 STEP -2:";
FOR i = 10 TO 2 STEP -2
    PRINT i;
NEXT i
PRINT ""

REM WHILE loop
PRINT "WHILE count < 4:";
count = 0
WHILE count < 4
    PRINT count;
    count = count + 1
WEND
PRINT ""

REM DO...LOOP WHILE
PRINT "DO..LOOP WHILE:";
n = 1
DO
    PRINT n;
    n = n * 2
LOOP WHILE n <= 16
PRINT ""

REM DO WHILE...LOOP
PRINT "DO WHILE..LOOP:";
n = 100
DO WHILE n >= 10
    PRINT n;
    n = n / 2
LOOP
PRINT ""

REM DO...LOOP UNTIL
PRINT "DO..LOOP UNTIL:";
n = 1
DO
    PRINT n;
    n = n + 1
LOOP UNTIL n > 5
PRINT ""

REM Nested FOR
PRINT "Nested FOR:"
FOR i = 1 TO 3
    FOR j = 1 TO 3
        PRINT i * 10 + j;
    NEXT j
    PRINT ""
NEXT i

PRINT ""
PRINT "All loop tests passed!"
END
