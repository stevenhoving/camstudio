#include "stdafx.h"
#include "resource.h"
#include "SlideToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAXTIMERANGE  30000
extern long timeStart;
extern long timeLength;

extern HWND viewWnd;
extern CSliderCtrl *sliderCtrlPtr;
extern void SetScrollTime(long time);
extern long playtime;
extern BOOL gfPlaying;

int SliderPosition = 7;

#define PLAYER  0
#define DUBBER  1
extern int pmode;

extern void SetTimeIndicator(CString timestr);

/////////////////////////////////////////////////////////////////////////////
// CSlideToolBar

CSlideToolBar::CSlideToolBar()
{
	
	m_slidevalue=0;
	if (pmode == DUBBER)
		SliderPosition = 7;
	else if (pmode == PLAYER)
		SliderPosition = 6;
		
}

CSlideToolBar::~CSlideToolBar()
{
}


BEGIN_MESSAGE_MAP(CSlideToolBar, CToolBar)
	//{{AFX_MSG_MAP(CSlideToolBar)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSlideToolBar message handlers

int CSlideToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//##################################################//
    // Load the dummy buttons toolbar.
	//
    if (!LoadToolBar (IDR_MAINFRAME))
        return -1;



	CRect rect;
	
	int sliderheight = 22;
	int offset = 0; 
	int initialwidth = 200; 
	
	SetButtonInfo (SliderPosition, ID_SLIDER, TBBS_SEPARATOR, initialwidth);
	GetItemRect (SliderPosition, &rect);    

	if (rect.Height() - 2  < sliderheight )
		sliderheight = rect.Height() - 2;

	offset = rect.Height() - sliderheight; 
	offset /=2;
	
	rect.top = rect.top + offset ;
	rect.bottom = rect.bottom - offset ;

	
	//CString msx;
	//msx.Format("offset %d, recttop %d, rectbottom %d, rectleft %d, rectright %d", offset, rect.top, rect.bottom, rect.left, rect.right);
	//MessageBox(msx,"note",MB_OK);

	
	if (!m_wndSliderCtrl.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP |// dwStyle
						TBS_HORZ |
						//TBS_AUTOTICKS |
						TBS_NOTICKS |
						TBS_BOTH,
						rect,					// rect
						this,		// CWnd* pParentWnd
						ID_SLIDER))		// UINT  nID
	{
		return -1;
	}




    	
	SetPositions();

	sliderCtrlPtr = &m_wndSliderCtrl;

	return 0;

}


void CSlideToolBar::SetPositions()
{
	m_wndSliderCtrl.SetRange(0, MAXTIMERANGE);
	m_wndSliderCtrl.SetPos(m_slidevalue);
	//m_wndSliderCtrl.SetPageSize(100);
	//m_wndSliderCtrl.SetLineSize(1);

		
}

void CSlideToolBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	//accept scrolling only if non playing ? 
	//accept only if a file exists ? 

	
	if (!gfPlaying) {

		CSliderCtrl* m_wndSliderCtrl = (CSliderCtrl*) pScrollBar;	
		long value = m_wndSliderCtrl->GetPos();
		value = timeStart + (value * timeLength) / MAXTIMERANGE;

		if (value < timeStart) 
			value  = timeStart;

		if (value > timeStart + timeLength - 1) 
			value  = timeStart + timeLength - 1;
			
		playtime = value;


		if (timeLength > 0) {

			float durationPlayed = (float) ((playtime - timeStart)/1000.0);
			CString durationStr;
			durationStr.Format("%8.1f sec",durationPlayed);
			SetTimeIndicator(durationStr);	

		}

		::InvalidateRect(viewWnd, NULL, FALSE);

		UpdateValues();

		//HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_SLIDER); 
		//::SetCursor(hCursor);

		CToolBar::OnHScroll(nSBCode, nPos, pScrollBar);

	}
		
	
}

void CSlideToolBar::UpdateValues()
{


}



void CSlideToolBar::AdjustTimeBar(CRect clientrect)
{

	CRect rect;	
	GetItemRect (SliderPosition, &rect);    

	int sliderheight = 22;
	int offset = 0; 
	int initialwidth = 200; 
	
	if (rect.Height() - 2  < sliderheight )
		sliderheight = rect.Height() - 2;

	offset = rect.Height() - sliderheight; 
	offset /=2;
	
	rect.top = rect.top + offset ;
	rect.bottom = rect.bottom - offset ;
	
	rect.right = clientrect.right - 10;

	m_wndSliderCtrl.MoveWindow( rect.left, rect.top, rect.right - rect.left, rect.Height(),TRUE );

}


void CSlideToolBar::EnableButton(BOOL setToOn, int nIndex)
{
	//m_bEnableChanged = TRUE;	
	//nIndex = CommandToIndex( nIndex );
	
	/*
	UINT nNewStyle = GetButtonStyle(nIndex) & ~TBBS_DISABLED;
	
	if (!setToOn)
	{
		nNewStyle |= TBBS_DISABLED;
		nNewStyle &= ~TBBS_PRESSED;
	}	
	SetButtonStyle(nIndex, nNewStyle);
	*/
	
	
}
