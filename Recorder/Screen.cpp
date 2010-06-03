#include "StdAfx.h"
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

bool CCamera::AddXNote(CDC* pDC)
{
	if (m_sXNote.m_bAnnotation) {

		CString str;
		TextAttributes taTmpXNote;		

		DWORD dwCurrTickCount =  GetTickCount();
		char cTmpBuffXNoteTimeStamp[64] = {0};

		CXnoteStopwatchFormat::FormatXnoteDelayedTimeString( cTmpBuffXNoteTimeStamp, cXNoteOpts.m_ulStartXnoteTickCounter, dwCurrTickCount ,m_sXNote.m_ulXnoteCameraDelayInMilliSec, cXNoteOpts.m_bXnoteDisplayCameraDelay );

		taTmpXNote = m_sXNote.m_taXNote;
		// Show on second line recent (xnote) snaptimes
		(void) sprintf( taTmpXNote.text.GetBuffer(64), "%s\n%s", cTmpBuffXNoteTimeStamp, cXNoteOpts.m_cSnapXnoteTimesString); 

		InsertText(pDC, m_rectFrame, taTmpXNote);

		// Determine if we want to switch from recording to pause mode.
		// automatically now just 3 seconds after last snap occurred.
		if ( dwCurrTickCount > (cXNoteOpts.m_ulSnapXnoteTickCounter + 3000) ) {
			CRecorderView::XNoteSetRecordingInPauseMode();
		}

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
	CString GetLength returns the length that you passed to it in the constructor and 
	not the length of the string. This can be confusing if you copy the string and loop 
	through the length. It can also be the cause of bugs is you get the length and use 
	CString.GetBuffer() and loop through the buffer for the length.
	It looks like CString.GetLength() is the size of the internal buffer and nothing more. 
	*/
	size_t length = rTextAttrs.text.GetLength();
	
	//jahoma, Second text line support
	size_t nMaxLength= length;
	size_t nBlockLength= length;
	size_t nNrOfLine = 1;
	size_t nNewLinePos = rTextAttrs.text.FindOneOf("\n");
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
		UINT n = 0;
		for ( n=0 ; n < nMaxLength ; n++ ) {
			if  ( cText[n] == '\n' ) {
				nNrOfLine++;
			}
		}
	}

	//UINT uFormat = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
	/* 
	 * uFormat; Specifies the method of formatting the text (see DrawText).
	 */
	UINT uFormat = DT_CENTER | DT_VCENTER;
	if ( nNrOfLine == 1 ){
		uFormat = uFormat | DT_SINGLELINE;
	}


	CSize sizeText = dcBits.GetTextExtent(rTextAttrs.text, length);			// length is blocklength
	CSize sizeFull(sizeText.cx + 10, nNrOfLine*sizeText.cy + 10);			// defines the height of the area
	CRect rectFull(0, 0, sizeFull.cx, sizeFull.cy);	// Define  size of the printable area, not the exact location  + 50=testje
	CRect rectPos(0, 0, 0, 0);
	switch (rTextAttrs.position)
	{
	case TOP_LEFT:
	default:
		rectPos.left = 0;
		rectPos.top = 0;
		//uFormat = uFormat | DT_LEFT;
		break;
	case TOP_CENTER:
		rectPos.left = (rectBase.Width() - sizeFull.cx) / 2;
		rectPos.top = 0;
		//uFormat = uFormat | DT_CENTER;
		break;
	case TOP_RIGHT:
		rectPos.left = rectBase.right - sizeFull.cx;
		rectPos.top = 0;
		//uFormat = uFormat | DT_RIGHT;
		break;
	case CENTER_LEFT:
		rectPos.left = 0;
		rectPos.top = (rectBase.Height() - sizeFull.cy) / 2;
		//uFormat = uFormat | DT_LEFT;
		break;
	case CENTER_CENTER:
		rectPos.left = (rectBase.Width() - sizeFull.cx) / 2;
		rectPos.top = (rectBase.Height() - sizeFull.cy) / 2;
		//uFormat = uFormat | DT_CENTER;
		break;
	case CENTER_RIGHT:
		rectPos.left = rectBase.right - sizeFull.cx;
		rectPos.top = (rectBase.Height() - sizeFull.cy) / 2;
		//uFormat = uFormat | DT_RIGHT;
		break;
	case BOTTOM_LEFT:
		rectPos.left = 0;
		rectPos.top = rectBase.bottom - sizeFull.cy;
		//uFormat = uFormat | DT_LEFT;
		break;
	case BOTTOM_CENTER:
		rectPos.left = (rectBase.Width() - sizeFull.cx) / 2;
		rectPos.top = rectBase.bottom - sizeFull.cy;
		//uFormat = uFormat | DT_CENTER;
		break;
	case BOTTOM_RIGHT:
		rectPos.left = rectBase.right - sizeFull.cx;
		rectPos.top = rectBase.bottom - sizeFull.cy;
		//uFormat = uFormat | DT_RIGHT;
		break;
	}
	rectPos.right = rectPos.left + sizeFull.cx;
	rectPos.bottom = rectPos.top + sizeFull.cy;

	/*
	TRACE("##  Camera::InsertText lines=[%i]    l-bl-ml=[%d,%d,%d]    tlbr:[%i,%i,%i,%i]   tekst[%s]\n", 
		nNrOfLine, 
		length, nBlockLength, nMaxLength,
		rectPos.top, rectPos.left, rectPos.bottom, rectPos.right,
		rTextAttrs.text 
		);
	*/

	CBitmap bm;
	bm.CreateCompatibleBitmap(pDC, sizeFull.cx, sizeFull.cy);
	CBitmap * pOldBM = dcBits.SelectObject(&bm);

	CBrush brush;
	brush.CreateSolidBrush(rTextAttrs.backgroundColor);
	CBrush * pOldBrush = dcBits.SelectObject(&brush);
	//dcBits.Rectangle(&rectPos);
	dcBits.SelectObject(pOldBrush);

	
	COLORREF old_bk_color = dcBits.GetBkColor();
	COLORREF old_txt_color = dcBits.GetTextColor();
	dcBits.SetBkColor(rTextAttrs.backgroundColor);
	dcBits.SetTextColor(rTextAttrs.textColor);
//	dcBits.DrawTextEx( (LPTSTR)(LPCTSTR)rTextAttrs.text, length, &rectFull, uFormat, 0);
	dcBits.DrawTextEx( (LPTSTR)(LPCTSTR)rTextAttrs.text, nMaxLength, &rectFull, uFormat, 0);
	dcBits.SetTextColor(old_txt_color);
	dcBits.SetBkColor(old_bk_color);

	pDC->BitBlt(rectPos.left, rectPos.top, sizeFull.cx, sizeFull.cy, &dcBits, 0, 0, SRCCOPY);
	dcBits.SelectObject(pOldBM);
	if (pOldFont){
		dcBits.SelectObject(pOldFont);
	}
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
	m_rectFrame = CRect(0, 0, m_rectView.Width(), m_rectView.Height());
	// setup DC's
	CDC* pScreenDC = CDC::FromHandle(::GetDC(0));
	CDC cMemDC;
	cMemDC.CreateCompatibleDC(pScreenDC);
	CBitmap cBitmap;
	cBitmap.CreateCompatibleBitmap(pScreenDC, m_rectView.Width(), m_rectView.Height());
	CBitmap* pOldBitmap = cMemDC.SelectObject(&cBitmap);

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

	return true;
}
