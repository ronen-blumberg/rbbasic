// main.cpp - RBIDE Entry Point
// RB BASIC (Ronen Blumberg Basic) Integrated Development Environment
// Copyright (c) 2025 Ronen Blumberg

#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "mainwindow.h"

// Global instance
HINSTANCE g_hInstance = NULL;

// Helper function to strip quotes from command line argument
std::string StripQuotes(const char* str) {
    if (!str || !str[0]) return "";
    
    std::string result(str);
    
    // Remove leading quotes
    while (!result.empty() && (result[0] == '"' || result[0] == '\'')) {
        result.erase(0, 1);
    }
    
    // Remove trailing quotes
    while (!result.empty() && (result[result.length()-1] == '"' || result[result.length()-1] == '\'')) {
        result.erase(result.length()-1);
    }
    
    // Remove leading/trailing whitespace
    while (!result.empty() && result[0] == ' ') {
        result.erase(0, 1);
    }
    while (!result.empty() && result[result.length()-1] == ' ') {
        result.erase(result.length()-1);
    }
    
    return result;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int nCmdShow)
{
    g_hInstance = hInstance;
    
    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES | ICC_COOL_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);
    
    // Create and show main window
    MainWindow mainWnd;
    if (!mainWnd.Create(hInstance)) {
        MessageBox(NULL, "Failed to create main window!", "Error", MB_ICONERROR);
        return 1;
    }
    
    mainWnd.Show(nCmdShow);
    
    // Check if file was passed on command line
    if (lpCmdLine && lpCmdLine[0]) {
        std::string filename = StripQuotes(lpCmdLine);
        if (!filename.empty()) {
            mainWnd.LoadFile(filename.c_str());
        }
    }
    
    // Message loop with accelerator support
    MSG msg;
    MainWindow* pMainWnd = &mainWnd;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator(pMainWnd->GetHWND(), pMainWnd->GetAccel(), &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    return (int)msg.wParam;
}
