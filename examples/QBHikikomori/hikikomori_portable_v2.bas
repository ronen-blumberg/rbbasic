' HIKIKOMORI GAME - QB64PE VERSION (PORTABLE)
' All resources embedded into executable

'===============================================
' $EMBED DIRECTIVES FOR PORTABILITY
'===============================================
$Embed:'./data/ending_titles.txt','endingtitles'
$Embed:'./data/poemDB.txt','poemdb'
$Embed:'./data/news.txt','news'
$Embed:'./data/warning.txt','warning'
$Embed:'./data/startpoint.txt','startpoint'
$Embed:'./data/chat1.txt','chat1'
$Embed:'./data/yuka_guide.txt','yukaguide'
$Embed:'./data/mike.txt','mike'
$Embed:'./data/ken.txt','ken'
$Embed:'./data/email1.txt','email1'
$Embed:'./data/email2.txt','email2'
$Embed:'./data/email3.txt','email3'
$Embed:'./data/guitar1.txt','guitar1'
$Embed:'./data/guitar2.txt','guitar2'
$Embed:'./data/guitar3.txt','guitar3'
$Embed:'./data/pray2.txt','pray2'
$Embed:'./data/guitar4.txt','guitar4'
$Embed:'./data/dream.txt','dream'
$Embed:'./data/dream2.txt','dream2'
$Embed:'./data/dream3.txt','dream3'
$Embed:'./data/nightmare1.txt','nightmare1'
$Embed:'./data/eddie.txt','eddie'
$Embed:'./data/dream_ta.txt','dreamta'
$Embed:'./data/johnny.txt','johnny'
$Embed:'./data/music.txt','musictext'
$Embed:'./data/music3.txt','music3'
$Embed:'./data/walk1.txt','walk1text'
$Embed:'./data/yuka.txt','yuka'
$Embed:'./data/meeting1.txt','meeting1'
$Embed:'./data/meeting2.txt','meeting2'
$Embed:'./data/meeting3.txt','meeting3'
$Embed:'./data/meeting4.txt','meeting4'
$Embed:'./data/workshop.txt','workshop'
$Embed:'./sound/dream1.wav','snddream1'
$Embed:'./sound/dream2.wav','snddream2'
$Embed:'./sound/walk1.wav','sndwalk1'
$Embed:'./sound/eddie.wav','sndeddie'
$Embed:'./sound/dream_eva.wav','snddreameva'
$Embed:'./sound/email1.wav','sndemail1'
$Embed:'./sound/pray_song.wav','sndpraysong'
$Embed:'./sound/love_song.wav','sndlovesong'
$Embed:'./sound/blues_song.wav','sndbluessong'
$Embed:'./sound/pray_song2.wav','sndpraysong2'
$Embed:'./sound/guitar_song.wav','sndguitarsong'
$Embed:'./sound/fbsloop44.wav','sndfbsloop44'
$Embed:'./sound/legends.ogg','sndlegends'
$Embed:'./sound/sabrina.wav','sndsabrina'
$Embed:'./imgs/hikpic.bmp','hikpic'
$Embed:'./fonts/Montserrat-Bold.ttf','montserrat'

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
' CENTRALIZED EMBEDDED DATA RETRIEVAL
'===============================================

Function GetEmbeddedData$ (handle As String)
    Select Case LCase$(handle)
        ' Text files
        Case "endingtitles": GetEmbeddedData$ = _Embedded$("endingtitles")
        Case "poemdb": GetEmbeddedData$ = _Embedded$("poemdb")
        Case "news": GetEmbeddedData$ = _Embedded$("news")
        Case "warning": GetEmbeddedData$ = _Embedded$("warning")
        Case "startpoint": GetEmbeddedData$ = _Embedded$("startpoint")
        Case "chat1": GetEmbeddedData$ = _Embedded$("chat1")
        Case "yukaguide": GetEmbeddedData$ = _Embedded$("yukaguide")
        Case "mike": GetEmbeddedData$ = _Embedded$("mike")
        Case "ken": GetEmbeddedData$ = _Embedded$("ken")
        Case "email1": GetEmbeddedData$ = _Embedded$("email1")
        Case "email2": GetEmbeddedData$ = _Embedded$("email2")
        Case "email3": GetEmbeddedData$ = _Embedded$("email3")
        Case "guitar1": GetEmbeddedData$ = _Embedded$("guitar1")
        Case "guitar2": GetEmbeddedData$ = _Embedded$("guitar2")
        Case "guitar3": GetEmbeddedData$ = _Embedded$("guitar3")
        Case "pray2": GetEmbeddedData$ = _Embedded$("pray2")
        Case "guitar4": GetEmbeddedData$ = _Embedded$("guitar4")
        Case "dream": GetEmbeddedData$ = _Embedded$("dream")
        Case "dream2": GetEmbeddedData$ = _Embedded$("dream2")
        Case "dream3": GetEmbeddedData$ = _Embedded$("dream3")
        Case "nightmare1": GetEmbeddedData$ = _Embedded$("nightmare1")
        Case "eddie": GetEmbeddedData$ = _Embedded$("eddie")
        Case "dreamta": GetEmbeddedData$ = _Embedded$("dreamta")
        Case "johnny": GetEmbeddedData$ = _Embedded$("johnny")
        Case "musictext": GetEmbeddedData$ = _Embedded$("musictext")
        Case "music3": GetEmbeddedData$ = _Embedded$("music3")
        Case "walk1text": GetEmbeddedData$ = _Embedded$("walk1text")
        Case "yuka": GetEmbeddedData$ = _Embedded$("yuka")
        Case "meeting1": GetEmbeddedData$ = _Embedded$("meeting1")
        Case "meeting2": GetEmbeddedData$ = _Embedded$("meeting2")
        Case "meeting3": GetEmbeddedData$ = _Embedded$("meeting3")
        Case "meeting4": GetEmbeddedData$ = _Embedded$("meeting4")
        Case "workshop": GetEmbeddedData$ = _Embedded$("workshop")
        
            ' Sound files
        Case "snddream1": GetEmbeddedData$ = _Embedded$("snddream1")
        Case "snddream2": GetEmbeddedData$ = _Embedded$("snddream2")
        Case "sndwalk1": GetEmbeddedData$ = _Embedded$("sndwalk1")
        Case "sndeddie": GetEmbeddedData$ = _Embedded$("sndeddie")
        Case "snddreameva": GetEmbeddedData$ = _Embedded$("snddreameva")
        Case "sndemail1": GetEmbeddedData$ = _Embedded$("sndemail1")
        Case "sndpraysong": GetEmbeddedData$ = _Embedded$("sndpraysong")
        Case "sndlovesong": GetEmbeddedData$ = _Embedded$("sndlovesong")
        Case "sndbluessong": GetEmbeddedData$ = _Embedded$("sndbluessong")
        Case "sndpraysong2": GetEmbeddedData$ = _Embedded$("sndpraysong2")
        Case "sndguitarsong": GetEmbeddedData$ = _Embedded$("sndguitarsong")
        Case "sndfbsloop44": GetEmbeddedData$ = _Embedded$("sndfbsloop44")
        Case "sndlegends": GetEmbeddedData$ = _Embedded$("sndlegends")
        Case "sndsabrina": GetEmbeddedData$ = _Embedded$("sndsabrina")
        
            ' Image and font
        Case "hikpic": GetEmbeddedData$ = _Embedded$("hikpic")
        Case "montserrat": GetEmbeddedData$ = _Embedded$("montserrat")
        
        Case Else: GetEmbeddedData$ = ""
    End Select
End Function

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
        Case "music.txt": GetTextHandle$ = "musictext"
        Case "music3.txt": GetTextHandle$ = "music3"
        Case "walk1.txt": GetTextHandle$ = "walk1text"
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
    Dim pos1 As Long
    Dim result As String

    If content = "" Then
        NextLine$ = ""
        Exit Function
    End If

    pos1 = InStr(content, Chr$(10))

    If pos1 > 0 Then
        result = Left$(content, pos1 - 1)
        content = Mid$(content, pos1 + 1)
    Else
        result = content
        content = ""
    End If

    If Right$(result, 1) = Chr$(13) Then result = Left$(result, Len(result) - 1)

    NextLine$ = result
End Function

'===============================================
' PERSON-RELATED SUBS
'===============================================

Sub PersonRestart (p As PERSON, pid As Integer)
    Dim i As Integer
    p.kCnt = 0
    p.rCnt = 0
    p.wCnt = 0
    p.NoKeyFoundIndex = 0
    p.Greeting = ""
    p.You = ""
    p.Script = ""
    For i = 1 To MAX_KEYWORDS
        personKeywords(pid, i) = ""
        personRStarts(pid, i) = 0
        personREnds(pid, i) = 0
        personRIndex(pid, i) = 0
    Next
    For i = 1 To MAX_REPLIES
        personReplies(pid, i) = ""
    Next
    For i = 1 To MAX_WORDS
        personWordIn(pid, i) = ""
        personWordOut(pid, i) = ""
    Next
End Sub

Sub PersonLoadArrays (p As PERSON, scriptFile As String, pid As Integer)
    Dim content As String
    Dim startR As Integer, endR As Integer, ReadingR As Integer, temp As Integer
    Dim fline As String, kWord As String
    Dim handle As String

    handle = GetTextHandle$(scriptFile)
    content = GetEmbeddedData$(handle)
    
    Do
        fline = NextLine$(content)
        If fline = "" And content = "" Then Exit Do

        Select Case Left$(fline, 2)
            Case "g:"
                p.Greeting = LTrim$(RTrim$(Mid$(fline, 3)))
            Case "y:"
                p.You = LTrim$(RTrim$(Mid$(fline, 3)))
            Case "c:"
                p.Script = LTrim$(RTrim$(Mid$(fline, 3)))
            Case "s:"
                p.wCnt = p.wCnt + 1
                temp = InStr(fline, ">")
                If temp Then
                    personWordIn(pid, p.wCnt) = " " + LTrim$(RTrim$(Mid$(fline, 3, temp - 3))) + " "
                    personWordOut(pid, p.wCnt) = " " + LTrim$(RTrim$(Mid$(fline, temp + 1))) + " "
                End If
            Case "r:"
                p.rCnt = p.rCnt + 1
                personReplies(pid, p.rCnt) = LTrim$(RTrim$(Mid$(fline, 3)))
                If Not ReadingR Then
                    ReadingR = -1
                    startR = p.rCnt
                End If
            Case "k:"
                If ReadingR Then
                    endR = p.rCnt
                    ReadingR = 0
                End If
                If p.rCnt Then
                    p.kCnt = p.kCnt + 1
                    kWord = LTrim$(RTrim$(Mid$(fline, 3)))
                    personKeywords(pid, p.kCnt) = " " + kWord + " "
                    personRStarts(pid, p.kCnt) = startR
                    personRIndex(pid, p.kCnt) = startR
                    personREnds(pid, p.kCnt) = endR
                    If kWord = "nokeyfound" Then p.NoKeyFoundIndex = p.kCnt
                End If
            Case "e:"
                Exit Do
        End Select
    Loop

    If ReadingR Then
        endR = p.rCnt
        p.kCnt = p.kCnt + 1
        personKeywords(pid, p.kCnt) = "nokeyfound"
        personRStarts(pid, p.kCnt) = startR
        personRIndex(pid, p.kCnt) = startR
        personREnds(pid, p.kCnt) = endR
        p.NoKeyFoundIndex = p.kCnt
    End If
End Sub

Function isolatePunctuation$ (s As String)
    Dim b As String, i As Integer
    b = ""
    For i = 1 To Len(s)
        If InStr(PUNCTUATION, Mid$(s, i, 1)) > 0 Then
            b = b + " " + Mid$(s, i, 1) + " "
        Else
            b = b + Mid$(s, i, 1)
        End If
    Next
    isolatePunctuation$ = b
End Function

Function joinPunctuation$ (s As String)
    Dim b As String, find As String, i As Integer, place As Integer
    b = s
    For i = 1 To Len(PUNCTUATION)
        find = " " + Mid$(PUNCTUATION, i, 1) + " "
        place = InStr(b, find)
        While place > 0
            If place = 1 Then
                b = Mid$(PUNCTUATION, i, 1) + Mid$(b, place + 3)
            Else
                b = Mid$(b, 1, place - 1) + Mid$(PUNCTUATION, i, 1) + Mid$(b, place + 3)
            End If
            place = InStr(b, find)
        Wend
    Next
    joinPunctuation$ = b
End Function

Function PersonGetReply$ (p As PERSON, rply2 As String, switch As Integer, pid As Integer)
    Dim inpt As String, tail As String, answ As String
    Dim kFlag As Integer, k As Integer, kFound As Integer, l As Integer, w As Integer

    If switch = 0 Then
        Print p.You + ": ";
        Line Input "", inpt
        If LCase$(inpt) = "q" Or LCase$(inpt) = "x" Or LCase$(inpt) = "goodbye" Or LCase$(inpt) = "good night" Or LCase$(inpt) = "bye" Then
            PersonGetReply$ = "Goodbye!"
            Exit Function
        End If
    Else
        inpt = rply2
    End If

    inpt = " " + inpt + " "
    inpt = isolatePunctuation$(inpt)

    For k = 1 To p.kCnt
        kFound = InStr(LCase$(inpt), LCase$(personKeywords(pid, k)))
        If kFound > 0 Then
            tail = " " + Mid$(inpt, kFound + Len(personKeywords(pid, k)))
            For l = 1 To Len(tail)
                For w = 1 To p.wCnt
                    If LCase$(Mid$(tail, l, Len(personWordIn(pid, w)))) = LCase$(personWordIn(pid, w)) Then
                        tail = Mid$(tail, 1, l - 1) + personWordOut(pid, w) + Mid$(tail, l + Len(personWordIn(pid, w)))
                        Exit For
                    End If
                Next w
            Next l
            kFlag = -1
            Exit For
        End If
    Next

    If kFlag = 0 Then k = p.NoKeyFoundIndex

    answ = personReplies(pid, Int((personREnds(pid, k) - personRStarts(pid, k) + 1) * Rnd) + personRStarts(pid, k))

    If k = p.NoKeyFoundIndex Then
        personRIndex(pid, k) = Int((personREnds(pid, k) - personRStarts(pid, k) + 1) * Rnd) + personRStarts(pid, k)
    End If

    If Right$(answ, 1) <> "*" Then
        PersonGetReply$ = answ
        Exit Function
    End If

    If LTrim$(RTrim$(tail)) = "" Then
        PersonGetReply$ = "Please elaborate on, " + personKeywords(pid, k)
    Else
        tail = joinPunctuation$(tail)
        PersonGetReply$ = Mid$(answ, 1, Len(answ) - 1) + tail
    End If
End Function

Sub slow (Text As String)
    Dim speed(0 To 4) As Integer, i As Integer
    speed(0) = 50: speed(1) = 100: speed(2) = 20: speed(3) = 300: speed(4) = 250
    For i = 1 To Len(Text)
        Print Mid$(Text, i, 1);
        _Delay speed(Int(Rnd * 5)) / 1000
    Next
End Sub

Sub PersonSpeakToText (p As PERSON, lines As String)
    Print p.Script + ": ";
    slow lines
    Print
End Sub

'===============================================
' SOUND FUNCTIONS
'===============================================

Sub StartSound (f As String)
    If currentSound Then
        If _SndPlaying(currentSound) Then _SndStop currentSound
        _SndClose currentSound
        currentSound = 0
    End If
    
    Dim soundData As String
    Dim handle As String
    handle = GetSoundHandle$(f)
    If handle = "" Then Exit Sub
    
    soundData = GetEmbeddedData$(handle)
    If soundData = "" Then Exit Sub
    
    currentSound = _SndOpen(soundData, "memory")
    If currentSound Then
        _SndLoop currentSound
    End If
End Sub

Sub StopSound
    If currentSound Then
        If _SndPlaying(currentSound) Then _SndStop currentSound
        _SndClose currentSound
        currentSound = 0
    End If
End Sub

Sub PlaySound (f As String, waitTime As Integer)
    Dim snd As Long
    Dim soundData As String
    Dim handle As String
    handle = GetSoundHandle$(f)
    If handle = "" Then Exit Sub
    
    soundData = GetEmbeddedData$(handle)
    If soundData = "" Then Exit Sub
    
    snd = _SndOpen(soundData, "memory")
    
    If snd Then
        _SndPlay snd
        If waitTime > 0 Then
            _Delay waitTime
        Else
            Do While _SndPlaying(snd)
                _Limit 30
            Loop
        End If
        _SndClose snd
    End If
End Sub

Sub PlaySoundAsync (f As String)
    Dim snd As Long
    Dim soundData As String
    Dim handle As String
    handle = GetSoundHandle$(f)
    If handle = "" Then Exit Sub
    
    soundData = GetEmbeddedData$(handle)
    If soundData = "" Then Exit Sub
    
    snd = _SndOpen(soundData, "memory")
    
    If snd Then
        _SndPlay snd
    End If
End Sub

'===============================================
' GAME FUNCTIONS
'===============================================

Sub conversation (file As String)
    Dim i As Integer
    Dim kenzu As PERSON
    PersonRestart kenzu, 1

    Cls
    For i = 1 To 10
        Locate 5, 5
        Print "             ";
        _Delay 0.25
        Locate 5, 5
        Print "connecting...";
        _Delay 0.25
    Next
    Cls

    Dim rply As String
    Dim rply3 As String
    PersonLoadArrays kenzu, file, 1
    Print kenzu.Greeting: Print

    Do
        rply = PersonGetReply$(kenzu, rply3, 0, 1)
        Print
        PersonSpeakToText kenzu, rply
        Print
    Loop Until rply = "Goodbye!"

    Cls
    Locate 5, 5
    Print "disconnected...  "
    Sleep
End Sub

Sub chatroom
    Dim i As Integer
    Dim guide As PERSON
    Dim mike As PERSON
    Dim ken As PERSON

    PersonRestart guide, 1
    PersonRestart mike, 2
    PersonRestart ken, 3

    Cls
    For i = 1 To 10
        Locate 5, 5
        Print "             ";
        _Delay 0.25
        Locate 5, 5
        Print "connecting...";
        _Delay 0.25
    Next
    Cls

    Dim inText As String
    Dim rply As String
    Dim rply2 As String
    Dim rply3 As String

    PersonLoadArrays guide, "yuka_guide.txt", 1
    PersonLoadArrays mike, "mike.txt", 2
    PersonLoadArrays ken, "ken.txt", 3

    Print guide.Greeting: Print

    Do
        Print: Print guide.You + ": ";
        Line Input "", inText
        If LCase$(inText) = "q" Or LCase$(inText) = "x" Or LCase$(inText) = "goodbye" Or LCase$(inText) = "good night" Or LCase$(inText) = "bye" Then
            Print: Print
            PersonSpeakToText guide, "Goodbye!"
            Exit Do
        End If
        rply = PersonGetReply$(guide, inText, 1, 1)
        Print: PersonSpeakToText guide, rply
        rply2 = PersonGetReply$(mike, inText, 1, 2)
        Print: PersonSpeakToText mike, rply2
        rply3 = PersonGetReply$(ken, inText, 1, 3)
        Print: PersonSpeakToText ken, rply3
    Loop Until inText = "bye"

    Cls
    Locate 5, 5
    Print "disconnected...  "
    Sleep
End Sub

Sub cp (Row As Integer, s As String)
    Locate Row, (100 - Len(s)) / 2
    Print s
End Sub

Function GetKeys$ (keysToCatch$)
    Dim k$
    Do
        k$ = InKey$
        While Len(k$) = 0
            k$ = InKey$
            _Limit 60
        Wend
    Loop Until InStr(keysToCatch$, k$)
    GetKeys$ = k$
End Function

Function dates1$ (months As Integer)
    Dim monthNames(0 To 11) As String
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

    Dim m As Integer
    m = (months + 11) Mod 12

    dates1$ = monthNames(m) + "," + Str$(Int((months - 1) / 12) + 1997)
End Function

Sub LoadPoemDB (filename As String)
    Dim content As String
    Dim fline As String
    Dim handle As String

    handle = GetTextHandle$(filename)
    content = GetEmbeddedData$(handle)

    Do
        fline = NextLine$(content)
        If fline = "" And content = "" Then Exit Do

        Select Case Left$(fline, 2)
            Case "p:": sAppend places(), LTrim$(RTrim$(Mid$(fline, 3)))
            Case "c:": sAppend colors(), LTrim$(RTrim$(Mid$(fline, 3)))
            Case "m:": sAppend metaphore(), LTrim$(RTrim$(Mid$(fline, 3)))
            Case "o:": sAppend objects(), LTrim$(RTrim$(Mid$(fline, 3)))
            Case "g:": sAppend good(), LTrim$(RTrim$(Mid$(fline, 3)))
            Case "b:": sAppend bad(), LTrim$(RTrim$(Mid$(fline, 3)))
            Case "e:": Exit Do
        End Select
    Loop
End Sub

Sub RestartPoem
    ReDim places(0) As String
    ReDim colors(0) As String
    ReDim metaphore(0) As String
    ReDim objects(0) As String
    ReDim good(0) As String
    ReDim bad(0) As String
End Sub

Sub upper (f As String)
    Cls
    ReDim lines(0) As String
    Dim fline As String
    Dim hi As Integer
    Dim i As Integer
    Dim content As String
    Dim handle As String
    
    handle = GetTextHandle$(f)
    content = GetEmbeddedData$(handle)
    
    Do
        fline = NextLine$(content)
        If fline = "" And content = "" Then Exit Do
        sAppend lines(), fline
    Loop
    
    hi = _Height \ _FontHeight
    
    For i = 1 To hi - 1
        Print
    Next
    
    For i = 0 To UBound(lines)
        Print Space$(9); lines(i)
        _Delay 0.5
    Next
    
    For i = 1 To hi
        Print
        _Delay 0.5
    Next
    
    Print "END"
    Sleep
End Sub

Sub news
    Dim l As String, k As String
    Dim content As String

    Cls
    content = GetEmbeddedData$("news")

    Do
        l = NextLine$(content)
        If l = "" And content = "" Then Exit Do
        sAppend newsbla(), l
    Loop

    Print "PRESS 'q' OR ESC TO STOP"

    Do
        If UBound(newsbla) > 0 Then
            Print newsbla(Int(Rnd * UBound(newsbla)) + 1)
        End If
        Print
        _Delay 3
        k = InKey$
    Loop Until k = Chr$(27) Or k = "q"
End Sub

Sub txtfile (f As String)
    Cls
    Dim buffer As String
    Dim handle As String
    
    handle = GetTextHandle$(f)
    If handle = "" Then
        Print "file could not be opened: "; f
        Exit Sub
    End If
    
    buffer = GetEmbeddedData$(handle)
    
    If buffer = "" Then
        Print "file could not be read!"
        Exit Sub
    End If
    
    Dim p As Long
    Do
        p = InStr(buffer, Chr$(13))
        If p > 0 Then
            buffer = Left$(buffer, p - 1) + Mid$(buffer, p + 1)
        End If
    Loop While p > 0
    Print buffer
End Sub

Sub poem
    Dim dreamMusic(0 To 4) As String, i As Integer
    dreamMusic(0) = "dream1.wav"
    dreamMusic(1) = "dream2.wav"
    dreamMusic(2) = "walk1.wav"
    dreamMusic(3) = "eddie.wav"
    dreamMusic(4) = "dream_eva.wav"

    Cls
    RestartPoem
    LoadPoemDB "poemDB.txt"

    cp 1, "A POEM..."
    For i = 1 To 3
        Print
        If UBound(places) > 0 Then Print places(Int(Rnd * UBound(places)) + 1)
        If UBound(colors) > 0 Then Print colors(Int(Rnd * UBound(colors)) + 1)
        If UBound(metaphore) > 0 Then Print metaphore(Int(Rnd * UBound(metaphore)) + 1)
        If UBound(objects) > 0 Then Print objects(Int(Rnd * UBound(objects)) + 1)
        If UBound(good) > 0 Then Print good(Int(Rnd * UBound(good)) + 1)
        If UBound(bad) > 0 Then Print bad(Int(Rnd * UBound(bad)) + 1)
    Next

    StartSound dreamMusic(Int(Rnd * 5))
    Sleep
    StopSound
End Sub

Sub email
    Dim k As String

    Cls
    If counter = 0 Then
        cp 5, "NO EMAIL..."
        counter = counter + 1
    ElseIf counter = 1 Then
        txtfile "email1.txt"
        StartSound "email1.wav"

        Print
        Print
        Print "1. REPLY OR 2. IGNORE"
        k = GetKeys$("12")
        If k = "2" Then
            StopSound
            Exit Sub
        ElseIf k = "1" Then
            Print
            Print
            Print "YOU REPLY TO YUKA-YUKA CENTER"
            counter = counter + 1
        End If
    ElseIf counter = 2 Then
        txtfile "email2.txt"
        StartSound "email1.wav"
    ElseIf counter = 3 Then
        txtfile "email3.txt"
        StartSound "email1.wav"
    End If
    Sleep
    StopSound
End Sub

Sub guitar
    If songcounter = 0 Then
        txtfile "guitar1.txt"
        StartSound "pray_song.wav"
        songcounter = songcounter + 1
    ElseIf songcounter = 1 Then
        txtfile "guitar2.txt"
        StartSound "love_song.wav"
        songcounter = songcounter + 1
    ElseIf songcounter = 2 Then
        txtfile "guitar3.txt"
        StartSound "blues_song.wav"
        songcounter = songcounter + 1
    ElseIf songcounter = 3 Then
        txtfile "pray2.txt"
        StartSound "pray_song2.wav"
        songcounter = songcounter + 1
    ElseIf songcounter = 4 Then
        txtfile "guitar4.txt"
        StartSound "guitar_song.wav"
        songcounter = 0
    End If
    Sleep
    StopSound
End Sub

Sub dreams
    Dim dream(0 To 6) As String
    dream(0) = "dream.txt"
    dream(1) = "dream2.txt"
    dream(2) = "dream3.txt"
    dream(3) = "nightmare1.txt"
    dream(4) = "eddie.txt"
    dream(5) = "dream_ta.txt"
    dream(6) = "johnny.txt"

    Dim dreamMusic(0 To 4) As String
    dreamMusic(0) = "dream1.wav"
    dreamMusic(1) = "dream2.wav"
    dreamMusic(2) = "walk1.wav"
    dreamMusic(3) = "eddie.wav"
    dreamMusic(4) = "dream_eva.wav"

    Cls
    txtfile dream(Int(Rnd * 7))
    StartSound dreamMusic(Int(Rnd * 5))
    Sleep
    StopSound
End Sub

Sub music
    Cls
    If counter < 2 Then
        txtfile "music.txt"
        StartSound "fbsloop44.wav"
    Else
        Color _RGB32(0, 0, 0), _RGB32(255, 255, 255)
        Cls
        txtfile "music3.txt"
        StartSound "legends.ogg"
    End If
    Sleep
    StopSound
    Color _RGB32(255, 255, 255), _RGB32(0, 0, 0)
    Cls
End Sub

Sub outside
    Dim k As String
    Dim meetings(0 To 3) As String
    meetings(0) = "meeting1.txt"
    meetings(1) = "meeting2.txt"
    meetings(2) = "meeting3.txt"
    meetings(3) = "meeting4.txt"

    If counter < 2 Then
        txtfile "walk1.txt"
        StartSound "walk1.wav"
        Sleep
        StopSound
        
    ElseIf counter = 2 Then
        txtfile "yuka.txt"
        StartSound "dream_eva.wav"
        Print
        Print
        Print "1. YOU NEED TO THINK ABOUT IT"
        Print "2. YES YOU WOULD LIKE TO TRY AND PARTICIPATE"
        k = GetKeys$("12")
        StopSound
        If k = "1" Then
            Exit Sub
        ElseIf k = "2" Then
            Print
            Print "THE GUIDE SHAKES YOUR HAND AND SAY 'THEN SEE YOU NEXT TIME' AND SMILE"
            counter = counter + 1
        End If
        Sleep
        
    ElseIf counter = 3 Then
        txtfile meetings(meetingCounter)
        StartSound "dream_eva.wav"
        Sleep
        StopSound
        meetingCounter = meetingCounter + 1
        If meetingCounter > 3 Then counter = 4
        
    ElseIf counter = 4 Then
        Color _RGB32(0, 0, 0), _RGB32(255, 255, 255)
        Cls
        txtfile "workshop.txt"
        StartSound "dream_eva.wav"
        Sleep
        StopSound
        finished = -1
    End If
End Sub

Sub warningscreen
    Dim k As String
    Screen _NewImage(800, 600, 32)
    txtfile "warning.txt"
    k = GetKeys$("12")
    If k = "2" Then
        System
    End If
End Sub

Sub opening
    Dim scrn As Long
    Dim bild As Long
    Dim fnt As Long
    Dim imageData As String
    Dim fontData As String

    scrn = _NewImage(800, 600, 32)
    Screen scrn

    ' Load and display embedded image
    imageData = GetEmbeddedData$("hikpic")
    If imageData <> "" Then
        bild = _LoadImage(imageData, 32, "memory")
        If bild < -1 Then
            _PutImage (0, 0), bild
            _FreeImage bild
        End If
    End If

    ' Load embedded font and draw text
    fontData = GetEmbeddedData$("montserrat")
    If fontData <> "" Then
        fnt = _LoadFont(fontData, 50, "memory")
        If fnt > 0 Then
            _Font fnt
        End If
    End If

    Color _RGB32(0, 255, 0)
    _PrintString (300, 150), "HIKIKOMORY"

    Color _RGB32(0, 80, 255)
    _PrintString (250, 200), "A GAME BY RON77"

    If fnt > 0 Then _Font 16

    StartSound "sabrina.wav"
    Sleep
    StopSound
    
    If fnt > 0 Then _FreeFont fnt
End Sub

Sub main
    Dim scrn As Long
    Dim k As String

    scrn = _NewImage(800, 600, 32)
    Screen scrn

    txtfile "startpoint.txt"
    Sleep
    StopSound

    Do
        Cls
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

        k = GetKeys$("123456789" + Chr$(27))

        Select Case k
            Case "9"
                news
            Case "3"
                music
            Case "1"
                dreams
            Case "5"
                outside
            Case "4"
                email
            Case "2"
                ' External URL - cannot be embedded
                Shell _DontWait "cmd /c start https://youtu.be/-CbyAk3Sn9I"
            Case "6"
                If counter < 2 Then
                    conversation "chat1.txt"
                Else
                    chatroom
                End If
            Case "8"
                guitar
            Case "7"
                poem
        End Select

        timePass = timePass + 1
    Loop Until k = Chr$(27) Or finished = -1
End Sub
