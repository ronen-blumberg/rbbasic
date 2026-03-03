REM QB64 Image Management Demo
REM Tests _NEWIMAGE, _LOADIMAGE, _PUTIMAGE, _FREEIMAGE

SCREEN 12

REM Create a new offscreen image
img = _NEWIMAGE(200, 150)
_PRINTSTRING (10, 10), "Created image handle: " + STR$(img)

REM Free it
IF img > 0 THEN
    _FREEIMAGE img
    _PRINTSTRING (10, 30), "Image freed"
END IF

REM Try to load an image file
pic = _LOADIMAGE("test.bmp")
IF pic > 0 THEN
    _PUTIMAGE (100, 100), pic
    _PRINTSTRING (10, 50), "Image loaded and displayed!"
    _FREEIMAGE pic
ELSE
    _PRINTSTRING (10, 50), "No image file (OK - testing compilation)"
END IF

_PRINTSTRING (10, 80), "Image test complete"
_DISPLAY

SLEEP 2
END
