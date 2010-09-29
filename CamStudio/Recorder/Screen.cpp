#include "StdAfx.h"
//#include "EffectsOptions.h"		Cause C1189
#include "Screen.h"
#include "ximage.h"
#include "XnoteStopwatchFormat.h"
#include "RecorderView.h"

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
	if (!m_cCursor.Record()) {
		return true;
	}

	CPoint ptCursor;
	VERIFY(::GetCursorPos(&ptCursor));
	ptCursor.x -= m_rectView.left;
	ptCursor.y -= m_rectView.top;

	// TODO: This shift left and up is kind of bogus.
	// The values are half the width and height of the higlight area.
	InsertHighLight(pDC, CPoint(ptCursor.x - 64, ptCursor.y - 64));

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
		if (iconinfo.hbmMask) {
			::DeleteObject(iconinfo.hbmMask);
		}
		if (iconinfo.hbmColor) {
			::DeleteObject(iconinfo.hbmColor);
		}
	}

	pDC->DrawIcon(ptCursor.x, ptCursor.y, hcur);
	return true;
}

bool CCamera::Annotate(CDC* pDC)
{
	AddTimestamp(pDC);
	AddXNote(pDC);
	AddCaption(pDC);
	AddWatermark(pDC);
	AddCursor(pDC);
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
			if  ( cText[n] == '\n' ) {
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

	CSize fullsize(128, 128);
	int highlightsize = m_cCursor.HighlightSize();
	COLORREF highlightcolor = m_cCursor.HighlightColor();
	if (m_cCursor.HighlightClick()) {
		// update color
		SHORT iKeyState = ::GetKeyState(VK_LBUTTON);
		bool bDown = (iKeyState & 0xF000) ? true : false;
		bool bToggle = (iKeyState & 0x0001);
		//if (iKeyState != 0 && iKeyState != 1) {
		if (bDown && !bToggle) {
			highlightcolor = m_cCursor.ClickLeftColor();
			// click highlights are 1.5 larger
			highlightsize = (3 * highlightsize)/2;
		}
		iKeyState = ::GetKeyState(VK_RBUTTON);
		bDown = (iKeyState & 0xF000) ? true : false;
		bToggle = (iKeyState & 0x0001);
		//if (iKeyState != 0 && iKeyState != 1) {
		if (bDown && !bToggle) {
			highlightcolor = m_cCursor.ClickRightColor();
			// click highlights are 1.5 larger
			highlightsize = (3 * highlightsize)/2;
		}
	}

	int highlightshape = m_cCursor.HighlightShape();
	//OffScreen Buffer
	CDC dcBits;
	dcBits.CreateCompatibleDC(pDC);
	CBitmap cBitmap;
	cBitmap.CreateCompatibleBitmap(pDC, fullsize.cx, fullsize.cy);
	CBitmap *pOldBitmap = dcBits.SelectObject(&cBitmap);

	// assume circle and square
	double x1 = ::floor((fullsize.cx - highlightsize)/2.0);
	double x2 = ::floor((fullsize.cx + highlightsize)/2.0);
	double y1 = ::floor((fullsize.cy - highlightsize)/2.0);
	double y2 = ::floor((fullsize.cy + highlightsize)/2.0);

	if ((highlightshape == 1) || (highlightshape == 3)) {
		//ellipse and rectangle
		x1 = ::floor((fullsize.cx - highlightsize)/2.0);
		x2 = ::floor((fullsize.cx + highlightsize)/2.0);
		y1 = ::floor((fullsize.cy - highlightsize/2.0)/2.0);
		y2 = ::floor((fullsize.cy + highlightsize/2.0)/2.0);
	}

	CBrush brush;
	brush.CreateSolidBrush(RGB(255, 255, 255));
	CBrush brushHL;
	brushHL.CreateSolidBrush(highlightcolor);
	CPen penNull;
	penNull.CreatePen(PS_NULL, 0, COLORREF(0));

	CBrush * pOldBrush = dcBits.SelectObject(&brush);
	CPen * pOldPen = dcBits.SelectObject(&penNull);
	dcBits.Rectangle(0, 0, fullsize.cx + 1, fullsize.cy + 1);

	//Draw the highlight
	dcBits.SelectObject(&brushHL);

	if ((highlightshape == 0) || (highlightshape == 1)) {
		dcBits.Ellipse((int)x1, (int)y1, (int)x2, (int)y2);
	} else if ((highlightshape == 2) || (highlightshape == 3)) {
		dcBits.Rectangle((int)x1, (int)y1, (int)x2, (int)y2);
	}

	dcBits.SelectObject(pOldBrush);
	dcBits.SelectObject(pOldPen);

	// OffScreen Buffer
	pDC->BitBlt(pt.x, pt.y, fullsize.cx, fullsize.cy, &dcBits, 0, 0, SRCAND);
	dcBits.SelectObject(pOldBitmap);
}

void CCamera::InsertImage(CDC *pDC, CRect& rectFrame, const ImageAttributes& rImgAttr)
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
	m_rectView = rectView;

	// HIER_BEN_IK

	// UNDER CONSTRUCTION...
	// == REMEMBER ======================================
	// Topleft position on screen is 0:0 not 1:1
	// Hence, Maxscreen size is logical size (e.g. 1650). Last pos is than 1650 minus 1
	// Width = right - left + 1 
	// Height = bottom - top - 1
	// ==================================================
	// Conclusion (for our Camstudio application) Weight and Height are one to low here..!
	// But where is this caused ???
	/////////////////////////////////////////////
	TRACE( _T("## CCamera::CaptureFrame  m_rectView.Width()=%d\n"), m_rectView.Width() );

	m_rectFrame = CRect(0, 0, m_rectView.Width(), m_rectView.Height());
	// setup DC's
	// CDC* pScreenDC = CDC::FromHandle(::GetDC(0));
	
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
	cMemDC.BitBlt(0, 0, m_rectView.Width(), m_rectView.Height(), pScreenDC, m_rectView.left, m_rectView.top, dwRop);

	Annotate(&cMemDC);

	// restore old bitmap
	cMemDC.SelectObject(pOldBitmap);

	// convert cBitmap to Image
	m_cImage.CreateFromHBITMAP(cBitmap);
	// TEST/TODO: shrink the output!
	//m_cImage.QIShrink((rectView.Width() * 3)/8, (rectView.Height() * 3)/8, 0, false);
	// TEST/TODO: convert to GrayScale!
	//m_cImage.GrayScale();

	++m_uFrameCount;
	ReleaseDC(0,hScreenDC);

	return true;
}
