REM Control Flow Test
REM Tests IF/THEN/ELSE, GOTO, GOSUB, ON GOTO, ON GOSUB

PRINT "=== Control Flow Tests ==="
PRINT ""

REM IF/THEN single line
x = 10
IF x > 5 THEN PRINT "x > 5: TRUE"
IF x < 5 THEN PRINT "x < 5: should not print"
PRINT ""

REM IF/THEN/ELSE block
IF x = 10 THEN
    PRINT "Block IF: x is 10"
ELSE
    PRINT "Block IF: x is other"
END IF
PRINT ""

REM Nested IF
y = 20
IF x = 10 THEN
    IF y = 20 THEN
        PRINT "Nested IF: x=10, y=20"
    ELSE
        PRINT "Nested IF: x=10, y<>20"
    END IF
ELSE
    PRINT "Nested IF: x<>10"
END IF
PRINT ""

REM GOTO
PRINT "Before GOTO"
GOTO 100
PRINT "This should NOT print"
100 PRINT "After GOTO to 100"
PRINT ""

REM GOSUB/RETURN
PRINT "Before GOSUB"
GOSUB 200
PRINT "After GOSUB return"
GOTO 300

200 PRINT "Inside GOSUB"
RETURN

300 PRINT ""

REM ON GOTO
n = 2
ON n GOTO 410, 420, 430
410 PRINT "ON GOTO: first": GOTO 440
420 PRINT "ON GOTO: second": GOTO 440
430 PRINT "ON GOTO: third": GOTO 440
440 PRINT ""

REM ON GOSUB
n = 3
ON n GOSUB 510, 520, 530
GOTO 540
510 PRINT "ON GOSUB: first": RETURN
520 PRINT "ON GOSUB: second": RETURN
530 PRINT "ON GOSUB: third": RETURN
540 PRINT ""

PRINT "All control flow tests passed!"
END
