// VideoWnd.cpp : implementation file
//

#include "stdafx.h"
#include "vscap.h"
#include "VideoWnd.h"
#include "TransRateDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void ErrMsg(char format[], ...);
extern int AreWindowsEdited();
#define DEFAULT_PERIOD 30
int g_refreshRate = 30;

extern int versionOp;
extern int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus);


/////////////////////////////////////////////////////////////////////////////
// CVideoWnd
CVideoWnd::CVideoWnd()
{
	CTransparentWnd();
	status = 0;
	baseType = 1;
	refreshRate = g_refreshRate;
	
	//m_borderYes = 1;
	//m_regionType = 2; //regionShape
	//m_regionPredefinedShape = regionRECTANGLE;

	
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

	if (rate != refreshRate)
	{
		refreshRate = rate;
		g_refreshRate = refreshRate;
		double delayPeriod = 1000 / refreshRate;		
		SetTimer(0x1, (int) delayPeriod, NULL);		

	}

}

void CVideoWnd::CreateTransparent(LPCTSTR pTitle, RECT rect,  HBITMAP BitmapID)
{
	CTransparentWnd::CreateTransparent( pTitle, rect,  BitmapID);

	BeginWaitCursor();
	
	//FrameGrabber	
	if(!m_FrameGrabber.GetSafeHwnd())
	{
			//m_FrameGrabber.Create(0,9,this);
			int ret = m_FrameGrabber.Create(0,9,this);
				
			//ret = 0;
			if (ret)
			{
				double delayPeriod = 1000 / refreshRate;
				//SetTimer(0x1, DEFAULT_PERIOD, NULL);
				SetTimer(0x1, (int) delayPeriod, NULL);				
				status = 1;

			}
			else {
				m_textstring.LoadString(IDS_STRING_NOWEBCAM);
			}

	}

	EndWaitCursor();

}

//FrameGrabber
void CVideoWnd::OnTimer(UINT nIDEvent) 
{
	if(!m_FrameGrabber.GetSafeHwnd())	return;

	if (!IsWindowVisible())  return;

	
	if ((trackingOn) || (editImageOn) || (editTransOn)) 
	{
		
		return;
	}

			
	
	LPBITMAPINFO lpBi = m_FrameGrabber.GetDIB();
	m_ImageBitmap.CreateFromDib(lpBi);
	InvalidateRect(NULL);
	
	

}



void CVideoWnd::OnPaint()
{

		CPaintDC dc(this);

				
		// Add your drawing code here!
		CDC *pDC = &dc;	
			
		//WIDTHHEIGHT
		CRect clrect;
		//GetClientRect(&clrect);
		clrect = m_rectWnd;
		clrect.right -=  clrect.left;
		clrect.bottom -=  clrect.top;
		clrect.left =  0;
		clrect.top =  0;

		int width = clrect.right - clrect.left;
		int height = clrect.bottom - clrect.top;

		CRect cRect;
		cRect =  clrect;

		
		/*
		if(m_ImageBitmap.GetSafeHandle())
		{		
				//old_pDC = pDC;
				//pDC = m_ImageBitmap.BegingModify();
				
				m_ImageBitmap.BitBlt(pDC,CPoint(0,0));
				//pDoc->m_ImageBitmap.BitBlt(pDC,CPoint(200,0));
				//HDC screenDC = ::GetDC(NULL);
				//pDoc->m_ImageBitmap.BitBlt(CDC::FromHandle(screenDC),CPoint(0,0));
				//::ReleaseDC(NULL,screenDC);
		}
		*/

	

		CDC memDC;	
		
		CBitmap*		 pOldMemBmp = NULL;	
		CBitmap		     NewMemBmp;	
		memDC.CreateCompatibleDC(&dc);	
		
		pDC = &memDC;

		NewMemBmp.CreateCompatibleBitmap(&dc,width,height);
		pOldMemBmp = pDC->SelectObject(&NewMemBmp);			
		if (!status)
		{			
			pDC->FillSolidRect(0,0,clrect.Width(),clrect.Height(),RGB(255,255,255));
			
		}
		else if(m_ImageBitmap.GetSafeHandle())
		{			
			//NewMemBmp.CreateCompatibleBitmap(&dc,width,height);
			//pOldMemBmp = pDC->SelectObject(&NewMemBmp);			
			m_ImageBitmap.BitBlt(pDC,CPoint(0,0));
			
		}
		

		CFont* oldfont;
		CFont dxfont;			

		dxfont.CreateFontIndirect(&m_textfont);
		oldfont = (CFont *) pDC->SelectObject(&dxfont);
					
		int textlength = m_textstring.GetLength(); //get number of bytes			

		//Draw Text
		pDC->SetBkMode(TRANSPARENT); 
		pDC->SetTextColor(rgb);
		pDC->DrawText((char *)LPCTSTR(m_textstring), textlength, &m_tracker.m_rect, m_horzalign | DT_VCENTER | DT_WORDBREAK );
		//DrawTextW(pDC->m_hDC, (unsigned short *)LPCTSTR(m_textstring), textlength, &m_tracker.m_rect, m_horzalign | DT_VCENTER | DT_WORDBREAK );

		//if (oldfont)
		pDC->SelectObject(oldfont);

		if ((m_borderYes) && (m_regionType==regionSHAPE)) {
			
			double rx,ry,rval;
			rx= cRect.Width() * m_roundrectFactor;
			ry= cRect.Height() * m_roundrectFactor;
			if (rx>ry)
				rval = ry;
			else
				rval = rx;

			CPen borderPen;
			CPen* oldPen;
			borderPen.CreatePen( PS_SOLID , m_borderSize, m_borderColor );
			oldPen = (CPen *) pDC->SelectObject(&borderPen);

			LOGBRUSH logbrush;
			CBrush borderBrush;
			CBrush* oldBrush;
			logbrush.lbStyle = BS_HOLLOW;				
			borderBrush.CreateBrushIndirect(&logbrush);

			oldBrush = (CBrush *) pDC->SelectObject(&borderBrush);  
			
			int drawOffset = m_borderSize/2;

			if (m_regionPredefinedShape == regionROUNDRECT)
			{					
				pDC->RoundRect(drawOffset-1, drawOffset-1, cRect.Width()-1-drawOffset, cRect.Height()-1-drawOffset, (int) rval, (int) rval);

			}
			else if (m_regionPredefinedShape == regionELLIPSE)
			{
				pDC->Ellipse(drawOffset-1,drawOffset-1, cRect.Width()-1-drawOffset, cRect.Height()-1-drawOffset);
			}
			else if (m_regionPredefinedShape == regionRECTANGLE)
			{
				pDC->Rectangle(drawOffset-1, drawOffset-1, cRect.Width()-1-drawOffset, cRect.Height()-1-drawOffset);

			}

			pDC->SelectObject(oldBrush);
			pDC->SelectObject(oldPen);
			borderPen.DeleteObject();
			borderBrush.DeleteObject();


		}
		

		CDC *winDC = &dc;	
		winDC->BitBlt(0,0,clrect.Width(),clrect.Height(),pDC,0,0,SRCCOPY);
				

		pDC->SelectObject(pOldMemBmp);	
		NewMemBmp.DeleteObject();		
		memDC.DeleteDC();						

		if (trackingOn)
			m_tracker.Draw(winDC);
	
		
}


void CVideoWnd::OnContextvideoSourceformat() 
{
	// TODO: Add your command handler code here
	if(m_FrameGrabber.GetSafeHwnd())
	{
		m_FrameGrabber.VideoSourceDialog();
		//OnUpdate(NULL, 0, NULL); 
		OnUpdateSize();
		Invalidate();
	}

	
	
}


void CVideoWnd::OnContextvideoVideosource() 
{
	// TODO: Add your command handler code here
	if(m_FrameGrabber.GetSafeHwnd())
	{
		m_FrameGrabber.VideoFormatDialog();
		//OnUpdate(NULL, 0, NULL); 

		OnUpdateSize();
		Invalidate();
	}


	
}


void CVideoWnd::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	//int isEdited = AreWindowsEdited();
	
	
	CPoint local = point;
	ScreenToClient(&local);

	
	if (menuLoaded == 0) {
			menu.LoadMenu(IDR_CONTEXTVIDEO);
			menuLoaded = 1;
	}		

	
	
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	
	//if (isEdited) 
	//{
		//DisableContextMenu();
	//}	
	//else
		OnUpdateContextMenu();

	//SetTimer(0x1, ADJUST_PERIOD, NULL);
	pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN,
						   point.x, point.y,
						   this); // route commands through main window

	
}



void CVideoWnd::OnUpdateContextMenu()
{
	if (menuLoaded == 0) {
		menu.LoadMenu(IDR_CONTEXTVIDEO);
		menuLoaded = 1;
	}		
		
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	

	if  ((m_FrameGrabber.GetSafeHwnd()) && (status))
	{
		pPopup->EnableMenuItem(ID_CONTEXTVIDEO_SOURCEFORMAT,MF_ENABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXTVIDEO_VIDEOSOURCE,MF_ENABLED|MF_BYCOMMAND);

	}		
	else
	{
		pPopup->EnableMenuItem(ID_CONTEXTVIDEO_SOURCEFORMAT,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);		
		pPopup->EnableMenuItem(ID_CONTEXTVIDEO_VIDEOSOURCE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);		
	}



	if ((trackingOn) || (editImageOn) || (editTransOn)) {

		pPopup->EnableMenuItem(ID_CONTEXT_EDITTEXT,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);		
		pPopup->EnableMenuItem(ID_CONTEXT_CLOSE, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);		
		pPopup->EnableMenuItem(ID_CONTEXT_RESIZE, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_EDITIMAGE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_EDITTRANSPARENCY,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);

		//pPopup->EnableMenuItem(ID_CONTEXTVIDEO_SOURCEFORMAT,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);		
		//pPopup->EnableMenuItem(ID_CONTEXTVIDEO_VIDEOSOURCE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);		

	}
	else {

		pPopup->EnableMenuItem(ID_CONTEXT_EDITTEXT,MF_ENABLED|MF_BYCOMMAND);		
		pPopup->EnableMenuItem(ID_CONTEXT_CLOSE, MF_ENABLED|MF_BYCOMMAND);		
		pPopup->EnableMenuItem(ID_CONTEXT_RESIZE, MF_ENABLED|MF_BYCOMMAND);		
		pPopup->EnableMenuItem(ID_CONTEXT_EDITIMAGE, MF_ENABLED|MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_CONTEXT_EDITTRANSPARENCY, MF_ENABLED|MF_BYCOMMAND);
		
	}
   
	

}


void CVideoWnd::OnUpdateSize()
{

	if (!status)
	{
		CSize sz(180,160);
		SetWindowPos(NULL,0,0, sz.cx-1, sz.cy-1, SWP_NOMOVE|SWP_NOZORDER);
		
		m_rectWnd.right = m_rectWnd.left + sz.cx - 1;
		m_rectWnd.bottom = m_rectWnd.top + sz.cy - 1;

		SetupRegion();
		Invalidate();

	}
	else if(m_FrameGrabber.GetSafeHwnd())
	{
		CSize sz = m_FrameGrabber.GetImageSize();
		SetWindowPos(NULL,0,0, sz.cx-1, sz.cy-1, SWP_NOMOVE|SWP_NOZORDER);

		m_rectWnd.right = m_rectWnd.left + sz.cx - 1;
		m_rectWnd.bottom = m_rectWnd.top + sz.cy - 1;
		
		/*
		CRect rcc;
		CRect rcw;
		GetClientRect(&rcc);
		GetWindowRect(&rcw);
		int dx = rcw.Width()-rcc.Width();
		int dy = rcw.Height()-rcc.Height();

		if(sz.cx && (rcc.Width()!=sz.cx || rcc.Height()!=sz.cy))
		{
			SetWindowPos(NULL,0,0,
						 sz.cx+dx, sz.cy+dy, SWP_NOMOVE|SWP_NOZORDER);
			
				GetParentFrame()->GetClientRect(&rcc);
				GetParentFrame()->GetWindowRect(&rcw);
				dx+= rcw.Width()-rcc.Width();
				dy+= rcw.Height()-rcc.Height();

			GetParentFrame()->SetWindowPos(NULL,0,0,
						sz.cx+dx, sz.cy+dy, SWP_NOMOVE|SWP_NOZORDER);
		}
		*/

		SetupRegion();
		Invalidate();
	}

}

//Dialogs does not appears for large image....

void CVideoWnd::OnContextvideoEdittransparencyrefreshrate() 
{
	// TODO: Add your command handler code here
	//if (versionOp<5) {
	//	int ret = MessageBox("This feature is only available in Win 2000/ XP." ,"Note",MB_OK | MB_ICONEXCLAMATION);
	//	return;
	//}
	//editTransOn = 1;

	CTransRateDialog etDlg;
	etDlg.PreModal(this);
	etDlg.DoModal();

	//editTransOn = 0;
	OnUpdateContextMenu();
	
}

void CVideoWnd::OnContextvideoEdittransparency() 
{
	// TODO: Add your command handler code here
	if (versionOp<5) {
		//int ret = MessageBox("This feature is only available in Win 2000/ XP." ,"Note",MB_OK | MB_ICONEXCLAMATION);
		int ret = MessageOut(this->m_hWnd,IDS_STRING_AVAILXP ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	if (refreshRate>10)
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
