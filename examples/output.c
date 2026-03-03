// RB_COMPILE_MODE: graphics
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#ifdef _WIN32
#include <windows.h>
#endif

#define RB_STR_SIZE 4096

// Numeric variables
double SND = 0.0;

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
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (WORD)((bg << 4) | fg));
    fflush(stdout);  // Ensure color is applied
}
#else
void RB_CLS() { printf("\033[2J\033[H"); }
void RB_LOCATE(int row, int col) { printf("\033[%d;%dH", row, col); }
void RB_COLOR(int fg, int bg) { printf("\033[%d;%dm", 30 + fg, 40 + bg); }
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
        default: screen_width = 640; screen_height = 480; break;
    }
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL init failed: %s\n", SDL_GetError());
        exit(1);
    }
    
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
    SDL_Surface* surf = SDL_LoadBMP(filename);
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
    init_graphics((int)(12.000000));
    // 
    SND = (double)rb_sndopen("test.wav");
    if ((SND > 0.000000)) {
        { // _PRINTSTRING
            rb_printstring((int)(10.000000), (int)(10.000000), "Sound loaded!");
            if (_auto_display) SDL_RenderPresent(renderer);
        }
        { // _SNDVOL
            int _sh = (int)(SND);
            int _vol = (int)(0.500000 * 128);
            if (_sh > 0 && _sh < MAX_SOUNDS && _sounds[_sh]) {
                Mix_VolumeChunk(_sounds[_sh], _vol < 0 ? 0 : _vol > 128 ? 128 : _vol);
            }
        }
        { // _SNDPLAY
            int _sh = (int)(SND);
            if (_sh > 0 && _sh < MAX_SOUNDS && _sounds[_sh]) {
                _sound_channels[_sh] = Mix_PlayChannel(-1, _sounds[_sh], 0);
            }
        }
        #ifdef _WIN32
        Sleep((DWORD)(2.000000 * 1000));
        #else
        usleep((unsigned int)(2.000000 * 1000000));
        #endif
        { // _SNDSTOP
            int _sh = (int)(SND);
            if (_sh > 0 && _sh < MAX_SOUNDS && _sound_channels[_sh] >= 0) {
                Mix_HaltChannel(_sound_channels[_sh]);
            }
        }
    } else {
        { // _PRINTSTRING
            rb_printstring((int)(10.000000), (int)(10.000000), "No sound file (OK - testing compilation)");
            if (_auto_display) SDL_RenderPresent(renderer);
        }
    }
    { // _PRINTSTRING
        rb_printstring((int)(10.000000), (int)(50.000000), "Sound test complete");
        if (_auto_display) SDL_RenderPresent(renderer);
    }
    SDL_RenderPresent(renderer);
    #ifdef _WIN32
    Sleep((DWORD)(2.000000 * 1000));
    #else
    usleep((unsigned int)(2.000000 * 1000000));
    #endif
    wait_for_close();  // Wait for user to close window or press ESC
    cleanup_graphics();
    exit(0);
    
    return 0;
}
