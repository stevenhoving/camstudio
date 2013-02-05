// CamRect.h	- include file for CamStudio Library rectangle api's
/////////////////////////////////////////////////////////////////////////////
#ifndef CAMRECT_H
#define CAMRECT_H

#pragma once

// CamRect.cpp
BOOL isRectEqual(RECT a, RECT b);
void NormalizeRect(LPRECT prc);
void FixRectSizePos(LPRECT prc,int maxxScreen, int maxyScreen, int minxScreen, int minyScreen);

#endif	// CAMRECT_H
