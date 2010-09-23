//********************************************************************************
//* FlashingWindow.CPP
//********************************************************************************

#if !defined(AFX_FLASHINGWND_H__INCLUDED_)
#define AFX_FLASHINGWND_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

class CFlashingWnd : public CWnd
{
public:
	CFlashingWnd();
	virtual ~CFlashingWnd();

	// TODO: magic number 200 here, why?  Explain
	static const UINT WM_FLASH_WINDOW = (WM_APP + 200);	// wparm = invert; lparam = draw

	BOOL CreateFlashing(LPCTSTR pTitle, RECT &rect);
	void DrawFlashingRect(bool bInvert, bool bDraw = true)
	{
		PaintBorder(bInvert, bDraw);
	}

	void SetUpRegion(const CRect& cRect, int type);
	
	CRect Rect() const				{return m_cRect;}
	bool NewRegionUsed() const		{return m_bNewRegionUsed;}
	bool NewRegionUsed(bool bUsed)	{return m_bNewRegionUsed = bUsed;}
private:
	void PaintBorder(bool bInvert, bool bDraw = true);
	//void SetUpRect(int x, int y, int width, int height);
	//void SetUpRect(int x, int y, int width, int height)
	//{
	//	m_cRect = CRect(x, y, x + width, y + height);
	//}
	void CheckRect(int diffx, int diffy);
	void MoveRegion(int diffx, int diffy);
	void UpdateRegionMove();
	void MakeFixedRegion(CRgn &wndRgn, CRgn &rgnTemp, CRgn &rgnTemp2, CRgn &rgnTemp3);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlashingWnd)
	//}}AFX_VIRTUAL

// Implementation
	// Generated message map functions
protected:
	//{{AFX_MSG(CFlashingWnd)
	afx_msg BOOL OnEraseBkgnd( CDC* pDC );
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnFlashWindow(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	COLORREF	m_clrBorderON;		// border ON color
	COLORREF	m_clrBorderOFF;		// border OFF color
	CRect		m_cRect;			// border region rectangle
	HCURSOR		m_hCursorMove;
	CPoint		m_ptStart;
	bool		m_bStartDrag;
	bool		m_bNewRegionUsed;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
