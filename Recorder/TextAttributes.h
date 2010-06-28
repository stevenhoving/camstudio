// TextAttributes.h
// include file for TextAttributes structure
/////////////////////////////////////////////////////////////////////////////
#ifndef TEXTATTRIBUTES_H	// because pragma once is compiler specific
#define TEXTATTRIBUTES_H

#pragma once

enum ePosType
{
	UNDEFINED
	, PREDEFINED_POSITION
	, ANY_POSITION
};


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

/*
TODO
struct OffsetRatio
{
	OffsetRatio( int xRatio = 0; int yRatio = 0 )
		: cx ( xRatio)				// Scale  LEFT=0, CENTER= 0.5, RIGHT= 1
		, cy ( yRatio)				// Scale  TOP=0,  CENTER= 0.5, BOTTOM= 1			
	{
	}
}
*/

struct TextAttributes
{
	TextAttributes( ePosition pos = TOP_LEFT, ePosType posType = UNDEFINED ,int xRatio = 0, int yRatio = 0, CString strText = "", COLORREF clrBackground = 0UL, COLORREF clrText = 0UL)
		: position(pos)
		, posType(posType)
		, xPosRatio(xRatio)
		, yPosRatio(yRatio)
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
		posType			= rhs.posType;
		xPosRatio		= rhs.xPosRatio;		// Scale  LEFT=0, CENTER= 50, RIGHT=100
		yPosRatio		= rhs.yPosRatio;		// Scale  TOP=0,  CENTER= 50, BOTTOM=100
		text			= rhs.text;
		backgroundColor = rhs.backgroundColor;
		textColor		= rhs.textColor;
		isFontSelected	= rhs.isFontSelected;
		logfont			= rhs.logfont;	// bitwise copy is OK; logfont is POD
		//::memcpy(&logfont, &rhs.logfont, sizeof(LOGFONT));

		return *this;
	}
	ePosition	position;
	ePosType	posType;
	int			xPosRatio;
	int			yPosRatio;
	CString		text;
	COLORREF	backgroundColor;
	COLORREF	textColor;
	int			isFontSelected;
	LOGFONT		logfont;
};

#endif	// TEXTATTRIBUTES_H
