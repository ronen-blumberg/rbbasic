REM ============================================================
REM HIKIKOMORI GAME - RBBasic VERSION
REM Converted from QB64 for RBBasic compatibility
REM Original by ron77
REM ============================================================

RANDOMIZE TIMER

REM ====== Path Setup ======
DATA_PATH$ = "data/"
SOUND_PATH$ = "sound/"
IMG_PATH$ = "imgs/"

REM ====== Punctuation for chatbot ======
punctuation$ = "?!,.:;<>(){}[]"

REM ====== Game state ======
timePass = 0
meetingCounter = 0
counter = 0
songcounter = 0
finished = 0
current_sound = 0
news_loaded = 0

REM ====== Shared parameter-passing variables ======
cp_row = 0
sound_wait = 0
current_pid = 0
reply_switch = 0
dates_months = 0
dummy$ = ""

REM ====== Person System Arrays (3 persons, 0-based) ======
CONST MAX_KEYWORDS = 100
CONST MAX_REPLIES = 200
CONST MAX_WORDS = 50

DIM person_greeting$(3)
DIM person_you$(3)
DIM person_script$(3)
DIM person_kcnt(3)
DIM person_rcnt(3)
DIM person_wcnt(3)
DIM person_nokeyindex(3)

DIM personKeywords$(3, 100)
DIM personReplies$(3, 200)
DIM personWordIn$(3, 50)
DIM personWordOut$(3, 50)
DIM personRStarts(3, 100)
DIM personREnds(3, 100)
DIM personRIndex(3, 100)

REM ====== Dynamic Array Storage ======
CONST MAX_LINES = 500

DIM newsbla$(MAX_LINES)
newsbla_count = 0
DIM places$(MAX_LINES)
places_count = 0
DIM colors_arr$(MAX_LINES)
colors_count = 0
DIM metaphore$(MAX_LINES)
metaphore_count = 0
DIM objects$(MAX_LINES)
objects_count = 0
DIM good_arr$(MAX_LINES)
good_count = 0
DIM bad_arr$(MAX_LINES)
bad_count = 0
DIM upper_lines$(MAX_LINES)
upper_lines_count = 0

REM ====== Month Names ======
DIM month_names$(11)
month_names$(0) = "January"
month_names$(1) = "February"
month_names$(2) = "March"
month_names$(3) = "April"
month_names$(4) = "May"
month_names$(5) = "June"
month_names$(6) = "July"
month_names$(7) = "August"
month_names$(8) = "September"
month_names$(9) = "October"
month_names$(10) = "November"
month_names$(11) = "December"

REM ====== Dream/Music/Meeting file arrays ======
DIM dream_files$(7)
dream_files$(0) = "dream.txt"
dream_files$(1) = "dream2.txt"
dream_files$(2) = "dream3.txt"
dream_files$(3) = "nightmare1.txt"
dream_files$(4) = "eddie.txt"
dream_files$(5) = "dream_ta.txt"
dream_files$(6) = "johnny.txt"

DIM dream_music$(5)
dream_music$(0) = "dream1.wav"
dream_music$(1) = "dream2.wav"
dream_music$(2) = "walk1.wav"
dream_music$(3) = "eddie.wav"
dream_music$(4) = "dream_eva.wav"

DIM meeting_files$(4)
meeting_files$(0) = "meeting1.txt"
meeting_files$(1) = "meeting2.txt"
meeting_files$(2) = "meeting3.txt"
meeting_files$(3) = "meeting4.txt"

REM ====== MAIN PROGRAM FLOW ======
CALL warningscreen
CALL opening
CALL gamemain
upper_file$ = "ending_titles.txt"
CALL upper
SYSTEM


REM ============================================================
REM UTILITY SUBS
REM ============================================================

SUB cp
    LOCATE cp_row, (80 - LEN(cp_text$)) / 2
    PRINT cp_text$
END SUB

SUB slow
    REM No _DELAY available - print text instantly
    PRINT slow_text$;
END SUB

SUB txtfile
    CLS
    tf$ = DATA_PATH$ + txtfile_name$
    OPEN tf$ FOR INPUT AS #1
    WHILE NOT EOF(1)
        LINE INPUT #1, tbuf$
        PRINT tbuf$
    WEND
    CLOSE #1
END SUB

SUB dates1
    dm = (dates_months + 11) MOD 12
    dyr = INT((dates_months - 1) / 12) + 1997
    dates_result$ = month_names$(dm) + "," + STR$(dyr)
END SUB

SUB isolatePunctuation
    ib$ = ""
    FOR ip = 1 TO LEN(isolate_in$)
        ic$ = MID$(isolate_in$, ip, 1)
        IF INSTR(punctuation$, ic$) > 0 THEN
            ib$ = ib$ + " " + ic$ + " "
        ELSE
            ib$ = ib$ + ic$
        END IF
    NEXT ip
    isolate_out$ = ib$
END SUB

SUB joinPunctuation
    jb$ = join_in$
    FOR jp = 1 TO LEN(punctuation$)
        jpc$ = MID$(punctuation$, jp, 1)
        jfind$ = " " + jpc$ + " "
        jplace = INSTR(jb$, jfind$)
        WHILE jplace > 0
            IF jplace = 1 THEN
                jb$ = jpc$ + MID$(jb$, jplace + 3)
            ELSE
                jb$ = MID$(jb$, 1, jplace - 1) + jpc$ + MID$(jb$, jplace + 3)
            END IF
            jplace = INSTR(jb$, jfind$)
        WEND
    NEXT jp
    join_out$ = jb$
END SUB


REM ============================================================
REM SOUND SUBS
REM ============================================================

SUB StartSound
    IF current_sound > 0 THEN
        _SNDSTOP current_sound
    END IF
    snd_path$ = SOUND_PATH$ + sound_file$
    current_sound = _SNDOPEN(snd_path$)
    IF current_sound > 0 THEN
        _SNDPLAY current_sound
    END IF
END SUB

SUB StopSound
    IF current_sound > 0 THEN
        _SNDSTOP current_sound
    END IF
END SUB

SUB PlaySound
    ps_path$ = SOUND_PATH$ + sound_file$
    ps_snd = _SNDOPEN(ps_path$)
    IF ps_snd > 0 THEN
        _SNDPLAY ps_snd
        IF sound_wait > 0 THEN
            SLEEP sound_wait
        END IF
    END IF
END SUB

SUB PlaySoundAsync
    pa_path$ = SOUND_PATH$ + sound_file$
    pa_snd = _SNDOPEN(pa_path$)
    IF pa_snd > 0 THEN
        _SNDPLAY pa_snd
    END IF
END SUB


REM ============================================================
REM PERSON SYSTEM SUBS (ELIZA-style chatbot)
REM ============================================================

SUB PersonRestart
    pid = current_pid
    person_kcnt(pid) = 0
    person_rcnt(pid) = 0
    person_wcnt(pid) = 0
    person_nokeyindex(pid) = 0
    person_greeting$(pid) = ""
    person_you$(pid) = ""
    person_script$(pid) = ""
    FOR pri = 0 TO MAX_KEYWORDS
        personKeywords$(pid, pri) = ""
        personRStarts(pid, pri) = 0
        personREnds(pid, pri) = 0
        personRIndex(pid, pri) = 0
    NEXT pri
    FOR pri = 0 TO MAX_REPLIES
        personReplies$(pid, pri) = ""
    NEXT pri
    FOR pri = 0 TO MAX_WORDS
        personWordIn$(pid, pri) = ""
        personWordOut$(pid, pri) = ""
    NEXT pri
END SUB

SUB PersonLoadArrays
    pid = current_pid
    pla_startR = 0
    pla_endR = 0
    pla_readingR = 0

    plaf$ = DATA_PATH$ + loadscript_file$
    OPEN plaf$ FOR INPUT AS #1

    WHILE NOT EOF(1)
        LINE INPUT #1, plafline$
        pla_prefix$ = LEFT$(plafline$, 2)

        SELECT CASE pla_prefix$
            CASE "g:"
                person_greeting$(pid) = LTRIM$(RTRIM$(MID$(plafline$, 3)))
            CASE "y:"
                person_you$(pid) = LTRIM$(RTRIM$(MID$(plafline$, 3)))
            CASE "c:"
                person_script$(pid) = LTRIM$(RTRIM$(MID$(plafline$, 3)))
            CASE "s:"
                person_wcnt(pid) = person_wcnt(pid) + 1
                pla_wc = person_wcnt(pid)
                pla_tmp = INSTR(plafline$, ">")
                IF pla_tmp > 0 THEN
                    personWordIn$(pid, pla_wc) = " " + LTRIM$(RTRIM$(MID$(plafline$, 3, pla_tmp - 3))) + " "
                    personWordOut$(pid, pla_wc) = " " + LTRIM$(RTRIM$(MID$(plafline$, pla_tmp + 1))) + " "
                END IF
            CASE "r:"
                person_rcnt(pid) = person_rcnt(pid) + 1
                pla_rc = person_rcnt(pid)
                personReplies$(pid, pla_rc) = LTRIM$(RTRIM$(MID$(plafline$, 3)))
                IF pla_readingR = 0 THEN
                    pla_readingR = 1
                    pla_startR = pla_rc
                END IF
            CASE "k:"
                IF pla_readingR = 1 THEN
                    pla_endR = person_rcnt(pid)
                    pla_readingR = 0
                END IF
                IF person_rcnt(pid) > 0 THEN
                    person_kcnt(pid) = person_kcnt(pid) + 1
                    pla_kc = person_kcnt(pid)
                    pla_kWord$ = LTRIM$(RTRIM$(MID$(plafline$, 3)))
                    personKeywords$(pid, pla_kc) = " " + pla_kWord$ + " "
                    personRStarts(pid, pla_kc) = pla_startR
                    personRIndex(pid, pla_kc) = pla_startR
                    personREnds(pid, pla_kc) = pla_endR
                    IF pla_kWord$ = "nokeyfound" THEN
                        person_nokeyindex(pid) = pla_kc
                    END IF
                END IF
            CASE "e:"
                EXIT WHILE
        END SELECT
    WEND
    CLOSE #1

    IF pla_readingR = 1 THEN
        pla_endR = person_rcnt(pid)
        person_kcnt(pid) = person_kcnt(pid) + 1
        pla_kc = person_kcnt(pid)
        personKeywords$(pid, pla_kc) = "nokeyfound"
        personRStarts(pid, pla_kc) = pla_startR
        personRIndex(pid, pla_kc) = pla_startR
        personREnds(pid, pla_kc) = pla_endR
        person_nokeyindex(pid) = pla_kc
    END IF
END SUB

SUB PersonGetReply
    pid = current_pid

    IF reply_switch = 0 THEN
        REM Get input from user
        pgr_prompt$ = person_you$(pid) + ": "
        PRINT pgr_prompt$;
        LINE INPUT "", pgr_inpt$
        pgr_chk$ = LCASE$(pgr_inpt$)
        IF pgr_chk$ = "q" OR pgr_chk$ = "x" OR pgr_chk$ = "goodbye" OR pgr_chk$ = "good night" OR pgr_chk$ = "bye" THEN
            reply_result$ = "Goodbye!"
            EXIT SUB
        END IF
    ELSE
        pgr_inpt$ = reply_input$
    END IF

    pgr_inpt$ = " " + pgr_inpt$ + " "
    isolate_in$ = pgr_inpt$
    CALL isolatePunctuation
    pgr_inpt$ = isolate_out$

    pgr_kFlag = 0
    pgr_kidx = 0
    pgr_tail$ = ""

    REM Split LCASE$ calls into separate vars to avoid temp buffer aliasing
    pgr_lcinpt$ = LCASE$(pgr_inpt$)
    FOR pgr_k = 1 TO person_kcnt(pid)
        pgr_lckey$ = LCASE$(personKeywords$(pid, pgr_k))
        pgr_kFound = INSTR(pgr_lcinpt$, pgr_lckey$)
        IF pgr_kFound > 0 THEN
            pgr_tail$ = " " + MID$(pgr_inpt$, pgr_kFound + LEN(personKeywords$(pid, pgr_k)))
            FOR pgr_l = 1 TO LEN(pgr_tail$)
                FOR pgr_w = 1 TO person_wcnt(pid)
                    pgr_lcpart$ = LCASE$(MID$(pgr_tail$, pgr_l, LEN(personWordIn$(pid, pgr_w))))
                    pgr_lcword$ = LCASE$(personWordIn$(pid, pgr_w))
                    IF pgr_lcpart$ = pgr_lcword$ THEN
                        pgr_before$ = MID$(pgr_tail$, 1, pgr_l - 1)
                        pgr_after$ = MID$(pgr_tail$, pgr_l + LEN(personWordIn$(pid, pgr_w)))
                        pgr_tail$ = pgr_before$ + personWordOut$(pid, pgr_w) + pgr_after$
                        EXIT FOR
                    END IF
                NEXT pgr_w
            NEXT pgr_l
            pgr_kFlag = 1
            pgr_kidx = pgr_k
            EXIT FOR
        END IF
    NEXT pgr_k

    IF pgr_kFlag = 0 THEN
        pgr_kidx = person_nokeyindex(pid)
    END IF

    pgr_range = personREnds(pid, pgr_kidx) - personRStarts(pid, pgr_kidx) + 1
    pgr_randidx = INT(pgr_range * RND) + personRStarts(pid, pgr_kidx)
    pgr_answ$ = personReplies$(pid, pgr_randidx)

    IF pgr_kidx = person_nokeyindex(pid) THEN
        personRIndex(pid, pgr_kidx) = pgr_randidx
    END IF

    IF RIGHT$(pgr_answ$, 1) <> "*" THEN
        reply_result$ = pgr_answ$
        EXIT SUB
    END IF

    pgr_rtrim$ = RTRIM$(pgr_tail$)
    pgr_trimtail$ = LTRIM$(pgr_rtrim$)
    IF LEN(pgr_trimtail$) = 0 THEN
        reply_result$ = "Please elaborate on, " + personKeywords$(pid, pgr_kidx)
    ELSE
        join_in$ = pgr_tail$
        CALL joinPunctuation
        pgr_tail$ = join_out$
        pgr_base$ = MID$(pgr_answ$, 1, LEN(pgr_answ$) - 1)
        reply_result$ = pgr_base$ + pgr_tail$
    END IF
END SUB

SUB PersonSpeakToText
    pid = current_pid
    pst_msg$ = person_script$(pid) + ": "
    PRINT pst_msg$;
    slow_text$ = speak_text$
    CALL slow
    PRINT
END SUB


REM ============================================================
REM GAME ACTIVITY SUBS
REM ============================================================

SUB conversation
    current_pid = 0
    CALL PersonRestart

    CLS
    LOCATE 5, 5
    PRINT "connecting..."
    SLEEP 2
    CLS

    loadscript_file$ = conv_file$
    CALL PersonLoadArrays
    PRINT person_greeting$(0)
    PRINT

    DO
        reply_switch = 0
        CALL PersonGetReply
        conv_rply$ = reply_result$
        PRINT
        current_pid = 0
        speak_text$ = conv_rply$
        CALL PersonSpeakToText
        PRINT
    LOOP UNTIL conv_rply$ = "Goodbye!"

    CLS
    LOCATE 5, 5
    PRINT "disconnected..."
    LINE INPUT "", dummy$
END SUB

SUB chatroom
    current_pid = 0
    CALL PersonRestart
    current_pid = 1
    CALL PersonRestart
    current_pid = 2
    CALL PersonRestart

    CLS
    LOCATE 5, 5
    PRINT "connecting..."
    SLEEP 2
    CLS

    current_pid = 0
    loadscript_file$ = "yuka_guide.txt"
    CALL PersonLoadArrays
    current_pid = 1
    loadscript_file$ = "mike.txt"
    CALL PersonLoadArrays
    current_pid = 2
    loadscript_file$ = "ken.txt"
    CALL PersonLoadArrays

    PRINT person_greeting$(0)
    PRINT

    DO
        PRINT
        cr_prompt$ = person_you$(0) + ": "
        PRINT cr_prompt$;
        LINE INPUT "", cr_inText$
        IF LCASE$(cr_inText$) = "q" OR LCASE$(cr_inText$) = "x" OR LCASE$(cr_inText$) = "goodbye" OR LCASE$(cr_inText$) = "good night" OR LCASE$(cr_inText$) = "bye" THEN
            PRINT
            PRINT
            current_pid = 0
            speak_text$ = "Goodbye!"
            CALL PersonSpeakToText
            EXIT DO
        END IF

        current_pid = 0
        reply_input$ = cr_inText$
        reply_switch = 1
        CALL PersonGetReply
        PRINT
        speak_text$ = reply_result$
        CALL PersonSpeakToText

        current_pid = 1
        reply_input$ = cr_inText$
        reply_switch = 1
        CALL PersonGetReply
        PRINT
        speak_text$ = reply_result$
        CALL PersonSpeakToText

        current_pid = 2
        reply_input$ = cr_inText$
        reply_switch = 1
        CALL PersonGetReply
        PRINT
        speak_text$ = reply_result$
        CALL PersonSpeakToText
    LOOP UNTIL LCASE$(cr_inText$) = "bye"

    CLS
    LOCATE 5, 5
    PRINT "disconnected..."
    LINE INPUT "", dummy$
END SUB

SUB news
    IF news_loaded = 0 THEN
        nf$ = DATA_PATH$ + "news.txt"
        OPEN nf$ FOR INPUT AS #1
        WHILE NOT EOF(1)
            LINE INPUT #1, nl$
            newsbla_count = newsbla_count + 1
            newsbla$(newsbla_count) = nl$
        WEND
        CLOSE #1
        news_loaded = 1
    END IF

    CLS
    PRINT "NEWS FEED"
    PRINT

    IF newsbla_count > 0 THEN
        FOR ni = 1 TO 10
            nidx = INT(RND * newsbla_count) + 1
            PRINT newsbla$(nidx)
            PRINT
        NEXT ni
    END IF

    LINE INPUT "Press ENTER to go back...", dummy$
END SUB

SUB email
    CLS
    SELECT CASE counter
        CASE 0
            cp_row = 5
            cp_text$ = "NO EMAIL..."
            CALL cp
            counter = counter + 1
        CASE 1
            txtfile_name$ = "email1.txt"
            CALL txtfile
            sound_file$ = "email1.wav"
            CALL StartSound
            PRINT
            PRINT
            PRINT "1. REPLY OR 2. IGNORE"
            LINE INPUT "Your choice: ", ek$
            IF ek$ = "2" THEN
                CALL StopSound
                EXIT SUB
            END IF
            IF ek$ = "1" THEN
                PRINT
                PRINT
                PRINT "YOU REPLY TO YUKA-YUKA CENTER"
                counter = counter + 1
            END IF
        CASE 2
            txtfile_name$ = "email2.txt"
            CALL txtfile
            sound_file$ = "email1.wav"
            CALL StartSound
        CASE 3
            txtfile_name$ = "email3.txt"
            CALL txtfile
            sound_file$ = "email1.wav"
            CALL StartSound
    END SELECT
    LINE INPUT "", dummy$
    CALL StopSound
END SUB

SUB guitar
    SELECT CASE songcounter
        CASE 0
            txtfile_name$ = "guitar1.txt"
            CALL txtfile
            sound_file$ = "pray_song.wav"
            CALL StartSound
            songcounter = songcounter + 1
        CASE 1
            txtfile_name$ = "guitar2.txt"
            CALL txtfile
            sound_file$ = "love_song.wav"
            CALL StartSound
            songcounter = songcounter + 1
        CASE 2
            txtfile_name$ = "guitar3.txt"
            CALL txtfile
            sound_file$ = "blues_song.wav"
            CALL StartSound
            songcounter = songcounter + 1
        CASE 3
            txtfile_name$ = "pray2.txt"
            CALL txtfile
            sound_file$ = "pray_song2.wav"
            CALL StartSound
            songcounter = songcounter + 1
        CASE 4
            txtfile_name$ = "guitar4.txt"
            CALL txtfile
            sound_file$ = "guitar_song.wav"
            CALL StartSound
            songcounter = 0
    END SELECT
    LINE INPUT "", dummy$
    CALL StopSound
END SUB

SUB dreams
    CLS
    didx = INT(RND * 7)
    txtfile_name$ = dream_files$(didx)
    CALL txtfile
    didx = INT(RND * 5)
    sound_file$ = dream_music$(didx)
    CALL StartSound
    LINE INPUT "", dummy$
    CALL StopSound
END SUB

SUB music
    CLS
    IF counter < 2 THEN
        txtfile_name$ = "music.txt"
        CALL txtfile
        sound_file$ = "fbsloop44.wav"
        CALL StartSound
    ELSE
        CLS
        txtfile_name$ = "music3.txt"
        CALL txtfile
        sound_file$ = "legends.ogg"
        CALL StartSound
    END IF
    LINE INPUT "", dummy$
    CALL StopSound
    CLS
END SUB

SUB outside
    SELECT CASE counter
        CASE 0, 1
            txtfile_name$ = "walk1.txt"
            CALL txtfile
            sound_file$ = "walk1.wav"
            CALL StartSound
            LINE INPUT "", dummy$
            CALL StopSound
        CASE 2
            txtfile_name$ = "yuka.txt"
            CALL txtfile
            sound_file$ = "dream_eva.wav"
            CALL StartSound
            PRINT
            PRINT
            PRINT "1. YOU NEED TO THINK ABOUT IT"
            PRINT "2. YES YOU WOULD LIKE TO TRY AND PARTICIPATE"
            LINE INPUT "Your choice: ", ok$
            CALL StopSound
            IF ok$ = "2" THEN
                PRINT
                PRINT "THE GUIDE SHAKES YOUR HAND AND SAY 'THEN SEE YOU NEXT TIME' AND SMILE"
                counter = counter + 1
            END IF
            LINE INPUT "", dummy$
        CASE 3
            txtfile_name$ = meeting_files$(meetingCounter)
            CALL txtfile
            sound_file$ = "dream_eva.wav"
            CALL StartSound
            LINE INPUT "", dummy$
            CALL StopSound
            meetingCounter = meetingCounter + 1
            IF meetingCounter > 3 THEN counter = 4
        CASE 4
            CLS
            txtfile_name$ = "workshop.txt"
            CALL txtfile
            sound_file$ = "dream_eva.wav"
            CALL StartSound
            LINE INPUT "", dummy$
            CALL StopSound
            finished = 1
    END SELECT
END SUB

SUB poem
    CLS
    CALL RestartPoem
    loadpoem_file$ = "poemDB.txt"
    CALL LoadPoemDB

    cp_row = 1
    cp_text$ = "A POEM..."
    CALL cp
    FOR pi = 1 TO 3
        PRINT
        IF places_count > 0 THEN
            pidx = INT(RND * places_count) + 1
            PRINT places$(pidx)
        END IF
        IF colors_count > 0 THEN
            pidx = INT(RND * colors_count) + 1
            PRINT colors_arr$(pidx)
        END IF
        IF metaphore_count > 0 THEN
            pidx = INT(RND * metaphore_count) + 1
            PRINT metaphore$(pidx)
        END IF
        IF objects_count > 0 THEN
            pidx = INT(RND * objects_count) + 1
            PRINT objects$(pidx)
        END IF
        IF good_count > 0 THEN
            pidx = INT(RND * good_count) + 1
            PRINT good_arr$(pidx)
        END IF
        IF bad_count > 0 THEN
            pidx = INT(RND * bad_count) + 1
            PRINT bad_arr$(pidx)
        END IF
    NEXT pi

    didx = INT(RND * 5)
    sound_file$ = dream_music$(didx)
    CALL StartSound
    LINE INPUT "", dummy$
    CALL StopSound
END SUB

SUB RestartPoem
    places_count = 0
    colors_count = 0
    metaphore_count = 0
    objects_count = 0
    good_count = 0
    bad_count = 0
END SUB

SUB LoadPoemDB
    lpf$ = DATA_PATH$ + loadpoem_file$
    OPEN lpf$ FOR INPUT AS #1

    WHILE NOT EOF(1)
        LINE INPUT #1, lpfline$
        lp_prefix$ = LEFT$(lpfline$, 2)
        lp_val$ = LTRIM$(RTRIM$(MID$(lpfline$, 3)))

        SELECT CASE lp_prefix$
            CASE "p:"
                places_count = places_count + 1
                places$(places_count) = lp_val$
            CASE "c:"
                colors_count = colors_count + 1
                colors_arr$(colors_count) = lp_val$
            CASE "m:"
                metaphore_count = metaphore_count + 1
                metaphore$(metaphore_count) = lp_val$
            CASE "o:"
                objects_count = objects_count + 1
                objects$(objects_count) = lp_val$
            CASE "g:"
                good_count = good_count + 1
                good_arr$(good_count) = lp_val$
            CASE "b:"
                bad_count = bad_count + 1
                bad_arr$(bad_count) = lp_val$
            CASE "e:"
                EXIT WHILE
        END SELECT
    WEND
    CLOSE #1
END SUB


REM ============================================================
REM SCREEN / UI SUBS
REM ============================================================

SUB warningscreen
    SCREEN 0
    txtfile_name$ = "warning.txt"
    CALL txtfile
    LINE INPUT "Press 1 to continue or 2 to quit: ", wk$
    IF wk$ = "2" THEN SYSTEM
END SUB

SUB opening
    SCREEN 0
    CLS
    PRINT
    PRINT
    PRINT
    PRINT
    cp_row = 6
    cp_text$ = "H I K I K O M O R Y"
    CALL cp
    PRINT
    cp_row = 8
    cp_text$ = "A GAME BY RON77"
    CALL cp
    PRINT
    PRINT
    cp_row = 12
    cp_text$ = "(RBBasic Edition)"
    CALL cp
    PRINT
    sound_file$ = "sabrina.wav"
    CALL StartSound
    LINE INPUT "", dummy$
END SUB

SUB upper
    CLS
    upper_lines_count = 0

    uf$ = DATA_PATH$ + upper_file$
    OPEN uf$ FOR INPUT AS #1
    WHILE NOT EOF(1)
        LINE INPUT #1, ufline$
        upper_lines_count = upper_lines_count + 1
        upper_lines$(upper_lines_count) = ufline$
    WEND
    CLOSE #1

    REM 25 rows for 800x600 screen
    uhi = 25

    REM Fill screen with blank lines to position at bottom
    FOR ui = 1 TO uhi - 1
        PRINT
    NEXT ui

    REM Print closing credits (each PRINT scrolls screen up)
    FOR ui = 1 TO upper_lines_count
        umsg$ = SPACE$(9) + upper_lines$(ui)
        PRINT umsg$
        SLEEP 1
    NEXT ui

    REM Scroll all text off screen
    FOR ui = 1 TO uhi
        PRINT
        SLEEP 1
    NEXT ui

    PRINT "END"
    LINE INPUT "", dummy$
END SUB


REM ============================================================
REM MAIN GAME LOOP
REM ============================================================

SUB gamemain
    SCREEN 0

    txtfile_name$ = "startpoint.txt"
    CALL txtfile
    LINE INPUT "", dummy$
    CALL StopSound

    DO
        CLS
        dates_months = timePass
        CALL dates1
        cp_row = 2
        cp_text$ = "DATE: " + dates_result$
        CALL cp
        cp_row = 4
        cp_text$ = "WHAT DO YOU WANT TO DO?"
        CALL cp
        cp_row = 6
        cp_text$ = "1. GO TO SLEEP..."
        CALL cp
        cp_row = 8
        cp_text$ = "2. PLAY A COMPUTER GAME..."
        CALL cp
        cp_row = 10
        cp_text$ = "3. LISTEN TO MUSIC..."
        CALL cp
        cp_row = 12
        cp_text$ = "4. CHECK E-MAIL..."
        CALL cp
        cp_row = 14
        cp_text$ = "5. GO OUTSIDE..."
        CALL cp
        cp_row = 16
        cp_text$ = "6. JOIN A CHAT-ROOM ON THE NET..."
        CALL cp
        cp_row = 18
        cp_text$ = "7. WRITE A POEM..."
        CALL cp
        cp_row = 20
        cp_text$ = "8. PLAY SOMETHING WITH YOUR GUITAR..."
        CALL cp
        cp_row = 22
        cp_text$ = "9. LISTEN TO THE NEWS..."
        CALL cp
        cp_row = 24
        cp_text$ = "TYPE Q TO EXIT GAME..."
        CALL cp

        LINE INPUT "Your choice: ", gk$

        SELECT CASE LCASE$(gk$)
            CASE "9"
                CALL news
            CASE "3"
                CALL music
            CASE "1"
                CALL dreams
            CASE "5"
                CALL outside
            CASE "4"
                CALL email
            CASE "2"
                SHELL "cmd /c start https://youtu.be/-CbyAk3Sn9I"
            CASE "6"
                IF counter < 2 THEN
                    conv_file$ = "chat1.txt"
                    CALL conversation
                ELSE
                    CALL chatroom
                END IF
            CASE "8"
                CALL guitar
            CASE "7"
                CALL poem
        END SELECT

        timePass = timePass + 1
    LOOP UNTIL LCASE$(gk$) = "q" OR finished = 1
END SUB
