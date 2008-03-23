//Player.cpp

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
//#include <string.h>


// Globals
HWND hWndMCI;                 /* window handle of the movie */
BOOL bIsOpenMovie = FALSE;        /* Open flag: TRUE == movie open, FALSE = none */
HMENU hMenuBar = NULL;          /* menu bar handle */
char szAppName [] = "Player";
char playfiledir[300];	
//char seps[] = "*\t\n";
char seps[] = "*";
#define WM_USER_PLAY 0x00401
HINSTANCE m_hInstance = NULL;


// function declarations 
long FAR PASCAL WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void OpenMCIMovieFile(HWND hWnd);
void OpenMCIMovieFileInit(HWND hWnd);
void UpdateMenubar(HWND hWnd);
void UpdateTitle(HWND hWnd, LPSTR lpstrMovie);
BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus);


/********************************************************************
************************** FUNCTIONS ********************************
********************************************************************/


//InitWindows - initialize stuff
HWND InitWindows(HINSTANCE hInstance, HINSTANCE hPrevInstance, int nCmdShow)
{
        HWND            hWnd;   /* window handle to return */
        int             iWinHeight;
        WORD    wVer;

		

        /* first let's make sure we are running on 1.1 */
        wVer = HIWORD(VideoForWindowsVersion());
        if (wVer < 0x010a){
                /* oops, we are too old, blow out of here */
                MessageBeep(MB_ICONHAND);
                //MessageBox(NULL, "Video for Windows version is too old",
                //         "Error", MB_OK|MB_ICONSTOP);
				
				MessageOut(NULL,IDS_STRING_VERSION ,IDS_STRING_NOTE,MB_OK | MB_ICONSTOP);

                return FALSE;
        }


				
		
        if (!hPrevInstance){
                WNDCLASS    wndclass;

                wndclass.style         = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
                wndclass.lpfnWndProc   = WndProc;
                wndclass.cbClsExtra    = 0;
                wndclass.cbWndExtra    = 0;
                wndclass.hInstance     = hInstance;
                wndclass.hIcon         = LoadIcon (hInstance, "AppIcon");
                wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
                wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
                wndclass.lpszMenuName  = szAppName;
                wndclass.lpszClassName = szAppName;

                if (!RegisterClass(&wndclass)){
                        //MessageBox(NULL, "RegisterClass failure", szAppName, MB_OK);
						MessageOut(NULL,IDS_STRING_REGCLASS ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
                        return NULL;
                }
        }

        iWinHeight = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU) +
                        (GetSystemMetrics(SM_CYFRAME) * 2);

        /* create the main window for the app */
        hWnd = CreateWindow(szAppName, szAppName, WS_OVERLAPPEDWINDOW |
                WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, 180, iWinHeight,
                NULL, NULL, hInstance, NULL);

		 


        if (hWnd == NULL){
                //MessageBox(NULL, "CreateWindow failure", szAppName, MB_OK);
				MessageOut(NULL,IDS_STRING_CREATEWIN ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
                return NULL;
        }

        hMenuBar = GetMenu(hWnd);       /* get the menu bar handle */
        UpdateMenubar(hWnd);            /* update menu bar to disable Movie menu */

        /* Show the main window */
        ShowWindow(hWnd, nCmdShow);
        UpdateWindow(hWnd);

        /* create the movie window using MCIWnd that has no file open initially */
        hWndMCI = MCIWndCreate(hWnd, hInstance, WS_CHILD |WS_VISIBLE | MCIWNDF_NOOPEN |
                                MCIWNDF_NOERRORDLG | MCIWNDF_NOTIFYSIZE | MCIWNDF_SHOWMODE , NULL);
		
        if (!hWndMCI){
                /* we didn't get the movie window, destroy the app's window and bail out */
                DestroyWindow(hWnd);
                return NULL;
        }		
		
		MCIWndSetInactiveTimer(hWndMCI, 20);
		MCIWndSetActiveTimer(hWndMCI, 20);
		//MCIWndSetRepeat( hWndMCI, TRUE );
		MCIWndSetRepeat( hWndMCI, FALSE );

		ShowWindow(hWndMCI, SW_SHOW);		

        return hWnd;
}




//WinMain - main routine.                                       |
int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                                LPSTR lpszCmdParam, int nCmdShow)
{
        HWND        hWnd;
        MSG         msg;		

		m_hInstance = hInstance;

        if ((hWnd = InitWindows(hInstance, hPrevInstance,nCmdShow)) == NULL)
                return 0;       /* died initializing, bail out */

		if (strlen(lpszCmdParam)!=0) {
			/////////////////////////
			/*
			token=strtok(lpszCmdParam,seps);
			if (token==NULL) {
				//MessageBox(NULL,"Error in reading parameters. AVI Aborted","Error",MB_OK);
				exit(1);
			}
			strcpy(playfiledir,token);		
			//playfiledir[strlen(playfiledir)]=0;
			PostMessage(hWnd,WM_USER_PLAY,0,0);
			*/			

			strcpy(playfiledir,lpszCmdParam);
			
			//MessageBox(NULL,lpszCmdParam,"Note",MB_OK);
			//exit(0);
			
			PostMessage(hWnd,WM_USER_PLAY,0,0);
		}

		

        while (GetMessage(&msg, NULL, 0, 0)){
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }
        return msg.wParam;
}



//WndProc - window proc for the app                            
long FAR PASCAL WndProc (HWND hWnd, UINT message, WPARAM wParam,
                                                LPARAM lParam)
{
        PAINTSTRUCT ps;
        WORD w;
        WORD    wMenu;
        RECT    rc;

        switch (message){
				case WM_USER_PLAY :					
					OpenMCIMovieFileInit(hWnd);
					return 0;

                case WM_CREATE:
                        return 0;

                case WM_INITMENUPOPUP:
                        /* be sure this isn't the system menu */
                        if (HIWORD(lParam))
                                return DefWindowProc(hWnd, WM_INITMENUPOPUP,
                                                wParam, lParam);

                        wMenu = LOWORD(lParam);
                        switch (wMenu){
                                case 0:   /* file menu */
                                        /* turn on/off CLOSE & PLAY */
                                        if (bIsOpenMovie) w = MF_ENABLED|MF_BYCOMMAND;
                                        else            w = MF_GRAYED|MF_BYCOMMAND;
                                        EnableMenuItem((HMENU)wParam, IDM_CLOSE, w);
                                        break;
                        } /* switch */
                        break;

                case WM_COMMAND:
                        switch (wParam) {
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
                                        MCIWndClose(hWndMCI);         // close the movie
                                        ShowWindow(hWndMCI, SW_HIDE); //hide the window
                                        UpdateMenubar(hWnd);
                                        UpdateTitle(hWnd, NULL);     // title bar back to plain
                                        break;
                                case IDM_EXIT:
                                        PostMessage(hWnd, WM_CLOSE, 0, 0L);
                                        break;

                                
                                case IDM_ABOUT:
                                        DialogBox(GetWindowInstance(hWnd),
                                                  MAKEINTRESOURCE(IDD_ABOUT),
                                                  hWnd,
                                                  //(int (__stdcall *)(void)) AboutDlgProc);  //VC++ 5
												  AboutDlgProc); //VC++ 6
                                        break;

                        }
                        return 0;

                case WM_PAINT:
                        BeginPaint(hWnd, &ps);
                        EndPaint(hWnd, &ps);
                        return 0;

                case WM_SIZE:
                        if (!IsIconic(hWnd) && hWndMCI && bIsOpenMovie)
                                MoveWindow(hWndMCI,0,0,LOWORD(lParam),HIWORD(lParam),TRUE);
                        break;

                case WM_DESTROY:
                        if (bIsOpenMovie)
                                MCIWndClose(hWndMCI);  // close an open movie
                        MCIWndDestroy(hWndMCI);    // now destroy the MCIWnd window
                        PostQuitMessage(0);
                        return 0;

                case MCIWNDM_NOTIFYSIZE:
			if (!IsIconic(hWnd)) {
                            if (bIsOpenMovie){
                                /* adjust to size of the movie window */
                                GetWindowRect(hWndMCI, &rc);
                                AdjustWindowRect(&rc, GetWindowLong(hWnd, GWL_STYLE), TRUE);
                                SetWindowPos(hWnd, NULL, 0, 0, rc.right - rc.left,
                                        rc.bottom - rc.top,
                                        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
                            } else {
                                /* movie closed, adjust to the default size */
                                int iWinHeight;

                                iWinHeight = GetSystemMetrics(SM_CYCAPTION) +
                                                GetSystemMetrics(SM_CYMENU) +
                                                (GetSystemMetrics(SM_CYFRAME) * 2);
                                SetWindowPos(hWnd, NULL, 0, 0, 180, iWinHeight,
                                        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
			    }
                        }
                        break;

                case WM_ACTIVATE:
                case WM_QUERYNEWPALETTE:
                case WM_PALETTECHANGED:
                        //
                        // Forward palette-related messages through to the MCIWnd,
                        // so it can do the right thing.
                        //
                        if (hWndMCI)
                                return SendMessage(hWndMCI, message, wParam, lParam);
                        break;
        } /* switch */
        return DefWindowProc(hWnd, message, wParam, lParam);
}

void UpdateMenubar(HWND hWnd)
{
        WORD w,wp;

        if (bIsOpenMovie){
                w = MF_ENABLED|MF_BYPOSITION;
				wp = MF_ENABLED;		
        } else {
                //w = MF_GRAYED|MF_DISABLED|MF_BYPOSITION;
				w = MF_ENABLED|MF_BYPOSITION;
				wp = MF_GRAYED;		
        }

		
        EnableMenuItem(hMenuBar, 1, w); /* change the Movie menu (#1) */
		EnableMenuItem(hMenuBar, ID_PLAY, wp);
        DrawMenuBar(hWnd);      /* re-draw the menu bar */
}


void UpdateTitle(HWND hWnd, LPSTR lpstrMovie)
{
        char achNewTitle[BUFFER_LENGTH];        // space for the title

        if (lpstrMovie != NULL)
                wsprintf((LPSTR)achNewTitle,"%s - %s", (LPSTR)szAppName,lpstrMovie);
        else
                lstrcpy((LPSTR)achNewTitle, (LPSTR)szAppName);
        SetWindowText(hWnd, (LPSTR)achNewTitle);
}


// OpenMCIMovieFile - open an AVI movie. 
void OpenMCIMovieFile(HWND hWnd)
{

		OPENFILENAME ofn;

        static char szFile [BUFFER_LENGTH];
        static char szFileTitle [BUFFER_LENGTH];


        memset(&ofn, 0, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hWnd;
        ofn.lpstrFilter = "Video for Windows\0*.avi\0\0";
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFileTitle = szFileTitle;
        ofn.nMaxFileTitle = sizeof(szFileTitle);
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        
        if (GetOpenFileName(&ofn)){        
            
                if (bIsOpenMovie)
                        MCIWndClose(hWndMCI);

                
            
                bIsOpenMovie = TRUE;              // assume the best
				if (MCIWndOpen(hWndMCI, ofn.lpstrFile, 0) == 0){
                        ShowWindow(hWndMCI, SW_SHOW);
                } else {
            
                        //MessageBox(hWnd, "Unable to open Movie", NULL,MB_ICONEXCLAMATION|MB_OK);
						MessageOut(hWnd,IDS_STRING_NOOPEN ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
                        bIsOpenMovie = FALSE;
                }
        }


        UpdateMenubar(hWnd);
        if (bIsOpenMovie)
                UpdateTitle(hWnd, (LPSTR)ofn.lpstrFileTitle);
        else
                UpdateTitle(hWnd, NULL);

        // update  
        InvalidateRect(hWnd, NULL, FALSE);
        UpdateWindow(hWnd);
		
}



void OpenMCIMovieFileInit(HWND hWnd)
{

        static char szFile [BUFFER_LENGTH];
        static char szFileTitle [BUFFER_LENGTH];

        if (bIsOpenMovie)
                MCIWndClose(hWndMCI);


		//MCIWndSetActiveTimer(hWndMCI, 20);
        
 
        bIsOpenMovie = TRUE;              
        if (MCIWndOpen(hWndMCI, playfiledir, 0) == 0){
		
                ShowWindow(hWndMCI, SW_SHOW);
        } else {
        
                //MessageBox(hWnd, "Unable to open Movie", NULL,
                //      MB_ICONEXCLAMATION|MB_OK);
				MessageOut(hWnd,IDS_STRING_NOOPEN ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
                bIsOpenMovie = FALSE;
        }

    
        
        UpdateMenubar(hWnd);
        if (bIsOpenMovie)
                UpdateTitle(hWnd, (LPSTR)"Playing AVI Movie");
        else
                UpdateTitle(hWnd, NULL);

        
        InvalidateRect(hWnd, NULL, FALSE);
        UpdateWindow(hWnd);


}



/* AboutDlgProc()
 *
 * Dialog Procedure for the "about" dialog box.
 *
 */

BOOL CALLBACK AboutDlgProc(
        HWND    hwnd,
        UINT    msg,
        WPARAM  wParam,
        LPARAM  lParam)
{
        switch (msg) {
        case WM_COMMAND:
                EndDialog(hwnd, TRUE);
                return TRUE;
        case WM_INITDIALOG:
                return TRUE;
        }
        return FALSE;
}


int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus)
{
	//CString tstr("");
	//CString mstr("");

	if (m_hInstance) {

		char Msg_buffer[1000];
		char Title_buffer[1000];
		LoadString(m_hInstance,strTitle,Title_buffer,1000);
		LoadString(m_hInstance,strMsg , Msg_buffer,1000);

		//LoadString(hinst, IDS_APP_NAME, gszAppName, SIZEOF(gszAppName));
		
		//tstr.LoadString( strTitle );
		//mstr.LoadString( strMsg );

		return ::MessageBox(hWnd,Msg_buffer,Title_buffer,mbstatus);

	}

	return IDOK;

}

