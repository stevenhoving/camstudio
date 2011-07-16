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

#include "LayeredWindowHelperST.h"
#include "Picture.h"

const int regionNULL			 = 0;
const int regionTRANSPARENTCOLOR = 1;
const int regionSHAPE			 = 2;

const int regionROUNDRECT	= 0;
const int regionELLIPSE		= 1;
const int regionRECTANGLE	= 2;

const int saveMethodNew		= 0;
const int saveMethodReplace	= 1;

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
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

	void RefreshWindowSize();
	void EnsureOnTopList(CTransparentWnd* transWnd);

	// accessor/mutators
	bool EditImageOn() const					{return m_bEditImageOn;}
	bool EditTransOn() const					{return m_bEditTransOn;}
	bool TrackingOn() const						{return m_bTrackingOn;}
	bool BorderYes(bool bBorder)				{return m_bBorderYes = bBorder;}
	bool BorderYes() const						{return m_bBorderYes;}
	int BorderSize(int iSize)					{return m_borderSize = iSize;}
	int BorderSize() const						{return m_borderSize;}
	int RegionPredefinedShape(int iShape)		{return m_regionPredefinedShape = iShape;}
	int RegionPredefinedShape() const			{return m_regionPredefinedShape;}
	int RegionCreated(bool iCreated)			{return m_regionCreated = iCreated;}
	int RegionCreated() const					{return m_regionCreated;}
	int BaseType() const						{return m_baseType;}
	int RegionType(int iType)					{return m_regionType = iType;}
	int RegionType() const						{return m_regionType;}
	int SaveMethod(int iMethod)					{return m_saveMethod = iMethod;}
	int SaveMethod() const						{return m_saveMethod;}
	int WidthPos(int iWidth)					{return m_widthPos = iWidth;}
	int WidthPos() const						{return m_widthPos;}
	int HeightPos(int iHeight)					{return m_heightPos = iHeight;}
	int HeightPos() const						{return m_heightPos;}
	long UniqueID() const						{return m_uniqueID;}
	CString ShapeString(CString str)			{return m_shapeStr = str;}
	CString ShapeString() const					{return m_shapeStr;}
	CString TextString() const					{return m_textstring;}
	CString TextString(CString str)				{return m_textstring = str;}
	CRect RectWnd() const						{return m_rectWnd;}
	COLORREF BorderColor(COLORREF color)		{return m_borderColor = color;}
	COLORREF BorderColor() const				{return m_borderColor;}
	COLORREF TransparentColor() const			{return m_transparentColor;}
	COLORREF TransparentColor(COLORREF color)	{return m_transparentColor = color;}
	COLORREF BackgroundColor() const			{return m_backgroundColor;}
	COLORREF BackgroundColor(COLORREF color)	{return m_backgroundColor = color;}
	HBITMAP BitMap() const						{return m_hbitmap;}

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransparentWnd)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	void InvalidateRegion();

protected:
	void CopyMembers(const CTransparentWnd& rhsWnd);

private:
	HANDLE AllocMakeDib(int reduced_width, int reduced_height, UINT iBits);

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
	//afx_msg LRESULT OnInvalidateWnd(WPARAM p1, LPARAM p2);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	//Data to be saved
protected:
	bool m_bEditImageOn;
	bool m_bEditTransOn;
	bool m_bTrackingOn;
	bool m_bBorderYes;
	int m_borderSize;
	int m_regionPredefinedShape;	// region for pre-defined shape, var applicable only to regiontype -- predefined shape
	bool m_regionCreated;			// region for transparent color already created, var applicable only to regiontype  -- transparent color
	int m_baseType;
	int m_regionType;
	int m_saveMethod;
	int m_widthPos;					// Width-Height (Formula): faction of original =  widthPos*0.025 + 0.2 ... form 0.2 to 5.2
	int m_heightPos;
	long m_uniqueID;
	CString m_shapeStr;				// name of the shape
	CString m_textstring;
	CRect m_rectWnd;
	COLORREF m_borderColor;
	COLORREF m_transparentColor;	// region for pre-defined shape, var applicable only to regiontype  -- transparent color
	COLORREF m_backgroundColor;
	HBITMAP m_hbitmap;

	unsigned short m_BitmapID;
	int m_menuLoaded;
	int m_horzalign;
	double m_roundrectFactor;
	CRectTracker m_tracker;
	CMenu m_menu;
	LOGFONT m_textfont;
	COLORREF m_rgb;					// textcolor
private:
	int m_vertalign;
	int m_factor;
	int m_charset;
	int m_valueTransparency;
	bool m_benable;
	CPicture m_picture;
	CRect m_rectOriginalWnd;

	// Temporary state variables
	CLayeredWindowHelperST G_Layered;
	int m_movewindow;
	POINT m_movepoint;
	CRgn m_wndRgn;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

