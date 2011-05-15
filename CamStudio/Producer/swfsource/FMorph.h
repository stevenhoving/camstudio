#ifndef FLASH_MORPH_H_FILE
#define FLASH_MORPH_H_FILE
#include "FShape.h"
#include <vector>

class FlashMorphGradientRecord 
{
public:
	FlashMorphGradientRecord(unsigned char _r1, FlashRGB _c1, unsigned char _r2, FlashRGB _c2) : r1(_r1), c1(_c1), r2(_r2), c2(_c2) {}
	void Write(N_STD::ostream &out);
private:
	unsigned char r1;
	FlashRGB c1;
	unsigned char r2;
	FlashRGB c2;
};

class FlashMorphGradient
{
public:
	FlashMorphGradient() {}
	
	void AddRecord(const FlashMorphGradientRecord &r) { records.push_back(r); }
	void Write(N_STD::ostream &out);
private:
	N_STD::vector<FlashMorphGradientRecord> records;
};

class FlashMorphFillStyle
{
public:
        FlashMorphFillStyle() {}
        virtual ~FlashMorphFillStyle() {}
	virtual void Write(N_STD::ostream &out) = 0;
};

class FlashMorphFillStyleSolid : public FlashMorphFillStyle
{
public:
	FlashMorphFillStyleSolid(FlashRGB &_c1, FlashRGB &_c2) : c1(_c1), c2(_c2) {}
        virtual ~FlashMorphFillStyleSolid() {}
	virtual void Write(N_STD::ostream &out);
private:
	FlashRGB c1;
	FlashRGB c2;
};

class FlashMorphFillStyleLinear : public FlashMorphFillStyle
{
public:
	FlashMorphFillStyleLinear(FlashMatrix &_fm1, FlashMatrix &_fm2, FlashMorphGradient& _fmg) : fm1(_fm1), fm2(_fm2), fmg(_fmg) {}
        virtual ~FlashMorphFillStyleLinear() {}
	virtual void Write(N_STD::ostream &out);
private:	
	FlashMatrix fm1;
	FlashMatrix fm2;
	FlashMorphGradient fmg;

};
class FlashMorphFillStyleRadial : public FlashMorphFillStyle
{
public:
	FlashMorphFillStyleRadial(FlashMatrix &_fm1, FlashMatrix &_fm2, FlashMorphGradient& _fmg) : fm1(_fm1), fm2(_fm2), fmg(_fmg) {}
        virtual ~FlashMorphFillStyleRadial() {}
	virtual void Write(N_STD::ostream &out);
private:	
	FlashMatrix fm1;
	FlashMatrix fm2;
	FlashMorphGradient fmg;
};
class FlashMorphFillStyleBitmap : public FlashMorphFillStyle
{
public:
	FlashMorphFillStyleBitmap(UWORD _bitmapID, FlashMatrix &_mstart, FlashMatrix &_mend, bool _tiled=false) :
	  bitmapID(_bitmapID), mstart(_mstart), mend(_mend), tiled(_tiled) {}
        virtual ~FlashMorphFillStyleBitmap() {}
	virtual void Write(N_STD::ostream &out);
private:
	UWORD bitmapID;
	FlashMatrix mstart;
	FlashMatrix mend;
	bool tiled;

};

class FlashMorphFillStyles
{
public:
        FlashMorphFillStyles() {}
        size_t AddFillStyle(FlashMorphFillStyle* s) { fillStyles.push_back(s); return fillStyles.size();}
        void Write(N_STD::ostream &out);
private:
        N_STD::vector<FlashMorphFillStyle*> fillStyles;
};

class FlashMorphLineStyle
{
public:
	FlashMorphLineStyle(UWORD _w1, FlashRGB _c1, UWORD _w2, FlashRGB _c2) : w1(_w1), c1(_c1), w2(_w2), c2(_c2) {}
	void Write(N_STD::ostream &out);
private:
	UWORD w1;
	FlashRGB c1;
	UWORD w2;
	FlashRGB c2;

};

class FlashMorphLineStyles
{
public:
        FlashMorphLineStyles() {}
        size_t AddLineStyle(FlashMorphLineStyle &s) { lineStyles.push_back(s); return lineStyles.size();}
        void Write(N_STD::ostream &out);
private:
        N_STD::vector<FlashMorphLineStyle> lineStyles;
};

class FlashTagDefineMorphShape : public FlashTag, public FlashIDEnabled
{
DEFINE_RW_INTERFACE
public:	
	FlashTagDefineMorphShape() {}
	FlashTagDefineMorphShape(FlashShape &_s1, FlashShape &_s2) : s1(_s1), s2(_s2) {}

	void SetMorphFillStyles(FlashMorphFillStyles &s) { fs1=s; }
	void SetMorphLineStyles(FlashMorphLineStyles &s) { fs2=s; }

	friend N_STD::ostream &operator<< (N_STD::ostream &out, FlashTagDefineMorphShape &data);
	friend N_STD::istream &operator>> (N_STD::istream &in,  FlashTagDefineMorphShape &data);

private:
	FlashShape s1;
	FlashShape s2;

	FlashMorphFillStyles fs1;
	FlashMorphLineStyles fs2;


};

#endif
