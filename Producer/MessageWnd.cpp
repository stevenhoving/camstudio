//********************************************************************************
//* TransparentWindow.CPP
//*
//* A transparent window class.
//*
//* Based on the idea of Jason Wylie
//*
//* (C) 1998 by Franz Polzer
//*
//* Visit me at:	stud1.tuwien.ac.at/~e9225140
//* Write to me:	e9225140@student.tuwien.ac.at
//********************************************************************************

#include "stdafx.h"
#include "MessageWnd.h"
#include "resource.h"
#include <windowsx.h>

#include <assert.h>


extern void ErrMsg(char format[], ...);
extern HWND hWndGlobal;
extern int versionOp;
extern int maxxScreen;
extern int maxyScreen;

HANDLE  AllocMakeDib( int reduced_width, int reduced_height, UINT bits );
extern HANDLE Bitmap2Dib(HBITMAP, UINT);
extern CString GetTempPath();

long currentWndID = 0;

//important: this window does not create or delete the m_hbitmap passed to it

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int AreWindowsEdited();
extern int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus);



//********************************************************************************
//* Constructor
//********************************************************************************

CTransparentWnd::CTransparentWnd()
{


	uniqueID = currentWndID;
	currentWndID++;
	if (currentWndID > 2147483647)
		currentWndID = 0;		

	saveMethod = saveMethodNew;
	

	m_textstring = "Please Wait";
	
	ZeroMemory(&m_textfont, sizeof(LOGFONT));
	m_textfont.lfHeight = 12;
	m_textfont.lfWidth = 8;	
	strcpy(m_textfont.lfFaceName,"Arial");
	rgb = RGB(0,0,0);

	
	m_tracker.m_rect.left = 20;
	m_tracker.m_rect.top = 20;
	m_tracker.m_rect.right = 160;
	m_tracker.m_rect.bottom = 140;

	m_tracker.m_nStyle = CRectTracker::dottedLine | CRectTracker::resizeOutside;
	m_tracker.m_sizeMin.cx = 64;
	m_tracker.m_sizeMin.cx = 48;

	m_factor = 2;

	m_horzalign = DT_CENTER;

	m_charset = ANSI_CHARSET;
	
	m_rectWnd.left =0;
	m_rectWnd.top = 0;
	m_rectWnd.right =180;
	m_rectWnd.bottom = 160;

	trackingOn = 0;
	editTransOn = 0;
	editImageOn = 0;


	enableTransparency = 0;
	valueTransparency = 50;

	menuLoaded = 0;
	m_movewindow = 0;

	m_regionCreated = 0;
	m_transparentColor = RGB(0,0,0);
	//m_regionType = regionTRANSPARENTCOLOR; 
	m_regionType = regionNULL; 
	m_hbitmap = NULL;

	m_regionPredefinedShape = regionROUNDRECT;
	m_roundrectFactor = 0.25;

	m_borderYes = 0;
	m_borderColor = RGB(0,0,128);
	m_borderSize = 5;

	m_backgroundColor = RGB(255,255,255);

	m_shapeStr = "Label";
	
	baseType = 0;

	//WidthHeight
	widthPos = 32;
	heightPos = 32;
	m_rectOriginalWnd = m_rectWnd;
  
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
	ON_WM_LBUTTONDOWN()	
	ON_WM_SETCURSOR()	
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP	
END_MESSAGE_MAP()
	

	
//********************************************************************************
//* CreateTransparent()
//*
//* Creates the main application window transparent
//********************************************************************************


BOOL CTransparentWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CWnd::PreCreateWindow(cs);
}

void CTransparentWnd::CreateTransparent(LPCTSTR pTitle, RECT rect,  HBITMAP BitmapID)
{

		
	CreateEx(	WS_EX_TOPMOST ,	
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

	
	if ((m_rectWnd.Width()>60) &&  (m_rectWnd.Height()>60))
	{
		m_tracker.m_rect.left =  20;
		m_tracker.m_rect.top =  20;
		m_tracker.m_rect.right = rect.right - rect.left - 20;
		m_tracker.m_rect.bottom = rect.bottom - rect.top - 20;	

	}
	else {
		
		m_tracker.m_rect.left = 1;
		m_tracker.m_rect.top = 1;
		m_tracker.m_rect.right = m_rectWnd.Width() - 2;
		m_tracker.m_rect.bottom = m_rectWnd.Height() - 2;

	}


	m_hbitmap = BitmapID;

	SetupRegion();

}


void CTransparentWnd::CreateTransparent(LPCTSTR pTitle, RECT rect,  CString bitmapFile, int fitBitmapSize)
{

	m_rectWnd = rect;

	//WidthHeight
	m_rectOriginalWnd = m_rectWnd;
	
	CreateEx(	WS_EX_TOPMOST ,	
						AfxRegisterWndClass(0),
						pTitle,
						WS_POPUP | WS_SYSMENU,
						rect,
						NULL,
						NULL,
						NULL );	
						
	

	if ((m_rectWnd.Width()>60) &&  (m_rectWnd.Height()>60))
	{
		m_tracker.m_rect.left =  20;
		m_tracker.m_rect.top =  20;
		m_tracker.m_rect.right = rect.right - rect.left - 20;
		m_tracker.m_rect.bottom = rect.bottom - rect.top - 20;	

	}
	else {
		
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
	}
	else if (m_regionType == regionTRANSPARENTCOLOR)
		SetupRegionByTransColor(pDC,m_transparentColor);
	else if (m_regionType == regionSHAPE)
	{

		m_regionCreated = 0; 

		CRect cRect;
		cRect = m_rectWnd;

		if (wndRgn.m_hObject) 
			wndRgn.DeleteObject();

		//trace rectwnd

		double rx,ry,rval;
		rx= cRect.Width() * m_roundrectFactor;
		ry= cRect.Height() * m_roundrectFactor;
		if (rx>ry)
			rval = ry;
		else
			rval = rx;


		if (m_regionPredefinedShape == regionROUNDRECT)
			wndRgn.CreateRoundRectRgn(0, 0, cRect.Width()-1, cRect.Height()-1, (int) rval, (int) rval);
		else if (m_regionPredefinedShape == regionELLIPSE)
			wndRgn.CreateEllipticRgn( 0, 0, cRect.Width()-1, cRect.Height()-1);
		else 
			wndRgn.CreateRectRgn(0, 0, cRect.Width()-1, cRect.Height()-1);


		SetWindowRgn((HRGN)wndRgn, TRUE);

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

	CDC				memDC;
	CBitmap			cBitmap, zoomBitmap;
	CBitmap			*old_zoomBitmap;
	HBITMAP		     pOldMemBmp = NULL;
	COLORREF		col;
	CRect			cRect;
	int				x, y;

	
	//need to make wndRgn a member...???
	CRgn rgnTemp;
	GetWindowRect(&cRect);
	
	
	
	memDC.CreateCompatibleDC(pDC);
	//pOldMemBmp = memDC.SelectObject(&cBitmap);
	

	//if window is zoomed, create a resized-bitmap for computation
	int needCleanZoom = 0;
	if ((m_rectWnd.Width() != m_rectOriginalWnd.Width()) ||
		(m_rectWnd.Height() != m_rectOriginalWnd.Height()))
	{

				
		zoomBitmap.CreateCompatibleBitmap(pDC,cRect.Width(),cRect.Height());
		old_zoomBitmap =  (CBitmap *) memDC.SelectObject(&zoomBitmap);							
				
		CDC tempDC;	
		HBITMAP		     pOldMemBmp = NULL;	
		tempDC.CreateCompatibleDC(pDC);		
		
		pOldMemBmp = (HBITMAP) ::SelectObject(tempDC.m_hDC,m_hbitmap);				 
		memDC.StretchBlt(0, 0, cRect.Width(), cRect.Height(), &tempDC, 0, 0, m_rectOriginalWnd.Width(), m_rectOriginalWnd.Height(), SRCCOPY);
			
		::SelectObject(tempDC.m_hDC,pOldMemBmp);	
		tempDC.DeleteDC();	

		needCleanZoom = 1; 

	}
	else
		pOldMemBmp = (HBITMAP) ::SelectObject(memDC.m_hDC,m_hbitmap);
	

	if (wndRgn.m_hObject) 
		wndRgn.DeleteObject();
	wndRgn.CreateRectRgn(0, 0, cRect.Width(), cRect.Height());

	

	
	int     yStart;
    BOOL    bStart=FALSE;
    for(x=0; x<=cRect.Width(); x++)
    {
            for(y=0; y<=cRect.Height(); y++)
            {
                    col = memDC.GetPixel(x, y);
                    if(col == transColor)
                    {
                            if(!bStart)
                            {
                                    yStart=y;
                                    bStart=TRUE;
                            }
                    }
                    else
                    {
                            if(bStart)
                            {
                                    rgnTemp.CreateRectRgn(x, yStart, x+1, y);
                                    wndRgn.CombineRgn(&wndRgn, &rgnTemp, RGN_DIFF);
                                    rgnTemp.DeleteObject(); 
                            }
                            bStart=FALSE;
                    }
            }
            // End of column
            if(bStart)
            {
                    rgnTemp.CreateRectRgn(x, yStart, x+1, y);
                    wndRgn.CombineRgn(&wndRgn, &rgnTemp, RGN_XOR);
                    rgnTemp.DeleteObject(); 
                    bStart=FALSE;
            }
    }


	if (needCleanZoom)
	{	
		memDC.SelectObject(old_zoomBitmap);					
		zoomBitmap.DeleteObject();

	}
	else
		::SelectObject(memDC.m_hDC,pOldMemBmp);	
	
	memDC.DeleteDC();

	SetWindowRgn((HRGN)wndRgn, TRUE);

	m_regionCreated = 1;
}


//********************************************************************************
//* CTransparentWnd message handlers
//********************************************************************************



BOOL CTransparentWnd::OnEraseBkgnd(CDC* pDC) 
{	
	
	return TRUE;
}


void CTransparentWnd::DrawStuff(CDC* dc)
{

	// Add your drawing code here!
	//CDC *pDC = &dc;
	CDC *pDC = dc;
	

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


	//BitBlt Background
	CRect	rect,cRect;
	GetWindowRect(&rect);	
	cRect = rect;

	if (m_hbitmap) {

		CDC memDC;	
		HBITMAP		     pOldMemBmp = NULL;	
		memDC.CreateCompatibleDC(pDC);
		pOldMemBmp = (HBITMAP) ::SelectObject(memDC.m_hDC,m_hbitmap);	
	
		//WidthHeight
		//pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
		pDC->StretchBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, m_rectOriginalWnd.Width(), m_rectOriginalWnd.Height(), SRCCOPY);
		

		::SelectObject(memDC.m_hDC,pOldMemBmp);	
		memDC.DeleteDC();	

	}
	else 
	{
		pDC->FillSolidRect(0,0,rect.Width(),rect.Height(),m_backgroundColor);

	}

	
	if ((m_factor==1) || (trackingOn))
	{
	
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
			

	}
	else {
		
		LPBITMAPINFO pbmiText = GetTextBitmap(pDC, &CRect(clrect),m_factor,&m_tracker.m_rect, &m_textfont, m_textstring, NULL, NULL, rgb, m_horzalign); 
		HBITMAP newbm = DrawResampleRGB(pDC, &CRect(clrect),m_factor, (LPBITMAPINFOHEADER) pbmiText); 

		if (pbmiText) {
			GlobalFreePtr(pbmiText);
			pbmiText = NULL;

		}

	}

	if (trackingOn)
		m_tracker.Draw(pDC);


}


void CTransparentWnd::OnPaint()
{
	CPaintDC dc(this);

	DrawStuff(&dc);
	
	
}


void CTransparentWnd::UpdateNoWait()
{

	CDC * winDC = GetWindowDC();
	DrawStuff(winDC);
	ReleaseDC(winDC);


}





LPBITMAPINFO CTransparentWnd::GetDCBitmap(CDC *thisDC, CRect* caprect) 
{

	int left =caprect->left;
	int top = caprect->top;
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

	
	HBITMAP hbm;	
    hbm = CreateCompatibleBitmap(thisDC->m_hDC, width, height);
	HBITMAP oldbm = (HBITMAP) SelectObject(hMemDC, hbm);	 	
	
	//Get Background
	::StretchBlt(hMemDC, 0, 0, width, height, thisDC->m_hDC, left, top, orig_width,orig_height,SRCCOPY);	

	CPen* oldPen;
	CBrush* oldBrush;	
	CFont dxfont, *oldfont;
		
	
	if (drawfont) {

		LOGFONT newlogFont = *drawfont;
		newlogFont.lfWidth *= factor;
		newlogFont.lfHeight *= factor; 

		//need CreateFontIndirectW ?
		dxfont.CreateFontIndirect(&newlogFont);
		oldfont = (CFont *) pMemDC->SelectObject(&dxfont);
	}
	//if no default font is selected, can cause errors

	
	
	if (pPen)
		oldPen = pMemDC->SelectObject(pPen);

	if (pBrush)
		oldBrush = pMemDC->SelectObject(pBrush);

	int textlength = textstr.GetLength(); //get number of bytes
	

	//Draw Text

	SetBkMode(hMemDC,TRANSPARENT); 
	SetTextColor(hMemDC,textcolor);
	//DrawTextEx(hMemDC, (char *)LPCTSTR(textstr), textlength, LPRECT(usetextRect), horzalign | DT_VCENTER | DT_WORDBREAK | DT_EDITCONTROL ,  NULL);
		
	//use adaptive antialias...if size< than maxxScreen maxyScreen
	if ((usetextRect.Width()>maxxScreen) || (usetextRect.Height()>maxyScreen))
	{   //use stroke path method, less buggy
	
		BeginPath(hMemDC);
		DrawTextEx(hMemDC, (char *)LPCTSTR(textstr), textlength, LPRECT(usetextRect), horzalign | DT_VCENTER | DT_WORDBREAK  ,  NULL);
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

	}
	else
		DrawTextEx(hMemDC, (char *)LPCTSTR(textstr), textlength, LPRECT(usetextRect), horzalign | DT_VCENTER | DT_WORDBREAK  , NULL);
	

	
	
	if ((m_borderYes) && (m_regionType==regionSHAPE)) {

				CRect cRect;
				cRect.left = left; 
				cRect.top = top;
				cRect.right = cRect.left + width - 1;
				cRect.bottom = cRect.top + height - 1;
				
				double rx,ry,rval;
				rx= cRect.Width() * m_roundrectFactor;
				ry= cRect.Height() * m_roundrectFactor;
				if (rx>ry)
					rval = ry;
				else
					rval = rx;

				CPen borderPen;
				CPen* oldPen;
				borderPen.CreatePen( PS_SOLID , m_borderSize*m_factor, m_borderColor );
				oldPen = (CPen *) pMemDC->SelectObject(&borderPen);

				LOGBRUSH logbrush;
				CBrush borderBrush;
				CBrush* oldBrush;
				logbrush.lbStyle = BS_HOLLOW;				
				borderBrush.CreateBrushIndirect(&logbrush);

				oldBrush = (CBrush *) pMemDC->SelectObject(&borderBrush);  
				
				int drawOffset = (m_borderSize*m_factor)/2;

				if (m_regionPredefinedShape == regionROUNDRECT)
				{					
					pMemDC->RoundRect(drawOffset-1, drawOffset-1, cRect.Width()-1-drawOffset, cRect.Height()-1-drawOffset, (int) rval, (int) rval);

				}
				else if (m_regionPredefinedShape == regionELLIPSE)
				{
					pMemDC->Ellipse(drawOffset-1,drawOffset-1, cRect.Width()-1-drawOffset, cRect.Height()-1-drawOffset);
				}
				else if (m_regionPredefinedShape == regionRECTANGLE)
				{
					pMemDC->Rectangle(drawOffset-1, drawOffset-1, cRect.Width()-1-drawOffset, cRect.Height()-1-drawOffset);

				}

				pMemDC->SelectObject(oldBrush);
				pMemDC->SelectObject(oldPen);
				borderPen.DeleteObject();
				borderBrush.DeleteObject();

	}

 	
	
	if (pBrush)
		pMemDC->SelectObject(oldBrush);
	
	if (pPen)
		pMemDC->SelectObject(oldPen);	

	
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
HBITMAP CTransparentWnd::DrawResampleRGB(CDC *thisDC, CRect* caprect,int factor, LPBITMAPINFOHEADER expanded_bmi) 
{

	int bits = 24;
					 
	LONG   Width =   expanded_bmi->biWidth;
	LONG   Height =  expanded_bmi->biHeight;	
	long Rowbytes = (Width*bits+31)/32 *4;

	long reduced_width = Width/factor;
	long reduced_height = Height/factor;
	long reduced_rowbytes = (reduced_width*bits+31)/32 *4;


	if ((factor<1) || (factor>3))
		return NULL;


	//Create destination buffer	
	//long dwSize = sizeof(BITMAPINFOHEADER) + reduced_rowbytes * reduced_height * 3;


	// Allocate room for a DIB and set the LPBI fields
	LPBITMAPINFOHEADER smallbi = (LPBITMAPINFOHEADER)GlobalLock(AllocMakeDib( reduced_width, reduced_height, 24));
	if (smallbi == NULL) { 
		return NULL;
	}    	
	
	// Get the bits from the bitmap and stuff them after the LPBI	
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

				if (factor==1)
					*reduced_ptr = *Ptr;				
				else  if (factor==2) {

					totalval = 0;
					totalval += *Ptr;
					totalval += *(Ptr + 3) ;
					totalval += *(Ptr + Rowbytes) ;
					totalval += *(Ptr + Rowbytes + 3) ;
					totalval/=4;

					if (totalval<0)
						totalval = 0;

					if (totalval>255)
						totalval = 255;

					*reduced_ptr = (BYTE) totalval;

				}
				else  if (factor==3) {

					totalval = 0;
					totalval += *Ptr;
					totalval += *(Ptr + 3) ;
					totalval += *(Ptr + 6) ;
					
					totalval += *(Ptr + Rowbytes) ;
					totalval += *(Ptr + Rowbytes + 3) ;
					totalval += *(Ptr + Rowbytes + 6) ;
					
					totalval += *(Ptr + Rowbytes + Rowbytes) ;
					totalval += *(Ptr + Rowbytes + Rowbytes + 3) ;
					totalval += *(Ptr + Rowbytes + Rowbytes + 6) ;
									
					totalval/=9;

					if (totalval<0)
						totalval = 0;

					if (totalval>255)
						totalval = 255;

					*reduced_ptr = (BYTE) totalval;
				
				} //else if factor
				

				reduced_ptr++;
				Ptr ++;			

			} //for  z

			Ptr += Ptr_incr;
			//Ptr += factor * 3; 


		} //for x
		 
		reduced_rowptr += reduced_rowbytes;
		//Rowptr += Rowbytes;
		Rowptr += Row_incr;

	}	// for y
	
	
	int ret = StretchDIBits ( thisDC->m_hDC,
								0, 0,reduced_width,reduced_height,
								0, 0,reduced_width,reduced_height,                        
								lpBits, (LPBITMAPINFO)smallbi,
								DIB_RGB_COLORS,SRCCOPY);				



	

	if (smallbi)
		GlobalFreePtr(smallbi);

		
	HBITMAP newbm = NULL;
	return newbm;

}


void CTransparentWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	CWnd::OnLButtonDown(nFlags, point);	

}



void CTransparentWnd::OnLButtonUp(UINT nFlags, CPoint point)
{

	

	CWnd::OnLButtonUp(nFlags, point);

}

void CTransparentWnd::OnMouseMove(UINT nFlags, CPoint point)
{


	 CWnd::OnMouseMove(nFlags, point);
	
}




void CTransparentWnd::InvalidateRegion()
{

	//m_regionCreated = 0;    //put this here?
	
	SetupRegion();

}


void CTransparentWnd::SetupRegion()
{

	CDC* tempDC = GetWindowDC();
	SetupRegion(tempDC);
	ReleaseDC(tempDC);


}

//WidthHeight
void CTransparentWnd::RefreshWindowSize() 
{
	double widthfract = widthPos*0.025 + 0.2;
	double heightfract = heightPos*0.025 + 0.2;
	
	m_rectWnd.right = m_rectWnd.left + (long) ((double) (m_rectOriginalWnd.Width()) * widthfract);
	m_rectWnd.bottom = m_rectWnd.top + (long) ((double) (m_rectOriginalWnd.Height()) * heightfract);

	if (m_rectWnd.Width()<40)
	{
		m_rectWnd.right = m_rectWnd.left + 40 - 1;

	}

	if (m_rectWnd.Width()>maxxScreen)
	{
		m_rectWnd.right = m_rectWnd.left + maxxScreen - 1;

	}

	if (m_rectWnd.Height()<40)
	{
		m_rectWnd.bottom = m_rectWnd.top + 40 - 1;

	}

	if (m_rectWnd.Height()>maxyScreen)
	{
		m_rectWnd.bottom = m_rectWnd.top + maxyScreen - 1;

	}

		
	SetWindowPos( &wndTopMost, m_rectWnd.left, m_rectWnd.top,m_rectWnd.right - m_rectWnd.left + 1 , m_rectWnd.bottom - m_rectWnd.top + 1, SWP_NOMOVE  );
	Invalidate();

}


//WidthHeight

void CTransparentWnd::OnSysCommand(UINT nID, LPARAM lParam)
{

	CWnd::OnSysCommand(nID, lParam);	
	

}



//need to unlock to use it and then
//use GlobalFreePtr to free it
HANDLE  AllocMakeDib( int reduced_width, int reduced_height, UINT bits )
{
	HANDLE              hdib ;	
	UINT                wLineLen ;
	DWORD               dwSize ;
	DWORD               wColSize ;
	LPBITMAPINFOHEADER  lpbi ;
	LPBYTE              lpBits ;
	
	//
	// DWORD align the width of the DIB
	// Figure out the size of the colour table
	// Calculate the size of the DIB
	//
	wLineLen = (reduced_width*bits+31)/32 * 4;
	wColSize = sizeof(RGBQUAD)*((bits <= 8) ? 1<<bits : 0);
	dwSize = sizeof(BITMAPINFOHEADER) + wColSize +
		(DWORD)(UINT)wLineLen*(DWORD)(UINT)reduced_height;

	//
	// Allocate room for a DIB and set the LPBI fields
	//
	hdib = GlobalAlloc(GHND,dwSize);
	if (!hdib)
		return hdib ;

	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib) ;

	lpbi->biSize = sizeof(BITMAPINFOHEADER) ;
	lpbi->biWidth = reduced_width ;
	lpbi->biHeight = reduced_height ;
	lpbi->biPlanes = 1 ;
	lpbi->biBitCount = (WORD) bits ;
	lpbi->biCompression = BI_RGB ;
	lpbi->biSizeImage = dwSize - sizeof(BITMAPINFOHEADER) - wColSize ;
	lpbi->biXPelsPerMeter = 0 ;
	lpbi->biYPelsPerMeter = 0 ;
	lpbi->biClrUsed = (bits <= 8) ? 1<<bits : 0;
	lpbi->biClrImportant = 0 ;

	//
	// Get the bits from the bitmap and stuff them after the LPBI
	//
	lpBits = (LPBYTE)(lpbi+1)+wColSize ;
	lpbi->biClrUsed = (bits <= 8) ? 1<<bits : 0;

	
	GlobalUnlock(hdib);

	return hdib ;
}
