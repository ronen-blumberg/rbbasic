#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// Numeric variables
double GUESS = 0.0;
double MAXGUESSES = 0.0;
double REMAINING = 0.0;
double TRIES = 0.0;
double SECRET = 0.0;

// String variables
char A_str[256] = "";

// Built-in helper functions

int _gosub_stack[100];
int _gosub_sp = 0;

int _rnd_initialized = 0;
double RB_RND() {
    if (!_rnd_initialized) {
        srand((unsigned)time(NULL));
        _rnd_initialized = 1;
    }
    // Use 32767.0 (common RAND_MAX value) to ensure proper division
    return (double)rand() / 32767.0;
}

void RB_LEFT(char* dest, const char* src, int len) {
    if (len <= 0) { dest[0] = '\0'; return; }
    int src_len = strlen(src);
    int copy_len = (len < src_len) ? len : src_len;
    strncpy(dest, src, copy_len);
    dest[copy_len] = '\0';
}

void RB_RIGHT(char* dest, const char* src, int len) {
    int src_len = strlen(src);
    if (len <= 0) { dest[0] = '\0'; return; }
    if (len >= src_len) { strcpy(dest, src); return; }
    strcpy(dest, src + (src_len - len));
}

void RB_MID(char* dest, const char* src, int start, int len) {
    int src_len = strlen(src);
    start = start - 1;  // BASIC uses 1-based indexing
    if (start < 0) start = 0;
    if (start >= src_len) {
        dest[0] = '\0';
        return;
    }
    int copy_len = (len < src_len - start) ? len : (src_len - start);
    strncpy(dest, src + start, copy_len);
    dest[copy_len] = '\0';
}

char* RB_CONCAT(char* dest, const char* s1, const char* s2) {
    strcpy(dest, s1);
    strcat(dest, s2);
    return dest;
}

int RB_INSTR(const char* haystack, const char* needle) {
    const char* pos = strstr(haystack, needle);
    return pos ? (int)(pos - haystack) + 1 : 0;  // 1-based
}

void RB_CHR(char* dest, int code) {
    dest[0] = (char)code;
    dest[1] = '\0';
}

void RB_STR(char* dest, double num) {
    sprintf(dest, "%g", num);
}

void RB_UCASE(char* dest, const char* src) {
    strcpy(dest, src);
    for (int i = 0; dest[i]; i++) {
        if (dest[i] >= 'a' && dest[i] <= 'z') {
            dest[i] = dest[i] - 32;
        }
    }
}

void RB_LCASE(char* dest, const char* src) {
    strcpy(dest, src);
    for (int i = 0; dest[i]; i++) {
        if (dest[i] >= 'A' && dest[i] <= 'Z') {
            dest[i] = dest[i] + 32;
        }
    }
}

FILE* _file_handles[256] = {NULL};

// System time functions
void RB_GET_DATE(char* dest) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    sprintf(dest, "%02d-%02d-%04d", t->tm_mon + 1, t->tm_mday, t->tm_year + 1900);
}

void RB_GET_TIME(char* dest) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    sprintf(dest, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
}

double RB_TIMER() {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    return (double)(t->tm_hour * 3600 + t->tm_min * 60 + t->tm_sec);
}

// Keyboard input
#ifdef _WIN32
#include <conio.h>
void RB_INKEY(char* dest) {
    if (_kbhit()) {
        int ch = _getch();
        dest[0] = (char)ch;
        dest[1] = '\0';
    } else {
        dest[0] = '\0';
    }
}

void RB_INPUT_STR(char* dest, int count) {
    int i = 0;
    while (i < count && i < 255) {
        int ch = _getch();
        if (ch == '\r' || ch == '\n') break;
        dest[i++] = (char)ch;
        putchar(ch);  // Echo character
    }
    dest[i] = '\0';
    fflush(stdout);
}
#else
void RB_INKEY(char* dest) { dest[0] = '\0'; }  // Stub for non-Windows

void RB_INPUT_STR(char* dest, int count) {
    int i = 0;
    while (i < count && i < 255 && (dest[i] = getchar()) != '\n') {
        i++;
    }
    dest[i] = '\0';
}
#endif

// Number formatting
void RB_HEX(char* dest, int num) {
    sprintf(dest, "%X", num);
}

void RB_OCT(char* dest, int num) {
    sprintf(dest, "%o", num);
}

void RB_TAB(char* dest, int column) {
    // TAB(n) returns string of spaces to reach column n
    // Note: In classic BASIC, column position is tracked by cursor
    // Here we just return n spaces for simplicity
    if (column < 1) column = 1;
    if (column > 255) column = 255;
    for (int i = 0; i < column && i < 255; i++) {
        dest[i] = ' ';
    }
    dest[column] = '\0';
}

// Screen control
#ifdef _WIN32
#include <windows.h>
void RB_CLS() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = {0, 0};
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, ' ', dwConSize, coordScreen, &cCharsWritten);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
    SetConsoleCursorPosition(hConsole, coordScreen);
}

void RB_LOCATE(int row, int col) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {(SHORT)(col - 1), (SHORT)(row - 1)};
    SetConsoleCursorPosition(hConsole, pos);
    fflush(stdout);  // Ensure cursor position is applied
}

void RB_COLOR(int fg, int bg) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (WORD)((bg << 4) | fg));
    fflush(stdout);  // Ensure color is applied
}
#else
void RB_CLS() { printf("\033[2J\033[H"); }
void RB_LOCATE(int row, int col) { printf("\033[%d;%dH", row, col); }
void RB_COLOR(int fg, int bg) { printf("\033[%d;%dm", 30 + fg, 40 + bg); }
#endif

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    char _temp_str[256];
    char _temp_str2[256];
    char _temp_str3[256];
    
L0:;
    // 
L1:;
    // 
L2:;
    RB_CLS();
L3:;
    srand((unsigned)time(NULL));
    _rnd_initialized = 1;
L4:;
    SECRET = (floor((RB_RND() * 100.000000)) + 1.000000);
L5:;
    TRIES = 0.000000;
L6:;
    MAXGUESSES = 7.000000;
L7:;
    printf("%s", "================================");
    printf("\n");
    fflush(stdout);  // Ensure output appears immediately
L8:;
    printf("%s", "   NUMBER GUESSING GAME");
    printf("\n");
    fflush(stdout);  // Ensure output appears immediately
L9:;
    printf("%s", "================================");
    printf("\n");
    fflush(stdout);  // Ensure output appears immediately
L10:;
    printf("\n");
    fflush(stdout);
L11:;
    printf("%s", "I'm thinking of a number");
    printf("\n");
    fflush(stdout);  // Ensure output appears immediately
L12:;
    printf("%s", "between 1 and 100.");
    printf("\n");
    fflush(stdout);  // Ensure output appears immediately
L13:;
    printf("\n");
    fflush(stdout);
L14:;
    printf("%s", "You have");
    printf(" ");
    printf("%g", MAXGUESSES);
    printf("%s", "guesses.");
    fflush(stdout);  // Ensure output appears immediately
L15:;
    printf("\n");
    fflush(stdout);
L16:;
    // 
L17:;
    while ((TRIES < MAXGUESSES)) {
    L18:;
        TRIES = (TRIES + 1.000000);
    L19:;
        printf("%s", "Guess #");
        printf(" ");
        printf("%g", TRIES);
        printf("%s", ": ");
        fflush(stdout);  // Ensure output appears immediately
    L20:;
        scanf("%lf", &GUESS);
        while (getchar() != '\n');  // Clear input buffer
    L21:;
        if (((GUESS < 1.000000) || (GUESS > 100.000000))) {
        L22:;
            printf("%s", "Please guess between 1 and 100!");
            printf("\n");
            fflush(stdout);  // Ensure output appears immediately
        L23:;
            TRIES = (TRIES - 1.000000);
        L24:;
            goto L17;
        }
    L25:;
        if ((GUESS == SECRET)) {
            goto L34;
        }
    L26:;
        if ((GUESS < SECRET)) {
        L27:;
            printf("%s", "Too low!");
            printf("\n");
            fflush(stdout);  // Ensure output appears immediately
        } else {
        L28:;
            printf("%s", "Too high!");
            printf("\n");
            fflush(stdout);  // Ensure output appears immediately
        }
    L29:;
        REMAINING = (MAXGUESSES - TRIES);
    L30:;
        if ((REMAINING > 0.000000)) {
        L31:;
            printf("%s", "You have");
            printf(" ");
            printf("%g", REMAINING);
            printf("%s", "guesses left.");
            fflush(stdout);  // Ensure output appears immediately
        }
    L32:;
        printf("\n");
        fflush(stdout);
    }
L33:;
    goto L44;
L34:;
    // 
L35:;
    printf("\n");
    fflush(stdout);
L36:;
    printf("%s", "*** CORRECT! ***");
    printf("\n");
    fflush(stdout);  // Ensure output appears immediately
L37:;
    printf("\n");
    fflush(stdout);
L38:;
    printf("%s", "You guessed the number in");
    printf(" ");
    printf("%g", TRIES);
    printf("%s", "tries!");
    fflush(stdout);  // Ensure output appears immediately
L39:;
    if ((TRIES == 1.000000)) {
        printf("%s", "Amazing! First try!");
        printf("\n");
        fflush(stdout);  // Ensure output appears immediately
    }
L40:;
    if ((TRIES <= 3.000000)) {
        printf("%s", "Excellent guessing!");
        printf("\n");
        fflush(stdout);  // Ensure output appears immediately
    }
L41:;
    if ((TRIES <= 5.000000)) {
        printf("%s", "Good job!");
        printf("\n");
        fflush(stdout);  // Ensure output appears immediately
    }
L42:;
    goto L49;
L43:;
    // 
L44:;
    printf("\n");
    fflush(stdout);
L45:;
    printf("%s", "Sorry, you're out of guesses!");
    printf("\n");
    fflush(stdout);  // Ensure output appears immediately
L46:;
    printf("%s", "The number was:");
    printf(" ");
    printf("%g", SECRET);
    fflush(stdout);  // Ensure output appears immediately
L47:;
    printf("\n");
    fflush(stdout);
L48:;
    // 
L49:;
    printf("\n");
    fflush(stdout);
L50:;
    printf("%s", "Play again? (Y/N) ");
    fflush(stdout);  // Ensure output appears immediately
L51:;
    strcpy(A_str, (RB_INPUT_STR(_temp_str, 1.000000), _temp_str));
L52:;
    printf("\n");
    fflush(stdout);
L53:;
    if ((strcmp((RB_UCASE(_temp_str, A_str), _temp_str), "Y") == 0)) {
        goto L2;
    }
L54:;
    printf("%s", "Thanks for playing!");
    printf("\n");
    fflush(stdout);  // Ensure output appears immediately
L55:;
    exit(0);
    
    return 0;
}
