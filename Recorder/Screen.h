#pragma once
#include "camcursor.h"
#include "profile.h"
#include "ximage.h"

////////////////////////////////////////////////////////
// What is used for what...
//
// rectView  : This is the screen view to capture, BTW TopLeft=1,1
//
// rectFrame :
//
////////////////////////////////////////////////////////
class CCamera
{
public:
	CCamera();
	CCamera( const CCamCursor& rCursor, 
				const sCaptionOpts& rCaption, 
				const sTimestampOpts& rTimestamp, 
				const sXNoteOpts& rXNote,  
				const sWatermarkOpts& rWatermark)
		: m_cCursor(rCursor)
		, m_sCaption(rCaption)
		, m_sTimestamp(rTimestamp)
		, m_sXNote(rXNote)
		, m_sWatermark(rWatermark)
		, m_uFrameCount(0)
	{
	}
	virtual ~CCamera();

	size_t FrameCount() const					{return m_uFrameCount;}
	HCURSOR Save(HCURSOR hCursor)				{return m_cCursor.Save(hCursor);}
	void Set(const CCamCursor& rCursor)			{m_cCursor = rCursor;}
	void Set(const sCaptionOpts& rCaption)		{m_sCaption = rCaption;}
	void Set(const sTimestampOpts& rTimestamp)	{m_sTimestamp = rTimestamp;}
	void Set(const sXNoteOpts& rXNote)	        {m_sXNote = rXNote;}   
	
	void Set(const sWatermarkOpts& rWatermark)	
	{
		// if name changed, or brightness or contrast
		// must reload image
		bool bReload = (m_strWatermarkName != rWatermark.m_iaWatermark.text)
			|| (m_sWatermark.m_iaWatermark.m_lBrightness != rWatermark.m_iaWatermark.m_lBrightness)
			|| (m_sWatermark.m_iaWatermark.m_lContrast != rWatermark.m_iaWatermark.m_lContrast);
		m_sWatermark = rWatermark;
		if (bReload) {
			LoadWatermark();
		}
	}
	
	void SetView(const CRect& rView)
	{
		m_rectView = rView;
		m_rectFrame = CRect(0, 0, m_rectView.Width(), m_rectView.Height());
	}

	bool CaptureFrame(const CRect& rectView);
	LPBITMAPINFOHEADER Image() const
	{
		return static_cast<LPBITMAPINFOHEADER>(m_cImage.GetDIB());
	}
	HBITMAP ImageBmp();

protected:
	void InsertCaption(CDC *pDC)				{VERIFY(AddCaption(pDC));}
	void InsertTimeStamp(CDC *pDC)				{VERIFY(AddTimestamp(pDC));}
	void InsertXNote(CDC *pDC)				    {VERIFY(AddXNote(pDC));}
	void InsertWatermark(CDC *pDC)				{VERIFY(AddWatermark(pDC));}
	void InsertCursor(CDC *pDC)					{VERIFY(AddCursor(pDC));}
	void InsertText(CDC* pDC, const CRect& rRect, TextAttributes& rTextAttrs);
	void InsertImage(CDC *pDC, CRect& rectFrame, const ImageAttributes& rImgAttr);
private:
	HBITMAP m_bBitMap;
	size_t m_uFrameCount;			// count of images captured
	CxImage m_cImage;				// result of CaptureFrame
	CxImage m_imageWatermark;		// Watermark Image
	CString m_strWatermarkName;		// Watermark filename
	CRect m_rectView;				// screen view to capture, BTW TopLeft=1,1
	// zoom should be within rectView boundaries
	// camera can pan and follow pointer
	// zoom level. kind of m_rectFrame.Size() == zoom * <stuff on screen>.Size()
//	double _zoom;
	CRect _zoomFrame; // m prefix is only for public stuff !!!
	CRect m_rectFrame;				// logical frame to capture BTW TopLeft=0,0
	// annotation objects
	CCamCursor m_cCursor;
	sCaptionOpts m_sCaption;
	sTimestampOpts m_sTimestamp;
	sXNoteOpts m_sXNote;
	sWatermarkOpts m_sWatermark;

	void InsertHighLight(CDC *pDC, CPoint pt);
	bool LoadWatermark();
	bool AddTimestamp(CDC* pDC);
	bool AddXNote(CDC* pDC);
	bool AddCaption(CDC* pDC);
	bool AddWatermark(CDC* pDC);
	bool AddCursor(CDC* pDC);
	bool AddClicks(CDC* pDC);
	bool Annotate(CDC* pDC);
};

