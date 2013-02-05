#include "StdAfx.h"
#include "Screen.h"
#include "ximage.h"
#include "RecorderView.h"
#include "../hook/ClickQueue.hpp"
#include "addons/EffectsOptions.h"
#include "addons/XnoteStopwatchFormat.h"
#include <gdiplus.h>
using namespace Gdiplus;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CCamera::CCamera()
{
}

CCamera::~CCamera()
{
}

bool CCamera::AddTimestamp(CDC* pDC)
{
	if (m_sTimestamp.m_bAnnotation) {
		CString str;
		TextAttributes tmpTimestamp;
		//char TimeBuff[256];
		struct tm   *newTime;
		time_t      szClock;
		time( &szClock );
		newTime = localtime( &szClock );
		tmpTimestamp = m_sTimestamp.m_taTimestamp;
		strftime(tmpTimestamp.text.GetBuffer(256), 256, m_sTimestamp.m_taTimestamp.text, newTime);
		InsertText(pDC, m_rectFrame, tmpTimestamp);
	}
	return true;
}

/////////////////////////////////////
// AddXNote
//
// Write stopwatchinfo to screen
// Firts line : Activation info. Which appliaction started and was this manually of automatic started.
// Second line: Show stopwatch running digits
// Third line : Extended info about the last (three) stopwatch snaps.
//
/////////////////////////////////////
bool CCamera::AddXNote(CDC* pDC)
{
	if (m_sXNote.m_bAnnotation) {

		// CString str;
		TextAttributes taTmpXNote;
		char cTmpBuffXNoteTimeStamp[128]= "" ;
		DWORD dwCurrTickCount =  GetTickCount();

		// Determine is Xnote Stopwatch is still running. If not just show regular time.
		CXnoteStopwatchFormat::FormatXnoteDelayedTimeString( 
				cTmpBuffXNoteTimeStamp, 
				cXNoteOpts.m_ulStartXnoteTickCounter, 
				cXNoteOpts.m_ulStartXnoteTickCounter == 0 ? 0 : dwCurrTickCount,	// Determine is Xnote Stopwatch is still running. If not, show only zero's
				m_sXNote.m_ulXnoteCameraDelayInMilliSec, 
				cXNoteOpts.m_bXnoteDisplayCameraDelayMode,
				cXNoteOpts.m_bXnoteDisplayCameraDelayDirection);

		// Load info how and where user defined to dispaly the annotation
		taTmpXNote = m_sXNote.m_taXNote;

		// Extend stopwatch info by adding on the last line the (three last) xnote stopwatch snaptimes
		(void) sprintf( taTmpXNote.text.GetBuffer(128), "%s\n%s\n%s", 
				cXNoteOpts.m_ulStartXnoteTickCounter == 0 ? "No stopwatch running" : cXNoteOpts.m_cXnoteStartEntendedInfo ,
				cTmpBuffXNoteTimeStamp, 
				cXNoteOpts.m_ulStartXnoteTickCounter == 0 ? "Waiting..." : cXNoteOpts.m_cSnapXnoteTimesString ); 
		

		InsertText(pDC, m_rectFrame, taTmpXNote);

		// Determine if we must switch from recording to pause mode.
		CRecorderView::XNoteSetRecordingInPauseMode();

	}
	return true;
}



bool CCamera::AddCaption(CDC* pDC)
{
	if (m_sCaption.m_bAnnotation){
		InsertText(pDC, m_rectFrame, m_sCaption.m_taCaption);
	}
	return true;
}

bool CCamera::LoadWatermark()
{
	m_imageWatermark.Destroy();
	m_strWatermarkName = m_sWatermark.m_iaWatermark.text;// cache
	CString extin(FindExtension(m_strWatermarkName));
	extin.MakeLower();
	int typein = CxImage::GetTypeIdFromName(extin);
	if (typein == CXIMAGE_FORMAT_UNKNOWN) {
		m_strWatermarkName = "";
		return false;
	}

	if (!m_imageWatermark.Load(m_strWatermarkName, typein)){
		m_strWatermarkName = "";
		return false;
	}

	m_imageWatermark.Light(m_sWatermark.m_iaWatermark.m_lBrightness, m_sWatermark.m_iaWatermark.m_lContrast);
	m_imageWatermark.SetTransIndex(0);
	m_imageWatermark.SetTransColor(m_imageWatermark.GetPixelColor(0L, 0L));
	return true;
}

bool CCamera::AddWatermark(CDC* pDC)
{
	if (!m_sWatermark.m_bAnnotation) {
		return m_sWatermark.m_bAnnotation;
	}
	InsertImage(pDC, m_rectFrame, m_sWatermark.m_iaWatermark);

	return true;
}
bool CCamera::AddCursor(CDC* pDC)
{
	CPoint ptCursor;
	VERIFY(::GetCursorPos(&ptCursor));
	ptCursor.x -= _zoomFrame.left;
	ptCursor.y -= _zoomFrame.top;
	double zoom = m_rectView.Width()/(double)_zoomFrame.Width(); // TODO: need access to zoom in this class badly
	ptCursor.x *= zoom;
	ptCursor.y *= zoom;

	// TODO: This shift left and up is kind of bogus.
	// The values are half the width and height of the higlight area.
	InsertHighLight(pDC, ptCursor);

	// Draw the Cursor
	HCURSOR hcur = m_cCursor.Cursor();
	ICONINFO iconinfo;
	BOOL ret = ::GetIconInfo(hcur, &iconinfo);
	if (ret) {
		ptCursor.x -= iconinfo.xHotspot;
		ptCursor.y -= iconinfo.yHotspot;

		// need to delete the hbmMask and hbmColor bitmaps
		// otherwise the program will crash after a while
		// after running out of resource
		// TODO: can we cache it and don't use GetIconInfo every frame?
		// We make several shots per second it will save us some resources if cursor is not changed
		if (iconinfo.hbmMask) {
			::DeleteObject(iconinfo.hbmMask);
		}
		if (iconinfo.hbmColor) {
			::DeleteObject(iconinfo.hbmColor);
		}
	}
	// TODO: Rewrite to handle better
	// HDC hScreenDC = ::GetDC(NULL);
	// HDC hMemDC = ::CreateCompatibleDC(hScreenDC);
	// ::DrawIconEx( hMemDC, ptCursor.x, ptCursor.y, hcur, 0, 0, 0, NULL, DI_NORMAL);
	pDC->DrawIcon(ptCursor.x, ptCursor.y, hcur);
	return true;
}

// visualize mouse events queue
bool CCamera::AddClicks(CDC* pDC)
{
	Graphics g(pDC->GetSafeHdc());
	g.SetSmoothingMode(SmoothingModeAntiAlias);
	g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	Color c;
	c.SetValue(m_cCursor.ClickLeftColor());
	Pen penLeft(c, m_cCursor.m_fRingWidth);
	c.SetValue(m_cCursor.ClickRightColor());
	Pen penRight(c, m_cCursor.m_fRingWidth);
	Pen penMiddle(m_cCursor.m_clrClickMiddle, m_cCursor.m_fRingWidth);

	DWORD now = GetTickCount();
	DWORD threshold = m_cCursor.m_iRingThreshold;

	DWORD ago;

	ClickQueue& cc = ClickQueue::getInstance();
	cc.Lock(); // different thread of the same process puts events in queue
	ClickQueue::QueueType& queue = cc.getQueue();

	ClickQueue::Iterator iter, iter2; // iter2 is used to clean up "down" events when "up" happend
	int maxsize = m_cCursor.m_iRingSize;
	// Remove expired events
	iter=queue.begin();
	while ((iter != queue.end()) && ((now - iter->time) > threshold)) // how to deal with integer overflow?
		++iter;
   //_ASSERTE(_CrtCheckMemory());
	if (iter != queue.begin()) // TODO: good place to dump events to log file before discarding them
		queue.erase(queue.begin(), iter);

	for(iter = queue.begin(); iter != queue.end(); ++iter)
	{
		ago = now - iter->time;
		int size = maxsize * ago / threshold;
		bool found = false;
		switch(iter->flags) // holds MW_ for mouse
		{	// once button is released we should remove all previous down events
		case 0xFFFFFFFF: continue; // <= WM_xBUTTONDOWN special case when button was released. Let it expire
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			iter2 = iter;
			while(iter2 != queue.begin() && !found) {
				--iter2;
				if(iter2->flags == iter->flags-1) { // DOWN and UP are 1 apart
					iter2->flags = 0xFFFFFFFF; // there should be no other down's actually
					found = true;
				}
			}
			break;
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			size = maxsize - size;//maxsize * (threshold - ago) / threshold;
			break;
		}
		Pen * pen;
		switch(iter->flags) // color {
		{
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			pen = &penRight;
			break;
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
			pen = &penMiddle;
			break;
		default:
			pen = &penLeft;
		} // } color
		CPoint pt = CPoint(iter->pt);
		pt.x -= _zoomFrame.left;
		pt.y -= _zoomFrame.top;
		double zoom = m_rectView.Width()/(double)_zoomFrame.Width();
		pt.x *= zoom;
		pt.y *= zoom;

		if (iter->flags == WM_MOUSEWHEEL)
		{
			int delta = iter->mouseData;
			delta >>= 16; // high word has delta
			g.DrawLine(pen, pt.x - size, pt.y, pt.x, pt.y - size * delta / 120);
			g.DrawLine(pen, pt.x + size, pt.y, pt.x, pt.y - size * delta / 120);
		}
		else
			g.DrawEllipse(pen, pt.x - size, pt.y - size, 2*size, 2*size);
	}
	cc.Unlock();
	return true;
}

bool CCamera::Annotate(CDC* pDC)
{
	AddTimestamp(pDC);
	AddXNote(pDC);
	AddCaption(pDC);
	AddWatermark(pDC);
	if (m_cCursor.Record())
		AddCursor(pDC);
	if (m_cCursor.HighlightClick())
		AddClicks(pDC);
	return true;
}

void CCamera::InsertText(CDC* pDC, const CRect& rectBase, TextAttributes& rTextAttrs)
{
	CDC dcBits;
	dcBits.CreateCompatibleDC(pDC);

	// select font
	CFont font;
	if (rTextAttrs.isFontSelected) {
		font.CreateFontIndirect(&rTextAttrs.logfont);
	}
	CFont *pOldFont = (font.m_hObject)
		? dcBits.SelectObject(&font)
		: 0;

	/*
	*  Some info for multiple line blocks
	* -------------------------------------
	*  CString GetLength returns the length that you passed to it in the constructor and 
	*  not the length of the string. This can be confusing if you copy the string and loop 
	*  through the length. It can also be the cause of bugs is you get the length and use 
	*  CString.GetBuffer() and loop through the buffer for the length.
	*  It looks like CString.GetLength() is the size of the internal buffer and nothing more. 
	*/

	// Prepare multiple lines text support
	size_t nBlockLength= rTextAttrs.text.GetLength();
	size_t nMaxLength= nBlockLength;   // For now we assume that Max and BlockLength are equal (as is with singleline strings)
	size_t nNrOfLine = 1;
	size_t nNewLinePos = rTextAttrs.text.FindOneOf("\n");

	// Define thickness of the border
	int nBorderLineThickness = 3;

	// Multiple lines text requires that we determine the longest string to print
	if ( nNewLinePos ) {
		/*
		CString GetLength returns the length that you passed to it in the constructor and 
		not the length of the string. This can be confusing if you copy the string and loop 
		through the length. It can also be the cause of bugs is you get the length and use 
		CString.GetBuffer() and loop through the buffer for the length.
		It looks like CString.GetLength() is the size of the internal buffer and nothing more. 
		Therefore we must determine length again using GetString()
		*/
		CString cText = rTextAttrs.text.GetString();
		//nMaxLength = strlen(rTextAttrs.text.GetString());
		nMaxLength = strlen(cText);
		UINT n = 0, m = 0;
		for ( n=0, m=0 ; n < nMaxLength ; n++, m++ ) {
         wchar_t cTextn = cText.GetString()[n];
			if  ( cTextn == '\n' ) {
				nNrOfLine++;
				nBlockLength = max(nBlockLength, m);
				m = 0;
			}
		}
		// (Because \n will not always terminate a string we have to check one time more.)
		// Now we know the real required blocklength to be able to draw all text.
		nBlockLength = max(nBlockLength, m);
	}


	/* 
	 * uFormat; Specifies the method of formatting the text (see DrawText).
	 */
	UINT uFormat = DT_CENTER | DT_VCENTER;
	if ( nNrOfLine == 1 ){
		uFormat = uFormat | DT_SINGLELINE;
	}


	CSize sizeText = dcBits.GetTextExtent(rTextAttrs.text, nBlockLength);		// Use Blocklength because with multiple lines this will be less than textlength.
	
	// Required area's and offset pointers.
	CSize innerRectDimensions( sizeText.cx , nNrOfLine * sizeText.cy ); 		// Used for drawing text
	CSize outerRectDimensions( sizeText.cx + 2*nBorderLineThickness, nNrOfLine * sizeText.cy + 2*nBorderLineThickness); 	// Text and border
	CRect innerRectRelativePositons( nBorderLineThickness, nBorderLineThickness, sizeText.cx + nBorderLineThickness, nNrOfLine*sizeText.cy + nBorderLineThickness); 
	CSize outerRectOffset (0,0);
	CSize innerRectOffset (0,0);
	CRect outerRectAbsolutePositions(0, 0, 0, 0);
	CRect innerRectAbsolutePositions(0, 0, 0, 0);

	// Ratio's for floating offset of retangle instead of fixed positions.
	int xPosRatio = 0;
	int yPosRatio = 0;

	// Instead of 9 fixed position we can now use any location on screen to display an annotation
	if ( rTextAttrs.posType != UNDEFINED ) {
		xPosRatio = rTextAttrs.xPosRatio;
		yPosRatio = rTextAttrs.yPosRatio;
	} else {

		// But, If user annoatation setting are not yet updated we have to reconstruct xPos and yPos ratio
		// Other option is that we just delete this block of code and let user redefine his/her settings.

		// Define outer X offset ratio
		switch (rTextAttrs.position)
		{
		case TOP_LEFT:
		case CENTER_LEFT:
		case BOTTOM_LEFT:
		default:
			xPosRatio = 0 ;
			//uFormat = uFormat | DT_LEFT;
			break;
		case TOP_CENTER:
		case CENTER_CENTER:
		case BOTTOM_CENTER:
			xPosRatio = 50;
			//uFormat = uFormat | DT_CENTER;
			break;
		case TOP_RIGHT:
		case CENTER_RIGHT:
		case BOTTOM_RIGHT:
			xPosRatio = 100;
			//uFormat = uFormat | DT_RIGHT;
			break;
		}

		// Define outer Y offset ratio
		switch (rTextAttrs.position)
		{
		case TOP_LEFT:
		case TOP_CENTER:
		case TOP_RIGHT:
		default:
			yPosRatio = 0;
			break;
		case CENTER_LEFT:
		case CENTER_CENTER:
		case CENTER_RIGHT:
			yPosRatio = 50;
			break;
		case BOTTOM_LEFT:
		case BOTTOM_CENTER:
		case BOTTOM_RIGHT:
			yPosRatio = 100;
			break;
		}
	}


	// Define outerRect offset based on ratio defined by user
	outerRectOffset.cx = (rectBase.Width() - outerRectDimensions.cx ) * xPosRatio / 100 ;
	outerRectOffset.cy = (rectBase.Height() - outerRectDimensions.cy ) * yPosRatio / 100 ;

	// Define Inner offsets
	innerRectOffset.cx = outerRectOffset.cx + nBorderLineThickness;
	innerRectOffset.cy  = outerRectOffset.cy  + nBorderLineThickness;

	// Absolute OuterRect : top, left, bottom, right	
	outerRectAbsolutePositions.top    = outerRectOffset.cy; 
	outerRectAbsolutePositions.left   = outerRectOffset.cx;
	outerRectAbsolutePositions.bottom = outerRectOffset.cy + outerRectDimensions.cy;
	outerRectAbsolutePositions.right  = outerRectOffset.cx + outerRectDimensions.cx;
		
	// Absolute InnerRect : top, left, bottom, right	
	innerRectAbsolutePositions.top    = innerRectOffset.cy; 
	innerRectAbsolutePositions.left   = innerRectOffset.cx;
	innerRectAbsolutePositions.bottom = innerRectOffset.cy + innerRectDimensions.cy;
	innerRectAbsolutePositions.right  = innerRectOffset.cx + innerRectDimensions.cx;

		
	// Define the area including borderline ???
	CBitmap bm;
	bm.CreateCompatibleBitmap(pDC, outerRectDimensions.cx , outerRectDimensions.cy); 
	CBitmap * pOldBM = dcBits.SelectObject(&bm);

	CBrush brush;
	brush.CreateSolidBrush(rTextAttrs.backgroundColor);
	CBrush * pOldBrush = dcBits.SelectObject(&brush);
	//dcBits.Rectangle(&rectPos);
	dcBits.SelectObject(pOldBrush);

	// Write the message in the innerRect
	COLORREF old_bk_color = dcBits.GetBkColor();
	COLORREF old_txt_color = dcBits.GetTextColor();
	dcBits.SetBkColor(rTextAttrs.backgroundColor);
	dcBits.SetTextColor(rTextAttrs.textColor);
	dcBits.DrawTextEx( (LPTSTR)(LPCTSTR)rTextAttrs.text, nMaxLength, &innerRectRelativePositons, uFormat, 0);
	dcBits.SetTextColor(old_txt_color);
	dcBits.SetBkColor(old_bk_color);

	// Bit-block transfer of the color data (Copies the source rectangle directly to the destination rectangle)
	pDC->BitBlt( outerRectOffset.cx , outerRectOffset.cy, outerRectDimensions.cx , outerRectDimensions.cy, &dcBits, 0, 0, SRCCOPY);
	dcBits.SelectObject(pOldBM);
	if (pOldFont){
		dcBits.SelectObject(pOldFont);
	}

/*	
	// Copy of code that could be used as example to rotate an image.
	// BTW Before we can use it within Camstudio we do a few changes her first

	// BitBlt the starting Bitmap into a memory HDC
	hdcNew = CreateCompatibleDC(hdc);
	hBmp = CreateCompatibleBitmap(hdc, 200,200);
	SelectObject(hdcNew, hBmp);
	BitBlt(hdcNew, 0, 0, 200, 200, hdc, (rt.right - rt.left - 200) / 2, 0, SRCCOPY);

	// Rotate that memory HDC
	RotateMemoryDC(hBmp, hdcNew, 200, 200, g_angle, hdcMem, dstX, dstY);
	DeleteObject(hBmp);
	DeleteDC(hdcNew);

	// Create the output HDC
	hdcNew = CreateCompatibleDC(hdc);
	hBmp = CreateCompatibleBitmap(hdc, 400,400);
	SelectObject(hdcNew, hBmp);
	rtmp.left = rtmp.top = 0;
	rtmp.right = rtmp.bottom = 400;

	// Fill the output HDC with the window background color and BitBlt the rotated bitmap into it
	FillRect(hdcNew, &rtmp, GetSysColorBrush(COLOR_WINDOW));
	BitBlt(hdcNew, (400 - dstX) / 2, (400-dstY) / 2, dstX, dstY, hdcMem, 0, 0, SRCCOPY);
	DeleteDC(hdcMem);
	BitBlt(hdc, (rt.left + rt.right - 400) / 2, rt.bottom - 400, 400, 400, hdcNew, 0, 0, SRCCOPY);
	DeleteObject(hBmp);
	DeleteDC(hdcNew);
	EndPaint(hWnd, &ps);
*/



}


void CCamera::InsertHighLight(CDC *pDC, CPoint pt)
{
	if (!(m_cCursor.Record() && m_cCursor.Highlight()))
		return;

	Graphics g(pDC->GetSafeHdc());
	Color c(m_cCursor.HighlightColor());
	SolidBrush brush(c);

	float highlightsize = m_cCursor.HighlightSize() / 2.f;

	int highlightshape = m_cCursor.HighlightShape();

	if ((highlightshape == 0) || (highlightshape == 1)) {
		g.FillEllipse(&brush, pt.x - highlightsize, pt.y - highlightsize, 2.f*highlightsize, 2.f*highlightsize);
	} else if ((highlightshape == 2) || (highlightshape == 3)) {
		g.FillRectangle(&brush, pt.x - highlightsize, pt.y - highlightsize, 2.f*highlightsize, 2.f*highlightsize);
	}
}

void CCamera::InsertImage(CDC *pDC, CRect& rectFrame, const ::ImageAttributes& rImgAttr)
{
	CRect rect;
	CSize size(m_imageWatermark.GetWidth(), m_imageWatermark.GetHeight());
	CSize full_size = size;
	CRect mRect(0, 0, full_size.cx, full_size.cy);
	switch (rImgAttr.position)
	{
	case TOP_LEFT:
		rect.left = 0;
		rect.top = 0;
		break;
	case TOP_CENTER:
		rect.left = (rectFrame.Width() - full_size.cx) / 2;
		rect.top = 0;
		break;
	case TOP_RIGHT:
		rect.left = rectFrame.right - full_size.cx;
		rect.top = 0;
		break;
	case CENTER_LEFT:
		rect.left = 0;
		rect.top = (rectFrame.Height() - full_size.cy) / 2;
		break;
	case CENTER_CENTER:
		rect.left = (rectFrame.Width() - full_size.cx) / 2;
		rect.top = (rectFrame.Height() - full_size.cy) / 2;
		break;
	case CENTER_RIGHT:
		rect.left = rectFrame.right - full_size.cx;
		rect.top = (rectFrame.Height() - full_size.cy) / 2;
		break;
	case BOTTOM_LEFT:
		rect.left = 0;
		rect.top = rectFrame.bottom - full_size.cy;
		break;
	case BOTTOM_CENTER:
		rect.left = (rectFrame.Width() - full_size.cx) / 2;
		rect.top = rectFrame.bottom - full_size.cy;
		break;
	case BOTTOM_RIGHT:
		rect.left = rectFrame.right - full_size.cx;
		rect.top = rectFrame.bottom - full_size.cy;
		break;
	default:
		rect.left = 0;
		rect.top = 0;
		break;
	}
	rect.right = rect.left + full_size.cx;
	rect.bottom = rect.top + full_size.cy;

	m_imageWatermark.Draw(*pDC, rect);
}

bool CCamera::CaptureFrame(const CRect& rectView)
{
	_zoomFrame = rectView; // we need it for cursor correction :(
	//m_rectView = rectView; // this is already done in SetView()
	// Severe change: rectView now is to be captured and scaled to m_rectView
	// this would easily allow to zoom in

	/////////////////////////////////////////////
	// 
	// == REMEMBER ======================================
	// Topleft position on screen is 0:0 not 1:1
	// Hence, Maxscreen size is logical size (e.g. 1050 x 1680). Last pos is than 1650 minus 1
	// Left value = right + width() - 1 = 0 + 1680 - 1 = 1679  (because first column = 0 )
	// Right value = bottom - top - 1 = 1050 - 0 -1 = 1049 (because first row = 0 )
	// Logical Width() = right - left + 1 = 1679 - 0 + 1 = 1680
	// Logical Height = bottomt - top + 1 = 1049 - 0 + 1 = 1050
	// But Crect Width() = right - left = 1679 - 0 = 1679
	// But Crect Height() = bottom - top = 1049 - 0 = 1049
	// ==================================================
	// Conclusion (for our Camstudio application) Weight and Height are one to low here..!
	// But where is this caused ???
	// ==================================================
	// Problem found...!
	// This function expect a param from class CRect.
	// But in some occassions this function is called with a rcUse structure (also a CRect) 
	//   but rcUse is build with Top,Left,Bottom,Right info only.
	// It appears that CRect calculate  Width() as Right - Left  without adding the required one additional pixel by default....!
	// Soo, on different locations one could find some add and subtracts to get pixel size correct.
	// 
	/////////////////////////////////////////////
	//TRACE( _T("## *** CCamera::CaptureFrame  m_rectView.  TLBR=/%d/%d/%d/%d/  WH=/%d/%d/\n"), m_rectView.top, m_rectView.left, m_rectView.bottom, m_rectView.right,  m_rectView.Width(), m_rectView.Height() );

	// And now we know why we have to add one additional picture here...!
	// Because in this function not the Weight/Hight are expected but Bottom anf Left value....!
	m_rectFrame = CRect(CPoint(0, 0), m_rectView.Size());

	//TRACE( _T("## *** CCamera::CaptureFrame  m_rectFrame.  TLBR=/%d/%d/%d/%d/  WH=/%d/%d/\n"), m_rectFrame.top, m_rectFrame.left, m_rectFrame.bottom, m_rectFrame.right,  m_rectFrame.Width(), m_rectFrame.Height() );

	// setup DC's
	// Applied bug fix, tracker  ID: 3075791 / memory leak, reported and solved by mlt_msk, hScreenDC is released afterwards.
	HDC hScreenDC = ::GetDC(0);
	CDC* pScreenDC = CDC::FromHandle(hScreenDC);

	CDC cMemDC;
	cMemDC.CreateCompatibleDC(pScreenDC);
	CBitmap cBitmap;
	cBitmap.CreateCompatibleBitmap(pScreenDC, m_rectView.Width(), m_rectView.Height());
	CBitmap* pOldBitmap = cMemDC.SelectObject(&cBitmap);

	///////////////////////////////////////////
	// Jun 2010, Todo, option.
	// If we put recording on pause we can put screen copy on a stack.
	// Doing so we can recall saved frames when pause gets released
	///////////////////////////////////////////

	// copy screen image to bitmap
	DWORD dwRop = SRCCOPY;
	// TODO: assume transparency and OS version
	dwRop |= CAPTUREBLT;

	// this is to estimate penalty for extra image creation & GDI+
	// some caching may be usefull since don't change zoom level often
	// thus makes sense to store `orig` & `g` as class members
	// SEVERE FPS drop noticed 40 fps -> 17 fps
	if (m_rectView.Width() == _zoomFrame.Width()) {
		cMemDC.BitBlt(0, 0, m_rectView.Width() + 1, m_rectView.Height() + 1, pScreenDC, m_rectView.left, m_rectView.top, dwRop);
	} else {
		Bitmap orig(rectView.Width(), rectView.Height());
		Graphics g(&orig);
		HDC origDC = g.GetHDC();

		::BitBlt(origDC, 0, 0, rectView.Width(), rectView.Height(), hScreenDC, rectView.left, rectView.top, dwRop);

		g.ReleaseHDC(origDC);

		Graphics g2( cMemDC.GetSafeHdc());

		Status s = g2.DrawImage(&orig,0,0,m_rectView.Width(),m_rectView.Height());
	}
	Annotate(&cMemDC);

	// restore old bitmap
	cMemDC.SelectObject(pOldBitmap);

	// convert cBitmap to Image
	m_cImage.CreateFromHBITMAP(cBitmap);
	// TEST/TODO: shrink the output!
	//m_cImage.QIShrink( (rectView.Width() +1 )* 3/8, (rectView.Height() + 1) * 3/8, 0, false);
	// TEST/TODO: convert to GrayScale!
	//m_cImage.GrayScale();

	++m_uFrameCount;
	ReleaseDC(0,hScreenDC);

	return true;
}
