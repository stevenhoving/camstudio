// TextAttributes.h
// include file for TextAttributes structure
/////////////////////////////////////////////////////////////////////////////
#ifndef TEXTATTRIBUTES_H	// because pragma once is compiler specific
#define TEXTATTRIBUTES_H

#pragma once

enum ePosition
{
	TOP_LEFT
	, TOP_CENTER
	, TOP_RIGHT
	, CENTER_LEFT
	, CENTER_CENTER
	, CENTER_RIGHT
	, BOTTOM_LEFT
	, BOTTOM_CENTER
	, BOTTOM_RIGHT
};

struct TextAttributes
{
	TextAttributes(ePosition pos = TOP_LEFT, CString strText = "", COLORREF clrBackground = 0UL, COLORREF clrText = 0UL)
		: position(pos)
		, text(strText)
		, backgroundColor(clrBackground)
		, textColor(clrText)
		, isFontSelected(FALSE)
	{
	}

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
	ePosition	position;
	CString		text;
	COLORREF	backgroundColor;
	COLORREF	textColor;
	int			isFontSelected;
	LOGFONT		logfont;
};

#endif	// TEXTATTRIBUTES_H
