REM ON ERROR GOTO / RESUME test program

PRINT "=== Error Handling Tests ==="
PRINT

REM Test 1: ON ERROR GOTO with file not found
10 PRINT "Test 1: ON ERROR GOTO"
20 ON ERROR GOTO 100
30 OPEN "nonexistent_file_xyz.txt" FOR INPUT AS #1
40 PRINT "After error line (RESUME NEXT continues here)"
50 GOTO 200

100 PRINT "Error caught! ERR ="; ERR; " ERL ="; ERL
110 RESUME NEXT

200 PRINT "Continued after error handler"
PRINT

REM Test 2: ON ERROR GOTO 0 (disable)
300 PRINT "Test 2: ON ERROR GOTO 0"
310 ON ERROR GOTO 0
320 PRINT "Error handling disabled"
PRINT

REM Test 3: ERR and ERL as variables
400 PRINT "Test 3: ERR/ERL values"
410 PRINT "Last ERR ="; ERR
420 PRINT "Last ERL ="; ERL
PRINT

PRINT "All error handling tests passed!"
END
