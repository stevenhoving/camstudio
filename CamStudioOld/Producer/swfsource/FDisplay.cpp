#include "FBase.h"
#include "FDisplay.h"
#include <iostream>
#include <strstream>


FlashTagPlaceObject::FlashTagPlaceObject(UWORD _depth, UWORD _charID, FlashMatrix &m)
: characterID(_charID), depth(_depth), matrix(m), hascfx(false)
{
}

FlashTagPlaceObject::FlashTagPlaceObject(UWORD _depth, UWORD _charID, FlashMatrix &m, FlashColorTransform &c)
: characterID(_charID), depth(_depth), matrix(m), hascfx(true)
{
  cfx = c;
}

N_STD::ostream& operator<<(N_STD::ostream& out, FlashTagPlaceObject &data)
{
	N_STD::ostrstream st;

	WRITE_UWORD2(data.characterID,st);
	WRITE_UWORD2(data.depth,st);

	st << data.matrix;
	if(data.hascfx) { st << data.cfx; }
	
	out << FlashTagHeader(4, st.pcount());
	out.write(st.rdbuf()->str(), st.pcount());
	return out;
}
N_STD::istream& operator>>(N_STD::istream& in, FlashTagPlaceObject &data)
{
	N_STD::streampos start = in.tellg();
	READ_UWORD(data.characterID);
	READ_UWORD(data.depth);
	in >> data.matrix;
	
	if(data.importsize + start != (UDWORD)in.tellg())
	{
		data.hascfx = true;
		in >> data.cfx;
	}
	return in;
}

N_STD::ostream& operator<<(N_STD::ostream& out, FlashTagRemoveObject &data)
{
	out << FlashTagHeader(5,4);
	WRITE_UWORD(data.charID);
	WRITE_UWORD(data.depth);
	return out;
}

N_STD::istream& operator>>(N_STD::istream& in, FlashTagRemoveObject &data)
{
	READ_UWORD(data.charID);
	READ_UWORD(data.depth);
	return in;
}
N_STD::ostream& operator<<(N_STD::ostream& out, FlashTagPlaceObject2 &data)
{
	N_STD::strstream o;
	BitBuffer b;
	b.Write(0,1); // CLIP ACTIONS
	b.Write(data.hasClipDepth,1); 
	b.Write(data.hasName,1);
	b.Write(data.hasRatio,1);
	b.Write(data.hasColorTransform,1);
	b.Write(data.hasMatrix,1);
	b.Write(data.hasCharID,1);
	b.Write(data.hasMove,1);
	if(data.hasColorTransform) data.cfx.SetTagVersion(2);
	o << b;
	WRITE_UWORD2(data.depth,o);
	if(data.hasCharID)   WRITE_UWORD2(data.charID,o);
	if(data.hasMatrix) o << data.matrix;
	if(data.hasColorTransform) o << data.cfx;
	if(data.hasRatio)    WRITE_UWORD2(data.ratio,o);
	if(data.hasName) { o.write(data.name.c_str(),data.name.size()); o << (char)0; }
	if(data.hasClipDepth) WRITE_UWORD2(data.clipDepth,o);

	out << FlashTagHeader(26,o.rdbuf()->pcount());
	out.write((char*)o.rdbuf()->str(),o.rdbuf()->pcount());
	
	return out;
}
N_STD::istream& operator>>(N_STD::istream& in, FlashTagPlaceObject2 &data)
{
	BitStreamIn b(&in);
	unsigned char tmp;
	b.Read(tmp,1); // CLIP ACTIONS
	unsigned char hasclipdepth;
	b.Read(hasclipdepth,1); 
	unsigned char hasname;
	b.Read(hasname,1);
	unsigned char hasratio;
	b.Read(hasratio,1);
	unsigned char hascfx;
	b.Read(hascfx,1);
	unsigned char hasmatrix;
	b.Read(hasmatrix,1);
	unsigned char hasid;
	b.Read(hasid,1);
	unsigned char move;
	b.Read(move,1);

	data.hasName = (hasname == 1);
	data.hasCharID = (hasid == 1);
	data.hasMove = (move == 1);
	data.hasMatrix = (hasmatrix == 1);
	data.hasColorTransform = (hascfx == 1);
	data.hasRatio = (hasratio == 1);
	data.hasClipDepth = (hasclipdepth == 1);
	
	b.Align();

	READ_UWORD(data.depth);
	if(hasid)
	{
		READ_UWORD(data.charID);
	}
	if(hasmatrix)
	{
		in >> data.matrix;
	}
	if(hascfx)
	{
		data.cfx.SetTagVersion(2);
		in >> data.cfx;
	}
	if(hasratio)
	{
		READ_UWORD(data.ratio);
	}
	else
		data.ratio = 0;

	if(hasname) 
	{ 
		unsigned int i;
		while((i = in.get()) != 0)
		{
			data.name+=(char)i;
		}				
	}
	if(hasclipdepth)
	{
		READ_UWORD(data.clipDepth);
	}

	return in;
}


N_STD::ostream& operator<<(N_STD::ostream& out, FlashTagRemoveObject2 &data)
{
	out << FlashTagHeader(28,2);
	WRITE_UWORD(data.depth);
	return out;
}
N_STD::istream& operator>>(N_STD::istream& in, FlashTagRemoveObject2 &data)
{
	READ_UWORD(data.depth);
	return in;
}
DEFINE_SIMPLE_TAG(FlashTagShowFrame,0x01)
