// RB_COMPILE_MODE: graphics
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#ifdef _WIN32
#include <windows.h>
#endif

#define RB_STR_SIZE 4096

// Numeric variables
double UY = 0.0;
double UI = 0.0;
double OP_IMG = 0.0;
double PI = 0.0;
double UP_OFFSET = 0.0;
double NI = 0.0;
double CR_START = 0.0;
double CA_COLOR = 0.0;
double CA_I = 0.0;
double PGR_RANDIDX = 0.0;
double PGR_RANGE = 0.0;
double PGR_W = 0.0;
double DIDX = 0.0;
double PGR_KFOUND = 0.0;
double UP_TOTAL = 0.0;
double PLA_KC = 0.0;
double PLA_TMP = 0.0;
double PLA_READINGR = 0.0;
double PLA_ENDR = 0.0;
double PLA_STARTR = 0.0;
double PRI = 0.0;
double PA_SND = 0.0;
double MAX_CHAT = 0.0;
double PIDX = 0.0;
double IP = 0.0;
double CR_ROW = 0.0;
double GL_COL = 0.0;
double MAX_REPLIES = 0.0;
double GI_CURSOR_ON = 0.0;
double PID = 0.0;
double DATES_MONTHS = 0.0;
double MEETINGCOUNTER = 0.0;
double GI_CLEARW = 0.0;
double GC_CLR = 0.0;
double GFX_COLS = 0.0;
double GI_ANY = 0.0;
double NEWSBLA_COUNT = 0.0;
double PLA_RC = 0.0;
double TIMEPASS = 0.0;
double COUNTER = 0.0;
double CHAT_COUNT = 0.0;
double CP_ROW = 0.0;
double PGR_L = 0.0;
double GFX_ROW = 0.0;
double COLORS_COUNT = 0.0;
double METAPHORE_COUNT = 0.0;
double MAX_WORDS = 0.0;
double BAD_COUNT = 0.0;
double PGR_KFLAG = 0.0;
double GFX_CHARH = 0.0;
double GL_ROW = 0.0;
double GFX_COL = 0.0;
double GI_CURSOR_TIMER = 0.0;
double SONGCOUNTER = 0.0;
double FINISHED = 0.0;
double GC_ROW = 0.0;
double PGR_KIDX = 0.0;
double SOUND_WAIT = 0.0;
double GFX_COLOR = 0.0;
double NIDX = 0.0;
double CURRENT_SOUND = 0.0;
double GI_SHIFTED = 0.0;
double GFX_BGCOLOR = 0.0;
double GI_K = 0.0;
double DYR = 0.0;
double JPLACE = 0.0;
double GFX_CHARW = 0.0;
double PLACES_COUNT = 0.0;
double MAX_LINES = 0.0;
double GC_COL = 0.0;
double PLA_WC = 0.0;
double OBJECTS_COUNT = 0.0;
double GOOD_COUNT = 0.0;
double CR_I = 0.0;
double CURRENT_PID = 0.0;
double NEWS_LOADED = 0.0;
double GS_I = 0.0;
double UPPER_LINES_COUNT = 0.0;
double GW_DEBOUNCE = 0.0;
double GI_COOLDOWN = 0.0;
double PS_SND = 0.0;
double GI_STARTROW = 0.0;
double CR_MAXLINES = 0.0;
double GFX_ROWS = 0.0;
double GI_WAIT = 0.0;
double MAX_KEYWORDS = 0.0;
double REPLY_SWITCH = 0.0;
double GI_WK = 0.0;
double GI_CX = 0.0;
double PGR_K = 0.0;
double DM = 0.0;
double GI_STARTCOL = 0.0;
double JP = 0.0;

// String variables
char GK_str[RB_STR_SIZE] = "";
char UFLINE_str[RB_STR_SIZE] = "";
char UF_str[RB_STR_SIZE] = "";
char LP_VAL_str[RB_STR_SIZE] = "";
char LPFLINE_str[RB_STR_SIZE] = "";
char LPF_str[RB_STR_SIZE] = "";
char LOADPOEM_FILE_str[RB_STR_SIZE] = "";
char EK_str[RB_STR_SIZE] = "";
char PA_PATH_str[RB_STR_SIZE] = "";
char PS_PATH_str[RB_STR_SIZE] = "";
char SOUND_PATH_str[RB_STR_SIZE] = "";
char IMG_PATH_str[RB_STR_SIZE] = "";
char JPC_str[RB_STR_SIZE] = "";
char JOIN_IN_str[RB_STR_SIZE] = "";
char JB_str[RB_STR_SIZE] = "";
char IC_str[RB_STR_SIZE] = "";
char GI_PROMPT_str[RB_STR_SIZE] = "";
char UP_PAD_str[RB_STR_SIZE] = "";
char ISOLATE_IN_str[RB_STR_SIZE] = "";
char DATES_RESULT_str[RB_STR_SIZE] = "";
char TBUF_str[RB_STR_SIZE] = "";
char PGR_BASE_str[RB_STR_SIZE] = "";
char TXTFILE_NAME_str[RB_STR_SIZE] = "";
char SOUND_FILE_str[RB_STR_SIZE] = "";
char GP_TEXT_str[RB_STR_SIZE] = "";
char PUNCTUATION_str[RB_STR_SIZE] = "";
char SND_PATH_str[RB_STR_SIZE] = "";
char PGR_TRIMTAIL_str[RB_STR_SIZE] = "";
char GI_RESULT_str[RB_STR_SIZE] = "";
char REPLY_INPUT_str[RB_STR_SIZE] = "";
char PGR_BEFORE_str[RB_STR_SIZE] = "";
char WK_str[RB_STR_SIZE] = "";
char CR_INTEXT_str[RB_STR_SIZE] = "";
char TF_str[RB_STR_SIZE] = "";
char SLOW_TEXT_str[RB_STR_SIZE] = "";
char GC_TEXT_str[RB_STR_SIZE] = "";
char LP_PREFIX_str[RB_STR_SIZE] = "";
char ISOLATE_OUT_str[RB_STR_SIZE] = "";
char GS_CH_str[RB_STR_SIZE] = "";
char JFIND_str[RB_STR_SIZE] = "";
char PLAF_str[RB_STR_SIZE] = "";
char LOADSCRIPT_FILE_str[RB_STR_SIZE] = "";
char PLA_PREFIX_str[RB_STR_SIZE] = "";
char PLA_KWORD_str[RB_STR_SIZE] = "";
char PGR_PROMPT_str[RB_STR_SIZE] = "";
char PGR_INPT_str[RB_STR_SIZE] = "";
char PGR_CHK_str[RB_STR_SIZE] = "";
char DUMMY_str[RB_STR_SIZE] = "";
char REPLY_RESULT_str[RB_STR_SIZE] = "";
char PGR_TAIL_str[RB_STR_SIZE] = "";
char DATA_PATH_str[RB_STR_SIZE] = "";
char PLAFLINE_str[RB_STR_SIZE] = "";
char PGR_LCKEY_str[RB_STR_SIZE] = "";
char OK_str[RB_STR_SIZE] = "";
char UPPER_FILE_str[RB_STR_SIZE] = "";
char PGR_LCPART_str[RB_STR_SIZE] = "";
char PGR_LCWORD_str[RB_STR_SIZE] = "";
char CONV_RPLY_str[RB_STR_SIZE] = "";
char IB_str[RB_STR_SIZE] = "";
char PGR_AFTER_str[RB_STR_SIZE] = "";
char PGR_ANSW_str[RB_STR_SIZE] = "";
char PGR_RTRIM_str[RB_STR_SIZE] = "";
char CONV_FILE_str[RB_STR_SIZE] = "";
char PST_MSG_str[RB_STR_SIZE] = "";
char JOIN_OUT_str[RB_STR_SIZE] = "";
char SPEAK_TEXT_str[RB_STR_SIZE] = "";
char CA_TEXT_str[RB_STR_SIZE] = "";
char NF_str[RB_STR_SIZE] = "";
char CR_LC_str[RB_STR_SIZE] = "";
char PGR_LCINPT_str[RB_STR_SIZE] = "";
char NL_str[RB_STR_SIZE] = "";

// SDL2 Graphics
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int screen_width = 640;
int screen_height = 480;
int screen_mode = 0;
int current_color = 15;  // Default white
int* pixel_buffer = NULL;  // Software pixel buffer for POINT/PAINT
int _auto_display = 0;  // Manual display mode (_DISPLAY used)

// QB64 image management
#define MAX_IMAGES 256
SDL_Texture* _images[MAX_IMAGES] = {0};
int _image_w[MAX_IMAGES] = {0};
int _image_h[MAX_IMAGES] = {0};
int _next_image = 1;

// QB64 mouse/keyboard state
int _mouse_x = 0, _mouse_y = 0;
int _mouse_buttons = 0;
int _mouse_wheel = 0;
const Uint8* _keyboard_state = NULL;

// EGA 16-color palette
SDL_Color palette[16] = {
    {0, 0, 0, 255},       // 0: Black
    {0, 0, 170, 255},     // 1: Blue
    {0, 170, 0, 255},     // 2: Green
    {0, 170, 170, 255},   // 3: Cyan
    {170, 0, 0, 255},     // 4: Red
    {170, 0, 170, 255},   // 5: Magenta
    {170, 85, 0, 255},    // 6: Brown
    {170, 170, 170, 255}, // 7: Light Gray
    {85, 85, 85, 255},    // 8: Dark Gray
    {85, 85, 255, 255},   // 9: Light Blue
    {85, 255, 85, 255},   // 10: Light Green
    {85, 255, 255, 255},  // 11: Light Cyan
    {255, 85, 85, 255},   // 12: Light Red
    {255, 85, 255, 255},  // 13: Light Magenta
    {255, 255, 85, 255},  // 14: Yellow
    {255, 255, 255, 255}  // 15: White
};

// SDL_mixer sound management
#define MAX_SOUNDS 64
Mix_Chunk* _sounds[MAX_SOUNDS] = {0};
int _sound_channels[MAX_SOUNDS] = {0};  // Channel assigned to each sound
int _next_sound = 1;
int _mixer_initialized = 0;

// Numeric arrays
double* CHAT_COLORS = NULL;
double PERSONRINDEX[4][101] = {0};
double PERSONRENDS[4][101] = {0};
double PERSONRSTARTS[4][101] = {0};
double PERSON_NOKEYINDEX[4] = {0};
double PERSON_WCNT[4] = {0};
double PERSON_RCNT[4] = {0};
double PERSON_KCNT[4] = {0};

// String arrays
char (*CHAT_LINES_str)[RB_STR_SIZE] = NULL;
char DREAM_MUSIC_str[6][RB_STR_SIZE] = {0};
char DREAM_FILES_str[8][RB_STR_SIZE] = {0};
char MEETING_FILES_str[5][RB_STR_SIZE] = {0};
char MONTH_NAMES_str[12][RB_STR_SIZE] = {0};
char (*UPPER_LINES_str)[RB_STR_SIZE] = NULL;
char (*BAD_ARR_str)[RB_STR_SIZE] = NULL;
char PERSONKEYWORDS_str[4][101][RB_STR_SIZE] = {0};
char PERSONREPLIES_str[4][201][RB_STR_SIZE] = {0};
char (*PLACES_str)[RB_STR_SIZE] = NULL;
char PERSONWORDOUT_str[4][51][RB_STR_SIZE] = {0};
char (*METAPHORE_str)[RB_STR_SIZE] = NULL;
char PERSONWORDIN_str[4][51][RB_STR_SIZE] = {0};
char (*OBJECTS_str)[RB_STR_SIZE] = NULL;
char PERSON_SCRIPT_str[4][RB_STR_SIZE] = {0};
char (*NEWSBLA_str)[RB_STR_SIZE] = NULL;
char PERSON_GREETING_str[4][RB_STR_SIZE] = {0};
char PERSON_YOU_str[4][RB_STR_SIZE] = {0};
char (*COLORS_ARR_str)[RB_STR_SIZE] = NULL;
char (*GOOD_ARR_str)[RB_STR_SIZE] = NULL;

// Built-in helper functions

int _gosub_stack[1000];
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

void RB_LTRIM(char* dest, const char* src) {
    while (*src == ' ') src++;
    strncpy(dest, src, 255); dest[255] = '\0';
}

void RB_RTRIM(char* dest, const char* src) {
    strncpy(dest, src, 255); dest[255] = '\0';
    int len = strlen(dest);
    while (len > 0 && dest[len-1] == ' ') dest[--len] = '\0';
}

void RB_TRIM(char* dest, const char* src) {
    while (*src == ' ') src++;
    strncpy(dest, src, 255); dest[255] = '\0';
    int len = strlen(dest);
    while (len > 0 && dest[len-1] == ' ') dest[--len] = '\0';
}

void RB_SPACE(char* dest, int count) {
    if (count < 0) count = 0;
    if (count > 255) count = 255;
    memset(dest, ' ', count);
    dest[count] = '\0';
}

void RB_STRING_FUNC(char* dest, int count, int ch) {
    if (count < 0) count = 0;
    if (count > 255) count = 255;
    memset(dest, ch, count);
    dest[count] = '\0';
}

void RB_ENVIRON(char* dest, const char* name) {
    char* val = getenv(name);
    if (val) { strncpy(dest, val, 255); dest[255] = '\0'; }
    else dest[0] = '\0';
}

char _command_str[RB_STR_SIZE] = "";

unsigned char _mem_segment[65536] = {0};
int _def_seg = 0;

FILE* _file_handles[256] = {NULL};
int _file_reclen[256] = {0};  // Record lengths for RANDOM mode

int _err = 0;
int _erl = 0;

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
    current_color = fg;  // Set graphics color for _PRINTSTRING
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (WORD)((bg << 4) | fg));
    fflush(stdout);  // Ensure color is applied
}
#else
void RB_CLS() { printf("\033[2J\033[H"); }
void RB_LOCATE(int row, int col) { printf("\033[%d;%dH", row, col); }
void RB_COLOR(int fg, int bg) { current_color = fg; printf("\033[%d;%dm", 30 + fg, 40 + bg); }
#endif

// PRINT USING - format a number according to format string
void RB_PRINT_USING_NUM(FILE* out, const char* fmt, double val) {
    int i = 0;
    int digits_before = 0, digits_after = 0;
    int has_dot = 0, has_comma = 0, has_plus = 0, has_minus = 0;
    int has_dollar = 0;
    // Count format characters
    for (i = 0; fmt[i]; i++) {
        if (fmt[i] == '#') { if (has_dot) digits_after++; else digits_before++; }
        else if (fmt[i] == '.') has_dot = 1;
        else if (fmt[i] == ',') has_comma = 1;
        else if (fmt[i] == '+') has_plus = 1;
        else if (fmt[i] == '-') has_minus = 1;
        else if (fmt[i] == '$' && fmt[i+1] == '$') { has_dollar = 1; i++; }
    }
    if (digits_before == 0) digits_before = 1;
    char fmtbuf[64];
    int w = digits_before + (has_dot ? 1 + digits_after : 0);
    if (has_dot)
        sprintf(fmtbuf, "%*.*f", w, digits_after, val);
    else
        sprintf(fmtbuf, "%*.0f", w, val);
    if (has_dollar) fprintf(out, "$");
    if (has_plus && val >= 0) fprintf(out, "+");
    fprintf(out, "%s", fmtbuf);
}

void RB_PRINT_USING_STR(FILE* out, const char* fmt, const char* val) {
    if (fmt[0] == '!' && fmt[1] == '\0') {
        fprintf(out, "%c", val[0]);  // First character only
    } else if (fmt[0] == '&') {
        fprintf(out, "%s", val);  // Entire string
    } else if (fmt[0] == '\\') {
        // Fixed width: count chars between backslashes
        int width = 2;
        for (int i = 1; fmt[i] && fmt[i] != '\\'; i++) width++;
        fprintf(out, "%-*.*s", width, width, val);
    } else {
        fprintf(out, "%s", val);
    }
}

#ifdef _WIN32
void RB_PLAY(const char* mml) {
    int octave = 4, tempo = 120, default_len = 4;
    int i = 0;
    double base_freq[12] = {16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.50, 25.96, 27.50, 29.14, 30.87};
    while (mml[i]) {
        while (mml[i] == ' ') i++;
        if (!mml[i]) break;
        char c = mml[i]; i++;
        if (c == 'O' || c == 'o') { if (mml[i] >= '0' && mml[i] <= '8') { octave = mml[i] - '0'; i++; } continue; }
        if (c == 'L' || c == 'l') { default_len = 0; while (mml[i] >= '0' && mml[i] <= '9') { default_len = default_len * 10 + (mml[i] - '0'); i++; } if (default_len == 0) default_len = 4; continue; }
        if (c == 'T' || c == 't') { tempo = 0; while (mml[i] >= '0' && mml[i] <= '9') { tempo = tempo * 10 + (mml[i] - '0'); i++; } if (tempo == 0) tempo = 120; continue; }
        if (c == '<') { if (octave > 0) octave--; continue; }
        if (c == '>') { if (octave < 8) octave++; continue; }
        if (c == 'P' || c == 'p' || c == 'R' || c == 'r') {
            int len = default_len;
            if (mml[i] >= '0' && mml[i] <= '9') { len = 0; while (mml[i] >= '0' && mml[i] <= '9') { len = len * 10 + (mml[i] - '0'); i++; } }
            int ms = (60000 / tempo) * 4 / (len > 0 ? len : 4);
            Sleep(ms);
            continue;
        }
        int note = -1;
        switch (c) {
            case 'C': case 'c': note = 0; break;
            case 'D': case 'd': note = 2; break;
            case 'E': case 'e': note = 4; break;
            case 'F': case 'f': note = 5; break;
            case 'G': case 'g': note = 7; break;
            case 'A': case 'a': note = 9; break;
            case 'B': case 'b': note = 11; break;
        }
        if (note < 0) continue;
        if (mml[i] == '#' || mml[i] == '+') { note++; i++; }
        else if (mml[i] == '-') { note--; i++; }
        int len = default_len;
        if (mml[i] >= '0' && mml[i] <= '9') { len = 0; while (mml[i] >= '0' && mml[i] <= '9') { len = len * 10 + (mml[i] - '0'); i++; } }
        int dot = 0;
        if (mml[i] == '.') { dot = 1; i++; }
        if (note < 0) note = 0;
        if (note > 11) note = 11;
        int freq = (int)(base_freq[note % 12] * (1 << octave));
        int ms = (60000 / tempo) * 4 / (len > 0 ? len : 4);
        if (dot) ms = ms * 3 / 2;
        if (freq > 0) Beep(freq, ms);
    }
}
#else
void RB_PLAY(const char* mml) { (void)mml; }  // Stub for non-Windows
#endif

// SDL2 Graphics Functions

void init_graphics(int mode) {
    screen_mode = mode;
    
    // Set dimensions based on mode
    switch(mode) {
        case 1:  screen_width = 320; screen_height = 200; break;  // CGA
        case 2:  screen_width = 640; screen_height = 200; break;  // CGA Hi-res
        case 7:  screen_width = 320; screen_height = 200; break;  // EGA
        case 9:  screen_width = 640; screen_height = 350; break;  // EGA Hi-res
        case 12: screen_width = 640; screen_height = 480; break;  // VGA
        case 13: screen_width = 320; screen_height = 200; break;  // VGA Mode X
        case 14: screen_width = 800; screen_height = 600; break;  // SVGA
        default: screen_width = 640; screen_height = 480; break;
    }
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL init failed: %s\n", SDL_GetError());
        exit(1);
    }
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    
    window = SDL_CreateWindow("RB BASIC Graphics",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              screen_width, screen_height,
                              SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }
    
    // Clear screen to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    
    // Allocate pixel buffer for POINT/PAINT
    pixel_buffer = (int*)calloc(screen_width * screen_height, sizeof(int));
}

void cleanup_graphics() {
    // Free image handles
    for (int i = 1; i < MAX_IMAGES; i++) {
        if (_images[i]) { SDL_DestroyTexture(_images[i]); _images[i] = NULL; }
    }
    // Free sound handles
    for (int i = 1; i < MAX_SOUNDS; i++) {
        if (_sounds[i]) { Mix_FreeChunk(_sounds[i]); _sounds[i] = NULL; }
    }
    if (_mixer_initialized) Mix_CloseAudio();
    if (pixel_buffer) { free(pixel_buffer); pixel_buffer = NULL; }
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

void wait_for_close() {
    if (!window) return;
    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = 0;
            }
        }
        SDL_Delay(10);  // Small delay to prevent high CPU usage
    }
}

SDL_Color get_palette_color(int color_index) {
    if (color_index < 0) color_index = 0;
    if (color_index > 15) color_index = 15;
    return palette[color_index];
}

void set_pixel(int x, int y, int color_index);
int get_pixel_color(int x, int y);
void flood_fill(int x, int y, int fill_color, int border_color);

void draw_line_buffered(int x0, int y0, int x1, int y1, int color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    for (;;) {
        set_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void fill_rect_buffered(int x1, int y1, int w, int h, int color) {
    for (int fy = y1; fy < y1 + h; fy++)
        for (int fx = x1; fx < x1 + w; fx++)
            set_pixel(fx, fy, color);
}

void draw_circle(int cx, int cy, int radius, int color) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    
    while (x <= y) {
        // Draw 8 octants
        set_pixel(cx + x, cy + y, color);
        set_pixel(cx - x, cy + y, color);
        set_pixel(cx + x, cy - y, color);
        set_pixel(cx - x, cy - y, color);
        set_pixel(cx + y, cy + x, color);
        set_pixel(cx - y, cy + x, color);
        set_pixel(cx + y, cy - x, color);
        set_pixel(cx - y, cy - x, color);
        
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
    if (_auto_display) SDL_RenderPresent(renderer);
}

void set_pixel(int x, int y, int color_index) {
    if (x < 0 || x >= screen_width || y < 0 || y >= screen_height) return;
    if (pixel_buffer) pixel_buffer[y * screen_width + x] = color_index;
    SDL_Color c = get_palette_color(color_index);
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawPoint(renderer, x, y);
}

int get_pixel_color(int x, int y) {
    if (x < 0 || x >= screen_width || y < 0 || y >= screen_height) return 0;
    if (pixel_buffer) return pixel_buffer[y * screen_width + x];
    return 0;
}

void flood_fill(int x, int y, int fill_color, int border_color) {
    if (!pixel_buffer) return;
    if (x < 0 || x >= screen_width || y < 0 || y >= screen_height) return;
    int target_color = get_pixel_color(x, y);
    if (target_color == fill_color) return;
    if (target_color == border_color) return;
    
    // Stack-based flood fill
    int stack_size = screen_width * screen_height;
    int* stack_x = (int*)malloc(stack_size * sizeof(int));
    int* stack_y = (int*)malloc(stack_size * sizeof(int));
    if (!stack_x || !stack_y) { free(stack_x); free(stack_y); return; }
    int sp = 0;
    stack_x[sp] = x; stack_y[sp] = y; sp++;
    
    SDL_Color c = get_palette_color(fill_color);
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    
    while (sp > 0) {
        sp--;
        int cx = stack_x[sp], cy = stack_y[sp];
        if (cx < 0 || cx >= screen_width || cy < 0 || cy >= screen_height) continue;
        int pc = pixel_buffer[cy * screen_width + cx];
        if (pc == fill_color || pc == border_color) continue;
        
        // Scan left
        int lx = cx;
        while (lx > 0) {
            int plc = pixel_buffer[cy * screen_width + (lx - 1)];
            if (plc == fill_color || plc == border_color) break;
            lx--;
        }
        // Scan right
        int rx = cx;
        while (rx < screen_width - 1) {
            int prc = pixel_buffer[cy * screen_width + (rx + 1)];
            if (prc == fill_color || prc == border_color) break;
            rx++;
        }
        
        // Fill the scanline
        for (int i = lx; i <= rx; i++) {
            pixel_buffer[cy * screen_width + i] = fill_color;
            SDL_RenderDrawPoint(renderer, i, cy);
        }
        
        // Push adjacent rows
        for (int i = lx; i <= rx; i++) {
            if (cy > 0 && sp < stack_size) {
                int pa = pixel_buffer[(cy-1) * screen_width + i];
                if (pa != fill_color && pa != border_color) {
                    stack_x[sp] = i; stack_y[sp] = cy - 1; sp++;
                }
            }
            if (cy < screen_height - 1 && sp < stack_size) {
                int pb = pixel_buffer[(cy+1) * screen_width + i];
                if (pb != fill_color && pb != border_color) {
                    stack_x[sp] = i; stack_y[sp] = cy + 1; sp++;
                }
            }
        }
    }
    free(stack_x); free(stack_y);
    if (_auto_display) SDL_RenderPresent(renderer);
}

void RB_DRAW(const char* cmd) {
    static int draw_x = -1, draw_y = -1;
    if (draw_x < 0) { draw_x = screen_width / 2; draw_y = screen_height / 2; }
    int i = 0;
    int draw_color = current_color;
    while (cmd[i]) {
        while (cmd[i] == ' ') i++;
        if (!cmd[i]) break;
        int blind = 0, ret = 0;
        // Check prefixes B and N
        while (cmd[i] == 'B' || cmd[i] == 'b' || cmd[i] == 'N' || cmd[i] == 'n') {
            if (cmd[i] == 'B' || cmd[i] == 'b') blind = 1;
            if (cmd[i] == 'N' || cmd[i] == 'n') ret = 1;
            i++;
        }
        char c = cmd[i]; i++;
        if (!c) break;
        // Parse number
        int neg = 0;
        if (cmd[i] == '-') { neg = 1; i++; }
        else if (cmd[i] == '+') { i++; }
        int n = 0;
        while (cmd[i] >= '0' && cmd[i] <= '9') { n = n * 10 + (cmd[i] - '0'); i++; }
        if (neg) n = -n;
        int sx = draw_x, sy = draw_y;
        int dx = 0, dy = 0;
        switch (c) {
            case 'U': case 'u': dy = -n; break;
            case 'D': case 'd': dy = n; break;
            case 'L': case 'l': dx = -n; break;
            case 'R': case 'r': dx = n; break;
            case 'E': case 'e': dx = n; dy = -n; break;
            case 'F': case 'f': dx = n; dy = n; break;
            case 'G': case 'g': dx = -n; dy = n; break;
            case 'H': case 'h': dx = -n; dy = -n; break;
            case 'M': case 'm': {
                // M x,y - move to position (absolute or relative)
                int rel = 0;
                while (cmd[i] == ' ') i++;
                if (cmd[i] == '+' || cmd[i] == '-') rel = 1;
                int mneg = 0;
                if (cmd[i] == '-') { mneg = 1; i++; }
                else if (cmd[i] == '+') { i++; }
                int mx = 0;
                while (cmd[i] >= '0' && cmd[i] <= '9') { mx = mx * 10 + (cmd[i] - '0'); i++; }
                if (mneg) mx = -mx;
                while (cmd[i] == ' ' || cmd[i] == ',') i++;
                int myneg = 0;
                if (cmd[i] == '-') { myneg = 1; i++; }
                else if (cmd[i] == '+') { i++; }
                int my = 0;
                while (cmd[i] >= '0' && cmd[i] <= '9') { my = my * 10 + (cmd[i] - '0'); i++; }
                if (myneg) my = -my;
                if (rel) { dx = mx; dy = my; }
                else {
                    if (!blind) draw_line_buffered(draw_x, draw_y, mx, my, draw_color);
                    draw_x = mx; draw_y = my;
                    if (_auto_display) SDL_RenderPresent(renderer);
                    continue;
                }
                break;
            }
            case 'C': case 'c': {
                // C n - change color
                draw_color = n;
                current_color = n;
                continue;
            }
            default: continue;
        }
        int nx = draw_x + dx, ny = draw_y + dy;
        if (!blind) draw_line_buffered(draw_x, draw_y, nx, ny, draw_color);
        if (ret) { /* don't move */ } else { draw_x = nx; draw_y = ny; }
    }
    if (_auto_display) SDL_RenderPresent(renderer);
}

void rb_process_events() {
    SDL_Event _evt;
    _mouse_wheel = 0;
    while (SDL_PollEvent(&_evt)) {
        if (_evt.type == SDL_QUIT) { cleanup_graphics(); exit(0); }
        if (_evt.type == SDL_KEYDOWN && _evt.key.keysym.sym == SDLK_ESCAPE) { cleanup_graphics(); exit(0); }
        if (_evt.type == SDL_MOUSEWHEEL) { _mouse_wheel = _evt.wheel.y; }
    }
    _mouse_buttons = SDL_GetMouseState(&_mouse_x, &_mouse_y);
    _keyboard_state = SDL_GetKeyboardState(NULL);
}

int rb_newimage(int w, int h) {
    if (_next_image >= MAX_IMAGES) return -1;
    int handle = _next_image++;
    _images[handle] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    if (!_images[handle]) return -1;
    _image_w[handle] = w;
    _image_h[handle] = h;
    SDL_SetTextureBlendMode(_images[handle], SDL_BLENDMODE_BLEND);
    return handle;
}

int rb_loadimage(const char* filename) {
    if (_next_image >= MAX_IMAGES) return -1;
    SDL_Surface* surf = IMG_Load(filename);
    if (!surf) return -1;
    int handle = _next_image++;
    _images[handle] = SDL_CreateTextureFromSurface(renderer, surf);
    _image_w[handle] = surf->w;
    _image_h[handle] = surf->h;
    SDL_FreeSurface(surf);
    if (!_images[handle]) { _next_image--; return -1; }
    return handle;
}

// CP437 8x8 bitmap font data (chars 32-127)
static const unsigned char _font8x8[96][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // 32 ' '
    {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00}, // 33 '!'
    {0x6C,0x6C,0x24,0x00,0x00,0x00,0x00,0x00}, // 34 '"'
    {0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00}, // 35 '#'
    {0x18,0x7E,0xC0,0x7C,0x06,0xFC,0x18,0x00}, // 36 '$'
    {0x00,0xC6,0xCC,0x18,0x30,0x66,0xC6,0x00}, // 37 '%'
    {0x38,0x6C,0x38,0x76,0xDC,0xCC,0x76,0x00}, // 38 '&'
    {0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00}, // 39 '''
    {0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00}, // 40 '('
    {0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00}, // 41 ')'
    {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00}, // 42 '*'
    {0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00}, // 43 '+'
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30}, // 44 ','
    {0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00}, // 45 '-'
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00}, // 46 '.'
    {0x02,0x06,0x0C,0x18,0x30,0x60,0xC0,0x00}, // 47 '/'
    {0x7C,0xC6,0xCE,0xDE,0xF6,0xE6,0x7C,0x00}, // 48 '0'
    {0x18,0x38,0x78,0x18,0x18,0x18,0x7E,0x00}, // 49 '1'
    {0x7C,0xC6,0x06,0x1C,0x30,0x66,0xFE,0x00}, // 50 '2'
    {0x7C,0xC6,0x06,0x3C,0x06,0xC6,0x7C,0x00}, // 51 '3'
    {0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x1E,0x00}, // 52 '4'
    {0xFE,0xC0,0xFC,0x06,0x06,0xC6,0x7C,0x00}, // 53 '5'
    {0x38,0x60,0xC0,0xFC,0xC6,0xC6,0x7C,0x00}, // 54 '6'
    {0xFE,0xC6,0x0C,0x18,0x30,0x30,0x30,0x00}, // 55 '7'
    {0x7C,0xC6,0xC6,0x7C,0xC6,0xC6,0x7C,0x00}, // 56 '8'
    {0x7C,0xC6,0xC6,0x7E,0x06,0x0C,0x78,0x00}, // 57 '9'
    {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00}, // 58 ':'
    {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x30}, // 59 ';'
    {0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00}, // 60 '<'
    {0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00}, // 61 '='
    {0x60,0x30,0x18,0x0C,0x18,0x30,0x60,0x00}, // 62 '>'
    {0x7C,0xC6,0x0C,0x18,0x18,0x00,0x18,0x00}, // 63 '?'
    {0x7C,0xC6,0xDE,0xDE,0xDE,0xC0,0x78,0x00}, // 64 '@'
    {0x38,0x6C,0xC6,0xC6,0xFE,0xC6,0xC6,0x00}, // 65 'A'
    {0xFC,0x66,0x66,0x7C,0x66,0x66,0xFC,0x00}, // 66 'B'
    {0x3C,0x66,0xC0,0xC0,0xC0,0x66,0x3C,0x00}, // 67 'C'
    {0xF8,0x6C,0x66,0x66,0x66,0x6C,0xF8,0x00}, // 68 'D'
    {0xFE,0x62,0x68,0x78,0x68,0x62,0xFE,0x00}, // 69 'E'
    {0xFE,0x62,0x68,0x78,0x68,0x60,0xF0,0x00}, // 70 'F'
    {0x3C,0x66,0xC0,0xC0,0xCE,0x66,0x3E,0x00}, // 71 'G'
    {0xC6,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0x00}, // 72 'H'
    {0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00}, // 73 'I'
    {0x1E,0x0C,0x0C,0x0C,0xCC,0xCC,0x78,0x00}, // 74 'J'
    {0xE6,0x66,0x6C,0x78,0x6C,0x66,0xE6,0x00}, // 75 'K'
    {0xF0,0x60,0x60,0x60,0x62,0x66,0xFE,0x00}, // 76 'L'
    {0xC6,0xEE,0xFE,0xD6,0xC6,0xC6,0xC6,0x00}, // 77 'M'
    {0xC6,0xE6,0xF6,0xDE,0xCE,0xC6,0xC6,0x00}, // 78 'N'
    {0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00}, // 79 'O'
    {0xFC,0x66,0x66,0x7C,0x60,0x60,0xF0,0x00}, // 80 'P'
    {0x7C,0xC6,0xC6,0xC6,0xD6,0xDE,0x7C,0x06}, // 81 'Q'
    {0xFC,0x66,0x66,0x7C,0x6C,0x66,0xE6,0x00}, // 82 'R'
    {0x7C,0xC6,0xC0,0x7C,0x06,0xC6,0x7C,0x00}, // 83 'S'
    {0x7E,0x5A,0x18,0x18,0x18,0x18,0x3C,0x00}, // 84 'T'
    {0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00}, // 85 'U'
    {0xC6,0xC6,0xC6,0xC6,0x6C,0x38,0x10,0x00}, // 86 'V'
    {0xC6,0xC6,0xC6,0xD6,0xFE,0xEE,0xC6,0x00}, // 87 'W'
    {0xC6,0xC6,0x6C,0x38,0x6C,0xC6,0xC6,0x00}, // 88 'X'
    {0x66,0x66,0x66,0x3C,0x18,0x18,0x3C,0x00}, // 89 'Y'
    {0xFE,0xC6,0x8C,0x18,0x32,0x66,0xFE,0x00}, // 90 'Z'
    {0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00}, // 91 '['
    {0xC0,0x60,0x30,0x18,0x0C,0x06,0x02,0x00}, // 92 '\\'
    {0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00}, // 93 ']'
    {0x10,0x38,0x6C,0xC6,0x00,0x00,0x00,0x00}, // 94 '^'
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF}, // 95 '_'
    {0x30,0x18,0x0C,0x00,0x00,0x00,0x00,0x00}, // 96 '`'
    {0x00,0x00,0x78,0x0C,0x7C,0xCC,0x76,0x00}, // 97 'a'
    {0xE0,0x60,0x7C,0x66,0x66,0x66,0xDC,0x00}, // 98 'b'
    {0x00,0x00,0x7C,0xC6,0xC0,0xC6,0x7C,0x00}, // 99 'c'
    {0x1C,0x0C,0x7C,0xCC,0xCC,0xCC,0x76,0x00}, // 100 'd'
    {0x00,0x00,0x7C,0xC6,0xFE,0xC0,0x7C,0x00}, // 101 'e'
    {0x1C,0x36,0x30,0x78,0x30,0x30,0x78,0x00}, // 102 'f'
    {0x00,0x00,0x76,0xCC,0xCC,0x7C,0x0C,0x78}, // 103 'g'
    {0xE0,0x60,0x6C,0x76,0x66,0x66,0xE6,0x00}, // 104 'h'
    {0x18,0x00,0x38,0x18,0x18,0x18,0x3C,0x00}, // 105 'i'
    {0x06,0x00,0x0E,0x06,0x06,0x66,0x66,0x3C}, // 106 'j'
    {0xE0,0x60,0x66,0x6C,0x78,0x6C,0xE6,0x00}, // 107 'k'
    {0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00}, // 108 'l'
    {0x00,0x00,0xEC,0xFE,0xD6,0xC6,0xC6,0x00}, // 109 'm'
    {0x00,0x00,0xDC,0x66,0x66,0x66,0x66,0x00}, // 110 'n'
    {0x00,0x00,0x7C,0xC6,0xC6,0xC6,0x7C,0x00}, // 111 'o'
    {0x00,0x00,0xDC,0x66,0x66,0x7C,0x60,0xF0}, // 112 'p'
    {0x00,0x00,0x76,0xCC,0xCC,0x7C,0x0C,0x1E}, // 113 'q'
    {0x00,0x00,0xDC,0x76,0x60,0x60,0xF0,0x00}, // 114 'r'
    {0x00,0x00,0x7C,0xC0,0x7C,0x06,0xFC,0x00}, // 115 's'
    {0x10,0x30,0x7C,0x30,0x30,0x34,0x18,0x00}, // 116 't'
    {0x00,0x00,0xCC,0xCC,0xCC,0xCC,0x76,0x00}, // 117 'u'
    {0x00,0x00,0xC6,0xC6,0xC6,0x6C,0x38,0x00}, // 118 'v'
    {0x00,0x00,0xC6,0xC6,0xD6,0xFE,0x6C,0x00}, // 119 'w'
    {0x00,0x00,0xC6,0x6C,0x38,0x6C,0xC6,0x00}, // 120 'x'
    {0x00,0x00,0xC6,0xC6,0xCE,0x76,0x06,0x7C}, // 121 'y'
    {0x00,0x00,0xFE,0x0C,0x38,0x60,0xFE,0x00}, // 122 'z'
    {0x0E,0x18,0x18,0x70,0x18,0x18,0x0E,0x00}, // 123 '{'
    {0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00}, // 124 '|'
    {0x70,0x18,0x18,0x0E,0x18,0x18,0x70,0x00}, // 125 '}'
    {0x76,0xDC,0x00,0x00,0x00,0x00,0x00,0x00}, // 126 '~'
};

void rb_printstring(int px, int py, const char* text) {
    SDL_Color c = get_palette_color(current_color);
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    int cx = px;
    for (int i = 0; text[i]; i++) {
        unsigned char ch = (unsigned char)text[i];
        if (ch < 32 || ch > 126) { cx += 8; continue; }
        const unsigned char* glyph = _font8x8[ch - 32];
        for (int row = 0; row < 8; row++) {
            unsigned char bits = glyph[row];
            for (int col = 0; col < 8; col++) {
                if (bits & (0x80 >> col)) {
                    SDL_RenderDrawPoint(renderer, cx + col, py + row);
                }
            }
        }
        cx += 8;
    }
}

void rb_init_mixer() {
    if (_mixer_initialized) return;
    // Ensure SDL audio subsystem is initialized
    if (SDL_WasInit(SDL_INIT_AUDIO) == 0) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
            printf("SDL audio init failed: %s\n", SDL_GetError());
            return;
        }
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer init failed: %s\n", Mix_GetError());
        return;
    }
    _mixer_initialized = 1;
}

int rb_sndopen(const char* filename) {
    rb_init_mixer();
    if (_next_sound >= MAX_SOUNDS) return -1;
    int handle = _next_sound++;
    _sounds[handle] = Mix_LoadWAV(filename);
    if (!_sounds[handle]) { _next_sound--; return -1; }
    return handle;
}

void SUB_GCLS();
void SUB_GCLSNOFLIP();
void SUB_GPRINT();
void SUB_GPRINTNOFLIP();
void SUB_GPRINTNL();
void SUB_GPRINTSAMELINE();
void SUB_GLOCATE();
void SUB_GCENTERPRINT();
void SUB_GCOLOR();
void SUB_GWAITKEY();
void SUB_GINPUT();
void SUB_GSLOW();
void SUB_TXTFILE();
void SUB_DATES1();
void SUB_ISOLATEPUNCTUATION();
void SUB_JOINPUNCTUATION();
void SUB_STARTSOUND();
void SUB_STOPSOUND();
void SUB_PLAYSOUND();
void SUB_PLAYSOUNDASYNC();
void SUB_PERSONRESTART();
void SUB_PERSONLOADARRAYS();
void SUB_PERSONGETREPLY();
void SUB_PERSONSPEAKTOTEXT();
void SUB_CHATADDLINE();
void SUB_CHATREDRAW();
void SUB_CONVERSATION();
void SUB_CHATROOM();
void SUB_NEWS();
void SUB_EMAIL();
void SUB_GUITAR();
void SUB_DREAMS();
void SUB_MUSIC();
void SUB_OUTSIDE();
void SUB_POEM();
void SUB_RESTARTPOEM();
void SUB_LOADPOEMDB();
void SUB_WARNINGSCREEN();
void SUB_OPENING();
void SUB_UPPER();
void SUB_GAMEMAIN();

// SUB GCLS
void SUB_GCLS() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    {
        int _x1 = (int)(0.000000);
        int _y1 = (int)(0.000000);
        int _x2 = (int)(799.000000);
        int _y2 = (int)(599.000000);
        int _color = (int)(0.000000);
        current_color = _color;
        fill_rect_buffered(_x1, _y1, _x2 - _x1, _y2 - _y1, current_color);
        if (_auto_display) SDL_RenderPresent(renderer);
    }
    RB_COLOR((int)(GFX_COLOR), 0);
    GFX_ROW = 0.000000;
    GFX_COL = 0.000000;
    SDL_RenderPresent(renderer);
}

// SUB GCLSNOFLIP
void SUB_GCLSNOFLIP() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    {
        int _x1 = (int)(0.000000);
        int _y1 = (int)(0.000000);
        int _x2 = (int)(799.000000);
        int _y2 = (int)(599.000000);
        int _color = (int)(0.000000);
        current_color = _color;
        fill_rect_buffered(_x1, _y1, _x2 - _x1, _y2 - _y1, current_color);
        if (_auto_display) SDL_RenderPresent(renderer);
    }
    RB_COLOR((int)(GFX_COLOR), 0);
    GFX_ROW = 0.000000;
    GFX_COL = 0.000000;
}

// SUB GPRINT
void SUB_GPRINT() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    if ((GFX_ROW < GFX_ROWS)) {
        { // _PRINTSTRING
            rb_printstring((int)((GFX_COL * GFX_CHARW)), (int)((GFX_ROW * GFX_CHARH)), GP_TEXT_str);
            if (_auto_display) SDL_RenderPresent(renderer);
        }
    }
    GFX_ROW = (GFX_ROW + 1.000000);
    GFX_COL = 0.000000;
    SDL_RenderPresent(renderer);
}

// SUB GPRINTNOFLIP
void SUB_GPRINTNOFLIP() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    if ((GFX_ROW < GFX_ROWS)) {
        { // _PRINTSTRING
            rb_printstring((int)((GFX_COL * GFX_CHARW)), (int)((GFX_ROW * GFX_CHARH)), GP_TEXT_str);
            if (_auto_display) SDL_RenderPresent(renderer);
        }
    }
    GFX_ROW = (GFX_ROW + 1.000000);
    GFX_COL = 0.000000;
}

// SUB GPRINTNL
void SUB_GPRINTNL() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    GFX_ROW = (GFX_ROW + 1.000000);
    GFX_COL = 0.000000;
}

// SUB GPRINTSAMELINE
void SUB_GPRINTSAMELINE() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    if ((GFX_ROW < GFX_ROWS)) {
        { // _PRINTSTRING
            rb_printstring((int)((GFX_COL * GFX_CHARW)), (int)((GFX_ROW * GFX_CHARH)), GP_TEXT_str);
            if (_auto_display) SDL_RenderPresent(renderer);
        }
    }
    GFX_COL = (GFX_COL + ((double)strlen(GP_TEXT_str)));
}

// SUB GLOCATE
void SUB_GLOCATE() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    GFX_ROW = GL_ROW;
    GFX_COL = GL_COL;
}

// SUB GCENTERPRINT
void SUB_GCENTERPRINT() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    GC_COL = ((GFX_COLS - ((double)strlen(GC_TEXT_str))) / 2.000000);
    if ((GC_COL < 0.000000)) {
        GC_COL = 0.000000;
    }
    if ((GC_ROW < GFX_ROWS)) {
        { // _PRINTSTRING
            rb_printstring((int)((GC_COL * GFX_CHARW)), (int)((GC_ROW * GFX_CHARH)), GC_TEXT_str);
            if (_auto_display) SDL_RenderPresent(renderer);
        }
    }
}

// SUB GCOLOR
void SUB_GCOLOR() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    RB_COLOR((int)(GC_CLR), 0);
    GFX_COLOR = GC_CLR;
}

// SUB GWAITKEY
void SUB_GWAITKEY() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    // 
    GC_CLR = 6.000000;
    SUB_GCOLOR();
    { // _PRINTSTRING
        rb_printstring((int)(8.000000), (int)(580.000000), "Press ENTER to continue...");
        if (_auto_display) SDL_RenderPresent(renderer);
    }
    SDL_RenderPresent(renderer);
    // 
    GW_DEBOUNCE = 1.000000;
    do {
        { // _LIMIT
            rb_process_events();
            int _fps = (int)(30.000000);
            if (_fps > 0) SDL_Delay(1000 / _fps);
        }
        if (((double)(_keyboard_state ? _keyboard_state[SDL_GetScancodeFromKey((int)(13.000000))] : 0) == 0.000000)) {
            GW_DEBOUNCE = 0.000000;
        }
        if (((GW_DEBOUNCE == 0.000000) && (double)(_keyboard_state ? _keyboard_state[SDL_GetScancodeFromKey((int)(13.000000))] : 0))) {
            break;
        }
    } while (1);
    // 
    do {
        { // _LIMIT
            rb_process_events();
            int _fps = (int)(30.000000);
            if (_fps > 0) SDL_Delay(1000 / _fps);
        }
        if (((double)(_keyboard_state ? _keyboard_state[SDL_GetScancodeFromKey((int)(13.000000))] : 0) == 0.000000)) {
            break;
        }
    } while (1);
    GC_CLR = 1.000000;
    SUB_GCOLOR();
}

// SUB GINPUT
void SUB_GINPUT() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    strcpy(GI_RESULT_str, "");
    GI_CURSOR_TIMER = 0.000000;
    GI_CURSOR_ON = 1.000000;
    GI_COOLDOWN = 0.000000;
    // 
    if ((((double)strlen(GI_PROMPT_str)) > 0.000000)) {
        strcpy(GP_TEXT_str, GI_PROMPT_str);
        SUB_GPRINTSAMELINE();
    }
    GI_STARTCOL = GFX_COL;
    GI_STARTROW = GFX_ROW;
    // 
    GI_WAIT = 1.000000;
    do {
        { // _LIMIT
            rb_process_events();
            int _fps = (int)(30.000000);
            if (_fps > 0) SDL_Delay(1000 / _fps);
        }
        GI_ANY = 0.000000;
        if ((double)(_keyboard_state ? _keyboard_state[SDL_GetScancodeFromKey((int)(13.000000))] : 0)) {
            GI_ANY = 1.000000;
        }
        if ((double)(_keyboard_state ? _keyboard_state[SDL_GetScancodeFromKey((int)(8.000000))] : 0)) {
            GI_ANY = 1.000000;
        }
        for (GI_WK = 32.000000; GI_WK <= 122.000000; GI_WK += 1) {
            if ((double)(_keyboard_state ? _keyboard_state[SDL_GetScancodeFromKey((int)(GI_WK))] : 0)) {
                GI_ANY = 1.000000;
            }
        }
        if ((GI_ANY == 0.000000)) {
            GI_WAIT = 0.000000;
        }
        if ((GI_WAIT == 0.000000)) {
            break;
        }
    } while (1);
    do {
        { // _LIMIT
            rb_process_events();
            int _fps = (int)(30.000000);
            if (_fps > 0) SDL_Delay(1000 / _fps);
        }
        // 
        GI_CURSOR_TIMER = (GI_CURSOR_TIMER + 1.000000);
        if ((GI_CURSOR_TIMER > 15.000000)) {
            if ((GI_CURSOR_ON == 1.000000)) {
                GI_CURSOR_ON = 0.000000;
            } else {
                GI_CURSOR_ON = 1.000000;
            }
            GI_CURSOR_TIMER = 0.000000;
        }
        if ((GI_COOLDOWN > 0.000000)) {
            GI_COOLDOWN = (GI_COOLDOWN - 1.000000);
        }
        if ((GI_COOLDOWN == 0.000000)) {
            GI_SHIFTED = 0.000000;
            if ((double)(_keyboard_state ? _keyboard_state[SDL_GetScancodeFromKey((int)(1073742049.000000))] : 0)) {
                GI_SHIFTED = 1.000000;
            }
            if ((double)(_keyboard_state ? _keyboard_state[SDL_GetScancodeFromKey((int)(1073742053.000000))] : 0)) {
                GI_SHIFTED = 1.000000;
            }
            // 
            if ((double)(_keyboard_state ? _keyboard_state[SDL_GetScancodeFromKey((int)(13.000000))] : 0)) {
                break;
            }
            // 
            if ((double)(_keyboard_state ? _keyboard_state[SDL_GetScancodeFromKey((int)(8.000000))] : 0)) {
                if ((((double)strlen(GI_RESULT_str)) > 0.000000)) {
                    strcpy(GI_RESULT_str, (RB_LEFT(_temp_str, GI_RESULT_str, (((double)strlen(GI_RESULT_str)) - 1.000000)), _temp_str));
                    GI_COOLDOWN = 5.000000;
                }
            }
            // 
            if ((GI_COOLDOWN == 0.000000)) {
                for (GI_K = 97.000000; GI_K <= 122.000000; GI_K += 1) {
                    if ((double)(_keyboard_state ? _keyboard_state[SDL_GetScancodeFromKey((int)(GI_K))] : 0)) {
                        if ((GI_SHIFTED == 1.000000)) {
                            strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, (RB_CHR(_temp_str, (GI_K - 32.000000)), _temp_str))));
                        } else {
                            strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, (RB_CHR(_temp_str, GI_K), _temp_str))));
                        }
                        GI_COOLDOWN = 5.000000;
                        break;
                    }
                }
            }
            // 
            if ((GI_COOLDOWN == 0.000000)) {
                for (GI_K = 48.000000; GI_K <= 57.000000; GI_K += 1) {
                    if ((double)(_keyboard_state ? _keyboard_state[SDL_GetScancodeFromKey((int)(GI_K))] : 0)) {
                        if ((GI_SHIFTED == 1.000000)) {
                            // 
                            { // SELECT CASE
                                double _select_val = GI_K;
                                if (_select_val == 49.000000) {
                                    strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, "!")));
                                } else if (_select_val == 50.000000) {
                                    strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, "@")));
                                } else if (_select_val == 51.000000) {
                                    strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, "#")));
                                } else if (_select_val == 52.000000) {
                                    strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, "$")));
                                } else if (_select_val == 53.000000) {
                                    strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, "%")));
                                } else if (_select_val == 55.000000) {
                                    strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, "&")));
                                } else if (_select_val == 57.000000) {
                                    strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, "(")));
                                } else if (_select_val == 48.000000) {
                                    strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, ")")));
                                }
                            } // END SELECT
                        } else {
                            strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, (RB_CHR(_temp_str, GI_K), _temp_str))));
                        }
                        GI_COOLDOWN = 5.000000;
                        break;
                    }
                }
            }
            // 
            if ((GI_COOLDOWN == 0.000000)) {
                if ((double)(_keyboard_state ? _keyboard_state[SDL_GetScancodeFromKey((int)(32.000000))] : 0)) {
                    strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, " ")));
                    GI_COOLDOWN = 5.000000;
                }
            }
            // 
            if ((GI_COOLDOWN == 0.000000)) {
                if ((double)(_keyboard_state ? _keyboard_state[SDL_GetScancodeFromKey((int)(46.000000))] : 0)) {
                    if ((GI_SHIFTED == 1.000000)) {
                        strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, ">")));
                    } else {
                        strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, ".")));
                    }
                    GI_COOLDOWN = 5.000000;
                }
            }
            if ((GI_COOLDOWN == 0.000000)) {
                if ((double)(_keyboard_state ? _keyboard_state[SDL_GetScancodeFromKey((int)(44.000000))] : 0)) {
                    if ((GI_SHIFTED == 1.000000)) {
                        strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, "<")));
                    } else {
                        strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, ",")));
                    }
                    GI_COOLDOWN = 5.000000;
                }
            }
            if ((GI_COOLDOWN == 0.000000)) {
                if ((double)(_keyboard_state ? _keyboard_state[SDL_GetScancodeFromKey((int)(45.000000))] : 0)) {
                    strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, "-")));
                    GI_COOLDOWN = 5.000000;
                }
            }
            if ((GI_COOLDOWN == 0.000000)) {
                if ((double)(_keyboard_state ? _keyboard_state[SDL_GetScancodeFromKey((int)(47.000000))] : 0)) {
                    if ((GI_SHIFTED == 1.000000)) {
                        strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, "?")));
                    } else {
                        strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, "/")));
                    }
                    GI_COOLDOWN = 5.000000;
                }
            }
            if ((GI_COOLDOWN == 0.000000)) {
                if ((double)(_keyboard_state ? _keyboard_state[SDL_GetScancodeFromKey((int)(39.000000))] : 0)) {
                    strcpy(GI_RESULT_str, (RB_CONCAT(_temp_str3, GI_RESULT_str, "'")));
                    GI_COOLDOWN = 5.000000;
                }
            }
        }
        // 
        GI_CLEARW = (GFX_COLS - GI_STARTCOL);
        if ((GI_CLEARW < 1.000000)) {
            GI_CLEARW = 1.000000;
        }
        {
            int _x1 = (int)((GI_STARTCOL * GFX_CHARW));
            int _y1 = (int)((GI_STARTROW * GFX_CHARH));
            int _x2 = (int)(799.000000);
            int _y2 = (int)((((GI_STARTROW * GFX_CHARH) + GFX_CHARH) - 1.000000));
            int _color = (int)(0.000000);
            current_color = _color;
            fill_rect_buffered(_x1, _y1, _x2 - _x1, _y2 - _y1, current_color);
            if (_auto_display) SDL_RenderPresent(renderer);
        }
        RB_COLOR((int)(GFX_COLOR), 0);
        if ((((double)strlen(GI_RESULT_str)) > 0.000000)) {
            { // _PRINTSTRING
                rb_printstring((int)((GI_STARTCOL * GFX_CHARW)), (int)((GI_STARTROW * GFX_CHARH)), GI_RESULT_str);
                if (_auto_display) SDL_RenderPresent(renderer);
            }
        }
        // 
        if ((GI_CURSOR_ON == 1.000000)) {
            GI_CX = ((GI_STARTCOL + ((double)strlen(GI_RESULT_str))) * GFX_CHARW);
            if ((GI_CX < 792.000000)) {
                {
                    int _x1 = (int)(GI_CX);
                    int _y1 = (int)((GI_STARTROW * GFX_CHARH));
                    int _x2 = (int)(((GI_CX + GFX_CHARW) - 1.000000));
                    int _y2 = (int)((((GI_STARTROW * GFX_CHARH) + GFX_CHARH) - 1.000000));
                    int _color = (int)(GFX_COLOR);
                    current_color = _color;
                    fill_rect_buffered(_x1, _y1, _x2 - _x1, _y2 - _y1, current_color);
                    if (_auto_display) SDL_RenderPresent(renderer);
                }
            }
        }
        SDL_RenderPresent(renderer);
    } while (1);
    GFX_ROW = (GI_STARTROW + 1.000000);
    GFX_COL = 0.000000;
}

// SUB GSLOW
void SUB_GSLOW() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    // 
    for (GS_I = 1.000000; GS_I <= ((double)strlen(SLOW_TEXT_str)); GS_I += 1) {
        strcpy(GS_CH_str, (RB_MID(_temp_str, SLOW_TEXT_str, GS_I, 1.000000), _temp_str));
        if (((GFX_COL < GFX_COLS) && (GFX_ROW < GFX_ROWS))) {
            { // _PRINTSTRING
                rb_printstring((int)((GFX_COL * GFX_CHARW)), (int)((GFX_ROW * GFX_CHARH)), GS_CH_str);
                if (_auto_display) SDL_RenderPresent(renderer);
            }
        }
        GFX_COL = (GFX_COL + 1.000000);
        SDL_RenderPresent(renderer);
        { // _LIMIT
            rb_process_events();
            int _fps = (int)(20.000000);
            if (_fps > 0) SDL_Delay(1000 / _fps);
        }
    }
}

// SUB TXTFILE
void SUB_TXTFILE() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    SUB_GCLS();
    strcpy(TF_str, (RB_CONCAT(_temp_str3, DATA_PATH_str, TXTFILE_NAME_str)));
    _file_handles[1] = fopen(TF_str, "r");
    while (!(feof(_file_handles[(int)(1.000000)]))) {
        if (fgets(TBUF_str, 255, _file_handles[1]) != NULL) {
            TBUF_str[strcspn(TBUF_str, "\n")] = 0;  // Remove newline
        }
        strcpy(GP_TEXT_str, TBUF_str);
        SUB_GPRINTNOFLIP();
        if ((GFX_ROW > 70.000000)) {
            GC_CLR = 6.000000;
            SUB_GCOLOR();
            strcpy(GP_TEXT_str, "-- more --");
            SUB_GPRINT();
            GC_CLR = 1.000000;
            SUB_GCOLOR();
            SUB_GWAITKEY();
            SUB_GCLS();
        }
    }
    if (_file_handles[1]) {
        fclose(_file_handles[1]);
        _file_handles[1] = NULL;
    }
    SDL_RenderPresent(renderer);
}

// SUB DATES1
void SUB_DATES1() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    DM = (fmod((DATES_MONTHS + 11.000000), 12.000000));
    DYR = (floor(((DATES_MONTHS - 1.000000) / 12.000000)) + 1997.000000);
    strcpy(DATES_RESULT_str, (RB_CONCAT(_temp_str3, (RB_CONCAT(_temp_str3, MONTH_NAMES_str[(int)(DM)], ",")), (RB_STR(_temp_str, DYR), _temp_str))));
}

// SUB ISOLATEPUNCTUATION
void SUB_ISOLATEPUNCTUATION() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    strcpy(IB_str, "");
    for (IP = 1.000000; IP <= ((double)strlen(ISOLATE_IN_str)); IP += 1) {
        strcpy(IC_str, (RB_MID(_temp_str, ISOLATE_IN_str, IP, 1.000000), _temp_str));
        if ((RB_INSTR(PUNCTUATION_str, IC_str) > 0.000000)) {
            strcpy(IB_str, (RB_CONCAT(_temp_str3, (RB_CONCAT(_temp_str3, (RB_CONCAT(_temp_str3, IB_str, " ")), IC_str)), " ")));
        } else {
            strcpy(IB_str, (RB_CONCAT(_temp_str3, IB_str, IC_str)));
        }
    }
    strcpy(ISOLATE_OUT_str, IB_str);
}

// SUB JOINPUNCTUATION
void SUB_JOINPUNCTUATION() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    strcpy(JB_str, JOIN_IN_str);
    for (JP = 1.000000; JP <= ((double)strlen(PUNCTUATION_str)); JP += 1) {
        strcpy(JPC_str, (RB_MID(_temp_str, PUNCTUATION_str, JP, 1.000000), _temp_str));
        strcpy(JFIND_str, (RB_CONCAT(_temp_str3, (RB_CONCAT(_temp_str3, " ", JPC_str)), " ")));
        JPLACE = RB_INSTR(JB_str, JFIND_str);
        while ((JPLACE > 0.000000)) {
            if ((JPLACE == 1.000000)) {
                strcpy(JB_str, (RB_CONCAT(_temp_str3, JPC_str, (RB_MID(_temp_str, JB_str, (JPLACE + 3.000000), 255), _temp_str))));
            } else {
                strcpy(JB_str, (RB_CONCAT(_temp_str3, (RB_CONCAT(_temp_str3, (RB_MID(_temp_str, JB_str, 1.000000, (JPLACE - 1.000000)), _temp_str), JPC_str)), (RB_MID(_temp_str, JB_str, (JPLACE + 3.000000), 255), _temp_str))));
            }
            JPLACE = RB_INSTR(JB_str, JFIND_str);
        }
    }
    strcpy(JOIN_OUT_str, JB_str);
}

// SUB STARTSOUND
void SUB_STARTSOUND() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    if ((CURRENT_SOUND > 0.000000)) {
        { // _SNDSTOP
            int _sh = (int)(CURRENT_SOUND);
            if (_sh > 0 && _sh < MAX_SOUNDS && _sound_channels[_sh] >= 0) {
                Mix_HaltChannel(_sound_channels[_sh]);
            }
        }
    }
    strcpy(SND_PATH_str, (RB_CONCAT(_temp_str3, SOUND_PATH_str, SOUND_FILE_str)));
    CURRENT_SOUND = (double)rb_sndopen(SND_PATH_str);
    if ((CURRENT_SOUND > 0.000000)) {
        { // _SNDPLAY
            int _sh = (int)(CURRENT_SOUND);
            if (_sh > 0 && _sh < MAX_SOUNDS && _sounds[_sh]) {
                _sound_channels[_sh] = Mix_PlayChannel(-1, _sounds[_sh], 0);
            }
        }
    }
}

// SUB STOPSOUND
void SUB_STOPSOUND() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    if ((CURRENT_SOUND > 0.000000)) {
        { // _SNDSTOP
            int _sh = (int)(CURRENT_SOUND);
            if (_sh > 0 && _sh < MAX_SOUNDS && _sound_channels[_sh] >= 0) {
                Mix_HaltChannel(_sound_channels[_sh]);
            }
        }
    }
}

// SUB PLAYSOUND
void SUB_PLAYSOUND() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    strcpy(PS_PATH_str, (RB_CONCAT(_temp_str3, SOUND_PATH_str, SOUND_FILE_str)));
    PS_SND = (double)rb_sndopen(PS_PATH_str);
    if ((PS_SND > 0.000000)) {
        { // _SNDPLAY
            int _sh = (int)(PS_SND);
            if (_sh > 0 && _sh < MAX_SOUNDS && _sounds[_sh]) {
                _sound_channels[_sh] = Mix_PlayChannel(-1, _sounds[_sh], 0);
            }
        }
        if ((SOUND_WAIT > 0.000000)) {
            #ifdef _WIN32
            Sleep((DWORD)(SOUND_WAIT * 1000));
            #else
            usleep((unsigned int)(SOUND_WAIT * 1000000));
            #endif
        }
    }
}

// SUB PLAYSOUNDASYNC
void SUB_PLAYSOUNDASYNC() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    strcpy(PA_PATH_str, (RB_CONCAT(_temp_str3, SOUND_PATH_str, SOUND_FILE_str)));
    PA_SND = (double)rb_sndopen(PA_PATH_str);
    if ((PA_SND > 0.000000)) {
        { // _SNDPLAY
            int _sh = (int)(PA_SND);
            if (_sh > 0 && _sh < MAX_SOUNDS && _sounds[_sh]) {
                _sound_channels[_sh] = Mix_PlayChannel(-1, _sounds[_sh], 0);
            }
        }
    }
}

// SUB PERSONRESTART
void SUB_PERSONRESTART() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    PID = CURRENT_PID;
    PERSON_KCNT[(int)(PID)] = 0.000000;
    PERSON_RCNT[(int)(PID)] = 0.000000;
    PERSON_WCNT[(int)(PID)] = 0.000000;
    PERSON_NOKEYINDEX[(int)(PID)] = 0.000000;
    strcpy(PERSON_GREETING_str[(int)(PID)], "");
    strcpy(PERSON_YOU_str[(int)(PID)], "");
    strcpy(PERSON_SCRIPT_str[(int)(PID)], "");
    for (PRI = 0.000000; PRI <= MAX_KEYWORDS; PRI += 1) {
        strcpy(PERSONKEYWORDS_str[(int)(PID)][(int)(PRI)], "");
        PERSONRSTARTS[(int)(PID)][(int)(PRI)] = 0.000000;
        PERSONRENDS[(int)(PID)][(int)(PRI)] = 0.000000;
        PERSONRINDEX[(int)(PID)][(int)(PRI)] = 0.000000;
    }
    for (PRI = 0.000000; PRI <= MAX_REPLIES; PRI += 1) {
        strcpy(PERSONREPLIES_str[(int)(PID)][(int)(PRI)], "");
    }
    for (PRI = 0.000000; PRI <= MAX_WORDS; PRI += 1) {
        strcpy(PERSONWORDIN_str[(int)(PID)][(int)(PRI)], "");
        strcpy(PERSONWORDOUT_str[(int)(PID)][(int)(PRI)], "");
    }
}

// SUB PERSONLOADARRAYS
void SUB_PERSONLOADARRAYS() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    PID = CURRENT_PID;
    PLA_STARTR = 0.000000;
    PLA_ENDR = 0.000000;
    PLA_READINGR = 0.000000;
    strcpy(PLAF_str, (RB_CONCAT(_temp_str3, DATA_PATH_str, LOADSCRIPT_FILE_str)));
    _file_handles[1] = fopen(PLAF_str, "r");
    while (!(feof(_file_handles[(int)(1.000000)]))) {
        if (fgets(PLAFLINE_str, 255, _file_handles[1]) != NULL) {
            PLAFLINE_str[strcspn(PLAFLINE_str, "\n")] = 0;  // Remove newline
        }
        strcpy(PLA_PREFIX_str, (RB_LEFT(_temp_str, PLAFLINE_str, 2.000000), _temp_str));
        { // SELECT CASE
            char _select_val[RB_STR_SIZE]; strcpy(_select_val, PLA_PREFIX_str);
            if (strcmp(_select_val, "g:") == 0) {
                strcpy(PERSON_GREETING_str[(int)(PID)], (RB_LTRIM(_temp_str, (RB_RTRIM(_temp_str, (RB_MID(_temp_str, PLAFLINE_str, 3.000000, 255), _temp_str)), _temp_str)), _temp_str));
            } else if (strcmp(_select_val, "y:") == 0) {
                strcpy(PERSON_YOU_str[(int)(PID)], (RB_LTRIM(_temp_str, (RB_RTRIM(_temp_str, (RB_MID(_temp_str, PLAFLINE_str, 3.000000, 255), _temp_str)), _temp_str)), _temp_str));
            } else if (strcmp(_select_val, "c:") == 0) {
                strcpy(PERSON_SCRIPT_str[(int)(PID)], (RB_LTRIM(_temp_str, (RB_RTRIM(_temp_str, (RB_MID(_temp_str, PLAFLINE_str, 3.000000, 255), _temp_str)), _temp_str)), _temp_str));
            } else if (strcmp(_select_val, "s:") == 0) {
                PERSON_WCNT[(int)(PID)] = (PERSON_WCNT[(int)(PID)] + 1.000000);
                PLA_WC = PERSON_WCNT[(int)(PID)];
                PLA_TMP = RB_INSTR(PLAFLINE_str, ">");
                if ((PLA_TMP > 0.000000)) {
                    strcpy(PERSONWORDIN_str[(int)(PID)][(int)(PLA_WC)], (RB_CONCAT(_temp_str3, (RB_CONCAT(_temp_str3, " ", (RB_LTRIM(_temp_str, (RB_RTRIM(_temp_str, (RB_MID(_temp_str, PLAFLINE_str, 3.000000, (PLA_TMP - 3.000000)), _temp_str)), _temp_str)), _temp_str))), " ")));
                    strcpy(PERSONWORDOUT_str[(int)(PID)][(int)(PLA_WC)], (RB_CONCAT(_temp_str3, (RB_CONCAT(_temp_str3, " ", (RB_LTRIM(_temp_str, (RB_RTRIM(_temp_str, (RB_MID(_temp_str, PLAFLINE_str, (PLA_TMP + 1.000000), 255), _temp_str)), _temp_str)), _temp_str))), " ")));
                }
            } else if (strcmp(_select_val, "r:") == 0) {
                PERSON_RCNT[(int)(PID)] = (PERSON_RCNT[(int)(PID)] + 1.000000);
                PLA_RC = PERSON_RCNT[(int)(PID)];
                strcpy(PERSONREPLIES_str[(int)(PID)][(int)(PLA_RC)], (RB_LTRIM(_temp_str, (RB_RTRIM(_temp_str, (RB_MID(_temp_str, PLAFLINE_str, 3.000000, 255), _temp_str)), _temp_str)), _temp_str));
                if ((PLA_READINGR == 0.000000)) {
                    PLA_READINGR = 1.000000;
                    PLA_STARTR = PLA_RC;
                }
            } else if (strcmp(_select_val, "k:") == 0) {
                if ((PLA_READINGR == 1.000000)) {
                    PLA_ENDR = PERSON_RCNT[(int)(PID)];
                    PLA_READINGR = 0.000000;
                }
                if ((PERSON_RCNT[(int)(PID)] > 0.000000)) {
                    PERSON_KCNT[(int)(PID)] = (PERSON_KCNT[(int)(PID)] + 1.000000);
                    PLA_KC = PERSON_KCNT[(int)(PID)];
                    strcpy(PLA_KWORD_str, (RB_LTRIM(_temp_str, (RB_RTRIM(_temp_str, (RB_MID(_temp_str, PLAFLINE_str, 3.000000, 255), _temp_str)), _temp_str)), _temp_str));
                    strcpy(PERSONKEYWORDS_str[(int)(PID)][(int)(PLA_KC)], (RB_CONCAT(_temp_str3, (RB_CONCAT(_temp_str3, " ", PLA_KWORD_str)), " ")));
                    PERSONRSTARTS[(int)(PID)][(int)(PLA_KC)] = PLA_STARTR;
                    PERSONRINDEX[(int)(PID)][(int)(PLA_KC)] = PLA_STARTR;
                    PERSONRENDS[(int)(PID)][(int)(PLA_KC)] = PLA_ENDR;
                    if ((strcmp(PLA_KWORD_str, "nokeyfound") == 0)) {
                        PERSON_NOKEYINDEX[(int)(PID)] = PLA_KC;
                    }
                }
            } else if (strcmp(_select_val, "e:") == 0) {
                break;
            }
        } // END SELECT
    }
    if (_file_handles[1]) {
        fclose(_file_handles[1]);
        _file_handles[1] = NULL;
    }
    if ((PLA_READINGR == 1.000000)) {
        PLA_ENDR = PERSON_RCNT[(int)(PID)];
        PERSON_KCNT[(int)(PID)] = (PERSON_KCNT[(int)(PID)] + 1.000000);
        PLA_KC = PERSON_KCNT[(int)(PID)];
        strcpy(PERSONKEYWORDS_str[(int)(PID)][(int)(PLA_KC)], "nokeyfound");
        PERSONRSTARTS[(int)(PID)][(int)(PLA_KC)] = PLA_STARTR;
        PERSONRINDEX[(int)(PID)][(int)(PLA_KC)] = PLA_STARTR;
        PERSONRENDS[(int)(PID)][(int)(PLA_KC)] = PLA_ENDR;
        PERSON_NOKEYINDEX[(int)(PID)] = PLA_KC;
    }
}

// SUB PERSONGETREPLY
void SUB_PERSONGETREPLY() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    PID = CURRENT_PID;
    if ((REPLY_SWITCH == 0.000000)) {
        // 
        strcpy(PGR_PROMPT_str, (RB_CONCAT(_temp_str3, PERSON_YOU_str[(int)(PID)], ": ")));
        GC_CLR = 7.000000;
        SUB_GCOLOR();
        strcpy(GI_PROMPT_str, PGR_PROMPT_str);
        SUB_GINPUT();
        strcpy(PGR_INPT_str, GI_RESULT_str);
        GC_CLR = 1.000000;
        SUB_GCOLOR();
        strcpy(PGR_CHK_str, (RB_LCASE(_temp_str, PGR_INPT_str), _temp_str));
        if ((((((strcmp(PGR_CHK_str, "q") == 0) || (strcmp(PGR_CHK_str, "x") == 0)) || (strcmp(PGR_CHK_str, "goodbye") == 0)) || (strcmp(PGR_CHK_str, "good night") == 0)) || (strcmp(PGR_CHK_str, "bye") == 0))) {
            strcpy(REPLY_RESULT_str, "Goodbye!");
            return;
        }
    } else {
        strcpy(PGR_INPT_str, REPLY_INPUT_str);
    }
    strcpy(PGR_INPT_str, (RB_CONCAT(_temp_str3, (RB_CONCAT(_temp_str3, " ", PGR_INPT_str)), " ")));
    strcpy(ISOLATE_IN_str, PGR_INPT_str);
    SUB_ISOLATEPUNCTUATION();
    strcpy(PGR_INPT_str, ISOLATE_OUT_str);
    PGR_KFLAG = 0.000000;
    PGR_KIDX = 0.000000;
    strcpy(PGR_TAIL_str, "");
    // 
    strcpy(PGR_LCINPT_str, (RB_LCASE(_temp_str, PGR_INPT_str), _temp_str));
    for (PGR_K = 1.000000; PGR_K <= PERSON_KCNT[(int)(PID)]; PGR_K += 1) {
        strcpy(PGR_LCKEY_str, (RB_LCASE(_temp_str, PERSONKEYWORDS_str[(int)(PID)][(int)(PGR_K)]), _temp_str));
        PGR_KFOUND = RB_INSTR(PGR_LCINPT_str, PGR_LCKEY_str);
        if ((PGR_KFOUND > 0.000000)) {
            strcpy(PGR_TAIL_str, (RB_CONCAT(_temp_str3, " ", (RB_MID(_temp_str, PGR_INPT_str, (PGR_KFOUND + ((double)strlen(PERSONKEYWORDS_str[(int)(PID)][(int)(PGR_K)]))), 255), _temp_str))));
            for (PGR_L = 1.000000; PGR_L <= ((double)strlen(PGR_TAIL_str)); PGR_L += 1) {
                for (PGR_W = 1.000000; PGR_W <= PERSON_WCNT[(int)(PID)]; PGR_W += 1) {
                    strcpy(PGR_LCPART_str, (RB_LCASE(_temp_str, (RB_MID(_temp_str, PGR_TAIL_str, PGR_L, ((double)strlen(PERSONWORDIN_str[(int)(PID)][(int)(PGR_W)]))), _temp_str)), _temp_str));
                    strcpy(PGR_LCWORD_str, (RB_LCASE(_temp_str, PERSONWORDIN_str[(int)(PID)][(int)(PGR_W)]), _temp_str));
                    if ((strcmp(PGR_LCPART_str, PGR_LCWORD_str) == 0)) {
                        strcpy(PGR_BEFORE_str, (RB_MID(_temp_str, PGR_TAIL_str, 1.000000, (PGR_L - 1.000000)), _temp_str));
                        strcpy(PGR_AFTER_str, (RB_MID(_temp_str, PGR_TAIL_str, (PGR_L + ((double)strlen(PERSONWORDIN_str[(int)(PID)][(int)(PGR_W)]))), 255), _temp_str));
                        strcpy(PGR_TAIL_str, (RB_CONCAT(_temp_str3, (RB_CONCAT(_temp_str3, PGR_BEFORE_str, PERSONWORDOUT_str[(int)(PID)][(int)(PGR_W)])), PGR_AFTER_str)));
                        break;
                    }
                }
            }
            PGR_KFLAG = 1.000000;
            PGR_KIDX = PGR_K;
            break;
        }
    }
    if ((PGR_KFLAG == 0.000000)) {
        PGR_KIDX = PERSON_NOKEYINDEX[(int)(PID)];
    }
    PGR_RANGE = ((PERSONRENDS[(int)(PID)][(int)(PGR_KIDX)] - PERSONRSTARTS[(int)(PID)][(int)(PGR_KIDX)]) + 1.000000);
    PGR_RANDIDX = (floor((PGR_RANGE * RB_RND())) + PERSONRSTARTS[(int)(PID)][(int)(PGR_KIDX)]);
    strcpy(PGR_ANSW_str, PERSONREPLIES_str[(int)(PID)][(int)(PGR_RANDIDX)]);
    if ((PGR_KIDX == PERSON_NOKEYINDEX[(int)(PID)])) {
        PERSONRINDEX[(int)(PID)][(int)(PGR_KIDX)] = PGR_RANDIDX;
    }
    if ((strcmp((RB_RIGHT(_temp_str, PGR_ANSW_str, 1.000000), _temp_str), "*") != 0)) {
        strcpy(REPLY_RESULT_str, PGR_ANSW_str);
        return;
    }
    strcpy(PGR_RTRIM_str, (RB_RTRIM(_temp_str, PGR_TAIL_str), _temp_str));
    strcpy(PGR_TRIMTAIL_str, (RB_LTRIM(_temp_str, PGR_RTRIM_str), _temp_str));
    if ((((double)strlen(PGR_TRIMTAIL_str)) == 0.000000)) {
        strcpy(REPLY_RESULT_str, (RB_CONCAT(_temp_str3, "Please elaborate on, ", PERSONKEYWORDS_str[(int)(PID)][(int)(PGR_KIDX)])));
    } else {
        strcpy(JOIN_IN_str, PGR_TAIL_str);
        SUB_JOINPUNCTUATION();
        strcpy(PGR_TAIL_str, JOIN_OUT_str);
        strcpy(PGR_BASE_str, (RB_MID(_temp_str, PGR_ANSW_str, 1.000000, (((double)strlen(PGR_ANSW_str)) - 1.000000)), _temp_str));
        strcpy(REPLY_RESULT_str, (RB_CONCAT(_temp_str3, PGR_BASE_str, PGR_TAIL_str)));
    }
}

// SUB PERSONSPEAKTOTEXT
void SUB_PERSONSPEAKTOTEXT() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    PID = CURRENT_PID;
    GC_CLR = 9.000000;
    SUB_GCOLOR();
    strcpy(PST_MSG_str, (RB_CONCAT(_temp_str3, PERSON_SCRIPT_str[(int)(PID)], ": ")));
    strcpy(GP_TEXT_str, PST_MSG_str);
    SUB_GPRINTSAMELINE();
    GC_CLR = 1.000000;
    SUB_GCOLOR();
    strcpy(SLOW_TEXT_str, SPEAK_TEXT_str);
    SUB_GSLOW();
    SUB_GPRINTNL();
}

// SUB CHATADDLINE
void SUB_CHATADDLINE() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    // 
    if ((CHAT_COUNT < MAX_CHAT)) {
        CHAT_COUNT = (CHAT_COUNT + 1.000000);
    } else {
        // 
        for (CA_I = 1.000000; CA_I <= (MAX_CHAT - 1.000000); CA_I += 1) {
            strcpy(CHAT_LINES_str[(int)(CA_I)], CHAT_LINES_str[(int)((CA_I + 1.000000))]);
            CHAT_COLORS[(int)(CA_I)] = CHAT_COLORS[(int)((CA_I + 1.000000))];
        }
    }
    strcpy(CHAT_LINES_str[(int)(CHAT_COUNT)], CA_TEXT_str);
    CHAT_COLORS[(int)(CHAT_COUNT)] = CA_COLOR;
}

// SUB CHATREDRAW
void SUB_CHATREDRAW() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    // 
    SUB_GCLSNOFLIP();
    GC_CLR = 2.000000;
    SUB_GCOLOR();
    GC_ROW = 0.000000;
    strcpy(GC_TEXT_str, "=== CHAT ROOM ===");
    SUB_GCENTERPRINT();
    // 
    CR_MAXLINES = 66.000000;
    CR_START = 1.000000;
    if ((CHAT_COUNT > CR_MAXLINES)) {
        CR_START = ((CHAT_COUNT - CR_MAXLINES) + 1.000000);
    }
    CR_ROW = 2.000000;
    for (CR_I = CR_START; CR_I <= CHAT_COUNT; CR_I += 1) {
        GC_CLR = CHAT_COLORS[(int)(CR_I)];
        SUB_GCOLOR();
        GFX_ROW = CR_ROW;
        GFX_COL = 1.000000;
        strcpy(GP_TEXT_str, CHAT_LINES_str[(int)(CR_I)]);
        SUB_GPRINTNOFLIP();
        CR_ROW = (CR_ROW + 1.000000);
    }
    GC_CLR = 1.000000;
    SUB_GCOLOR();
    GFX_ROW = (CR_ROW + 1.000000);
    GFX_COL = 0.000000;
    SDL_RenderPresent(renderer);
}

// SUB CONVERSATION
void SUB_CONVERSATION() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    CURRENT_PID = 0.000000;
    SUB_PERSONRESTART();
    SUB_GCLS();
    GC_CLR = 6.000000;
    SUB_GCOLOR();
    GL_ROW = 5.000000;
    GL_COL = 5.000000;
    SUB_GLOCATE();
    strcpy(GP_TEXT_str, "connecting...");
    SUB_GPRINT();
    #ifdef _WIN32
    Sleep((DWORD)(2.000000 * 1000));
    #else
    usleep((unsigned int)(2.000000 * 1000000));
    #endif
    SUB_GCLS();
    GC_CLR = 1.000000;
    SUB_GCOLOR();
    strcpy(LOADSCRIPT_FILE_str, CONV_FILE_str);
    SUB_PERSONLOADARRAYS();
    // 
    CHAT_COUNT = 0.000000;
    strcpy(CA_TEXT_str, PERSON_GREETING_str[(int)(0.000000)]);
    CA_COLOR = 9.000000;
    SUB_CHATADDLINE();
    SUB_CHATREDRAW();
    do {
        REPLY_SWITCH = 0.000000;
        GFX_ROW = (CHAT_COUNT + 4.000000);
        if ((GFX_ROW > 68.000000)) {
            GFX_ROW = 68.000000;
        }
        GFX_COL = 0.000000;
        SUB_PERSONGETREPLY();
        strcpy(CONV_RPLY_str, REPLY_RESULT_str);
        // 
        strcpy(CA_TEXT_str, (RB_CONCAT(_temp_str3, (RB_CONCAT(_temp_str3, PERSON_YOU_str[(int)(0.000000)], ": ")), GI_RESULT_str)));
        CA_COLOR = 7.000000;
        SUB_CHATADDLINE();
        // 
        CURRENT_PID = 0.000000;
        strcpy(CA_TEXT_str, (RB_CONCAT(_temp_str3, (RB_CONCAT(_temp_str3, PERSON_SCRIPT_str[(int)(0.000000)], ": ")), CONV_RPLY_str)));
        CA_COLOR = 9.000000;
        SUB_CHATADDLINE();
        SUB_CHATREDRAW();
    } while (!((strcmp(CONV_RPLY_str, "Goodbye!") == 0)));
    SUB_GCLS();
    GC_CLR = 6.000000;
    SUB_GCOLOR();
    GL_ROW = 5.000000;
    GL_COL = 5.000000;
    SUB_GLOCATE();
    strcpy(GP_TEXT_str, "disconnected...");
    SUB_GPRINT();
    SUB_GWAITKEY();
}

// SUB CHATROOM
void SUB_CHATROOM() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    CURRENT_PID = 0.000000;
    SUB_PERSONRESTART();
    CURRENT_PID = 1.000000;
    SUB_PERSONRESTART();
    CURRENT_PID = 2.000000;
    SUB_PERSONRESTART();
    SUB_GCLS();
    GC_CLR = 6.000000;
    SUB_GCOLOR();
    GL_ROW = 5.000000;
    GL_COL = 5.000000;
    SUB_GLOCATE();
    strcpy(GP_TEXT_str, "connecting...");
    SUB_GPRINT();
    #ifdef _WIN32
    Sleep((DWORD)(2.000000 * 1000));
    #else
    usleep((unsigned int)(2.000000 * 1000000));
    #endif
    SUB_GCLS();
    GC_CLR = 1.000000;
    SUB_GCOLOR();
    CURRENT_PID = 0.000000;
    strcpy(LOADSCRIPT_FILE_str, "yuka_guide.txt");
    SUB_PERSONLOADARRAYS();
    CURRENT_PID = 1.000000;
    strcpy(LOADSCRIPT_FILE_str, "mike.txt");
    SUB_PERSONLOADARRAYS();
    CURRENT_PID = 2.000000;
    strcpy(LOADSCRIPT_FILE_str, "ken.txt");
    SUB_PERSONLOADARRAYS();
    // 
    CHAT_COUNT = 0.000000;
    strcpy(CA_TEXT_str, PERSON_GREETING_str[(int)(0.000000)]);
    CA_COLOR = 9.000000;
    SUB_CHATADDLINE();
    SUB_CHATREDRAW();
    do {
        // 
        GFX_ROW = (CHAT_COUNT + 4.000000);
        if ((GFX_ROW > 68.000000)) {
            GFX_ROW = 68.000000;
        }
        GFX_COL = 0.000000;
        GC_CLR = 7.000000;
        SUB_GCOLOR();
        strcpy(GI_PROMPT_str, (RB_CONCAT(_temp_str3, PERSON_YOU_str[(int)(0.000000)], ": ")));
        SUB_GINPUT();
        strcpy(CR_INTEXT_str, GI_RESULT_str);
        strcpy(CR_LC_str, (RB_LCASE(_temp_str, CR_INTEXT_str), _temp_str));
        if ((((((strcmp(CR_LC_str, "q") == 0) || (strcmp(CR_LC_str, "x") == 0)) || (strcmp(CR_LC_str, "goodbye") == 0)) || (strcmp(CR_LC_str, "good night") == 0)) || (strcmp(CR_LC_str, "bye") == 0))) {
            strcpy(CA_TEXT_str, (RB_CONCAT(_temp_str3, PERSON_SCRIPT_str[(int)(0.000000)], ": Goodbye!")));
            CA_COLOR = 9.000000;
            SUB_CHATADDLINE();
            SUB_CHATREDRAW();
            break;
        }
        // 
        strcpy(CA_TEXT_str, (RB_CONCAT(_temp_str3, (RB_CONCAT(_temp_str3, PERSON_YOU_str[(int)(0.000000)], ": ")), CR_INTEXT_str)));
        CA_COLOR = 7.000000;
        SUB_CHATADDLINE();
        // 
        CURRENT_PID = 0.000000;
        strcpy(REPLY_INPUT_str, CR_INTEXT_str);
        REPLY_SWITCH = 1.000000;
        SUB_PERSONGETREPLY();
        strcpy(CA_TEXT_str, (RB_CONCAT(_temp_str3, (RB_CONCAT(_temp_str3, PERSON_SCRIPT_str[(int)(0.000000)], ": ")), REPLY_RESULT_str)));
        CA_COLOR = 9.000000;
        SUB_CHATADDLINE();
        CURRENT_PID = 1.000000;
        strcpy(REPLY_INPUT_str, CR_INTEXT_str);
        REPLY_SWITCH = 1.000000;
        SUB_PERSONGETREPLY();
        strcpy(CA_TEXT_str, (RB_CONCAT(_temp_str3, (RB_CONCAT(_temp_str3, PERSON_SCRIPT_str[(int)(1.000000)], ": ")), REPLY_RESULT_str)));
        CA_COLOR = 10.000000;
        SUB_CHATADDLINE();
        CURRENT_PID = 2.000000;
        strcpy(REPLY_INPUT_str, CR_INTEXT_str);
        REPLY_SWITCH = 1.000000;
        SUB_PERSONGETREPLY();
        strcpy(CA_TEXT_str, (RB_CONCAT(_temp_str3, (RB_CONCAT(_temp_str3, PERSON_SCRIPT_str[(int)(2.000000)], ": ")), REPLY_RESULT_str)));
        CA_COLOR = 4.000000;
        SUB_CHATADDLINE();
        SUB_CHATREDRAW();
    } while (!((strcmp((RB_LCASE(_temp_str, CR_INTEXT_str), _temp_str), "bye") == 0)));
    SUB_GCLS();
    GC_CLR = 6.000000;
    SUB_GCOLOR();
    GL_ROW = 5.000000;
    GL_COL = 5.000000;
    SUB_GLOCATE();
    strcpy(GP_TEXT_str, "disconnected...");
    SUB_GPRINT();
    SUB_GWAITKEY();
}

// SUB NEWS
void SUB_NEWS() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    if ((NEWS_LOADED == 0.000000)) {
        strcpy(NF_str, (RB_CONCAT(_temp_str3, DATA_PATH_str, "news.txt")));
        _file_handles[1] = fopen(NF_str, "r");
        while (!(feof(_file_handles[(int)(1.000000)]))) {
            if (fgets(NL_str, 255, _file_handles[1]) != NULL) {
                NL_str[strcspn(NL_str, "\n")] = 0;  // Remove newline
            }
            NEWSBLA_COUNT = (NEWSBLA_COUNT + 1.000000);
            strcpy(NEWSBLA_str[(int)(NEWSBLA_COUNT)], NL_str);
        }
        if (_file_handles[1]) {
            fclose(_file_handles[1]);
            _file_handles[1] = NULL;
        }
        NEWS_LOADED = 1.000000;
    }
    SUB_GCLS();
    GC_CLR = 2.000000;
    SUB_GCOLOR();
    strcpy(GP_TEXT_str, "NEWS FEED");
    SUB_GPRINT();
    SUB_GPRINTNL();
    GC_CLR = 1.000000;
    SUB_GCOLOR();
    if ((NEWSBLA_COUNT > 0.000000)) {
        for (NI = 1.000000; NI <= 10.000000; NI += 1) {
            NIDX = (floor((RB_RND() * NEWSBLA_COUNT)) + 1.000000);
            strcpy(GP_TEXT_str, NEWSBLA_str[(int)(NIDX)]);
            SUB_GPRINT();
            SUB_GPRINTNL();
        }
    }
    SUB_GWAITKEY();
}

// SUB EMAIL
void SUB_EMAIL() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    SUB_GCLS();
    { // SELECT CASE
        double _select_val = COUNTER;
        if (_select_val == 0.000000) {
            GC_CLR = 6.000000;
            SUB_GCOLOR();
            GC_ROW = 5.000000;
            strcpy(GC_TEXT_str, "NO EMAIL...");
            SUB_GCENTERPRINT();
            SDL_RenderPresent(renderer);
            COUNTER = (COUNTER + 1.000000);
        } else if (_select_val == 1.000000) {
            strcpy(TXTFILE_NAME_str, "email1.txt");
            SUB_TXTFILE();
            strcpy(SOUND_FILE_str, "email1.wav");
            SUB_STARTSOUND();
            SUB_GPRINTNL();
            SUB_GPRINTNL();
            GC_CLR = 2.000000;
            SUB_GCOLOR();
            strcpy(GP_TEXT_str, "1. REPLY OR 2. IGNORE");
            SUB_GPRINT();
            GC_CLR = 7.000000;
            SUB_GCOLOR();
            strcpy(GI_PROMPT_str, "Your choice: ");
            SUB_GINPUT();
            strcpy(EK_str, GI_RESULT_str);
            if ((strcmp(EK_str, "2") == 0)) {
                SUB_STOPSOUND();
                return;
            }
            if ((strcmp(EK_str, "1") == 0)) {
                SUB_GPRINTNL();
                SUB_GPRINTNL();
                GC_CLR = 1.000000;
                SUB_GCOLOR();
                strcpy(GP_TEXT_str, "YOU REPLY TO YUKA-YUKA CENTER");
                SUB_GPRINT();
                COUNTER = (COUNTER + 1.000000);
            }
        } else if (_select_val == 2.000000) {
            strcpy(TXTFILE_NAME_str, "email2.txt");
            SUB_TXTFILE();
            strcpy(SOUND_FILE_str, "email1.wav");
            SUB_STARTSOUND();
        } else if (_select_val == 3.000000) {
            strcpy(TXTFILE_NAME_str, "email3.txt");
            SUB_TXTFILE();
            strcpy(SOUND_FILE_str, "email1.wav");
            SUB_STARTSOUND();
        }
    } // END SELECT
    SUB_GWAITKEY();
    SUB_STOPSOUND();
}

// SUB GUITAR
void SUB_GUITAR() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    { // SELECT CASE
        double _select_val = SONGCOUNTER;
        if (_select_val == 0.000000) {
            strcpy(TXTFILE_NAME_str, "guitar1.txt");
            SUB_TXTFILE();
            strcpy(SOUND_FILE_str, "pray_song.wav");
            SUB_STARTSOUND();
            SONGCOUNTER = (SONGCOUNTER + 1.000000);
        } else if (_select_val == 1.000000) {
            strcpy(TXTFILE_NAME_str, "guitar2.txt");
            SUB_TXTFILE();
            strcpy(SOUND_FILE_str, "love_song.wav");
            SUB_STARTSOUND();
            SONGCOUNTER = (SONGCOUNTER + 1.000000);
        } else if (_select_val == 2.000000) {
            strcpy(TXTFILE_NAME_str, "guitar3.txt");
            SUB_TXTFILE();
            strcpy(SOUND_FILE_str, "blues_song.wav");
            SUB_STARTSOUND();
            SONGCOUNTER = (SONGCOUNTER + 1.000000);
        } else if (_select_val == 3.000000) {
            strcpy(TXTFILE_NAME_str, "pray2.txt");
            SUB_TXTFILE();
            strcpy(SOUND_FILE_str, "pray_song2.wav");
            SUB_STARTSOUND();
            SONGCOUNTER = (SONGCOUNTER + 1.000000);
        } else if (_select_val == 4.000000) {
            strcpy(TXTFILE_NAME_str, "guitar4.txt");
            SUB_TXTFILE();
            strcpy(SOUND_FILE_str, "guitar_song.wav");
            SUB_STARTSOUND();
            SONGCOUNTER = 0.000000;
        }
    } // END SELECT
    SUB_GWAITKEY();
    SUB_STOPSOUND();
}

// SUB DREAMS
void SUB_DREAMS() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    SUB_GCLS();
    DIDX = floor((RB_RND() * 7.000000));
    strcpy(TXTFILE_NAME_str, DREAM_FILES_str[(int)(DIDX)]);
    SUB_TXTFILE();
    DIDX = floor((RB_RND() * 5.000000));
    strcpy(SOUND_FILE_str, DREAM_MUSIC_str[(int)(DIDX)]);
    SUB_STARTSOUND();
    SUB_GWAITKEY();
    SUB_STOPSOUND();
}

// SUB MUSIC
void SUB_MUSIC() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    SUB_GCLS();
    if ((COUNTER < 2.000000)) {
        strcpy(TXTFILE_NAME_str, "music.txt");
        SUB_TXTFILE();
        strcpy(SOUND_FILE_str, "fbsloop44.wav");
        SUB_STARTSOUND();
    } else {
        SUB_GCLS();
        strcpy(TXTFILE_NAME_str, "music3.txt");
        SUB_TXTFILE();
        strcpy(SOUND_FILE_str, "legends.ogg");
        SUB_STARTSOUND();
    }
    SUB_GWAITKEY();
    SUB_STOPSOUND();
    SUB_GCLS();
}

// SUB OUTSIDE
void SUB_OUTSIDE() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    { // SELECT CASE
        double _select_val = COUNTER;
        if (_select_val == 0.000000 || _select_val == 1.000000) {
            strcpy(TXTFILE_NAME_str, "walk1.txt");
            SUB_TXTFILE();
            strcpy(SOUND_FILE_str, "walk1.wav");
            SUB_STARTSOUND();
            SUB_GWAITKEY();
            SUB_STOPSOUND();
        } else if (_select_val == 2.000000) {
            strcpy(TXTFILE_NAME_str, "yuka.txt");
            SUB_TXTFILE();
            strcpy(SOUND_FILE_str, "dream_eva.wav");
            SUB_STARTSOUND();
            SUB_GPRINTNL();
            SUB_GPRINTNL();
            GC_CLR = 2.000000;
            SUB_GCOLOR();
            strcpy(GP_TEXT_str, "1. YOU NEED TO THINK ABOUT IT");
            SUB_GPRINT();
            strcpy(GP_TEXT_str, "2. YES YOU WOULD LIKE TO TRY AND PARTICIPATE");
            SUB_GPRINT();
            GC_CLR = 7.000000;
            SUB_GCOLOR();
            strcpy(GI_PROMPT_str, "Your choice: ");
            SUB_GINPUT();
            strcpy(OK_str, GI_RESULT_str);
            SUB_STOPSOUND();
            if ((strcmp(OK_str, "2") == 0)) {
                SUB_GPRINTNL();
                GC_CLR = 1.000000;
                SUB_GCOLOR();
                strcpy(GP_TEXT_str, "THE GUIDE SHAKES YOUR HAND AND SAY 'THEN SEE YOU NEXT TIME' AND SMILE");
                SUB_GPRINT();
                COUNTER = (COUNTER + 1.000000);
            }
            SUB_GWAITKEY();
        } else if (_select_val == 3.000000) {
            strcpy(TXTFILE_NAME_str, MEETING_FILES_str[(int)(MEETINGCOUNTER)]);
            SUB_TXTFILE();
            strcpy(SOUND_FILE_str, "dream_eva.wav");
            SUB_STARTSOUND();
            SUB_GWAITKEY();
            SUB_STOPSOUND();
            MEETINGCOUNTER = (MEETINGCOUNTER + 1.000000);
            if ((MEETINGCOUNTER > 3.000000)) {
                COUNTER = 4.000000;
            }
        } else if (_select_val == 4.000000) {
            SUB_GCLS();
            strcpy(TXTFILE_NAME_str, "workshop.txt");
            SUB_TXTFILE();
            strcpy(SOUND_FILE_str, "dream_eva.wav");
            SUB_STARTSOUND();
            SUB_GWAITKEY();
            SUB_STOPSOUND();
            FINISHED = 1.000000;
        }
    } // END SELECT
}

// SUB POEM
void SUB_POEM() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    SUB_GCLS();
    SUB_RESTARTPOEM();
    strcpy(LOADPOEM_FILE_str, "poemDB.txt");
    SUB_LOADPOEMDB();
    GC_CLR = 2.000000;
    SUB_GCOLOR();
    GC_ROW = 1.000000;
    strcpy(GC_TEXT_str, "A POEM...");
    SUB_GCENTERPRINT();
    GFX_ROW = 3.000000;
    GFX_COL = 0.000000;
    GC_CLR = 5.000000;
    SUB_GCOLOR();
    for (PI = 1.000000; PI <= 3.000000; PI += 1) {
        SUB_GPRINTNL();
        if ((PLACES_COUNT > 0.000000)) {
            PIDX = (floor((RB_RND() * PLACES_COUNT)) + 1.000000);
            strcpy(GP_TEXT_str, PLACES_str[(int)(PIDX)]);
            SUB_GPRINTNOFLIP();
        }
        if ((COLORS_COUNT > 0.000000)) {
            PIDX = (floor((RB_RND() * COLORS_COUNT)) + 1.000000);
            strcpy(GP_TEXT_str, COLORS_ARR_str[(int)(PIDX)]);
            SUB_GPRINTNOFLIP();
        }
        if ((METAPHORE_COUNT > 0.000000)) {
            PIDX = (floor((RB_RND() * METAPHORE_COUNT)) + 1.000000);
            strcpy(GP_TEXT_str, METAPHORE_str[(int)(PIDX)]);
            SUB_GPRINTNOFLIP();
        }
        if ((OBJECTS_COUNT > 0.000000)) {
            PIDX = (floor((RB_RND() * OBJECTS_COUNT)) + 1.000000);
            strcpy(GP_TEXT_str, OBJECTS_str[(int)(PIDX)]);
            SUB_GPRINTNOFLIP();
        }
        if ((GOOD_COUNT > 0.000000)) {
            PIDX = (floor((RB_RND() * GOOD_COUNT)) + 1.000000);
            strcpy(GP_TEXT_str, GOOD_ARR_str[(int)(PIDX)]);
            SUB_GPRINTNOFLIP();
        }
        if ((BAD_COUNT > 0.000000)) {
            PIDX = (floor((RB_RND() * BAD_COUNT)) + 1.000000);
            strcpy(GP_TEXT_str, BAD_ARR_str[(int)(PIDX)]);
            SUB_GPRINTNOFLIP();
        }
    }
    SDL_RenderPresent(renderer);
    DIDX = floor((RB_RND() * 5.000000));
    strcpy(SOUND_FILE_str, DREAM_MUSIC_str[(int)(DIDX)]);
    SUB_STARTSOUND();
    SUB_GWAITKEY();
    SUB_STOPSOUND();
}

// SUB RESTARTPOEM
void SUB_RESTARTPOEM() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    PLACES_COUNT = 0.000000;
    COLORS_COUNT = 0.000000;
    METAPHORE_COUNT = 0.000000;
    OBJECTS_COUNT = 0.000000;
    GOOD_COUNT = 0.000000;
    BAD_COUNT = 0.000000;
}

// SUB LOADPOEMDB
void SUB_LOADPOEMDB() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    strcpy(LPF_str, (RB_CONCAT(_temp_str3, DATA_PATH_str, LOADPOEM_FILE_str)));
    _file_handles[1] = fopen(LPF_str, "r");
    while (!(feof(_file_handles[(int)(1.000000)]))) {
        if (fgets(LPFLINE_str, 255, _file_handles[1]) != NULL) {
            LPFLINE_str[strcspn(LPFLINE_str, "\n")] = 0;  // Remove newline
        }
        strcpy(LP_PREFIX_str, (RB_LEFT(_temp_str, LPFLINE_str, 2.000000), _temp_str));
        strcpy(LP_VAL_str, (RB_LTRIM(_temp_str, (RB_RTRIM(_temp_str, (RB_MID(_temp_str, LPFLINE_str, 3.000000, 255), _temp_str)), _temp_str)), _temp_str));
        { // SELECT CASE
            char _select_val[RB_STR_SIZE]; strcpy(_select_val, LP_PREFIX_str);
            if (strcmp(_select_val, "p:") == 0) {
                PLACES_COUNT = (PLACES_COUNT + 1.000000);
                strcpy(PLACES_str[(int)(PLACES_COUNT)], LP_VAL_str);
            } else if (strcmp(_select_val, "c:") == 0) {
                COLORS_COUNT = (COLORS_COUNT + 1.000000);
                strcpy(COLORS_ARR_str[(int)(COLORS_COUNT)], LP_VAL_str);
            } else if (strcmp(_select_val, "m:") == 0) {
                METAPHORE_COUNT = (METAPHORE_COUNT + 1.000000);
                strcpy(METAPHORE_str[(int)(METAPHORE_COUNT)], LP_VAL_str);
            } else if (strcmp(_select_val, "o:") == 0) {
                OBJECTS_COUNT = (OBJECTS_COUNT + 1.000000);
                strcpy(OBJECTS_str[(int)(OBJECTS_COUNT)], LP_VAL_str);
            } else if (strcmp(_select_val, "g:") == 0) {
                GOOD_COUNT = (GOOD_COUNT + 1.000000);
                strcpy(GOOD_ARR_str[(int)(GOOD_COUNT)], LP_VAL_str);
            } else if (strcmp(_select_val, "b:") == 0) {
                BAD_COUNT = (BAD_COUNT + 1.000000);
                strcpy(BAD_ARR_str[(int)(BAD_COUNT)], LP_VAL_str);
            } else if (strcmp(_select_val, "e:") == 0) {
                break;
            }
        } // END SELECT
    }
    if (_file_handles[1]) {
        fclose(_file_handles[1]);
        _file_handles[1] = NULL;
    }
}

// SUB WARNINGSCREEN
void SUB_WARNINGSCREEN() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    SUB_GCLS();
    GC_CLR = 3.000000;
    SUB_GCOLOR();
    strcpy(TXTFILE_NAME_str, "warning.txt");
    SUB_TXTFILE();
    GC_CLR = 7.000000;
    SUB_GCOLOR();
    strcpy(GI_PROMPT_str, "Press 1 to continue or 2 to quit: ");
    SUB_GINPUT();
    strcpy(WK_str, GI_RESULT_str);
    if ((strcmp(WK_str, "2") == 0)) {
        wait_for_close();  // Wait for user to close window or press ESC
        cleanup_graphics();
        exit(0);
    }
}

// SUB OPENING
void SUB_OPENING() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    SUB_GCLS();
    // 
    OP_IMG = (double)rb_loadimage((RB_CONCAT(_temp_str3, IMG_PATH_str, "opening.png")));
    if ((OP_IMG > 0.000000)) {
        { // _PUTIMAGE
            int _src_h = (int)(OP_IMG);
            if (_src_h > 0 && _src_h < MAX_IMAGES && _images[_src_h]) {
                SDL_Rect _dst = {(int)(0.000000), (int)(0.000000), _image_w[_src_h], _image_h[_src_h]};
                SDL_RenderCopy(renderer, _images[_src_h], NULL, &_dst);
                if (_auto_display) SDL_RenderPresent(renderer);
            }
        }
        { // _FREEIMAGE
            int _h = (int)(OP_IMG);
            if (_h > 0 && _h < MAX_IMAGES && _images[_h]) {
                SDL_DestroyTexture(_images[_h]);
                _images[_h] = NULL;
            }
        }
    }
    GC_CLR = 2.000000;
    SUB_GCOLOR();
    GC_ROW = 52.000000;
    strcpy(GC_TEXT_str, "H I K I K O M O R Y");
    SUB_GCENTERPRINT();
    GC_CLR = 5.000000;
    SUB_GCOLOR();
    GC_ROW = 55.000000;
    strcpy(GC_TEXT_str, "A GAME BY RON77");
    SUB_GCENTERPRINT();
    GC_CLR = 4.000000;
    SUB_GCOLOR();
    GC_ROW = 58.000000;
    strcpy(GC_TEXT_str, "(RBBasic Graphics Edition)");
    SUB_GCENTERPRINT();
    SDL_RenderPresent(renderer);
    strcpy(SOUND_FILE_str, "sabrina.wav");
    SUB_STARTSOUND();
    SUB_GWAITKEY();
}

// SUB UPPER
void SUB_UPPER() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    SUB_GCLS();
    UPPER_LINES_COUNT = 0.000000;
    strcpy(UF_str, (RB_CONCAT(_temp_str3, DATA_PATH_str, UPPER_FILE_str)));
    _file_handles[1] = fopen(UF_str, "r");
    while (!(feof(_file_handles[(int)(1.000000)]))) {
        if (fgets(UFLINE_str, 255, _file_handles[1]) != NULL) {
            UFLINE_str[strcspn(UFLINE_str, "\n")] = 0;  // Remove newline
        }
        UPPER_LINES_COUNT = (UPPER_LINES_COUNT + 1.000000);
        strcpy(UPPER_LINES_str[(int)(UPPER_LINES_COUNT)], UFLINE_str);
    }
    if (_file_handles[1]) {
        fclose(_file_handles[1]);
        _file_handles[1] = NULL;
    }
    GC_CLR = 2.000000;
    SUB_GCOLOR();
    // 
    UP_TOTAL = ((UPPER_LINES_COUNT + GFX_ROWS) * GFX_CHARH);
    for (UP_OFFSET = 0.000000; UP_OFFSET <= UP_TOTAL; UP_OFFSET += 1) {
        SUB_GCLSNOFLIP();
        for (UI = 1.000000; UI <= UPPER_LINES_COUNT; UI += 1) {
            UY = ((((UI - 1.000000) * GFX_CHARH) - UP_OFFSET) + 600.000000);
            if (((UY > (-8.000000)) && (UY < 600.000000))) {
                strcpy(UP_PAD_str, (RB_CONCAT(_temp_str3, (RB_SPACE(_temp_str, (int)(12.000000)), _temp_str), UPPER_LINES_str[(int)(UI)])));
                { // _PRINTSTRING
                    rb_printstring((int)(0.000000), (int)(UY), UP_PAD_str);
                    if (_auto_display) SDL_RenderPresent(renderer);
                }
            }
        }
        SDL_RenderPresent(renderer);
        { // _LIMIT
            rb_process_events();
            int _fps = (int)(20.000000);
            if (_fps > 0) SDL_Delay(1000 / _fps);
        }
    }
    GC_CLR = 5.000000;
    SUB_GCOLOR();
    SUB_GCLS();
    GC_ROW = 37.000000;
    strcpy(GC_TEXT_str, "E N D");
    SUB_GCENTERPRINT();
    SDL_RenderPresent(renderer);
    SUB_GWAITKEY();
}

// SUB GAMEMAIN
void SUB_GAMEMAIN() {
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    strcpy(TXTFILE_NAME_str, "startpoint.txt");
    SUB_TXTFILE();
    SUB_GWAITKEY();
    SUB_STOPSOUND();
    do {
        SUB_GCLS();
        DATES_MONTHS = TIMEPASS;
        SUB_DATES1();
        GC_CLR = 2.000000;
        SUB_GCOLOR();
        GC_ROW = 5.000000;
        strcpy(GC_TEXT_str, (RB_CONCAT(_temp_str3, "DATE: ", DATES_RESULT_str)));
        SUB_GCENTERPRINT();
        GC_CLR = 5.000000;
        SUB_GCOLOR();
        GC_ROW = 10.000000;
        strcpy(GC_TEXT_str, "WHAT DO YOU WANT TO DO?");
        SUB_GCENTERPRINT();
        GC_CLR = 1.000000;
        SUB_GCOLOR();
        GC_ROW = 15.000000;
        strcpy(GC_TEXT_str, "1. GO TO SLEEP...");
        SUB_GCENTERPRINT();
        GC_ROW = 19.000000;
        strcpy(GC_TEXT_str, "2. PLAY A COMPUTER GAME...");
        SUB_GCENTERPRINT();
        GC_ROW = 23.000000;
        strcpy(GC_TEXT_str, "3. LISTEN TO MUSIC...");
        SUB_GCENTERPRINT();
        GC_ROW = 27.000000;
        strcpy(GC_TEXT_str, "4. CHECK E-MAIL...");
        SUB_GCENTERPRINT();
        GC_ROW = 31.000000;
        strcpy(GC_TEXT_str, "5. GO OUTSIDE...");
        SUB_GCENTERPRINT();
        GC_ROW = 35.000000;
        strcpy(GC_TEXT_str, "6. JOIN A CHAT-ROOM ON THE NET...");
        SUB_GCENTERPRINT();
        GC_ROW = 39.000000;
        strcpy(GC_TEXT_str, "7. WRITE A POEM...");
        SUB_GCENTERPRINT();
        GC_ROW = 43.000000;
        strcpy(GC_TEXT_str, "8. PLAY SOMETHING WITH YOUR GUITAR...");
        SUB_GCENTERPRINT();
        GC_ROW = 47.000000;
        strcpy(GC_TEXT_str, "9. LISTEN TO THE NEWS...");
        SUB_GCENTERPRINT();
        GC_CLR = 3.000000;
        SUB_GCOLOR();
        GC_ROW = 53.000000;
        strcpy(GC_TEXT_str, "TYPE Q TO EXIT GAME...");
        SUB_GCENTERPRINT();
        SDL_RenderPresent(renderer);
        GC_CLR = 7.000000;
        SUB_GCOLOR();
        GFX_ROW = 58.000000;
        GFX_COL = 30.000000;
        strcpy(GI_PROMPT_str, "Your choice: ");
        SUB_GINPUT();
        strcpy(GK_str, GI_RESULT_str);
        { // SELECT CASE
            char _select_val[RB_STR_SIZE]; strcpy(_select_val, (RB_LCASE(_temp_str, GK_str), _temp_str));
            if (strcmp(_select_val, "9") == 0) {
                SUB_NEWS();
            } else if (strcmp(_select_val, "3") == 0) {
                SUB_MUSIC();
            } else if (strcmp(_select_val, "1") == 0) {
                SUB_DREAMS();
            } else if (strcmp(_select_val, "5") == 0) {
                SUB_OUTSIDE();
            } else if (strcmp(_select_val, "4") == 0) {
                SUB_EMAIL();
            } else if (strcmp(_select_val, "2") == 0) {
                system("cmd /c start https://youtu.be/-CbyAk3Sn9I");
            } else if (strcmp(_select_val, "6") == 0) {
                if ((COUNTER < 2.000000)) {
                    strcpy(CONV_FILE_str, "chat1.txt");
                    SUB_CONVERSATION();
                } else {
                    SUB_CHATROOM();
                }
            } else if (strcmp(_select_val, "8") == 0) {
                SUB_GUITAR();
            } else if (strcmp(_select_val, "7") == 0) {
                SUB_POEM();
            }
        } // END SELECT
        TIMEPASS = (TIMEPASS + 1.000000);
    } while (!(((strcmp((RB_LCASE(_temp_str, GK_str), _temp_str), "q") == 0) || (FINISHED == 1.000000))));
}

int main(int argc, char* argv[]) {
    // Build COMMAND$ from command line arguments
    if (argc > 1) {
        _command_str[0] = '\0';
        for (int _ci = 1; _ci < argc; _ci++) {
            if (_ci > 1) strncat(_command_str, " ", RB_STR_SIZE - strlen(_command_str) - 1);
            strncat(_command_str, argv[_ci], RB_STR_SIZE - strlen(_command_str) - 1);
        }
    }
    
    char _temp_str[RB_STR_SIZE];
    char _temp_str2[RB_STR_SIZE];
    char _temp_str3[RB_STR_SIZE];
    
    // 
    // 
    // 
    // 
    // 
    // 
    srand((unsigned int)(RB_TIMER()));
    _rnd_initialized = 1;
    // 
    init_graphics((int)(14.000000));
    SDL_RenderPresent(renderer);
    // 
    // 
    {
        int _pattr = (int)(0.000000);
        int _pcolor = (int)(1710638.000000);
        if (_pattr >= 0 && _pattr < 16) {
            // Extract RGB from packed color value (QBasic format: &HRRGGBB or 6-bit EGA)
            int _r, _g, _b;
            if (_pcolor > 63) {
                // Treat as packed RGB (&HRRGGBB)
                _r = (_pcolor >> 16) & 0xFF;
                _g = (_pcolor >> 8) & 0xFF;
                _b = _pcolor & 0xFF;
            } else {
                // 6-bit EGA color: RGBRGB (2 bits each, high/low intensity)
                _r = ((_pcolor & 4) ? 170 : 0) + ((_pcolor & 32) ? 85 : 0);
                _g = ((_pcolor & 2) ? 170 : 0) + ((_pcolor & 16) ? 85 : 0);
                _b = ((_pcolor & 1) ? 170 : 0) + ((_pcolor & 8) ? 85 : 0);
            }
            palette[_pattr].r = _r;
            palette[_pattr].g = _g;
            palette[_pattr].b = _b;
            palette[_pattr].a = 255;
        }
    }
    {
        int _pattr = (int)(1.000000);
        int _pcolor = (int)(65345.000000);
        if (_pattr >= 0 && _pattr < 16) {
            // Extract RGB from packed color value (QBasic format: &HRRGGBB or 6-bit EGA)
            int _r, _g, _b;
            if (_pcolor > 63) {
                // Treat as packed RGB (&HRRGGBB)
                _r = (_pcolor >> 16) & 0xFF;
                _g = (_pcolor >> 8) & 0xFF;
                _b = _pcolor & 0xFF;
            } else {
                // 6-bit EGA color: RGBRGB (2 bits each, high/low intensity)
                _r = ((_pcolor & 4) ? 170 : 0) + ((_pcolor & 32) ? 85 : 0);
                _g = ((_pcolor & 2) ? 170 : 0) + ((_pcolor & 16) ? 85 : 0);
                _b = ((_pcolor & 1) ? 170 : 0) + ((_pcolor & 8) ? 85 : 0);
            }
            palette[_pattr].r = _r;
            palette[_pattr].g = _g;
            palette[_pattr].b = _b;
            palette[_pattr].a = 255;
        }
    }
    {
        int _pattr = (int)(2.000000);
        int _pcolor = (int)(16776960.000000);
        if (_pattr >= 0 && _pattr < 16) {
            // Extract RGB from packed color value (QBasic format: &HRRGGBB or 6-bit EGA)
            int _r, _g, _b;
            if (_pcolor > 63) {
                // Treat as packed RGB (&HRRGGBB)
                _r = (_pcolor >> 16) & 0xFF;
                _g = (_pcolor >> 8) & 0xFF;
                _b = _pcolor & 0xFF;
            } else {
                // 6-bit EGA color: RGBRGB (2 bits each, high/low intensity)
                _r = ((_pcolor & 4) ? 170 : 0) + ((_pcolor & 32) ? 85 : 0);
                _g = ((_pcolor & 2) ? 170 : 0) + ((_pcolor & 16) ? 85 : 0);
                _b = ((_pcolor & 1) ? 170 : 0) + ((_pcolor & 8) ? 85 : 0);
            }
            palette[_pattr].r = _r;
            palette[_pattr].g = _g;
            palette[_pattr].b = _b;
            palette[_pattr].a = 255;
        }
    }
    {
        int _pattr = (int)(3.000000);
        int _pcolor = (int)(16729156.000000);
        if (_pattr >= 0 && _pattr < 16) {
            // Extract RGB from packed color value (QBasic format: &HRRGGBB or 6-bit EGA)
            int _r, _g, _b;
            if (_pcolor > 63) {
                // Treat as packed RGB (&HRRGGBB)
                _r = (_pcolor >> 16) & 0xFF;
                _g = (_pcolor >> 8) & 0xFF;
                _b = _pcolor & 0xFF;
            } else {
                // 6-bit EGA color: RGBRGB (2 bits each, high/low intensity)
                _r = ((_pcolor & 4) ? 170 : 0) + ((_pcolor & 32) ? 85 : 0);
                _g = ((_pcolor & 2) ? 170 : 0) + ((_pcolor & 16) ? 85 : 0);
                _b = ((_pcolor & 1) ? 170 : 0) + ((_pcolor & 8) ? 85 : 0);
            }
            palette[_pattr].r = _r;
            palette[_pattr].g = _g;
            palette[_pattr].b = _b;
            palette[_pattr].a = 255;
        }
    }
    {
        int _pattr = (int)(4.000000);
        int _pcolor = (int)(4491519.000000);
        if (_pattr >= 0 && _pattr < 16) {
            // Extract RGB from packed color value (QBasic format: &HRRGGBB or 6-bit EGA)
            int _r, _g, _b;
            if (_pcolor > 63) {
                // Treat as packed RGB (&HRRGGBB)
                _r = (_pcolor >> 16) & 0xFF;
                _g = (_pcolor >> 8) & 0xFF;
                _b = _pcolor & 0xFF;
            } else {
                // 6-bit EGA color: RGBRGB (2 bits each, high/low intensity)
                _r = ((_pcolor & 4) ? 170 : 0) + ((_pcolor & 32) ? 85 : 0);
                _g = ((_pcolor & 2) ? 170 : 0) + ((_pcolor & 16) ? 85 : 0);
                _b = ((_pcolor & 1) ? 170 : 0) + ((_pcolor & 8) ? 85 : 0);
            }
            palette[_pattr].r = _r;
            palette[_pattr].g = _g;
            palette[_pattr].b = _b;
            palette[_pattr].a = 255;
        }
    }
    {
        int _pattr = (int)(5.000000);
        int _pcolor = (int)(16777215.000000);
        if (_pattr >= 0 && _pattr < 16) {
            // Extract RGB from packed color value (QBasic format: &HRRGGBB or 6-bit EGA)
            int _r, _g, _b;
            if (_pcolor > 63) {
                // Treat as packed RGB (&HRRGGBB)
                _r = (_pcolor >> 16) & 0xFF;
                _g = (_pcolor >> 8) & 0xFF;
                _b = _pcolor & 0xFF;
            } else {
                // 6-bit EGA color: RGBRGB (2 bits each, high/low intensity)
                _r = ((_pcolor & 4) ? 170 : 0) + ((_pcolor & 32) ? 85 : 0);
                _g = ((_pcolor & 2) ? 170 : 0) + ((_pcolor & 16) ? 85 : 0);
                _b = ((_pcolor & 1) ? 170 : 0) + ((_pcolor & 8) ? 85 : 0);
            }
            palette[_pattr].r = _r;
            palette[_pattr].g = _g;
            palette[_pattr].b = _b;
            palette[_pattr].a = 255;
        }
    }
    {
        int _pattr = (int)(6.000000);
        int _pcolor = (int)(8947848.000000);
        if (_pattr >= 0 && _pattr < 16) {
            // Extract RGB from packed color value (QBasic format: &HRRGGBB or 6-bit EGA)
            int _r, _g, _b;
            if (_pcolor > 63) {
                // Treat as packed RGB (&HRRGGBB)
                _r = (_pcolor >> 16) & 0xFF;
                _g = (_pcolor >> 8) & 0xFF;
                _b = _pcolor & 0xFF;
            } else {
                // 6-bit EGA color: RGBRGB (2 bits each, high/low intensity)
                _r = ((_pcolor & 4) ? 170 : 0) + ((_pcolor & 32) ? 85 : 0);
                _g = ((_pcolor & 2) ? 170 : 0) + ((_pcolor & 16) ? 85 : 0);
                _b = ((_pcolor & 1) ? 170 : 0) + ((_pcolor & 8) ? 85 : 0);
            }
            palette[_pattr].r = _r;
            palette[_pattr].g = _g;
            palette[_pattr].b = _b;
            palette[_pattr].a = 255;
        }
    }
    {
        int _pattr = (int)(7.000000);
        int _pcolor = (int)(16746496.000000);
        if (_pattr >= 0 && _pattr < 16) {
            // Extract RGB from packed color value (QBasic format: &HRRGGBB or 6-bit EGA)
            int _r, _g, _b;
            if (_pcolor > 63) {
                // Treat as packed RGB (&HRRGGBB)
                _r = (_pcolor >> 16) & 0xFF;
                _g = (_pcolor >> 8) & 0xFF;
                _b = _pcolor & 0xFF;
            } else {
                // 6-bit EGA color: RGBRGB (2 bits each, high/low intensity)
                _r = ((_pcolor & 4) ? 170 : 0) + ((_pcolor & 32) ? 85 : 0);
                _g = ((_pcolor & 2) ? 170 : 0) + ((_pcolor & 16) ? 85 : 0);
                _b = ((_pcolor & 1) ? 170 : 0) + ((_pcolor & 8) ? 85 : 0);
            }
            palette[_pattr].r = _r;
            palette[_pattr].g = _g;
            palette[_pattr].b = _b;
            palette[_pattr].a = 255;
        }
    }
    {
        int _pattr = (int)(8.000000);
        int _pcolor = (int)(16711935.000000);
        if (_pattr >= 0 && _pattr < 16) {
            // Extract RGB from packed color value (QBasic format: &HRRGGBB or 6-bit EGA)
            int _r, _g, _b;
            if (_pcolor > 63) {
                // Treat as packed RGB (&HRRGGBB)
                _r = (_pcolor >> 16) & 0xFF;
                _g = (_pcolor >> 8) & 0xFF;
                _b = _pcolor & 0xFF;
            } else {
                // 6-bit EGA color: RGBRGB (2 bits each, high/low intensity)
                _r = ((_pcolor & 4) ? 170 : 0) + ((_pcolor & 32) ? 85 : 0);
                _g = ((_pcolor & 2) ? 170 : 0) + ((_pcolor & 16) ? 85 : 0);
                _b = ((_pcolor & 1) ? 170 : 0) + ((_pcolor & 8) ? 85 : 0);
            }
            palette[_pattr].r = _r;
            palette[_pattr].g = _g;
            palette[_pattr].b = _b;
            palette[_pattr].a = 255;
        }
    }
    {
        int _pattr = (int)(9.000000);
        int _pcolor = (int)(65535.000000);
        if (_pattr >= 0 && _pattr < 16) {
            // Extract RGB from packed color value (QBasic format: &HRRGGBB or 6-bit EGA)
            int _r, _g, _b;
            if (_pcolor > 63) {
                // Treat as packed RGB (&HRRGGBB)
                _r = (_pcolor >> 16) & 0xFF;
                _g = (_pcolor >> 8) & 0xFF;
                _b = _pcolor & 0xFF;
            } else {
                // 6-bit EGA color: RGBRGB (2 bits each, high/low intensity)
                _r = ((_pcolor & 4) ? 170 : 0) + ((_pcolor & 32) ? 85 : 0);
                _g = ((_pcolor & 2) ? 170 : 0) + ((_pcolor & 16) ? 85 : 0);
                _b = ((_pcolor & 1) ? 170 : 0) + ((_pcolor & 8) ? 85 : 0);
            }
            palette[_pattr].r = _r;
            palette[_pattr].g = _g;
            palette[_pattr].b = _b;
            palette[_pattr].a = 255;
        }
    }
    {
        int _pattr = (int)(10.000000);
        int _pcolor = (int)(4521796.000000);
        if (_pattr >= 0 && _pattr < 16) {
            // Extract RGB from packed color value (QBasic format: &HRRGGBB or 6-bit EGA)
            int _r, _g, _b;
            if (_pcolor > 63) {
                // Treat as packed RGB (&HRRGGBB)
                _r = (_pcolor >> 16) & 0xFF;
                _g = (_pcolor >> 8) & 0xFF;
                _b = _pcolor & 0xFF;
            } else {
                // 6-bit EGA color: RGBRGB (2 bits each, high/low intensity)
                _r = ((_pcolor & 4) ? 170 : 0) + ((_pcolor & 32) ? 85 : 0);
                _g = ((_pcolor & 2) ? 170 : 0) + ((_pcolor & 16) ? 85 : 0);
                _b = ((_pcolor & 1) ? 170 : 0) + ((_pcolor & 8) ? 85 : 0);
            }
            palette[_pattr].r = _r;
            palette[_pattr].g = _g;
            palette[_pattr].b = _b;
            palette[_pattr].a = 255;
        }
    }
    {
        int _pattr = (int)(11.000000);
        int _pcolor = (int)(16755200.000000);
        if (_pattr >= 0 && _pattr < 16) {
            // Extract RGB from packed color value (QBasic format: &HRRGGBB or 6-bit EGA)
            int _r, _g, _b;
            if (_pcolor > 63) {
                // Treat as packed RGB (&HRRGGBB)
                _r = (_pcolor >> 16) & 0xFF;
                _g = (_pcolor >> 8) & 0xFF;
                _b = _pcolor & 0xFF;
            } else {
                // 6-bit EGA color: RGBRGB (2 bits each, high/low intensity)
                _r = ((_pcolor & 4) ? 170 : 0) + ((_pcolor & 32) ? 85 : 0);
                _g = ((_pcolor & 2) ? 170 : 0) + ((_pcolor & 16) ? 85 : 0);
                _b = ((_pcolor & 1) ? 170 : 0) + ((_pcolor & 8) ? 85 : 0);
            }
            palette[_pattr].r = _r;
            palette[_pattr].g = _g;
            palette[_pattr].b = _b;
            palette[_pattr].a = 255;
        }
    }
    {
        int _pattr = (int)(12.000000);
        int _pcolor = (int)(2263074.000000);
        if (_pattr >= 0 && _pattr < 16) {
            // Extract RGB from packed color value (QBasic format: &HRRGGBB or 6-bit EGA)
            int _r, _g, _b;
            if (_pcolor > 63) {
                // Treat as packed RGB (&HRRGGBB)
                _r = (_pcolor >> 16) & 0xFF;
                _g = (_pcolor >> 8) & 0xFF;
                _b = _pcolor & 0xFF;
            } else {
                // 6-bit EGA color: RGBRGB (2 bits each, high/low intensity)
                _r = ((_pcolor & 4) ? 170 : 0) + ((_pcolor & 32) ? 85 : 0);
                _g = ((_pcolor & 2) ? 170 : 0) + ((_pcolor & 16) ? 85 : 0);
                _b = ((_pcolor & 1) ? 170 : 0) + ((_pcolor & 8) ? 85 : 0);
            }
            palette[_pattr].r = _r;
            palette[_pattr].g = _g;
            palette[_pattr].b = _b;
            palette[_pattr].a = 255;
        }
    }
    {
        int _pattr = (int)(13.000000);
        int _pcolor = (int)(6693512.000000);
        if (_pattr >= 0 && _pattr < 16) {
            // Extract RGB from packed color value (QBasic format: &HRRGGBB or 6-bit EGA)
            int _r, _g, _b;
            if (_pcolor > 63) {
                // Treat as packed RGB (&HRRGGBB)
                _r = (_pcolor >> 16) & 0xFF;
                _g = (_pcolor >> 8) & 0xFF;
                _b = _pcolor & 0xFF;
            } else {
                // 6-bit EGA color: RGBRGB (2 bits each, high/low intensity)
                _r = ((_pcolor & 4) ? 170 : 0) + ((_pcolor & 32) ? 85 : 0);
                _g = ((_pcolor & 2) ? 170 : 0) + ((_pcolor & 16) ? 85 : 0);
                _b = ((_pcolor & 1) ? 170 : 0) + ((_pcolor & 8) ? 85 : 0);
            }
            palette[_pattr].r = _r;
            palette[_pattr].g = _g;
            palette[_pattr].b = _b;
            palette[_pattr].a = 255;
        }
    }
    {
        int _pattr = (int)(14.000000);
        int _pcolor = (int)(16737928.000000);
        if (_pattr >= 0 && _pattr < 16) {
            // Extract RGB from packed color value (QBasic format: &HRRGGBB or 6-bit EGA)
            int _r, _g, _b;
            if (_pcolor > 63) {
                // Treat as packed RGB (&HRRGGBB)
                _r = (_pcolor >> 16) & 0xFF;
                _g = (_pcolor >> 8) & 0xFF;
                _b = _pcolor & 0xFF;
            } else {
                // 6-bit EGA color: RGBRGB (2 bits each, high/low intensity)
                _r = ((_pcolor & 4) ? 170 : 0) + ((_pcolor & 32) ? 85 : 0);
                _g = ((_pcolor & 2) ? 170 : 0) + ((_pcolor & 16) ? 85 : 0);
                _b = ((_pcolor & 1) ? 170 : 0) + ((_pcolor & 8) ? 85 : 0);
            }
            palette[_pattr].r = _r;
            palette[_pattr].g = _g;
            palette[_pattr].b = _b;
            palette[_pattr].a = 255;
        }
    }
    {
        int _pattr = (int)(15.000000);
        int _pcolor = (int)(13421772.000000);
        if (_pattr >= 0 && _pattr < 16) {
            // Extract RGB from packed color value (QBasic format: &HRRGGBB or 6-bit EGA)
            int _r, _g, _b;
            if (_pcolor > 63) {
                // Treat as packed RGB (&HRRGGBB)
                _r = (_pcolor >> 16) & 0xFF;
                _g = (_pcolor >> 8) & 0xFF;
                _b = _pcolor & 0xFF;
            } else {
                // 6-bit EGA color: RGBRGB (2 bits each, high/low intensity)
                _r = ((_pcolor & 4) ? 170 : 0) + ((_pcolor & 32) ? 85 : 0);
                _g = ((_pcolor & 2) ? 170 : 0) + ((_pcolor & 16) ? 85 : 0);
                _b = ((_pcolor & 1) ? 170 : 0) + ((_pcolor & 8) ? 85 : 0);
            }
            palette[_pattr].r = _r;
            palette[_pattr].g = _g;
            palette[_pattr].b = _b;
            palette[_pattr].a = 255;
        }
    }
    // 
    GFX_COLS = 100.000000;
    GFX_ROWS = 75.000000;
    GFX_CHARW = 8.000000;
    GFX_CHARH = 8.000000;
    GFX_ROW = 0.000000;
    GFX_COL = 0.000000;
    GFX_COLOR = 1.000000;
    GFX_BGCOLOR = 0.000000;
    // 
    strcpy(DATA_PATH_str, "data/");
    strcpy(SOUND_PATH_str, "sound/");
    strcpy(IMG_PATH_str, "imgs/");
    // 
    strcpy(PUNCTUATION_str, "?!,.:;<>(){}[]");
    // 
    TIMEPASS = 0.000000;
    MEETINGCOUNTER = 0.000000;
    COUNTER = 0.000000;
    SONGCOUNTER = 0.000000;
    FINISHED = 0.000000;
    CURRENT_SOUND = 0.000000;
    NEWS_LOADED = 0.000000;
    // 
    CP_ROW = 0.000000;
    SOUND_WAIT = 0.000000;
    CURRENT_PID = 0.000000;
    REPLY_SWITCH = 0.000000;
    DATES_MONTHS = 0.000000;
    strcpy(DUMMY_str, "");
    // 
    GC_ROW = 0.000000;
    GC_CLR = 0.000000;
    GL_ROW = 0.000000;
    GL_COL = 0.000000;
    // 
    MAX_KEYWORDS = 100.000000;
    MAX_REPLIES = 200.000000;
    MAX_WORDS = 50.000000;
    // 
    MAX_LINES = 500.000000;
        {
        int _size = 1;        
        _size *= ((int)MAX_LINES + 1);        
                NEWSBLA_str = (char (*)[RB_STR_SIZE])calloc(_size, RB_STR_SIZE);
                if (!NEWSBLA_str) {
                    fprintf(stderr, "Out of memory allocating array NEWSBLA$\n");
                    exit(1);
                }
        }
    NEWSBLA_COUNT = 0.000000;
        {
        int _size = 1;        
        _size *= ((int)MAX_LINES + 1);        
                PLACES_str = (char (*)[RB_STR_SIZE])calloc(_size, RB_STR_SIZE);
                if (!PLACES_str) {
                    fprintf(stderr, "Out of memory allocating array PLACES$\n");
                    exit(1);
                }
        }
    PLACES_COUNT = 0.000000;
        {
        int _size = 1;        
        _size *= ((int)MAX_LINES + 1);        
                COLORS_ARR_str = (char (*)[RB_STR_SIZE])calloc(_size, RB_STR_SIZE);
                if (!COLORS_ARR_str) {
                    fprintf(stderr, "Out of memory allocating array COLORS_ARR$\n");
                    exit(1);
                }
        }
    COLORS_COUNT = 0.000000;
        {
        int _size = 1;        
        _size *= ((int)MAX_LINES + 1);        
                METAPHORE_str = (char (*)[RB_STR_SIZE])calloc(_size, RB_STR_SIZE);
                if (!METAPHORE_str) {
                    fprintf(stderr, "Out of memory allocating array METAPHORE$\n");
                    exit(1);
                }
        }
    METAPHORE_COUNT = 0.000000;
        {
        int _size = 1;        
        _size *= ((int)MAX_LINES + 1);        
                OBJECTS_str = (char (*)[RB_STR_SIZE])calloc(_size, RB_STR_SIZE);
                if (!OBJECTS_str) {
                    fprintf(stderr, "Out of memory allocating array OBJECTS$\n");
                    exit(1);
                }
        }
    OBJECTS_COUNT = 0.000000;
        {
        int _size = 1;        
        _size *= ((int)MAX_LINES + 1);        
                GOOD_ARR_str = (char (*)[RB_STR_SIZE])calloc(_size, RB_STR_SIZE);
                if (!GOOD_ARR_str) {
                    fprintf(stderr, "Out of memory allocating array GOOD_ARR$\n");
                    exit(1);
                }
        }
    GOOD_COUNT = 0.000000;
        {
        int _size = 1;        
        _size *= ((int)MAX_LINES + 1);        
                BAD_ARR_str = (char (*)[RB_STR_SIZE])calloc(_size, RB_STR_SIZE);
                if (!BAD_ARR_str) {
                    fprintf(stderr, "Out of memory allocating array BAD_ARR$\n");
                    exit(1);
                }
        }
    BAD_COUNT = 0.000000;
        {
        int _size = 1;        
        _size *= ((int)MAX_LINES + 1);        
                UPPER_LINES_str = (char (*)[RB_STR_SIZE])calloc(_size, RB_STR_SIZE);
                if (!UPPER_LINES_str) {
                    fprintf(stderr, "Out of memory allocating array UPPER_LINES$\n");
                    exit(1);
                }
        }
    UPPER_LINES_COUNT = 0.000000;
    // 
    strcpy(MONTH_NAMES_str[(int)(0.000000)], "January");
    strcpy(MONTH_NAMES_str[(int)(1.000000)], "February");
    strcpy(MONTH_NAMES_str[(int)(2.000000)], "March");
    strcpy(MONTH_NAMES_str[(int)(3.000000)], "April");
    strcpy(MONTH_NAMES_str[(int)(4.000000)], "May");
    strcpy(MONTH_NAMES_str[(int)(5.000000)], "June");
    strcpy(MONTH_NAMES_str[(int)(6.000000)], "July");
    strcpy(MONTH_NAMES_str[(int)(7.000000)], "August");
    strcpy(MONTH_NAMES_str[(int)(8.000000)], "September");
    strcpy(MONTH_NAMES_str[(int)(9.000000)], "October");
    strcpy(MONTH_NAMES_str[(int)(10.000000)], "November");
    strcpy(MONTH_NAMES_str[(int)(11.000000)], "December");
    // 
    strcpy(DREAM_FILES_str[(int)(0.000000)], "dream.txt");
    strcpy(DREAM_FILES_str[(int)(1.000000)], "dream2.txt");
    strcpy(DREAM_FILES_str[(int)(2.000000)], "dream3.txt");
    strcpy(DREAM_FILES_str[(int)(3.000000)], "nightmare1.txt");
    strcpy(DREAM_FILES_str[(int)(4.000000)], "eddie.txt");
    strcpy(DREAM_FILES_str[(int)(5.000000)], "dream_ta.txt");
    strcpy(DREAM_FILES_str[(int)(6.000000)], "johnny.txt");
    strcpy(DREAM_MUSIC_str[(int)(0.000000)], "dream1.wav");
    strcpy(DREAM_MUSIC_str[(int)(1.000000)], "dream2.wav");
    strcpy(DREAM_MUSIC_str[(int)(2.000000)], "walk1.wav");
    strcpy(DREAM_MUSIC_str[(int)(3.000000)], "eddie.wav");
    strcpy(DREAM_MUSIC_str[(int)(4.000000)], "dream_eva.wav");
    strcpy(MEETING_FILES_str[(int)(0.000000)], "meeting1.txt");
    strcpy(MEETING_FILES_str[(int)(1.000000)], "meeting2.txt");
    strcpy(MEETING_FILES_str[(int)(2.000000)], "meeting3.txt");
    strcpy(MEETING_FILES_str[(int)(3.000000)], "meeting4.txt");
    // 
    MAX_CHAT = 60.000000;
        {
        int _size = 1;        
        _size *= ((int)MAX_CHAT + 1);        
                CHAT_LINES_str = (char (*)[RB_STR_SIZE])calloc(_size, RB_STR_SIZE);
                if (!CHAT_LINES_str) {
                    fprintf(stderr, "Out of memory allocating array CHAT_LINES$\n");
                    exit(1);
                }
        }
        {
        int _size = 1;        
        _size *= ((int)MAX_CHAT + 1);        
                CHAT_COLORS = (double*)calloc(_size, sizeof(double));
                if (!CHAT_COLORS) {
                    fprintf(stderr, "Out of memory allocating array CHAT_COLORS\n");
                    exit(1);
                }
        }
    CHAT_COUNT = 0.000000;
    // 
    RB_COLOR((int)(1.000000), 0);
    // 
    SUB_WARNINGSCREEN();
    SUB_OPENING();
    SUB_GAMEMAIN();
    strcpy(UPPER_FILE_str, "ending_titles.txt");
    SUB_UPPER();
    wait_for_close();  // Wait for user to close window or press ESC
    cleanup_graphics();
    exit(0);
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    
    return 0;
}
