#pragma once

// CamRect.cpp
BOOL isRectEqual(RECT a, RECT b);
void NormalizeRect(LPRECT prc);
void FixRectSizePos(LPRECT prc, int maxxScreen, int maxyScreen, int minxScreen, int minyScreen);
