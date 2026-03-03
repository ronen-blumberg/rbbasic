REM CONST and SWAP test program

PRINT "=== CONST and SWAP Tests ==="
PRINT

REM Test 1: CONST numeric
CONST PI = 3.14159
PRINT "PI ="; PI

REM Test 2: CONST string
CONST GREETING$ = "Hello World"
PRINT "Greeting: "; GREETING$
PRINT

REM Test 3: SWAP numeric
a = 10
b = 20
PRINT "Before SWAP: a ="; a; " b ="; b
SWAP a, b
PRINT "After SWAP: a ="; a; " b ="; b
PRINT

REM Test 4: SWAP string
x$ = "First"
y$ = "Second"
PRINT "Before SWAP: x$ = "; x$; " y$ = "; y$
SWAP x$, y$
PRINT "After SWAP: x$ = "; x$; " y$ = "; y$
PRINT

PRINT "All CONST/SWAP tests passed!"
END
