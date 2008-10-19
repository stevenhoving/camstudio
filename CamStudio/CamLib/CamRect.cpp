// recatangle.cpp	- CamStudio Library rectangle functions
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "CamRect.h"

BOOL isRectEqual(RECT a, RECT b)
{
	CRect rectA(a);
	return rectA == b;

	//if ((a.left == b.left)
	//	&& (a.right == b.right)
	//	&& (a.top == b.top)
	//	&& (a.bottom == b.bottom))
	//	return TRUE;
	//else
	//	return FALSE;
}

// The rectangle is normalized for fourth-quadrant positioning, which Windows typically uses for coordinates.
// this is dubious
void NormalizeRect(LPRECT prc)
{
	CRect rectSrc(*prc);
	rectSrc.NormalizeRect();
	::CopyRect(prc, rectSrc);
}

void FixRectSizePos(LPRECT prc, int maxxScreen, int maxyScreen)
{
	NormalizeRect(prc);

	int width = prc->right - prc->left + 1;
	int height = prc->bottom - prc->top + 1;

	if (width>maxxScreen) {
		prc->left = 0;
		prc->right = maxxScreen - 1;
	}

	if (height > maxyScreen) {
		prc->top = 0;
		prc->bottom = maxyScreen - 1;
	}

	if (prc->left < 0) {
		prc->left = 0;
		prc->right = width - 1;
	}

	if (prc->top < 0) {
		prc->top = 0;
		prc->bottom = height - 1;
	}

	if (prc->right > maxxScreen - 1) {
		prc->right = maxxScreen - 1;
		prc->left = maxxScreen - width;
	}

	if (prc->bottom > maxyScreen - 1) {
		prc->bottom = maxyScreen - 1;
		prc->top= maxyScreen - height;
	}
}
