10 REM Loop Examples - FOR, WHILE, DO loops
20 REM Demonstrates different loop types in RB BASIC
30 CLS
40 PRINT "RB BASIC Loop Examples"
50 PRINT "======================"
60 PRINT
70 PRINT "1. FOR...NEXT Loop:"
80 PRINT "   Counting from 1 to 5:"
90 FOR I = 1 TO 5
100   PRINT "   "; I
110 NEXT I
120 PRINT
130 PRINT "2. FOR...NEXT with STEP:"
140 PRINT "   Even numbers from 2 to 10:"
150 FOR I = 2 TO 10 STEP 2
160   PRINT "   "; I
170 NEXT I
180 PRINT
190 PRINT "3. WHILE...WEND Loop:"
200 PRINT "   Countdown from 5:"
210 COUNT = 5
220 WHILE COUNT > 0
230   PRINT "   "; COUNT
240   COUNT = COUNT - 1
250 WEND
260 PRINT
270 PRINT "4. DO...LOOP:"
280 PRINT "   Sum of numbers 1 to 5:"
290 TOTAL = 0
300 NUM = 1
310 DO WHILE NUM <= 5
320   TOTAL = TOTAL + NUM
330   NUM = NUM + 1
340 LOOP
350 PRINT "   Sum = "; TOTAL
360 PRINT
370 PRINT "5. Nested Loops:"
380 PRINT "   Multiplication table (1-5):"
390 FOR ROW = 1 TO 5
400   FOR COL = 1 TO 5
410     RESULT = ROW * COL
420     PRINT USING "####"; RESULT;
430   NEXT COL
440   PRINT
450 NEXT ROW
460 PRINT
470 PRINT "Press any key to exit..."
480 A$ = INPUT$(1)
490 END
