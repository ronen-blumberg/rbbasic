10 REM Simple Graphics Test - No pauses
20 SCREEN 12
30 CLS
40 REM Draw some pixels
50 FOR I = 1 TO 200
60   X = INT(RND * 640)
70   Y = INT(RND * 480)
80   C = INT(RND * 16)
90   PSET (X, Y), C
100 NEXT I
110 REM Draw some lines
120 FOR I = 1 TO 10
130   X1 = INT(RND * 640)
140   Y1 = INT(RND * 480)
150   X2 = INT(RND * 640)
160   Y2 = INT(RND * 480)
170   C = INT(RND * 16)
180   LINE (X1, Y1)-(X2, Y2), C
190 NEXT I
200 REM Draw some circles
210 FOR I = 1 TO 10
220   X = INT(RND * 640)
230   Y = INT(RND * 480)
240   R = INT(RND * 50) + 10
250   C = INT(RND * 16)
260   CIRCLE (X, Y), R, C
270 NEXT I
280 REM Wait 3 seconds
290 FOR D = 1 TO 3000
300   FOR J = 1 TO 1000: NEXT J
310 NEXT D
320 REM Done
330 SCREEN 0
340 PRINT "Graphics test complete!"
350 END
