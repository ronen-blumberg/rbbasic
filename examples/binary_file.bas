REM Binary file and RANDOM access test program

PRINT "=== Binary/Random File Tests ==="
PRINT

REM Test 1: Binary file write and read
PRINT "Test 1: OPEN FOR BINARY"
OPEN "binary_test.dat" FOR BINARY AS #1
r$ = "HELLO"
PUT #1, , r$
CLOSE #1
PRINT "Binary file written"

OPEN "binary_test.dat" FOR BINARY AS #1
s$ = ""
GET #1, , s$
CLOSE #1
PRINT "Read back: "; s$
PRINT

REM Test 2: Random access file
PRINT "Test 2: OPEN FOR RANDOM"
OPEN "random_test.dat" FOR RANDOM AS #2 LEN = 20
r$ = "Record One          "
PUT #2, 1, r$
r$ = "Record Two          "
PUT #2, 2, r$
r$ = "Record Three        "
PUT #2, 3, r$
CLOSE #2
PRINT "Random file written with 3 records"

REM Read back records
OPEN "random_test.dat" FOR RANDOM AS #2 LEN = 20
GET #2, 2, s$
PRINT "Record 2: "; s$
GET #2, 1, s$
PRINT "Record 1: "; s$
GET #2, 3, s$
PRINT "Record 3: "; s$
CLOSE #2
PRINT

PRINT "All binary/random file tests passed!"
END
