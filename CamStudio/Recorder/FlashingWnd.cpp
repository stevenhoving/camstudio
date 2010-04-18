//********************************************************************************
//* FlashingWindow.CPP
//*
//********************************************************************************

#include "stdafx.h"
#include "Recorder.h"
#include "FlashingWnd.h"
#include "MainFrm.h"			// for maxxScreen, maxyScreen

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace {	// annonymous

const int THICKNESS			= 8;
const int SMALLTHICKNESS	= 4;
const int SIDELEN			= 12;
const int SIDELEN2			= 24;

}	// namespace annonymous

CRect newRect;
bool bNewRegionUsed = false;
bool bReadingRegion = false;
bool bWritingRegion = false;

//********************************************************************************
//* Constructor
//********************************************************************************

CFlashingWnd::CFlashingWnd()
: m_clrBorderON(RGB(255, 255, 180))
, m_clrBorderOFF(RGB(0, 255, 80))
, m_hCursorMove(::LoadCursor(NULL, IDC_SIZEALL))
, m_iType(0)
, m_bStartDrag(false)
{
	m_rgnOld.CreateRectRgn(0,0,0,0);
}

//********************************************************************************
//* Destructor
//********************************************************************************

CFlashingWnd::~CFlashingWnd()
{
	if (m_hCursorMove) {
		DeleteObject(m_hCursorMove);
	}
}

BEGIN_MESSAGE_MAP(CFlashingWnd, CWnd)
	//{{AFX_MSG_MAP(CFlashingWnd)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_FLASH_WINDOW, OnFlashWindow)
END_MESSAGE_MAP()

//********************************************************************************
//* CreateFlashing()
//*
//* Creates the main application window Flashing
//********************************************************************************

BOOL CFlashingWnd::CreateFlashing(LPCTSTR pTitle, RECT &rect)
{
	BOOL bResult = CreateEx(WS_EX_TOPMOST, AfxRegisterWndClass(0, LoadCursor(0, IDC_ARROW)), pTitle, WS_POPUP, rect, 0, 0, 0);
	if (!bResult) {
		::OnError("CFlashingWnd::CreateFlashing");
	}
	return bResult;
}

//********************************************************************************
//* SetupRegion()
//*
//* Set the Window Region for transparancy outside the mask region
//********************************************************************************

void CFlashingWnd::SetUpRegion(const CRect& cRect, int type)
{
	CRgn wndRgn;

	m_cRect = cRect;
	m_cRect.right--;
	m_cRect.bottom--;

	if (type == 0) {
		wndRgn.CreateRectRgn(0, 0, m_cRect.Width() + THICKNESS + THICKNESS, m_cRect.Height() + THICKNESS + THICKNESS);

		CRgn rgnTemp;
		CRgn rgnTemp2;
		CRgn rgnTemp3;
		rgnTemp.CreateRectRgn(THICKNESS, THICKNESS, m_cRect.Width() + THICKNESS + 1, m_cRect.Height() + THICKNESS + 1);
		rgnTemp2.CreateRectRgn(0, SIDELEN2, m_cRect.Width()+THICKNESS+THICKNESS, m_cRect.Height() - SIDELEN + 1);
		rgnTemp3.CreateRectRgn(SIDELEN2,0, m_cRect.Width() - SIDELEN + 1, m_cRect.Height() + THICKNESS + THICKNESS);

		wndRgn.CombineRgn(&wndRgn,&rgnTemp,RGN_DIFF);
		wndRgn.CombineRgn(&wndRgn,&rgnTemp2,RGN_DIFF);
		wndRgn.CombineRgn(&wndRgn,&rgnTemp3,RGN_DIFF);

		wndRgn.OffsetRgn( m_cRect.left-THICKNESS, m_cRect.top-THICKNESS );
	} else {
		wndRgn.CreateRectRgn(0, 0, m_cRect.Width() + SMALLTHICKNESS + SMALLTHICKNESS, m_cRect.Height() + SMALLTHICKNESS + SMALLTHICKNESS);
		CRgn rgnTemp;
		rgnTemp.CreateRectRgn(SMALLTHICKNESS, SMALLTHICKNESS, m_cRect.Width()+SMALLTHICKNESS+1, m_cRect.Height() + SMALLTHICKNESS + 1);

		wndRgn.CombineRgn(&wndRgn, &rgnTemp, RGN_DIFF);

		wndRgn.OffsetRgn(m_cRect.left - SMALLTHICKNESS, m_cRect.top - SMALLTHICKNESS);
	}

	// HRGN newregion = (HRGN) wndRgn.Detach();
	m_rgnOld.CopyRgn(&wndRgn);	// save region
	SetWindowRgn(wndRgn, TRUE);
	//m_rgnOld.FromHandle((HRGN)(wndRgn.Detach()));	// BUG: SetWindowRgnloses region control
}

//********************************************************************************
//* CFlashingWnd message handlers
//********************************************************************************

void CFlashingWnd::PaintBorder(bool bInvert, bool bDraw)
{
	if ((m_cRect.right>m_cRect.left) && (m_cRect.bottom>m_cRect.top)) {
		COLORREF clrColor = 
			(bInvert)
				? m_clrBorderOFF			
				: (bDraw) 
					? m_clrBorderON
					: m_clrBorderOFF;

		CWindowDC cWindowDC(CWnd::FromHandle(m_hWnd));
		CBrush newbrush;
		newbrush.CreateSolidBrush(clrColor);
		CPen newpen;
		newpen.CreatePen(PS_SOLID, 1, clrColor);
		CBrush * pOldBrush = cWindowDC.SelectObject(&newbrush);
		CPen * pOldPen = cWindowDC.SelectObject(&newpen);
		
		if (bInvert) {
			cWindowDC.PatBlt(0, 0, maxxScreen, maxyScreen, PATINVERT);
		} else {
			CRect rectNew(m_cRect);
			rectNew.InflateRect(THICKNESS, THICKNESS);
			cWindowDC.Rectangle(&rectNew);
		}

		cWindowDC.SelectObject(pOldPen);
		cWindowDC.SelectObject(pOldBrush);
	}
}


//ver 1.8
BOOL CFlashingWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CFlashingWnd::OnLButtonDown(UINT /*nFlags*/, CPoint /*point*/)
{
	if (!cRegionOpts.m_bSupportMouseDrag) {
		return;
	}

	if ((0 == m_iType) && !m_bStartDrag) {
		GetCursorPos(&m_ptStart);
		m_bStartDrag = true;
		SetCapture();
	}
}

void CFlashingWnd::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/)
{
	if (!cRegionOpts.m_bSupportMouseDrag) {
		return;
	}

	if ((0 == m_iType) && m_bStartDrag) {
		CPoint pt;
		GetCursorPos(&pt);
		ReleaseCapture();
		m_bStartDrag = false;
		int diffx = pt.x - m_ptStart.x;
		int diffy = pt.y - m_ptStart.y;
		MoveRegion(diffx, diffy);
		//CSize sizeDiff(pt - m_ptStart);
		//MoveRegion(sizeDiff.cx, sizeDiff.cy);
	}
}

void CFlashingWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (cRegionOpts.m_bSupportMouseDrag) {
		//if (0 == m_iType) {
		if ((0 == m_iType) && m_bStartDrag) {
			//if (m_iStartDrag)
			//{				
			//	POINT currPoint;
			//	GetCursorPos( &currPoint );
			//	int diffx = currPoint.x-m_ptStart.x;
			//	int diffy = currPoint.y-m_ptStart.y;
			//	MoveRegion(diffx, diffy);
			//	m_ptStart = currPoint;
			//}

			if (m_hCursorMove) {
				SetCursor(m_hCursorMove);
			}
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CFlashingWnd::CheckRect(int diffx, int diffy)
{
	CRect saveRect(m_cRect);
	m_cRect.OffsetRect(diffx, diffy);
	TRACE(_T("m_cRect.Width: %d\nsaveRect.Width: %d\n"), m_cRect.Width(), saveRect.Width());

	if (m_cRect.left < 0) {
		m_cRect.left = 0;
		m_cRect.right = m_cRect.left + saveRect.Width();
	}

	if (m_cRect.top < 0) {
		m_cRect.top = 0;
		m_cRect.bottom = m_cRect.top + saveRect.Height();
	}

	if ((maxxScreen - 1) < m_cRect.right) {
		m_cRect.right = maxxScreen - 1;
		m_cRect.left = m_cRect.right - saveRect.Width();
	}

	if ((maxyScreen - 1) < m_cRect.bottom) {
		m_cRect.bottom = maxyScreen - 1;
		m_cRect.top = m_cRect.bottom - saveRect.Height();
	}
	TRACE(_T("m_cRect.Width: %d\nsaveRect.Width: %d\n"), m_cRect.Width(), saveRect.Width());
}

void CFlashingWnd::MoveRegion(int diffx, int diffy)
{
	if (m_iType)
		return;

	CheckRect(diffx, diffy);
	CRgn wndRgn;
	CRgn rgnTemp;
	CRgn rgnTemp2;
	CRgn rgnTemp3;
	MakeFixedRegion(wndRgn, rgnTemp, rgnTemp2, rgnTemp3);
	VERIFY(ERROR != m_rgnOld.CopyRgn(&wndRgn));
	if (SetWindowRgn(wndRgn, TRUE)) {
		UpdateRegionMove();
	}
}

void CFlashingWnd::UpdateRegionMove()
{
	// TODO: what is this???? primative read/write locking?
	bWritingRegion = false;
	while (bReadingRegion) {
	}
	bWritingRegion = true;
	newRect = m_cRect;
	bNewRegionUsed = true;
	bWritingRegion = false;
}

// Create the "4 corner" region surrounding cRect
void CFlashingWnd::MakeFixedRegion(CRgn &wndRgn, CRgn &rgnTemp, CRgn &rgnTemp2, CRgn &rgnTemp3)
{
	CRect rectWnd(0, 0, m_cRect.Width() + THICKNESS + THICKNESS, m_cRect.Height() + THICKNESS + THICKNESS);
	CRect rectTemp(THICKNESS, THICKNESS, m_cRect.Width() + THICKNESS + 1, m_cRect.Height() + THICKNESS + 1);
	CRect rectTemp2(0, SIDELEN2, m_cRect.Width() + THICKNESS + THICKNESS, m_cRect.Height() - SIDELEN + 1);
	CRect rectTemp3(SIDELEN2, 0, m_cRect.Width() - SIDELEN + 1, m_cRect.Height() + THICKNESS + THICKNESS);

	VERIFY(wndRgn.CreateRectRgnIndirect(rectWnd));
	VERIFY(rgnTemp.CreateRectRgnIndirect(rectTemp));
	VERIFY(rgnTemp2.CreateRectRgnIndirect(rectTemp2));
	VERIFY(rgnTemp3.CreateRectRgnIndirect(rectTemp3));

	VERIFY(ERROR != wndRgn.CombineRgn(&wndRgn, &rgnTemp, RGN_DIFF));
	VERIFY(ERROR != wndRgn.CombineRgn(&wndRgn, &rgnTemp2, RGN_DIFF));
	VERIFY(ERROR != wndRgn.CombineRgn(&wndRgn, &rgnTemp3, RGN_DIFF));

	VERIFY(ERROR != wndRgn.OffsetRgn(m_cRect.left - THICKNESS, m_cRect.top - THICKNESS));
}

LRESULT CFlashingWnd::OnFlashWindow(WPARAM wInvert, LPARAM lDraw)
{
	TRACE(_T("CFlashingWnd::OnFlashWindow:\nInvert: %d\nDraw: %ld\n"), wInvert, lDraw);
	bool bInvert = wInvert ? true : false;
	bool bDraw = lDraw ? true : false;
	DrawFlashingRect(bInvert, bDraw);
	return 1L;
}
