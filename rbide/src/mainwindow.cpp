// mainwindow.cpp - Main Window Implementation
// RB BASIC (Ronen Blumberg Basic) IDE Main Window with QBASIC-style syntax highlighting
// Copyright (c) 2025 Ronen Blumberg

#include "mainwindow.h"
#include "resource.h"
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shlwapi.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <set>

// These libraries are linked via build.bat command line instead
// (MinGW doesn't support #pragma comment)

// Scintilla message definitions
#define SCI_SETLEXER 4001
#define SCI_STYLESETFORE 2051
#define SCI_STYLESETBACK 2052
#define SCI_STYLESETBOLD 2053
#define SCI_STYLESETSIZE 2055
#define SCI_STYLESETFONT 2056
#define SCI_SETTEXT 2181
#define SCI_GETTEXT 2182
#define SCI_GETTEXTLENGTH 2183
#define SCI_SETMARGINWIDTHN 2242
#define SCI_SETMARGINTYPEN 2240
#define SCI_SETSELFORE 2067
#define SCI_SETSELBACK 2068
#define SCI_SETCARETFORE 2069
#define SCI_STYLECLEARALL 2050
#define SCI_SETKEYWORDS 4005
#define SCI_SETPROPERTY 4004
#define SCI_GETMODIFY 2159
#define SCI_SETSAVEPOINT 2014
#define SCI_UNDO 2176
#define SCI_REDO 2011
#define SCI_CANUNDO 2174
#define SCI_CANREDO 2016
#define SCI_CUT 2177
#define SCI_COPY 2178
#define SCI_PASTE 2179
#define SCI_SELECTALL 2013
#define SCI_EMPTYUNDOBUFFER 2175
#define SCI_GETDIRECTFUNCTION 2184
#define SCI_GETDIRECTPOINTER 2185
#define SCI_SETSEL 2160
#define SCI_GETCURRENTPOS 2008
#define SCI_LINEFROMPOSITION 2166
#define SCI_POSITIONFROMLINE 2167
#define SCI_GETLINEENDPOSITION 2136
#define SCI_INSERTTEXT 2003
#define SCI_GETLINE 2153

#define SCLEX_NULL 0
#define SCLEX_FREEBASIC 75

#define SC_MARGIN_NUMBER 0
#define STYLE_DEFAULT 32
#define STYLE_LINENUMBER 33

// QBASIC color scheme (retro 1980s/90s style)
#define COLOR_QBASIC_BACKGROUND RGB(0, 0, 170)      // Classic blue background
#define COLOR_QBASIC_TEXT RGB(255, 255, 85)         // Bright yellow text
#define COLOR_QBASIC_KEYWORD RGB(85, 255, 255)      // Cyan keywords
#define COLOR_QBASIC_STRING RGB(255, 85, 255)       // Magenta strings
#define COLOR_QBASIC_COMMENT RGB(85, 255, 85)       // Bright green comments
#define COLOR_QBASIC_NUMBER RGB(255, 255, 255)      // White numbers
#define COLOR_QBASIC_OPERATOR RGB(255, 255, 255)    // White operators
#define COLOR_QBASIC_LINENUMBER RGB(170, 170, 170)  // Gray line numbers
#define COLOR_QBASIC_SELECTION RGB(255, 255, 255)   // White selection
#define COLOR_QBASIC_SELBACK RGB(0, 0, 128)         // Dark blue selection background

// GWBASIC/QBASIC keywords
const char* BASIC_KEYWORDS = 
    "ABS ACCESS ALIAS AND ANY APPEND AS ASC ATN "
    "BASE BEEP BINARY BLOAD BSAVE "
    "CALL CALLS CASE CDBL CDECL CHAIN CHDIR CHR$ CINT CIRCLE CLEAR CLNG CLOSE CLS "
    "COLOR COM COMMAND$ COMMON CONST COS CSNG CSRLIN CVD CVDMBF CVI CVL CVS CVSMBF "
    "DATA DATE$ DECLARE DEF DEFDBL DEFINT DEFLNG DEFSNG DEFSTR DELETE DIM DO DOUBLE DRAW "
    "ELSE ELSEIF END ENVIRON ENVIRON$ EOF EQV ERASE ERDEV ERDEV$ ERL ERR ERROR EXIT EXP "
    "FIELD FILES FIX FOR FRE FREEFILE FUNCTION "
    "GET GOSUB GOTO "
    "HEX$ "
    "IF IMP INKEY$ INP INPUT INPUT$ INSTR INT INTEGER IOCTL IOCTL$ IS "
    "KEY KILL "
    "LBOUND LCASE$ LEFT$ LEN LET LINE LIST LOC LOCAL LOCATE LOCK LOF LOG LONG LOOP LPOS LPRINT LSET LTRIM$ "
    "MID$ MKD$ MKDMBF$ MKI$ MKL$ MKS$ MKSMBF$ MOD "
    "NAME NEXT NOT "
    "OCT$ OFF ON OPEN OPTION OR OUT OUTPUT "
    "PAINT PALETTE PCOPY PEEK PEN PLAY PMAP POINT POKE POS PRESET PRINT PSET PUT "
    "RANDOM RANDOMIZE READ REDIM REM RESET RESTORE RESUME RETURN RIGHT$ RMDIR RND RSET RTRIM$ RUN "
    "SADD SCREEN SEEK SEG SELECT SETMEM SGN SHARED SHELL SIN SINGLE SLEEP SOUND SPACE$ SPC SQR "
    "STATIC STEP STICK STOP STR$ STRING STRING$ SUB SWAP SYSTEM "
    "TAB TAN THEN TIME$ TIMER TO TROFF TRON TYPE "
    "UBOUND UCASE$ UEVENT UNLOCK UNTIL USING "
    "VAL VARPTR VARPTR$ VARSEG VIEW "
    "WAIT WEND WHILE WIDTH WINDOW WRITE "
    "XOR";

extern HINSTANCE g_hInstance;

// Constructor
MainWindow::MainWindow() 
    : m_hwnd(NULL)
    , m_hwndEdit(NULL)
    , m_hwndOutput(NULL)
    , m_hwndStatus(NULL)
    , m_hwndSplitter(NULL)
    , m_hInstance(NULL)
    , m_hScintilla(NULL)
    , m_hAccel(NULL)
    , m_modified(false)
    , m_outputVisible(true)
    , m_splitterPos(150)
    , m_pScintilla(0)
    , m_fnScintilla(NULL)
{
}

// Destructor
MainWindow::~MainWindow() {
    if (m_hScintilla) {
        FreeLibrary(m_hScintilla);
    }
}

// Window procedure (static)
LRESULT CALLBACK MainWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    MainWindow* pThis = NULL;
    
    if (msg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (MainWindow*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
        pThis->m_hwnd = hwnd;
    } else {
        pThis = (MainWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }
    
    if (pThis) {
        return pThis->HandleMessage(msg, wParam, lParam);
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Message handler
LRESULT MainWindow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            CreateMenus();
            if (!InitScintilla()) {
                return -1;
            }
            CreateControls();
            SetupEditor();
            InitAccelerators();
            return 0;
            
        case WM_SIZE:
            ResizeControls();
            return 0;
            
        case WM_COMMAND:
            return HandleCommand(wParam, lParam);
            
        case WM_CLOSE:
            if (PromptSave()) {
                DestroyWindow(m_hwnd);
            }
            return 0;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    
    return DefWindowProc(m_hwnd, msg, wParam, lParam);
}

// Command handler
LRESULT MainWindow::HandleCommand(WPARAM wParam, LPARAM lParam) {
    switch (LOWORD(wParam)) {
        // File menu
        case IDM_FILE_NEW:
            FileNew();
            return 0;
        case IDM_FILE_OPEN:
            FileOpen();
            return 0;
        case IDM_FILE_SAVE:
            FileSave();
            return 0;
        case IDM_FILE_SAVEAS:
            FileSaveAs();
            return 0;
        case IDM_FILE_EXIT:
            SendMessage(m_hwnd, WM_CLOSE, 0, 0);
            return 0;
            
        // Edit menu
        case IDM_EDIT_UNDO:
            EditUndo();
            return 0;
        case IDM_EDIT_REDO:
            EditRedo();
            return 0;
        case IDM_EDIT_CUT:
            EditCut();
            return 0;
        case IDM_EDIT_COPY:
            EditCopy();
            return 0;
        case IDM_EDIT_PASTE:
            EditPaste();
            return 0;
        case IDM_EDIT_SELECTALL:
            EditSelectAll();
            return 0;
        case IDM_EDIT_FIND:
            EditFind();
            return 0;
        case IDM_EDIT_ADDLINE:
            AddLineNumber();
            return 0;
        case IDM_EDIT_RENUMBER:
            RenumberLines();
            return 0;
        case IDM_EDIT_REMOVENUMBERS:
            RemoveLineNumbers();
            return 0;
            
        // Run menu
        case IDM_RUN_COMPILE:
            RunCompile();
            return 0;
        case IDM_RUN_RUN:
            RunProgram();
            return 0;
        case IDM_RUN_COMPILERUN:
            RunCompileAndRun();
            return 0;
        case IDM_RUN_STOP:
            StopProgram();
            return 0;
            
        // Help menu
        case IDM_HELP_ABOUT:
            MessageBox(m_hwnd, 
                "RB BASIC IDE v1.0\n\n"
                "A retro-styled BASIC development environment\n"
                "with RB Basic syntax highlighting\n\n"
                "Copyright (c) 2025 Ronen Blumberg",
                "About RB BASIC IDE", 
                MB_ICONINFORMATION);
            return 0;
        case IDM_HELP_MANUAL:
            MessageBox(m_hwnd,
                "RB BASIC IDE Quick Help\n\n"
                "File Menu:\n"
                "  Ctrl+N - New file\n"
                "  Ctrl+O - Open file\n"
                "  Ctrl+S - Save file\n\n"
                "Edit Menu:\n"
                "  Ctrl+L - Add Line Number\n"
                "  Ctrl+R - Renumber Lines\n"
                "  Ctrl+Shift+R - Remove Line Numbers\n"
                "  Ctrl+Z/Y/X/C/V/A - Undo/Redo/Cut/Copy/Paste/SelectAll\n\n"
                "Run Menu:\n"
                "  F7 - Compile (generates output.c and compile.bat)\n"
                "  F9 - Run (executes program.exe)\n"
                "  F5 - Compile & Run (both steps)\n\n"
                "Process:\n"
                "  rbbasic.exe transpiles .BAS to output.c\n"
                "  compile.bat compiles output.c to program.exe\n"
                "  program.exe runs your BASIC program",
                "RB BASIC IDE Help",
                MB_ICONINFORMATION);
            return 0;
    }
    
    return 0;
}

// Initialize accelerators
void MainWindow::InitAccelerators() {
    ACCEL accel[10];
    
    // Ctrl+N - New
    accel[0].fVirt = FCONTROL | FVIRTKEY;
    accel[0].key = 'N';
    accel[0].cmd = IDM_FILE_NEW;
    
    // Ctrl+O - Open
    accel[1].fVirt = FCONTROL | FVIRTKEY;
    accel[1].key = 'O';
    accel[1].cmd = IDM_FILE_OPEN;
    
    // Ctrl+S - Save
    accel[2].fVirt = FCONTROL | FVIRTKEY;
    accel[2].key = 'S';
    accel[2].cmd = IDM_FILE_SAVE;
    
    // F5 - Compile & Run
    accel[3].fVirt = FVIRTKEY;
    accel[3].key = VK_F5;
    accel[3].cmd = IDM_RUN_COMPILERUN;
    
    // F7 - Compile
    accel[4].fVirt = FVIRTKEY;
    accel[4].key = VK_F7;
    accel[4].cmd = IDM_RUN_COMPILE;
    
    // F9 - Run
    accel[5].fVirt = FVIRTKEY;
    accel[5].key = VK_F9;
    accel[5].cmd = IDM_RUN_RUN;
    
    // Ctrl+L - Add Line Number
    accel[6].fVirt = FCONTROL | FVIRTKEY;
    accel[6].key = 'L';
    accel[6].cmd = IDM_EDIT_ADDLINE;
    
    // Ctrl+R - Renumber Lines
    accel[7].fVirt = FCONTROL | FVIRTKEY;
    accel[7].key = 'R';
    accel[7].cmd = IDM_EDIT_RENUMBER;
    
    // Ctrl+Shift+R - Remove Line Numbers
    accel[8].fVirt = FCONTROL | FSHIFT | FVIRTKEY;
    accel[8].key = 'R';
    accel[8].cmd = IDM_EDIT_REMOVENUMBERS;
    
    // Ctrl+F - Find
    accel[9].fVirt = FCONTROL | FVIRTKEY;
    accel[9].key = 'F';
    accel[9].cmd = IDM_EDIT_FIND;
    
    m_hAccel = CreateAcceleratorTable(accel, 10);
}

// Create main window
bool MainWindow::Create(HINSTANCE hInstance) {
    m_hInstance = hInstance;
    
    // Register window class
    WNDCLASSEX wcex = {0};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wcex.lpszClassName = "RBIDEMainWindow";
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    
    if (!RegisterClassEx(&wcex)) {
        return false;
    }
    
    // Create window
    m_hwnd = CreateWindowEx(
        0,
        "RBIDEMainWindow",
        "RB BASIC IDE",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1024, 768,
        NULL, NULL,
        hInstance,
        this
    );
    
    return (m_hwnd != NULL);
}

// Show window
void MainWindow::Show(int nCmdShow) {
    ShowWindow(m_hwnd, nCmdShow);
    UpdateWindow(m_hwnd);
    UpdateTitle();
}

// Initialize Scintilla
bool MainWindow::InitScintilla() {
    // Load Scintilla DLL
    m_hScintilla = LoadLibrary("SciLexer.dll");
    if (!m_hScintilla) {
        MessageBox(m_hwnd, 
            "Failed to load SciLexer.dll!\n\n"
            "Please ensure SciLexer.dll is in the same directory as the executable.",
            "Error", MB_ICONERROR);
        return false;
    }
    return true;
}

// Create menus
void MainWindow::CreateMenus() {
    HMENU hMenuBar = CreateMenu();
    
    // File menu
    HMENU hFileMenu = CreatePopupMenu();
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_NEW, "&New\tCtrl+N");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_OPEN, "&Open...\tCtrl+O");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_SAVE, "&Save\tCtrl+S");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_SAVEAS, "Save &As...");
    AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_EXIT, "E&xit");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, "&File");
    
    // Edit menu
    HMENU hEditMenu = CreatePopupMenu();
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_UNDO, "&Undo\tCtrl+Z");
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_REDO, "&Redo\tCtrl+Y");
    AppendMenu(hEditMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_CUT, "Cu&t\tCtrl+X");
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_COPY, "&Copy\tCtrl+C");
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_PASTE, "&Paste\tCtrl+V");
    AppendMenu(hEditMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_SELECTALL, "Select &All\tCtrl+A");
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_FIND, "&Find...\tCtrl+F");
    AppendMenu(hEditMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_ADDLINE, "Add &Line Number\tCtrl+L");
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_RENUMBER, "Re&number Lines\tCtrl+R");
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_REMOVENUMBERS, "Remove Line &Numbers\tCtrl+Shift+R");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hEditMenu, "&Edit");
    
    // Run menu
    HMENU hRunMenu = CreatePopupMenu();
    AppendMenu(hRunMenu, MF_STRING, IDM_RUN_COMPILE, "&Compile\tF7");
    AppendMenu(hRunMenu, MF_STRING, IDM_RUN_RUN, "&Run\tF9");
    AppendMenu(hRunMenu, MF_STRING, IDM_RUN_COMPILERUN, "Compile && R&un\tF5");
    AppendMenu(hRunMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hRunMenu, MF_STRING, IDM_RUN_STOP, "&Stop");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hRunMenu, "&Run");
    
    // Help menu
    HMENU hHelpMenu = CreatePopupMenu();
    AppendMenu(hHelpMenu, MF_STRING, IDM_HELP_MANUAL, "&Manual");
    AppendMenu(hHelpMenu, MF_STRING, IDM_HELP_ABOUT, "&About");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hHelpMenu, "&Help");
    
    SetMenu(m_hwnd, hMenuBar);
}

// Create controls
void MainWindow::CreateControls() {
    // Create Scintilla editor
    m_hwndEdit = CreateWindowEx(
        0,
        "Scintilla",
        "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
        0, 0, 0, 0,
        m_hwnd,
        (HMENU)IDC_EDITOR,
        m_hInstance,
        NULL
    );
    
    // Get Scintilla direct function
    m_fnScintilla = (sptr_t (*)(sptr_t, int, uptr_t, sptr_t))
        SendMessage(m_hwndEdit, SCI_GETDIRECTFUNCTION, 0, 0);
    m_pScintilla = SendMessage(m_hwndEdit, SCI_GETDIRECTPOINTER, 0, 0);
    
    // Create output panel (read-only edit control)
    m_hwndOutput = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "EDIT",
        "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
        0, 0, 0, 0,
        m_hwnd,
        (HMENU)IDC_OUTPUT,
        m_hInstance,
        NULL
    );
    
    // Set output panel font
    HFONT hFont = CreateFont(
        16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "Consolas"
    );
    SendMessage(m_hwndOutput, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // Create status bar
    m_hwndStatus = CreateWindowEx(
        0,
        STATUSCLASSNAME,
        NULL,
        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
        0, 0, 0, 0,
        m_hwnd,
        (HMENU)IDC_STATUS,
        m_hInstance,
        NULL
    );
}

// Setup editor with QBASIC color scheme
void MainWindow::SetupEditor() {
    // Clear all styles first
    SendEditor(SCI_STYLECLEARALL);
    
    // Set default style (QBASIC blue background, yellow text)
    SendEditor(SCI_STYLESETFONT, STYLE_DEFAULT, (sptr_t)"Courier New");
    SendEditor(SCI_STYLESETSIZE, STYLE_DEFAULT, 11);
    SendEditor(SCI_STYLESETBACK, STYLE_DEFAULT, COLOR_QBASIC_BACKGROUND);
    SendEditor(SCI_STYLESETFORE, STYLE_DEFAULT, COLOR_QBASIC_TEXT);
    
    // Apply default style to all
    SendEditor(SCI_STYLECLEARALL);
    
    // Setup syntax highlighting
    SetupSyntaxHighlighting();
    
    // Line numbers margin
    SendEditor(SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER);
    SendEditor(SCI_SETMARGINWIDTHN, 0, 40);
    SendEditor(SCI_STYLESETBACK, STYLE_LINENUMBER, COLOR_QBASIC_BACKGROUND);
    SendEditor(SCI_STYLESETFORE, STYLE_LINENUMBER, COLOR_QBASIC_LINENUMBER);
    
    // Selection colors
    SendEditor(SCI_SETSELFORE, 1, COLOR_QBASIC_SELECTION);
    SendEditor(SCI_SETSELBACK, 1, COLOR_QBASIC_SELBACK);
    
    // Caret color (cursor)
    SendEditor(SCI_SETCARETFORE, RGB(255, 255, 85), 0);
    
    // Initial text
    SetEditorText("REM RB BASIC - Ronen Blumberg BASIC\nREM A retro RB Basic language\n\n10 PRINT \"Hello, World!\"\n20 END\n");
    
    SendEditor(SCI_SETSAVEPOINT);
    m_modified = false;
}

// Setup syntax highlighting for BASIC
void MainWindow::SetupSyntaxHighlighting() {
    // Use FreeBasic lexer (closest to GWBASIC/QBASIC)
    SendEditor(SCI_SETLEXER, SCLEX_FREEBASIC, 0);
    
    // Note: FreeBasic lexer treats both REM and ' as comments
    // REM is the primary comment style in RB BASIC
    // The lexer will highlight both, but users should use REM
    
    // Set keywords (REM is included as a keyword)
    SendEditor(SCI_SETKEYWORDS, 0, (sptr_t)BASIC_KEYWORDS);
    
    // Style 0: Default (already set)
    // Style 1: Comment - Bright green (REM statements)
    SendEditor(SCI_STYLESETFORE, 1, COLOR_QBASIC_COMMENT);
    SendEditor(SCI_STYLESETBACK, 1, COLOR_QBASIC_BACKGROUND);
    
    // Style 2: Number - White
    SendEditor(SCI_STYLESETFORE, 2, COLOR_QBASIC_NUMBER);
    SendEditor(SCI_STYLESETBACK, 2, COLOR_QBASIC_BACKGROUND);
    
    // Style 3: Keyword - Cyan
    SendEditor(SCI_STYLESETFORE, 3, COLOR_QBASIC_KEYWORD);
    SendEditor(SCI_STYLESETBACK, 3, COLOR_QBASIC_BACKGROUND);
    SendEditor(SCI_STYLESETBOLD, 3, 1);
    
    // Style 4: String - Magenta
    SendEditor(SCI_STYLESETFORE, 4, COLOR_QBASIC_STRING);
    SendEditor(SCI_STYLESETBACK, 4, COLOR_QBASIC_BACKGROUND);
    
    // Style 5: Preprocessor - Keep as keyword color
    SendEditor(SCI_STYLESETFORE, 5, COLOR_QBASIC_KEYWORD);
    SendEditor(SCI_STYLESETBACK, 5, COLOR_QBASIC_BACKGROUND);
    
    // Style 6: Operator - White
    SendEditor(SCI_STYLESETFORE, 6, COLOR_QBASIC_OPERATOR);
    SendEditor(SCI_STYLESETBACK, 6, COLOR_QBASIC_BACKGROUND);
    
    // Style 7: Identifier - Yellow (default text)
    SendEditor(SCI_STYLESETFORE, 7, COLOR_QBASIC_TEXT);
    SendEditor(SCI_STYLESETBACK, 7, COLOR_QBASIC_BACKGROUND);
    
    // Additional styles for completeness
    for (int i = 8; i < 20; i++) {
        SendEditor(SCI_STYLESETBACK, i, COLOR_QBASIC_BACKGROUND);
        SendEditor(SCI_STYLESETFORE, i, COLOR_QBASIC_TEXT);
    }
}

// Resize controls
void MainWindow::ResizeControls() {
    RECT rcClient;
    GetClientRect(m_hwnd, &rcClient);
    
    int statusHeight = 0;
    if (m_hwndStatus) {
        RECT rcStatus;
        GetWindowRect(m_hwndStatus, &rcStatus);
        statusHeight = rcStatus.bottom - rcStatus.top;
        MoveWindow(m_hwndStatus, 0, rcClient.bottom - statusHeight, 
                   rcClient.right, statusHeight, TRUE);
    }
    
    int availHeight = rcClient.bottom - statusHeight;
    
    if (m_outputVisible) {
        // Editor takes top portion, output takes bottom
        int editorHeight = availHeight - m_splitterPos;
        
        MoveWindow(m_hwndEdit, 0, 0, rcClient.right, editorHeight, TRUE);
        MoveWindow(m_hwndOutput, 0, editorHeight, rcClient.right, m_splitterPos, TRUE);
    } else {
        // Editor takes full height
        MoveWindow(m_hwndEdit, 0, 0, rcClient.right, availHeight, TRUE);
        ShowWindow(m_hwndOutput, SW_HIDE);
    }
    
    UpdateStatusBar();
}

// Update title bar
void MainWindow::UpdateTitle() {
    std::string title = "RB BASIC IDE";
    if (!m_currentFile.empty()) {
        char filename[MAX_PATH];
        strcpy(filename, m_currentFile.c_str());
        PathStripPath(filename);
        title += " - ";
        title += filename;
    } else {
        title += " - Untitled";
    }
    if (m_modified) {
        title += " *";
    }
    SetWindowText(m_hwnd, title.c_str());
}

// Update status bar
void MainWindow::UpdateStatusBar() {
    char status[256];
    if (m_currentFile.empty()) {
        sprintf(status, "Ready | QBASIC Mode");
    } else {
        sprintf(status, "%s | QBASIC Mode", m_currentFile.c_str());
    }
    SendMessage(m_hwndStatus, SB_SETTEXT, 0, (LPARAM)status);
}

// File operations
void MainWindow::FileNew() {
    if (!PromptSave()) return;
    
    m_currentFile.clear();
    SetEditorText("");
    SendEditor(SCI_SETSAVEPOINT);
    m_modified = false;
    UpdateTitle();
    UpdateStatusBar();
}

void MainWindow::FileOpen() {
    if (!PromptSave()) return;
    
    char filename[MAX_PATH] = "";
    OPENFILENAME ofn = {0};
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = m_hwnd;
    ofn.lpstrFilter = "BASIC Files (*.bas)\0*.bas\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    ofn.lpstrDefExt = "bas";
    
    if (GetOpenFileName(&ofn)) {
        LoadFile(filename);
    }
}

bool MainWindow::LoadFile(const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        MessageBox(m_hwnd, "Failed to open file!", "Error", MB_ICONERROR);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    SetEditorText(content);
    m_currentFile = filename;
    SendEditor(SCI_SETSAVEPOINT);
    m_modified = false;
    UpdateTitle();
    UpdateStatusBar();
    
    return true;
}

bool MainWindow::FileSave() {
    if (m_currentFile.empty()) {
        return FileSaveAs();
    }
    
    std::string content = GetEditorText();
    std::ofstream file(m_currentFile.c_str(), std::ios::binary);
    if (!file.is_open()) {
        MessageBox(m_hwnd, "Failed to save file!", "Error", MB_ICONERROR);
        return false;
    }
    
    file.write(content.c_str(), content.length());
    file.close();
    
    SendEditor(SCI_SETSAVEPOINT);
    m_modified = false;
    UpdateTitle();
    
    return true;
}

bool MainWindow::FileSaveAs() {
    char filename[MAX_PATH] = "";
    if (!m_currentFile.empty()) {
        strcpy(filename, m_currentFile.c_str());
    }
    
    OPENFILENAME ofn = {0};
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = m_hwnd;
    ofn.lpstrFilter = "BASIC Files (*.bas)\0*.bas\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    ofn.lpstrDefExt = "bas";
    
    if (GetSaveFileName(&ofn)) {
        m_currentFile = filename;
        return FileSave();
    }
    
    return false;
}

bool MainWindow::PromptSave() {
    if (!IsModified()) return true;
    
    int result = MessageBox(m_hwnd,
        "Do you want to save changes?",
        "RB BASIC IDE",
        MB_YESNOCANCEL | MB_ICONQUESTION);
    
    if (result == IDYES) {
        return FileSave();
    } else if (result == IDNO) {
        return true;
    } else {
        return false;
    }
}

// Edit operations
void MainWindow::EditUndo() {
    SendEditor(SCI_UNDO);
}

void MainWindow::EditRedo() {
    SendEditor(SCI_REDO);
}

void MainWindow::EditCut() {
    SendEditor(SCI_CUT);
}

void MainWindow::EditCopy() {
    SendEditor(SCI_COPY);
}

void MainWindow::EditPaste() {
    SendEditor(SCI_PASTE);
}

void MainWindow::EditSelectAll() {
    SendEditor(SCI_SELECTALL);
}

void MainWindow::EditFind() {
    MessageBox(m_hwnd, "Find functionality coming soon!", "Find", MB_ICONINFORMATION);
}

// Compilation and running
void MainWindow::RunCompile() {
    ClearOutput();
    AppendOutput("=== Compiling RB BASIC (Ronen Blumberg Basic) ===\n\n");
    
    // Save current file if needed
    if (m_currentFile.empty()) {
        if (!FileSaveAs()) {
            AppendOutput("Error: File must be saved before compiling.\n");
            return;
        }
    } else if (IsModified()) {
        FileSave();
    }
    
    // Get the parent directory (rbbasic-portable root)
    char rbbasicRoot[MAX_PATH];
    GetModuleFileName(NULL, rbbasicRoot, MAX_PATH);
    PathRemoveFileSpec(rbbasicRoot);  // Remove rbide.exe
    PathRemoveFileSpec(rbbasicRoot);  // Remove rbide folder
    
    // Check for rbbasic.exe compiler in parent directory
    char compilerPath[MAX_PATH];
    sprintf(compilerPath, "%s\\rbbasic.exe", rbbasicRoot);
    
    if (GetFileAttributes(compilerPath) == INVALID_FILE_ATTRIBUTES) {
        AppendOutput("Error: rbbasic.exe not found in parent directory.\n");
        AppendOutput("Expected: ");
        AppendOutput(compilerPath);
        AppendOutput("\n");
        return;
    }
    
    // Get the directory of the source file
    char sourceDir[MAX_PATH];
    strcpy(sourceDir, m_currentFile.c_str());
    PathRemoveFileSpec(sourceDir);
    
    // Create temporary files for capturing output
    char tempOutFile[MAX_PATH];
    char tempErrFile[MAX_PATH];
    sprintf(tempOutFile, "%s\\rbide_stdout.tmp", sourceDir);
    sprintf(tempErrFile, "%s\\rbide_stderr.tmp", sourceDir);
    
    // Build command line: cmd /c "rbbasic.exe source.bas > stdout.tmp 2> stderr.tmp"
    char cmdLine[2048];
    sprintf(cmdLine, "cmd.exe /c \"\"%s\" \"%s\" > \"%s\" 2> \"%s\"\"", 
            compilerPath, m_currentFile.c_str(), tempOutFile, tempErrFile);
    
    AppendOutput("Step 1: Transpiling BASIC to C\n");
    AppendOutput("Compiler: ");
    AppendOutput(compilerPath);
    AppendOutput("\n");
    AppendOutput("Source: ");
    AppendOutput(m_currentFile.c_str());
    AppendOutput("\n\n");
    
    // Execute rbbasic.exe with output redirection
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    
    if (!CreateProcess(NULL, cmdLine, NULL, NULL, FALSE, 
                      CREATE_NO_WINDOW, NULL, sourceDir, &si, &pi)) {
        AppendOutput("Error: Could not run rbbasic.exe compiler.\n");
        return;
    }
    
    // Wait but process messages to keep IDE responsive
    while (WaitForSingleObject(pi.hProcess, 50) == WAIT_TIMEOUT) {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    // Read stdout output
    std::ifstream outFile(tempOutFile);
    if (outFile.is_open()) {
        std::string line;
        while (std::getline(outFile, line)) {
            AppendOutput(line.c_str());
            AppendOutput("\n");
        }
        outFile.close();
    }
    
    // Read stderr output (errors/warnings)
    std::ifstream errFile(tempErrFile);
    if (errFile.is_open()) {
        std::string line;
        bool hasErrors = false;
        while (std::getline(errFile, line)) {
            if (!hasErrors) {
                AppendOutput("\n*** PARSING ERRORS ***\n");
                hasErrors = true;
            }
            AppendOutput(line.c_str());
            AppendOutput("\n");
        }
        errFile.close();
    }
    
    // Clean up temp files
    DeleteFile(tempOutFile);
    DeleteFile(tempErrFile);
    
    if (exitCode != 0) {
        char msg[128];
        sprintf(msg, "\nRB BASIC transpiler failed with exit code: %lu\n", (unsigned long)exitCode);
        AppendOutput(msg);
        AppendOutput("Please fix the syntax errors above and try again.\n");
        return;
    }
    
    AppendOutput("\nTranspilation successful! Generated output.c and compile.bat\n\n");
    
    // Now run compile.bat to compile output.c to program.exe
    char compileBat[MAX_PATH];
    sprintf(compileBat, "%s\\compile.bat", sourceDir);
    
    if (GetFileAttributes(compileBat) == INVALID_FILE_ATTRIBUTES) {
        AppendOutput("Error: compile.bat not generated.\n");
        return;
    }
    
    AppendOutput("Step 2: Compiling C to executable\n");
    AppendOutput("Running: compile.bat\n\n");
    
    // Run compile.bat with output capture
    char compileCmdLine[2048];
    sprintf(compileCmdLine, "cmd.exe /c \"\"%s\" > \"%s\" 2> \"%s\"\"", 
            compileBat, tempOutFile, tempErrFile);
    
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    
    if (!CreateProcess(NULL, compileCmdLine, NULL, NULL, FALSE, 
                      CREATE_NO_WINDOW, NULL, sourceDir, &si, &pi)) {
        AppendOutput("Error: Could not run compile.bat.\n");
        return;
    }
    
    // Wait but process messages to keep IDE responsive
    while (WaitForSingleObject(pi.hProcess, 50) == WAIT_TIMEOUT) {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    GetExitCodeProcess(pi.hProcess, &exitCode);
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    // Read compilation output
    std::ifstream compOutFile(tempOutFile);
    if (compOutFile.is_open()) {
        std::string line;
        while (std::getline(compOutFile, line)) {
            AppendOutput(line.c_str());
            AppendOutput("\n");
        }
        compOutFile.close();
    }
    
    // Read compilation errors
    std::ifstream compErrFile(tempErrFile);
    if (compErrFile.is_open()) {
        std::string line;
        bool hasErrors = false;
        while (std::getline(compErrFile, line)) {
            if (!hasErrors) {
                AppendOutput("\n*** COMPILATION ERRORS ***\n");
                hasErrors = true;
            }
            AppendOutput(line.c_str());
            AppendOutput("\n");
        }
        compErrFile.close();
    }
    
    // Clean up temp files
    DeleteFile(tempOutFile);
    DeleteFile(tempErrFile);
    
    if (exitCode == 0) {
        AppendOutput("\nCompilation successful! Created program.exe\n");
    } else {
        char msg[128];
        sprintf(msg, "\nCompilation failed with exit code: %lu\n", (unsigned long)exitCode);
        AppendOutput(msg);
    }
}

void MainWindow::RunProgram() {
    ClearOutput();
    AppendOutput("=== Running Program ===\n\n");
    
    if (m_currentFile.empty()) {
        AppendOutput("Error: No file to run. Please compile first.\n");
        return;
    }
    
    // Get the directory of the source file
    char sourceDir[MAX_PATH];
    strcpy(sourceDir, m_currentFile.c_str());
    PathRemoveFileSpec(sourceDir);
    
    // program.exe is in the same directory as the source file
    char exeName[MAX_PATH];
    sprintf(exeName, "%s\\program.exe", sourceDir);
    
    // Check if executable exists
    if (GetFileAttributes(exeName) == INVALID_FILE_ATTRIBUTES) {
        AppendOutput("Error: program.exe not found. Please compile first.\n");
        AppendOutput("Expected: ");
        AppendOutput(exeName);
        AppendOutput("\n");
        return;
    }
    
    AppendOutput("Running: ");
    AppendOutput(exeName);
    AppendOutput("\n\n");
    
    // Execute program in a separate window (non-blocking)
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;  // Show console window
    
    // Use CREATE_NEW_CONSOLE so it runs in its own window
    if (CreateProcess(NULL, exeName, NULL, NULL, FALSE, 
                      CREATE_NEW_CONSOLE, NULL, sourceDir, &si, &pi)) {
        // Don't wait - let it run independently
        // Just close the handles immediately
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        
        AppendOutput("Program launched in separate window.\n");
    } else {
        AppendOutput("Error: Could not run program.exe\n");
    }
}

void MainWindow::RunCompileAndRun() {
    RunCompile();
    
    // Check if compilation was successful by looking for "successful" in output
    char outputText[4096];
    GetWindowText(m_hwndOutput, outputText, sizeof(outputText));
    if (strstr(outputText, "successful")) {
        RunProgram();
    }
}

void MainWindow::StopProgram() {
    AppendOutput("\n=== Stop not yet implemented ===\n");
}

// Output panel helpers
void MainWindow::AppendOutput(const char* text) {
    int len = GetWindowTextLength(m_hwndOutput);
    SendMessage(m_hwndOutput, EM_SETSEL, len, len);
    SendMessage(m_hwndOutput, EM_REPLACESEL, 0, (LPARAM)text);
}

void MainWindow::ClearOutput() {
    SetWindowText(m_hwndOutput, "");
}

// Editor text helpers
std::string MainWindow::GetEditorText() {
    int length = SendEditor(SCI_GETTEXTLENGTH);
    if (length <= 0) return "";
    
    char* buffer = new char[length + 1];
    SendEditor(SCI_GETTEXT, length + 1, (sptr_t)buffer);
    std::string result(buffer);
    delete[] buffer;
    
    return result;
}

void MainWindow::SetEditorText(const std::string& text) {
    SendEditor(SCI_SETTEXT, 0, (sptr_t)text.c_str());
}

bool MainWindow::IsModified() {
    return SendEditor(SCI_GETMODIFY) != 0;
}

void MainWindow::SetModified(bool modified) {
    if (!modified) {
        SendEditor(SCI_SETSAVEPOINT);
    }
    m_modified = modified;
    UpdateTitle();
}

// Scintilla wrapper
sptr_t MainWindow::SendEditor(unsigned int msg, uptr_t wParam, sptr_t lParam) {
    if (m_fnScintilla && m_pScintilla) {
        return m_fnScintilla(m_pScintilla, msg, wParam, lParam);
    }
    return 0;
}

// Line numbering features for BASIC programs
void MainWindow::AddLineNumber() {
    // Get current cursor position
    int currentPos = SendEditor(SCI_GETCURRENTPOS);
    
    // Get current line number (editor line, not BASIC line number)
    int editorLine = SendEditor(SCI_LINEFROMPOSITION, currentPos);
    
    // Find line number from previous line with a number
    int prevLineNum = 0;
    
    // Search backwards from current line to find previous BASIC line number
    for (int i = editorLine - 1; i >= 0; i--) {
        int lineStart = SendEditor(SCI_POSITIONFROMLINE, i);
        int lineEnd = SendEditor(SCI_GETLINEENDPOSITION, i);
        int lineLength = lineEnd - lineStart;
        
        if (lineLength > 0) {
            char* lineText = new char[lineLength + 1];
            SendEditor(SCI_GETLINE, i, (sptr_t)lineText);
            lineText[lineLength] = '\0';
            
            // Try to parse line number from start of line
            int lineNum = atoi(lineText);
            delete[] lineText;
            
            if (lineNum > 0) {
                prevLineNum = lineNum;
                break;
            }
        }
    }
    
    // Calculate next line number (previous + 10)
    int nextLineNum = prevLineNum + 10;
    
    // Get position at start of current line
    int lineStart = SendEditor(SCI_POSITIONFROMLINE, editorLine);
    
    // Check if we're at an empty line or start of line
    int lineEnd = SendEditor(SCI_GETLINEENDPOSITION, editorLine);
    bool isEmptyLine = (lineStart == lineEnd);
    
    // Create the line number string
    char lineNumStr[16];
    sprintf(lineNumStr, "%d ", nextLineNum);
    
    if (isEmptyLine || currentPos == lineStart) {
        // Insert at start of current line
        SendEditor(SCI_INSERTTEXT, lineStart, (sptr_t)lineNumStr);
        // Move cursor to after the line number
        SendEditor(SCI_SETSEL, lineStart + strlen(lineNumStr), lineStart + strlen(lineNumStr));
    } else {
        // Insert new line with line number
        char newLineStr[32];
        sprintf(newLineStr, "\n%d ", nextLineNum);
        SendEditor(SCI_INSERTTEXT, currentPos, (sptr_t)newLineStr);
        // Move cursor to after the line number on new line
        SendEditor(SCI_SETSEL, currentPos + strlen(newLineStr), currentPos + strlen(newLineStr));
    }
}

void MainWindow::RenumberLines() {
    std::string text = GetEditorText();
    std::istringstream iss(text);
    std::ostringstream oss;
    std::string line;
    int lineNum = 10;
    std::map<int, int> lineMap; // old -> new mapping
    
    // First pass: renumber and build map
    while (std::getline(iss, line)) {
        if (line.empty()) {
            oss << "\n";
            continue;
        }
        
        // Check if line starts with a number
        int oldNum = atoi(line.c_str());
        if (oldNum > 0) {
            // Store mapping
            lineMap[oldNum] = lineNum;
            
            // Find where the actual code starts (after line number)
            size_t pos = 0;
            while (pos < line.length() && isdigit(line[pos])) pos++;
            while (pos < line.length() && isspace(line[pos])) pos++;
            
            // Write new line number and code
            char newLine[16];
            sprintf(newLine, "%d ", lineNum);
            oss << newLine;
            if (pos < line.length()) {
                oss << line.substr(pos);
            }
            oss << "\n";
            
            lineNum += 10;
        } else {
            // Line without number - leave as is
            oss << line << "\n";
        }
    }
    
    // Second pass: update GOTO/GOSUB references
    std::string result = oss.str();
    std::istringstream iss2(result);
    std::ostringstream oss2;
    
    while (std::getline(iss2, line)) {
        std::string updated = UpdateLineReferences(line, lineMap);
        oss2 << updated << "\n";
    }
    
    SetEditorText(oss2.str());
}

void MainWindow::RemoveLineNumbers() {
    std::string text = GetEditorText();
    std::istringstream iss(text);
    std::ostringstream oss;
    std::string line;
    
    while (std::getline(iss, line)) {
        if (line.empty()) {
            oss << "\n";
            continue;
        }
        
        // Check if line starts with a number
        if (isdigit(line[0])) {
            // Find where the actual code starts
            size_t pos = 0;
            while (pos < line.length() && isdigit(line[pos])) pos++;
            while (pos < line.length() && isspace(line[pos])) pos++;
            
            // Write only the code part
            if (pos < line.length()) {
                oss << line.substr(pos);
            }
            oss << "\n";
        } else {
            oss << line << "\n";
        }
    }
    
    SetEditorText(oss.str());
}

std::string MainWindow::UpdateLineReferences(const std::string& line, 
                                             const std::map<int, int>& lineMap) {
    std::string result = line;
    std::string upper = line;
    
    // Convert to uppercase for keyword matching
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    
    // Find GOTO/GOSUB/THEN keywords
    std::vector<std::string> keywords = {"GOTO ", "GOSUB ", "THEN "};
    
    for (const auto& keyword : keywords) {
        size_t pos = 0;
        while ((pos = upper.find(keyword, pos)) != std::string::npos) {
            // Found keyword, look for line number after it
            size_t numStart = pos + keyword.length();
            
            // Skip whitespace
            while (numStart < result.length() && isspace(result[numStart])) {
                numStart++;
            }
            
            // Extract line number
            if (numStart < result.length() && isdigit(result[numStart])) {
                size_t numEnd = numStart;
                while (numEnd < result.length() && isdigit(result[numEnd])) {
                    numEnd++;
                }
                
                int oldLineNum = atoi(result.substr(numStart, numEnd - numStart).c_str());
                
                // Check if we have a mapping for this line number
                if (lineMap.find(oldLineNum) != lineMap.end()) {
                    int newLineNum = lineMap.at(oldLineNum);
                    char newNumStr[16];
                    sprintf(newNumStr, "%d", newLineNum);
                    
                    // Replace old number with new number
                    result = result.substr(0, numStart) + 
                             std::string(newNumStr) + 
                             result.substr(numEnd);
                    
                    // Update upper case version too
                    upper = result;
                    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
                }
            }
            
            pos = numStart;
        }
    }
    
    return result;
}

// Toggle output panel visibility
void MainWindow::ToggleOutput(bool show) {
    m_outputVisible = show;
    ResizeControls();
}