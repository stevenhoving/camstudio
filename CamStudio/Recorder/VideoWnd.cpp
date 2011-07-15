// VideoWnd.cpp : implementation file
//

#include "stdafx.h"
#include "Recorder.h"
#include "VideoWnd.h"
#include "TransRateDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEFAULT_PERIOD 30
int iRrefreshRate = DEFAULT_PERIOD;

/////////////////////////////////////////////////////////////////////////////
// CVideoWnd
CVideoWnd::CVideoWnd()
{
	m_iStatus = 0;
	m_baseType = 1;
	m_iRefreshRate = iRrefreshRate;
}

CVideoWnd::~CVideoWnd()
{
}

BEGIN_MESSAGE_MAP(CVideoWnd, CTransparentWnd)
	//{{AFX_MSG_MAP(CVideoWnd)
	ON_COMMAND(ID_CONTEXTVIDEO_SOURCEFORMAT, OnContextvideoSourceformat)
	ON_COMMAND(ID_CONTEXTVIDEO_VIDEOSOURCE, OnContextvideoVideosource)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CONTEXTVIDEO_EDITTRANSPARENCYREFRESHRATE, OnContextvideoEdittransparencyrefreshrate)
	ON_COMMAND(ID_CONTEXTVIDEO_EDITTRANSPARENCY, OnContextvideoEdittransparency)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVideoWnd message handlers

void CVideoWnd::AdjustRefreshRate(int rate)
{

	if (rate != m_iRefreshRate)
	{
		m_iRefreshRate = rate;
		iRrefreshRate = m_iRefreshRate;
		double delayPeriod = 1000 / m_iRefreshRate;
		SetTimer(0x1, (int) delayPeriod, NULL);

	}

}

void CVideoWnd::CreateTransparent(LPCTSTR pTitle, RECT rect, HBITMAP BitmapID)
{
	CTransparentWnd::CreateTransparent( pTitle, rect, BitmapID);

	BeginWaitCursor();

	//FrameGrabber
	if (!m_FrameGrabber.GetSafeHwnd())
	{
		//m_FrameGrabber.Create(0,9,this);
		int ret = m_FrameGrabber.Create(0,9,this);

		//ret = 0;
		if (ret)
		{
			double delayPeriod = 1000 / m_iRefreshRate;
			//SetTimer(0x1, DEFAULT_PERIOD, NULL);
			SetTimer(0x1, (int) delayPeriod, NULL);
			m_iStatus = 1;

		}
		else {
			m_textstring.LoadString(IDS_STRING_NOWEBCAM);
		}

	}

	EndWaitCursor();

}

//FrameGrabber
void CVideoWnd::OnTimer(UINT /*nIDEvent*/)
{
	if (!m_FrameGrabber.GetSafeHwnd()) {
		return;
	}

	if (!IsWindowVisible()) {
		return;
	}

	if (m_bTrackingOn || m_bEditImageOn || m_bEditTransOn) {
		return;
	}

	LPBITMAPINFO lpBi = m_FrameGrabber.GetDIB();
	m_ImageBitmap.CreateFromDib(lpBi);
	InvalidateRect(NULL);
}

void CVideoWnd::OnPaint()
{
	CPaintDC dc(this);

	//WIDTHHEIGHT
	CRect clrect(m_rectWnd);
	clrect.right -= clrect.left;
	clrect.bottom -= clrect.top;
	clrect.left = 0;
	clrect.top = 0;

	int width = clrect.right - clrect.left;
	int height = clrect.bottom - clrect.top;

	CRect cRect(clrect);
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CDC *pDC = &memDC;
	CBitmap NewMemBmp;
	NewMemBmp.CreateCompatibleBitmap(&dc, width, height);
	CBitmap* pOldMemBmp = pDC->SelectObject(&NewMemBmp);
	if (!m_iStatus) {
		pDC->FillSolidRect(0, 0, clrect.Width(), clrect.Height(), RGB(255,255,255));
	} else if (m_ImageBitmap.GetSafeHandle()) {
		m_ImageBitmap.BitBlt(pDC, CPoint(0,0));
	}

	CFont dxfont;
	dxfont.CreateFontIndirect(&m_textfont);
	CFont* oldfont = (CFont *) pDC->SelectObject(&dxfont);

	//Draw Text
	int textlength = m_textstring.GetLength(); //get number of bytes
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_rgb);
	pDC->DrawText((char *)LPCTSTR(m_textstring), textlength, &m_tracker.m_rect, m_horzalign | DT_VCENTER | DT_WORDBREAK );

	pDC->SelectObject(oldfont);

	if ((m_bBorderYes) && (regionSHAPE == m_regionType)) {
		double rx = cRect.Width() * m_roundrectFactor;
		double ry = cRect.Height() * m_roundrectFactor;
		double rval = (rx > ry) ? ry : rx;

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

	CDC *pWinDC = &dc;
	pWinDC->BitBlt(0,0,clrect.Width(),clrect.Height(),pDC,0,0,SRCCOPY);

	pDC->SelectObject(pOldMemBmp);
	NewMemBmp.DeleteObject();
	memDC.DeleteDC();

	if (m_bTrackingOn) {
		m_tracker.Draw(pWinDC);
	}
}

void CVideoWnd::OnContextvideoSourceformat()
{
	if (m_FrameGrabber.GetSafeHwnd()) {
		m_FrameGrabber.VideoSourceDialog();
		//OnUpdate(NULL, 0, NULL);
		OnUpdateSize();
		Invalidate();
	}
}

void CVideoWnd::OnContextvideoVideosource()
{
	// TODO: Add your command handler code here
	if (m_FrameGrabber.GetSafeHwnd()) {
		m_FrameGrabber.VideoFormatDialog();
		//OnUpdate(NULL, 0, NULL);

		OnUpdateSize();
		Invalidate();
	}
}

void CVideoWnd::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: Add your message handler code here
	//int isEdited = AreWindowsEdited();

	CPoint local = point;
	ScreenToClient(&local);

	if (m_menuLoaded == 0) {
		m_menu.LoadMenu(IDR_CONTEXTVIDEO);
		m_menuLoaded = 1;
	}

	CMenu* pPopup = m_menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	//if (isEdited)
	//{
	//DisableContextMenu();
	//}
	//else
	OnUpdateContextMenu();

	//SetTimer(0x1, ADJUST_PERIOD, NULL);
	// route commands through main window
	pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN, point.x, point.y, this);
}

void CVideoWnd::OnUpdateContextMenu()
{
	if (m_menuLoaded == 0) {
		m_menu.LoadMenu(IDR_CONTEXTVIDEO);
		m_menuLoaded = 1;
	}

	CMenu* pPopup = m_menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	if ((m_FrameGrabber.GetSafeHwnd()) && (m_iStatus))
	{
		pPopup->EnableMenuItem(ID_CONTEXTVIDEO_SOURCEFORMAT,MF_ENABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXTVIDEO_VIDEOSOURCE,MF_ENABLED|MF_BYCOMMAND);

	} else {
		pPopup->EnableMenuItem(ID_CONTEXTVIDEO_SOURCEFORMAT,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXTVIDEO_VIDEOSOURCE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
	}

	if (m_bTrackingOn || m_bEditImageOn || m_bEditTransOn) {
		pPopup->EnableMenuItem(ID_CONTEXT_EDITTEXT,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_CLOSE, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_RESIZE, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_EDITIMAGE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_EDITTRANSPARENCY,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);

		//pPopup->EnableMenuItem(ID_CONTEXTVIDEO_SOURCEFORMAT,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		//pPopup->EnableMenuItem(ID_CONTEXTVIDEO_VIDEOSOURCE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
	} else {
		pPopup->EnableMenuItem(ID_CONTEXT_EDITTEXT,MF_ENABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_CLOSE, MF_ENABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_RESIZE, MF_ENABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_EDITIMAGE, MF_ENABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_EDITTRANSPARENCY, MF_ENABLED|MF_BYCOMMAND);
	}
}

void CVideoWnd::OnUpdateSize()
{
	if (!m_iStatus) {
		CSize sz(180,160);
		ImagePos(sz);
		SetupRegion();
		Invalidate();
	} else if (m_FrameGrabber.GetSafeHwnd()) {
		CSize sz = m_FrameGrabber.GetImageSize();
		ImagePos(sz);


		//CRect rcc;
		//CRect rcw;
		//GetClientRect(&rcc);
		//GetWindowRect(&rcw);
		//int dx = rcw.Width()-rcc.Width();
		//int dy = rcw.Height()-rcc.Height();

		//if (sz.cx && (rcc.Width()!=sz.cx || rcc.Height()!=sz.cy))
		//{
		//	SetWindowPos(NULL,0,0,
		//		sz.cx+dx, sz.cy+dy, SWP_NOMOVE|SWP_NOZORDER);

		//	GetParentFrame()->GetClientRect(&rcc);
		//	GetParentFrame()->GetWindowRect(&rcw);
		//	dx+= rcw.Width()-rcc.Width();
		//	dy+= rcw.Height()-rcc.Height();

		//	GetParentFrame()->SetWindowPos(NULL,0,0,
		//		sz.cx+dx, sz.cy+dy, SWP_NOMOVE|SWP_NOZORDER);
		//}

		SetupRegion();
		Invalidate();
	}
}

//Dialogs does not appears for large image....

void CVideoWnd::OnContextvideoEdittransparencyrefreshrate()
{
	// TODO: Add your command handler code here
	//if (versionOp<5) {
	// int ret = MessageBox("This feature is only available in Win 2000/ XP." ,"Note",MB_OK | MB_ICONEXCLAMATION);
	// return;
	//}
	//editTransOn = 1;

	CTransRateDlg etDlg(this);
	etDlg.DoModal();

	//editTransOn = 0;
	OnUpdateContextMenu();
}

void CVideoWnd::OnContextvideoEdittransparency()
{
	CRecorderApp *pApp = reinterpret_cast<CRecorderApp *>(AfxGetApp());
	if (!pApp) {
		// TODO: report FATAL error.
		return;
	}
	if (pApp->VersionOp() < 5) {
		//int ret = MessageBox("This feature is only available in Win 2000/ XP." ,"Note",MB_OK | MB_ICONEXCLAMATION);
		/*int ret = */
		MessageOut(m_hWnd, IDS_STRING_AVAILXP, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	if (m_iRefreshRate>10)
	{
		//int ret = MessageBox("Enabling Transparency at a high refresh rate will cause dialog boxes to freeze. If this happens, you will have to close the Video Annotation window to make the dialog boxes appear. Reduce the refresh rate ?" ,"Note",MB_YESNOCANCEL | MB_ICONQUESTION);
		int ret = MessageOut(this->m_hWnd,IDS_STRING_TRANSBLAHBLAH ,IDS_STRING_NOTE,MB_YESNOCANCEL | MB_ICONQUESTION);
		if (ret == IDYES)
		{
			AdjustRefreshRate(10);

		}
		else if (ret == IDCANCEL)
			return;

	}

	CTransparentWnd::EditTransparency();
}

void CVideoWnd::ImagePos( CSize &sz )
{
	SetWindowPos(NULL,0,0, sz.cx-1, sz.cy-1, SWP_NOMOVE|SWP_NOZORDER);

	m_rectWnd.right = m_rectWnd.left + sz.cx - 1;
	m_rectWnd.bottom = m_rectWnd.top + sz.cy - 1;
}
