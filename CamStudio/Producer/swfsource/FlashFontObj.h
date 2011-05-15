#ifndef FLASHFONTOBJ_H
#define FLASHFONTOBJ_H

#include "FShape.h"
#include <iostream>

class FlashFontFactory
{
public:
	FlashFontFactory() {}

	bool GetGlyphShape(const char *fontname, UWORD charindex, FlashShape& s,bool bold = false, bool italic = false, bool uLine = false);
    int GetGlyphAdvance(const char *fontname, UWORD charindex, int pointsize);
	
	UWORD WriteText(std::ostream &out, const char *fontname, const char *text, int x, int y, FlashRGB color, int pointsize, int depth, int extraspacing = 0, bool bold = false, bool italic = false, bool uLine = false);
	UWORD WriteText(std::ostream &out, const char *fontname, const char *text, int x, int y, FlashRGB color, int pointsize, int depth, FlashRect& textBounds, int extraspacing = 0, bool bold =false ,bool italic = false,bool uLine = false);
	UWORD GetBounds(std::ostream &out, const char *fontname, const char *text, int x, int y, FlashRGB color, int pointsize, int depth, FlashRect& textBounds, int extraspacing = 0, bool bold =false ,bool italic = false,bool uLine = false);

};

#endif
