REM REDIM and ERASE test program

PRINT "=== REDIM/ERASE Tests ==="
PRINT

REM Test 1: Basic DIM and fill
DIM arr(5)
FOR i = 0 TO 5
    arr(i) = i * 10
NEXT
PRINT "Original array:"
FOR i = 0 TO 5
    PRINT "arr("; i; ") ="; arr(i)
NEXT
PRINT

REM Test 2: REDIM (reset array)
REDIM arr(10)
PRINT "After REDIM arr(10):"
PRINT "arr(0) ="; arr(0); " (should be 0 - data cleared)"
arr(8) = 88
PRINT "arr(8) ="; arr(8); " (should be 88)"
PRINT

REM Test 3: ERASE
ERASE arr
PRINT "After ERASE: array freed"
PRINT

REM Test 4: DIM SHARED (should work same as DIM)
DIM SHARED scores(3)
scores(1) = 100
scores(2) = 95
scores(3) = 87
PRINT "DIM SHARED scores:"
FOR i = 1 TO 3
    PRINT "scores("; i; ") ="; scores(i)
NEXT
PRINT

PRINT "All REDIM/ERASE tests passed!"
END
