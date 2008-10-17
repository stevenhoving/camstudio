// CamStudio Player Classic
//
// Version 1.0
// - Original version from RenderSoft
// Version 1.1 - 08 Oct 2008
// - Changed player so its window is 800x600 initially
// - Made player center to screen initially and when opening and closing videos
// - Fixed bug which forced title to be "Player" initially
// - Changed background color of window to black
// - Fixed bug which caused title of window to show "Playing AVI Movie" rather than the movie's filename
// - Cleaned up resources, added VS_VERSION_INFO, nicened up about dialog, deleted useless extra icon
// - Renamed player to CamStudio Player Classic
// - Removed a few magic numbers
// - Moved applicable string literals from the code into the string table
// - Moved some variables closer to their initial use
// - Formatted code nicely

#define INC_OLE2
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <commdlg.h>
#include <string.h>
#include <stdlib.h>
#include <direct.h>
#include <digitalv.h>
#include <vfw.h>
#include "player.h"
#include "resource.h"

// Globals
int defaultWidth = 800;			// Default width of the player window
int defaultHeight = 600;		// Default height of the player window
int miminumVfwVersion = 266;	// Minimum version of Video for Windows required by CSCP (266 = v1.1)
BOOL bIsOpenMovie = FALSE;		// Whether a movie is open
HWND hWndMCI;					// Movie window handle
HMENU hMenuBar = NULL;			// Menu bar handle
HINSTANCE m_hInstance = NULL;	// Instance of the player
char playfiledir[300];			// File that is currently being played

/*
 * Initialize stuff.
 */
HWND InitWindows(HINSTANCE hInstance, HINSTANCE hPrevInstance, int nCmdShow)
{
	// Window must be created with this title for the menu to initialize properly; use szAppName for the actual app title.
	// This is due to the menu in the resources having the ID "PLAYER". If you are changing one of these values, make sure to change both to the same thing.
	static char szAppNameInitial[] = "PLAYER";

	// First let's make sure we are running on 1.1
	if (HIWORD(VideoForWindowsVersion()) < miminumVfwVersion)
	{
		// Oops, we are too old, blow out of here
		MessageOut(NULL, IDS_STRING_VERSION, IDS_STRING_NOTE, MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	if (!hPrevInstance)
	{
		WNDCLASS wndclass;

		wndclass.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = WndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInstance;
		wndclass.hIcon = LoadIcon(hInstance, "APPICON");
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
		wndclass.lpszMenuName = szAppNameInitial;
		wndclass.lpszClassName = szAppNameInitial;

		if (!RegisterClass(&wndclass))
		{
			MessageOut(NULL, IDS_STRING_REGCLASS, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
			return NULL;
		}
	}

	int iWinHeight = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU) +	(GetSystemMetrics(SM_CYFRAME) * 2) + defaultHeight;

	// Create the main window for the application
	POINT location = GetCenterCoords(defaultWidth, defaultHeight);
	HWND hWnd = CreateWindow(szAppNameInitial, szAppNameInitial, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, location.x, location.y, defaultWidth, iWinHeight, NULL, NULL, hInstance, NULL);

	if (hWnd == NULL)
	{
		MessageOut(NULL, IDS_STRING_CREATEWIN, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
		return NULL;
	}

	hMenuBar = GetMenu(hWnd); // Get the menu bar handle
	UpdateMenubar(hWnd); // Update menu bar to disable Movie menu
	UpdateTitle(hWnd, NULL); // Update title to show correct title

	// Show the main window
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Create the movie window using MCIWnd that has no file open initially
	hWndMCI = MCIWndCreate(hWnd, hInstance, WS_CHILD | WS_VISIBLE | MCIWNDF_NOOPEN | MCIWNDF_NOERRORDLG | MCIWNDF_NOTIFYSIZE | MCIWNDF_SHOWMODE, NULL);

	if (!hWndMCI)
	{
		// We didn't get the movie window, destroy the app's window and bail out
		DestroyWindow(hWnd);
		return NULL;
	}

	MCIWndSetInactiveTimer(hWndMCI, 20);
	MCIWndSetActiveTimer(hWndMCI, 20);
	MCIWndSetRepeat(hWndMCI, FALSE);

	ShowWindow(hWndMCI, SW_HIDE);

	return hWnd;
}

/*
 * Main routine.
 */
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG msg;

	m_hInstance = hInstance;

	if ((hWnd = InitWindows(hInstance, hPrevInstance,nCmdShow)) == NULL)
	{
		return 0; // Died initializing, bail out
	}

	if (strlen(lpszCmdParam) != 0)
	{
		strcpy(playfiledir, lpszCmdParam);
		PostMessage(hWnd, WM_USER_PLAY, 0, 0);
	}

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

/*
 * Window process for the application.
 */
long FAR PASCAL WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	WORD w;
	WORD wMenu;
	RECT rc;

	switch (message)
	{
		case WM_USER_PLAY:
			OpenMCIMovieFileInit(hWnd);
			return 0;
		case WM_CREATE:
			return 0;
		case WM_INITMENUPOPUP:
			// Be sure this isn't the system menu
			if (HIWORD(lParam))
			{
				return DefWindowProc(hWnd, WM_INITMENUPOPUP, wParam, lParam);
			}

			wMenu = LOWORD(lParam);
			switch (wMenu)
			{
				// File menu
				case 0:
					// turn on/off CLOSE & PLAY
					if (bIsOpenMovie)
					{
						w = MF_ENABLED | MF_BYCOMMAND;
					}
					else
					{
						w = MF_GRAYED | MF_BYCOMMAND;
					}

					EnableMenuItem((HMENU)wParam, IDM_CLOSE, w);
					break;
			}
			break;
		case WM_COMMAND:
			switch (wParam)
			{
				/* File Menu */
				case ID_PLAY:
					MCIWndPlay(hWndMCI);
					break;
				case ID_STOP:
					MCIWndStop(hWndMCI);
					break;
				case IDM_OPEN:
					OpenMCIMovieFile(hWnd);
					break;
				case IDM_CLOSE:
					bIsOpenMovie = FALSE;
					MCIWndClose(hWndMCI); // close the movie
					ShowWindow(hWndMCI, SW_HIDE); // hide the window
					UpdateMenubar(hWnd);
					UpdateTitle(hWnd, NULL); // title bar back to plain
					break;
				case IDM_EXIT:
					PostMessage(hWnd, WM_CLOSE, 0, 0L);
					break;
				case IDM_ABOUT:
					DialogBox(GetWindowInstance(hWnd), MAKEINTRESOURCE(IDD_ABOUT), hWnd, AboutDlgProc);
					break;
			}

			return 0;
		case WM_PAINT:
			BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			return 0;
		case WM_SIZE:
			if (!IsIconic(hWnd) && hWndMCI && bIsOpenMovie)
			{
				MoveWindow(hWndMCI, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
			}
			break;
		case WM_DESTROY:
			if (bIsOpenMovie)
			{
				MCIWndClose(hWndMCI); // Close an open movie
			}

			MCIWndDestroy(hWndMCI); // Now destroy the MCIWnd window
			PostQuitMessage(0);
			return 0;
		case MCIWNDM_NOTIFYSIZE:
			if (!IsIconic(hWnd))
			{
				if (bIsOpenMovie)
				{
					// Adjust to size of the movie window
					GetWindowRect(hWndMCI, &rc);
					AdjustWindowRect(&rc, GetWindowLong(hWnd, GWL_STYLE), TRUE);
					POINT location = GetCenterCoords(rc.right - rc.left, rc.bottom - rc.top);
					SetWindowPos(hWnd, NULL, location.x, location.y, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
					MoveWindow(hWnd, location.x, location.y, rc.right - rc.left, rc.bottom - rc.top, true);
				}
				else
				{
					// Movie closed, adjust to the default size
					int iWinHeight = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU) + (GetSystemMetrics(SM_CYFRAME) * 2) + defaultHeight;
					POINT location = GetCenterCoords(defaultWidth, iWinHeight);
					SetWindowPos(hWnd, NULL, location.x, location.y, defaultWidth, iWinHeight, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
					MoveWindow(hWnd, location.x, location.y, defaultWidth, iWinHeight, true);
				}
			}

			break;
		case WM_ACTIVATE:
		case WM_QUERYNEWPALETTE:
		case WM_PALETTECHANGED:
			// Forward palette-related messages through to the MCIWnd, so it can do the right thing.
			if (hWndMCI)
			{
				return SendMessage(hWndMCI, message, wParam, lParam);
			}

			break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

/*
 * Updates the menu bar.
 */
void UpdateMenubar(HWND hWnd)
{
	WORD wp;

	if (bIsOpenMovie)
	{
		wp = MF_ENABLED;
	}
	else
	{
		wp = MF_GRAYED;
	}

	EnableMenuItem(hMenuBar, 1, MF_ENABLED | MF_BYPOSITION); // Change the Movie menu (#1)
	EnableMenuItem(hMenuBar, ID_PLAY, wp);
	DrawMenuBar(hWnd); // Re-draw the menu bar
}

/*
 * Updates the application title bar.
 */
void UpdateTitle(HWND hWnd, LPSTR lpstrMovie)
{
	char szAppName[BUFFER_LENGTH]; // space for the title
	char achNewTitle[BUFFER_LENGTH]; // space for the title
	LoadString(m_hInstance, IDS_STRING_APPTITLE, szAppName, BUFFER_LENGTH);

	if (lpstrMovie != NULL)
	{
		wsprintf((LPSTR)achNewTitle, "%s - %s", (LPSTR)szAppName, lpstrMovie);
	}
	else
	{
		lstrcpy((LPSTR)achNewTitle, (LPSTR)szAppName);
	}

	SetWindowText(hWnd, (LPSTR)achNewTitle);
}

/*
 * Open an AVI movie.
 */
void OpenMCIMovieFile(HWND hWnd)
{
	OPENFILENAME ofn;

	static char szFile[BUFFER_LENGTH];
	static char szFileTitle[BUFFER_LENGTH];

	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = "AVI Files\0*.avi\0\0";
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFileTitle = szFileTitle;
	ofn.nMaxFileTitle = sizeof(szFileTitle);
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn))
	{
		if (bIsOpenMovie)
		{
			MCIWndClose(hWndMCI);
		}

		bIsOpenMovie = TRUE; // Assume the best
		if (MCIWndOpen(hWndMCI, ofn.lpstrFile, 0) == 0)
		{
			ShowWindow(hWndMCI, SW_SHOW);
		}
		else
		{
			MessageOut(hWnd, IDS_STRING_NOOPEN, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
			bIsOpenMovie = FALSE;
		}
	}

	UpdateMenubar(hWnd);
	if (bIsOpenMovie)
	{
		UpdateTitle(hWnd, (LPSTR)ofn.lpstrFileTitle);
	}
	else
	{
		UpdateTitle(hWnd, NULL);
	}

	InvalidateRect(hWnd, NULL, FALSE);
	UpdateWindow(hWnd);
}

/*
 * Initialize the opened movie.
 */
void OpenMCIMovieFileInit(HWND hWnd)
{
	static char szFile[BUFFER_LENGTH];
	static char szFileTitle[BUFFER_LENGTH];

	if (bIsOpenMovie)
	{
		MCIWndClose(hWndMCI);
	}

	bIsOpenMovie = TRUE;
	if (MCIWndOpen(hWndMCI, playfiledir, 0) == 0)
	{
		ShowWindow(hWndMCI, SW_SHOW);
	}
	else
	{
		MessageOut(hWnd, IDS_STRING_NOOPEN, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
		bIsOpenMovie = FALSE;
	}

	UpdateMenubar(hWnd);
	UpdateTitle(hWnd, (LPSTR)playfiledir);

	InvalidateRect(hWnd, NULL, FALSE);
	UpdateWindow(hWnd);
}

/*
 * Dialog Procedure for the "about" dialog box.
 */
BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (msg)
	{
		case WM_COMMAND:
			if (wParam == 1055)
			{
				ShellExecute(NULL, "open", "http://www.camstudio.org/", NULL, NULL, SW_SHOW);
			}
			else
			{
				EndDialog(hwnd, TRUE);
				return TRUE;
			}

			break;
		case WM_INITDIALOG:
			return TRUE;
	}

	return FALSE;
}

/*
 * Displays a message box.
 */
int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus)
{
	if (m_hInstance)
	{
		char Msg_buffer[1000];
		char Title_buffer[1000];
		LoadString(m_hInstance, strTitle, Title_buffer, 1000);
		LoadString(m_hInstance, strMsg, Msg_buffer, 1000);

		return ::MessageBox(hWnd, Msg_buffer, Title_buffer, mbstatus);
	}

	return IDOK;
}

/*
 * Get the size of the screen.
 */
SIZE GetPrimaryScreenSize(void)
{
	SIZE s;

	s.cx = GetSystemMetrics(SM_CXFULLSCREEN);
	s.cy = GetSystemMetrics(SM_CYFULLSCREEN);

	return s;
}

/*
 * Gets the coordinates to center a window on-screen.
 */
POINT GetCenterCoords(int width, int height)
{
	SIZE s = GetPrimaryScreenSize();

	POINT p;
	p.x = (s.cx - width) / 2;
	p.y = (s.cy - height) / 2;

	return p;
}