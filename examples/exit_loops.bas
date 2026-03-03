REM EXIT statement test program

PRINT "=== EXIT Statement Tests ==="
PRINT

REM Test 1: EXIT FOR
PRINT "Test 1: EXIT FOR"
FOR i = 1 TO 100
    IF i = 5 THEN EXIT FOR
NEXT
PRINT "Exited FOR at i ="; i
PRINT

REM Test 2: EXIT DO (with WHILE)
PRINT "Test 2: EXIT DO"
x = 0
DO WHILE x < 100
    x = x + 1
    IF x = 3 THEN EXIT DO
LOOP
PRINT "Exited DO at x ="; x
PRINT

REM Test 3: EXIT WHILE
PRINT "Test 3: EXIT WHILE"
y = 0
WHILE y < 100
    y = y + 1
    IF y = 7 THEN EXIT WHILE
WEND
PRINT "Exited WHILE at y ="; y
PRINT

REM Test 4: Nested EXIT FOR
PRINT "Test 4: Nested EXIT FOR"
total = 0
FOR i = 1 TO 10
    FOR j = 1 TO 10
        IF j = 3 THEN EXIT FOR
        total = total + 1
    NEXT
NEXT
PRINT "Total (should be 20):"; total
PRINT

PRINT "All EXIT tests passed!"
END
