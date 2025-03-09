#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // Link with Winsock library

#define DEFAULT_PORT 5000
#define MAX_CLIENTS 10

SOCKET serverSocket;
HWND startButton, stopButton, statusLabel, portLabel, portInput, ipLabel, ipInput, clientInfoLabel;
bool serverRunning = false;

void handleError(const char* message) {
    MessageBox(NULL, message, "Error", MB_OK | MB_ICONERROR);
    exit(EXIT_FAILURE);
}

DWORD WINAPI ServerThread(LPVOID lpParam) {
    WSADATA wsaData;
    struct sockaddr_in serverAddr, clientAddr;
    int addrLen = sizeof(clientAddr);
    SOCKET clientSocket;
    char portBuffer[10];
    GetWindowText(portInput, portBuffer, 10);
    int port = atoi(portBuffer);
    if (port == 0) port = DEFAULT_PORT;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        handleError("WSAStartup failed");
    }

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        handleError("Socket creation failed");
    }

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    // Bind socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        handleError("Bind failed");
    }

    // Listen for connections
    if (listen(serverSocket, MAX_CLIENTS) == SOCKET_ERROR) {
        handleError("Listen failed");
    }

    SetWindowText(statusLabel, "Server running...");

    // Accept client connection
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
    if (clientSocket == INVALID_SOCKET) {
        handleError("Accept failed");
    }

    char clientIP[INET_ADDRSTRLEN];
    strcpy(clientIP, inet_ntoa(clientAddr.sin_addr));
    std::string clientInfo = "Client connected: " + std::string(clientIP);
    SetWindowText(clientInfoLabel, clientInfo.c_str());

    // Cleanup
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    serverRunning = false;
    SetWindowText(statusLabel, "Server stopped.");
    SetWindowText(clientInfoLabel, "No clients connected.");

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        CreateWindow("STATIC", "Port:", WS_VISIBLE | WS_CHILD, 20, 20, 50, 20, hwnd, NULL, NULL, NULL);
        portInput = CreateWindow("EDIT", "5000", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            70, 20, 100, 20, hwnd, NULL, NULL, NULL);
        
        CreateWindow("STATIC", "IP:", WS_VISIBLE | WS_CHILD, 20, 50, 50, 20, hwnd, NULL, NULL, NULL);
        ipInput = CreateWindow("EDIT", "127.0.0.1", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            70, 50, 150, 20, hwnd, NULL, NULL, NULL);
        
        startButton = CreateWindow("BUTTON", "Start Server", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            20, 80, 120, 30, hwnd, (HMENU)1, NULL, NULL);
        stopButton = CreateWindow("BUTTON", "Stop Server", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            160, 80, 120, 30, hwnd, (HMENU)2, NULL, NULL);
        statusLabel = CreateWindow("STATIC", "Server not running", WS_VISIBLE | WS_CHILD,
            20, 120, 200, 30, hwnd, NULL, NULL, NULL);
        clientInfoLabel = CreateWindow("STATIC", "No clients connected.", WS_VISIBLE | WS_CHILD,
            20, 150, 250, 30, hwnd, NULL, NULL, NULL);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1 && !serverRunning) {
            serverRunning = true;
            CreateThread(NULL, 0, ServerThread, NULL, 0, NULL);
        }
        else if (LOWORD(wParam) == 2 && serverRunning) {
            closesocket(serverSocket);
            serverRunning = false;
            SetWindowText(statusLabel, "Server stopped.");
            SetWindowText(clientInfoLabel, "No clients connected.");
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "ChessServerWindow";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow("ChessServerWindow", "Chess Server", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 350, 230, NULL, NULL, hInstance, NULL);

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
