// Constants
#define BUFFER_LENGTH	256
#define WM_USER_PLAY	0x00401

// File menu items
#define IDM_OPEN   1
#define IDM_CLOSE  2
#define IDM_EXIT   3
#define IDM_ABOUT  4
#define IDD_ABOUT  101

// Function declarations
long FAR PASCAL WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void OpenMCIMovieFile(HWND hWnd);
void OpenMCIMovieFileInit(HWND hWnd);
void UpdateMenubar(HWND hWnd);
void UpdateTitle(HWND hWnd, LPSTR lpstrMovie);
BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus);
SIZE GetPrimaryScreenSize(void);
POINT GetCenterCoords(int width, int height);

// Enable Windows XP visual styles
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
