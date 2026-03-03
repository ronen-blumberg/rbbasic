REM QB64 Extensions Demo
REM Tests _DISPLAY, _LIMIT, _RGB, _RGB32, _NEWIMAGE
REM _PUTIMAGE, _FREEIMAGE, _PRINTSTRING, _MOUSEX, _MOUSEY
REM _MOUSEBUTTON, _KEYDOWN

SCREEN 12

REM Draw colored rectangles using palette
FOR i = 0 TO 15
    LINE (i * 40, 0)-(i * 40 + 39, 30), i, BF
NEXT i

REM Print text on graphics screen
_PRINTSTRING (10, 50), "QB64 Extensions Demo"
_PRINTSTRING (10, 70), "Press ESC to exit"

REM Test _RGB function (returns packed color value)
c1 = _RGB(255, 0, 0)
c2 = _RGB(0, 255, 0)
c3 = _RGB32(0, 0, 255, 128)

REM Test _NEWIMAGE - create offscreen image
img = _NEWIMAGE(100, 100)

REM Test _FREEIMAGE
IF img > 0 THEN
    _FREEIMAGE img
END IF

REM Game loop with _DISPLAY and _LIMIT
frame = 0

DO
    REM Clear area for animation
    LINE (0, 100)-(639, 479), 0, BF

    REM Draw bouncing ball
    bx = 320 + INT(SIN(frame / 20.0) * 200)
    by = 300 + INT(COS(frame / 15.0) * 100)
    CIRCLE (bx, by), 20, 14
    PAINT (bx, by), 14, 14

    REM Show frame count
    _PRINTSTRING (10, 110), "Frame: " + STR$(frame)

    REM Manual display update
    _DISPLAY

    REM Cap at 30 FPS
    _LIMIT 30

    frame = frame + 1
LOOP UNTIL frame > 90

REM Show final frame
_PRINTSTRING (200, 240), "Demo Complete!"
_DISPLAY

SLEEP 2
END
