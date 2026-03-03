' HIKIKOMORI GAME - QB64PE VERSION (PORTABLE)
' All resources embedded into executable

'===============================================
' $EMBED DIRECTIVES FOR PORTABILITY
'===============================================
$Embed:'.\data\ending_titles.txt','endingtitles'
$Embed:'.\data\poemDB.txt','poemdb'
$Embed:'.\data\news.txt','news'
$Embed:'.\data\warning.txt','warning'
$Embed:'.\data\startpoint.txt','startpoint'
$Embed:'.\data\chat1.txt','chat1'
$Embed:'.\data\yuka_guide.txt','yukaguide'
$Embed:'.\data\mike.txt','mike'
$Embed:'.\data\ken.txt','ken'
$Embed:'.\data\email1.txt','email1'
$Embed:'.\data\email2.txt','email2'
$Embed:'.\data\email3.txt','email3'
$Embed:'.\data\guitar1.txt','guitar1'
$Embed:'.\data\guitar2.txt','guitar2'
$Embed:'.\data\guitar3.txt','guitar3'
$Embed:'.\data\pray2.txt','pray2'
$Embed:'.\data\guitar4.txt','guitar4'
$Embed:'.\data\dream.txt','dream'
$Embed:'.\data\dream2.txt','dream2'
$Embed:'.\data\dream3.txt','dream3'
$Embed:'.\data\nightmare1.txt','nightmare1'
$Embed:'.\data\eddie.txt','eddie'
$Embed:'.\data\dream_ta.txt','dreamta'
$Embed:'.\data\johnny.txt','johnny'
$Embed:'.\data\music.txt','music'
$Embed:'.\data\music3.txt','music3'
$Embed:'.\data\walk1.txt','walk1'
$Embed:'.\data\yuka.txt','yuka'
$Embed:'.\data\meeting1.txt','meeting1'
$Embed:'.\data\meeting2.txt','meeting2'
$Embed:'.\data\meeting3.txt','meeting3'
$Embed:'.\data\meeting4.txt','meeting4'
$Embed:'.\data\workshop.txt','workshop'
$Embed:'.\sound\dream1.wav','snddream1'
$Embed:'.\sound\dream2.wav','snddream2'
$Embed:'.\sound\walk1.wav','sndwalk1'
$Embed:'.\sound\eddie.wav','sndeddie'
$Embed:'.\sound\dream_eva.wav','snddreameva'
$Embed:'.\sound\email1.wav','sndemail1'
$Embed:'.\sound\pray_song.wav','sndpraysong'
$Embed:'.\sound\love_song.wav','sndlovesong'
$Embed:'.\sound\blues_song.wav','sndbluessong'
$Embed:'.\sound\pray_song2.wav','sndpraysong2'
$Embed:'.\sound\guitar_song.wav','sndguitarsong'
$Embed:'.\sound\fbsloop44.wav','sndfbsloop44'
$Embed:'.\sound\legends.ogg','sndlegends'
$Embed:'.\sound\sabrina.wav','sndsabrina'
$Embed:'.\imgs\hikpic.bmp','hikpic'
$Embed:'.\fonts\Montserrat-Bold.ttf','montserrat'

'===============================================
' PROGRAM START
'===============================================

_Title "HIKIKOMORI GAME"
Randomize Timer

Dim Shared As Integer timePass, meetingCounter
ReDim Shared newsbla(0) As String
Dim Shared counter As Integer
Dim Shared songcounter As Integer
ReDim Shared places(0) As String
ReDim Shared colors(0) As String
ReDim Shared metaphore(0) As String
ReDim Shared objects(0) As String
ReDim Shared good(0) As String
ReDim Shared bad(0) As String
Dim Shared finished As Integer

' Person UDT
Type PERSON
    intext As String
    Greeting As String
    You As String
    Script As String
    kCnt As Integer
    rCnt As Integer
    wCnt As Integer
    NoKeyFoundIndex As Integer
End Type

' Arrays for each PERSON instance
Const MAX_PERSONS = 4
Const MAX_KEYWORDS = 100
Const MAX_REPLIES = 200
Const MAX_WORDS = 50

Dim Shared personKeywords(1 To MAX_PERSONS, 1 To MAX_KEYWORDS) As String
Dim Shared personReplies(1 To MAX_PERSONS, 1 To MAX_REPLIES) As String
Dim Shared personWordIn(1 To MAX_PERSONS, 1 To MAX_WORDS) As String
Dim Shared personWordOut(1 To MAX_PERSONS, 1 To MAX_WORDS) As String
Dim Shared personRStarts(1 To MAX_PERSONS, 1 To MAX_KEYWORDS) As Integer
Dim Shared personREnds(1 To MAX_PERSONS, 1 To MAX_KEYWORDS) As Integer
Dim Shared personRIndex(1 To MAX_PERSONS, 1 To MAX_KEYWORDS) As Integer
Dim Shared personID As Integer
Dim Shared currentSound As Long

Const PUNCTUATION = "?!,.:;<>(){}[]"

'===============================================
' MAIN PROGRAM
'===============================================

warningscreen
opening
main
upper "ending_titles.txt"
System

'===============================================
' HANDLE LOOKUP FUNCTIONS
'===============================================
Function GetSoundHandle$ (f As String)
    Select Case f
        Case "dream1.wav": GetSoundHandle$ = "snddream1"
        Case "dream2.wav": GetSoundHandle$ = "snddream2"
        Case "walk1.wav": GetSoundHandle$ = "sndwalk1"
        Case "eddie.wav": GetSoundHandle$ = "sndeddie"
        Case "dream_eva.wav": GetSoundHandle$ = "snddreameva"
        Case "email1.wav": GetSoundHandle$ = "sndemail1"
        Case "pray_song.wav": GetSoundHandle$ = "sndpraysong"
        Case "love_song.wav": GetSoundHandle$ = "sndlovesong"
        Case "blues_song.wav": GetSoundHandle$ = "sndbluessong"
        Case "pray_song2.wav": GetSoundHandle$ = "sndpraysong2"
        Case "guitar_song.wav": GetSoundHandle$ = "sndguitarsong"
        Case "fbsloop44.wav": GetSoundHandle$ = "sndfbsloop44"
        Case "legends.ogg": GetSoundHandle$ = "sndlegends"
        Case "sabrina.wav": GetSoundHandle$ = "sndsabrina"
        Case Else: GetSoundHandle$ = ""
    End Select
End Function

Function GetTextHandle$ (f As String)
    Select Case f
        Case "ending_titles.txt": GetTextHandle$ = "endingtitles"
        Case "poemDB.txt": GetTextHandle$ = "poemdb"
        Case "news.txt": GetTextHandle$ = "news"
        Case "warning.txt": GetTextHandle$ = "warning"
        Case "startpoint.txt": GetTextHandle$ = "startpoint"
        Case "chat1.txt": GetTextHandle$ = "chat1"
        Case "yuka_guide.txt": GetTextHandle$ = "yukaguide"
        Case "mike.txt": GetTextHandle$ = "mike"
        Case "ken.txt": GetTextHandle$ = "ken"
        Case "email1.txt": GetTextHandle$ = "email1"
        Case "email2.txt": GetTextHandle$ = "email2"
        Case "email3.txt": GetTextHandle$ = "email3"
        Case "guitar1.txt": GetTextHandle$ = "guitar1"
        Case "guitar2.txt": GetTextHandle$ = "guitar2"
        Case "guitar3.txt": GetTextHandle$ = "guitar3"
        Case "pray2.txt": GetTextHandle$ = "pray2"
        Case "guitar4.txt": GetTextHandle$ = "guitar4"
        Case "dream.txt": GetTextHandle$ = "dream"
        Case "dream2.txt": GetTextHandle$ = "dream2"
        Case "dream3.txt": GetTextHandle$ = "dream3"
        Case "nightmare1.txt": GetTextHandle$ = "nightmare1"
        Case "eddie.txt": GetTextHandle$ = "eddie"
        Case "dream_ta.txt": GetTextHandle$ = "dreamta"
        Case "johnny.txt": GetTextHandle$ = "johnny"
        Case "music.txt": GetTextHandle$ = "music"
        Case "music3.txt": GetTextHandle$ = "music3"
        Case "walk1.txt": GetTextHandle$ = "walk1"
        Case "yuka.txt": GetTextHandle$ = "yuka"
        Case "meeting1.txt": GetTextHandle$ = "meeting1"
        Case "meeting2.txt": GetTextHandle$ = "meeting2"
        Case "meeting3.txt": GetTextHandle$ = "meeting3"
        Case "meeting4.txt": GetTextHandle$ = "meeting4"
        Case "workshop.txt": GetTextHandle$ = "workshop"
        Case Else: GetTextHandle$ = ""
    End Select
End Function

'===============================================
' UTILITY FUNCTIONS
'===============================================

Sub sAppend (arr() As String, Item As String)
    ReDim _Preserve arr(LBound(arr) To UBound(arr) + 1) As String
    arr(UBound(arr)) = Item
End Sub

Sub nAppend (arr() As Integer, Item As Integer)
    ReDim _Preserve arr(LBound(arr) To UBound(arr) + 1) As Integer
    arr(UBound(arr)) = Item
End Sub

Function rnd_range# (first As Double, last As Double)
    rnd_range# = Rnd * (last - first) + first
End Function

Function NextLine$ (content As String)
    DIM pos AS LONG
    DIM result AS STRING
    
    IF content = "" THEN
        NextLine$ = ""
        EXIT FUNCTION
    END IF
    
    pos = INSTR(content, CHR$(10))
    
    IF pos > 0 THEN
        result = LEFT$(content, pos - 1)
        content = MID$(content, pos + 1)
    ELSE
        result = content
        content = ""
    END IF
    
    IF RIGHT$(result, 1) = CHR$(13) THEN result = LEFT$(result, LEN(result) - 1)
    
    NextLine$ = result
END FUNCTION

'===============================================
' PERSON-RELATED SUBS
'===============================================

SUB PersonRestart (p AS PERSON, pid AS INTEGER)
    DIM i AS INTEGER
    p.kCnt = 0
    p.rCnt = 0
    p.wCnt = 0
    p.NoKeyFoundIndex = 0
    p.Greeting = ""
    p.You = ""
    p.Script = ""
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

SUB PersonLoadArrays (p AS PERSON, scriptFile AS STRING, pid AS INTEGER)
    DIM content AS STRING
    DIM startR AS INTEGER, endR AS INTEGER, ReadingR AS INTEGER, temp AS INTEGER
    DIM fline AS STRING, kWord AS STRING

    content = _EMBEDDED$(GetTextHandle$(scriptFile))
    
    DO
        fline = NextLine$(content)
        IF fline = "" AND content = "" THEN EXIT DO

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

    IF ReadingR THEN
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
    DIM speed(0 TO 4) AS INTEGER, i AS INTEGER
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
' SOUND FUNCTIONS
'===============================================

SUB StartSound (f AS STRING)
    IF currentSound THEN
        IF _SNDPLAYING(currentSound) THEN _SNDSTOP currentSound
        _SNDCLOSE currentSound
        currentSound = 0
    END IF
    
    DIM soundData AS STRING
    DIM handle AS STRING
    handle = GetSoundHandle$(f)
    IF handle = "" THEN EXIT SUB
    
    soundData = _EMBEDDED$(handle)
    currentSound = _SNDOPEN(soundData, "memory")
    IF currentSound THEN
        _SNDLOOP currentSound
    END IF
END SUB

SUB StopSound
    IF currentSound THEN
        IF _SNDPLAYING(currentSound) THEN _SNDSTOP currentSound
        _SNDCLOSE currentSound
        currentSound = 0
    END IF
END SUB

SUB PlaySound (f AS STRING, waitTime AS INTEGER)
    DIM snd AS LONG
    DIM soundData AS STRING
    DIM handle AS STRING
    handle = GetSoundHandle$(f)
    IF handle = "" THEN EXIT SUB
    
    soundData = _EMBEDDED$(handle)
    snd = _SNDOPEN(soundData, "memory")
    
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

SUB PlaySoundAsync (f AS STRING)
    DIM snd AS LONG
    DIM soundData AS STRING
    DIM handle AS STRING
    handle = GetSoundHandle$(f)
    IF handle = "" THEN EXIT SUB
    
    soundData = _EMBEDDED$(handle)
    snd = _SNDOPEN(soundData, "memory")
    
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
        PRINT "             ";
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
        PRINT "             ";
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

SUB LoadPoemDB (filename AS STRING)
    DIM content AS STRING
    DIM fline AS STRING

    content = _EMBEDDED$(GetTextHandle$(filename))

    DO
        fline = NextLine$(content)
        IF fline = "" AND content = "" THEN EXIT DO

        SELECT CASE LEFT$(fline, 2)
            CASE "p:": sAppend places(), LTRIM$(RTRIM$(MID$(fline, 3)))
            CASE "c:": sAppend colors(), LTRIM$(RTRIM$(MID$(fline, 3)))
            CASE "m:": sAppend metaphore(), LTRIM$(RTRIM$(MID$(fline, 3)))
            CASE "o:": sAppend objects(), LTRIM$(RTRIM$(MID$(fline, 3)))
            CASE "g:": sAppend good(), LTRIM$(RTRIM$(MID$(fline, 3)))
            CASE "b:": sAppend bad(), LTRIM$(RTRIM$(MID$(fline, 3)))
            CASE "e:": EXIT DO
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

SUB upper (f AS STRING)
    CLS
    REDIM lines(0) AS STRING
    DIM fline AS STRING
    DIM hi AS INTEGER
    DIM i AS INTEGER
    DIM content AS STRING
    
    content = _EMBEDDED$(GetTextHandle$(f))
    
    DO
        fline = NextLine$(content)
        IF fline = "" AND content = "" THEN EXIT DO
        sAppend lines(), fline
    LOOP
    
    hi = _HEIGHT \ _FONTHEIGHT
    
    FOR i = 1 TO hi - 1
        PRINT
    NEXT
    
    FOR i = 0 TO UBOUND(lines)
        PRINT SPACE$(9); lines(i)
        _DELAY 0.5
    NEXT
    
    FOR i = 1 TO hi
        PRINT
        _DELAY 0.5
    NEXT
    
    PRINT "END"
    SLEEP
END SUB

SUB news
    DIM l AS STRING, k AS STRING
    DIM content AS STRING

    CLS
    content = _EMBEDDED$("news")

    DO
        l = NextLine$(content)
        IF l = "" AND content = "" THEN EXIT DO
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

SUB txtfile (f AS STRING)
    CLS
    DIM buffer AS STRING
    DIM handle AS STRING
    
    handle = GetTextHandle$(f)
    IF handle = "" THEN
        PRINT "file could not be opened!"
        EXIT SUB
    END IF
    
    buffer = _EMBEDDED$(handle)
    
    IF buffer = "" THEN
        PRINT "file could not be read!"
        EXIT SUB
    END IF
    
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
    DIM dreamMusic(0 TO 4) AS STRING, i AS INTEGER
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
            StopSound
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
        StartSound "fbsloop44.wav"
    ELSE
        COLOR _RGB32(0, 0, 0), _RGB32(255, 255, 255)
        CLS
        txtfile "music3.txt"
        StartSound "legends.ogg"
    END IF
    SLEEP
    StopSound
    COLOR _RGB32(255, 255, 255), _RGB32(0, 0, 0)
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
        COLOR _RGB32(0, 0, 0), _RGB32(255, 255, 255)
        CLS
        txtfile "workshop.txt"
        StartSound "dream_eva.wav"
        SLEEP
        StopSound
        finished = -1
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

SUB opening
    DIM scrn AS LONG
    DIM bild AS LONG
    DIM fnt AS LONG
    DIM imageData AS STRING
    DIM fontData AS STRING

    scrn = _NEWIMAGE(800, 600, 32)
    SCREEN scrn

    ' Load and display embedded image
    imageData = _EMBEDDED$("hikpic")
    bild = _LOADIMAGE(imageData, 32, "memory")
    IF bild < -1 THEN
        _PUTIMAGE (0, 0), bild
        _FREEIMAGE bild
    END IF

    ' Load embedded font and draw text
    fontData = _EMBEDDED$("montserrat")
    fnt = _LOADFONT(fontData, 50, "memory")
    IF fnt > 0 THEN
        _FONT fnt
    END IF

    COLOR _RGB32(0, 255, 0)
    _PRINTSTRING (300, 150), "HIKIKOMORY"

    COLOR _RGB32(0, 80, 255)
    _PRINTSTRING (250, 200), "A GAME BY RON77"

    IF fnt > 0 THEN _FONT 16

    StartSound "sabrina.wav"
    SLEEP
    StopSound
    
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
                ' External URL - cannot be embedded
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
