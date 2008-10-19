// ImageAttributes.h
// include file for ImageAttributes structure
/////////////////////////////////////////////////////////////////////////////
#ifndef IMAGEATTRIBUTES_H	// because pragma once is compiler specific
#define IMAGEATTRIBUTES_H

#pragma once

#include "TextAttributes.h"	// for position

struct ImageAttributes
{
	ePosition position;
	CString text;

	const ImageAttributes& operator=(const ImageAttributes& obj)
	{
		if (this == &obj)
			return *this;

		text = obj.text;
		position = obj.position;
		return *this;
	}
};

#endif	// IMAGEATTRIBUTES_H
