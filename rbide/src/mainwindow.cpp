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
#define SCI_GOTOPOS 2025
#define SCI_GOTOLINE 2024
#define SCI_GETLENGTH 2006
#define SCI_GETCHARAT 2007
#define SCI_LINECOUNT 2154
#define SCI_SCROLLCARET 2169
#define SCI_SEARCHNEXT 2367
#define SCI_SEARCHPREV 2368
#define SCI_SETSEARCHFLAGS 2198
#define SCI_SETTARGETSTART 2190
#define SCI_SETTARGETEND 2192
#define SCI_SEARCHINTARGET 2197
#define SCI_REPLACETARGET 2194
#define SCI_GETTARGETSTART 2191
#define SCI_GETTARGETEND 2193
#define SCI_AUTOCSHOW 2100
#define SCI_AUTOCCANCEL 2101
#define SCI_AUTOCACTIVE 2102
#define SCI_AUTOCSETSEPARATOR 2106
#define SCI_AUTOCSETIGNORECASE 2115
#define SCI_WORDSTARTPOSITION 2266
#define SCI_WORDENDPOSITION 2267
#define SCI_MARKERDEFINE 2040
#define SCI_MARKERADD 2043
#define SCI_MARKERDELETE 2044
#define SCI_MARKERDELETEALL 2045
#define SCI_SETMARGINSENSITIVEN 2246
#define SCI_ENSUREVISIBLEENFORCEPOLICY 2234
#define SCI_SETINDICATORCURRENT 2500
#define SCI_INDICSETSTYLE 2080
#define SCI_INDICSETFORE 2082

#define SCFIND_MATCHCASE 4
#define SCFIND_WHOLEWORD 2

#define SC_MARK_CIRCLE 0
#define SC_MARK_SHORTARROW 4
#define SC_MARK_BACKGROUND 22

#define SCN_CHARADDED 2001
#define SCN_UPDATEUI 2007

#define INDIC_SQUIGGLE 1
#define INDIC_SQUIGGLEPIXMAP 20

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
    , m_hwndFindReplace(NULL)
    , m_hRecentMenu(NULL)
    , m_currentError(-1)
    , m_pScintilla(0)
    , m_fnScintilla(NULL)
{
    memset(&m_fr, 0, sizeof(m_fr));
    memset(m_findText, 0, sizeof(m_findText));
    memset(m_replaceText, 0, sizeof(m_replaceText));
    m_findReplaceMsg = RegisterWindowMessage(FINDMSGSTRING);
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
            LoadRecentFiles();
            UpdateRecentFilesMenu();
            return 0;
            
        case WM_SIZE:
            ResizeControls();
            return 0;
            
        case WM_COMMAND:
            return HandleCommand(wParam, lParam);

        case WM_NOTIFY: {
            NMHDR* nmhdr = (NMHDR*)lParam;
            if (nmhdr->hwndFrom == m_hwndEdit) {
                // Scintilla notification - ch is at offset 16 (after NMHDR + position)
                int code = nmhdr->code;
                if (code == SCN_CHARADDED) {
                    ShowAutoComplete();
                }
            }
            return 0;
        }
            
        case WM_CLOSE:
            if (PromptSave()) {
                DestroyWindow(m_hwnd);
            }
            return 0;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    
    // Handle registered Find/Replace message
    if (msg == m_findReplaceMsg && m_findReplaceMsg != 0) {
        HandleFindReplace(lParam);
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
        case IDM_EDIT_REPLACE:
            EditReplace();
            return 0;
        case IDM_EDIT_FINDNEXT:
            EditFindNext(true);
            return 0;
        case IDM_EDIT_GOTO:
            EditGoto();
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
        case IDM_RUN_NEXTERROR:
            GotoNextError();
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
        case IDM_HELP_CONTEXT:
            ContextHelp();
            return 0;
        case IDM_HELP_MANUAL:
            MessageBox(m_hwnd,
                "RB BASIC IDE Quick Help\n\n"
                "File Menu:\n"
                "  Ctrl+N - New file\n"
                "  Ctrl+O - Open file\n"
                "  Ctrl+S - Save file\n\n"
                "Edit Menu:\n"
                "  Ctrl+F - Find\n"
                "  Ctrl+H - Find & Replace\n"
                "  F3 - Find Next\n"
                "  Ctrl+G - Go to Line\n"
                "  Ctrl+L - Add Line Number\n"
                "  Ctrl+R - Renumber Lines\n"
                "  Ctrl+Shift+R - Remove Line Numbers\n"
                "  Ctrl+Z/Y/X/C/V/A - Undo/Redo/Cut/Copy/Paste/SelectAll\n\n"
                "Run Menu:\n"
                "  F5 - Compile & Run\n"
                "  F7 - Compile\n"
                "  F9 - Run\n"
                "  F4 - Next Error\n\n"
                "Help:\n"
                "  F1 - Context Help for keyword at cursor",
                "RB BASIC IDE Help",
                MB_ICONINFORMATION);
            return 0;
    }

    // Recent files
    int cmd = LOWORD(wParam);
    if (cmd >= IDM_FILE_RECENT1 && cmd <= IDM_FILE_RECENT5) {
        OpenRecentFile(cmd - IDM_FILE_RECENT1);
        return 0;
    }

    return 0;
}

// Initialize accelerators
void MainWindow::InitAccelerators() {
    ACCEL accel[16];
    int n = 0;

    accel[n].fVirt = FCONTROL | FVIRTKEY; accel[n].key = 'N'; accel[n].cmd = IDM_FILE_NEW; n++;
    accel[n].fVirt = FCONTROL | FVIRTKEY; accel[n].key = 'O'; accel[n].cmd = IDM_FILE_OPEN; n++;
    accel[n].fVirt = FCONTROL | FVIRTKEY; accel[n].key = 'S'; accel[n].cmd = IDM_FILE_SAVE; n++;
    accel[n].fVirt = FVIRTKEY; accel[n].key = VK_F5; accel[n].cmd = IDM_RUN_COMPILERUN; n++;
    accel[n].fVirt = FVIRTKEY; accel[n].key = VK_F7; accel[n].cmd = IDM_RUN_COMPILE; n++;
    accel[n].fVirt = FVIRTKEY; accel[n].key = VK_F9; accel[n].cmd = IDM_RUN_RUN; n++;
    accel[n].fVirt = FCONTROL | FVIRTKEY; accel[n].key = 'L'; accel[n].cmd = IDM_EDIT_ADDLINE; n++;
    accel[n].fVirt = FCONTROL | FVIRTKEY; accel[n].key = 'R'; accel[n].cmd = IDM_EDIT_RENUMBER; n++;
    accel[n].fVirt = FCONTROL | FSHIFT | FVIRTKEY; accel[n].key = 'R'; accel[n].cmd = IDM_EDIT_REMOVENUMBERS; n++;
    accel[n].fVirt = FCONTROL | FVIRTKEY; accel[n].key = 'F'; accel[n].cmd = IDM_EDIT_FIND; n++;
    // New shortcuts
    accel[n].fVirt = FCONTROL | FVIRTKEY; accel[n].key = 'H'; accel[n].cmd = IDM_EDIT_REPLACE; n++;
    accel[n].fVirt = FVIRTKEY; accel[n].key = VK_F3; accel[n].cmd = IDM_EDIT_FINDNEXT; n++;
    accel[n].fVirt = FCONTROL | FVIRTKEY; accel[n].key = 'G'; accel[n].cmd = IDM_EDIT_GOTO; n++;
    accel[n].fVirt = FVIRTKEY; accel[n].key = VK_F4; accel[n].cmd = IDM_RUN_NEXTERROR; n++;
    accel[n].fVirt = FVIRTKEY; accel[n].key = VK_F1; accel[n].cmd = IDM_HELP_CONTEXT; n++;

    m_hAccel = CreateAcceleratorTable(accel, n);
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
    // Recent files submenu
    m_hRecentMenu = CreatePopupMenu();
    AppendMenu(m_hRecentMenu, MF_STRING | MF_GRAYED, IDM_FILE_RECENT1, "(empty)");
    AppendMenu(hFileMenu, MF_POPUP, (UINT_PTR)m_hRecentMenu, "Recent &Files");
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
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_REPLACE, "Find && &Replace...\tCtrl+H");
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_FINDNEXT, "Find &Next\tF3");
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_GOTO, "&Go to Line...\tCtrl+G");
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
    AppendMenu(hRunMenu, MF_STRING, IDM_RUN_NEXTERROR, "Next &Error\tF4");
    AppendMenu(hRunMenu, MF_STRING, IDM_RUN_STOP, "&Stop");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hRunMenu, "&Run");
    
    // Help menu
    HMENU hHelpMenu = CreatePopupMenu();
    AppendMenu(hHelpMenu, MF_STRING, IDM_HELP_CONTEXT, "Context &Help\tF1");
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
    AddRecentFile(filename);

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
    if (m_hwndFindReplace) {
        SetFocus(m_hwndFindReplace);
        return;
    }
    memset(&m_fr, 0, sizeof(m_fr));
    m_fr.lStructSize = sizeof(FINDREPLACE);
    m_fr.hwndOwner = m_hwnd;
    m_fr.lpstrFindWhat = m_findText;
    m_fr.wFindWhatLen = sizeof(m_findText);
    m_fr.Flags = FR_DOWN;
    m_hwndFindReplace = FindText(&m_fr);
}

void MainWindow::EditReplace() {
    if (m_hwndFindReplace) {
        DestroyWindow(m_hwndFindReplace);
        m_hwndFindReplace = NULL;
    }
    memset(&m_fr, 0, sizeof(m_fr));
    m_fr.lStructSize = sizeof(FINDREPLACE);
    m_fr.hwndOwner = m_hwnd;
    m_fr.lpstrFindWhat = m_findText;
    m_fr.wFindWhatLen = sizeof(m_findText);
    m_fr.lpstrReplaceWith = m_replaceText;
    m_fr.wReplaceWithLen = sizeof(m_replaceText);
    m_fr.Flags = FR_DOWN;
    m_hwndFindReplace = ReplaceText(&m_fr);
}

void MainWindow::EditFindNext(bool forward) {
    if (m_findText[0] == '\0') {
        EditFind();
        return;
    }
    int flags = 0;
    if (m_fr.Flags & FR_MATCHCASE) flags |= SCFIND_MATCHCASE;
    if (m_fr.Flags & FR_WHOLEWORD) flags |= SCFIND_WHOLEWORD;
    SendEditor(SCI_SETSEARCHFLAGS, flags);

    int pos = SendEditor(SCI_GETCURRENTPOS);
    int docLen = SendEditor(SCI_GETLENGTH);

    if (forward) {
        SendEditor(SCI_SETTARGETSTART, pos);
        SendEditor(SCI_SETTARGETEND, docLen);
        int found = SendEditor(SCI_SEARCHINTARGET, strlen(m_findText), (sptr_t)m_findText);
        if (found == -1) {
            // Wrap around
            SendEditor(SCI_SETTARGETSTART, 0);
            SendEditor(SCI_SETTARGETEND, pos);
            found = SendEditor(SCI_SEARCHINTARGET, strlen(m_findText), (sptr_t)m_findText);
        }
        if (found >= 0) {
            int end = SendEditor(SCI_GETTARGETEND);
            SendEditor(SCI_SETSEL, found, end);
            SendEditor(SCI_SCROLLCARET);
        } else {
            MessageBox(m_hwnd, "Text not found.", "Find", MB_ICONINFORMATION);
        }
    } else {
        SendEditor(SCI_SETTARGETSTART, pos > 0 ? pos - 1 : 0);
        SendEditor(SCI_SETTARGETEND, 0);
        int found = SendEditor(SCI_SEARCHINTARGET, strlen(m_findText), (sptr_t)m_findText);
        if (found == -1) {
            SendEditor(SCI_SETTARGETSTART, docLen);
            SendEditor(SCI_SETTARGETEND, pos);
            found = SendEditor(SCI_SEARCHINTARGET, strlen(m_findText), (sptr_t)m_findText);
        }
        if (found >= 0) {
            int end = SendEditor(SCI_GETTARGETEND);
            SendEditor(SCI_SETSEL, found, end);
            SendEditor(SCI_SCROLLCARET);
        } else {
            MessageBox(m_hwnd, "Text not found.", "Find", MB_ICONINFORMATION);
        }
    }
}

void MainWindow::HandleFindReplace(LPARAM lParam) {
    FINDREPLACE* pfr = (FINDREPLACE*)lParam;
    if (pfr->Flags & FR_DIALOGTERM) {
        m_hwndFindReplace = NULL;
        return;
    }
    if (pfr->Flags & FR_FINDNEXT) {
        EditFindNext((pfr->Flags & FR_DOWN) != 0);
    }
    if (pfr->Flags & FR_REPLACE) {
        // Replace current selection and find next
        // Replace current selection then find next
        int pos = SendEditor(SCI_GETCURRENTPOS);
        int anchor = SendEditor(SCI_GETLENGTH); // just ensure valid
        (void)anchor;
        // Set target to current selection area
        SendEditor(SCI_SETTARGETSTART, SendEditor(SCI_GETTARGETSTART));
        SendEditor(SCI_SETTARGETEND, SendEditor(SCI_GETTARGETEND));
        if (SendEditor(SCI_GETTARGETSTART) < SendEditor(SCI_GETTARGETEND)) {
            SendEditor(SCI_REPLACETARGET, strlen(m_replaceText), (sptr_t)m_replaceText);
        }
        (void)pos;
        EditFindNext(true);
    }
    if (pfr->Flags & FR_REPLACEALL) {
        int flags = 0;
        if (pfr->Flags & FR_MATCHCASE) flags |= SCFIND_MATCHCASE;
        if (pfr->Flags & FR_WHOLEWORD) flags |= SCFIND_WHOLEWORD;
        SendEditor(SCI_SETSEARCHFLAGS, flags);

        int count = 0;
        int docLen = SendEditor(SCI_GETLENGTH);
        SendEditor(SCI_SETTARGETSTART, 0);
        SendEditor(SCI_SETTARGETEND, docLen);

        int findLen = strlen(m_findText);
        int replaceLen = strlen(m_replaceText);

        while (SendEditor(SCI_SEARCHINTARGET, findLen, (sptr_t)m_findText) >= 0) {
            SendEditor(SCI_REPLACETARGET, replaceLen, (sptr_t)m_replaceText);
            count++;
            // Adjust target for next search
            int newEnd = SendEditor(SCI_GETTARGETEND);
            docLen = SendEditor(SCI_GETLENGTH);
            SendEditor(SCI_SETTARGETSTART, newEnd);
            SendEditor(SCI_SETTARGETEND, docLen);
        }
        char msg[128];
        sprintf(msg, "Replaced %d occurrence(s).", count);
        MessageBox(m_hwnd, msg, "Replace All", MB_ICONINFORMATION);
    }
}

void MainWindow::EditGoto() {
    int currentLine = SendEditor(SCI_LINEFROMPOSITION, SendEditor(SCI_GETCURRENTPOS)) + 1;
    int totalLines = SendEditor(SCI_LINECOUNT);
    char prompt[128];
    sprintf(prompt, "Line number (1-%d):", totalLines);

    // Use a simple input box via a small dialog
    // For simplicity, use GetDlgItemText approach with a message box prompt
    // Actually, let's create a tiny modal dialog inline
    HWND hDlg = CreateWindowEx(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
        "STATIC", "Go to Line", WS_POPUP | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 250, 120, m_hwnd, NULL, m_hInstance, NULL);
    CreateWindowEx(0, "STATIC", prompt, WS_CHILD | WS_VISIBLE,
        10, 10, 220, 20, hDlg, NULL, m_hInstance, NULL);
    HWND hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE | ES_NUMBER, 10, 35, 220, 24, hDlg, (HMENU)1001, m_hInstance, NULL);
    CreateWindowEx(0, "BUTTON", "OK",
        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 60, 65, 60, 25, hDlg, (HMENU)IDOK, m_hInstance, NULL);
    CreateWindowEx(0, "BUTTON", "Cancel",
        WS_CHILD | WS_VISIBLE, 130, 65, 60, 25, hDlg, (HMENU)IDCANCEL, m_hInstance, NULL);

    char lineStr[16];
    sprintf(lineStr, "%d", currentLine);
    SetWindowText(hEdit, lineStr);
    SendMessage(hEdit, EM_SETSEL, 0, -1);

    // Center on parent
    RECT rcParent, rcDlg;
    GetWindowRect(m_hwnd, &rcParent);
    GetWindowRect(hDlg, &rcDlg);
    int x = rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2;
    int y = rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2;
    SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);

    ShowWindow(hDlg, SW_SHOW);
    SetFocus(hEdit);
    EnableWindow(m_hwnd, FALSE);

    MSG msg;
    bool done = false;
    bool accepted = false;
    while (!done && GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_RETURN) {
            accepted = true; done = true; break;
        }
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
            done = true; break;
        }
        if (msg.message == WM_COMMAND) {
            if (LOWORD(msg.wParam) == IDOK) { accepted = true; done = true; break; }
            if (LOWORD(msg.wParam) == IDCANCEL) { done = true; break; }
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    char buf[32];
    GetWindowText(hEdit, buf, sizeof(buf));
    EnableWindow(m_hwnd, TRUE);
    DestroyWindow(hDlg);
    SetFocus(m_hwndEdit);

    if (accepted && buf[0]) {
        int line = atoi(buf);
        if (line >= 1 && line <= totalLines) {
            SendEditor(SCI_GOTOLINE, line - 1);
            SendEditor(SCI_SCROLLCARET);
        }
    }
}

// Compilation and running
void MainWindow::RunCompile() {
    ClearOutput();
    m_errors.clear();
    m_currentError = -1;
    AppendOutput("=== Compiling RB BASIC (Ronen Blumberg Basic) ===\r\n\r\n");

    // Save current file if needed
    if (m_currentFile.empty()) {
        if (!FileSaveAs()) {
            AppendOutput("Error: File must be saved before compiling.\r\n");
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

    // Check for rbbasic.exe compiler
    char compilerPath[MAX_PATH];
    sprintf(compilerPath, "%s\\rbbasic.exe", rbbasicRoot);

    if (GetFileAttributes(compilerPath) == INVALID_FILE_ATTRIBUTES) {
        AppendOutput("Error: rbbasic.exe not found.\r\nExpected: ");
        AppendOutput(compilerPath);
        AppendOutput("\r\n");
        return;
    }

    // Find GCC in the mingw32 directory
    char gccPath[MAX_PATH];
    sprintf(gccPath, "%s\\mingw32\\bin\\gcc.exe", rbbasicRoot);
    if (GetFileAttributes(gccPath) == INVALID_FILE_ATTRIBUTES) {
        AppendOutput("Error: gcc.exe not found.\r\nExpected: ");
        AppendOutput(gccPath);
        AppendOutput("\r\n");
        return;
    }

    // Get the directory of the source file
    char sourceDir[MAX_PATH];
    strcpy(sourceDir, m_currentFile.c_str());
    PathRemoveFileSpec(sourceDir);
    if (sourceDir[0] == '\0') strcpy(sourceDir, ".");

    // Temp files for output capture
    char tempOutFile[MAX_PATH], tempErrFile[MAX_PATH];
    sprintf(tempOutFile, "%s\\rbide_stdout.tmp", sourceDir);
    sprintf(tempErrFile, "%s\\rbide_stderr.tmp", sourceDir);

    // --- Step 1: Transpile BASIC to C ---
    AppendOutput("Step 1: Transpiling BASIC to C\r\n");
    AppendOutput("Source: ");
    AppendOutput(m_currentFile.c_str());
    AppendOutput("\r\n\r\n");

    char cmdLine[2048];
    sprintf(cmdLine, "cmd.exe /c \"\"%s\" \"%s\" > \"%s\" 2> \"%s\"\"",
            compilerPath, m_currentFile.c_str(), tempOutFile, tempErrFile);

    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    if (!CreateProcess(NULL, cmdLine, NULL, NULL, FALSE,
                      CREATE_NO_WINDOW, NULL, sourceDir, &si, &pi)) {
        AppendOutput("Error: Could not run rbbasic.exe compiler.\r\n");
        return;
    }

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

    // Collect all output for error parsing
    std::string allOutput;

    std::ifstream outFile(tempOutFile);
    if (outFile.is_open()) {
        std::string line;
        while (std::getline(outFile, line)) {
            AppendOutput(line.c_str());
            AppendOutput("\r\n");
            allOutput += line + "\n";
        }
        outFile.close();
    }

    std::ifstream errFile(tempErrFile);
    if (errFile.is_open()) {
        std::string line;
        bool hasErrors = false;
        while (std::getline(errFile, line)) {
            if (!hasErrors) {
                AppendOutput("\r\n*** PARSING ERRORS ***\r\n");
                hasErrors = true;
            }
            AppendOutput(line.c_str());
            AppendOutput("\r\n");
            allOutput += line + "\n";
        }
        errFile.close();
    }

    DeleteFile(tempOutFile);
    DeleteFile(tempErrFile);

    if (exitCode != 0) {
        char msg[128];
        sprintf(msg, "\r\nTranspiler failed with exit code: %lu\r\n", (unsigned long)exitCode);
        AppendOutput(msg);
        ParseErrors(allOutput);
        if (!m_errors.empty()) {
            AppendOutput("Use F4 to navigate to errors.\r\n");
        }
        return;
    }

    AppendOutput("\r\nTranspilation successful!\r\n\r\n");

    // --- Step 2: Direct GCC compilation (no batch file) ---
    AppendOutput("Step 2: Compiling C to executable (direct GCC)\r\n");

    // Determine output files
    char outputC[MAX_PATH], shimC[MAX_PATH], programExe[MAX_PATH];
    sprintf(outputC, "%s\\output.c", sourceDir);
    sprintf(shimC, "%s\\shim_winmain.c", sourceDir);
    sprintf(programExe, "%s\\program.exe", sourceDir);

    if (GetFileAttributes(outputC) == INVALID_FILE_ATTRIBUTES) {
        AppendOutput("Error: output.c not generated.\r\n");
        return;
    }

    // Always generate shim_winmain.c (WinMain entry point shim)
    {
        FILE* shimFile = fopen(shimC, "wb");
        if (shimFile) {
            fprintf(shimFile,
                "#ifdef _WIN32\n"
                "#include <windows.h>\n"
                "#include <stdio.h>\n"
                "extern int main(int, char**);\n"
                "int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {\n"
                "    AllocConsole();\n"
                "    FILE *dummy;\n"
                "    freopen_s(&dummy, \"CONOUT$\", \"w\", stdout);\n"
                "    freopen_s(&dummy, \"CONOUT$\", \"w\", stderr);\n"
                "    freopen_s(&dummy, \"CONIN$\", \"r\", stdin);\n"
                "    return main(__argc, __argv);\n"
                "}\n"
                "#endif\n");
            fclose(shimFile);
        }
    }

    // Determine compile mode by scanning output.c
    // Priority: RB_COMPILE_MODE comment (from transpiler) > header-based detection
    bool needsSDL = false;       // Full graphics mode
    bool needsSDLImage = false;
    bool needsSDLMixer = false;
    bool isConsoleAudio = false;  // Console program with SDL audio only
    {
        std::ifstream checkFile(outputC);
        if (checkFile.is_open()) {
            std::string line;
            int linesChecked = 0;
            bool foundCompileMode = false;
            while (std::getline(checkFile, line) && linesChecked < 30) {
                // Check for explicit compile mode from transpiler
                if (line.find("RB_COMPILE_MODE: graphics") != std::string::npos) {
                    needsSDL = true;
                    foundCompileMode = true;
                } else if (line.find("RB_COMPILE_MODE: console_audio") != std::string::npos) {
                    isConsoleAudio = true;
                    needsSDLMixer = true;
                    foundCompileMode = true;
                } else if (line.find("RB_COMPILE_MODE: console") != std::string::npos) {
                    foundCompileMode = true;
                }
                // Also scan for specific libraries needed
                if (line.find("SDL2/SDL_image.h") != std::string::npos) {
                    needsSDLImage = true;
                }
                if (line.find("SDL2/SDL_mixer.h") != std::string::npos) {
                    needsSDLMixer = true;
                }
                // Fallback: if no compile mode comment, use old header detection
                if (!foundCompileMode && line.find("SDL2/SDL.h") != std::string::npos) {
                    needsSDL = true;
                }
                linesChecked++;
            }
            // If compile mode was found, needsSDL was set correctly by the mode
            // For console_audio mode, needsSDL stays false (no graphics)
            checkFile.close();
        }
    }

    // Build GCC command line
    std::string gccCmd = "\"";
    gccCmd += gccPath;
    gccCmd += "\" \"";
    gccCmd += outputC;
    gccCmd += "\"";

    if (needsSDL) {
        // Graphics: SDL2main provides WinMain entry point
        // Do NOT include shim_winmain.c (causes infinite recursion with libmingw32)
        char sdlInc[MAX_PATH], sdlLib[MAX_PATH];
        sprintf(sdlInc, "%s\\lib\\SDL2-mingw32\\i686-w64-mingw32\\include", rbbasicRoot);
        sprintf(sdlLib, "%s\\lib\\SDL2-mingw32\\i686-w64-mingw32\\lib", rbbasicRoot);
        gccCmd += " -I\"";
        gccCmd += sdlInc;
        gccCmd += "\" -L\"";
        gccCmd += sdlLib;
        gccCmd += "\" -lmingw32 -lSDL2main -lSDL2";
        if (needsSDLImage) gccCmd += " -lSDL2_image";
        if (needsSDLMixer) gccCmd += " -lSDL2_mixer";
        gccCmd += " -mwindows";
    } else if (isConsoleAudio) {
        // Console program with SDL audio: no shim, no -mwindows, explicit -mconsole
        // SDL2 import lib may set GUI subsystem, -mconsole overrides it
        char sdlInc[MAX_PATH], sdlLib[MAX_PATH];
        sprintf(sdlInc, "%s\\lib\\SDL2-mingw32\\i686-w64-mingw32\\include", rbbasicRoot);
        sprintf(sdlLib, "%s\\lib\\SDL2-mingw32\\i686-w64-mingw32\\lib", rbbasicRoot);
        gccCmd += " -I\"";
        gccCmd += sdlInc;
        gccCmd += "\" -L\"";
        gccCmd += sdlLib;
        gccCmd += "\" -lSDL2 -lSDL2_mixer -lwinmm -mconsole";
    } else {
        // Console: include WinMain shim for console allocation
        gccCmd += " \"";
        gccCmd += shimC;
        gccCmd += "\"";
    }

    gccCmd += " -o \"";
    gccCmd += programExe;
    gccCmd += "\" -lm";

    AppendOutput("GCC: ");
    AppendOutput(gccPath);
    AppendOutput("\r\n");
    if (needsSDL) AppendOutput("Mode: Graphics (SDL2)\r\n");
    else if (isConsoleAudio) AppendOutput("Mode: Console + Audio (SDL2_mixer)\r\n");
    else AppendOutput("Mode: Console\r\n");
    AppendOutput("\r\n");

    // Run GCC via cmd for output capture
    char gccCmdLine[4096];
    sprintf(gccCmdLine, "cmd.exe /c \"%s > \"%s\" 2> \"%s\"\"",
            gccCmd.c_str(), tempOutFile, tempErrFile);

    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    memset(&pi, 0, sizeof(pi));

    if (!CreateProcess(NULL, gccCmdLine, NULL, NULL, FALSE,
                      CREATE_NO_WINDOW, NULL, sourceDir, &si, &pi)) {
        AppendOutput("Error: Could not run gcc.exe.\r\n");
        return;
    }

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

    std::ifstream compOutFile(tempOutFile);
    if (compOutFile.is_open()) {
        std::string line;
        while (std::getline(compOutFile, line)) {
            AppendOutput(line.c_str());
            AppendOutput("\r\n");
        }
        compOutFile.close();
    }

    std::ifstream compErrFile(tempErrFile);
    if (compErrFile.is_open()) {
        std::string line;
        bool hasErrors = false;
        while (std::getline(compErrFile, line)) {
            if (!hasErrors) {
                AppendOutput("*** GCC OUTPUT ***\r\n");
                hasErrors = true;
            }
            AppendOutput(line.c_str());
            AppendOutput("\r\n");
        }
        compErrFile.close();
    }

    DeleteFile(tempOutFile);
    DeleteFile(tempErrFile);

    if (exitCode == 0) {
        AppendOutput("\r\nCompilation successful! Created program.exe\r\n");

        // Copy required SDL2 DLLs to output directory for graphics or audio programs
        if (needsSDL || isConsoleAudio) {
            char sdlBin[MAX_PATH];
            sprintf(sdlBin, "%s\\lib\\SDL2-mingw32\\i686-w64-mingw32\\bin", rbbasicRoot);

            char srcDll[MAX_PATH], dstDll[MAX_PATH];

            // Always copy SDL2.dll for programs that use SDL
            sprintf(srcDll, "%s\\SDL2.dll", sdlBin);
            sprintf(dstDll, "%s\\SDL2.dll", sourceDir);
            if (CopyFile(srcDll, dstDll, FALSE)) {
                AppendOutput("Copied SDL2.dll\r\n");
            }

            // Copy SDL2_image.dll if needed
            if (needsSDLImage) {
                sprintf(srcDll, "%s\\SDL2_image.dll", sdlBin);
                sprintf(dstDll, "%s\\SDL2_image.dll", sourceDir);
                if (CopyFile(srcDll, dstDll, FALSE)) {
                    AppendOutput("Copied SDL2_image.dll\r\n");
                }
            }

            // Copy SDL2_mixer.dll if needed
            if (needsSDLMixer) {
                sprintf(srcDll, "%s\\SDL2_mixer.dll", sdlBin);
                sprintf(dstDll, "%s\\SDL2_mixer.dll", sourceDir);
                if (CopyFile(srcDll, dstDll, FALSE)) {
                    AppendOutput("Copied SDL2_mixer.dll\r\n");
                }
            }
        }
    } else {
        char msg[128];
        sprintf(msg, "\r\nGCC compilation failed with exit code: %lu\r\n", (unsigned long)exitCode);
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

// --- Error Navigation ---

void MainWindow::ParseErrors(const std::string& output) {
    m_errors.clear();
    m_currentError = -1;

    // Parse patterns like:
    // "Parse error at line X, column Y: message"
    // "Error on line X: message"
    // "line X" (generic)
    std::istringstream iss(output);
    std::string line;
    while (std::getline(iss, line)) {
        // Check for "at line N" or "on line N"
        std::string upper = line;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

        size_t pos = upper.find("LINE ");
        if (pos != std::string::npos) {
            pos += 5; // skip "LINE "
            if (pos < line.size() && isdigit(line[pos])) {
                int lineNum = atoi(line.c_str() + pos);
                if (lineNum > 0) {
                    ErrorInfo err;
                    err.line = lineNum;
                    err.message = line;
                    m_errors.push_back(err);
                }
            }
        }
    }
}

void MainWindow::GotoNextError() {
    if (m_errors.empty()) {
        AppendOutput("No errors to navigate.\r\n");
        return;
    }
    m_currentError++;
    if (m_currentError >= (int)m_errors.size()) {
        m_currentError = 0;
    }

    int targetLine = m_errors[m_currentError].line - 1; // 0-based
    if (targetLine < 0) targetLine = 0;

    // Check if lines have BASIC line numbers - search for the matching one
    int totalLines = SendEditor(SCI_LINECOUNT);
    bool found = false;

    for (int i = 0; i < totalLines; i++) {
        int lineStart = SendEditor(SCI_POSITIONFROMLINE, i);
        int lineEnd = SendEditor(SCI_GETLINEENDPOSITION, i);
        int lineLen = lineEnd - lineStart;
        if (lineLen > 0 && lineLen < 1024) {
            char buf[1024];
            SendEditor(SCI_GETLINE, i, (sptr_t)buf);
            buf[lineLen] = '\0';
            int basicLine = atoi(buf);
            if (basicLine == m_errors[m_currentError].line) {
                SendEditor(SCI_GOTOLINE, i);
                SendEditor(SCI_SCROLLCARET);
                found = true;
                break;
            }
        }
    }

    if (!found) {
        // Fall back to editor line number (0-based)
        if (targetLine < totalLines) {
            SendEditor(SCI_GOTOLINE, targetLine);
            SendEditor(SCI_SCROLLCARET);
        }
    }

    // Show error in status bar
    char status[512];
    sprintf(status, "Error %d/%d: %s", m_currentError + 1,
            (int)m_errors.size(), m_errors[m_currentError].message.c_str());
    SendMessage(m_hwndStatus, SB_SETTEXT, 0, (LPARAM)status);
    SetFocus(m_hwndEdit);
}

// --- Auto-Complete ---

static const char* AC_KEYWORDS[] = {
    "ABS", "ACCESS", "AND", "APPEND", "AS", "ASC", "ATN",
    "BEEP", "BINARY",
    "CALL", "CASE", "CDBL", "CHR$", "CINT", "CIRCLE", "CLEAR", "CLNG", "CLOSE", "CLS",
    "COLOR", "COMMAND$", "CONST", "COS", "CSNG", "CSRLIN",
    "DATA", "DATE$", "DECLARE", "DEF", "DIM", "DO", "DOUBLE", "DRAW",
    "ELSE", "ELSEIF", "END", "ENVIRON", "ENVIRON$", "EOF", "ERASE", "ERL", "ERR", "ERROR", "EXIT", "EXP",
    "FOR", "FREEFILE", "FUNCTION",
    "GET", "GOSUB", "GOTO",
    "HEX$",
    "IF", "INKEY$", "INPUT", "INPUT$", "INSTR", "INT", "INTEGER",
    "KEY", "KILL",
    "LBOUND", "LCASE$", "LEFT$", "LEN", "LET", "LINE", "LOC", "LOCATE", "LOF", "LOG", "LONG", "LOOP", "LTRIM$",
    "MID$", "MOD",
    "NEXT", "NOT",
    "OCT$", "ON", "OPEN", "OR", "OUTPUT",
    "PAINT", "PALETTE", "PCOPY", "PEEK", "PLAY", "POINT", "POKE", "POS", "PRESET", "PRINT", "PSET", "PUT",
    "RANDOM", "RANDOMIZE", "READ", "REDIM", "REM", "RESTORE", "RESUME", "RETURN", "RIGHT$", "RND", "RTRIM$",
    "SCREEN", "SEEK", "SELECT", "SGN", "SHARED", "SHELL", "SIN", "SINGLE", "SLEEP", "SOUND",
    "SPACE$", "SPC", "SQR", "STATIC", "STEP", "STOP", "STR$", "STRING", "STRING$", "SUB", "SWAP", "SYSTEM",
    "TAB", "TAN", "THEN", "TIME$", "TIMER", "TO", "TRIM$", "TYPE",
    "UBOUND", "UCASE$", "UNTIL", "USING",
    "VAL", "VIEW",
    "WAIT", "WEND", "WHILE", "WIDTH", "WINDOW", "WRITE",
    "XOR",
    NULL
};

std::string MainWindow::GetWordAtCursor() {
    int pos = SendEditor(SCI_GETCURRENTPOS);
    int start = SendEditor(SCI_WORDSTARTPOSITION, pos, 1);
    int end = pos;
    int len = end - start;
    if (len <= 0 || len > 64) return "";

    std::string word;
    for (int i = start; i < end; i++) {
        word += (char)SendEditor(SCI_GETCHARAT, i);
    }
    return word;
}

void MainWindow::ShowAutoComplete() {
    std::string word = GetWordAtCursor();
    if (word.length() < 3) return;

    // Convert to uppercase for matching
    std::string upper = word;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

    // Build list of matching keywords
    std::string matches;
    for (int i = 0; AC_KEYWORDS[i] != NULL; i++) {
        std::string kw = AC_KEYWORDS[i];
        if (kw.length() >= upper.length() && kw.substr(0, upper.length()) == upper) {
            if (!matches.empty()) matches += " ";
            matches += kw;
        }
    }

    if (!matches.empty()) {
        SendEditor(SCI_AUTOCSETIGNORECASE, 1);
        SendEditor(SCI_AUTOCSETSEPARATOR, ' ');
        SendEditor(SCI_AUTOCSHOW, word.length(), (sptr_t)matches.c_str());
    }
}

// --- Context Help (F1) ---

void MainWindow::ContextHelp() {
    std::string word = GetWordAtCursor();
    if (word.empty()) {
        AppendOutput("Place cursor on a keyword and press F1 for help.\r\n");
        return;
    }

    std::string upper = word;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

    // Keyword help lookup
    struct HelpEntry { const char* keyword; const char* syntax; const char* desc; };
    static const HelpEntry helpTable[] = {
        {"PRINT", "PRINT expr [; expr] ...", "Display text or values on screen"},
        {"INPUT", "INPUT [\"prompt\";] var", "Read user input from keyboard"},
        {"IF", "IF condition THEN ... [ELSE ...] END IF", "Conditional execution"},
        {"FOR", "FOR var = start TO end [STEP n] ... NEXT [var]", "Counted loop"},
        {"WHILE", "WHILE condition ... WEND", "Loop while condition is true"},
        {"DO", "DO [WHILE|UNTIL cond] ... LOOP [WHILE|UNTIL cond]", "Flexible loop structure"},
        {"DIM", "DIM var(size) | DIM var AS type", "Declare variable or array"},
        {"SUB", "SUB name(params) ... END SUB", "Define a subroutine"},
        {"FUNCTION", "FUNCTION name(params) ... END FUNCTION", "Define a function"},
        {"SELECT", "SELECT CASE expr ... CASE val ... END SELECT", "Multi-way branching"},
        {"GOTO", "GOTO line", "Jump to a line number or label"},
        {"GOSUB", "GOSUB line ... RETURN", "Call subroutine at line, RETURN to continue"},
        {"OPEN", "OPEN file FOR mode AS #n", "Open file (INPUT/OUTPUT/APPEND/BINARY/RANDOM)"},
        {"CLOSE", "CLOSE #n", "Close an open file"},
        {"READ", "READ var [, var...]", "Read values from DATA statements"},
        {"DATA", "DATA value [, value...]", "Define data values for READ"},
        {"RESTORE", "RESTORE [line]", "Reset DATA pointer"},
        {"LET", "LET var = expr", "Assign value to variable (LET is optional)"},
        {"REM", "REM comment text", "Add a comment (ignored by compiler)"},
        {"END", "END", "Terminate program execution"},
        {"CLS", "CLS", "Clear the screen"},
        {"COLOR", "COLOR fore [, back]", "Set text foreground/background color"},
        {"LOCATE", "LOCATE row, col", "Move cursor to position"},
        {"SCREEN", "SCREEN mode", "Set graphics screen mode (7,8,9,12,13)"},
        {"PSET", "PSET (x, y) [, color]", "Set a pixel on graphics screen"},
        {"LINE", "LINE (x1,y1)-(x2,y2) [,color] [,B[F]]", "Draw line or box"},
        {"CIRCLE", "CIRCLE (x, y), radius [, color]", "Draw a circle"},
        {"PAINT", "PAINT (x, y), fill [, border]", "Flood fill an area"},
        {"DRAW", "DRAW command$", "Execute GML turtle graphics commands"},
        {"INKEY$", "k$ = INKEY$", "Read keyboard without waiting (returns empty if no key)"},
        {"CHR$", "CHR$(n)", "Return character for ASCII code n"},
        {"ASC", "ASC(char$)", "Return ASCII code of first character"},
        {"LEN", "LEN(string$)", "Return length of string"},
        {"LEFT$", "LEFT$(string$, n)", "Return leftmost n characters"},
        {"RIGHT$", "RIGHT$(string$, n)", "Return rightmost n characters"},
        {"MID$", "MID$(string$, start [, len])", "Return/assign substring"},
        {"STR$", "STR$(number)", "Convert number to string"},
        {"VAL", "VAL(string$)", "Convert string to number"},
        {"INSTR", "INSTR([start,] search$, find$)", "Find substring position"},
        {"UCASE$", "UCASE$(string$)", "Convert string to uppercase"},
        {"LCASE$", "LCASE$(string$)", "Convert string to lowercase"},
        {"LTRIM$", "LTRIM$(string$)", "Remove leading spaces"},
        {"RTRIM$", "RTRIM$(string$)", "Remove trailing spaces"},
        {"TRIM$", "TRIM$(string$)", "Remove leading and trailing spaces"},
        {"SPACE$", "SPACE$(n)", "Return string of n spaces"},
        {"STRING$", "STRING$(n, char$|code)", "Return string of n repeated characters"},
        {"INT", "INT(n)", "Return integer part (floor)"},
        {"ABS", "ABS(n)", "Return absolute value"},
        {"SGN", "SGN(n)", "Return sign (-1, 0, or 1)"},
        {"SQR", "SQR(n)", "Return square root"},
        {"RND", "RND [(n)]", "Return random number 0 to 1"},
        {"RANDOMIZE", "RANDOMIZE [TIMER]", "Seed random number generator"},
        {"SIN", "SIN(angle)", "Sine (angle in radians)"},
        {"COS", "COS(angle)", "Cosine (angle in radians)"},
        {"TAN", "TAN(angle)", "Tangent (angle in radians)"},
        {"ATN", "ATN(n)", "Arctangent (returns radians)"},
        {"LOG", "LOG(n)", "Natural logarithm"},
        {"EXP", "EXP(n)", "e raised to power n"},
        {"TIMER", "TIMER", "Seconds since midnight"},
        {"SWAP", "SWAP var1, var2", "Exchange values of two variables"},
        {"CONST", "CONST name = value", "Declare a constant"},
        {"REDIM", "REDIM [PRESERVE] array(newsize)", "Resize dynamic array"},
        {"ERASE", "ERASE array", "Free array memory"},
        {"TYPE", "TYPE name ... END TYPE", "Define user-defined type (struct)"},
        {"SHELL", "SHELL [command$]", "Execute system command"},
        {"ENVIRON$", "ENVIRON$(varname$)", "Get environment variable value"},
        {"COMMAND$", "COMMAND$", "Get command line arguments"},
        {"PEEK", "PEEK(address)", "Read byte from emulated memory"},
        {"POKE", "POKE address, value", "Write byte to emulated memory"},
        {"PLAY", "PLAY command$", "Play music using MML notation"},
        {"SOUND", "SOUND frequency, duration", "Generate a tone"},
        {"ON", "ON expr GOTO/GOSUB line1, line2...", "Computed branch"},
        {"EXIT", "EXIT FOR|DO|WHILE|SUB|FUNCTION", "Exit current block"},
        {"SHARED", "SHARED var", "Access module-level variable in SUB/FUNCTION"},
        {"STATIC", "STATIC var", "Preserve variable between calls"},
        {NULL, NULL, NULL}
    };

    ClearOutput();
    bool found = false;
    for (int i = 0; helpTable[i].keyword != NULL; i++) {
        if (upper == helpTable[i].keyword) {
            char buf[512];
            sprintf(buf, "=== %s ===\r\n\r\nSyntax: %s\r\n\r\n%s\r\n",
                    helpTable[i].keyword, helpTable[i].syntax, helpTable[i].desc);
            AppendOutput(buf);
            found = true;
            break;
        }
    }

    if (!found) {
        char buf[256];
        sprintf(buf, "No help available for '%s'.\r\n", word.c_str());
        AppendOutput(buf);
    }
}

// --- Recent Files ---

void MainWindow::LoadRecentFiles() {
    m_recentFiles.clear();
    char configPath[MAX_PATH];
    GetModuleFileName(NULL, configPath, MAX_PATH);
    PathRemoveFileSpec(configPath);
    strcat(configPath, "\\rbide_recent.txt");

    std::ifstream f(configPath);
    if (f.is_open()) {
        std::string line;
        while (std::getline(f, line) && m_recentFiles.size() < 5) {
            if (!line.empty() && GetFileAttributes(line.c_str()) != INVALID_FILE_ATTRIBUTES) {
                m_recentFiles.push_back(line);
            }
        }
        f.close();
    }
}

void MainWindow::SaveRecentFiles() {
    char configPath[MAX_PATH];
    GetModuleFileName(NULL, configPath, MAX_PATH);
    PathRemoveFileSpec(configPath);
    strcat(configPath, "\\rbide_recent.txt");

    std::ofstream f(configPath);
    if (f.is_open()) {
        for (const auto& path : m_recentFiles) {
            f << path << "\n";
        }
        f.close();
    }
}

void MainWindow::AddRecentFile(const std::string& path) {
    // Remove if already in list
    for (auto it = m_recentFiles.begin(); it != m_recentFiles.end(); ++it) {
        if (*it == path) { m_recentFiles.erase(it); break; }
    }
    // Insert at front
    m_recentFiles.insert(m_recentFiles.begin(), path);
    // Keep max 5
    if (m_recentFiles.size() > 5) m_recentFiles.resize(5);
    SaveRecentFiles();
    UpdateRecentFilesMenu();
}

void MainWindow::UpdateRecentFilesMenu() {
    if (!m_hRecentMenu) return;

    // Clear existing items
    while (GetMenuItemCount(m_hRecentMenu) > 0) {
        DeleteMenu(m_hRecentMenu, 0, MF_BYPOSITION);
    }

    if (m_recentFiles.empty()) {
        AppendMenu(m_hRecentMenu, MF_STRING | MF_GRAYED, IDM_FILE_RECENT1, "(empty)");
    } else {
        for (int i = 0; i < (int)m_recentFiles.size(); i++) {
            char filename[MAX_PATH];
            strcpy(filename, m_recentFiles[i].c_str());
            PathStripPath(filename);
            char label[MAX_PATH + 8];
            sprintf(label, "&%d %s", i + 1, filename);
            AppendMenu(m_hRecentMenu, MF_STRING, IDM_FILE_RECENT1 + i, label);
        }
    }
}

void MainWindow::OpenRecentFile(int index) {
    if (index < 0 || index >= (int)m_recentFiles.size()) return;
    if (!PromptSave()) return;
    LoadFile(m_recentFiles[index].c_str());
}