REM TYPE...END TYPE test program

PRINT "=== TYPE...END TYPE Tests ==="
PRINT

REM Define a simple type
TYPE PersonType
    name AS STRING * 30
    age AS INTEGER
    score AS DOUBLE
END TYPE

REM Test 1: Create typed variable and set fields
PRINT "Test 1: Basic TYPE usage"
DIM person AS PersonType
person.name = "Alice"
person.age = 25
person.score = 95.5
PRINT "Name: "; person.name
PRINT "Age:"; person.age
PRINT "Score:"; person.score
PRINT

REM Test 2: Second typed variable
PRINT "Test 2: Multiple typed variables"
DIM person2 AS PersonType
person2.name = "Bob"
person2.age = 30
person2.score = 88.0
PRINT "Person 2: "; person2.name; " age"; person2.age
PRINT

REM Test 3: Modify fields
PRINT "Test 3: Modify fields"
person.age = 26
person.score = 97.0
PRINT "Updated age:"; person.age
PRINT "Updated score:"; person.score
PRINT

PRINT "All TYPE tests passed!"
END
