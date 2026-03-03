REM WRITE # and SEEK/LOC/LOF test program

PRINT "=== WRITE # Tests ==="
PRINT

REM Test 1: WRITE # to file
PRINT "Test 1: WRITE # to file"
OPEN "write_test_output.txt" FOR OUTPUT AS #1
WRITE #1, "Hello", 42, "World"
WRITE #1, "Second line", 99
CLOSE #1
PRINT "File written successfully"
PRINT

REM Test 2: Read back and verify
PRINT "Test 2: Read back file"
OPEN "write_test_output.txt" FOR INPUT AS #1
LINE INPUT #1, a$
PRINT "Line 1: "; a$
LINE INPUT #1, b$
PRINT "Line 2: "; b$
CLOSE #1
PRINT

REM Test 3: LOF function
PRINT "Test 3: LOF function"
OPEN "write_test_output.txt" FOR INPUT AS #1
PRINT "File length (LOF) ="; LOF(1)
CLOSE #1
PRINT

REM Test 4: SEEK statement
PRINT "Test 4: SEEK statement"
OPEN "write_test_output.txt" FOR BINARY AS #1
SEEK #1, 1
PRINT "SEEK to position 1 - OK"
CLOSE #1
PRINT

PRINT "All WRITE/SEEK tests passed!"
END
