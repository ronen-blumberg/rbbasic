10 REM Graphics Demo - Auto-running version
20 REM No keyboard input required - just watch!
30 SCREEN 12
40 CLS
50 RANDOMIZE
60 REM Section 1: Random pixels
70 FOR I = 1 TO 500
80   X = INT(RND * 640)
90   Y = INT(RND * 480)
100   C = INT(RND * 16)
110   PSET (X, Y), C
120 NEXT I
130 REM Small delay
140 SLEEP 3
170 CLS
180 REM Section 2: Random lines
190 FOR I = 1 TO 20
200   X1 = INT(RND * 640)
210   Y1 = INT(RND * 480)
220   X2 = INT(RND * 640)
230   Y2 = INT(RND * 480)
240   C = INT(RND * 16)
250   LINE (X1, Y1)-(X2, Y2), C
260 NEXT I
270 REM Small delay
280 SLEEP 3
310 CLS
320 REM Section 3: Random circles
330 FOR I = 1 TO 15
340   X = INT(RND * 640)
350   Y = INT(RND * 480)
360   R = INT(RND * 50) + 10
370   C = INT(RND * 16)
380   CIRCLE (X, Y), R, C
390 NEXT I
400 REM Small delay
410 SLEEP 3
440 CLS
450 REM Section 4: Boxes
460 FOR I = 1 TO 10
470   X1 = INT(RND * 580)
480   Y1 = INT(RND * 430)
490   X2 = X1 + INT(RND * 50) + 10
500   Y2 = Y1 + INT(RND * 50) + 10
510   C = INT(RND * 16)
520   LINE (X1, Y1)-(X2, Y2), C, B
530 NEXT I
540 REM Small delay
550 SLEEP 3
580 CLS
590 REM Section 5: Filled boxes
600 FOR I = 1 TO 10
610   X1 = INT(RND * 580)
620   Y1 = INT(RND * 430)
630   X2 = X1 + INT(RND * 50) + 10
640   Y2 = Y1 + INT(RND * 50) + 10
650   C = INT(RND * 16)
660   LINE (X1, Y1)-(X2, Y2), C, BF
670 NEXT I
680 REM Small delay
690 SLEEP 3
720 CLS
730 REM Section 6: Simple animation - bouncing ball
740 X = 320
750 Y = 240
760 DX = 3
770 DY = 2
780 FOR I = 1 TO 200
790   REM Erase old circle
800   CIRCLE (X, Y), 20, 0
810   REM Move
820   X = X + DX
830   Y = Y + DY
840   REM Bounce off edges
850   IF X < 20 OR X > 620 THEN DX = -DX
860   IF Y < 20 OR Y > 460 THEN DY = -DY
870   REM Draw new circle
880   CIRCLE (X, Y), 20, 14
890   REM Small delay
900   SLEEP 0.05
910 NEXT I
920 REM End - close graphics and return to text mode
930 SCREEN 0
940 PRINT "Graphics demo complete!"
950 END
