REM QB64 Sound Demo
REM Tests _SNDOPEN, _SNDPLAY, _SNDSTOP, _SNDVOL

SCREEN 12

REM Try to load a WAV file
snd = _SNDOPEN("test.wav")

IF snd > 0 THEN
    _PRINTSTRING (10, 10), "Sound loaded!"
    _SNDVOL snd, 0.5
    _SNDPLAY snd
    SLEEP 2
    _SNDSTOP snd
ELSE
    _PRINTSTRING (10, 10), "No sound file (OK - testing compilation)"
END IF

_PRINTSTRING (10, 50), "Sound test complete"
_DISPLAY
SLEEP 2
END
