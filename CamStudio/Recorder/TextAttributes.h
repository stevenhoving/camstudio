// TextAttributes.h
// include file for TextAttributes structure
/////////////////////////////////////////////////////////////////////////////
#ifndef TEXTATTRIBUTES_H	// because pragma once is compiler specific
#define TEXTATTRIBUTES_H

#pragma once

enum ePosition
{
	TOP_LEFT = 0,
	TOP_CENTER = 1,
	TOP_RIGHT = 2,
	CENTER_LEFT = 3,
	CENTER_CENTER = 4,
	CENTER_RIGHT = 5,
	BOTTOM_LEFT = 6,
	BOTTOM_CENTER = 7,
	BOTTOM_RIGHT = 8,
};

struct TextAttributes
{
	ePosition position;
	CString text;
	COLORREF backgroundColor;
	COLORREF textColor;
	int isFontSelected;
	LOGFONT logfont;

	const TextAttributes& operator=(const TextAttributes& rhs)
	{
		if (this == &rhs)
			return *this;

		position		= rhs.position;
		text			= rhs.text;
		backgroundColor = rhs.backgroundColor;
		textColor		= rhs.textColor;
		isFontSelected	= rhs.isFontSelected;
		logfont			= rhs.logfont;	// bitwise copy is OK; logfont is POD
		//::memcpy(&logfont, &rhs.logfont, sizeof(LOGFONT));

		return *this;
	}
};

#endif	// TEXTATTRIBUTES_H
