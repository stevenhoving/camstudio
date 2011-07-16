/////////////////////////////////////////////////////////////////////////////
//
// TransparentWindow.CPP
//
// A transparent window class.
//
// Based on the idea of Jason Wylie
//
// (C) 1998 by Franz Polzer
//
// Visit me at: stud1.tuwien.ac.at/~e9225140
// Write to me: e9225140@student.tuwien.ac.at
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Recorder.h"
#include "TransparentWnd.h"
#include "MainFrm.h"			// for maxxScreen, maxyScreen
#include "RecorderView.h"
#include "MouseCaptureWnd.h"
#include "resource.h"

#include "TextDialog.h"
#include "EditTransparency.h"
#include "EditImage.h"

#include "ListManager.h"
#include "ScreenAnnotations.h"
#include "ResizeDialog.h"
#include "CStudioLib.h"

#include <windowsx.h>
#include <Gdiplus.h>
using namespace Gdiplus;

// WTF???! Can this life be easier?
// works both ways
// TODO: move it somewhere
__inline __declspec(naked) DWORD COLORREFtoARGB(COLORREF, BYTE)
{
	__asm
	{
		mov eax, DWORD PTR 4[esp]
		bswap eax
		mov al, BYTE PTR 8[esp]
		rcr eax, 8
		ret
	}
}

extern CScreenAnnotationsDlg sadlg;

extern HANDLE Bitmap2Dib(HBITMAP, UINT);

long lCurrentWndID = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//important: this window does not create or delete the m_hbitmap passed to it

//********************************************************************************
//* Constructor
//********************************************************************************

CTransparentWnd::CTransparentWnd()
:	m_uniqueID(lCurrentWndID++)
	, m_saveMethod(saveMethodNew)
	, m_textstring("Right Click to Edit Text")
	, m_rgb(RGB(0,0,0))
	, m_factor(2)
	, m_horzalign(DT_CENTER)
	, m_charset(ANSI_CHARSET)
	, m_bTrackingOn(false)
	, m_bEditTransOn(false)
	, m_bEditImageOn(false)
	, m_benable(0)
	, m_valueTransparency(50)
	, m_menuLoaded(0)
	, m_movewindow(0)
	, m_regionCreated(0)
	, m_transparentColor(RGB(0,0,0))
	, m_regionType(regionNULL)
	, m_hbitmap(0)
	, m_regionPredefinedShape(regionROUNDRECT)
	, m_roundrectFactor(0.2)
	, m_bBorderYes(false)
	, m_borderColor(RGB(0,0,128))
	, m_borderSize(5)
	, m_backgroundColor(RGB(255,255,255))
	, m_shapeStr("Label")
	, m_baseType(0)
	, m_widthPos(32)
	, m_heightPos(32)
	, m_rectOriginalWnd(m_rectWnd)
{
	if (lCurrentWndID > 2147483647)
		lCurrentWndID = 0;

	ZeroMemory(&m_textfont, sizeof(LOGFONT));
	m_textfont.lfHeight = 12;
	m_textfont.lfWidth = 8;
	strcpy_s(m_textfont.lfFaceName,"Arial");

	m_tracker.m_rect.left = 20;
	m_tracker.m_rect.top = 20;
	m_tracker.m_rect.right = 160;
	m_tracker.m_rect.bottom = 140;

	m_tracker.m_nStyle = CRectTracker::dottedLine | CRectTracker::resizeOutside;
	m_tracker.m_sizeMin.cx = 64;
	m_tracker.m_sizeMin.cx = 48;

	m_rectWnd.left =0;
	m_rectWnd.top = 0;
	m_rectWnd.right =180;
	m_rectWnd.bottom = 160;
}

//********************************************************************************
//* Destructor
//********************************************************************************

CTransparentWnd::~CTransparentWnd()
{
}

BEGIN_MESSAGE_MAP(CTransparentWnd, CWnd)
	//{{AFX_MSG_MAP(CTransparentWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CONTEXT_CLOSEALL, OnContextCloseall)
	ON_COMMAND(ID_CONTEXT_CLOSE, OnContextClose)
	ON_COMMAND(ID_CONTEXT_SAVELAYOUT, OnContextSaveLayout)
	ON_COMMAND(ID_CONTEXT_RESIZE, OnContextResize)
	ON_COMMAND(ID_CONTEXT_REFRESH, OnContextRefresh)
	ON_COMMAND(ID_CONTEXT_EDITTEXT, OnContextEditText)
	ON_COMMAND(ID_CONTEXT_SAVE, OnContextSave)
	ON_COMMAND(ID_CONTEXT_EDITTRANSPARENCY, OnContextEditTransparency)
	ON_COMMAND(ID_CONTEXT_ANTIALIAS_NOANTIALIAS, OnContextNoAntiAlias)
	ON_COMMAND(ID_CONTEXT_ANTIALIAS_ANTIALIASX2, OnContextAntiAlias2)
	ON_COMMAND(ID_CONTEXT_ANTIALIAS_ANTIALIASX3SLOWEST, OnContextAntiAlias3)
	ON_COMMAND(ID_CONTEXT_EDITIMAGE, OnContextEditImage)
	ON_COMMAND(ID_CONTEXT_CLONE, OnContextClone)
	//}}AFX_MSG_MAP
//	ON_REGISTERED_MESSAGE (WM_USER_INVALIDATEWND, OnInvalidateWnd)
END_MESSAGE_MAP()

//********************************************************************************
//* CreateTransparent()
//*
//* Creates the main application window transparent
//********************************************************************************

BOOL CTransparentWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	// the CREATESTRUCT cs

	//This line here prevents taskbar buttons from appearing for each TransparentWnd
	//cs.hwndParent = hWndGlobal; //this will cause the text tracker unable to move/resize
	cs.hwndParent = hMouseCaptureWnd;

	return CWnd::PreCreateWindow(cs);
}

void CTransparentWnd::CreateTransparent(LPCTSTR pTitle, RECT rect, HBITMAP BitmapID)
{
	CreateEx( WS_EX_TOPMOST ,
		AfxRegisterWndClass(0),
		pTitle,
		WS_POPUP | WS_SYSMENU,
		rect,
		NULL,
		NULL,
		NULL );

	m_rectWnd = rect;

	//WidthHeight
	m_rectOriginalWnd = m_rectWnd;

	if ((m_rectWnd.Width()>60) && (m_rectWnd.Height()>60)) {
		m_tracker.m_rect.left = 20;
		m_tracker.m_rect.top = 20;
		m_tracker.m_rect.right = rect.right - rect.left - 20;
		m_tracker.m_rect.bottom = rect.bottom - rect.top - 20;
	} else {
		m_tracker.m_rect.left = 1;
		m_tracker.m_rect.top = 1;
		m_tracker.m_rect.right = m_rectWnd.Width() - 2;
		m_tracker.m_rect.bottom = m_rectWnd.Height() - 2;
	}

	m_hbitmap = BitmapID;

	SetupRegion();
}

void CTransparentWnd::CreateTransparent(LPCTSTR pTitle, RECT rect, CString bitmapFile, int fitBitmapSize)
{
	if (m_picture.Load(bitmapFile)) {
		HBITMAP testtrans;
		if (m_picture.IPicturePtr()->get_Handle( (unsigned int *) &testtrans ) == S_OK ) {
			m_hbitmap = testtrans;

			if (fitBitmapSize) {
				rect.right = rect.left + m_picture.Width() - 1;
				rect.bottom = rect.top + m_picture.Height() - 1;
			}
		}
	}

	m_rectWnd = rect;

	//WidthHeight
	m_rectOriginalWnd = m_rectWnd;

	CreateEx( WS_EX_TOPMOST, AfxRegisterWndClass(0), pTitle, WS_POPUP | WS_SYSMENU, rect, NULL, NULL, NULL);

	if ((m_rectWnd.Width() > 60) && (m_rectWnd.Height() > 60)) {
		m_tracker.m_rect.left = 20;
		m_tracker.m_rect.top = 20;
		m_tracker.m_rect.right = rect.right - rect.left - 20;
		m_tracker.m_rect.bottom = rect.bottom - rect.top - 20;
	} else {
		m_tracker.m_rect.left = 1;
		m_tracker.m_rect.top = 1;
		m_tracker.m_rect.right = m_rectWnd.Width() - 2;
		m_tracker.m_rect.bottom = m_rectWnd.Height() - 2;
	}

	SetupRegion();
}

void CTransparentWnd::SetupRegion(CDC *pDC)
{
	if (m_regionType == regionNULL) {
		m_regionCreated = 0;
		SetWindowRgn((HRGN)NULL, TRUE);
	} else if (m_regionType == regionTRANSPARENTCOLOR) {
		SetupRegionByTransColor(pDC,m_transparentColor);
	} else if (m_regionType == regionSHAPE) {
		m_regionCreated = 0;

		CRect cRect;
		cRect = m_rectWnd;

		if (m_wndRgn.m_hObject) {
			m_wndRgn.DeleteObject();
		}

		//trace rectwnd

		double rx = cRect.Width() * m_roundrectFactor;
		double ry = cRect.Height() * m_roundrectFactor;
		double rval = (rx > ry) ? ry : rx;

		if (m_regionPredefinedShape == regionROUNDRECT)
			m_wndRgn.CreateRoundRectRgn(0, 0, cRect.Width()-1, cRect.Height()-1, (int) rval, (int) rval);
		else if (m_regionPredefinedShape == regionELLIPSE)
			m_wndRgn.CreateEllipticRgn( 0, 0, cRect.Width()-1, cRect.Height()-1);
		else
			m_wndRgn.CreateRectRgn(0, 0, cRect.Width()-1, cRect.Height()-1);

		SetWindowRgn((HRGN)m_wndRgn, TRUE);
	}
}

//********************************************************************************
//* SetupRegion()
//*
//* Set the Window Region for transparancy outside the mask region
//********************************************************************************

void CTransparentWnd::SetupRegionByTransColor(CDC *pDC, COLORREF transColor)
{
	if (m_regionCreated) {
		//this line here block....the creation of regon under certain conditions
		//SetWindowRgn((HRGN)wndRgn, TRUE);
		return;
	}

	if (!m_hbitmap)
		return;

	CBitmap cBitmap;
	CBitmap zoomBitmap;
	CBitmap * old_zoomBitmap = 0;
	HBITMAP pOldMemBmp = NULL;
	COLORREF col;

	//need to make wndRgn a member...???
	CRgn rgnTemp;

	CRect cRect;
	GetWindowRect(&cRect);

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	//if window is zoomed, create a resized-bitmap for computation
	int needCleanZoom = 0;
	if ((m_rectWnd.Width() != m_rectOriginalWnd.Width())
		|| (m_rectWnd.Height() != m_rectOriginalWnd.Height())) {
		zoomBitmap.CreateCompatibleBitmap(pDC,cRect.Width(),cRect.Height());
		old_zoomBitmap = (CBitmap *) memDC.SelectObject(&zoomBitmap);

		CDC tempDC;
		tempDC.CreateCompatibleDC(pDC);
		HBITMAP pOldMemBmp = (HBITMAP) ::SelectObject(tempDC.m_hDC,m_hbitmap);
		memDC.StretchBlt(0, 0, cRect.Width(), cRect.Height(), &tempDC, 0, 0, m_rectOriginalWnd.Width(), m_rectOriginalWnd.Height(), SRCCOPY);

		::SelectObject(tempDC.m_hDC,pOldMemBmp);
		tempDC.DeleteDC();

		needCleanZoom = 1;
	} else {
		pOldMemBmp = (HBITMAP) ::SelectObject(memDC.m_hDC, m_hbitmap);
	}

	if (m_wndRgn.m_hObject) {
		m_wndRgn.DeleteObject();
	}
	m_wndRgn.CreateRectRgn(0, 0, cRect.Width(), cRect.Height());

	BOOL bStart = FALSE;
	int yStart = 0;
	for (int x = 0; x <= cRect.Width(); x++) {
		int y = 0;
		for (y = 0; y<=cRect.Height(); y++) {
			col = memDC.GetPixel(x, y);
			if (col == transColor) {
				if (!bStart) {
					yStart = y;
					bStart = TRUE;
				}
			} else {
				if (bStart) {
					rgnTemp.CreateRectRgn(x, yStart, x+1, y);
					m_wndRgn.CombineRgn(&m_wndRgn, &rgnTemp, RGN_DIFF);
					rgnTemp.DeleteObject();
				}
				bStart = FALSE;
			}
		}
		// End of column
		if (bStart) {
			rgnTemp.CreateRectRgn(x, yStart, x+1, y);
			m_wndRgn.CombineRgn(&m_wndRgn, &rgnTemp, RGN_XOR);
			rgnTemp.DeleteObject();
			bStart=FALSE;
		}
	}

	if (needCleanZoom) {
		memDC.SelectObject(old_zoomBitmap);
		zoomBitmap.DeleteObject();
	} else {
		::SelectObject(memDC.m_hDC,pOldMemBmp);
	}

	memDC.DeleteDC();

	SetWindowRgn((HRGN)m_wndRgn, TRUE);

	m_regionCreated = 1;
}

//********************************************************************************
//* CTransparentWnd message handlers
//********************************************************************************

BOOL CTransparentWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CTransparentWnd::OnPaint()
{
	CPaintDC dc(this);

	// Add your drawing code here!
	CDC *pDC = &dc;

	//WIDTHHEIGHT
	CRect clrect;
	//GetClientRect(&clrect);
	clrect = m_rectWnd;
	clrect.right -= clrect.left;
	clrect.bottom -= clrect.top;
	clrect.left = 0;
	clrect.top = 0;

//	int width = clrect.right - clrect.left;
//	int height = clrect.bottom - clrect.top;

	//BitBlt Background
	CRect rect;
	CRect cRect;
	GetWindowRect(&rect);
	cRect = rect;

	if (m_hbitmap) {
		CDC memDC;
		HBITMAP pOldMemBmp = NULL;
		memDC.CreateCompatibleDC(pDC);
		pOldMemBmp = (HBITMAP) ::SelectObject(memDC.m_hDC,m_hbitmap);

		//WidthHeight
		//pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
		pDC->StretchBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, m_rectOriginalWnd.Width(), m_rectOriginalWnd.Height(), SRCCOPY);
		::SelectObject(memDC.m_hDC,pOldMemBmp);
		memDC.DeleteDC();
	} else {
		pDC->FillSolidRect(0,0,rect.Width(),rect.Height(),m_backgroundColor);
	}

	//if ((m_factor == 1) || m_bTrackingOn) {
	if (true) {
		CFont dxfont;
		dxfont.CreateFontIndirect(&m_textfont);
		CFont* oldfont = (CFont *) pDC->SelectObject(&dxfont);

		int textlength = m_textstring.GetLength(); //get number of bytes

		//Draw Text
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(m_rgb);
		//pDC->DrawText((char *)LPCTSTR(m_textstring), textlength, &m_tracker.m_rect, m_horzalign | DT_VCENTER | DT_WORDBREAK );

		// TODO: this is just a dirty fix to use GDI+
		HDC hdc = pDC->GetSafeHdc();
		Graphics g(hdc);
		switch(m_factor) {
			case 1:
				g.SetTextRenderingHint(TextRenderingHintSystemDefault);
				break;
			case 2:
				g.SetTextRenderingHint(TextRenderingHintAntiAlias);
				break;
			case 3:
				g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
				break;
		}
		Font f(hdc);
		StringFormat sf;
		sf.SetAlignment((Gdiplus::StringAlignment)m_horzalign); // happen to coincide with left, center, right
		sf.SetLineAlignment(StringAlignmentCenter); // for DT_VCENTER
		size_t size = 0;
		wchar_t wstr[1024];
		mbstowcs_s(&size,wstr,1023,m_textstring,_TRUNCATE);
		size_t wlen = wcsnlen_s(wstr,1023);
		RectF r(m_tracker.m_rect.left,m_tracker.m_rect.top,m_tracker.m_rect.Width(),m_tracker.m_rect.Height());
		Color c((ARGB)COLORREFtoARGB(pDC->GetTextColor(),255));
		SolidBrush b(c);
		g.DrawString(wstr, wlen, &f, r, &sf, &b);
		// end of GDI+ fix

		//DrawTextW(pDC->m_hDC, (unsigned short *)LPCTSTR(m_textstring), textlength, &m_tracker.m_rect, m_horzalign | DT_VCENTER | DT_WORDBREAK );

		//if (oldfont)
		pDC->SelectObject(oldfont);

		if ((m_bBorderYes) && (regionSHAPE == m_regionType)) {
			double rx = cRect.Width() * m_roundrectFactor;
			double ry = cRect.Height() * m_roundrectFactor;
			double rval = (rx>ry) ? ry : rx;

			CPen borderPen;
			borderPen.CreatePen( PS_SOLID , m_borderSize, m_borderColor );
			CPen* oldPen = (CPen *) pDC->SelectObject(&borderPen);

			LOGBRUSH logbrush;
			logbrush.lbStyle = BS_HOLLOW;
			CBrush borderBrush;
			borderBrush.CreateBrushIndirect(&logbrush);
			CBrush* oldBrush = (CBrush *) pDC->SelectObject(&borderBrush);

			int drawOffset = m_borderSize/2;
			if (m_regionPredefinedShape == regionROUNDRECT) {
				pDC->RoundRect(drawOffset-1, drawOffset-1, cRect.Width()-1-drawOffset, cRect.Height()-1-drawOffset, (int) rval, (int) rval);
			} else if (m_regionPredefinedShape == regionELLIPSE) {
				pDC->Ellipse(drawOffset-1,drawOffset-1, cRect.Width()-1-drawOffset, cRect.Height()-1-drawOffset);
			} else if (m_regionPredefinedShape == regionRECTANGLE) {
				pDC->Rectangle(drawOffset-1, drawOffset-1, cRect.Width()-1-drawOffset, cRect.Height()-1-drawOffset);
			}

			pDC->SelectObject(oldBrush);
			pDC->SelectObject(oldPen);
			borderPen.DeleteObject();
			borderBrush.DeleteObject();
		}
	} else {
		//LPBITMAPINFO pbmiText = GetTextBitmap(pDC, &CRect(clrect),m_factor,&m_tracker.m_rect, &m_textfont, m_textstring, NULL, NULL, rgb, m_horzalign);
		//HBITMAP newbm = DrawResampleRGB(pDC, &CRect(clrect), m_factor, (LPBITMAPINFOHEADER) pbmiText);
		LPBITMAPINFO pbmiText = GetTextBitmap(pDC, &clrect, m_factor, &m_tracker.m_rect, &m_textfont, m_textstring, NULL, NULL, m_rgb, m_horzalign);
//		HBITMAP newbm = DrawResampleRGB(pDC, &clrect, m_factor, (LPBITMAPINFOHEADER) pbmiText);

		if (pbmiText) {
			GlobalFreePtr(pbmiText);
			pbmiText = NULL;
		}
	}

	if (m_bTrackingOn) {
		m_tracker.Draw(pDC);
	}
}

void CTransparentWnd::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	//not very stable when editing is on
	EnsureOnTopList(this);

	bool bIsEdited = AreWindowsEdited();

	if (m_menuLoaded == 0) {
		m_menu.LoadMenu(IDR_CONTEXTMENU);
		m_menuLoaded = 1;
	}

	CMenu* pPopup = m_menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	if (bIsEdited) {
		DisableContextMenu();
	} else {
		OnUpdateContextMenu();
	}

	// route commands through main window
	pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN, point.x, point.y, this);
}

void CTransparentWnd::OnContextSaveLayout()
{
	sadlg.SaveLayoutNew();
}

void CTransparentWnd::OnContextSave()
{
	if (m_saveMethod == saveMethodNew) {
		sadlg.SaveShapeNew(this);
	} else {
		sadlg.SaveShapeReplace(this);
	}
}

void CTransparentWnd::OnContextEditText()
{
	EditText();
}

void CTransparentWnd::EditText()
{
	m_bTrackingOn = true;

	//WidthHeight
	SetWindowRgn((HRGN)NULL, TRUE);

	Invalidate();

	CTextDlg txdlg;
	txdlg.PreModal(&m_textstring, &m_textfont, &m_rgb, this, &m_horzalign);
	txdlg.DoModal();

	m_bTrackingOn = false;

	OnUpdateContextMenu();

	//WidthHeight
	//All code areas with SetWindowRgn((HRGN)NULL, TRUE) will need m_regionCreated = 0 ?
	m_regionCreated = 0;
	InvalidateRegion();
	Invalidate();
}

LPBITMAPINFO CTransparentWnd::GetDCBitmap(CDC *thisDC, CRect* caprect)
{
 //	int left =caprect->left;
//	int top = caprect->top;
	int width = caprect->Width();
	int height = caprect->Height();

	HDC hMemDC = ::CreateCompatibleDC(thisDC->m_hDC);
	HBITMAP hbm;

	hbm = CreateCompatibleBitmap(thisDC->m_hDC, width, height);
	HBITMAP oldbm = (HBITMAP) SelectObject(hMemDC, hbm);

	SelectObject(hMemDC,oldbm);
	LPBITMAPINFOHEADER pBM_HEADER = (LPBITMAPINFOHEADER)GlobalLock(Bitmap2Dib(hbm, 24));

	if (pBM_HEADER == NULL) {
		return NULL;
	}

	DeleteObject(hbm);
	DeleteDC(hMemDC);

	return (LPBITMAPINFO) pBM_HEADER;
}

//caprect and drawtextrect in normal coordinates
LPBITMAPINFO CTransparentWnd::GetTextBitmap(CDC *thisDC, CRect* caprect,int factor,CRect* drawtextRect, LOGFONT* drawfont, CString textstr, CPen* pPen, CBrush * pBrush, COLORREF textcolor, int horzalign)
{
	int left =caprect->left;
	int top = caprect->top;
	int width = caprect->Width();
	int height = caprect->Height();
	int orig_width = width;
	int orig_height = height;

	width *= factor;
	height *= factor;

	CRect usetextRect = *drawtextRect;
	usetextRect.left *= factor;
	usetextRect.top *= factor;
	usetextRect.right *= factor;
	usetextRect.bottom *= factor;

	HDC hMemDC = ::CreateCompatibleDC(thisDC->m_hDC);
	CDC* pMemDC = CDC::FromHandle(hMemDC);
	HBITMAP hbm = CreateCompatibleBitmap(thisDC->m_hDC, width, height);
	HBITMAP oldbm = (HBITMAP) SelectObject(hMemDC, hbm);

	//Get Background
	::StretchBlt(hMemDC, 0, 0, width, height, thisDC->m_hDC, left, top, orig_width,orig_height,SRCCOPY);

	CFont dxfont;
	CFont *oldfont	= (CFont*)0;
	if (drawfont) {
		LOGFONT newlogFont = *drawfont;
		newlogFont.lfWidth *= factor;
		newlogFont.lfHeight *= factor;

		//need CreateFontIndirectW ?
		dxfont.CreateFontIndirect(&newlogFont);
		oldfont = (CFont *) pMemDC->SelectObject(&dxfont);
	}
	//if no default font is selected, can cause errors
	CPen* oldPen = (CPen*)0;
	if (pPen)
		oldPen = pMemDC->SelectObject(pPen);

	CBrush* oldBrush = (CBrush*)0;
	if (pBrush)
		oldBrush = pMemDC->SelectObject(pBrush);

	int textlength = textstr.GetLength(); //get number of bytes

	//Draw Text

	SetBkMode(hMemDC,TRANSPARENT);
	SetTextColor(hMemDC,textcolor);
	//DrawTextEx(hMemDC, (char *)LPCTSTR(textstr), textlength, LPRECT(usetextRect), horzalign | DT_VCENTER | DT_WORDBREAK | DT_EDITCONTROL , NULL);

	//use adaptive antialias...if size< than maxxScreen maxyScreen
	CRecorderApp *pApp = (CRecorderApp *)AfxGetApp();
	if ((pApp->VersionOp() >= 5) && ((usetextRect.Width() > maxxScreen) || (usetextRect.Height() > maxyScreen))) {
		//use stroke path method, less buggy

		BeginPath(hMemDC);
		DrawTextEx(hMemDC, (char *)LPCTSTR(textstr), textlength, LPRECT(usetextRect), horzalign | DT_VCENTER | DT_WORDBREAK , NULL);
		EndPath(hMemDC);

		//CPen testpen;
		//testpen.CreatePen(PS_SOLID,0,textcolor);
		//oldPen = pMemDC->SelectObject(&testpen);

		CBrush testbrush;
		testbrush.CreateSolidBrush(textcolor);
		oldBrush = pMemDC->SelectObject(&testbrush);
		//StrokeAndFillPath(hMemDC);
		FillPath(hMemDC);
		pMemDC->SelectObject(oldBrush);
		//pMemDC->SelectObject(oldPen);

		testbrush.DeleteObject();
		//testpen.DeleteObject();
	} else {
		DrawTextEx(hMemDC, (char *)LPCTSTR(textstr), textlength, LPRECT(usetextRect), horzalign | DT_VCENTER | DT_WORDBREAK , NULL);
	}

	if ((m_bBorderYes) && (regionSHAPE == m_regionType)) {
		CRect cRect;
		cRect.left = left;
		cRect.top = top;
		cRect.right = cRect.left + width - 1;
		cRect.bottom = cRect.top + height - 1;

		double rx = cRect.Width() * m_roundrectFactor;
		double ry = cRect.Height() * m_roundrectFactor;
		double rval = (rx>ry) ? ry : rx;

		CPen borderPen;
		borderPen.CreatePen( PS_SOLID , m_borderSize*m_factor, m_borderColor );
		CPen* oldPen = (CPen *) pMemDC->SelectObject(&borderPen);

		LOGBRUSH logbrush;
		logbrush.lbStyle = BS_HOLLOW;
		CBrush borderBrush;
		borderBrush.CreateBrushIndirect(&logbrush);
		CBrush* oldBrush = (CBrush *) pMemDC->SelectObject(&borderBrush);

		int drawOffset = (m_borderSize*m_factor)/2;

		if (m_regionPredefinedShape == regionROUNDRECT) {
			pMemDC->RoundRect(drawOffset-1, drawOffset-1, cRect.Width()-1-drawOffset, cRect.Height()-1-drawOffset, (int) rval, (int) rval);
		} else if (m_regionPredefinedShape == regionELLIPSE) {
			pMemDC->Ellipse(drawOffset-1,drawOffset-1, cRect.Width()-1-drawOffset, cRect.Height()-1-drawOffset);
		} else if (m_regionPredefinedShape == regionRECTANGLE) {
			pMemDC->Rectangle(drawOffset-1, drawOffset-1, cRect.Width()-1-drawOffset, cRect.Height()-1-drawOffset);
		}

		pMemDC->SelectObject(oldBrush);
		pMemDC->SelectObject(oldPen);
		borderPen.DeleteObject();
		borderBrush.DeleteObject();
	}

	if (pBrush) {
		pMemDC->SelectObject(oldBrush);
	}

	if (pPen) {
		pMemDC->SelectObject(oldPen);
	}

	if (drawfont) {
		//no need to destroy dcfont?
		//assume the destructor of Cfont will take care of freeing dxfont resources
		pMemDC->SelectObject(oldfont);
		dxfont.DeleteObject();
	}

	SelectObject(hMemDC,oldbm);

	LPBITMAPINFOHEADER pBM_HEADER = (LPBITMAPINFOHEADER)GlobalLock(Bitmap2Dib(hbm, 24));
	if (pBM_HEADER == NULL) {
		return NULL;
	}

	pMemDC->Detach();

	DeleteObject(hbm);
	DeleteDC(hMemDC);

	return (LPBITMAPINFO) pBM_HEADER;
}

//AntiAlias 24 Bit Image
//valid factors : 1, 2, 3
HBITMAP CTransparentWnd::DrawResampleRGB(CDC* /*thisDC*/, CRect* /*caprect*/,int factor, LPBITMAPINFOHEADER expanded_bmi)
{
	int iBits = 24;

	LONG Width = expanded_bmi->biWidth;
	LONG Height = expanded_bmi->biHeight;
	long Rowbytes = (Width*iBits+31)/32 *4;

	long reduced_width = Width/factor;
	long reduced_height = Height/factor;
	long reduced_rowbytes = (reduced_width*iBits+31)/32 *4;

	if ((factor<1) || (factor>3))
		return NULL;

	//Create destination buffer
	//long dwSize = sizeof(BITMAPINFOHEADER) + reduced_rowbytes * reduced_height * 3;

	// Allocate room for a DIB and set the LPBI fields
	LPBITMAPINFOHEADER smallbi = (LPBITMAPINFOHEADER)GlobalLock(AllocMakeDib( reduced_width, reduced_height, 24));
	if (smallbi == NULL) {
		return NULL;
	}

	// Get the iBits from the bitmap and stuff them after the LPBI
	LPBYTE lpBits = (LPBYTE)(smallbi+1);

	//Perform the re-sampling
	long x,y,z;
	LPBYTE reduced_ptr;
	LPBYTE Ptr;

	LPBYTE reduced_rowptr = lpBits;
	LPBYTE Rowptr = (LPBYTE) (expanded_bmi + 1);

	//Set the pointers
	reduced_ptr = lpBits;
	Ptr = (LPBYTE) (expanded_bmi + 1);

	int Ptr_incr = (factor-1)*3;
	int Row_incr = Rowbytes * factor;

	int totalval;

	for (y=0;y< reduced_height;y++) {
		//Set to start of each row
		reduced_ptr = reduced_rowptr;
		Ptr = Rowptr;

		for (x=0;x< reduced_width;x++) {
			//Ptr_Pixel = Ptr;

			//for each RGB component
			for (z=0;z<3;z++) {
				if (factor==1) {
					*reduced_ptr = *Ptr;
				} else if (factor==2) {
					totalval = 0;
					totalval += *Ptr;
					totalval += *(Ptr + 3);
					totalval += *(Ptr + Rowbytes);
					totalval += *(Ptr + Rowbytes + 3);
					totalval/=4;

					if (totalval<0) {
						totalval = 0;
					}

					if (totalval>255) {
						totalval = 255;
					}

					*reduced_ptr = (BYTE) totalval;
				} else if (factor==3) {
					totalval = 0;
					totalval += *Ptr;
					totalval += *(Ptr + 3);
					totalval += *(Ptr + 6);

					totalval += *(Ptr + Rowbytes);
					totalval += *(Ptr + Rowbytes + 3);
					totalval += *(Ptr + Rowbytes + 6);

					totalval += *(Ptr + Rowbytes + Rowbytes);
					totalval += *(Ptr + Rowbytes + Rowbytes + 3);
					totalval += *(Ptr + Rowbytes + Rowbytes + 6);

					totalval/=9;

					if (totalval<0) {
						totalval = 0;
					}

					if (totalval>255) {
						totalval = 255;
					}

					*reduced_ptr = (BYTE) totalval;

				} //else if factor

				reduced_ptr++;
				Ptr ++;
			} //for z

			Ptr += Ptr_incr;
			//Ptr += factor * 3;
		} //for x

		reduced_rowptr += reduced_rowbytes;
		//Rowptr += Rowbytes;
		Rowptr += Row_incr;
	} // for y

//	int ret = StretchDIBits ( thisDC->m_hDC, 0, 0,reduced_width,reduced_height, 0, 0,reduced_width,reduced_height, lpBits, (LPBITMAPINFO)smallbi, DIB_RGB_COLORS,SRCCOPY);

	if (smallbi) {
		GlobalFreePtr(smallbi);
	}

	HBITMAP newbm = NULL;
	return newbm;
}

//need to unlock to use it and then
//use GlobalFreePtr to free it
HANDLE CTransparentWnd::AllocMakeDib( int reduced_width, int reduced_height, UINT iBits )
{
	// DWORD align the width of the DIB
	// Figure out the size of the colour table
	// Calculate the size of the DIB
	UINT wLineLen = (reduced_width*iBits+31)/32 * 4;
	DWORD wColSize = sizeof(RGBQUAD)*((iBits <= 8) ? 1<<iBits : 0);
	DWORD dwSize = sizeof(BITMAPINFOHEADER) + wColSize + (DWORD)(UINT)wLineLen * (DWORD)(UINT)reduced_height;

	// Allocate room for a DIB and set the LPBI fields
	HANDLE hdib = GlobalAlloc(GHND,dwSize);
	if (!hdib)
		return hdib;

	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);
	lpbi->biSize = sizeof(BITMAPINFOHEADER);
	lpbi->biWidth = reduced_width;
	lpbi->biHeight = reduced_height;
	lpbi->biPlanes = 1;
	lpbi->biBitCount = (WORD) iBits;
	lpbi->biCompression = BI_RGB;
	lpbi->biSizeImage = dwSize - sizeof(BITMAPINFOHEADER) - wColSize;
	lpbi->biXPelsPerMeter = 0;
	lpbi->biYPelsPerMeter = 0;
	lpbi->biClrUsed = (iBits <= 8) ? 1<<iBits : 0;
	lpbi->biClrImportant = 0;

	// Get the iBits from the bitmap and stuff them after the LPBI
//	LPBYTE lpBits = (LPBYTE)(lpbi+1)+wColSize;
	lpbi->biClrUsed = (iBits <= 8) ? 1<<iBits : 0;

	GlobalUnlock(hdib);

	return hdib;
}

/*LRESULT CTransparentWnd::OnInvalidateWnd(WPARAM p1, LPARAM p2)
{
	//Invalidate();
	return 0;
}*/

BOOL CTransparentWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// forward to tracker

	if (m_bTrackingOn) {
		if (pWnd == this && m_tracker.SetCursor(this, nHitTest))
			return TRUE;
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CTransparentWnd::EnsureOnTopList(CTransparentWnd* transWnd )
{
	if (m_baseType > 0)
		return; //if not screen annotation...skip

	ListManager.EnsureOnTopList(transWnd);
}

void CTransparentWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	EnsureOnTopList(this);

	CRect truerect;
	m_tracker.GetTrueRect( &truerect);

	if (m_bTrackingOn) {
		m_tracker.Track( this, point, FALSE,NULL);
		Invalidate();
	}

	CWnd::OnLButtonDown(nFlags, point);

	if (m_bTrackingOn && truerect.PtInRect(point)) {
	} else {
		if (m_movewindow == 0) {
			m_movewindow = 1;
			GetCursorPos(&m_movepoint);
			SetCapture();
		}
		//PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x,point.y));
	}
}

void CTransparentWnd::OnContextEditTransparency()
{
	EditTransparency();
}

void CTransparentWnd::EditTransparency()
{
	CRecorderApp *pApp = (CRecorderApp *)AfxGetApp();
	if (pApp->VersionOp() < 5) {
		//int ret = MessageBox("This feature is only available in Win 2000/ XP." ,"Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(this->m_hWnd,IDS_STRING_AVAILXP ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	m_bEditTransOn = true;

	CEditTransparencyDlg etDlg(m_benable, m_valueTransparency, this);
	etDlg.DoModal();

	m_bEditTransOn = false;
	OnUpdateContextMenu();
}

void CTransparentWnd::InvalidateTransparency()
{
	if (m_benable) {
		G_Layered.AddLayeredStyle(m_hWnd);
		// As transparency is defined and limited in dialogWindow we can cast from int to BYTE without loosing any info 
		G_Layered.SetTransparentPercentage(m_hWnd, static_cast<BYTE>(m_valueTransparency) );
	} else {
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, ::GetWindowLong(m_hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
	}
}

void CTransparentWnd::OnContextNoAntiAlias()
{
	m_factor = 1;
	OnUpdateContextMenu();
	Invalidate();
}

void CTransparentWnd::OnContextAntiAlias2()
{
	m_factor = 2;
	OnUpdateContextMenu();
	Invalidate();
}

void CTransparentWnd::OnContextAntiAlias3()
{
	m_factor = 3;
	OnUpdateContextMenu();
	Invalidate();
}

void CTransparentWnd::DisableContextMenu()
{
	if (m_menuLoaded == 0) {
		m_menu.LoadMenu(IDR_CONTEXTMENU);
		m_menuLoaded = 1;
	}

	CMenu* pPopup = m_menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	pPopup->EnableMenuItem(ID_CONTEXT_EDITTEXT,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
	pPopup->EnableMenuItem(ID_CONTEXT_CLOSEALL, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
	pPopup->EnableMenuItem(ID_CONTEXT_CLOSE, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
	pPopup->EnableMenuItem(ID_CONTEXT_SAVELAYOUT, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
	pPopup->EnableMenuItem(ID_CONTEXT_RESIZE, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
	pPopup->EnableMenuItem(ID_CONTEXT_REFRESH, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
	pPopup->EnableMenuItem(ID_CONTEXT_EDITTEXT, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
	pPopup->EnableMenuItem(ID_CONTEXT_SAVE, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
	pPopup->EnableMenuItem(ID_CONTEXT_EDITIMAGE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
	pPopup->EnableMenuItem(ID_CONTEXT_CLONE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
	pPopup->EnableMenuItem(ID_CONTEXT_EDITTRANSPARENCY,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
	pPopup->EnableMenuItem(ID_CONTEXT_EDITTRANSPARENCY,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
}

void CTransparentWnd::OnUpdateContextMenu()
{
	if (m_menuLoaded == 0) {
		m_menu.LoadMenu(IDR_CONTEXTMENU);
		m_menuLoaded = 1;
	}

	CMenu* pPopup = m_menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	pPopup->CheckMenuItem(ID_CONTEXT_ANTIALIAS_NOANTIALIAS, m_factor == 1 ? MF_CHECKED : MF_UNCHECKED );
	pPopup->CheckMenuItem(ID_CONTEXT_ANTIALIAS_ANTIALIASX2, m_factor == 2 ? MF_CHECKED : MF_UNCHECKED );
	pPopup->CheckMenuItem(ID_CONTEXT_ANTIALIAS_ANTIALIASX3SLOWEST, m_factor == 3 ? MF_CHECKED : MF_UNCHECKED );

	if (m_saveMethod == saveMethodReplace) {
		pPopup->EnableMenuItem(ID_CONTEXT_EDITTEXT,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_CLOSEALL, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_CLOSE, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_SAVELAYOUT, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_RESIZE, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_REFRESH, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_EDITTEXT, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_SAVE, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_EDITIMAGE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_CLONE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);

		pPopup->EnableMenuItem(ID_CONTEXT_EDITTRANSPARENCY,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		return;
	}

	if (m_bTrackingOn || m_bEditImageOn) {
		pPopup->EnableMenuItem(ID_CONTEXT_EDITTEXT,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_CLOSEALL, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_CLOSE, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_SAVELAYOUT, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_RESIZE, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_REFRESH, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_EDITTEXT, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_SAVE, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_EDITIMAGE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_CLONE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
	} else {
		pPopup->EnableMenuItem(ID_CONTEXT_EDITTEXT,MF_ENABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_CLOSEALL, MF_ENABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_CLOSE, MF_ENABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_SAVELAYOUT, MF_ENABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_RESIZE, MF_ENABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_REFRESH, MF_ENABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_EDITTEXT, MF_ENABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_SAVE, MF_ENABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_EDITIMAGE, MF_ENABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_CLONE, MF_ENABLED|MF_BYCOMMAND);
	}

	if (m_bEditTransOn) {
		pPopup->EnableMenuItem(ID_CONTEXT_EDITTRANSPARENCY,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
	} else {
		pPopup->EnableMenuItem(ID_CONTEXT_EDITTRANSPARENCY, MF_ENABLED|MF_BYCOMMAND);
	}
}

void CTransparentWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_movewindow) {
		m_movewindow = 0;
		ReleaseCapture();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CTransparentWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_movewindow) {
		POINT currpoint;
		GetCursorPos(&currpoint);

//		int nWidth = m_rectWnd.right - m_rectWnd.left + 1;
//		int nHeight = m_rectWnd.bottom - m_rectWnd.top + 1;

		m_rectWnd.left += currpoint.x - m_movepoint.x;
		m_rectWnd.top += currpoint.y - m_movepoint.y;
		m_rectWnd.right += currpoint.x - m_movepoint.x;
		m_rectWnd.bottom += currpoint.y - m_movepoint.y;

		SetWindowPos( &wndTopMost, m_rectWnd.left, m_rectWnd.top, 0, 0, SWP_NOSIZE );
		m_movepoint = currpoint;
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CTransparentWnd::OnContextEditImage()
{
	EditImage();
}

void CTransparentWnd::EditImage()
{
	m_bEditImageOn = true;

	CEditImageDlg editDlg;
	editDlg.PreModal(this);	// TODO: What is this doing?
	editDlg.DoModal();

	m_bEditImageOn = false;

	OnUpdateContextMenu();
	Invalidate();
}

void CTransparentWnd::InvalidateRegion()
{
	//m_regionCreated = 0; //put this here?

	SetupRegion();
}

void CTransparentWnd::SetupRegion()
{
	// Argh!!!! CTransparentWnd may not be valid!
	if (!::IsWindow(*this))
		return;

	CDC* tempDC = GetWindowDC();
	SetupRegion(tempDC);
	ReleaseDC(tempDC);
}

void CTransparentWnd::ReloadPic(CString filename)
{
	RECT rect;
	rect = m_rectWnd;

	if (m_picture.Load(filename)) {
		HBITMAP testtrans = NULL;
		if (m_picture.IPicturePtr()->get_Handle( (unsigned int *) &testtrans ) == S_OK ) {
			m_hbitmap = testtrans;

			//Autofit
			//if (fitBitmapSize) {

			rect.right = rect.left + m_picture.Width() - 1;
			rect.bottom = rect.top + m_picture.Height() - 1;

			//}

			m_rectWnd = rect;

			//WidthHeight
			m_rectOriginalWnd = m_rectWnd;

			if ((m_rectWnd.Width()>100) && (m_rectWnd.Height()>100)) {
				m_tracker.m_rect.left = 20;
				m_tracker.m_rect.top = 20;
				m_tracker.m_rect.right = rect.right - rect.left - 20;
				m_tracker.m_rect.bottom = rect.bottom - rect.top - 20;
			} else {
				m_tracker.m_rect.left = 1;
				m_tracker.m_rect.top = 1;
				m_tracker.m_rect.right = m_rectWnd.Width() - 2;
				m_tracker.m_rect.bottom = m_rectWnd.Height() - 2;
			}

			m_regionCreated = NULL;
			SetupRegion();

			SetWindowPos( &wndTopMost, m_rectWnd.left, m_rectWnd.top, m_rectWnd.Width(), m_rectWnd.Height(), SWP_NOMOVE );
		} else {
			m_hbitmap = NULL;
		}
	} else {
		m_hbitmap = NULL;
	}
}

// *****************************************************************************
// These functions needs to be changed everytime TransparentWnd adds new members
// *****************************************************************************
CTransparentWnd* CTransparentWnd::Clone(int offsetx, int offsety)
{
	CTransparentWnd* newWnd = new CTransparentWnd;
	newWnd->CopyMembers(*this);

	newWnd->m_rectWnd.OffsetRect(offsetx, offsety);

	CString pTitle(m_shapeStr);
	newWnd->CreateEx(WS_EX_TOPMOST, AfxRegisterWndClass(0), LPCTSTR(pTitle), WS_POPUP | WS_SYSMENU, newWnd->m_rectWnd, NULL, NULL, NULL);
	newWnd->m_regionCreated = NULL;
	newWnd->m_tracker.m_rect = m_tracker.m_rect;

	if (m_hbitmap == NULL) {
		newWnd->m_hbitmap = NULL;
	} else {
		//CPicture picture;
		//int randnum = rand();
		//char numstr[50];
		//sprintf(numstr,"%d", randnum);
		//CString cnumstr(numstr);
		//CString fxstr("\\~txPic");
		//CString exstr(".bmp");
		//CString tempFile = GetTempFolder(iTempPathAccess, specifieddir) + fxstr + cnumstr + exstr;
		CString tempFile;
		tempFile.Format("%s\\~txPic%d.bmp", GetTempFolder(cProgramOpts.m_iTempPathAccess, cProgramOpts.m_strSpecifiedDir), rand());
		int ret = m_picture.CopyToPicture(&newWnd->m_picture,tempFile);
		if (!ret) {
			//randnum = rand();
			//sprintf(numstr, "%d", randnum);
			//tempFile = GetTempFolder(iTempPathAccess, specifieddir) + fxstr + cnumstr + exstr;
			tempFile.Format("%s\\~txPic%d.bmp", GetTempFolder(cProgramOpts.m_iTempPathAccess, cProgramOpts.m_strSpecifiedDir), rand());
			ret = m_picture.CopyToPicture(&newWnd->m_picture,tempFile);
			if (!ret) {
				//if 2nd try fails
				newWnd->m_hbitmap = NULL;
				return newWnd;
			}
		}

		HBITMAP testtrans = NULL;
		if (newWnd->m_picture.IPicturePtr()->get_Handle((unsigned int *) &testtrans) == S_OK) {
			newWnd->m_hbitmap = testtrans;
			newWnd->SetupRegion();
		} else {
			newWnd->m_hbitmap = NULL;
		}

		DeleteFile(tempFile);
	}

	return newWnd;
}

CTransparentWnd* CTransparentWnd::CloneByPos(int x, int y)
{
	int offsetx = x - m_rectWnd.left;
	int offsety = y - m_rectWnd.top;
	return Clone(offsetx, offsety);
}

void CTransparentWnd::CopyMembers(const CTransparentWnd& rhsWnd)
{
	m_textstring			= rhsWnd.m_textstring;
	m_shapeStr				= rhsWnd.m_shapeStr;
	m_vertalign				= rhsWnd.m_vertalign;
	m_horzalign				= rhsWnd.m_horzalign;
	m_textfont				= rhsWnd.m_textfont;
	m_rgb					= rhsWnd.m_rgb;
	m_factor				= rhsWnd.m_factor;
	m_charset				= rhsWnd.m_charset;
	m_rectWnd				= rhsWnd.m_rectWnd;
	m_benable	= rhsWnd.m_benable;
	m_valueTransparency		= rhsWnd.m_valueTransparency;
	m_transparentColor		= rhsWnd.m_transparentColor;
	m_regionType			= rhsWnd.m_regionType;
	m_regionPredefinedShape = rhsWnd.m_regionPredefinedShape;
	m_roundrectFactor		= rhsWnd.m_roundrectFactor;
	m_bBorderYes			= rhsWnd.m_bBorderYes;
	m_borderSize			= rhsWnd.m_borderSize;
	m_borderColor			= rhsWnd.m_borderColor;
	m_backgroundColor		= rhsWnd.m_backgroundColor;

	//WidthHeight
	m_rectOriginalWnd		= rhsWnd.m_rectOriginalWnd;
	m_widthPos				= rhsWnd.m_widthPos;
	m_heightPos				= rhsWnd.m_heightPos;

	//uniqueID is not copied
}

void CTransparentWnd::OnContextRefresh()
{
	Invalidate();
	InvalidateRegion();
}

//unconfirmed
void CTransparentWnd::OnContextClone()
{
	CTransparentWnd* cloneWnd = NULL;

	srand( (unsigned)time( NULL ) );
	int x = (rand() % 40) - 20;
	int y = (rand() % 40) - 20;

	cloneWnd = Clone(x, y);

	if (cloneWnd) {
		ListManager.AddDisplayArray(cloneWnd);

		cloneWnd->InvalidateRegion();
		cloneWnd->InvalidateTransparency();
		cloneWnd->ShowWindow(SW_SHOW);
	}
}

void CTransparentWnd::OnContextCloseall()
{
	sadlg.CloseAllWindows(1);
}

void CTransparentWnd::OnContextClose()
{
	ShowWindow(SW_HIDE);
	ListManager.RemoveDisplayArray(this,1);
}

BOOL CTransparentWnd::SaveShape(FILE* fptr)
{
	if (fptr == NULL)
		return FALSE;

	long shapeversion = 1;
	fwrite(&shapeversion, sizeof(long), 1, fptr);

	fwrite(&m_tracker.m_rect.left, sizeof(long), 1, fptr);
	fwrite(&m_tracker.m_rect.top, sizeof(long), 1, fptr);
	fwrite(&m_tracker.m_rect.right, sizeof(long), 1, fptr);
	fwrite(&m_tracker.m_rect.bottom, sizeof(long), 1, fptr);

	int len = m_textstring.GetLength();
	fwrite(&len, sizeof(int), 1, fptr);
	fwrite(LPCTSTR(m_textstring), len, 1, fptr);

	len = m_shapeStr.GetLength();
	fwrite(&len, sizeof(int), 1, fptr);
	fwrite((LPCTSTR)m_shapeStr, len, 1, fptr);

	fwrite(&m_vertalign, sizeof(int), 1, fptr);
	fwrite(&m_horzalign, sizeof(int), 1, fptr);

	fwrite(&m_textfont, sizeof(LOGFONT), 1, fptr);
	fwrite(&m_rgb, sizeof(COLORREF), 1, fptr);

	fwrite(&m_factor, sizeof(int), 1, fptr);
	fwrite(&m_charset, sizeof(int), 1, fptr);

	fwrite(&m_rectWnd.left, sizeof(long), 1, fptr);
	fwrite(&m_rectWnd.top, sizeof(long), 1, fptr);
	fwrite(&m_rectWnd.right, sizeof(long), 1, fptr);
	fwrite(&m_rectWnd.bottom, sizeof(long), 1, fptr);

	fwrite(&m_benable, sizeof(int), 1, fptr);
	fwrite(&m_valueTransparency, sizeof(int), 1, fptr);
	fwrite(&m_transparentColor, sizeof(COLORREF), 1, fptr);

	fwrite(&m_regionCreated, sizeof(int), 1, fptr);
	fwrite(&m_regionType, sizeof(int), 1, fptr);
	fwrite(&m_regionPredefinedShape, sizeof(int), 1, fptr);
	fwrite(&m_roundrectFactor, sizeof(double), 1, fptr);

	fwrite(&m_bBorderYes, sizeof(int), 1, fptr);
	fwrite(&m_borderSize, sizeof(int), 1, fptr);

	fwrite(&m_borderColor, sizeof(COLORREF), 1, fptr);
	fwrite(&m_backgroundColor, sizeof(COLORREF), 1, fptr);

	fwrite(&m_rectOriginalWnd.left, sizeof(long), 1, fptr);
	fwrite(&m_rectOriginalWnd.top, sizeof(long), 1, fptr);
	fwrite(&m_rectOriginalWnd.right, sizeof(long), 1, fptr);
	fwrite(&m_rectOriginalWnd.bottom, sizeof(long), 1, fptr);

	fwrite(&m_widthPos, sizeof(int), 1, fptr);
	fwrite(&m_heightPos, sizeof(int), 1, fptr);

	BOOL ret = TRUE;
	if (m_hbitmap) {
		ret = m_picture.SaveToFile(fptr);
	} else {
		DWORD sizefile = 0;
		fwrite(&sizefile, sizeof(DWORD), 1, fptr);
	}

	return ret;
}

BOOL CTransparentWnd::LoadShape(FILE* fptr)
{
	if (fptr == NULL)
		return FALSE;

	BOOL ret = TRUE;
	int len = 0;

	long shapeversion = 1;
	fread( (void *) &shapeversion, sizeof(long), 1, fptr );

	fread( (void *) &m_tracker.m_rect.left, sizeof(long), 1, fptr );
	fread( (void *) &m_tracker.m_rect.top, sizeof(long), 1, fptr );
	fread( (void *) &m_tracker.m_rect.right, sizeof(long), 1, fptr );
	fread( (void *) &m_tracker.m_rect.bottom, sizeof(long), 1, fptr );

	fread( (void *) &len, sizeof(int), 1, fptr );
	if ((0 < len) && (len < 100000)) {
		//char *buf = (char *) malloc(len + 2);
		char *buf = new char[len + 2];			// No memory leak here, buf is deleted in this block of code too
		fread(buf, len, 1, fptr );
		buf[len] = 0;
		buf[len+1] = 0;
		m_textstring = (char *) buf;
		//free(buf);
		delete [] buf;
	}

	fread( (void *) &len, sizeof(int), 1, fptr );
	if ((len>0) && (len<100000)) {
		//char *buf = (char *) malloc(len + 2);
		char *buf = new char[len + 2];			// No memory leak here, buf is deleted in this block of code too
		fread( (void *) buf, len, 1, fptr );
		buf[len] = 0;
		buf[len+1] = 0;
		m_shapeStr = (char *) buf;
		//free(buf);
		delete [] buf;
	}

	fread( (void *) &m_vertalign, sizeof(int), 1, fptr );
	fread( (void *) &m_horzalign, sizeof(int), 1, fptr );

	fread( (void *) &m_textfont, sizeof(LOGFONT), 1, fptr );
	fread( (void *) &m_rgb, sizeof(COLORREF), 1, fptr );

	fread( (void *) &m_factor, sizeof(int), 1, fptr );
	fread( (void *) &m_charset, sizeof(int), 1, fptr );

	fread( (void *) &m_rectWnd.left, sizeof(long), 1, fptr );
	fread( (void *) &m_rectWnd.top, sizeof(long), 1, fptr );
	fread( (void *) &m_rectWnd.right, sizeof(long), 1, fptr );
	fread( (void *) &m_rectWnd.bottom, sizeof(long), 1, fptr );

	fread( (void *) &m_benable, sizeof(int), 1, fptr );
	fread( (void *) &m_valueTransparency, sizeof(int), 1, fptr );
	fread( (void *) &m_transparentColor, sizeof(COLORREF), 1, fptr );

	fread( (void *) &m_regionCreated, sizeof(int), 1, fptr );
	fread( (void *) &m_regionType, sizeof(int), 1, fptr );
	fread( (void *) &m_regionPredefinedShape, sizeof(int), 1, fptr );
	fread( (void *) &m_roundrectFactor, sizeof(double), 1, fptr );

	fread( (void *) &m_bBorderYes, sizeof(int), 1, fptr );
	fread( (void *) &m_borderSize, sizeof(int), 1, fptr );

	fread( (void *) &m_borderColor, sizeof(COLORREF), 1, fptr );
	fread( (void *) &m_backgroundColor, sizeof(COLORREF), 1, fptr );

	fread( (void *) &m_rectOriginalWnd.left, sizeof(long), 1, fptr );
	fread( (void *) &m_rectOriginalWnd.top, sizeof(long), 1, fptr );
	fread( (void *) &m_rectOriginalWnd.right, sizeof(long), 1, fptr );
	fread( (void *) &m_rectOriginalWnd.bottom, sizeof(long), 1, fptr );

	fread( (void *) &m_widthPos, sizeof(int), 1, fptr );
	fread( (void *) &m_heightPos, sizeof(int), 1, fptr );

	ret = TRUE;
	if (m_picture.LoadFromFile(fptr)) {
		if (m_picture.IPicturePtr() == NULL) {
			//Case : No image
			m_hbitmap = NULL;
		} else {
			//Case : Has image
			HBITMAP testtrans = NULL;
			if (m_picture.IPicturePtr()->get_Handle( (unsigned int *) &testtrans ) == S_OK ) {
				m_hbitmap = testtrans;
				SetupRegion();
			} else {
				m_hbitmap = NULL;
			}
		}
	} else {
		//ErrMsg("\nPic Fail");
		//Case : Image Load Error
		m_hbitmap = NULL;
		ret = FALSE;
	}

	if (shapeversion>1) {
		//new version...
	}
	if (shapeversion>2) {
	}

	return ret;
}

//WidthHeight
void CTransparentWnd::RefreshWindowSize()
{
	double widthfract = m_widthPos * 0.025 + 0.2;
	double heightfract = m_heightPos*0.025 + 0.2;

	m_rectWnd.right = m_rectWnd.left + (long) ((double) (m_rectOriginalWnd.Width()) * widthfract);
	m_rectWnd.bottom = m_rectWnd.top + (long) ((double) (m_rectOriginalWnd.Height()) * heightfract);

	if (m_rectWnd.Width()<40) {
		m_rectWnd.right = m_rectWnd.left + 40 - 1;
	}

	if (m_rectWnd.Width()>maxxScreen) {
		m_rectWnd.right = m_rectWnd.left + maxxScreen - 1;
	}

	if (m_rectWnd.Height()<40) {
		m_rectWnd.bottom = m_rectWnd.top + 40 - 1;
	}

	if (m_rectWnd.Height()>maxyScreen) {
		m_rectWnd.bottom = m_rectWnd.top + maxyScreen - 1;
	}

	SetWindowPos( &wndTopMost, m_rectWnd.left, m_rectWnd.top,m_rectWnd.right - m_rectWnd.left + 1 , m_rectWnd.bottom - m_rectWnd.top + 1, SWP_NOMOVE );
	Invalidate();
}

//WidthHeight
void CTransparentWnd::OnContextResize()
{
	// TODO: Add your command handler code here
	m_bTrackingOn = true;

	//WidthHeight
	SetWindowRgn((HRGN)NULL, TRUE);

	Invalidate();

	CResizeDlg rsDlg;
	rsDlg.PreModal(this);
	if (rsDlg.DoModal() == IDOK) {
	}

	m_bTrackingOn = false;

	OnUpdateContextMenu();

	//WidthHeight
	m_regionCreated = 0;
	InvalidateRegion();
	Invalidate();
}

void CTransparentWnd::OnSysCommand(UINT nID, LPARAM lParam)
{
	CWnd::OnSysCommand(nID, lParam);
}
