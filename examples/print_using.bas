REM PRINT USING test program

PRINT "=== PRINT USING Tests ==="
PRINT

REM Test 1: Basic numeric format
PRINT "Test 1: Numeric formatting"
PRINT USING "###.##"; 3.14159
PRINT USING "###.##"; 42.5
PRINT USING "#####"; 12345
PRINT

REM Test 2: Dollar sign
PRINT "Test 2: Dollar format"
PRINT USING "$$###.##"; 99.95
PRINT USING "$$###.##"; 1234.56
PRINT

REM Test 3: Multiple values
PRINT "Test 3: Negative numbers"
PRINT USING "###.##"; -42.5
PRINT USING "####"; 0
PRINT

PRINT "All PRINT USING tests passed!"
END
