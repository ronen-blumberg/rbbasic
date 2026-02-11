10 REM Graphics Demo - Requires SDL2
20 REM Simple graphics demonstration for RB BASIC
30 REM Make sure SDL2 is installed in lib\SDL2-mingw32\
40 SCREEN 12
50 CLS
60 RANDOMIZE
70 PRINT "RB BASIC Graphics Demo"
80 PRINT "Get Ready..."
90 SLEEP 5
100 CLS
110 REM Draw some random pixels
120 FOR I = 1 TO 500
130   X = INT(RND * 640)
140   Y = INT(RND * 480)
150   C = INT(RND * 16)
160   PSET (X, Y), C
170 NEXT I
180 SLEEP 5
190 CLS
200 REM Draw some lines
210 FOR I = 1 TO 20
220   X1 = INT(RND * 640)
230   Y1 = INT(RND * 480)
240   X2 = INT(RND * 640)
250   Y2 = INT(RND * 480)
260   C = INT(RND * 16)
270   LINE (X1, Y1)-(X2, Y2), C
280 NEXT I
290 SLEEP 5
300 CLS
310 REM Draw some circles
320 FOR I = 1 TO 15
330   X = INT(RND * 640)
340   Y = INT(RND * 480)
350   R = INT(RND * 50) + 10
360   C = INT(RND * 16)
370   CIRCLE (X, Y), R, C
380 NEXT I
390 SLEEP 5
400 CLS
410 REM Draw boxes
420 FOR I = 1 TO 10
430   X1 = INT(RND * 580)
440   Y1 = INT(RND * 430)
450   X2 = X1 + INT(RND * 50) + 10
460   Y2 = Y1 + INT(RND * 50) + 10
470   C = INT(RND * 16)
480   LINE (X1, Y1)-(X2, Y2), C, B
490 NEXT I
500 SLEEP 5
510 CLS
520 REM Draw filled boxes
530 FOR I = 1 TO 10
540   X1 = INT(RND * 580)
550   Y1 = INT(RND * 430)
560   X2 = X1 + INT(RND * 50) + 10
570   Y2 = Y1 + INT(RND * 50) + 10
580   C = INT(RND * 16)
590   LINE (X1, Y1)-(X2, Y2), C, BF
600 NEXT I
610 SLEEP 5
620 CLS
630 REM Simple animation
640 X = 320
650 Y = 240
660 DX = 3
670 DY = 2
680 FOR I = 1 TO 200
690   REM Erase old circle
700   CIRCLE (X, Y), 20, 0
710   REM Move
720   X = X + DX
730   Y = Y + DY
740   REM Bounce off edges
750   IF X < 20 OR X > 620 THEN DX = -DX
760   IF Y < 20 OR Y > 460 THEN DY = -DY
770   REM Draw new circle
780   CIRCLE (X, Y), 20, 14
790   REM Small delay (draw and erase quickly)
800   SLEEP 0.05
810 NEXT I
820 REM 
830 LOCATE 1, 1
840 PRINT "Graphics demo complete!"
850 SLEEP 5
860 SCREEN 0
870 END
