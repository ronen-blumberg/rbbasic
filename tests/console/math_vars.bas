REM Math and Variables Test
REM Tests basic arithmetic, variables, built-in math functions

PRINT "=== Math and Variables Tests ==="
PRINT ""

REM Basic arithmetic
a = 10
b = 3
PRINT "10 + 3 ="; a + b
PRINT "10 - 3 ="; a - b
PRINT "10 * 3 ="; a * b
PRINT "10 / 3 ="; INT(a / b * 100) / 100
PRINT "10 MOD 3 ="; a MOD b
PRINT "10 ^ 2 ="; a ^ 2
PRINT ""

REM Integer functions
PRINT "INT(7.9) ="; INT(7.9)
PRINT "INT(-7.9) ="; INT(-7.9)
PRINT "ABS(-42) ="; ABS(-42)
PRINT "SGN(-5) ="; SGN(-5)
PRINT "SGN(0) ="; SGN(0)
PRINT "SGN(5) ="; SGN(5)
PRINT ""

REM Math functions
PRINT "SQR(144) ="; SQR(144)
PRINT "LOG(1) ="; LOG(1)
PRINT ""

REM String variables
a$ = "Hello"
b$ = " World"
c$ = a$ + b$
PRINT "Concat: "; c$
PRINT "LEN ="; LEN(c$)
PRINT "LEFT$ ="; LEFT$(c$, 5)
PRINT "RIGHT$ ="; RIGHT$(c$, 5)
PRINT "MID$ ="; MID$(c$, 4, 5)
PRINT ""

REM Type conversions
x = 42
PRINT "STR$(42) = ["; STR$(x); "]"
PRINT "VAL(123.5) ="; VAL("123.5")
PRINT "CHR$(65) ="; CHR$(65)
PRINT ""

REM UCASE/LCASE
d$ = "Hello World"
PRINT "UCASE$ ="; UCASE$(d$)
PRINT "LCASE$ ="; LCASE$(d$)
PRINT ""

PRINT "All math/variable tests passed!"
END
