// ImageAttributes.h
// include file for ImageAttributes structure
/////////////////////////////////////////////////////////////////////////////
#ifndef IMAGEATTRIBUTES_H	// because pragma once is compiler specific
#define IMAGEATTRIBUTES_H

#pragma once

#include "TextAttributes.h"	// for position

struct ImageAttributes
{
	ImageAttributes(ePosition pos = TOP_LEFT, CString strText = "")
		: position(pos)
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
		m_lBrightness	= rhs.m_lBrightness;
		m_lContrast		= rhs.m_lContrast;
		return *this;
	}
	ePosition	position;
	CString		text;
	long		m_lBrightness;
	long		m_lContrast;
};

#endif	// IMAGEATTRIBUTES_H
