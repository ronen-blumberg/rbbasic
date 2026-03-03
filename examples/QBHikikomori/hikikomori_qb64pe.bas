' HIKIKOMORI GAME - QB64PE VERSION (PORTABLE)

'===============================================
' $EMBED DIRECTIVES FOR PORTABILITY
' These must be the first lines seen by the compiler
'===============================================
$EMBED "data/ending_titles.txt"
$EMBED "data/poemDB.txt"
$EMBED "data/news.txt"
$EMBED "data/warning.txt"
$EMBED "data/startpoint.txt"
$EMBED "data/chat1.txt"
$EMBED "data/yuka_guide.txt"
$EMBED "data/mike.txt"
$EMBED "data/ken.txt"
$EMBED "data/email1.txt"
$EMBED "data/email2.txt"
$EMBED "data/email3.txt"
$EMBED "data/guitar1.txt"
$EMBED "data/guitar2.txt"
$EMBED "data/guitar3.txt"
$EMBED "data/pray2.txt"
$EMBED "data/guitar4.txt"
$EMBED "data/dream.txt"
$EMBED "data/dream2.txt"
$EMBED "data/dream3.txt"
$EMBED "data/nightmare1.txt"
$EMBED "data/eddie.txt"
$EMBED "data/dream_ta.txt"
$EMBED "data/johnny.txt"
$EMBED "data/music.txt"
$EMBED "data/music3.txt"
$EMBED "data/walk1.txt"
$EMBED "data/yuka.txt"
$EMBED "data/meeting1.txt"
$EMBED "data/meeting2.txt"
$EMBED "data/meeting3.txt"
$EMBED "data/meeting4.txt"
$EMBED "data/workshop.txt"
$EMBED "sound/dream1.wav"
$EMBED "sound/dream2.wav"
$EMBED "sound/walk1.wav"
$EMBED "sound/eddie.wav"
$EMBED "sound/dream_eva.wav"
$EMBED "sound/email1.wav"
$EMBED "sound/pray_song.wav"
$EMBED "sound/love_song.wav"
$EMBED "sound/blues_song.wav"
$EMBED "sound/pray_song2.wav"
$EMBED "sound/guitar_song.wav"
$EMBED "sound/fbsloop44.wav"
$EMBED "sound/legends.ogg"
$EMBED "sound/sabrina.wav"
$EMBED "imgs/hikpic.bmp"
$EMBED "fonts/Montserrat-Bold.ttf"
'===============================================

_TITLE "HIKIKOMORI GAME"
RANDOMIZE TIMER

DIM SHARED AS INTEGER timePass, meetingCounter
REDIM SHARED newsbla(0) AS STRING
DIM SHARED counter AS INTEGER
DIM SHARED songcounter AS INTEGER
REDIM SHARED places(0) AS STRING
REDIM SHARED colors(0) AS STRING
REDIM SHARED metaphore(0) AS STRING
REDIM SHARED objects(0) AS STRING
REDIM SHARED good(0) AS STRING
REDIM SHARED bad(0) AS STRING
DIM SHARED finished AS INTEGER ' QB64 uses INTEGER for boolean

' Person UDT - converted from FreeBASIC TYPE with methods
TYPE PERSON
    intext AS STRING
    Greeting AS STRING
    You AS STRING
    Script AS STRING
    kCnt AS INTEGER
    rCnt AS INTEGER
    wCnt AS INTEGER
    NoKeyFoundIndex AS INTEGER
END TYPE

' Arrays for each PERSON instance (we'll use indices to track which person)
CONST MAX_PERSONS = 4
CONST MAX_KEYWORDS = 100
CONST MAX_REPLIES = 200
CONST MAX_WORDS = 50

DIM SHARED personKeywords(1 TO MAX_PERSONS, 1 TO MAX_KEYWORDS) AS STRING
DIM SHARED personReplies(1 TO MAX_PERSONS, 1 TO MAX_REPLIES) AS STRING
DIM SHARED personWordIn(1 TO MAX_PERSONS, 1 TO MAX_WORDS) AS STRING
DIM SHARED personWordOut(1 TO MAX_PERSONS, 1 TO MAX_WORDS) AS STRING
DIM SHARED personRStarts(1 TO MAX_PERSONS, 1 TO MAX_KEYWORDS) AS INTEGER
DIM SHARED personREnds(1 TO MAX_PERSONS, 1 TO MAX_KEYWORDS) AS INTEGER
DIM SHARED personRIndex(1 TO MAX_PERSONS, 1 TO MAX_KEYWORDS) AS INTEGER
DIM SHARED personID AS INTEGER ' Current person ID being loaded
DIM SHARED currentSound AS LONG ' Add this with your other SHARED variables at the top

CONST PUNCTUATION = "?!,.:;<>(){}[]"

' File paths (kept for convenience in _EMBEDDED$ calls)
CONST SOUND_PATH = "sound/"
CONST DATA_PATH = "data/"
CONST IMG_PATH = "imgs/"

' Forward declarations handled by QB64 automatically

'===============================================
' MAIN PROGRAM
'===============================================

warningscreen
opening
main
upper "ending_titles.txt"
SYSTEM


'===============================================
' UTILITY FUNCTIONS
'===============================================

SUB sAppend (arr() AS STRING, Item AS STRING)
    REDIM _PRESERVE arr(LBOUND(arr) TO UBOUND(arr) + 1) AS STRING
    arr(UBOUND(arr)) = Item
END SUB

SUB nAppend (arr() AS INTEGER, Item AS INTEGER)
    REDIM _PRESERVE arr(LBOUND(arr) TO UBOUND(arr) + 1) AS INTEGER
    arr(UBOUND(arr)) = Item
END SUB

FUNCTION rnd_range# (first AS DOUBLE, last AS DOUBLE)
    rnd_range# = RND * (last - first) + first
END FUNCTION

' ** NEW HELPER FUNCTION TO READ LINES FROM EMBEDDED STRING **
FUNCTION NextLine$ (content AS STRING)
    DIM pos AS LONG
    DIM result AS STRING
    
    IF content = "" THEN
        NextLine$ = ""
        EXIT FUNCTION
    END IF
    
    ' Find next Line Feed (CHR$(10))
    pos = INSTR(content, CHR$(10))
    
    IF pos > 0 THEN
        result = LEFT$(content, pos - 1)
        content = MID$(content, pos + 1)
    ELSE
        ' Last line
        result = content
        content = ""
    END IF
    
    ' Handle carriage return (CHR$(13)) if present
    IF RIGHT$(result, 1) = CHR$(13) THEN result = LEFT$(result, LEN(result) - 1)
    
    NextLine$ = result
END FUNCTION

'===============================================
' PERSON-RELATED SUBS (converted from TYPE methods)
'===============================================

SUB PersonRestart (p AS PERSON, pid AS INTEGER)
    p.kCnt = 0
    p.rCnt = 0
    p.wCnt = 0
    p.NoKeyFoundIndex = 0
    p.Greeting = ""
    p.You = ""
    p.Script = ""
    ' Clear arrays for this person
    FOR i = 1 TO MAX_KEYWORDS
        personKeywords(pid, i) = ""
        personRStarts(pid, i) = 0
        personREnds(pid, i) = 0
        personRIndex(pid, i) = 0
    NEXT
    FOR i = 1 TO MAX_REPLIES
        personReplies(pid, i) = ""
    NEXT
    FOR i = 1 TO MAX_WORDS
        personWordIn(pid, i) = ""
        personWordOut(pid, i) = ""
    NEXT
END SUB

' ** MODIFIED: Now uses _EMBEDDED$ and NextLine$ **
SUB PersonLoadArrays (p AS PERSON, scriptFile AS STRING, pid AS INTEGER)
    DIM content AS STRING ' New variable for embedded content
    DIM startR AS INTEGER, endR AS INTEGER, ReadingR AS INTEGER, temp AS INTEGER
    DIM fline AS STRING, kWord AS STRING

    content = _EMBEDDED$(DATA_PATH + scriptFile) ' Get embedded content
    
    DO ' Start loop to process lines from the content string
        fline = NextLine$(content)
        IF fline = "" THEN EXIT DO ' Replaces WHILE NOT EOF(h)

        SELECT CASE LEFT$(fline, 2)
            CASE "g:"
                p.Greeting = LTRIM$(RTRIM$(MID$(fline, 3)))
            CASE "y:"
                p.You = LTRIM$(RTRIM$(MID$(fline, 3)))
            CASE "c:"
                p.Script = LTRIM$(RTRIM$(MID$(fline, 3)))
            CASE "s:"
                p.wCnt = p.wCnt + 1
                temp = INSTR(fline, ">")
                IF temp THEN
                    personWordIn(pid, p.wCnt) = " " + LTRIM$(RTRIM$(MID$(fline, 3, temp - 3))) + " "
                    personWordOut(pid, p.wCnt) = " " + LTRIM$(RTRIM$(MID$(fline, temp + 1))) + " "
                END IF
            CASE "r:"
                p.rCnt = p.rCnt + 1
                personReplies(pid, p.rCnt) = LTRIM$(RTRIM$(MID$(fline, 3)))
                IF NOT ReadingR THEN
                    ReadingR = -1
                    startR = p.rCnt
                END IF
            CASE "k:"
                IF ReadingR THEN
                    endR = p.rCnt
                    ReadingR = 0
                END IF
                IF p.rCnt THEN
                    p.kCnt = p.kCnt + 1
                    kWord = LTRIM$(RTRIM$(MID$(fline, 3)))
                    personKeywords(pid, p.kCnt) = " " + kWord + " "
                    personRStarts(pid, p.kCnt) = startR
                    personRIndex(pid, p.kCnt) = startR
                    personREnds(pid, p.kCnt) = endR
                    IF kWord = "nokeyfound" THEN p.NoKeyFoundIndex = p.kCnt
                END IF
            CASE "e:"
                EXIT DO 
        END SELECT
    LOOP 

    IF ReadingR THEN 'handle last bits
        endR = p.rCnt
        p.kCnt = p.kCnt + 1
        personKeywords(pid, p.kCnt) = "nokeyfound"
        personRStarts(pid, p.kCnt) = startR
        personRIndex(pid, p.kCnt) = startR
        personREnds(pid, p.kCnt) = endR
        p.NoKeyFoundIndex = p.kCnt
    END IF
END SUB

FUNCTION isolatePunctuation$ (s AS STRING)
    DIM b AS STRING, i AS INTEGER
    b = ""
    FOR i = 1 TO LEN(s)
        IF INSTR(PUNCTUATION, MID$(s, i, 1)) > 0 THEN
            b = b + " " + MID$(s, i, 1) + " "
        ELSE
            b = b + MID$(s, i, 1)
        END IF
    NEXT
    isolatePunctuation$ = b
END FUNCTION

FUNCTION joinPunctuation$ (s AS STRING)
    DIM b AS STRING, find AS STRING, i AS INTEGER, place AS INTEGER
    b = s
    FOR i = 1 TO LEN(PUNCTUATION)
        find = " " + MID$(PUNCTUATION, i, 1) + " "
        place = INSTR(b, find)
        WHILE place > 0
            IF place = 1 THEN
                b = MID$(PUNCTUATION, i, 1) + MID$(b, place + 3)
            ELSE
                b = MID$(b, 1, place - 1) + MID$(PUNCTUATION, i, 1) + MID$(b, place + 3)
            END IF
            place = INSTR(b, find)
        WEND
    NEXT
    joinPunctuation$ = b
END FUNCTION

FUNCTION PersonGetReply$ (p AS PERSON, rply2 AS STRING, switch AS INTEGER, pid AS INTEGER)
    DIM inpt AS STRING, tail AS STRING, answ AS STRING
    DIM kFlag AS INTEGER, k AS INTEGER, kFound AS INTEGER, l AS INTEGER, w AS INTEGER

    IF switch = 0 THEN
        'USER INPUT SECTION
        PRINT p.You + ": ";
        LINE INPUT "", inpt
        IF LCASE$(inpt) = "q" OR LCASE$(inpt) = "x" OR LCASE$(inpt) = "goodbye" OR LCASE$(inpt) = "good night" OR LCASE$(inpt) = "bye" THEN
            PersonGetReply$ = "Goodbye!"
            EXIT FUNCTION
        END IF
    ELSE
        inpt = rply2
    END IF

    inpt = " " + inpt + " "
    inpt = isolatePunctuation$(inpt)

    FOR k = 1 TO p.kCnt
        kFound = INSTR(LCASE$(inpt), LCASE$(personKeywords(pid, k)))
        IF kFound > 0 THEN
            tail = " " + MID$(inpt, kFound + LEN(personKeywords(pid, k)))
            FOR l = 1 TO LEN(tail)
                FOR w = 1 TO p.wCnt
                    IF LCASE$(MID$(tail, l, LEN(personWordIn(pid, w)))) = LCASE$(personWordIn(pid, w)) THEN
                        tail = MID$(tail, 1, l - 1) + personWordOut(pid, w) + MID$(tail, l + LEN(personWordIn(pid, w)))
                        EXIT FOR
                    END IF
                NEXT w
            NEXT l
            kFlag = -1
            EXIT FOR
        END IF
    NEXT

    IF kFlag = 0 THEN k = p.NoKeyFoundIndex

    answ = personReplies(pid, INT((personREnds(pid, k) - personRStarts(pid, k) + 1) * RND) + personRStarts(pid, k))

    IF k = p.NoKeyFoundIndex THEN
        personRIndex(pid, k) = INT((personREnds(pid, k) - personRStarts(pid, k) + 1) * RND) + personRStarts(pid, k)
    END IF

    IF RIGHT$(answ, 1) <> "*" THEN
        PersonGetReply$ = answ
        EXIT FUNCTION
    END IF

    IF LTRIM$(RTRIM$(tail)) = "" THEN
        PersonGetReply$ = "Please elaborate on, " + personKeywords(pid, k)
    ELSE
        tail = joinPunctuation$(tail)
        PersonGetReply$ = MID$(answ, 1, LEN(answ) - 1) + tail
    END IF
END FUNCTION

SUB slow (Text AS STRING)
    DIM speed(0 TO 4) AS INTEGER, i as INTEGER
    speed(0) = 50: speed(1) = 100: speed(2) = 20: speed(3) = 300: speed(4) = 250
    FOR i = 1 TO LEN(Text)
        PRINT MID$(Text, i, 1);
        _DELAY speed(INT(RND * 5)) / 1000
    NEXT
END SUB

SUB PersonSpeakToText (p AS PERSON, lines AS STRING)
    PRINT p.Script + ": ";
    slow lines
    PRINT
END SUB

'===============================================
' SOUND FUNCTIONS (MODIFIED for _EMBEDDED$)
'===============================================

' ** MODIFIED: Now uses _SNDOPEN with _EMBEDDED$ data **
SUB StartSound (f AS STRING)
    ' Stop any currently playing sound first
    IF currentSound THEN
        IF _SNDPLAYING(currentSound) THEN _SNDSTOP currentSound
        _SNDCLOSE currentSound
        currentSound = 0
    END IF
    
    DIM soundData AS STRING
    soundData = _EMBEDDED$(SOUND_PATH + f)

    ' Start new sound
    currentSound = _SNDOPEN(soundData) ' Use the sound data string
    IF currentSound THEN
        _SNDLOOP currentSound ' Loop so it keeps playing until stopped
    END IF
END SUB

SUB StopSound
    IF currentSound THEN
        IF _SNDPLAYING(currentSound) THEN _SNDSTOP currentSound
        _SNDCLOSE currentSound
        currentSound = 0
    END IF
END SUB

' ** MODIFIED: Now uses _SNDOPEN with _EMBEDDED$ data **
SUB PlaySound (f AS STRING, waitTime AS INTEGER)
    DIM snd AS LONG
    DIM soundData AS STRING
    soundData = _EMBEDDED$(SOUND_PATH + f)

    snd = _SNDOPEN(soundData) ' Use the sound data string
    
    IF snd THEN
        _SNDPLAY snd
        IF waitTime > 0 THEN
            _DELAY waitTime
        ELSE
            DO WHILE _SNDPLAYING(snd)
                _LIMIT 30
            LOOP
        END IF
        _SNDCLOSE snd
    END IF
END SUB

' ** MODIFIED: Now uses _SNDOPEN with _EMBEDDED$ data **
SUB PlaySoundAsync (f AS STRING)
    DIM snd AS LONG
    DIM soundData AS STRING
    soundData = _EMBEDDED$(SOUND_PATH + f)
    
    snd = _SNDOPEN(soundData) ' Use the sound data string
    
    IF snd THEN
        _SNDPLAY snd
    END IF
END SUB

'===============================================
' GAME FUNCTIONS
'===============================================

SUB conversation (file AS STRING)
    DIM i AS INTEGER
    DIM kenzu AS PERSON
    PersonRestart kenzu, 1

    CLS
    FOR i = 1 TO 10
        LOCATE 5, 5
        PRINT "           ";
        _DELAY 0.25
        LOCATE 5, 5
        PRINT "connecting...";
        _DELAY 0.25
    NEXT
    CLS

    DIM rply AS STRING
    DIM rply3 AS STRING
    PersonLoadArrays kenzu, file, 1
    PRINT kenzu.Greeting: PRINT

    DO
        rply = PersonGetReply$(kenzu, rply3, 0, 1)
        PRINT
        PersonSpeakToText kenzu, rply
        PRINT
    LOOP UNTIL rply = "Goodbye!"

    CLS
    LOCATE 5, 5
    PRINT "disconnected...  "
    SLEEP
END SUB

SUB chatroom
    DIM i AS INTEGER
    DIM guide AS PERSON
    DIM mike AS PERSON
    DIM ken AS PERSON

    PersonRestart guide, 1
    PersonRestart mike, 2
    PersonRestart ken, 3

    CLS
    FOR i = 1 TO 10
        LOCATE 5, 5
        PRINT "           ";
        _DELAY 0.25
        LOCATE 5, 5
        PRINT "connecting...";
        _DELAY 0.25
    NEXT
    CLS

    DIM inText AS STRING
    DIM rply AS STRING
    DIM rply2 AS STRING
    DIM rply3 AS STRING

    PersonLoadArrays guide, "yuka_guide.txt", 1
    PersonLoadArrays mike, "mike.txt", 2
    PersonLoadArrays ken, "ken.txt", 3

    PRINT guide.Greeting: PRINT

    DO
        PRINT: PRINT guide.You + ": ";
        LINE INPUT "", inText
        IF LCASE$(inText) = "q" OR LCASE$(inText) = "x" OR LCASE$(inText) = "goodbye" OR LCASE$(inText) = "good night" OR LCASE$(inText) = "bye" THEN
            PRINT: PRINT
            PersonSpeakToText guide, "Goodbye!"
            EXIT DO
        END IF
        rply = PersonGetReply$(guide, inText, 1, 1)
        PRINT: PersonSpeakToText guide, rply
        rply2 = PersonGetReply$(mike, inText, 1, 2)
        PRINT: PersonSpeakToText mike, rply2
        rply3 = PersonGetReply$(ken, inText, 1, 3)
        PRINT: PersonSpeakToText ken, rply3
    LOOP UNTIL inText = "bye"

    CLS
    LOCATE 5, 5
    PRINT "disconnected...  "
    SLEEP
END SUB

SUB cp (Row AS INTEGER, s AS STRING)
    LOCATE Row, (100 - LEN(s)) / 2
    PRINT s
END SUB

FUNCTION GetKeys$ (keysToCatch$)
    DIM k$
    DO
        k$ = INKEY$
        WHILE LEN(k$) = 0
            k$ = INKEY$
            _LIMIT 60
        WEND
    LOOP UNTIL INSTR(keysToCatch$, k$)
    GetKeys$ = k$
END FUNCTION

FUNCTION dates1$ (months AS INTEGER)
    DIM monthNames(0 TO 11) AS STRING
    monthNames(0) = "January"
    monthNames(1) = "February"
    monthNames(2) = "March"
    monthNames(3) = "April"
    monthNames(4) = "May"
    monthNames(5) = "June"
    monthNames(6) = "July"
    monthNames(7) = "August"
    monthNames(8) = "September"
    monthNames(9) = "October"
    monthNames(10) = "November"
    monthNames(11) = "December"

    DIM m AS INTEGER
    m = (months + 11) MOD 12

    dates1$ = monthNames(m) + "," + STR$(INT((months - 1) / 12) + 1997)
END FUNCTION

' ** MODIFIED: Now uses _EMBEDDED$ and NextLine$ **
SUB LoadPoemDB (filename AS STRING)
    DIM content AS STRING ' New variable for embedded content
    DIM fline AS STRING

    content = _EMBEDDED$(DATA_PATH + filename)

    DO
        fline = NextLine$(content)
        IF fline = "" THEN EXIT DO ' Replaces WHILE NOT EOF(h)

        SELECT CASE LEFT$(fline, 2)
            CASE "p:": sAppend places(), LTRIM$(RTRIM$(MID$(fline, 3)))
            CASE "c:": sAppend colors(), LTRIM$(RTRIM$(MID$(fline, 3)))
            CASE "m:": sAppend metaphore(), LTRIM$(RTRIM$(MID$(fline, 3)))
            CASE "o:": sAppend objects(), LTRIM$(RTRIM$(MID$(fline, 3)))
            CASE "g:": sAppend good(), LTRIM$(RTRIM$(MID$(fline, 3)))
            CASE "b:": sAppend bad(), LTRIM$(RTRIM$(MID$(fline, 3)))
            CASE "e:": EXIT DO ' Replaces EXIT WHILE
        END SELECT
    LOOP
END SUB

SUB RestartPoem
    REDIM places(0) AS STRING
    REDIM colors(0) AS STRING
    REDIM metaphore(0) AS STRING
    REDIM objects(0) AS STRING
    REDIM good(0) AS STRING
    REDIM bad(0) AS STRING
END SUB

' ** MODIFIED: Now uses _EMBEDDED$ and NextLine$ **
SUB upper (f AS STRING)
    CLS
    REDIM lines(0) AS STRING
    DIM fline AS STRING
    DIM hi AS INTEGER
    DIM i AS INTEGER
    DIM content AS STRING
    
    content = _EMBEDDED$(DATA_PATH + f)
    
    DO
        fline = NextLine$(content)
        IF fline = "" THEN EXIT DO
        sAppend lines(), fline
    LOOP
    
    ' Calculate number of text rows (pixel height / font height)
    hi = _HEIGHT \ _FONTHEIGHT
    
    ' Fill screen with blank lines to position cursor at bottom
    FOR i = 1 TO hi - 1
        PRINT
    NEXT
    
    'Print closing credits (each PRINT scrolls screen up)
    FOR i = 0 TO UBOUND(lines)
        PRINT SPACE$(9); lines(i) ' 9 spaces to indent (like column 10)
        _DELAY 0.5
    NEXT
    
    'Clear screen by scrolling all text off
    FOR i = 1 TO hi
        PRINT
        _DELAY 0.5
    NEXT
    
    PRINT "END"
    SLEEP
END SUB

' ** MODIFIED: Now uses _EMBEDDED$ and NextLine$ **
SUB news
    DIM l AS STRING, k AS STRING
    DIM content AS STRING ' New variable for embedded content

    CLS
    content = _EMBEDDED$(DATA_PATH + "news.txt")

    DO
        l = NextLine$(content)
        IF l = "" THEN EXIT DO
        sAppend newsbla(), l
    LOOP

    PRINT "PRESS 'q' OR ESC TO STOP"

    DO
        IF UBOUND(newsbla) > 0 THEN
            PRINT newsbla(INT(RND * UBOUND(newsbla)) + 1)
        END IF
        PRINT
        _DELAY 3
        k = INKEY$
    LOOP UNTIL k = CHR$(27) OR k = "q"
END SUB

' ** MODIFIED: Now uses _EMBEDDED$ for full file content **
SUB txtfile (f AS STRING)
    CLS
    DIM buffer AS STRING
    
    ' Use _EMBEDDED$ to get the entire file content as a string
    buffer = _EMBEDDED$(DATA_PATH + f)
    
    IF buffer = "" THEN
        PRINT "file could not be opened!"
        EXIT SUB
    END IF
    
    ' Remove CR characters (ASCII 13) to fix double line spacing
    DIM p AS LONG
    DO
        p = INSTR(buffer, CHR$(13))
        IF p > 0 THEN
            buffer = LEFT$(buffer, p - 1) + MID$(buffer, p + 1)
        END IF
    LOOP WHILE p > 0
    PRINT buffer
END SUB

SUB poem
    DIM dreamMusic(0 TO 4) AS STRING, i as INTEGER
    dreamMusic(0) = "dream1.wav"
    dreamMusic(1) = "dream2.wav"
    dreamMusic(2) = "walk1.wav"
    dreamMusic(3) = "eddie.wav"
    dreamMusic(4) = "dream_eva.wav"

    CLS
    RestartPoem
    LoadPoemDB "poemDB.txt"

    cp 1, "A POEM..."
    FOR i = 1 TO 3
        PRINT
        IF UBOUND(places) > 0 THEN PRINT places(INT(RND * UBOUND(places)) + 1)
        IF UBOUND(colors) > 0 THEN PRINT colors(INT(RND * UBOUND(colors)) + 1)
        IF UBOUND(metaphore) > 0 THEN PRINT metaphore(INT(RND * UBOUND(metaphore)) + 1)
        IF UBOUND(objects) > 0 THEN PRINT objects(INT(RND * UBOUND(objects)) + 1)
        IF UBOUND(good) > 0 THEN PRINT good(INT(RND * UBOUND(good)) + 1)
        IF UBOUND(bad) > 0 THEN PRINT bad(INT(RND * UBOUND(bad)) + 1)
    NEXT

    StartSound dreamMusic(INT(RND * 5))
    SLEEP
    StopSound
END SUB

SUB email
    DIM k AS STRING

    CLS
    IF counter = 0 THEN
        cp 5, "NO EMAIL..."
        counter = counter + 1
    ELSEIF counter = 1 THEN
        txtfile "email1.txt"
        StartSound "email1.wav"

        PRINT
        PRINT
        PRINT "1. REPLY OR 2. IGNORE"
        k = getKeys$("12")
        IF k = "2" THEN
            EXIT SUB
        ELSEIF k = "1" THEN
            PRINT
            PRINT
            PRINT "YOU REPLY TO YUKA-YUKA CENTER"
            counter = counter + 1
        END IF
    ELSEIF counter = 2 THEN
        txtfile "email2.txt"
        StartSound "email1.wav"
    ELSEIF counter = 3 THEN
        txtfile "email3.txt"
        StartSound "email1.wav"
    END IF
    SLEEP
    StopSound
END SUB

SUB guitar
    IF songcounter = 0 THEN
        txtfile "guitar1.txt"
        StartSound "pray_song.wav"
        songcounter = songcounter + 1
    ELSEIF songcounter = 1 THEN
        txtfile "guitar2.txt"
        StartSound "love_song.wav"
        songcounter = songcounter + 1
    ELSEIF songcounter = 2 THEN
        txtfile "guitar3.txt"
        StartSound "blues_song.wav"
        songcounter = songcounter + 1
    ELSEIF songcounter = 3 THEN
        txtfile "pray2.txt"
        StartSound "pray_song2.wav"
        songcounter = songcounter + 1
    ELSEIF songcounter = 4 THEN
        txtfile "guitar4.txt"
        StartSound "guitar_song.wav"
        songcounter = 0
    END IF
    SLEEP
    StopSound
END SUB

SUB dreams
    DIM dream(0 TO 6) AS STRING
    dream(0) = "dream.txt"
    dream(1) = "dream2.txt"
    dream(2) = "dream3.txt"
    dream(3) = "nightmare1.txt"
    dream(4) = "eddie.txt"
    dream(5) = "dream_ta.txt"
    dream(6) = "johnny.txt"

    DIM dreamMusic(0 TO 4) AS STRING
    dreamMusic(0) = "dream1.wav"
    dreamMusic(1) = "dream2.wav"
    dreamMusic(2) = "walk1.wav"
    dreamMusic(3) = "eddie.wav"
    dreamMusic(4) = "dream_eva.wav"

    CLS
    txtfile dream(INT(RND * 7))
    StartSound dreamMusic(INT(RND * 5))
    SLEEP
    StopSound
END SUB

SUB music
    CLS
    IF counter < 2 THEN
        txtfile "music.txt"
        StartSound "fbsloop44.wav" ', 5
    ELSE
        COLOR _RGB32(0,0,0), _RGB32(255,255,255)
        CLS
        txtfile "music3.txt"
        StartSound "legends.ogg" ', 0 ' Play until done
    END IF
    SLEEP
    StopSound
    COLOR _RGB32(255,255,255), _RGB32(0,0,0)
    CLS
END SUB

SUB outside
    DIM k AS STRING
    DIM meetings(0 TO 3) AS STRING
    meetings(0) = "meeting1.txt"
    meetings(1) = "meeting2.txt"
    meetings(2) = "meeting3.txt"
    meetings(3) = "meeting4.txt"

    IF counter < 2 THEN
        txtfile "walk1.txt"
        StartSound "walk1.wav"
        SLEEP
        StopSound
        
    ELSEIF counter = 2 THEN
        txtfile "yuka.txt"
        StartSound "dream_eva.wav"
        PRINT
        PRINT
        PRINT "1. YOU NEED TO THINK ABOUT IT"
        PRINT "2. YES YOU WOULD LIKE TO TRY AND PARTICIPATE"
        k = getKeys$("12")
        StopSound
        IF k = "1" THEN
            EXIT SUB
        ELSEIF k = "2" THEN
            PRINT
            PRINT "THE GUIDE SHAKES YOUR HAND AND SAY 'THEN SEE YOU NEXT TIME' AND SMILE"
            counter = counter + 1
        END IF
        SLEEP
        
    ELSEIF counter = 3 THEN
        txtfile meetings(meetingcounter)
        StartSound "dream_eva.wav"
        SLEEP
        StopSound
        meetingcounter = meetingcounter + 1
        IF meetingcounter > 3 THEN counter = 4
        
    ELSEIF counter = 4 THEN
        COLOR _RGB32(0, 0, 0), _RGB32(255, 255, 255) ' Black text on white background
        CLS
        txtfile "workshop.txt"
        StartSound "dream_eva.wav"
        SLEEP
        StopSound
        finished = -1 ' TRUE
    END IF
END SUB

SUB warningscreen
    DIM k AS STRING
    SCREEN _NEWIMAGE(800, 600, 32)
    txtfile "warning.txt"
    k = getKeys$("12")
    IF k = "2" THEN
        SYSTEM
    END IF
END SUB

' ** MODIFIED: Now uses _LOADIMAGE and _LOADFONT with _EMBEDDED$ data **
SUB opening
    DIM scrn AS LONG
    DIM bild AS LONG
    DIM fnt AS LONG
    DIM imageBytes AS STRING ' To hold embedded image data
    DIM fontBytes AS STRING ' To hold embedded font data

    scrn = _NEWIMAGE(800, 600, 32)
    SCREEN scrn

    ' Load and display image
    imageBytes = _EMBEDDED$(IMG_PATH + "hikpic.bmp")
    bild = _LOADIMAGE(imageBytes, 32) ' Use image data
    IF bild < -1 THEN
        _PUTIMAGE (0, 0), bild
        _FREEIMAGE bild
    END IF

    ' Load font and draw text
    fontBytes = _EMBEDDED$("fonts/Montserrat-Bold.ttf")
    fnt = _LOADFONT(fontBytes, 50) ' Use font data
    IF fnt > 0 THEN
        _FONT fnt
    END IF

    COLOR _RGB32(0, 255, 0)
    _PRINTSTRING (300, 150), "HIKIKOMORY"

    COLOR _RGB32(0, 80, 255)
    _PRINTSTRING (250, 200), "A GAME BY RON77"

    IF fnt > 0 THEN _FONT 16 ' Reset to default font

    StartSound "sabrina.wav"
    SLEEP
    
    IF fnt > 0 THEN _FREEFONT fnt
END SUB

SUB main
    DIM scrn AS LONG
    DIM k AS STRING

    scrn = _NEWIMAGE(800, 600, 32)
    SCREEN scrn

    txtfile "startpoint.txt"
    SLEEP
    StopSound

    DO
        CLS
        cp 2, "DATE: " + dates1$(timePass)
        cp 4, "WHAT DO YOU WANT TO DO?"
        cp 6, "1. GO TO SLEEP..."
        cp 8, "2. PLAY A COMPUTER GAME..."
        cp 10, "3. LISTEN TO MUSIC..."
        cp 12, "4. CHECK E-MAIL..."
        cp 14, "5. GO OUTSIDE..."
        cp 16, "6. JOIN A CHAT-ROOM ON THE NET..."
        cp 18, "7. WRITE A POEM..."
        cp 20, "8. PLAY SOMETHING WITH YOUR GUITAR..."
        cp 22, "9. LISTEN TO THE NEWS..."
        cp 24, "PRESS ESC TO EXIT GAME..."

        k = getKeys$("123456789" + CHR$(27))

        SELECT CASE k
            CASE "9"
                news
            CASE "3"
                music
            CASE "1"
                dreams
            CASE "5"
                outside
            CASE "4"
                email
            CASE "2"
                ' ** NOTE: External URL. This cannot be embedded. **
                SHELL _DONTWAIT "cmd /c start https://youtu.be/-CbyAk3Sn9I"
            CASE "6"
                IF counter < 2 THEN
                    conversation "chat1.txt"
                ELSE
                    chatroom
                END IF
            CASE "8"
                guitar
            CASE "7"
                poem
        END SELECT

        timePass = timePass + 1
    LOOP UNTIL k = CHR$(27) OR finished = -1
END SUB