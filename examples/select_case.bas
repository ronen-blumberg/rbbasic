REM SELECT CASE test program
REM Tests values, ranges, IS comparisons, CASE ELSE, and string SELECT

PRINT "=== SELECT CASE Tests ==="
PRINT

REM Test 1: Simple numeric values
score = 85
PRINT "Score:"; score
SELECT CASE score
    CASE 100
        PRINT "Perfect!"
    CASE 90
        PRINT "Excellent!"
    CASE 85
        PRINT "Very Good!"
    CASE 70
        PRINT "Good"
    CASE ELSE
        PRINT "Keep trying"
END SELECT
PRINT

REM Test 2: Ranges with TO
age = 25
PRINT "Age:"; age
SELECT CASE age
    CASE 0 TO 12
        PRINT "Child"
    CASE 13 TO 17
        PRINT "Teenager"
    CASE 18 TO 64
        PRINT "Adult"
    CASE 65 TO 120
        PRINT "Senior"
    CASE ELSE
        PRINT "Invalid age"
END SELECT
PRINT

REM Test 3: IS comparisons
temp = 72
PRINT "Temperature:"; temp
SELECT CASE temp
    CASE IS < 32
        PRINT "Freezing"
    CASE IS < 60
        PRINT "Cold"
    CASE IS < 80
        PRINT "Comfortable"
    CASE IS < 100
        PRINT "Hot"
    CASE ELSE
        PRINT "Extreme"
END SELECT
PRINT

REM Test 4: Multiple values per case
day = 3
PRINT "Day:"; day
SELECT CASE day
    CASE 1, 7
        PRINT "Weekend"
    CASE 2, 3, 4, 5, 6
        PRINT "Weekday"
    CASE ELSE
        PRINT "Invalid day"
END SELECT
PRINT

REM Test 5: String SELECT CASE
g$ = "B"
PRINT "Grade: "; g$
SELECT CASE g$
    CASE "A"
        PRINT "Excellent work!"
    CASE "B"
        PRINT "Good job!"
    CASE "C"
        PRINT "Average"
    CASE "D"
        PRINT "Below average"
    CASE "F"
        PRINT "Failing"
    CASE ELSE
        PRINT "Unknown grade"
END SELECT
PRINT

PRINT "All SELECT CASE tests passed!"
END
