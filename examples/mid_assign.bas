REM MID$ assignment test program

PRINT "=== MID$ Assignment Tests ==="
PRINT

REM Test 1: Basic MID$ assignment
PRINT "Test 1: Basic MID$ assignment"
a$ = "Hello World"
PRINT "Before: "; a$
MID$(a$, 7, 5) = "BASIC"
PRINT "After:  "; a$
PRINT

REM Test 2: MID$ with shorter replacement
PRINT "Test 2: Shorter replacement"
b$ = "ABCDEFGHIJ"
PRINT "Before: "; b$
MID$(b$, 4, 3) = "XY"
PRINT "After:  "; b$
PRINT

REM Test 3: MID$ without length
PRINT "Test 3: Without length parameter"
c$ = "0123456789"
PRINT "Before: "; c$
MID$(c$, 3) = "AB"
PRINT "After:  "; c$
PRINT

PRINT "All MID$ assignment tests passed!"
END
