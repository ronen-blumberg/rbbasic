REM ============================================================
REM HIKIKOMORI GAME - RBBasic GRAPHICS VERSION
REM All output via _PRINTSTRING, all input via _KEYDOWN
REM 800x600 pixel 32-bit color graphic screen
REM Original by ron77 - Graphics version
REM ============================================================

RANDOMIZE TIMER

REM ====== Graphics Setup ======
SCREEN 14
_DISPLAY

REM ====== Color Palette Setup ======
REM Palette values as decimal (RRGGBB packed): index, packed_rgb
PALETTE 0, 1710638
PALETTE 1, 65345
PALETTE 2, 16776960
PALETTE 3, 16729156
PALETTE 4, 4491519
PALETTE 5, 16777215
PALETTE 6, 8947848
PALETTE 7, 16746496
PALETTE 8, 16711935
PALETTE 9, 65535
PALETTE 10, 4521796
PALETTE 11, 16755200
PALETTE 12, 2263074
PALETTE 13, 6693512
PALETTE 14, 16737928
PALETTE 15, 13421772

REM ====== Graphics State ======
CONST GFX_COLS = 100
CONST GFX_ROWS = 75
CONST GFX_CHARW = 8
CONST GFX_CHARH = 8
gfx_row = 0
gfx_col = 0
gfx_color = 1
gfx_bgcolor = 0

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

REM ====== Graphics I/O shared variables ======
gc_row = 0
gc_clr = 0
gl_row = 0
gl_col = 0

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

REM ====== Chat history for chatbot display ======
CONST MAX_CHAT = 60
DIM chat_lines$(MAX_CHAT)
DIM chat_colors(MAX_CHAT)
chat_count = 0

REM ====== Set initial color ======
COLOR 1

REM ====== MAIN PROGRAM FLOW ======
CALL warningscreen
CALL opening
CALL gamemain
upper_file$ = "ending_titles.txt"
CALL upper
SYSTEM


REM ============================================================
REM GRAPHICS I/O SYSTEM
REM ============================================================

SUB GCls
    LINE (0, 0)-(799, 599), 0, BF
    COLOR gfx_color
    gfx_row = 0
    gfx_col = 0
    _DISPLAY
END SUB

SUB GClsNoFlip
    LINE (0, 0)-(799, 599), 0, BF
    COLOR gfx_color
    gfx_row = 0
    gfx_col = 0
END SUB

SUB GPrint
    IF gfx_row < GFX_ROWS THEN
        _PRINTSTRING (gfx_col * GFX_CHARW, gfx_row * GFX_CHARH), gp_text$
    END IF
    gfx_row = gfx_row + 1
    gfx_col = 0
    _DISPLAY
END SUB

SUB GPrintNoFlip
    IF gfx_row < GFX_ROWS THEN
        _PRINTSTRING (gfx_col * GFX_CHARW, gfx_row * GFX_CHARH), gp_text$
    END IF
    gfx_row = gfx_row + 1
    gfx_col = 0
END SUB

SUB GPrintNL
    gfx_row = gfx_row + 1
    gfx_col = 0
END SUB

SUB GPrintSameLine
    IF gfx_row < GFX_ROWS THEN
        _PRINTSTRING (gfx_col * GFX_CHARW, gfx_row * GFX_CHARH), gp_text$
    END IF
    gfx_col = gfx_col + LEN(gp_text$)
END SUB

SUB GLocate
    gfx_row = gl_row
    gfx_col = gl_col
END SUB

SUB GCenterPrint
    gc_col = (GFX_COLS - LEN(gc_text$)) / 2
    IF gc_col < 0 THEN gc_col = 0
    IF gc_row < GFX_ROWS THEN
        _PRINTSTRING (gc_col * GFX_CHARW, gc_row * GFX_CHARH), gc_text$
    END IF
END SUB

SUB GColor
    COLOR gc_clr
    gfx_color = gc_clr
END SUB

SUB GWaitKey
    REM Show prompt at bottom
    gc_clr = 6
    CALL GColor
    _PRINTSTRING (8, 580), "Press ENTER to continue..."
    _DISPLAY

    REM Wait for Enter release first (debounce)
    gw_debounce = 1
    DO
        _LIMIT 30
        IF _KEYDOWN(13) = 0 THEN gw_debounce = 0
        IF gw_debounce = 0 AND _KEYDOWN(13) THEN EXIT DO
    LOOP

    REM Wait for Enter release
    DO
        _LIMIT 30
        IF _KEYDOWN(13) = 0 THEN EXIT DO
    LOOP

    gc_clr = 1
    CALL GColor
END SUB

SUB GInput
    gi_result$ = ""
    gi_cursor_timer = 0
    gi_cursor_on = 1
    gi_cooldown = 0

    REM Show prompt
    IF LEN(gi_prompt$) > 0 THEN
        gp_text$ = gi_prompt$
        CALL GPrintSameLine
    END IF
    gi_startcol = gfx_col
    gi_startrow = gfx_row

    REM Wait for all keys released first
    gi_wait = 1
    DO
        _LIMIT 30
        gi_any = 0
        IF _KEYDOWN(13) THEN gi_any = 1
        IF _KEYDOWN(8) THEN gi_any = 1
        FOR gi_wk = 32 TO 122
            IF _KEYDOWN(gi_wk) THEN gi_any = 1
        NEXT gi_wk
        IF gi_any = 0 THEN gi_wait = 0
        IF gi_wait = 0 THEN EXIT DO
    LOOP

    DO
        _LIMIT 30

        REM Blink cursor
        gi_cursor_timer = gi_cursor_timer + 1
        IF gi_cursor_timer > 15 THEN
            IF gi_cursor_on = 1 THEN
                gi_cursor_on = 0
            ELSE
                gi_cursor_on = 1
            END IF
            gi_cursor_timer = 0
        END IF

        IF gi_cooldown > 0 THEN gi_cooldown = gi_cooldown - 1

        IF gi_cooldown = 0 THEN
            gi_shifted = 0
            IF _KEYDOWN(1073742049) THEN gi_shifted = 1
            IF _KEYDOWN(1073742053) THEN gi_shifted = 1

            REM Check Enter
            IF _KEYDOWN(13) THEN EXIT DO

            REM Check Backspace
            IF _KEYDOWN(8) THEN
                IF LEN(gi_result$) > 0 THEN
                    gi_result$ = LEFT$(gi_result$, LEN(gi_result$) - 1)
                    gi_cooldown = 5
                END IF
            END IF

            REM Check letter keys a-z
            IF gi_cooldown = 0 THEN
                FOR gi_k = 97 TO 122
                    IF _KEYDOWN(gi_k) THEN
                        IF gi_shifted = 1 THEN
                            gi_result$ = gi_result$ + CHR$(gi_k - 32)
                        ELSE
                            gi_result$ = gi_result$ + CHR$(gi_k)
                        END IF
                        gi_cooldown = 5
                        EXIT FOR
                    END IF
                NEXT gi_k
            END IF

            REM Check number keys 0-9
            IF gi_cooldown = 0 THEN
                FOR gi_k = 48 TO 57
                    IF _KEYDOWN(gi_k) THEN
                        IF gi_shifted = 1 THEN
                            REM Shifted number keys: special chars
                            SELECT CASE gi_k
                                CASE 49
                                    gi_result$ = gi_result$ + "!"
                                CASE 50
                                    gi_result$ = gi_result$ + "@"
                                CASE 51
                                    gi_result$ = gi_result$ + "#"
                                CASE 52
                                    gi_result$ = gi_result$ + "$"
                                CASE 53
                                    gi_result$ = gi_result$ + "%"
                                CASE 55
                                    gi_result$ = gi_result$ + "&"
                                CASE 57
                                    gi_result$ = gi_result$ + "("
                                CASE 48
                                    gi_result$ = gi_result$ + ")"
                            END SELECT
                        ELSE
                            gi_result$ = gi_result$ + CHR$(gi_k)
                        END IF
                        gi_cooldown = 5
                        EXIT FOR
                    END IF
                NEXT gi_k
            END IF

            REM Check space
            IF gi_cooldown = 0 THEN
                IF _KEYDOWN(32) THEN
                    gi_result$ = gi_result$ + " "
                    gi_cooldown = 5
                END IF
            END IF

            REM Check punctuation keys
            IF gi_cooldown = 0 THEN
                IF _KEYDOWN(46) THEN
                    IF gi_shifted = 1 THEN
                        gi_result$ = gi_result$ + ">"
                    ELSE
                        gi_result$ = gi_result$ + "."
                    END IF
                    gi_cooldown = 5
                END IF
            END IF
            IF gi_cooldown = 0 THEN
                IF _KEYDOWN(44) THEN
                    IF gi_shifted = 1 THEN
                        gi_result$ = gi_result$ + "<"
                    ELSE
                        gi_result$ = gi_result$ + ","
                    END IF
                    gi_cooldown = 5
                END IF
            END IF
            IF gi_cooldown = 0 THEN
                IF _KEYDOWN(45) THEN
                    gi_result$ = gi_result$ + "-"
                    gi_cooldown = 5
                END IF
            END IF
            IF gi_cooldown = 0 THEN
                IF _KEYDOWN(47) THEN
                    IF gi_shifted = 1 THEN
                        gi_result$ = gi_result$ + "?"
                    ELSE
                        gi_result$ = gi_result$ + "/"
                    END IF
                    gi_cooldown = 5
                END IF
            END IF
            IF gi_cooldown = 0 THEN
                IF _KEYDOWN(39) THEN
                    gi_result$ = gi_result$ + "'"
                    gi_cooldown = 5
                END IF
            END IF
        END IF

        REM Redraw input area
        gi_clearw = GFX_COLS - gi_startcol
        IF gi_clearw < 1 THEN gi_clearw = 1
        LINE (gi_startcol * GFX_CHARW, gi_startrow * GFX_CHARH)-(799, gi_startrow * GFX_CHARH + GFX_CHARH - 1), 0, BF
        COLOR gfx_color
        IF LEN(gi_result$) > 0 THEN
            _PRINTSTRING (gi_startcol * GFX_CHARW, gi_startrow * GFX_CHARH), gi_result$
        END IF

        REM Draw cursor
        IF gi_cursor_on = 1 THEN
            gi_cx = (gi_startcol + LEN(gi_result$)) * GFX_CHARW
            IF gi_cx < 792 THEN
                LINE (gi_cx, gi_startrow * GFX_CHARH)-(gi_cx + GFX_CHARW - 1, gi_startrow * GFX_CHARH + GFX_CHARH - 1), gfx_color, BF
            END IF
        END IF

        _DISPLAY
    LOOP

    gfx_row = gi_startrow + 1
    gfx_col = 0
END SUB

SUB GSlow
    REM Typewriter effect: print one char at a time
    FOR gs_i = 1 TO LEN(slow_text$)
        gs_ch$ = MID$(slow_text$, gs_i, 1)
        IF gfx_col < GFX_COLS AND gfx_row < GFX_ROWS THEN
            _PRINTSTRING (gfx_col * GFX_CHARW, gfx_row * GFX_CHARH), gs_ch$
        END IF
        gfx_col = gfx_col + 1
        _DISPLAY
        _LIMIT 20
    NEXT gs_i
END SUB


REM ============================================================
REM UTILITY SUBS
REM ============================================================

SUB txtfile
    CALL GCls
    tf$ = DATA_PATH$ + txtfile_name$
    OPEN tf$ FOR INPUT AS #1
    WHILE NOT EOF(1)
        LINE INPUT #1, tbuf$
        gp_text$ = tbuf$
        CALL GPrintNoFlip
        IF gfx_row > 70 THEN
            gc_clr = 6
            CALL GColor
            gp_text$ = "-- more --"
            CALL GPrint
            gc_clr = 1
            CALL GColor
            CALL GWaitKey
            CALL GCls
        END IF
    WEND
    CLOSE #1
    _DISPLAY
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
        REM Get input from user on graphics screen
        pgr_prompt$ = person_you$(pid) + ": "
        gc_clr = 7
        CALL GColor
        gi_prompt$ = pgr_prompt$
        CALL GInput
        pgr_inpt$ = gi_result$
        gc_clr = 1
        CALL GColor
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
    gc_clr = 9
    CALL GColor
    pst_msg$ = person_script$(pid) + ": "
    gp_text$ = pst_msg$
    CALL GPrintSameLine
    gc_clr = 1
    CALL GColor
    slow_text$ = speak_text$
    CALL GSlow
    CALL GPrintNL
END SUB

SUB ChatAddLine
    REM Add a line to chat history using shared ca_text$, ca_color
    IF chat_count < MAX_CHAT THEN
        chat_count = chat_count + 1
    ELSE
        REM Shift lines up
        FOR ca_i = 1 TO MAX_CHAT - 1
            chat_lines$(ca_i) = chat_lines$(ca_i + 1)
            chat_colors(ca_i) = chat_colors(ca_i + 1)
        NEXT ca_i
    END IF
    chat_lines$(chat_count) = ca_text$
    chat_colors(chat_count) = ca_color
END SUB

SUB ChatRedraw
    REM Redraw chat history on screen
    CALL GClsNoFlip
    gc_clr = 2
    CALL GColor
    gc_row = 0
    gc_text$ = "=== CHAT ROOM ==="
    CALL GCenterPrint

    REM Show last N lines that fit on screen (rows 2 to 68)
    cr_maxlines = 66
    cr_start = 1
    IF chat_count > cr_maxlines THEN cr_start = chat_count - cr_maxlines + 1

    cr_row = 2
    FOR cr_i = cr_start TO chat_count
        gc_clr = chat_colors(cr_i)
        CALL GColor
        gfx_row = cr_row
        gfx_col = 1
        gp_text$ = chat_lines$(cr_i)
        CALL GPrintNoFlip
        cr_row = cr_row + 1
    NEXT cr_i

    gc_clr = 1
    CALL GColor
    gfx_row = cr_row + 1
    gfx_col = 0
    _DISPLAY
END SUB


REM ============================================================
REM GAME ACTIVITY SUBS
REM ============================================================

SUB conversation
    current_pid = 0
    CALL PersonRestart

    CALL GCls
    gc_clr = 6
    CALL GColor
    gl_row = 5
    gl_col = 5
    CALL GLocate
    gp_text$ = "connecting..."
    CALL GPrint
    SLEEP 2
    CALL GCls

    gc_clr = 1
    CALL GColor
    loadscript_file$ = conv_file$
    CALL PersonLoadArrays

    REM Init chat display
    chat_count = 0
    ca_text$ = person_greeting$(0)
    ca_color = 9
    CALL ChatAddLine
    CALL ChatRedraw

    DO
        reply_switch = 0
        gfx_row = chat_count + 4
        IF gfx_row > 68 THEN gfx_row = 68
        gfx_col = 0
        CALL PersonGetReply
        conv_rply$ = reply_result$

        REM Add user input to chat
        ca_text$ = person_you$(0) + ": " + gi_result$
        ca_color = 7
        CALL ChatAddLine

        REM Add bot reply to chat
        current_pid = 0
        ca_text$ = person_script$(0) + ": " + conv_rply$
        ca_color = 9
        CALL ChatAddLine
        CALL ChatRedraw

    LOOP UNTIL conv_rply$ = "Goodbye!"

    CALL GCls
    gc_clr = 6
    CALL GColor
    gl_row = 5
    gl_col = 5
    CALL GLocate
    gp_text$ = "disconnected..."
    CALL GPrint
    CALL GWaitKey
END SUB

SUB chatroom
    current_pid = 0
    CALL PersonRestart
    current_pid = 1
    CALL PersonRestart
    current_pid = 2
    CALL PersonRestart

    CALL GCls
    gc_clr = 6
    CALL GColor
    gl_row = 5
    gl_col = 5
    CALL GLocate
    gp_text$ = "connecting..."
    CALL GPrint
    SLEEP 2
    CALL GCls

    gc_clr = 1
    CALL GColor
    current_pid = 0
    loadscript_file$ = "yuka_guide.txt"
    CALL PersonLoadArrays
    current_pid = 1
    loadscript_file$ = "mike.txt"
    CALL PersonLoadArrays
    current_pid = 2
    loadscript_file$ = "ken.txt"
    CALL PersonLoadArrays

    REM Init chat display
    chat_count = 0
    ca_text$ = person_greeting$(0)
    ca_color = 9
    CALL ChatAddLine
    CALL ChatRedraw

    DO
        REM Get user input
        gfx_row = chat_count + 4
        IF gfx_row > 68 THEN gfx_row = 68
        gfx_col = 0
        gc_clr = 7
        CALL GColor
        gi_prompt$ = person_you$(0) + ": "
        CALL GInput
        cr_inText$ = gi_result$

        cr_lc$ = LCASE$(cr_inText$)
        IF cr_lc$ = "q" OR cr_lc$ = "x" OR cr_lc$ = "goodbye" OR cr_lc$ = "good night" OR cr_lc$ = "bye" THEN
            ca_text$ = person_script$(0) + ": Goodbye!"
            ca_color = 9
            CALL ChatAddLine
            CALL ChatRedraw
            EXIT DO
        END IF

        REM Add user line
        ca_text$ = person_you$(0) + ": " + cr_inText$
        ca_color = 7
        CALL ChatAddLine

        REM Get replies from all 3 bots
        current_pid = 0
        reply_input$ = cr_inText$
        reply_switch = 1
        CALL PersonGetReply
        ca_text$ = person_script$(0) + ": " + reply_result$
        ca_color = 9
        CALL ChatAddLine

        current_pid = 1
        reply_input$ = cr_inText$
        reply_switch = 1
        CALL PersonGetReply
        ca_text$ = person_script$(1) + ": " + reply_result$
        ca_color = 10
        CALL ChatAddLine

        current_pid = 2
        reply_input$ = cr_inText$
        reply_switch = 1
        CALL PersonGetReply
        ca_text$ = person_script$(2) + ": " + reply_result$
        ca_color = 4
        CALL ChatAddLine

        CALL ChatRedraw
    LOOP UNTIL LCASE$(cr_inText$) = "bye"

    CALL GCls
    gc_clr = 6
    CALL GColor
    gl_row = 5
    gl_col = 5
    CALL GLocate
    gp_text$ = "disconnected..."
    CALL GPrint
    CALL GWaitKey
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

    CALL GCls
    gc_clr = 2
    CALL GColor
    gp_text$ = "NEWS FEED"
    CALL GPrint
    CALL GPrintNL

    gc_clr = 1
    CALL GColor
    IF newsbla_count > 0 THEN
        FOR ni = 1 TO 10
            nidx = INT(RND * newsbla_count) + 1
            gp_text$ = newsbla$(nidx)
            CALL GPrint
            CALL GPrintNL
        NEXT ni
    END IF

    CALL GWaitKey
END SUB

SUB email
    CALL GCls
    SELECT CASE counter
        CASE 0
            gc_clr = 6
            CALL GColor
            gc_row = 5
            gc_text$ = "NO EMAIL..."
            CALL GCenterPrint
            _DISPLAY
            counter = counter + 1
        CASE 1
            txtfile_name$ = "email1.txt"
            CALL txtfile
            sound_file$ = "email1.wav"
            CALL StartSound
            CALL GPrintNL
            CALL GPrintNL
            gc_clr = 2
            CALL GColor
            gp_text$ = "1. REPLY OR 2. IGNORE"
            CALL GPrint
            gc_clr = 7
            CALL GColor
            gi_prompt$ = "Your choice: "
            CALL GInput
            ek$ = gi_result$
            IF ek$ = "2" THEN
                CALL StopSound
                EXIT SUB
            END IF
            IF ek$ = "1" THEN
                CALL GPrintNL
                CALL GPrintNL
                gc_clr = 1
                CALL GColor
                gp_text$ = "YOU REPLY TO YUKA-YUKA CENTER"
                CALL GPrint
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
    CALL GWaitKey
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
    CALL GWaitKey
    CALL StopSound
END SUB

SUB dreams
    CALL GCls
    didx = INT(RND * 7)
    txtfile_name$ = dream_files$(didx)
    CALL txtfile
    didx = INT(RND * 5)
    sound_file$ = dream_music$(didx)
    CALL StartSound
    CALL GWaitKey
    CALL StopSound
END SUB

SUB music
    CALL GCls
    IF counter < 2 THEN
        txtfile_name$ = "music.txt"
        CALL txtfile
        sound_file$ = "fbsloop44.wav"
        CALL StartSound
    ELSE
        CALL GCls
        txtfile_name$ = "music3.txt"
        CALL txtfile
        sound_file$ = "legends.ogg"
        CALL StartSound
    END IF
    CALL GWaitKey
    CALL StopSound
    CALL GCls
END SUB

SUB outside
    SELECT CASE counter
        CASE 0, 1
            txtfile_name$ = "walk1.txt"
            CALL txtfile
            sound_file$ = "walk1.wav"
            CALL StartSound
            CALL GWaitKey
            CALL StopSound
        CASE 2
            txtfile_name$ = "yuka.txt"
            CALL txtfile
            sound_file$ = "dream_eva.wav"
            CALL StartSound
            CALL GPrintNL
            CALL GPrintNL
            gc_clr = 2
            CALL GColor
            gp_text$ = "1. YOU NEED TO THINK ABOUT IT"
            CALL GPrint
            gp_text$ = "2. YES YOU WOULD LIKE TO TRY AND PARTICIPATE"
            CALL GPrint
            gc_clr = 7
            CALL GColor
            gi_prompt$ = "Your choice: "
            CALL GInput
            ok$ = gi_result$
            CALL StopSound
            IF ok$ = "2" THEN
                CALL GPrintNL
                gc_clr = 1
                CALL GColor
                gp_text$ = "THE GUIDE SHAKES YOUR HAND AND SAY 'THEN SEE YOU NEXT TIME' AND SMILE"
                CALL GPrint
                counter = counter + 1
            END IF
            CALL GWaitKey
        CASE 3
            txtfile_name$ = meeting_files$(meetingCounter)
            CALL txtfile
            sound_file$ = "dream_eva.wav"
            CALL StartSound
            CALL GWaitKey
            CALL StopSound
            meetingCounter = meetingCounter + 1
            IF meetingCounter > 3 THEN counter = 4
        CASE 4
            CALL GCls
            txtfile_name$ = "workshop.txt"
            CALL txtfile
            sound_file$ = "dream_eva.wav"
            CALL StartSound
            CALL GWaitKey
            CALL StopSound
            finished = 1
    END SELECT
END SUB

SUB poem
    CALL GCls
    CALL RestartPoem
    loadpoem_file$ = "poemDB.txt"
    CALL LoadPoemDB

    gc_clr = 2
    CALL GColor
    gc_row = 1
    gc_text$ = "A POEM..."
    CALL GCenterPrint
    gfx_row = 3
    gfx_col = 0

    gc_clr = 5
    CALL GColor
    FOR pi = 1 TO 3
        CALL GPrintNL
        IF places_count > 0 THEN
            pidx = INT(RND * places_count) + 1
            gp_text$ = places$(pidx)
            CALL GPrintNoFlip
        END IF
        IF colors_count > 0 THEN
            pidx = INT(RND * colors_count) + 1
            gp_text$ = colors_arr$(pidx)
            CALL GPrintNoFlip
        END IF
        IF metaphore_count > 0 THEN
            pidx = INT(RND * metaphore_count) + 1
            gp_text$ = metaphore$(pidx)
            CALL GPrintNoFlip
        END IF
        IF objects_count > 0 THEN
            pidx = INT(RND * objects_count) + 1
            gp_text$ = objects$(pidx)
            CALL GPrintNoFlip
        END IF
        IF good_count > 0 THEN
            pidx = INT(RND * good_count) + 1
            gp_text$ = good_arr$(pidx)
            CALL GPrintNoFlip
        END IF
        IF bad_count > 0 THEN
            pidx = INT(RND * bad_count) + 1
            gp_text$ = bad_arr$(pidx)
            CALL GPrintNoFlip
        END IF
    NEXT pi

    _DISPLAY
    didx = INT(RND * 5)
    sound_file$ = dream_music$(didx)
    CALL StartSound
    CALL GWaitKey
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
    CALL GCls
    gc_clr = 3
    CALL GColor
    txtfile_name$ = "warning.txt"
    CALL txtfile
    gc_clr = 7
    CALL GColor
    gi_prompt$ = "Press 1 to continue or 2 to quit: "
    CALL GInput
    wk$ = gi_result$
    IF wk$ = "2" THEN SYSTEM
END SUB

SUB opening
    CALL GCls

    REM Try to load and display opening image
    op_img = _LOADIMAGE(IMG_PATH$ + "opening.png")
    IF op_img > 0 THEN
        _PUTIMAGE (0, 0), op_img
        _FREEIMAGE op_img
    END IF

    gc_clr = 2
    CALL GColor
    gc_row = 52
    gc_text$ = "H I K I K O M O R Y"
    CALL GCenterPrint
    gc_clr = 5
    CALL GColor
    gc_row = 55
    gc_text$ = "A GAME BY RON77"
    CALL GCenterPrint
    gc_clr = 4
    CALL GColor
    gc_row = 58
    gc_text$ = "(RBBasic Graphics Edition)"
    CALL GCenterPrint
    _DISPLAY

    sound_file$ = "sabrina.wav"
    CALL StartSound
    CALL GWaitKey
END SUB

SUB upper
    CALL GCls
    upper_lines_count = 0

    uf$ = DATA_PATH$ + upper_file$
    OPEN uf$ FOR INPUT AS #1
    WHILE NOT EOF(1)
        LINE INPUT #1, ufline$
        upper_lines_count = upper_lines_count + 1
        upper_lines$(upper_lines_count) = ufline$
    WEND
    CLOSE #1

    gc_clr = 2
    CALL GColor

    REM Pixel-by-pixel scrolling credits
    up_total = (upper_lines_count + GFX_ROWS) * GFX_CHARH
    FOR up_offset = 0 TO up_total
        CALL GClsNoFlip
        FOR ui = 1 TO upper_lines_count
            uy = ((ui - 1) * GFX_CHARH) - up_offset + 600
            IF uy > -8 AND uy < 600 THEN
                up_pad$ = SPACE$(12) + upper_lines$(ui)
                _PRINTSTRING (0, uy), up_pad$
            END IF
        NEXT ui
        _DISPLAY
        _LIMIT 20
    NEXT up_offset

    gc_clr = 5
    CALL GColor
    CALL GCls
    gc_row = 37
    gc_text$ = "E N D"
    CALL GCenterPrint
    _DISPLAY
    CALL GWaitKey
END SUB


REM ============================================================
REM MAIN GAME LOOP
REM ============================================================

SUB gamemain
    txtfile_name$ = "startpoint.txt"
    CALL txtfile
    CALL GWaitKey
    CALL StopSound

    DO
        CALL GCls
        dates_months = timePass
        CALL dates1

        gc_clr = 2
        CALL GColor
        gc_row = 5
        gc_text$ = "DATE: " + dates_result$
        CALL GCenterPrint

        gc_clr = 5
        CALL GColor
        gc_row = 10
        gc_text$ = "WHAT DO YOU WANT TO DO?"
        CALL GCenterPrint

        gc_clr = 1
        CALL GColor
        gc_row = 15
        gc_text$ = "1. GO TO SLEEP..."
        CALL GCenterPrint
        gc_row = 19
        gc_text$ = "2. PLAY A COMPUTER GAME..."
        CALL GCenterPrint
        gc_row = 23
        gc_text$ = "3. LISTEN TO MUSIC..."
        CALL GCenterPrint
        gc_row = 27
        gc_text$ = "4. CHECK E-MAIL..."
        CALL GCenterPrint
        gc_row = 31
        gc_text$ = "5. GO OUTSIDE..."
        CALL GCenterPrint
        gc_row = 35
        gc_text$ = "6. JOIN A CHAT-ROOM ON THE NET..."
        CALL GCenterPrint
        gc_row = 39
        gc_text$ = "7. WRITE A POEM..."
        CALL GCenterPrint
        gc_row = 43
        gc_text$ = "8. PLAY SOMETHING WITH YOUR GUITAR..."
        CALL GCenterPrint
        gc_row = 47
        gc_text$ = "9. LISTEN TO THE NEWS..."
        CALL GCenterPrint

        gc_clr = 3
        CALL GColor
        gc_row = 53
        gc_text$ = "TYPE Q TO EXIT GAME..."
        CALL GCenterPrint
        _DISPLAY

        gc_clr = 7
        CALL GColor
        gfx_row = 58
        gfx_col = 30
        gi_prompt$ = "Your choice: "
        CALL GInput
        gk$ = gi_result$

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
