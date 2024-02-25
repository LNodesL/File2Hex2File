#include <windows.h>
#include <commdlg.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <shellapi.h>

#define IDC_MAIN_EDIT 101
#define WM_USER_CONVERSION_START (WM_USER + 1)
#define WM_USER_CONVERSION_COMPLETE (WM_USER + 2)
#define ID_FILE_NEW 200
#define ID_FILE_RESTART 201
#define ID_FILE_OPEN 202

HINSTANCE globalInstance;
HWND hwndEdit;

struct FileProcessParams {
    std::string filePath;
    HWND hwndMain;
};

std::string toHexString(const std::vector<unsigned char>& data);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI ProcessFileThread(LPVOID lpParam);
void OpenFile(HWND hwnd);
void ClearEditControlContent();
void ResetWindowForFileSelection(HWND hwnd);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR pCmdLine, int nCmdShow) {
    globalInstance = hInstance;
    const char CLASS_NAME[] = "MainHexViewerClass";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClass(&wc);
    HWND hwndMain = CreateWindowEx(0, CLASS_NAME, "File2Hex", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, NULL, NULL, hInstance, NULL);
    if (hwndMain == NULL) return 0;
    ShowWindow(hwndMain, nCmdShow);
    UpdateWindow(hwndMain);
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        hwndEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY, 0, 0, 500, 400, hwnd, (HMENU)IDC_MAIN_EDIT, globalInstance, NULL);
        ShowWindow(hwndEdit, SW_HIDE);
        HMENU hMenubar = CreateMenu();
        HMENU hFileMenu = CreateMenu();
        AppendMenu(hFileMenu, MF_STRING, ID_FILE_OPEN, "Open File (Ctrl+O)");
        AppendMenu(hFileMenu, MF_STRING, ID_FILE_NEW, "New Window (Ctrl+N)");
        AppendMenu(hFileMenu, MF_STRING, ID_FILE_RESTART, "Reset Window (Ctrl+R)");
        AppendMenu(hFileMenu, MF_STRING, ID_FILE_RESTART, "Quit (Ctrl+Q)");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFileMenu, "Options");
        SetMenu(hwnd, hMenubar);
        break;
    }
    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case ID_FILE_NEW:
            TCHAR szFileName[MAX_PATH];
            GetModuleFileName(NULL, szFileName, MAX_PATH);
            ShellExecute(NULL, "open", szFileName, NULL, NULL, SW_SHOW);
            break;
        case ID_FILE_RESTART:
            ClearEditControlContent();
            ResetWindowForFileSelection(hwnd);
            break;
        case ID_FILE_OPEN:
            OpenFile(hwnd);
            break;
        }
        break;
    }
    case WM_KEYDOWN: {
        bool ctrlPressed = GetKeyState(VK_CONTROL) & 0x8000; // Check if CTRL key is pressed
        if (ctrlPressed) {
            switch (wParam) {
            case 'N': // CTRL+N
                TCHAR szFileName[MAX_PATH];
                GetModuleFileName(NULL, szFileName, MAX_PATH);
                ShellExecute(NULL, "open", szFileName, NULL, NULL, SW_SHOW);
                break;
            case 'R': // CTRL+R
                ClearEditControlContent();
                ResetWindowForFileSelection(hwnd);
                break;
            case 'O': // CTRL+O
                OpenFile(hwnd);
                break;
            }
        }
        break;
    }
    case WM_SIZE: {
        HWND hwndEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
        if (hwndEdit) {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            SetWindowPos(hwndEdit, NULL, 0, 0, width, height, SWP_NOZORDER);
        }
        break;
    }
    case WM_USER_CONVERSION_START: {
        HWND hwndEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
        ShowWindow(hwndEdit, SW_SHOW);
        break;
    }
    case WM_USER_CONVERSION_COMPLETE: {
        HWND hwndEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
        ShowWindow(hwndEdit, SW_HIDE);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void OpenFile(HWND hwnd) {
    OPENFILENAME ofn;
    char fileName[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "All Files\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "";
    if (GetOpenFileName(&ofn)) {
        ClearEditControlContent(); // Clears the content
        ResetWindowForFileSelection(hwnd); // Resets window state
        FileProcessParams* params = new FileProcessParams{fileName, hwnd};
        CreateThread(NULL, 0, ProcessFileThread, params, 0, NULL);
    }
}

DWORD WINAPI ProcessFileThread(LPVOID lpParam) {
    FileProcessParams* params = static_cast<FileProcessParams*>(lpParam);
    std::ifstream file(params->filePath, std::ios::binary);
    if (!file.is_open()) {
        MessageBox(NULL, "Failed to open file", "Error", MB_ICONERROR);
        delete params;
        return 0;
    }
    HWND hwndMain = params->hwndMain;
    PostMessage(hwndMain, WM_USER_CONVERSION_START, 0, 0);
    const int chunkSize = 1024;
    std::vector<unsigned char> buffer(chunkSize, 0);
    std::string hexString;
    HWND hwndEdit = GetDlgItem(params->hwndMain, IDC_MAIN_EDIT);
    while (file.read(reinterpret_cast<char*>(buffer.data()), chunkSize) || file.gcount() > 0) {
        if(file.gcount() > 0) buffer.resize(file.gcount());
        hexString = toHexString(buffer);
        SendMessage(hwndEdit, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
        SendMessage(hwndEdit, EM_REPLACESEL, 0, (LPARAM)hexString.c_str());
        buffer.clear();
        buffer.resize(chunkSize, 0);
    }
    delete params;
    return 0;
}

std::string toHexString(const std::vector<unsigned char>& data) {
    std::stringstream hexStream;
    hexStream << std::hex << std::setfill('0');
    for (auto byte : data) {
        hexStream << std::setw(2) << static_cast<int>(byte);
    }
    return hexStream.str();
}

void ClearEditControlContent() {
    SetWindowText(hwndEdit, "");
}

void ResetWindowForFileSelection(HWND hwnd) {
    ShowWindow(hwndEdit, SW_HIDE);
}
