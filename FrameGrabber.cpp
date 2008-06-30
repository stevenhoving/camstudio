// CFrameGrabber.cpp : implementation file
//
// (c) Vadim Gorbatenko, 1999 
// gvv@mail.tomsknet.ru
// All rights reserved
//
// CFrameGrabber window class
// Release: 1199

#include "stdafx.h"
#include "FrameGrabber.h"

#define	DEFAULT_CAPTURE_DRIVER	0 //in most cases

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "vfw32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef LRESULT (CALLBACK *FRAMECALLBACK)(HWND , LPVIDEOHDR);
#define	IMAGEWIDTH(lpd) ((LPBITMAPINFOHEADER)lpd)->biWidth
#define	IMAGEHEIGHT(lpd) ((LPBITMAPINFOHEADER)lpd)->biHeight
#define	IMAGEBITS(lpd) ((LPBITMAPINFOHEADER)lpd)->biBitCount

LRESULT PASCAL _grabber_CallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr);

CFrameGrabber	*theOnlyOneGrabber = NULL;
/////////////////////////////////////////////////////////////////////////////
// CFrameGrabber

CFrameGrabber::CFrameGrabber()
{
	dwLastCallback = NULL;
	imageData = NULL;
	vfs = 0;//offset image buffer
}

CFrameGrabber::~CFrameGrabber()
{
	if(imageData)
	{	HGLOBAL hg = GlobalHandle(imageData);
		GlobalUnlock(hg);
		GlobalFree(hg);}
}


BEGIN_MESSAGE_MAP(CFrameGrabber, CWnd)
	//{{AFX_MSG_MAP(CFrameGrabber)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFrameGrabber message handlers
// create unvisible child window at (x,y)
BOOL CFrameGrabber::Create(int x, int y, CWnd *pParentWnd)
{
	if(theOnlyOneGrabber)
		return FALSE;

	ASSERT(!GetSafeHwnd());
	
	if(GetSafeHwnd()) {
		return FALSE;		//already created
		}
	
	HWND hWnd=capCreateCaptureWindow(_T("AviCap_FrameGrabber_lite"),
        							WS_CHILD|WS_CLIPSIBLINGS, x, y, 160, 120, 
									pParentWnd?pParentWnd->GetSafeHwnd():NULL, 0xffff);
	if(!hWnd)
		return FALSE;
	
	//initial setup for capture
	if( !capDriverConnect(hWnd, DEFAULT_CAPTURE_DRIVER)||
		!capSetCallbackOnFrame(hWnd, _grabber_CallbackProc)||
		!capPreview(hWnd, FALSE))
	
	{::DestroyWindow(hWnd);
		return FALSE;}
	//subclass window
	SubclassWindow(hWnd);
	
	update_buffer_size();
	if(!imageData)
	{::DestroyWindow(hWnd);
		return FALSE;}
	theOnlyOneGrabber = this;
	return TRUE;
}

void CFrameGrabber::OnDestroy() 
{
	//disconnect from capture driver
	if(theOnlyOneGrabber) {
	capSetCallbackOnFrame(GetSafeHwnd(), NULL);
	capDriverDisconnect(GetSafeHwnd());
	}
	CWnd::OnDestroy();
	theOnlyOneGrabber = NULL;
}

BOOL	CFrameGrabber::VideoFormatDialog()
{
	if(!GetSafeHwnd())	return FALSE;
	BOOL r = (BOOL)capDlgVideoFormat(GetSafeHwnd());
	update_buffer_size();
	return r;
}

BOOL	CFrameGrabber::VideoSourceDialog()
{
	if(!GetSafeHwnd())	return FALSE;
	BOOL r =(BOOL)capDlgVideoSource(GetSafeHwnd());
	update_buffer_size();
	return r;
}

LPBITMAPINFO	CFrameGrabber::GetDIB()
{
	if(!imageData)	return NULL;
	//get the new one of more then 20 ms passed
	if((timeGetTime()-dwLastCallback)>20)
		capGrabFrameNoStop(GetSafeHwnd());
	return (LPBITMAPINFO)imageData;
}

LPBYTE			CFrameGrabber::GetImageBitsBuffer()
{
	if(!imageData)	return NULL;
	//get the new one of more then 20 ms passed
	if((timeGetTime()-dwLastCallback)>20)
		capGrabFrameNoStop(GetSafeHwnd());
	return ((LPBYTE)imageData)+vfs;
}

CSize		CFrameGrabber::GetImageSize()
{
	if(!imageData)	return CSize(0,0);
	else	return CSize(IMAGEWIDTH(imageData), IMAGEHEIGHT(imageData));
}

DWORD		CFrameGrabber::GetImageBitsResolution()
{
	if(!imageData)	return 0;
	else	return IMAGEBITS(imageData);
}


// this is internal use only!
BOOL	validCallHint=FALSE;
void	CFrameGrabber::SetImageData(LPVOID data)
{
	ASSERT(validCallHint);
	// do not call this method indirectly!
	if(!validCallHint)	return;
	if(!imageData)		return;
	CopyMemory(	((LPBYTE)imageData)+vfs,
				data, 
				IMAGEWIDTH(imageData)*IMAGEHEIGHT(imageData)*IMAGEBITS(imageData)/8);
	dwLastCallback	=  timeGetTime();
	if(IsWindowVisible())
		InvalidateRect(NULL);

}

void	CFrameGrabber::update_buffer_size()
{
	vfs=capGetVideoFormatSize(GetSafeHwnd());
		if(!vfs)
		{
			if(imageData)
				{HGLOBAL hg = GlobalHandle(imageData);
				GlobalUnlock(hg);
				GlobalFree(hg);}
				vfs = 0;
				imageData = NULL;
			return;		
		}

	DWORD	lastBufferSize=0;
	if(imageData)
		lastBufferSize = vfs +
		IMAGEWIDTH(imageData)*IMAGEHEIGHT(imageData)*IMAGEBITS(imageData)/8;
	
	DWORD newBufferSize;
	

	BITMAPINFO *lpBmpInfo =(BITMAPINFO	*)( new char[vfs]);
	
	((LPBITMAPINFOHEADER)lpBmpInfo)->biSize= sizeof BITMAPINFOHEADER;
	
	if(!capGetVideoFormat(GetSafeHwnd(), lpBmpInfo, (WORD)vfs))
	{
		delete lpBmpInfo;
		
		if(imageData)
			{HGLOBAL hg = GlobalHandle(imageData);
			GlobalUnlock(hg);
			GlobalFree(hg);}
			vfs = 0;
			imageData = NULL;
		return;		
	}

	newBufferSize = vfs + IMAGEWIDTH(lpBmpInfo)*IMAGEHEIGHT(lpBmpInfo)*IMAGEBITS(lpBmpInfo)/8;
	
	SetWindowPos(NULL,0,0,IMAGEWIDTH(lpBmpInfo),IMAGEHEIGHT(lpBmpInfo),
				SWP_NOMOVE|SWP_NOZORDER);

	if(newBufferSize > lastBufferSize)
	{
		if(imageData)//reallocate
		{	HGLOBAL hg = GlobalHandle(imageData);
			GlobalUnlock(hg);
			GlobalFree(hg);}

		imageData = GlobalLock(GlobalAlloc(GMEM_MOVEABLE, newBufferSize+256/*add 256 bytes, just in case...*/));
	}

	memcpy(imageData, lpBmpInfo, vfs);
	
	delete lpBmpInfo;
}

//////////////////////////////////////////////////////////////////
LRESULT PASCAL _grabber_CallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr)
{
	
	ASSERT_VALID(theOnlyOneGrabber);
	validCallHint = TRUE;
	theOnlyOneGrabber ->SetImageData(lpVHdr->lpData);
	validCallHint = FALSE;
	return 0;
}
