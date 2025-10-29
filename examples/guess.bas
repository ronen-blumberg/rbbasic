10 REM Number Guessing Game
20 REM Classic BASIC game for RB BASIC
30 CLS
40 RANDOMIZE
50 SECRET = INT(RND * 100) + 1
60 TRIES = 0
70 MAXGUESSES = 7
80 PRINT "================================"
90 PRINT "   NUMBER GUESSING GAME"
100 PRINT "================================"
110 PRINT
120 PRINT "I'm thinking of a number"
130 PRINT "between 1 and 100."
140 PRINT
150 PRINT "You have"; MAXGUESSES; "guesses."
160 PRINT
170 REM Main game loop
180 WHILE TRIES < MAXGUESSES
190   TRIES = TRIES + 1
200   PRINT "Guess #"; TRIES; ": ";
210   INPUT GUESS
220   IF GUESS < 1 OR GUESS > 100 THEN
230     PRINT "Please guess between 1 and 100!"
240     TRIES = TRIES - 1
250     GOTO 180
260   END IF
270   IF GUESS = SECRET THEN GOTO 400
280   IF GUESS < SECRET THEN
290     PRINT "Too low!"
300   ELSE
310     PRINT "Too high!"
320   END IF
330   REMAINING = MAXGUESSES - TRIES
340   IF REMAINING > 0 THEN
350     PRINT "You have"; REMAINING; "guesses left."
360   END IF
370   PRINT
380 WEND
390 GOTO 500
400 REM Player won!
410 PRINT
420 PRINT "*** CORRECT! ***"
430 PRINT
440 PRINT "You guessed the number in"; TRIES; "tries!"
450 IF TRIES = 1 THEN PRINT "Amazing! First try!"
460 IF TRIES <= 3 THEN PRINT "Excellent guessing!"
470 IF TRIES <= 5 THEN PRINT "Good job!"
480 GOTO 600
490 REM Player lost
500 PRINT
510 PRINT "Sorry, you're out of guesses!"
520 PRINT "The number was:"; SECRET
530 PRINT
540 REM Play again?
600 PRINT
610 PRINT "Play again? (Y/N) ";
620 A$ = INPUT$(1)
630 PRINT
640 IF UCASE$(A$) = "Y" THEN GOTO 30
650 PRINT "Thanks for playing!"
660 END
