#ifndef FFONT_H_FILE
#define FFONT_H_FILE

#include "FBase.h"
#include "FShape.h"

#include <vector>
#include <hash_map>

typedef FlashIDFactory FlashFontIDFactory;
typedef FlashIDEnabled FlashFontIDEnabled;

#define FTDFI_UNICODE		(1 << 5)
#define FTDFI_SHIFTJIS		(1 << 4)
#define FTDFI_ANSI			(1 << 3)
#define FTDFI_ITALIC		(1 << 2)
#define FTDFI_BOLD			(1 << 1)
#define FTDFI_WIDECODES		(1)

class FlashTagDefineFontInfo : public FlashTag
{
DEFINE_RW_INTERFACE
public:
	FlashTagDefineFontInfo(char *_str, unsigned char _flags, UWORD id) : str(_str), flags(_flags), FontID(id) {}

	void AddCode(UWORD code) { codes.push_back(code); }

	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineFontInfo &data);
	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineFontInfo &data);

private:
	UWORD FontID;
	N_STD::vector<UWORD> codes;
	gc_vector<char *> strings;
	char *str;
	unsigned char flags;
};


class FlashTagDefineFont : public FlashTag, public FlashFontIDEnabled
{
DEFINE_RW_INTERFACE
public:
	FlashTagDefineFont() {}
	
	int AddShape(FlashShape& shape);
	int AddShape(FlashShape& shape, int glyphId);
	int GetGlyphId(int glyph);
	
	int GetShapeCount() { return shapes.size(); }

	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineFont &data);
	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineFont &data);
private:
	N_STD_HMAP::hash_map<int, int> glyphs;
	N_STD::vector<FlashShape> shapes;
};

class FlashTextRecord : public FlashVersionEnabled
{
public:	
	FlashTextRecord() {};
	virtual void Write(N_STD::ostream &out, unsigned char bitsGlyph=0, unsigned char bitsAdvance=0) = 0;
	virtual bool isGlyph(void) = 0;
	virtual int returnGlyphBits(void) { return -1; }
	virtual int returnAdvBits(void) { return -1; }
};

class FlashTagDefineText;
class FlashTagDefineText2;

class FlashTextRecordStyle : public FlashTextRecord
{
public:
	FlashTextRecordStyle(bool hasFont, bool hasFlashRGB, bool hasOffsetx, bool hasOffsety, 
		UWORD FontID, UWORD FontHeight, FlashRGB color, SWORD offsetx, SWORD offsety) :		
			mhasFont(hasFont), mhasFlashRGB(hasFlashRGB), mhasOffsetx(hasOffsetx), mhasOffsety(hasOffsety), 
			mFontID(FontID), mFontHeight(FontHeight), mcolor(color), moffsetx(offsetx), moffsety(offsety) {}
	
	virtual void Write(N_STD::ostream &out, unsigned char bitsGlyph=0, unsigned char bitsAdvance=0);
	virtual void Read(N_STD::istream &in, unsigned char bitsGlyph=0, unsigned char bitsAdvance=0);

	virtual bool isGlyph(void);
	

private:
	FlashTextRecordStyle() {}
	bool mhasFont;
	bool mhasFlashRGB;
	bool mhasOffsetx;
	bool mhasOffsety;
	UWORD mFontID;
	UWORD mFontHeight;
	FlashRGB mcolor;
	SWORD moffsetx;
	SWORD moffsety;

	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineText &data);
	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineText2 &data);
	

};
typedef flash_pair<UWORD, SWORD> FlashGlyphEntry;

class FlashTextRecordGlyph : public FlashTextRecord
{
	
public:
    FlashTextRecordGlyph() {}
    virtual ~FlashTextRecordGlyph() {}
	virtual void Write(N_STD::ostream &out, unsigned char bitsGlyph=0, unsigned char bitsAdvance=0);
	virtual void Read(N_STD::istream &in, unsigned char bitsGlyph=0, unsigned char bitsAdvance=0);
	virtual bool isGlyph(void) { return true; }
	virtual int returnGlyphBits(void);
	virtual int returnAdvBits(void);

	void AddGlyph(FlashGlyphEntry &e) { v.push_back(e); }
	int GetGlyphCount() { return v.size(); }
	N_STD::vector<FlashGlyphEntry> GetGlyphEntrys() { return v; }

private:
	void FlashTextRecordStyle();
	N_STD::vector<FlashGlyphEntry> v;
};

class FlashTagDefineText : public FlashTag, public FlashIDEnabled
{
DEFINE_RW_INTERFACE
public:
	FlashTagDefineText(FlashRect &r, FlashMatrix &m) : rect(r), matrix(m) {}

	void AddTextRecord(FlashTextRecord *r);

	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineText &data);
	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineText &data);

private:
	FlashRect rect;
	FlashMatrix matrix;
	N_STD::vector<FlashTextRecord *> records;
	gc_vector<FlashTextRecord *> gc_records;

};

class FlashTagDefineText2 : public FlashTag, public FlashIDEnabled
{
DEFINE_RW_INTERFACE
public:
	FlashTagDefineText2(FlashRect &r, FlashMatrix &m) : rect(r), matrix(m) {}

	void AddTextRecord(FlashTextRecord *r);

	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineText2 &data);
	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineText2 &data);

private:
	FlashRect rect;
	FlashMatrix matrix;
	N_STD::vector<FlashTextRecord *> records;
	gc_vector<FlashTextRecord *> gc_records;

};


class FlashKerningRecord : public FlashVersionEnabled
{
public:
	FlashKerningRecord(UWORD _code1, UWORD _code2, SWORD _adjustment) : code1(_code1), code2(_code2), adjustment(_adjustment) {}
	FlashKerningRecord() {}
private:

	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashKerningRecord &data);
	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashKerningRecord &data);

	UWORD code1;
	UWORD code2;
	SWORD adjustment;

};

#define FTDF2_HASLAYOUT			(1 << 7)
#define FTDF2_SHIFTJIS			(1 << 6)
#define FTDF2_UNICODE			(1 << 5)
#define FTDF2_ANSI				(1 << 4)
#define FTDF2_WIDEOFFSETS		(1 << 3)
#define FTDF2_WIDECODES			(1 << 2)
#define FTDF2_ITALIC			(1 << 1)
#define FTDF2_BOLD				(1)

class FlashFontLayout
	{		
	public:
		FlashFontLayout() {}
		FlashFontLayout(SWORD ascent, SWORD descent, SWORD leading,
			N_STD::vector<SWORD> &advance) : fontAscent(ascent), fontDescent(descent),
			fontLeading(leading), fontAdvanceTable(advance) {}
		
		FlashFontLayout(SWORD ascent, SWORD descent, SWORD leading,
			N_STD::vector<SWORD> &advance, N_STD::vector<FlashRect> &bounds,
			N_STD::vector<FlashKerningRecord> &kerning) 
			: fontAscent(ascent), fontDescent(descent),
			fontLeading(leading), fontAdvanceTable(advance),
			fontBoundsTable(bounds), fontKerningTable(kerning){}
		
		void Write(N_STD::ostream &out);
		void Read(N_STD::istream &in, UWORD NumGlyphs);

		N_STD::vector<SWORD> &GetAdvanceTable()
		{
			return fontAdvanceTable;
		}

		N_STD::vector<FlashRect> &GetBoundsTable()
		{
			return fontBoundsTable;
		}

		SWORD fontAscent;
		SWORD fontDescent;
		SWORD fontLeading;

	private:
		N_STD::vector<SWORD>fontAdvanceTable;
		
		N_STD::vector<FlashRect> fontBoundsTable;
		N_STD::vector<FlashKerningRecord> fontKerningTable;
	};

class FlashTagDefineFont2 : public FlashTag, public FlashFontIDEnabled
{
	DEFINE_RW_INTERFACE
public:
	FlashTagDefineFont2(unsigned char fontflags,	const char *_fontname) : flags(fontflags), fontname(_fontname), layout(false) {}
	
	void SetLayout(FlashFontLayout &l) {layout = true; layout_data = l; }	
	FlashFontLayout &GetLayout() { return layout_data; }	
	void AddCode(UWORD code) { codes.push_back(code); }

	int AddShape(FlashShape& shape) { shapes.push_back(shape); int index = shapes.size() - 1; AddCode(index); return index; }
	int AddShape(FlashShape& shape, int glyphId) { shapes.push_back(shape);
											    	int index = shapes.size() - 1;
													glyphs[glyphId] = index;
													AddCode(index);
													return index;
												 }

	int GetGlyphId(int glyph)
	{
		if(glyphs.find(glyph) == glyphs.end())
		{
			return -1;
		}
		return glyphs[glyph];
	}
	
	int GetShapeCount() { return shapes.size(); }

private:

	N_STD_HMAP::hash_map<int, int> glyphs;
	N_STD::vector<UWORD> codes;
	N_STD::vector<FlashShape> shapes;
	bool layout;
    unsigned char flags;
	const char *fontname;
	FlashFontLayout layout_data;

	
	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineFont2 &data);
	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineFont2 &data);

};

#define FTDEB_HASTEXT			(1 << 15)
#define FTDEB_WORDWRAP			(1 << 14)
#define FTDEB_MULTILINE			(1 << 13)
#define FTDEB_PASSWORD			(1 << 12)
#define FTDEB_READONLY			(1 << 11)
#define FTDEB_HASTEXTCOLOR		(1 << 10)
#define FTDEB_HASMAXLENGTH		(1 << 9)
#define FTDEB_HASFONT			(1 << 8)
//reserved 2 bits
#define FTDEB_HASLAYOUT			(1 << 5)
#define FTDEB_NOSELECT			(1 << 4)
#define FTDEB_BORDER			(1 << 3)
//reserved 1 bit
#define FTDEB_HTML   			(1 << 1)
#define FTDEB_USEOUTLINES		(1)

class FlashTagDefineEditBox : public FlashTag, public FlashIDEnabled
{
DEFINE_RW_INTERFACE
public:	
	FlashTagDefineEditBox(FlashRect &r, UWORD flags, UWORD fontID, UWORD fontHeight, 
						  FlashRGB color /*WITH ALPHA*/, UWORD maxLength, unsigned char align, 
						  UWORD leftmargin, UWORD rightmargin, UWORD indent, UWORD leading, 
						  char *variable, char *initialtext) :
						mr(r), mflags(flags), mfontID(fontID),mfontHeight(fontHeight),mcolor(color),
						mmaxLength(maxLength), malign(align), mleftmargin(leftmargin), mrightmargin(rightmargin),
						mindent(indent), mleading(leading), mvariable(variable), 
						minitialtext(initialtext) {}
private:

	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineEditBox &data);
	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineEditBox &data);

	gc_vector<char *> gc;
	FlashRect mr;
	UWORD mflags; 
	UWORD mfontID; 
	UWORD mfontHeight;
	FlashRGB mcolor /*WITH ALPHA*/; 
	UWORD mmaxLength; 
	unsigned char malign;
	UWORD mleftmargin; 
	UWORD mrightmargin; 
	UWORD mindent; 
	UWORD mleading;
	char *mvariable; 
	char *minitialtext;
};

// TODO : Flash MX tag 62 - new font info (something like a FlashTagDefineFontInfo2?):
// font_id - UI16 
// name_length - UI8 
// name - name_length bytes 
// unknown - UI16 
// character codes for referenced fonts - UI16[nglyphs] - could be unicode 

#endif
