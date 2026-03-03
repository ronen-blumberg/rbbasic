REM SUBs and DEF FN Test
REM Tests SUB calls (no params) and DEF FN

PRINT "=== SUB/FUNCTION Tests ==="
PRINT ""

REM Test SUB with no params
CALL PrintLine
PRINT "After SUB call"
CALL PrintLine
PRINT ""

REM Test DEF FN
DEF FNDouble(x) = x * 2
DEF FNCube(x) = x * x * x
DEF FNCircleArea(r) = 3.14159 * r * r
PRINT "FNDouble(8) ="; FNDouble(8)
PRINT "FNCube(3) ="; FNCube(3)
PRINT "FNCircleArea(5) ="; INT(FNCircleArea(5) * 100) / 100
PRINT ""

REM Test GOSUB as pseudo-function
total = 0
val1 = 10
GOSUB 1000
val1 = 20
GOSUB 1000
val1 = 30
GOSUB 1000
PRINT "Running total:"; total
PRINT ""

PRINT "All SUB/FUNCTION tests passed!"
END

1000 REM Add val1 to total
total = total + val1
RETURN

SUB PrintLine
    PRINT "----------------"
END SUB
