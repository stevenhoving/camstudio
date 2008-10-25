// CamCursor.h	dlecaration of the CamCursor class 
// Mouse Capture functions
/////////////////////////////////////////////////////////////////////////////
#ifndef CAMCURSOR_H
#define CAMCURSOR_H
#pragma once

#include <vector>
#include <algorithm>

/////////////////////////////////////////////////////////////////////////////
// CCamCursor
/////////////////////////////////////////////////////////////////////////////
class CCamCursor
{
public:
	CCamCursor()
	{
	}
	virtual ~CCamCursor()
	{
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
private:
	// Cursor variables
	int		m_iSelect;			// selected (active) cursor [0..2]
	HCURSOR m_hSavedCursor;		// active screen cursor
	HCURSOR m_hLoadCursor;		// resource cursor
	HCURSOR m_hCustomCursor;	// file cursor
	CString m_strDir;			// directory to load icon images
	CString m_strFileName;
	std::vector <DWORD> m_vIconID;
};

extern CCamCursor CamCursor;		// cursors

#endif	// CAMCURSOR_H
