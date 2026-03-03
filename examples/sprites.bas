REM GET/PUT sprite demo
SCREEN 12

REM Draw a small sprite
LINE (10, 10)-(30, 30), 14, BF
CIRCLE (20, 20), 8, 12

REM Capture sprite into array
DIM sprite(500)
GET (10, 10)-(30, 30), sprite

REM Draw captured sprite at multiple locations
PUT (100, 100), sprite, PSET
PUT (200, 100), sprite, PSET
PUT (150, 200), sprite, XOR

REM Wait for user to close
SLEEP 0
END
