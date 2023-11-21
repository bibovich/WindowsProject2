#include <windows.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

// Function to perform XOR-based encryption and decryption
std::string EncryptDecrypt(std::string& input) {
    const char key = 'K'; // XOR key
    for (auto& c : input)
        c ^= key;

    return input;
}

// Function to encrypt a file using XOR-based encryption
void EncryptFile(const fs::path& filePath) {
    // Open the input file for reading in binary mode
    std::ifstream inputFile(filePath, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file for encryption: " << filePath << std::endl;
        return;
    }

    // Read the content of the file into a string
    std::string fileContent((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    inputFile.close();

    // Encrypt the content using the EncryptDecrypt function
    std::string encryptedContent = EncryptDecrypt(fileContent);

    // Open the output file for writing in binary mode
    std::ofstream outputFile(filePath, std::ios::binary | std::ios::trunc);
    if (!outputFile.is_open()) {
        std::cerr << "Error opening file for writing: " << filePath << std::endl;
        return;
    }

    // Write the encrypted content back to the file
    outputFile.write(encryptedContent.c_str(), encryptedContent.size());
    outputFile.close();
}

// Function to decrypt a file using XOR-based encryption (since XOR is symmetric)
void DecryptFile(const fs::path& filePath) {
    EncryptFile(filePath);
}

// Window Procedure callback function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        // Create "Cipher" button
        CreateWindow(_T("BUTTON"), _T("Cipher"), WS_VISIBLE | WS_CHILD,
            200, 50, 100, 30, hwnd, (HMENU)1, NULL, NULL);
        break;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == 1) { // Cipher button clicked
            // Open File Dialog
            OPENFILENAME ofn;
            TCHAR szFile[260];

            // Configure the OPENFILENAME structure (ofn) for the file open dialog:
            // Initialize to zero, set structure size, specify the owner window,
            // provide buffer for the file name, set filter for file types,
            // and define flags for path and file existence checks.
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = szFile;
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = _T("All Files\0*.*\0WAV Files\0*.wav\0JPG Files\0*.jpg\0MP3 Files\0*.mp3\0");
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            // Get the selected file and perform encryption/decryption
            if (GetOpenFileName(&ofn)) {
                DecryptFile(ofn.lpstrFile);
                MessageBox(hwnd, _T("File encrypted/decrypted successfully."), _T("Cipher"), MB_OK | MB_ICONINFORMATION);
            }
        }
        break;
    }
    case WM_DESTROY: {
        // Quit the application
        PostQuitMessage(0);
        break;
    }
    default: {
        // Default window procedure for other messages
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    }
    return 0;
}

// Entry point for the Windows application
_Use_decl_annotations_ int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    // Window class registration
    const TCHAR* CLASS_NAME = _T("EncryptDecryptApp");
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    // Window creation
    HWND hwnd = CreateWindowEx(0, CLASS_NAME, _T("Encrypt/Decrypt App"), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
        return 0;

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
