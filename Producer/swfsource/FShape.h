#ifndef FSHAPE_H_FILE
#define FSHAPE_H_FILE

#include <vector>
#include "FBase.h"

class FlashShapeCommon
{
public:
	FlashShapeCommon() : NBitsFillStyle(0), NBitsLineStyle(0), DefineTagVersion(1), flags(0) {}

	int NBitsFillStyle;
	int NBitsLineStyle;
	int DefineTagVersion;
	int flags;
};

class FlashFillStyle : public FlashVersionEnabled, public FlashCopyEnabled<FlashFillStyle>
{
public:	
	FlashFillStyle() : type(0) {}
	FlashFillStyle(UBYTE _type) : type(_type){}
	
	virtual ~FlashFillStyle() 
	{
	}

	DEFINE_FLASHCOPY_INTERFACE( FlashFillStyle, FlashFillStyle );

	virtual void Write(N_STD::ostream &out) {}
	virtual void Read(N_STD::istream &in) {}
	
	UBYTE GetType() const { return type; }

protected:
	UBYTE type;
	
	friend N_STD::ostream &operator<< (N_STD::ostream &out, FlashFillStyle &data);
	friend N_STD::istream &operator>> (N_STD::istream &in,  FlashFillStyle &data);

};
	/* FILL TYPES
		0x00 - solid
		0x10 - linear gradient
		0x12 - radial gradient
		0x40 - tiled bitmap
		0x41 - clipped bitmap
	*/
class FlashFillStyleArray;
class FlashFillStyleSolid : public FlashFillStyle 
{
public:	
	FlashFillStyleSolid(FlashRGB  &color) : FlashFillStyle(0), r1(color) { type = 0; }
	virtual ~FlashFillStyleSolid() 
	{
	}

	DEFINE_FLASHCOPY_INTERFACE( FlashFillStyleSolid, FlashFillStyle )

	virtual void Write(N_STD::ostream &out);
	virtual void Read(N_STD::istream &in);
	
	FlashRGB GetRGB() { return r1; }
	void SetRGB(FlashRGB &r) { r1=r; }
private:
	friend N_STD::istream &operator>> (N_STD::istream &in,  FlashFillStyleArray &data);
	FlashFillStyleSolid() {}

	FlashRGB r1;
};

class FlashGradientRecord : public FlashVersionEnabled
{
public:	
	FlashGradientRecord() {}
	virtual ~FlashGradientRecord() {}
	void AddGradient(UBYTE ratio, FlashRGB &r) { colors.push_back(r); ratios.push_back(ratio); }
	N_STD::vector<UBYTE>& GetRatios() { return ratios; }
	N_STD::vector<FlashRGB>& GetColors() { return colors; }
   
	
private:

	N_STD::vector<UBYTE> ratios;
	N_STD::vector<FlashRGB> colors;
	
	friend N_STD::ostream &operator<< (N_STD::ostream &out, FlashGradientRecord &data);
	friend N_STD::istream &operator>> (N_STD::istream &in,  FlashGradientRecord &data);

};

class FlashFillStyleGradient : public FlashFillStyle
{
public:	
	FlashFillStyleGradient(FlashMatrix &m, FlashGradientRecord &g) : FlashFillStyle(0x10), matrix(m), gradient(g){	type = 0x10; }
	virtual ~FlashFillStyleGradient() {}
	void SetLinear() { type = 0x10; }
	void SetRadial() { type = 0x12; }
	FlashMatrix& GetMatrix() { return matrix; }
	FlashGradientRecord& GetGradientRecord() { return gradient; }

	DEFINE_FLASHCOPY_INTERFACE( FlashFillStyleGradient, FlashFillStyle )

	virtual void Write(N_STD::ostream &out);
	virtual void Read(N_STD::istream &in);

private:
	friend N_STD::istream &operator>> (N_STD::istream &in,  FlashFillStyleArray &data);
	FlashFillStyleGradient() {}

	FlashMatrix matrix;
	FlashGradientRecord gradient;	
};

class FlashFillStyleBitmap : public FlashFillStyle
{

public:
	FlashFillStyleBitmap(UWORD _bitmapID, FlashMatrix &m, bool _tiled=false) : bitmapID(_bitmapID), matrix(m), tiled(_tiled) { type = tiled ? 0x40 : 0x41;}
	virtual ~FlashFillStyleBitmap() {}
	virtual void Write(N_STD::ostream &out);
	virtual void Read(N_STD::istream &in);
   UWORD GetBitmapID() const { return bitmapID; }
   FlashMatrix& GetMatrix() { return matrix; }
   bool IsTiled() const { return tiled; }

	DEFINE_FLASHCOPY_INTERFACE( FlashFillStyleBitmap, FlashFillStyle )

private:
	friend N_STD::istream &operator>> (N_STD::istream &in,  FlashFillStyleArray &data);
	FlashFillStyleBitmap() {}
        
	UWORD bitmapID;
	FlashMatrix matrix;
	bool tiled;
};

class FlashFillStyleArray : public FlashVersionEnabled
{
public:
	FlashFillStyleArray() {}
	FlashFillStyleArray( const FlashFillStyleArray &that );
	virtual ~FlashFillStyleArray() {}
	size_t AddFillStyle(FlashFillStyle *s)
	{
		styles.push_back( s );
		//gc.push_back( s );
		return styles.size();
	}

	inline void   ReserveSpace( long i_nNbElemsToAdd ) { styles.reserve( styles.capacity() + i_nNbElemsToAdd ); }
	int           GetNBits();
	
	N_STD::vector<FlashFillStyle*> & GetStyles() { return styles; }
private:
	N_STD::vector<FlashFillStyle*> styles;
	gc_vector<FlashFillStyle*> gc;
	friend N_STD::ostream &operator<< (N_STD::ostream &out, FlashFillStyleArray &data);
	friend N_STD::istream &operator>> (N_STD::istream &in,  FlashFillStyleArray &data);
	
};

class FlashLineStyleArray;
class FlashLineStyle : public FlashVersionEnabled, public FlashCopyEnabled<FlashLineStyle>
{
public:
	FlashLineStyle(UWORD _width, FlashRGB &_color) : width(_width), color(_color) {}
	
	DEFINE_FLASHCOPY_INTERFACE( FlashLineStyle, FlashLineStyle )

	friend N_STD::ostream &operator<< (N_STD::ostream &out, FlashLineStyle &data);
	friend N_STD::istream &operator>> (N_STD::istream &in,  FlashLineStyle &data);

	// Get methods:
	UWORD GetWidth() { return width; }
	FlashRGB GetColor() { return color; }

private:
	friend N_STD::istream &operator>> (N_STD::istream &in,  FlashLineStyleArray &data);
	FlashLineStyle() {}

	UWORD width;
	FlashRGB color;	

};

class FlashLineStyleArray : public FlashVersionEnabled
{
public:
	FlashLineStyleArray() {}
	FlashLineStyleArray( const FlashLineStyleArray &that );
	virtual ~FlashLineStyleArray() {}

	size_t AddLineStyle(FlashLineStyle *s)
	{
		styles.push_back(s);
		//gc.push_back(s);
		return styles.size();
	}

	inline void   ReserveSpace( long i_nNbElemsToAdd ) { styles.reserve( styles.capacity() + i_nNbElemsToAdd ); }
	int GetNBits();

	N_STD::vector<FlashLineStyle*> & GetStyles() { return styles; }

private:
	N_STD::vector<FlashLineStyle*> styles;
	gc_vector<FlashLineStyle*> gc;
	friend N_STD::ostream &operator<< (N_STD::ostream &out, FlashLineStyleArray &data);
	friend N_STD::istream &operator>> (N_STD::istream &in,  FlashLineStyleArray &data);
};

class FlashShapeRecord : public FlashVersionEnabled
{
public:
	FlashShapeRecord() {};
	virtual ~FlashShapeRecord() {};
	virtual void Write(BitBuffer &out, FlashShapeCommon &c)=0;
	virtual void Read(BitStreamIn &in, FlashShapeCommon &data)=0;
};

class FlashShape;
class FlashShapeWithStyle;
class FlashShapeRecordStraight : public FlashShapeRecord
{
public:	
	FlashShapeRecordStraight(SWORD _dx, SWORD _dy) : dx(_dx), dy(_dy) {}
	virtual ~FlashShapeRecordStraight() {}
	virtual void Write(BitBuffer &out, FlashShapeCommon &data);
	virtual void Read(BitStreamIn &in, FlashShapeCommon &data);
	SWORD GetDX() const { return dx; }
	SWORD GetDY() const { return dy; }

private:
	FlashShapeRecordStraight() {}
	SWORD dx;
	SWORD dy;
	friend class FlashShape;
	friend N_STD::istream &operator>> (N_STD::istream &in,   FlashShape &data);
	friend N_STD::istream &operator>> (N_STD::istream &in,   FlashShapeWithStyle &data);

};

class FlashShapeRecordEnd : public FlashShapeRecord
{
public:
	FlashShapeRecordEnd() {}
	virtual ~FlashShapeRecordEnd() {}
	virtual void Write(BitBuffer &out, FlashShapeCommon &data);
	virtual void Read(BitStreamIn &in, FlashShapeCommon &data);
};

class FlashShapeRecordCurved : public FlashShapeRecord
{
public:	
	FlashShapeRecordCurved(SWORD _dxc, SWORD _dyc, SWORD _dxa, SWORD _dya) : 
	  dxa(_dxa), dya(_dya), dxc(_dxc), dyc(_dyc)  {}
	virtual ~FlashShapeRecordCurved() {};
	virtual void Write(BitBuffer &out, FlashShapeCommon &data);
	virtual void Read(BitStreamIn &in, FlashShapeCommon &data);
	SWORD GetDXA() const { return dxa; }
	SWORD GetDYA() const { return dya; }
	SWORD GetDXC() const { return dxc; }
	SWORD GetDYC() const { return dyc; }
private:
	FlashShapeRecordCurved() {}
	SWORD dxa;
	SWORD dya;
	SWORD dxc;
	SWORD dyc;
	friend class FlashShape;
	friend N_STD::istream &operator>> (N_STD::istream &in,   FlashShape &data);
	friend N_STD::istream &operator>> (N_STD::istream &in,   FlashShapeWithStyle &data);
};

class FlashShapeRecordChange : public FlashShapeRecord
{
public:
	FlashShapeRecordChange();
	FlashShapeRecordChange(SDWORD dx, SDWORD dy);
	virtual ~FlashShapeRecordChange() {}
	
	void NewFillStyles(FlashFillStyleArray &a, FlashLineStyleArray &c);

	void ChangeMoveDelta(SDWORD _dx, SDWORD _dy)
	{
		moveto=true;
		
		dx=_dx;
		dy=_dy;
	}
	void ChangeFillStyle0(UWORD style);
	void ChangeFillStyle1(UWORD style);
	void ChangeLineStyle(UWORD style);

	virtual void Write(BitBuffer &out, FlashShapeCommon &data);
	virtual void Read(BitStreamIn &in, FlashShapeCommon &data);

	bool GetMoveTo( SDWORD *x, SDWORD *y ) const 
	{ 
		if ( moveto) 
		{ 
			*x = dx; *y = dy; 
		}
		return moveto; 
	}
	bool		GetNewStyles() const { return newstyles; }
	FlashFillStyleArray &GetFillStyleArray() { return fillstyles; }
	FlashLineStyleArray &GetLineStyleArray() { return linestyles; }
	bool		GetLineStyle( UWORD *s ) const 
	{ 
		if ( linestyle ) 
			*s = styleline; 
		return linestyle; 
	}
	bool		GetFillStyle1( UWORD *s ) const 
	{ 
		if ( fillstyle1 ) 
			*s = stylefill1; 
		return fillstyle1; 
	}
	bool		GetFillStyle0( UWORD *s ) const
	{ 
		if ( fillstyle0 ) 
			*s = stylefill0; 
		return fillstyle0; 
	}

	void SetMoveTo(bool move)
	{
		moveto = move;
	}

private:
	bool newstyles;
	bool linestyle;
	bool fillstyle1;
	bool fillstyle0;
	bool moveto;

	UWORD stylefill0;
	UWORD stylefill1;
	UWORD styleline;
	SDWORD dx;
	SDWORD dy;

	FlashFillStyleArray fillstyles;
	FlashLineStyleArray linestyles;
	
	friend class FlashShape;
	friend N_STD::istream &operator>> (N_STD::istream &in,   FlashShape &data);
	friend N_STD::istream &operator>> (N_STD::istream &in,   FlashShapeWithStyle &data);
};

class FlashShape
{
public:
	FlashShape() {}
	~FlashShape() {}

	FlashRect GetBounds();
	void AddRecord(const FlashShapeRecordChange &r)
	{
		record_change.push_back(r);
		record_sequencer.push_back(flash_pair<int,long>(0,(long)record_change.size()-1));
	}
	void AddRecord(const FlashShapeRecordStraight &r)
	{
		record_straight.push_back(r);
		record_sequencer.push_back(flash_pair<int,long>(1,(long)record_straight.size()-1));
	}
	void AddRecord(const FlashShapeRecordCurved &r)
	{
		record_curved.push_back(r);
		record_sequencer.push_back(flash_pair<int,long>(2,(long)record_curved.size()-1));
	}

	N_STD::vector<FlashShapeRecordChange> &RecordChanges() { return record_change; }
	N_STD::vector<FlashShapeRecordStraight> &RecordStraights() { return record_straight; }
	N_STD::vector<FlashShapeRecordCurved> &RecordCurveds() { return record_curved; }
	N_STD::vector<flash_pair<int,long> > &RecordSequencer() { return record_sequencer; }

protected:
	N_STD::vector<FlashShapeRecordChange> record_change;
	N_STD::vector<FlashShapeRecordStraight> record_straight;
	N_STD::vector<FlashShapeRecordCurved> record_curved;

	N_STD::vector<flash_pair<int,long> > record_sequencer;

	friend N_STD::ostream &operator<< (N_STD::ostream &out,  FlashShape &data);
	friend N_STD::istream &operator>> (N_STD::istream &in,   FlashShape &data);

};

class FlashShapeWithStyle : public FlashVersionEnabled, public FlashShape
{
public:
	FlashShapeWithStyle(){}
	virtual ~FlashShapeWithStyle(){}
	
	void AddFillStyle(FlashFillStyle *s)
	{
		fillstyles.AddFillStyle(s);
	}
	void SetFillStyleArray(FlashFillStyleArray &_fillstyles)
	{
		fillstyles=_fillstyles;
	}
	void SetLineStyleArray(FlashLineStyleArray &_linestyles)
	{
		linestyles=_linestyles;
	}
	FlashFillStyleArray &GetFillStyleArray() { return fillstyles; }
	FlashLineStyleArray &GetLineStyleArray() { return linestyles; }
private: 
	FlashFillStyleArray fillstyles;
	FlashLineStyleArray linestyles;

	friend N_STD::ostream &operator<< (N_STD::ostream &out,  FlashShapeWithStyle &data);
	friend N_STD::istream &operator>> (N_STD::istream &in,   FlashShapeWithStyle &data);
};

class FlashTagDefineShapeBase : public FlashTag, public FlashIDEnabled
{
DEFINE_RW_INTERFACE
public:
	FlashTagDefineShapeBase(FlashShapeWithStyle &s) : shapes(s) { }
	FlashTagDefineShapeBase( bool count ) : FlashIDEnabled( count ) { }
	~FlashTagDefineShapeBase() {}
	
	FlashShapeWithStyle &GetShapes() { return shapes; }
	FlashRect rimport;

	FlashShapeWithStyle shapes;
private:
	friend N_STD::ostream &operator<< (N_STD::ostream &out, FlashTagDefineShapeBase &data);
	friend N_STD::istream &operator>> (N_STD::istream &in,  FlashTagDefineShapeBase &data);

	
	friend class FlashImporter;

protected:
	FlashTagDefineShapeBase() {}
	int version;
	int header;
};

class FlashTagDefineShape1 : public FlashTagDefineShapeBase
{
	DEFINE_RW_INTERFACE
public:
	FlashTagDefineShape1(FlashShapeWithStyle &s) : FlashTagDefineShapeBase(s) { header=2;  version=1; }

	FlashTagDefineShape1() : FlashTagDefineShapeBase( false ) { header=2;  version=1; }
};

class FlashTagDefineShape2 : public FlashTagDefineShapeBase
{
	DEFINE_RW_INTERFACE
public:
	FlashTagDefineShape2(FlashShapeWithStyle &s) : FlashTagDefineShapeBase(s) { header=22; version=2; }

	FlashTagDefineShape2() : FlashTagDefineShapeBase( false ) { header=22; version=2; }
};

class FlashTagDefineShape3 : public FlashTagDefineShapeBase
{
	DEFINE_RW_INTERFACE
public:	
	FlashTagDefineShape3(FlashShapeWithStyle &s) : FlashTagDefineShapeBase(s) { header=32; version=3; }

	FlashTagDefineShape3() : FlashTagDefineShapeBase( false ) { header=32; version=3; }

};
#endif
