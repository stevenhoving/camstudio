// CamCursor.h	dlecaration of the CamCursor class 
// Mouse Capture functions
/////////////////////////////////////////////////////////////////////////////
#ifndef CAMCURSOR_H
#define CAMCURSOR_H
#pragma once

#include "Profile.h"

#include <vector>
#include <algorithm>
// TODO: reorganize code and move bodies to cpp. Even better to merge CamCursor with CursorOptionsDlg class.
//#include "../hook/hook.h" // for getCursor

/////////////////////////////////////////////////////////////////////////////
// CCamCursor
/////////////////////////////////////////////////////////////////////////////
enum eCursorType
{
	ACTIVE
	, CUSTOM
	, CUSTOMFILE
};

class CCamCursor
{
public:
	CCamCursor()
		: m_bRecord(true)
		, m_iSelect(ACTIVE)
		, m_iHighlightSize(64)
		, m_bHighlightClick(false)
		, m_clrHighlight(0xa0ffff80)
		, m_clrClickLeft(0xa0ff0000)
		, m_clrClickRight(0xa00000ff)
		, m_clrClickMiddle(0xa000ff00)
		, m_fRingWidth(1.5)
		, m_iRingSize(20)
		, m_iRingThreshold(1000)
	{
	}
	CCamCursor(const CCamCursor& rhs)
	{
		*this = rhs;
	}
	virtual ~CCamCursor()
	{
	}

	// TODO: can't we just use default copy constructor???
	/*
	CCamCursor& operator=(const CCamCursor& rhs)
	{
		if (this == &rhs)
			return *this;

		//TRACE("CCamCursor::Save assignment\n");
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
	}*/

	HCURSOR Load() const					{return m_hLoadCursor;}
	HCURSOR Load(HCURSOR hCursor)			{return m_hLoadCursor = hCursor;}
	HCURSOR Save() const					{return m_hSavedCursor;}
	HCURSOR Save(HCURSOR hCursor)
	{	// mlt_msk: what are we saving on? int assignment?
		return (m_hSavedCursor == hCursor)
			? m_hSavedCursor
			: (m_hSavedCursor = hCursor);
	}
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

	HCURSOR Cursor()	{return Cursor(m_iSelect);}
	HCURSOR Cursor(int iCursorType)
	{
		switch (iCursorType)
		{
		default:
		case ACTIVE:
			//m_hSavedCursor = getCursor();
			return m_hSavedCursor;
		case CUSTOM:
			return m_hCustomCursor;
		case CUSTOMFILE:
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

	bool Read(Setting& cProfile)
	{
		cProfile.lookupValue("RecordCursor", m_bRecord);
		cProfile.lookupValue("CursorType", m_iCustomSel);
		cProfile.lookupValue("CursorSel", m_iSelect);
		cProfile.lookupValue("Highlight", m_bHighlight);
		cProfile.lookupValue("HighlightSize", m_iHighlightSize);
		cProfile.lookupValue("HighlightShape", m_iHighlightShape);
		cProfile.lookupValue("HighlightColor", (int&)m_clrHighlight);
		cProfile.lookupValue("HighlightClick", m_bHighlightClick);
		cProfile.lookupValue("RingThreshold", m_iRingThreshold);
		cProfile.lookupValue("RingSize", m_iRingSize);
		cProfile.lookupValue("RingWidth", m_fRingWidth);
		cProfile.lookupValue("ClickColorLeft", (int&)m_clrClickLeft);
		cProfile.lookupValue("ClickColorMiddle", (int&)m_clrClickMiddle);
		cProfile.lookupValue("ClickColorRight", (int&)m_clrClickRight);
		std::string text;
		if (cProfile.lookupValue("CursorDir", text))
			m_strFileName = text.c_str();
		return true;
	}

	bool Write(Setting& cProfile)
	{
		UpdateSetting(cProfile,"RecordCursor", m_bRecord, Setting::TypeBoolean);
		UpdateSetting(cProfile,"CursorType", m_iCustomSel, Setting::TypeInt);
		UpdateSetting(cProfile,"CursorSel", m_iSelect,Setting::TypeInt);
		UpdateSetting(cProfile,"Highlight", m_bHighlight, Setting::TypeBoolean);
		UpdateSetting(cProfile,"HighlightSize", m_iHighlightSize, Setting::TypeInt);
		UpdateSetting(cProfile,"HighlightShape", m_iHighlightShape, Setting::TypeInt);
		UpdateSetting(cProfile,"HighlightColor", (long&)m_clrHighlight, Setting::TypeInt);
		UpdateSetting(cProfile,"RingThreshold", m_iRingThreshold, Setting::TypeInt);
		UpdateSetting(cProfile,"RingSize", m_iRingSize, Setting::TypeInt);
		UpdateSetting(cProfile,"RingWidth", m_fRingWidth, Setting::TypeFloat);
		UpdateSetting(cProfile,"HighlightClick", m_bHighlightClick, Setting::TypeBoolean);
		UpdateSetting(cProfile,"ClickColorLeft", (long&)m_clrClickLeft, Setting::TypeInt);
		UpdateSetting(cProfile,"ClickColorMiddle", (long&)m_clrClickMiddle, Setting::TypeInt);
		UpdateSetting(cProfile,"ClickColorRight", (long&)m_clrClickRight, Setting::TypeInt);
		std::string text(m_strFileName);
		UpdateSetting(cProfile,"CursorDir", text, Setting::TypeString);
		return true;
	}

	// why do we ever want to make everything private and use getter/setter even for plain types with no onchange events???
	// let's keep it simple
	DWORD	m_clrClickMiddle;	// wheel click color ARGB
	float		m_fRingWidth;	// ring width for click events, Gdiplus::REAL
	int			m_iRingThreshold;	// time in ms for ring to grow till m_iRingSize
	int			m_iRingSize;	// maximum ring size

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
