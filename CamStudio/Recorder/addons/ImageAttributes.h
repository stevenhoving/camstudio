// ImageAttributes.h
// include file for ImageAttributes structure
/////////////////////////////////////////////////////////////////////////////
#ifndef IMAGEATTRIBUTES_H	// because pragma once is compiler specific
#define IMAGEATTRIBUTES_H

#pragma once

#include "TextAttributes.h"	// for position

struct ImageAttributes
{
	ImageAttributes(ePosition pos = TOP_LEFT, ePosType posType = UNDEFINED, int xRatio = 0 , int yRatio = 0, CString strText = "")
		: position(pos)
		, posType(posType)
		, xPosRatio(xRatio)
		, yPosRatio(yRatio)
		, text(strText)
		, m_lBrightness(0L)
		, m_lContrast(0L)
	{
	}

	const ImageAttributes& operator=(const ImageAttributes& rhs)
	{
		if (this == &rhs)
			return *this;

		text			= rhs.text;
		position		= rhs.position;
		posType			= rhs.posType;
		xPosRatio		= rhs.xPosRatio;		// Scale  LEFT=0, CENTER= 50, RIGHT=100
		yPosRatio		= rhs.yPosRatio;		// Scale  TOP=0,  CENTER= 50, BOTTOM=100
		m_lBrightness	= rhs.m_lBrightness;
		m_lContrast		= rhs.m_lContrast;
		return *this;
	}
	ePosition	position;
	ePosType	posType;
	int			xPosRatio;
	int			yPosRatio;
	CString		text;
	long		m_lBrightness;
	long		m_lContrast;
};

#endif	// IMAGEATTRIBUTES_H
