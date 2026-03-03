REM ============================================================
REM HOMELESSNESS - A MIND SIMULATION V2.0
REM Corrected for RBBasic compatibility
REM Original by ronblue77 (ron77)
REM ============================================================

SCREEN 0

REM Arrays need DIM, simple variables are implicit (all global in RBBasic)
DIM scenario$(3)
DIM status$(3)
DIM action$(5)
DIM home$(6)
DIM shelter$(6)
DIM money$(5)
DIM job$(3)
DIM drug$(4)

RANDOMIZE TIMER

FOR i = 1 TO 3
    READ r$
    scenario$(i) = r$
NEXT i
FOR i = 1 TO 3
    READ status$(i)
NEXT i
FOR i = 1 TO 5
    READ action$(i)
NEXT i
FOR i = 1 TO 6
    READ home$(i)
NEXT i
FOR i = 1 TO 6
    READ shelter$(i)
NEXT i
FOR i = 1 TO 5
    READ money$(i)
NEXT i
FOR i = 1 TO 3
    READ job$(i)
NEXT i
FOR i = 1 TO 4
    READ drug$(i)
NEXT i

REM Main game loop
DO
    d = 1
    over = 0
    health = 50
    dollars = 20
    CALL opening

    DO
        CALL game_turn
    LOOP UNTIL over = -1

    LINE INPUT "Play again? (y/n): ", reply$
    IF LCASE$(reply$) = "n" THEN END
LOOP

SUB opening
    CLS
    cp_row = 4: cp_text$ = "HOMELESSNESS - SURVIVING THE STREETS": CALL cp
    cp_row = 6: cp_text$ = "A MIND SIMULATION GAME": CALL cp
    cp_row = 8: cp_text$ = "by ronblue77 (ron77)": CALL cp
    cp_row = 10: cp_text$ = "IMPORTANT NOTE:": CALL cp
    cp_row = 11: cp_text$ = "THIS GAME IS MEANT TO BRING AWARENESS TO THE PROBLEM OF HOMELESSNESS": CALL cp
    cp_row = 12: cp_text$ = "I DO NOT MEAN TO MOCK OR INSULT ANYONE! PLEASE HELP PUT AN END TO HOMELESSNESS": CALL cp
    cp_row = 16: cp_text$ = "Type Q to quit or press ENTER to play": CALL cp
    LINE INPUT "", reply$
    IF LCASE$(reply$) = "q" THEN
        cp_row = 19: cp_text$ = "GOODBYE!": CALL cp
        END
    END IF
    CLS
    LOCATE 6, 14: INPUT "Enter name: ", playername$

    msg$ = playername$ + scenario$(INT(RND * 3 + 1))
    PRINT msg$: PRINT
    PRINT "GOOD LUCK!"
    LINE INPUT "Press ENTER to continue...", reply$
END SUB


SUB game_turn
    CLS
    cp_row = 1: cp_text$ = "Type Q to quit": CALL cp
    cp_row = 2: cp_text$ = "Day: " + STR$(d) + " Health (0-100%): " + STR$(health) + " Money: " + STR$(dollars) + "$": CALL cp
    LOCATE 4, 10: PRINT status$(INT(RND * 3 + 1))
    PRINT
    FOR i = 1 TO 5
        PRINT action$(i): PRINT
    NEXT i

    REM Choice loop - repeats if player picks an invalid option
    valid_choice = 0
    WHILE valid_choice = 0
        LINE INPUT "Your choice (1-5, Q to quit): ", ac$
        IF LCASE$(ac$) = "q" THEN over = -1: EXIT SUB

        valid_choice = 1
        SELECT CASE ac$
            CASE "1"
                result$ = money$(INT(RND * 5 + 1))
                PRINT result$
                IF result$ = money$(4) THEN over = -1
                IF result$ = money$(1) OR result$ = money$(2) OR result$ = money$(3) THEN
                    dollars = dollars + 20
                    health = health + 25
                END IF
            CASE "2"
                result$ = shelter$(INT(RND * 6 + 1))
                PRINT result$
                IF result$ = shelter$(4) THEN over = -1
            CASE "3"
                result$ = home$(INT(RND * 6 + 1))
                PRINT result$
                IF result$ = home$(5) THEN over = -1
            CASE "4"
                IF health > 20 THEN
                    result$ = job$(INT(RND * 3 + 1))
                    PRINT result$
                    IF result$ = job$(2) THEN over = -1
                ELSE
                    PRINT "NOBODY WILL HIRE YOU UNTIL YOUR HEALTH GETS BETTER. CHOOSE ANOTHER OPTION"
                    valid_choice = 0
                END IF
            CASE "5"
                IF dollars >= 20 THEN
                    health = health - 20
                    result$ = drug$(INT(RND * 4 + 1))
                    PRINT result$
                    IF result$ = drug$(4) THEN over = -1
                ELSE
                    PRINT "YOU DON'T HAVE ENOUGH MONEY. CHOOSE ANOTHER OPTION"
                    valid_choice = 0
                END IF
            CASE ELSE
                PRINT "Invalid choice. Please enter 1-5 or Q to quit."
                valid_choice = 0
        END SELECT
    WEND

    LINE INPUT "Press ENTER to continue...", reply$
    PRINT

    REM Check if game is over from the action result
    IF over = -1 THEN
        IF health <= 0 OR result$ = drug$(4) OR result$ = shelter$(4) OR result$ = money$(4) THEN
            PRINT "YOU DIED ON THE STREETS - GAME OVER"
        ELSE
            PRINT "YOU SURVIVED THE STREETS - GAME OVER": PRINT
        END IF
        msg$ = "YOU SURVIVED ON THE STREETS FOR: " + STR$(d) + " DAYS"
        PRINT msg$
        SLEEP 3
        EXIT SUB
    END IF

    REM Advance to next day
    d = d + 1
    dollars = dollars - 10
    health = health - 10
    IF health > 100 THEN health = 100
    IF dollars < 0 THEN dollars = 0

    REM Check if health ran out
    IF health <= 0 THEN
        over = -1
        PRINT "YOU DIED ON THE STREETS - GAME OVER"
        msg$ = "YOU SURVIVED ON THE STREETS FOR: " + STR$(d) + " DAYS"
        PRINT msg$
        SLEEP 3
        EXIT SUB
    END IF

    CLS
    LINE INPUT "Press ENTER to continue to the next day...", reply$
    CLS
END SUB


SUB cp
    LOCATE cp_row, (80 - LEN(cp_text$)) / 2: PRINT cp_text$
END SUB


DATA " you are a runaway teenager who ran from home... you need to survive on the streets until you can find a home."
DATA " you are a middle-aged person whose wife filed for divorce and threw you out of your house... you live in your old car until you get a new place to live."
DATA " you are broke with no job and no family... you have been kicked out of your home. Now you have to find a solution to your problem."

DATA "It's morning, rise and shine... you almost don't have any money and you are hungry."
DATA "It's noon. You have been sitting and begging for change from passing people. Some have given you some money."
DATA "It's night. You are tired and need to find a safe place to sleep."

DATA "Press 1 to: go beg or look for food or money"
DATA "Press 2 to: find help and/or shelter"
DATA "Press 3 to: call from a street phone to a family member or a friend to ask for help"
DATA "Press 4 to: try to find a job or work"
DATA "Press 5 to: do drugs or drink alcohol to forget the pain, suffering, and loneliness"

DATA "You call your grandma and you both cry... she promises to try to get you some help... she sends you her love... you promise to keep in touch with her."
DATA "You call a friend and she gives you some money and lets you stay at her place for the night... you get to have a good shower, some clean clothes, and eat a good meal, then you're back on the streets again."
DATA "You call a hotline for aid to homeless persons... they come and pick you up, give you some food and shelter for the night. You talk to a social worker and he gives you his number just in case you need it."
DATA "You call a relative but no one answers... you are left alone on the streets."
DATA "You call your family and they pick you up and let you stay for as long as you need... you are off the streets!"
DATA "You call but there is no answer."

DATA "You go to a shelter for homeless people... you are robbed of your money by violent addicts... you manage to run away from there and spend the rest of the night at a park crying."
DATA "You try to sleep at the park but you get arrested by a policeman... you spend a few hours in jail where you are beaten by inmates... by morning you are on the street again."
DATA "You have enough money to rent a motel room for the night... you shower and wash your clothes and sleep in a warm bed and dream you have a home."
DATA "You sleep at the park on a bench where you freeze to death because of the cold weather... you are now in heaven. RIP."
DATA "You sleep on the street on a corner. When you wake up you find a 20-dollar bill and a note that says 'May God help you, this is the little I can do.'"
DATA "You are arrested by the police and now instead of surviving on the street you have to survive being in jail."

DATA "You sit for hours and people pass you by as if you are invisible. Then some kind lady comes and invites you to eat at a diner at her expense... you talk with her for two hours and tell her your story... then she says 'bye.'"
DATA "You sit at a corner and people give you coins and dollars. You get enough money to get by for a few days."
DATA "You sit and some man offers you work for some money and a meal. You accept... you go to his house and help him paint his house, then he gives you 10 dollars and a home-cooked meal."
DATA "No one gives you anything! You're hungry and you have no choice but to look for food in the garbage... you get sick... very sick and you die on the streets from food poisoning... RIP."
DATA "You sit all day for nothing. Nobody gives you anything."

DATA "You try to find work but nobody is interested."
DATA "You find work as a dishwasher at a diner. After a week you can afford a place to call home. You are off the street!"
DATA "You find work but you are fired really quickly."

DATA "You get drunk and stoned from cheap alcohol and for a few hours you forget about all your problems."
DATA "You do cheap street drugs and you get sick and collapse on the street... you wake up at the hospital and when you are better you are out on the street again."
DATA "You get stoned and pass out. When you wake in the morning everything you had is gone - you got mugged."
DATA "You do drugs and overdose and die on the streets. RIP."
