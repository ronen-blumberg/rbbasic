// mainwindow.h - Main Window Class
// RB BASIC IDE Main Window

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <windows.h>
#include <commdlg.h>
#include <string>
#include <vector>
#include <map>

// Forward declare Scintilla types if not already defined
#ifndef SCINTILLA_H
typedef long sptr_t;
typedef unsigned long uptr_t;
#endif

class MainWindow {
public:
    MainWindow();
    ~MainWindow();
    
    bool Create(HINSTANCE hInstance);
    void Show(int nCmdShow);
    bool LoadFile(const char* filename);
	    // ADD THESE 3 LINES:
    HWND GetHWND() const { return m_hwnd; }
    HACCEL GetAccel() const { return m_hAccel; }
    void InitAccelerators();
    
private:
    HWND m_hwnd;              // Main window
    HWND m_hwndEdit;          // Scintilla editor
    HWND m_hwndOutput;        // Output panel (read-only edit control)
    HWND m_hwndStatus;        // Status bar
    HWND m_hwndSplitter;      // Splitter bar
    
    HINSTANCE m_hInstance;
    HMODULE m_hScintilla;     // Scintilla DLL handle
    
	HACCEL m_hAccel;  // ADD THIS LINE
	
    std::string m_currentFile;
    bool m_modified;
    bool m_outputVisible;
    int m_splitterPos;        // Splitter position from bottom

    // Find/Replace
    HWND m_hwndFindReplace;
    FINDREPLACE m_fr;
    char m_findText[256];
    char m_replaceText[256];
    UINT m_findReplaceMsg;

    // Recent files
    std::vector<std::string> m_recentFiles;
    HMENU m_hRecentMenu;

    // Error navigation
    struct ErrorInfo { int line; std::string message; };
    std::vector<ErrorInfo> m_errors;
    int m_currentError;
    
    // Scintilla direct access
    sptr_t m_pScintilla;
    sptr_t (*m_fnScintilla)(sptr_t, int, uptr_t, sptr_t);
    
    // Window procedure
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleCommand(WPARAM wParam, LPARAM lParam);
    
    // Initialization
    bool InitScintilla();
    void CreateMenus();
    void CreateControls();
    void SetupEditor();
    void SetupSyntaxHighlighting();
    void UpdateTitle();
    void UpdateStatusBar();
    
    // Layout
    void ResizeControls();
    void ToggleOutput(bool show);
    
    // File operations
    void FileNew();
    void FileOpen();
    bool FileSave();
    bool FileSaveAs();
    bool PromptSave();
    
    // Edit operations
    void EditUndo();
    void EditRedo();
    void EditCut();
    void EditCopy();
    void EditPaste();
    void EditSelectAll();
    void EditFind();
    void EditReplace();
    void EditFindNext(bool forward = true);
    void EditGoto();
    void HandleFindReplace(LPARAM lParam);
    
    // Compilation and running
    void RunCompile();
    void RunProgram();
    void RunCompileAndRun();
    void StopProgram();
    void GotoNextError();
    void ParseErrors(const std::string& output);
    
    // Helpers
    void AppendOutput(const char* text);
    void ClearOutput();
    std::string GetEditorText();
    void SetEditorText(const std::string& text);
    bool IsModified();
    void SetModified(bool modified);

    // Auto-complete
    void ShowAutoComplete();

    // Context help
    void ContextHelp();
    std::string GetWordAtCursor();

    // Recent files
    void LoadRecentFiles();
    void SaveRecentFiles();
    void AddRecentFile(const std::string& path);
    void UpdateRecentFilesMenu();
    void OpenRecentFile(int index);
	
	void HighlightSyntax();
    void ProcessLine(const char* line, int len, int startPos);
    void SetTextColor(int start, int length, COLORREF color);
    
    void AddLineNumber();
    void RenumberLines();
    void RemoveLineNumbers();
    std::string UpdateLineReferences(const std::string& line, 
                                     const std::map<int, int>& lineMap);	
	
    // Scintilla wrapper
    sptr_t SendEditor(unsigned int msg, uptr_t wParam = 0, sptr_t lParam = 0);
};

#endif // MAINWINDOW_H
