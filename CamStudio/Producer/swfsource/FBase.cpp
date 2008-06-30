#include "FBase.h"
#include <math.h>
#include <iostream>
#include <sstream>

FlashFixed::FlashFixed(void) : upperval(0), lowerval(0) 
{
}

FlashFixed::FlashFixed(SWORD u, UWORD l) : upperval(u), lowerval(l) 
{
}

FlashFixed::FlashFixed(double f) : upperval((UWORD)floor(f)), lowerval(UWORD((f-floor(f))*0x10000)) 
{
}

N_STD::ostream &operator<< (N_STD::ostream &out, const FlashFixed &data)
{
	WRITE_UWORD(data.upperval)
	WRITE_UWORD(data.lowerval)
    return out;
}

N_STD::istream &operator>> (N_STD::istream &in,  FlashFixed &data)
{
    READ_UWORD(data.upperval)
    READ_UWORD(data.lowerval)
    return in;
}

FlashFixed16::FlashFixed16(void) : upperval(0), lowerval(0) 
{
}

FlashFixed16::FlashFixed16(SBYTE u, UBYTE l) : upperval(u), lowerval(l) 
{
}

FlashFixed16::FlashFixed16(double f) : upperval((UBYTE)floor(f)), lowerval(UBYTE((f-floor(f))*0x100))
{
}

N_STD::ostream &operator<< (N_STD::ostream &out, const FlashFixed16 &data)
{
    WRITE_UBYTE(data.upperval)
	WRITE_UBYTE(data.lowerval)
	return out;
}

N_STD::istream &operator>> (N_STD::istream &in,  FlashFixed16 &data)
{
    READ_UBYTE(data.upperval)
	READ_UBYTE(data.lowerval)
	return in;
}



N_STD::ostream &operator<< (N_STD::ostream &out, const FlashMatrix &data)
{
    BitBuffer b;
    b.Write(data.scale,1);
    if(data.scale)
    {
        int bits = fbase_max(GetBitSizeSigned(data.scalex.ConvertToRaw()),
                       GetBitSizeSigned(data.scaley.ConvertToRaw()));
        b.Write(bits,5);    
        b.Write(PackBitsSigned(data.scalex.ConvertToRaw()),bits);
        b.Write(PackBitsSigned(data.scaley.ConvertToRaw()),bits);
        
    }
    b.Write(data.rotate,1);
    if(data.rotate)
    {
        int bits = fbase_max(GetBitSizeSigned(data.rotatex.ConvertToRaw()),
                       GetBitSizeSigned(data.rotatey.ConvertToRaw()));
        b.Write(bits,5);
        b.Write(PackBitsSigned(data.rotatex.ConvertToRaw()),bits);
        b.Write(PackBitsSigned(data.rotatey.ConvertToRaw()),bits);
        
    }
    
    int bits = fbase_max(GetBitSizeSigned(data.translatex),
                   GetBitSizeSigned(data.translatey));
    b.Write(bits,5);
    b.Write(PackBitsSigned(data.translatex),bits);
    b.Write(PackBitsSigned(data.translatey),bits);

    out << b;
    return out;
}

N_STD::istream &operator>> (N_STD::istream &in,  FlashMatrix &data)
{
    
    BitStreamIn i(&in);
    unsigned char ct;
    i.Read(ct,1);
    data.scale = (ct==1);
    unsigned int bits;
    if(data.scale)
    {
        i.Read(bits,5); 
        SDWORD x;
        SDWORD y;
        i.Read(x,bits);
        data.scalex.GetFromRaw(UnPackBitsSigned(x,bits));
        i.Read(y,bits);
        data.scaley.GetFromRaw(UnPackBitsSigned(y,bits));
        
    }
    i.Read(ct,1);
    data.rotate = (ct==1);
    if(data.rotate)
    {
        i.Read(bits,5); 
        SDWORD x;
        SDWORD y;
        i.Read(x,bits);
        data.rotatex.GetFromRaw(UnPackBitsSigned(x,bits));
        i.Read(y,bits);
        data.rotatey.GetFromRaw(UnPackBitsSigned(y,bits));

    
    }
    
    i.Read(bits,5);
    
    SDWORD transx;
    SDWORD transy;

    if(bits > 0)
	{
		i.Read(transx,bits);
		i.Read(transy,bits);

		data.translatex = UnPackBitsSigned(transx,bits);
		data.translatey = UnPackBitsSigned(transy,bits);
	} 
	else
	{
		data.translatex = 0;
		data.translatey = 0;

	}
    return in;
}

N_STD::ostream& operator<<(N_STD::ostream& out, FlashColorTransform &data)
{
    data.addFlashRGB.SetAlphaWriteMode(data.GetTagVersion() > 1);
    data.multFlashRGB.SetAlphaWriteMode(data.GetTagVersion() > 1);

    BitBuffer b;
    b.Write(data.add,1);
    b.Write(data.mult,1);

    int num=fbase_max(data.multFlashRGB.GetNumBits(),data.addFlashRGB.GetNumBits());
    b.Write(num,4);
    
    if(data.mult)
    {
        data.multFlashRGB.Write(b,num);
    }
    if(data.add)
    {
        data.addFlashRGB.Write(b,num);
    }
    
    out << b;
    return out;
}
N_STD::istream& operator>>(N_STD::istream& in, FlashColorTransform &data)
{
    data.addFlashRGB.SetAlphaWriteMode(data.GetTagVersion() > 1);
    data.multFlashRGB.SetAlphaWriteMode(data.GetTagVersion() > 1);

    BitStreamIn b(&in);
    unsigned char ct;
    b.Read(ct,1);
    data.add = (ct != 0);
    b.Read(ct,1);
    data.mult = (ct != 0);
    
    int num;
    b.Read(num,4);
    
    if(data.mult)
    {
        data.multFlashRGB.SetAlphaWriteMode(true);
        data.multFlashRGB.Read(b,num);
    }
    if(data.add)
    {
        data.addFlashRGB.SetAlphaWriteMode(true);
        data.addFlashRGB.Read(b,num);
    }
    return in;
}

void FlashRGB::Write(BitBuffer &out, int num)
{
    if(num != -1)
    {       
        out.Write(PackBitsSigned(r),num);
        out.Write(PackBitsSigned(g),num);
        out.Write(PackBitsSigned(b),num);
        if(alpha) out.Write(PackBitsSigned(a),num);
    }
    else
    {
        out.Write(r,8);
        out.Write(g,8);
        out.Write(b,8);
        if(alpha) out.Write(a,8);
    }
}

void FlashRGB::Read(BitStreamIn &in, int num)
{
    if(num != -1)
    {       
        in.Read(r,num);
        in.Read(g,num);
        in.Read(b,num);
        if(alpha) in.Read(a,num);

        r=UnPackBitsSigned(r,num);
        g=UnPackBitsSigned(g,num);
        b=UnPackBitsSigned(b,num);
        if(alpha) a=UnPackBitsSigned(a,num);
    }
    else
    {
        in.Read(r,8);
        in.Read(g,8);
        in.Read(b,8);
        if(alpha) in.Read(a,8);
    }

}
N_STD::ostream &operator<< (N_STD::ostream &out, const FlashRGB &data)
{
    out.put((data.r < 0xff) ? ((char)data.r) : (char)0xff);
    out.put((data.g < 0xff) ? ((char)data.g) : (char)0xff);
    out.put((data.b < 0xff) ? ((char)data.b) : (char)0xff);
    if(data.alpha) out.put((data.a < 0xff) ? ((char)data.a) : (char)0xff);
    return out;
}
N_STD::istream &operator>> (N_STD::istream &in,  FlashRGB &data)
{
    data.r = in.get();
    data.g = in.get();
    data.b = in.get();
    if(data.alpha) data.a = in.get();
    return in;
}


BitStreamOut &operator<< (BitStreamOut &out, BitBuffer &data)
{
    int displacement=0;
    for(N_STD::vector<char>::iterator i = data.v.begin(); i != data.v.end(); i++)
    {
        displacement+=8;
        if(data.displace-displacement > 8)
        {
            out.Write((char)*i,8);
        }
        else out.Write((char)*i,data.displace-displacement);
    }
    return out;
}

N_STD::ostream &operator<< (N_STD::ostream &out, BitBuffer &data)
{
    for(N_STD::vector<char>::iterator i = data.v.begin(); i != data.v.end(); i++)
    {
        out.put((char)*i);
    }
    return out;
}

N_STD::istream &operator>> (N_STD::istream &in,  BitBuffer &data)
{
    return in;
}

N_STD::ostream &operator<< (N_STD::ostream &out, const FlashRect &data)
{
    int minbits = fbase_max(fbase_max(GetBitSize(data.x1), GetBitSize(data.y1)), 
                    fbase_max(GetBitSize(data.x2), GetBitSize(data.y2)))+1;

    BitBuffer b;
    b.Write(minbits,5);
    b.Write(data.x1,minbits);
    b.Write(data.x2,minbits);
    b.Write(data.y1,minbits);
    b.Write(data.y2,minbits);
    out << b;
    return out;
}

FlashHeader::FlashHeader(UBYTE _version, UDWORD _filesize, int width, int height, UWORD _framerate, UWORD _framecount)
{
    movieCompressed = false;
    version=_version;
    filesize=_filesize;
    size=FlashRect(0,0,width, height);
    frameRate=FlashFixed16(_framerate >> 8, _framerate & 0xff);
    frameCount=_framecount;
}

FlashHeader::FlashHeader(UBYTE _version, UDWORD _filesize, int width, int height, double _framerate, UWORD _framecount)
{
    movieCompressed = false;
    version=_version;
    filesize=_filesize;
    size=FlashRect(0,0,width, height);
    frameRate=FlashFixed16(_framerate);
    frameCount=_framecount;
}

N_STD::istream &operator>> (N_STD::istream &in,  FlashRect &data)
{
    // TODO: DOES NOT FUNCTION
    char c = in.get();
    char bit_size = (c >> 3) & 0x1f;
    in.putback(c);
    BitStreamIn b(&in,5);
    SWORD tmp;
    
    b.Read(tmp, bit_size);
    data.x1=UnPackBitsSigned(tmp,bit_size);
    b.Read(tmp, bit_size);
    data.x2=UnPackBitsSigned(tmp,bit_size);
    b.Read(tmp, bit_size);
    data.y1=UnPackBitsSigned(tmp,bit_size);
    b.Read(tmp, bit_size);
    data.y2=UnPackBitsSigned(tmp,bit_size);
       
    return in;
}

N_STD::ostream &operator << (N_STD::ostream &out, const FlashHeader &data)
{
    int minbits = (int)ceil(((fbase_max(GetBitSize(data.size.GetX2()), GetBitSize(data.size.GetY2())+1)) * 4 + 5) / 8.0);
		
	UDWORD size_adjust = 12+minbits;
#if 0
// TODO
   if(movieCompressed && data.version > 5)
	   out << "CWS";
   else
	   out << "FWS";
#else
	 out << "FWS";
#endif
    out.put(data.version);
    WRITE_UDWORD(data.filesize+size_adjust)
    out << data.size;

	WRITE_UWORD(data.frameRate.ConvertToRaw())    
    WRITE_UWORD(data.frameCount)
    
    return out;
}
N_STD::istream &operator >> (N_STD::istream &in,  FlashHeader &data)
{
    char tmp[3];
	SWORD tmp1;
    in.read(tmp,3); // TODO: SHOULD BE "FWS" or "CWS", add error handling
    data.version = in.get();
    if( data.version > 5 && tmp[0] == 'C' )
       data.movieCompressed = true;
    else
       data.movieCompressed = false;
    READ_UDWORD(data.filesize)
    in >> data.size;
    READ_UWORD(tmp1)
	data.frameRate.GetFromRaw(tmp1);
    READ_UWORD(data.frameCount)
    return in;
}

N_STD::ostream &operator << (N_STD::ostream &out, const FlashTagHeader &data)
{
    if (data.length < 63) 
    {
        UWORD write = ((data.tagID << 6) | data.length);
        out.put(write & 0xff);
        out.put(write >> 8);
    }
    else
    {
        UWORD write = ((data.tagID << 6) | 0x3f);
        out.put(write & 0xff);
        out.put(write >> 8);
        WRITE_UDWORD(data.length);
    }
    return out;
}
N_STD::istream &operator >> (N_STD::istream &in,  FlashTagHeader &data)
{
    UWORD c  = in.get(); 
    UWORD c2 = in.get(); 

    UWORD read = (c2 << 8) | c;
    
    data.tagID = read >> 6;
    
    if((read & 0x3f) != 0x3f)
    {
        data.length = read & 0x3f;
    }
    else
    {
        READ_UDWORD(data.length);
    }

    
    return in;
    

}

N_STD::ostream &operator << (N_STD::ostream &out, const FlashTag &data) { return out;}
N_STD::istream &operator >> (N_STD::istream &in,  FlashTag &data) { return in;}

UWORD FlashIDFactory::IDCharacter=1;


FlashMatrix CreateMatrix(FlashRect bounds,float scaleX,float scaleY,float rotation,float translateX,float translateY,bool bScale, bool bRotate)
{	
	const double DegToRad = 3.14159265358979323 / 180.0;	// pi/2

	int centerX = int( (bounds.GetX1()+bounds.GetX2())/2.0 );
	int centerY = int( (bounds.GetY1()+bounds.GetY2())/2.0 );
	float sX = scaleX ;
	float sY =  scaleY ;
	float rot = rotation;

// 	float skewX = 1.0;
// 	float skewY = 1.0;

	// We want to rotate the matrix about its own center, not the origin. So it is necessary
	// to move the object to the origin, rotate it, and move it back.
/*
	float deltaX = centerX * cos( DegToRad * rot ) - centerY * sin(DegToRad*rot) - centerX; 
	float deltaY = centerX * sin( DegToRad * rot ) + centerY * cos(DegToRad*rot) - centerY;
*/
	// This is the code from David Michie to make a swf matrix (no skew)

	//   Assign(cos(rx)*sx,  sin(rx)*sx,  0.0,
	//          cos(ry)*sy,  sin(ry)*sy,  0.0,
	//          x,           y,           1.0);

	// lee: using the Folef and Van Damme gives a slightly different
	// matrix (a matrix rotated from Flash's, actually). So converting the
	// above given that cos(x+90) = -sin(x) & sin(x+90) = cos x,
	// gives the matrix below.

	// make the matrix:
	//	| a b tx|
	//	| c d ty|
	//

	double a =  cos( DegToRad*rot ) * sX; //ScaleX
	double b =  sin( DegToRad*rot ) * sX; //RotateSkew0
	double c = -sin( DegToRad*rot ) * sY; //RotateSkew1
	double d =  cos( DegToRad*rot ) * sY; //ScaleY

	// By Yiyi
	double deltaX = centerX * cos( DegToRad * rot ) - centerY * sin(DegToRad*rot); 
	double deltaY = centerX * sin( DegToRad * rot ) + centerY * cos(DegToRad*rot);
	
	return FlashMatrix(bScale, FlashFixed( a ), FlashFixed( d ),
						bRotate,FlashFixed( b ), FlashFixed( c ),


//						translateX - int( deltaX * sX ), 
//						translateY - int( deltaY * sY ) 

						// By Yiyi
						(int)(translateX + centerX - int( deltaX * sX )), 
						(int)(translateY + centerY - int( deltaY * sY )) 
					  );
}

N_STD::ostream &operator << (N_STD::ostream &out, const FlashTagRawData &data)
{
	out.write(data.ptrData, data.lenData);
	return out;
}
N_STD::istream &operator >> (N_STD::istream &in, FlashTagRawData &data)
{
	throw N_STD_EXCEP::exception();
	return in;	
}