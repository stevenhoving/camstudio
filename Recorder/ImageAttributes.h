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
	{
	}

	const ImageAttributes& operator=(const ImageAttributes& obj)
	{
		if (this == &obj)
			return *this;

		text = obj.text;
		position = obj.position;
		return *this;
	}
	ePosition	position;
	CString		text;
};

#endif	// IMAGEATTRIBUTES_H
