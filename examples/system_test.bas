REM Phase 4: System Functions Test
PRINT "=== System Functions Tests ==="
PRINT

REM Test 1: SPACE$ function
PRINT "Test 1: SPACE$"
PRINT "["; SPACE$(5); "]"
PRINT

REM Test 2: STRING$ function
PRINT "Test 2: STRING$"
PRINT STRING$(10, "*")
PRINT STRING$(5, 65)
PRINT

REM Test 3: LTRIM$/RTRIM$/TRIM$
PRINT "Test 3: TRIM functions"
a$ = "   Hello   "
PRINT "["; LTRIM$(a$); "]"
PRINT "["; RTRIM$(a$); "]"
PRINT "["; TRIM$(a$); "]"
PRINT

REM Test 4: ENVIRON$
PRINT "Test 4: ENVIRON$"
p$ = ENVIRON$("PATH")
IF LEN(p$) > 0 THEN
    PRINT "PATH is set (length ="; LEN(p$); ")"
ELSE
    PRINT "PATH not found"
END IF
PRINT

REM Test 5: COMMAND$
PRINT "Test 5: COMMAND$"
PRINT "Command line: ["; COMMAND$; "]"
PRINT

REM Test 6: PEEK/POKE
PRINT "Test 6: PEEK/POKE"
POKE 100, 42
PRINT "PEEK(100) ="; PEEK(100)
POKE 101, 255
PRINT "PEEK(101) ="; PEEK(101)
PRINT

REM Test 7: DEF SEG
PRINT "Test 7: DEF SEG"
DEF SEG = 0
PRINT "DEF SEG = 0 set OK"
DEF SEG
PRINT "DEF SEG reset OK"
PRINT

PRINT "All system function tests passed!"
END
