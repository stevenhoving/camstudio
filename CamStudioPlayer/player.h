#pragma once

// Constants
#define BUFFER_LENGTH 256
#define WM_USER_PLAY 0x00401

// Function declarations
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void OpenMCIMovieFile(HWND hWnd);
void OpenMCIMovieFileInit(HWND hWnd);
void UpdateMenubar(HWND hWnd);
void UpdateTitle(HWND hWnd, LPSTR lpstrMovie);
//DLGPROC
//typedef INT_PTR(CALLBACK* DLGPROC)(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus);
SIZE GetPrimaryScreenSize();
POINT GetCenterCoords(int width, int height);
