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


#if _MSC_VER > 1000
#pragma once
#endif


class CTransparentWnd : public CWnd
{

public:
	CTransparentWnd();

	//void CreateTransparent(LPCTSTR pTitle, RECT &rect, unsigned short MaskID, unsigned short BitmapID);
	void CreateTransparent(LPCTSTR pTitle, RECT rect,  HBITMAP BitmapID);
	void CreateTransparent(LPCTSTR pTitle, RECT rect,  CString bitmapFile, int fitBitmapSize);
	void SetupRegionByTransColor(CDC *pDC, COLORREF transColor);
	void SetupRegion(CDC *pDC);
	void SetupRegion();
	LPBITMAPINFO GetDCBitmap(CDC *thisDC,CRect* caprect); 
	LPBITMAPINFO GetTextBitmap(CDC *thisDC, CRect* caprect,int factor,CRect* drawtextRect, LOGFONT* drawfont, CString textstr, CPen* pPen, CBrush * pBrush, COLORREF textcolor, int horzalign); 
	HBITMAP DrawResampleRGB(CDC *thisDC, CRect* caprect,int factor, LPBITMAPINFOHEADER expanded_bmi); 
	void InvalidateTransparency();
	void OnUpdateContextMenu();	
	void ReloadPic(CString filename);
	CTransparentWnd* Clone(int offsetx, int offsety);
	CTransparentWnd* CloneByPos(int x, int y);
	void CopyMembers(CTransparentWnd *newWnd);
	void DisableContextMenu();

	void EditTransparency();
	void EditText();
	void EditImage();	
	BOOL SaveShape(FILE* fptr);
	BOOL LoadShape(FILE* fptr);

	//WidthHeight
	void RefreshWindowSize();
	void EnsureOnTopList(CTransparentWnd* transWnd ); 
	 		
	//Data to be saved	
	CRectTracker m_tracker;
	CString m_textstring;
	CString m_shapeStr;  //name of the shape
	int m_vertalign; 
	int m_horzalign;	
	LOGFONT m_textfont;
	COLORREF rgb; //textcolor	
	int m_factor;
	int m_charset;
	CRect m_rectWnd;
	int enableTransparency;
	int valueTransparency;		
	COLORREF m_transparentColor; //region for pre-defined shape, var applicable only to regiontype  -- transparent color
	int m_regionCreated; //region for transparent color already created, var applicable only to regiontype  -- transparent color
	int m_regionType; 
	int m_regionPredefinedShape;  //region for pre-defined shape, var applicable only to regiontype -- predefined shape
	double m_roundrectFactor; 
	int m_borderYes;
	int m_borderSize;
	COLORREF m_borderColor;
	COLORREF m_backgroundColor;
	CPicture picture;	
	int widthPos;    //WidthHeight 	(Formula): faction of original =  widthPos*0.025 + 0.2 ... form 0.2 to 5.2
	int heightPos;
	CRect m_rectOriginalWnd;

	
	//Temporary state variables
	CMenu menu;	
	CLayeredWindowHelperST G_Layered;
	int trackingOn;
	int editTransOn;
	int editImageOn;
	int menuLoaded;	
	int m_movewindow;
	POINT m_movepoint;	
	HBITMAP m_hbitmap;
	CRgn wndRgn;
	long uniqueID;
	int saveMethod;
	int baseType;

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransparentWnd)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	void InvalidateRegion();
	virtual ~CTransparentWnd();

protected:
	unsigned short m_BitmapID;

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
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

