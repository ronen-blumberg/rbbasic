REM QB64 Mouse and Keyboard Demo
REM Tests _MOUSEX, _MOUSEY, _MOUSEBUTTON, _KEYDOWN

SCREEN 12

frame = 0
DO
    REM Clear screen
    LINE (0, 0)-(639, 479), 0, BF

    REM Get mouse position
    mx = _MOUSEX
    my = _MOUSEY

    REM Draw crosshair at mouse position
    LINE (mx - 10, my)-(mx + 10, my), 15
    LINE (mx, my - 10)-(mx, my + 10), 15

    REM Check mouse buttons
    lb = _MOUSEBUTTON(1)
    rb = _MOUSEBUTTON(2)

    REM Draw circle if left button pressed
    IF lb THEN
        CIRCLE (mx, my), 15, 12
        PAINT (mx, my), 12, 12
    END IF

    REM Draw square if right button pressed
    IF rb THEN
        LINE (mx - 10, my - 10)-(mx + 10, my + 10), 10, BF
    END IF

    REM Display info
    _PRINTSTRING (10, 10), "Mouse: " + STR$(mx) + "," + STR$(my)
    _PRINTSTRING (10, 30), "Click to draw!"

    _DISPLAY
    _LIMIT 60

    frame = frame + 1
LOOP UNTIL frame > 180

END
