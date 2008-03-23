#ifndef FBASE_H_FILE
#define FBASE_H_FILE

//
// Platform-dependent namespaces:
//
#ifdef _WIN32
//#define N_STD        _STL
#define N_STD        std
#else
#define N_STD        std
#endif

#ifdef __MWERKS__
#define N_STD_EXCEP  std
#else
#define N_STD_EXCEP
#endif

#ifdef __MWERKS__
#define N_STD_HMAP   Metrowerks
#else
#define N_STD_HMAP   std
#endif

#pragma warning( disable : 4786 )

#include <iostream>
#include <vector>
#include <math.h>

#include <stdlib.h>
#include <string.h>

/*static unsigned long arr1[] = {
		0x0, 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f, 0xff,
        0x1ff, 0x3ff, 0x7ff, 0xfff, 0x1fff, 0x3fff, 0x7fff, 0xffff,
        0x1ffff, 0x3ffff, 0x7ffff, 0xfffff, 0x1fffff, 0x3fffff, 0x7fffff, 0xffffff,
        0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff
};*/

class FlashImporter;

typedef unsigned char UBYTE;
typedef char SBYTE;

typedef unsigned short UWORD;
typedef short SWORD;

typedef unsigned int UDWORD;
typedef int SDWORD;

// Platform-dependent byte order:
#ifdef __MACOS__
#define BIG_ENDIAN
#else
#undef BIG_ENDIAN
#endif

#ifndef BIG_ENDIAN
// Little endian:
#define WRITE_UBYTE(x) { UBYTE s = (x); out.write((char*)&s,1); }
#define WRITE_SBYTE(x) { SBYTE s = (x); out.write((char*)&s,1); }

#define WRITE_UWORD(x) { UWORD s = (x); out.write((char*)&s,2); }
#define WRITE_SWORD(x) { SWORD s = (x); out.write((char*)&s,2); }

#define WRITE_UDWORD(x) { UDWORD s = (x); out.write((char*)&s,4); }
#define WRITE_SDWORD(x) { SDWORD s = (x); out.write((char*)&s,4); }

#define WRITE_UBYTE2(x,f) { UBYTE s = (x); f.write((char*)&s,1); }
#define WRITE_SBYTE2(x,f) { SBYTE s = (x); f.write((char*)&s,1); }

#define WRITE_UWORD2(x,f) { UWORD s = (x); f.write((char*)&s,2); }
#define WRITE_SWORD2(x,f) { SWORD s = (x); f.write((char*)&s,2); }

#define WRITE_UDWORD2(x,f) { UDWORD s = (x); f.write((char*)&s,4); }
#define WRITE_SDWORD2(x,f) { SDWORD s = (x); f.write((char*)&s,4); }

#define READ_UBYTE(x) { in.read((char*)&x,1); }
#define READ_SBYTE(x) { in.read((char*)&x,1); }

#define READ_UWORD(x) { in.read((char*)&x,2); }
#define READ_SWORD(x) { in.read((char*)&x,2); }

#define READ_UDWORD(x) { in.read((char*)&x,4); }
#define READ_SDWORD(x) { in.read((char*)&x,4); }

#define READ_UBYTE2(x,f) { f.read((char*)&x,1); }
#define READ_SBYTE2(x,f) { f.read((char*)&x,1); }

#define READ_UWORD2(x,f) { f.read((char*)&x,2); }
#define READ_SWORD2(x,f) { f.read((char*)&x,2); }

#define READ_UDWORD2(x,f) { f.read((char*)&x,4); }
#define READ_SDWORD2(x,f) { f.read((char*)&x,4); }

#else
// Big endian needs byte swapping:
#define WRITE_UBYTE(x) { UBYTE s = (x); out.write((char*)&s,1); }
#define WRITE_SBYTE(x) { SBYTE s = (x); out.write((char*)&s,1); }

#define WRITE_UWORD(x) { UWORD s = (x); char* pc = (char*)&s; out.write(pc+1,1); out.write(pc,1); }
#define WRITE_SWORD(x) { SWORD s = (x); char* pc = (char*)&s; out.write(pc+1,1); out.write(pc,1); }

#define WRITE_UDWORD(x) { UDWORD s = (x); char* pc = (char*)&s; out.write(pc+3,1); out.write(pc+2,1); out.write(pc+1,1); out.write(pc,1); }
#define WRITE_SDWORD(x) { SDWORD s = (x); char* pc = (char*)&s; out.write(pc+3,1); out.write(pc+2,1); out.write(pc+1,1); out.write(pc,1); }

#define WRITE_UBYTE2(x,f) { UBYTE s = (x); f.write((char*)&s,1); }
#define WRITE_SBYTE2(x,f) { SBYTE s = (x); f.write((char*)&s,1); }

#define WRITE_UWORD2(x,f) { UWORD s = (x); char* pc = (char*)&s; f.write(pc+1,1); f.write(pc,1); }
#define WRITE_SWORD2(x,f) { SWORD s = (x); char* pc = (char*)&s; f.write(pc+1,1); f.write(pc,1); }

#define WRITE_UDWORD2(x,f) { UDWORD s = (x); char* pc = (char*)&s; f.write(pc+3,1); f.write(pc+2,1); f.write(pc+1,1); f.write(pc,1); }
#define WRITE_SDWORD2(x,f) { SDWORD s = (x); char* pc = (char*)&s; f.write(pc+3,1); f.write(pc+2,1); f.write(pc+1,1); f.write(pc,1); }

#define READ_UBYTE(x) { in.read((char*)&x,1); }
#define READ_SBYTE(x) { in.read((char*)&x,1); }

#define READ_UWORD(x) { char* pc = (char*)&x; in.read(pc+1,1); in.read(pc,1); }
#define READ_SWORD(x) { char* pc = (char*)&x; in.read(pc+1,1); in.read(pc,1); }

#define READ_UDWORD(x) { char* pc = (char*)&x; in.read(pc+3,1); in.read(pc+2,1); in.read(pc+1,1); in.read(pc,1); }
#define READ_SDWORD(x) { char* pc = (char*)&x; in.read(pc+3,1); in.read(pc+2,1); in.read(pc+1,1); in.read(pc,1); }

#define READ_UBYTE2(x,f) { f.read((char*)&x,1); }
#define READ_SBYTE2(x,f) { f.read((char*)&x,1); }

#define READ_UWORD2(x,f) { char* pc = (char*)&x; f.read(pc+1,1); f.read(pc,1); }
#define READ_SWORD2(x,f) { char* pc = (char*)&x; f.read(pc+1,1); f.read(pc,1); }

#define READ_UDWORD2(x,f) { char* pc = (char*)&x; f.read(pc+3,1); f.read(pc+2,1); f.read(pc+1,1); f.read(pc,1); }
#define READ_SDWORD2(x,f) { char* pc = (char*)&x; f.read(pc+3,1); f.read(pc+2,1); f.read(pc+1,1); f.read(pc,1); }
#endif

#define DEFINE_RW_INTERFACE								\
public:													\
virtual void Write(N_STD::ostream &out) { out << *this; } \
virtual void Read(N_STD::istream &in) { in >> *this; }

template <class T> T fbase_max(T a, T b)  {return ((a > b) ? a : b); } 
template <class T> T fbase_min(T a, T b)  {return ((a < b) ? a : b); } 

template<class T, class U>
    struct flash_pair {
    typedef T first_type;
    typedef U second_type;
    T first;
    U second;
    flash_pair() {}
    flash_pair(const T& x, const U& y) : first(x), second(y) {}
    template<class V, class W>
        flash_pair(const flash_pair<V, W>& pr) : first(pr.first), second(pr.second) {}
    };

class FlashVersionEnabled
{
public:
    FlashVersionEnabled() : version(1) {}
    virtual void SetTagVersion(int v) { version = v; }
    virtual int GetTagVersion(void) const { return version; }
private:
    int version;
};

template<class T> T LSHR(T x, int num);

template<class T> int GetBitSize(T x)
{
    int size=0;
    while(x > 0)
    {
      x=LSHR(x,1);
      size++;
    } 
    return size;
}

template<class T> int GetBitSizeSigned(T x)
{
    int size=0;
    int sign = (long(x) < 0);
    T tmp=x;
    if(sign) tmp = tmp*(-1);
    size = GetBitSize(tmp);
    return (size+1);
}

template<class T> T PackBitsSigned(T x)
{    
    UDWORD v=0x0;
    int sign = (long(x) < 0);
    if(sign) x=~x + 1;

    int i = GetBitSizeSigned(x);
//    v = arr1[i];
    for(int b=0; b < i; b++)
    {
        v = (v << 1) | 0x01;
    }
    v = (x & v) | (GetBit(x,sizeof(T)*8-1));
    if(sign) return (T)(~v + 1);
    return (v); 
}

template<class T> T UnPackBitsSigned(T x, int size)
{
    int sign = GetBit(x,size-1);
    
    x = GetIsolatedBits(x,0,size);
    
    T v = 0;
    if(sign == 1) 
    {
        int bit1 = size;
        int bit2 = sizeof(T)*8;
//		v = arr1[bit2-bit1];
        for(int b=0; b < bit2-bit1; b++)
		  {
            v = (v << 1) | 0x01;
        }
//		v <<= bit1;
        for(int b2=0; b2 < bit1; b2++)
        {
            v = (v << 1);
        }

    }
    return (x | v); 
}

template<class T> char GetBit(T x, int bit)
{   
    T y = 1 << bit;
    if((x & y)==0) return (char)0;
    return((char)1);
}

template<class T> T IsolateBits(T x, int bit1, int bit2)
{
    T v=0;
    //v = arr1[bit2-bit1];
    for(int b=0; b < bit2-bit1; b++)
    {
        v = (v << 1) | 0x01;
    }
	//v <<= bit1;
    for(int b2=0; b2 < bit1; b2++)
    {
        v = (v << 1);
    }   
    return (x & v);
}
template<class T> T LSHR(T x, int num)
{
    char sign = (GetBit(x,sizeof(T)*8-1));
    T v = IsolateBits(x, 0, sizeof(T)*8-1);
    v >>= num;
    if(sign==1) 
    {
        v |= (((T)0x1) << (sizeof(T)*8-1));
    }
    return v;
}

template<class T> T GetIsolatedBits(T x, int bit1, int bit2)
{
    T r = IsolateBits(x,bit1,bit2);
    return ((T)(LSHR(r, bit1)));
}



class BitStreamIn
{
public:
    BitStreamIn(N_STD::istream *i, int off=0) :
      in(i), offset(off) {}
    ~BitStreamIn()
    {
        Align();
    }
    //TODO Add Exception handling for nbits > bitsizeof()
    template <class T> void Read(T &r, int nbits)
    {
        if (nbits == 0) return;
		N_STD::vector<int> tmp;

        int count   = 0;
        int read     = 0-offset;
        int to_go    = nbits;
        int last;
        do
        {       
            last = in->get();
            tmp.push_back(last);
            read += 8;
            count++;
        } while(read < to_go);
        
        
        UDWORD bits=0;

        int bitsleft = (count*8)-(offset+nbits);
        int bitsright = (count*8)-offset;
        
        int count2 = 0;
        for(N_STD::vector<int>::iterator i = tmp.begin(); i != tmp.end(); i++)
        {

            count2++;
            int shift = 8*(count-(count2));

            int b1 = fbase_max(bitsleft-shift,0);
            int b2 = fbase_min(bitsright-shift,8);
            
            int c = GetIsolatedBits(*i, b1, b2);
            
            bits = bits << (b2-b1);
            bits |= (T)(c & 0xff);
            
        }
        offset = (offset+nbits)%8;
        
        if(offset != 0)
        {       
            in->putback(last);      
        }
        r = (T)bits;
    }
    
    void Align() 
	{ 
		if(offset != 0) { 
			in->get(); 
			offset = 0;
		}
	}
	
	N_STD::istream &GetStream() { return *in; }
private:
    N_STD::istream *in;
    int offset;
};

class BitStreamOut
{

public:
    

    BitStreamOut(N_STD::ostream *o) :
      out (o), displace(0), curchar(0), remaining(false){ }
    ~BitStreamOut() 
    { 
        if(remaining) (*out).put(curchar); 
    }
    
    N_STD::ostream *out;
public:
    template <class T>
    void Write(T data, int bitsize)
    {

        for(int i = 0; i < bitsize; i++)
        {           
            remaining = true;
            int offset = 7-(displace%8);

            unsigned char val = GetBit(data, (bitsize)-1-i);        
            unsigned char set = 0x01 << (offset);

            curchar = (curchar & ~set) | (val << (offset));
            displace++;         
            
            offset = 7-(displace%8);
            if(offset%8==7)
            {
                (*out).put((char)curchar);
                curchar=0;
                remaining=false;
                displace=0;
            }

        }

    }
    void Align()
    {
        int offset = 7-(displace%8);
        displace+=offset;
        (*out).put((char)curchar);
        curchar=0;      
        remaining=false;
        displace=0;
    }
    
private:
    int displace;
    char curchar;
    bool remaining; 
};

template<class T> BitStreamOut &operator<< (BitStreamOut &o, const T t)
{
    o.Write(t,sizeof(T)*8);
}

class BitBuffer
{
public:
    BitBuffer() : displace(0) {}
    ~BitBuffer() {}

    void WriteBytes(char *c, int numbytes)
    {
        for(int i=0; i < numbytes; i++)
        {
            Write(c[i],8);
        }
    }
    void Write(UDWORD data, int bitsize)
    {
        for(int i = 0; i < bitsize; i++)
        {
            int pos    = displace/8;
            int offset = 7-(displace%8);

            if(offset==7)
            {
                v.push_back(0);
            }
            unsigned char val = GetBit(data, (bitsize)-1-i);        
            unsigned char set = 0x01 << (offset);

			int size = (int)v.size();

            v[pos] = (v[pos] & ~set) | (val << (offset));
            displace++;         
        }
    }
    void Align()
    {
		int	bitsleft = displace % 8;
		if ( bitsleft != 0 )
		{
	        int offset = 8-(bitsleft);

		    displace += offset;
		}
    }

private:
    long displace;
    N_STD::vector<char> v;
    
    friend BitStreamOut &operator<< (BitStreamOut &out, BitBuffer &data);
    friend N_STD::ostream &operator<< (N_STD::ostream &out, BitBuffer &data);
    friend N_STD::istream &operator>> (N_STD::istream &in,  BitBuffer &data);
};

class FlashFixed
{
//private:
public:
    SWORD upperval;
    UWORD lowerval;
public: 
    FlashFixed(void); 
    FlashFixed(SWORD u, UWORD l); 
    FlashFixed(double f); 

    UDWORD ConvertToRaw () const
    {
        UDWORD r = ((SDWORD)upperval)<<16;
        r |= lowerval;
        return r;
    }
    void GetFromRaw(SDWORD raw)
    {
        upperval = raw >> 16;
        lowerval = (raw & 0xffff);
    }
    double ConvertToDouble() const
    {
        return upperval + double(lowerval) / 0x10000;
    }
    void GetFromDouble(double x)
    {
        upperval = (UWORD)floor(x);
        lowerval = (UWORD)((x-floor(x))*0x10000);
    }
    
    friend N_STD::ostream &operator<< (N_STD::ostream &out, const FlashFixed &data);
    friend N_STD::istream &operator>> (N_STD::istream &in,  FlashFixed &data);

};

class FlashFixed16
{
//private
public:
    SBYTE upperval;
    UBYTE lowerval;
public: 
    FlashFixed16(void); 
    FlashFixed16(SBYTE u, UBYTE l); 
    FlashFixed16(double f); 
    
    UWORD ConvertToRaw() const
    {
        UWORD r = ((SWORD)upperval)<<8;
        r |= lowerval;
        return r;
    }
    void GetFromRaw(SWORD raw)
    {
        upperval = raw >> 8;
        lowerval = (raw & 0xff);
    }
    double ConvertToDouble()
    {
        return upperval + double(lowerval) / 0x100;
    }
    void GetFromDouble(double x)
    {
        upperval = (UBYTE)floor(x);
        lowerval = (UBYTE)((x-floor(x))*0x100);
    }
    
    friend N_STD::ostream &operator<< (N_STD::ostream &out, const FlashFixed16 &data);
    friend N_STD::istream &operator>> (N_STD::istream &in,  FlashFixed16 &data);    
};

class FlashMatrix
{
public:
    FlashMatrix() : scale(false), scalex(0), scaley(0),
                  rotate(false), rotatex(0), rotatey(0),
                  translatex(0), translatey(0)
    {
    }
    FlashMatrix(bool _scale, FlashFixed _scalex, FlashFixed _scaley,
                bool _rotate, FlashFixed _rotatex, FlashFixed _rotatey,
                SWORD _translatex, SWORD _translatey)
                : scale(_scale), scalex(_scalex), scaley(_scaley),
                  rotate(_rotate), rotatex(_rotatex), rotatey(_rotatey),
                  translatex(_translatex), translatey(_translatey)
    {
    }
    
    void SetScale(FlashFixed _scalex, FlashFixed _scaley) { scalex=_scalex; scaley=_scaley; scale=true;}
    void SetRotate(FlashFixed _rotatex, FlashFixed _rotatey) { rotatex=_rotatex; rotatey=_rotatey; rotate = true;}
    void SetTranslate(SWORD _translatex, SWORD _translatey) { translatex=_translatex; translatey=_translatey;}

    bool HasScale() const { return scale; }
    bool HasRotate() const { return rotate; }

    FlashFixed GetRotateX() const { return rotatex; }
    FlashFixed GetRotateY() const { return rotatey; }

    FlashFixed GetScaleX() const { return scalex; }
    FlashFixed GetScaleY() const { return scaley; }

	// EH: changed return types
    SWORD GetTranslateX() const { return translatex; }
    SWORD GetTranslateY() const { return translatey; }

private:
    bool scale;
    FlashFixed scalex;
    FlashFixed scaley;
    bool rotate;
    FlashFixed rotatex;
    FlashFixed rotatey;
    SWORD translatex;
    SWORD translatey;

    friend N_STD::ostream &operator<< (N_STD::ostream &out, const FlashMatrix &data);
    friend N_STD::istream &operator>> (N_STD::istream &in,  FlashMatrix &data);
};

class FlashMatrixScale : public FlashMatrix
{
    FlashMatrixScale(FlashFixed _scalex, FlashFixed _scaley) : 
        FlashMatrix(true, _scalex, _scaley, false, 0, 0, 0, 0)
    {
        
    }       
};

class FlashMatrixRotate : public FlashMatrix
{
    FlashMatrixRotate(FlashFixed _rotatex, FlashFixed _rotatey) : 
        FlashMatrix(false, 0, 0, true, _rotatex, _rotatey, 0, 0)
    {
        
    }       
};

class FlashMatrixTranslate : public FlashMatrix
{
    FlashMatrixTranslate(SWORD x, SWORD y) : 
        FlashMatrix(false, 0, 0, false, 0, 0, x, y)
    {
        
    }       
};

class FlashRGB
{
public: 
    FlashRGB() : r(0), g(0), b(0), a(0xff), alpha(false) { }
    FlashRGB(SWORD _r, SWORD _g, SWORD _b) : r(_r), g(_g), b(_b), a(0xff), alpha(false){ }
    FlashRGB(SWORD _r, SWORD _g, SWORD _b, SWORD _a) : r(_r), g(_g), b(_b), a(_a), alpha(true){ }

    void Write(BitBuffer &out, int num=-1);
    
    void Read(BitStreamIn &in, int num=-1);

    bool GetAlphaWriteMode(void)   { return (alpha); }
    void SetAlphaWriteMode(bool a) { alpha=a; }

    int GetNumBits() 
    { 
        if(alpha) return fbase_max(fbase_max(GetBitSizeSigned(a),GetBitSizeSigned(b)),fbase_max(GetBitSizeSigned(r),GetBitSizeSigned(g)));
        return fbase_max(GetBitSizeSigned(b),fbase_max(GetBitSizeSigned(r),GetBitSizeSigned(g)));
    }
    
    SWORD GetR() { return r; }
    SWORD GetG() { return g; }
    SWORD GetB() { return b; }
    SWORD GetA() { return a; }
    void SetR(SWORD _r) {r = _r; }
    void SetG(SWORD _g) {g = _g; }
    void SetB(SWORD _b) {b = _b; }
    void SetA(SWORD _a) {a = _a; }
    void SetRGBA(SWORD _r, SWORD _g, SWORD _b, SWORD _a)
    {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }
    void SetRGB(SWORD _r, SWORD _g, SWORD _b)
    {
        r = _r;
        g = _g;
        b = _b;
    }

private:    
    SWORD r;
    SWORD g;
    SWORD b;
    SWORD a;
    bool alpha;
    friend N_STD::ostream &operator<< (N_STD::ostream &out, const FlashRGB &data);
    friend N_STD::istream &operator>> (N_STD::istream &in,  FlashRGB &data);
};

class FlashColorTransform : public FlashVersionEnabled
{
public:
	FlashColorTransform() : add(false), mult(false){}
	
	FlashColorTransform(bool _add, FlashRGB& _addFlashRGB, bool _mult, FlashRGB& _multFlashRGB)
		: add(_add), mult(_mult), addFlashRGB(_addFlashRGB), multFlashRGB(_multFlashRGB){}
	    
    friend N_STD::ostream& operator<<(N_STD::ostream& out, FlashColorTransform &data);
    friend N_STD::istream& operator>>(N_STD::istream& in, FlashColorTransform &data);

    bool HasAdd() const { return add; }
    bool HasMult() const { return mult; }
    
    FlashRGB GetAddRGB() const { return addFlashRGB; }
    FlashRGB GetMultRGB() const { return multFlashRGB; }

	void SetAddRGB(FlashRGB _addFlashRGB) 
	{
		addFlashRGB = _addFlashRGB;
		add = true;
	}
	void SetMultRGB(FlashRGB _multFlashRGB) 
	{
		multFlashRGB = _multFlashRGB;
		mult = true;
	}

	void SetAdd(short _addFlashR, short _addFlashG, short _addFlashB) 
	{
		addFlashRGB = FlashRGB(_addFlashR, _addFlashG, _addFlashB);
		add = true;
	}
	void SetMult(FlashFixed16 _multFlashR, FlashFixed16 _multFlashG, FlashFixed16 _multFlashB) 
	{
		multFlashRGB = FlashRGB(_multFlashR.ConvertToRaw(), _multFlashG.ConvertToRaw(), _multFlashB.ConvertToRaw());
		mult = true;
	}

private:
    bool add;
    bool mult;
    FlashRGB addFlashRGB;
    FlashRGB multFlashRGB;
};

class FlashRect
{
public:
    FlashRect(int _x1, int _y1, int _x2, int _y2) : 
      x1(_x1), y1(_y1), x2(_x2), y2(_y2), defined(true) {}
    
    FlashRect() : x1(0), y1(0), x2(0), y2(0), defined(false) {}
    
    ~FlashRect() {}

    bool IsDefined() { return(defined); }
    
    void SetRect(int _x1, int _y1, int _x2, int _y2)
    {
      x1 = _x1;
      y1 = _y1;
      x2 = _x2;
      y2 = _y2;
    }
    
    SWORD GetX1() const { return x1; }
    SWORD GetX2() const { return x2; }
    SWORD GetY1() const { return y1; }
    SWORD GetY2() const { return y2; }

    void BoundWith(const FlashRect &r) { x1 = fbase_min(x1,r.x1); x2 = fbase_max(x2, r.x2); y1 = fbase_min(y1,r.y1); y2 = fbase_max(y2, r.y2); };
private:
    int x1; 
    int y1;
    int x2;
    int y2;
    bool defined;

    friend N_STD::ostream &operator<< (N_STD::ostream &out, const FlashRect &data);
    friend N_STD::istream &operator>> (N_STD::istream &in,  FlashRect &data);
};


class FlashHeader
{
public:
    FlashHeader(UBYTE _version, UDWORD _filesize, int width, int height, UWORD _framerate, UWORD _framecount);
    FlashHeader(UBYTE _version, UDWORD _filesize, int width, int height, double _framerate, UWORD _framecount);
    FlashHeader() : movieCompressed( false ) {}
    ~FlashHeader() {}

    UBYTE  GetVersion(void) { return version; }
    UDWORD GetFilesize(void) { return filesize; }
    FlashRect GetScreenSize(void) { return size; }
    FlashFixed16 GetFrameRate(void) { return frameRate; }
    UWORD GetFrameCount(void) { return frameCount; }
    bool IsMovieCompressed() { return movieCompressed; }

private:
    
    friend N_STD::ostream &operator << (N_STD::ostream &out, const FlashHeader &data);
    friend N_STD::istream &operator >> (N_STD::istream &in,  FlashHeader &data);

    UBYTE  version;
    UDWORD filesize;
    FlashRect size;
    FlashFixed16 frameRate;
    UWORD frameCount;
    bool  movieCompressed;     // Flash MX Compress Movie flag - TODO
};

class FlashTagHeader
{
DEFINE_RW_INTERFACE
public:
    FlashTagHeader() {}
    FlashTagHeader(UWORD _tagID, UDWORD _length) : tagID(_tagID), length(_length) {}
    virtual ~FlashTagHeader() {}

    UWORD GetTagID(void) { return tagID; }
    UDWORD GetTagLength(void) { return length; }
    void SetTagID(UWORD _tagID) { tagID = _tagID; }
    void SetTagLength(UDWORD _length) { length = _length; }

private:
    UWORD  tagID;
    UDWORD length;

    friend N_STD::ostream &operator << (N_STD::ostream &out, const FlashTagHeader &data);
    friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagHeader &data);

};


class FlashTag : public FlashTagHeader
{
DEFINE_RW_INTERFACE
public:
    FlashTag() {}
    virtual ~FlashTag() {}
	virtual bool isFrame() { return false; }
	virtual bool isSpriteEnabled() { return false; }

	void SetTagHeader( UWORD _tagID, UDWORD _length ) { SetTagID(_tagID); SetTagLength(_length); /*TODO: to be removed*/ importsize = _length; }
protected:
    UDWORD importsize;     // TODO : to be removed and replaced by FlashTagHeader::[Get|Set]TagLength
private:        
	friend N_STD::ostream &operator << (N_STD::ostream &out, const FlashTag &data);
    friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTag &data);
};

class FlashSpriteEnabled : public FlashTag
{
public:
    FlashSpriteEnabled() {}
    virtual ~FlashSpriteEnabled() {}	

	virtual bool isSpriteEnabled() { return true; }
};

class FlashIDFactory
{
public: 
    FlashIDFactory() {};

    UWORD GetCharacterID() 
    { 
        UWORD _IDCharacter = IDCharacter; 
        IDCharacter++; 
        return (_IDCharacter); 
    }
    static void ResetCount()
    {
        IDCharacter=1;
    }
private:    
    static UWORD IDCharacter;
};

class FlashIDEnabled
{
public:
    FlashIDEnabled(bool count=true) { charID = idFactory.GetCharacterID(); }
    
    void SetID(UWORD i) { charID=i; }
    UWORD GetID(void) const { return (charID); }
private:
    FlashIDFactory idFactory;
    UWORD charID;
};

class FlashTagRawData : public FlashTag
{
DEFINE_RW_INTERFACE
public:
	char *ptrData;
	int  lenData;

	friend N_STD::ostream &operator << (N_STD::ostream &out, const FlashTagRawData &data);
    friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagRawData &data);
};

#define DEFINE_SIMPLE_TAG(x, n) \
N_STD::ostream &operator << (N_STD::ostream &out, const x &data) \
{ \
    return (out << FlashTagHeader(n,0)); \
} \
N_STD::istream &operator >> (N_STD::istream &in,  x &data) \
{ \
    return in; \
}

#define DECLARE_SIMPLE_TAG(x) \
class x : public FlashTag \
{                               \
	DEFINE_RW_INTERFACE			\
public:                         \
    x() {} \
    ~x() {} \
    friend N_STD::ostream &operator << (N_STD::ostream &out, const x &data); \
    friend N_STD::istream &operator >> (N_STD::istream &in,  x &data); \
};

#define DECLARE_SIMPLE_TAG2(x) \
class x : public FlashSpriteEnabled \
{                               \
	DEFINE_RW_INTERFACE			\
public:                         \
    x() {} \
    ~x() {} \
    friend N_STD::ostream &operator << (N_STD::ostream &out, const x &data); \
    friend N_STD::istream &operator >> (N_STD::istream &in,  x &data); \
};

#define DECLARE_SIMPLE_TAG3(x) \
class x : public FlashSpriteEnabled \
{                               \
	DEFINE_RW_INTERFACE			\
public:                         \
    x() {} \
    ~x() {} \
	virtual bool isFrame() { return true; }\
    friend N_STD::ostream &operator << (N_STD::ostream &out, const x &data); \
    friend N_STD::istream &operator >> (N_STD::istream &in,  x &data); \
};

template<class T> class gc_vector: public N_STD::vector<T>
{
public:	
	gc_vector() {}
	gc_vector(const gc_vector &c) {}
	~gc_vector() { for(N_STD::vector<T>::iterator i = begin(); i != end(); i++) { delete *i; }}
};

FlashMatrix CreateMatrix(FlashRect bounds,float scaleX,float scaleY,float rotation,float translateX,float translateY,bool bScale, bool bRotate);

template <class T> class FlashCopyEnabled
{
public:
	FlashCopyEnabled() { }
	virtual T * Copy() const = 0;

};

#define DEFINE_FLASHCOPY_INTERFACE(T, P)	\
	virtual P * Copy() const { return new T( *this ); }

#endif
