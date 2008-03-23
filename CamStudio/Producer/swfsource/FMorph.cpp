#include "FBase.h"
#include "FMorph.h"
#include <strstream>
#include <iostream>

void FlashMorphGradientRecord::Write(N_STD::ostream &out)
{
	c1.SetAlphaWriteMode(true);
	c2.SetAlphaWriteMode(true);
	out.put(r1);
	out << c1;
	out.put(r2);
	out << c2;
}

void FlashMorphGradient::Write(N_STD::ostream &out)
{
	out.put((unsigned char)records.size());
	for(N_STD::vector<FlashMorphGradientRecord>::iterator i=records.begin();
		i != records.end(); i++)
		{
			(*i).Write(out);
		}
}

void FlashMorphFillStyleSolid::Write(N_STD::ostream &out)
{
	out.put(0x00);
	c1.SetAlphaWriteMode(true);
	c2.SetAlphaWriteMode(true);
	out << c1;
	out << c2;
}

void FlashMorphFillStyleLinear::Write(N_STD::ostream &out)
{
	out.put(0x10);
	out << fm1;
	out << fm2;
	fmg.Write(out);
}

void FlashMorphFillStyleRadial::Write(N_STD::ostream &out)
{
	out.put(0x12);
	out << fm1;
	out << fm2;
	fmg.Write(out);
}

void FlashMorphFillStyles::Write(N_STD::ostream &out)
{
	if(fillStyles.size() >= 0xff)
	{
		out.put((char)0xff);
		WRITE_UWORD((UWORD)fillStyles.size());
	}
	else 
	{
		out.put((unsigned char)fillStyles.size());
	}
	for(N_STD::vector<FlashMorphFillStyle *>::iterator i=fillStyles.begin();
		i != fillStyles.end(); i++)
		{
			(*i)->Write(out);
		}
}

void FlashMorphLineStyle::Write(N_STD::ostream &out)
{
	WRITE_UWORD(w1);
	WRITE_UWORD(w2);
	c1.SetAlphaWriteMode(true);
	c2.SetAlphaWriteMode(true);
	out << c1;
	out << c2;
}

void FlashMorphLineStyles::Write(N_STD::ostream &out)
{
	if(lineStyles.size() >= 0xff)
	{
		out.put((char)0xff);
		WRITE_UWORD((UWORD)lineStyles.size());
	}
	else 
	{
		out.put((unsigned char)lineStyles.size());
	}
	for(N_STD::vector<FlashMorphLineStyle>::iterator i=lineStyles.begin();
		i != lineStyles.end(); i++)
		{
			(*i).Write(out);
		}
}
void FlashMorphFillStyleBitmap::Write(N_STD::ostream &out)
{
	out.put((char)(tiled ? 0x40  : 0x41));
	WRITE_UWORD(bitmapID);
	out << mstart;
	out << mend;
}

N_STD::ostream &operator<< (N_STD::ostream &out, FlashTagDefineMorphShape &data)
{
	N_STD::ostrstream tmp;
	N_STD::ostrstream tmp2;
	WRITE_UWORD2(data.GetID(),tmp);
	tmp << data.s1.GetBounds();
	tmp << data.s2.GetBounds();

	data.fs1.Write(tmp2);
	data.fs2.Write(tmp2);
	tmp2 << data.s1;
	WRITE_UDWORD2(tmp2.pcount(),tmp);
	
	tmp.write(tmp2.rdbuf()->str(), tmp2.pcount());	
	tmp << data.s2;
	
	out << FlashTagHeader(46, tmp.pcount());
	out.write(tmp.rdbuf()->str(), tmp.pcount());
	return out;
}

N_STD::istream &operator>> (N_STD::istream &in,  FlashTagDefineMorphShape &data)
{
#if 0
// TODO - to be completed...
	UWORD id;
	READ_UWORD(id);
	data.SetID(id);
#endif

	return in;
}
	
