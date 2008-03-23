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

	void CreateFlashing(LPCTSTR pTitle, RECT &rect);
	void SetUpRegion(int x, int y, int width, int height, int type);
	void SetUpRect(int x, int y, int width, int height);
	void PaintBorder(COLORREF colorval);
	void PaintInvertedBorder(COLORREF colorval);	
	void MoveRegion(int diffx, int diffy);
	void CheckRect(int diffx, int diffy);
	void UpdateRegionMove();
	void MakeFixedRegion(CRgn  &wndRgn, CRgn  &rgnTemp, CRgn  &rgnTemp2,CRgn  &rgnTemp3);


public:
	CRect cRect;
	HRGN oldregion;



// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlashingWnd)
	afx_msg BOOL OnEraseBkgnd( CDC* pDC );
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFlashingWnd();

protected:
	

	// Generated message map functions
protected:
	//{{AFX_MSG(CFlashingWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
