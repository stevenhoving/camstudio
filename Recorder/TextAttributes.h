// TextAttributes.h
// include file for TextAttributes structure
/////////////////////////////////////////////////////////////////////////////
#ifndef TEXTATTRIBUTES_H	// because pragma once is compiler specific
#define TEXTATTRIBUTES_H

#pragma once

enum Position
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
	Position position;
	CString text;
	COLORREF backgroundColor;
	COLORREF textColor;
	int isFontSelected;
	LOGFONT logfont;

	const TextAttributes& operator=(const TextAttributes& obj)
	{
		if (this == &obj)
			return *this;

		text = obj.text;
		::memcpy(&logfont, &obj.logfont, sizeof(LOGFONT));
		isFontSelected = obj.isFontSelected;
		backgroundColor = obj.backgroundColor;
		textColor = obj.textColor;
		position = obj.position;

		return *this;
	}
};

#endif	// TEXTATTRIBUTES_H
