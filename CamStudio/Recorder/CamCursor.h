// CamCursor.h	dlecaration of the CamCursor class 
// Mouse Capture functions
/////////////////////////////////////////////////////////////////////////////
#ifndef CAMCURSOR_H
#define CAMCURSOR_H
#pragma once

#include "Profile.h"

#include <vector>
#include <algorithm>

/////////////////////////////////////////////////////////////////////////////
// CCamCursor
/////////////////////////////////////////////////////////////////////////////
class CCamCursor
{
public:
	CCamCursor()
		: m_bRecord(true)
		, m_iHighlightSize(64)
		, m_bHighlightClick(false)
		, m_clrHighlight(RGB(255,255,125))
		, m_clrClickLeft(RGB(255,0,0))
		, m_clrClickRight(RGB(0,0,255))
	{
	}
	CCamCursor(const CCamCursor& rhs)
	{
		*this = rhs;
	}
	virtual ~CCamCursor()
	{
	}

	CCamCursor& operator=(const CCamCursor& rhs)
	{
		if (this == &rhs)
			return *this;

		m_iSelect			= rhs.m_iSelect;
		m_hSavedCursor		= rhs.m_hSavedCursor;
		m_hLoadCursor		= rhs.m_hLoadCursor;
		m_hCustomCursor		= rhs.m_hCustomCursor;
		m_strDir			= rhs.m_strDir;
		m_strFileName		= m_strFileName;
		m_vIconID			= rhs.m_vIconID;
		m_bRecord			= rhs.m_bRecord;
		m_iCustomSel		= rhs.m_iCustomSel;
		m_bHighlight		= rhs.m_bHighlight;
		m_iHighlightSize	= rhs.m_iHighlightSize;
		m_iHighlightShape	= rhs.m_iHighlightShape;
		m_clrHighlight		= rhs.m_clrHighlight;
		m_bHighlightClick	= rhs.m_bHighlightClick;
		m_clrClickLeft		= rhs.m_clrClickLeft;
		m_clrClickRight		= rhs.m_clrClickRight;

		return *this;
	}

	HCURSOR Load() const					{return m_hLoadCursor;}
	HCURSOR Load(HCURSOR hCursor)			{return m_hLoadCursor = hCursor;}
	HCURSOR Save() const					{return m_hSavedCursor;}
	HCURSOR Save(HCURSOR hCursor)			{return m_hSavedCursor = hCursor;}
	HCURSOR Custom() const					{return m_hCustomCursor;}
	HCURSOR Custom(HCURSOR hCursor)			{return m_hCustomCursor = hCursor;}
	CString Dir() const						{return m_strDir;}
	CString Dir(CString strDir)				{return m_strDir = strDir;}
	CString FileName() const				{return m_strFileName;}
	CString FileName(CString strFileName)	{return m_strFileName = strFileName;}
	int Select() const						{return m_iSelect;}
	int Select(int iSelect)
	{
		return m_iSelect = ((0 <= iSelect) && (iSelect < 3)) ? iSelect : 0;
	}

	HCURSOR FetchCursorHandle(int iCursorType)
	{
		switch (iCursorType)
		{
		case 0:
			return (m_hSavedCursor)
				? m_hSavedCursor
				: (m_hSavedCursor = ::GetCursor());
		case 1:
			return m_hCustomCursor;
		default:
			return m_hLoadCursor;
		}
	}

	void AddID(DWORD dwID)
	{
		if (std::find(m_vIconID.begin(), m_vIconID.end(), dwID) == m_vIconID.end()) {
			m_vIconID.push_back(dwID);
		}
	}

	size_t SizeID() const	{return m_vIconID.size();}
	DWORD GetID(size_t uIndex)
	{
		return (uIndex < m_vIconID.size()) ? m_vIconID[uIndex] : 0;
	}

	bool Record() const						{return m_bRecord;}
	bool Record(bool bRec)					{return m_bRecord = bRec;}
	int CustomType() const					{return m_iCustomSel;}
	int CustomType(int iType)				{return m_iCustomSel = iType;}
	bool Highlight() const					{return m_bHighlight;}
	bool Highlight(bool bHiLight)			{return m_bHighlight = bHiLight;}
	int HighlightSize() const				{return m_iHighlightSize;}
	int HighlightSize(int iSize)			{return m_iHighlightSize = iSize;}
	int HighlightShape() const				{return m_iHighlightShape;}
	int HighlightShape(int iShape)			{return m_iHighlightShape = iShape;}
	COLORREF HighlightColor() const			{return m_clrHighlight;}
	COLORREF HighlightColor(COLORREF clr)	{return m_clrHighlight = clr;}
	bool HighlightClick() const				{return m_bHighlightClick;}
	bool HighlightClick(bool bHiLight)		{return m_bHighlightClick = bHiLight;}
	COLORREF ClickLeftColor() const			{return m_clrClickLeft;}
	COLORREF ClickLeftColor(COLORREF clr)	{return m_clrClickLeft = clr;}
	COLORREF ClickRightColor() const		{return m_clrClickRight;}
	COLORREF ClickRightColor(COLORREF clr)	{return m_clrClickRight = clr;}

	bool Read(CProfile& cProfile)
	{
		VERIFY(cProfile.Read(RECORDCURSOR, m_bRecord));
		VERIFY(cProfile.Read(CURSORTYPE, m_iCustomSel));
		VERIFY(cProfile.Read(CUSTOMSEL, m_iSelect));
		VERIFY(cProfile.Read(HIGHLIGHTCURSOR, m_bHighlight));
		VERIFY(cProfile.Read(HIGHLIGHTSIZE, m_iHighlightSize));
		VERIFY(cProfile.Read(HIGHLIGHTSHAPE, m_iHighlightShape));
		VERIFY(cProfile.Read(HIGHLIGHTCOLOR, m_clrHighlight));	// todo: constant
		VERIFY(cProfile.Read(HIGHLIGHTCLICK, m_bHighlightClick));
		VERIFY(cProfile.Read(HIGHLIGHTCLICKCOLORLEFT, m_clrClickLeft));
		VERIFY(cProfile.Read(HIGHLIGHTCLICKCOLORRIGHT, m_clrClickRight));
		//VERIFY(cProfile.Read(CURSORDIR, m_strFileName));	// what it should be
		int iLen = m_strFileName.GetLength();
		VERIFY(cProfile.Read(CURSORDIR, iLen));
		//VERIFY(cProfile.Write(SAVEDIR, m_strDir));		// what it should be
		iLen = m_strDir.GetLength();
		VERIFY(cProfile.Read(SAVEDIR, iLen));
		return true;
	}

	bool Write(CProfile& cProfile)
	{
		VERIFY(cProfile.Write(RECORDCURSOR, m_bRecord));
		VERIFY(cProfile.Write(CURSORTYPE, m_iCustomSel));
		VERIFY(cProfile.Write(CUSTOMSEL, m_iSelect));
		VERIFY(cProfile.Write(HIGHLIGHTCURSOR, m_bHighlight));
		VERIFY(cProfile.Write(HIGHLIGHTSIZE, m_iHighlightSize));
		VERIFY(cProfile.Write(HIGHLIGHTSHAPE, m_iHighlightShape));
		VERIFY(cProfile.Write(HIGHLIGHTCOLOR, m_clrHighlight));	// todo: constant
		VERIFY(cProfile.Write(HIGHLIGHTCLICK, m_bHighlightClick));
		VERIFY(cProfile.Write(HIGHLIGHTCLICKCOLORLEFT, m_clrClickLeft));
		VERIFY(cProfile.Write(HIGHLIGHTCLICKCOLORRIGHT, m_clrClickRight));
		//VERIFY(cProfile.Write(CURSORDIR, m_strFileName));	// what it should be
		int iLen = m_strFileName.GetLength();
		VERIFY(cProfile.Write(CURSORDIR, iLen));
		//VERIFY(cProfile.Write(SAVEDIR, m_strDir));		// what it should be
		iLen = m_strDir.GetLength();
		VERIFY(cProfile.Write(SAVEDIR, iLen));
		return true;
	}

private:
	// Cursor variables
	int		m_iSelect;			// selected (active) cursor [0..2]
	HCURSOR m_hSavedCursor;		// active screen cursor
	HCURSOR m_hLoadCursor;		// resource cursor
	HCURSOR m_hCustomCursor;	// file cursor
	CString m_strDir;			// directory to load icon images
	CString m_strFileName;
	std::vector <DWORD> m_vIconID;

	bool		m_bRecord;			// record screen cursor
	int			m_iCustomSel;		// cursor type (actual, predef, custom)
	bool		m_bHighlight;		// highlight screen cursor
	int			m_iHighlightSize;	// cursor highlilght size
	int			m_iHighlightShape;	// cursor highlilght shape
	COLORREF	m_clrHighlight;		// cursor highlilght color
	bool		m_bHighlightClick;	// highlight cursor buttons
	COLORREF	m_clrClickLeft;		// left click color
	COLORREF	m_clrClickRight;	// right click color
};

extern CCamCursor CamCursor;		// cursors

#endif	// CAMCURSOR_H
