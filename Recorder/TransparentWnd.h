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

#if !defined(AFX_TRANSPARENTWND_H__INCLUDED_)
#define AFX_TRANSPARENTWND_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#pragma message("TransparentWnd included")

#include "LayeredWindowHelperST.h"
#include "Picture.h"

#define regionNULL 0
#define regionTRANSPARENTCOLOR 1
#define regionSHAPE 2

#define regionROUNDRECT 0
#define regionELLIPSE 1
#define regionRECTANGLE 2

#define saveMethodNew 0
#define saveMethodReplace 1

class CTransparentWnd : public CWnd
{
public:
	CTransparentWnd();
	virtual ~CTransparentWnd();

	//void CreateTransparent(LPCTSTR pTitle, RECT &rect, unsigned short MaskID, unsigned short BitmapID);
	void CreateTransparent(LPCTSTR pTitle, RECT rect,  HBITMAP BitmapID);
	void CreateTransparent(LPCTSTR pTitle, RECT rect,  CString bitmapFile, int fitBitmapSize);

	CTransparentWnd* Clone(int offsetx, int offsety);
	CTransparentWnd* CloneByPos(int x, int y);

	void SetupRegionByTransColor(CDC *pDC, COLORREF transColor);
	void SetupRegion(CDC *pDC);
	void SetupRegion();
	LPBITMAPINFO GetDCBitmap(CDC *thisDC,CRect* caprect);
	LPBITMAPINFO GetTextBitmap(CDC *thisDC, CRect* caprect,int factor,CRect* drawtextRect, LOGFONT* drawfont, CString textstr, CPen* pPen, CBrush * pBrush, COLORREF textcolor, int horzalign);
	HBITMAP DrawResampleRGB(CDC *thisDC, CRect* caprect,int factor, LPBITMAPINFOHEADER expanded_bmi);
	void InvalidateTransparency();
	void OnUpdateContextMenu();
	void ReloadPic(CString filename);
	void DisableContextMenu();

	void EditTransparency();
	void EditText();
	void EditImage();
	BOOL SaveShape(FILE* fptr);
	BOOL LoadShape(FILE* fptr);

	//WidthHeight
	void RefreshWindowSize();
	void EnsureOnTopList(CTransparentWnd* transWnd);
protected:
	void CopyMembers(const CTransparentWnd& rhsWnd);

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransparentWnd)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	void InvalidateRegion();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTransparentWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnContextCloseall();
	afx_msg void OnContextClose();
	afx_msg void OnContextSaveLayout();
	afx_msg void OnContextResize();
	afx_msg void OnContextRefresh();
	afx_msg void OnContextEditText();
	afx_msg void OnContextSave();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnContextEditTransparency();
	afx_msg void OnContextNoAntiAlias();
	afx_msg void OnContextAntiAlias2();
	afx_msg void OnContextAntiAlias3();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnContextEditImage();
	afx_msg void OnContextClone();
	//}}AFX_MSG
	afx_msg LRESULT OnInvalidateWnd(WPARAM p1, LPARAM p2);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	//Data to be saved
public:
	int editImageOn;
	int editTransOn;
	int trackingOn;
	int m_borderYes;
	int m_borderSize;
	int m_regionPredefinedShape;	// region for pre-defined shape, var applicable only to regiontype -- predefined shape
	int m_regionCreated;			// region for transparent color already created, var applicable only to regiontype  -- transparent color
	int baseType;
	int m_regionType;
	int saveMethod;
	int widthPos;					// Width-Height (Formula): faction of original =  widthPos*0.025 + 0.2 ... form 0.2 to 5.2
	int heightPos;
	long uniqueID;
	CString m_shapeStr;				// name of the shape
	CString m_textstring;
	CRect m_rectWnd;
	COLORREF m_borderColor;
	COLORREF m_transparentColor;	// region for pre-defined shape, var applicable only to regiontype  -- transparent color
	COLORREF m_backgroundColor;
	HBITMAP m_hbitmap;
protected:
	unsigned short m_BitmapID;
	int menuLoaded;
	int m_horzalign;
	double m_roundrectFactor;
	CRectTracker m_tracker;
	CMenu menu;
	LOGFONT m_textfont;
	COLORREF rgb;					// textcolor
private:
	int m_vertalign;
	int m_factor;
	int m_charset;
	int enableTransparency;
	int valueTransparency;
	CPicture picture;
	CRect m_rectOriginalWnd;

	//Temporary state variables
	CLayeredWindowHelperST G_Layered;
	int m_movewindow;
	POINT m_movepoint;
	CRgn wndRgn;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

