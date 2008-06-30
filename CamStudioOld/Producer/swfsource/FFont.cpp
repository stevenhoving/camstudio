#include "FFont.h"
#include <sstream>
#include <strstream>


N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineFontInfo &data)
{
	out << FlashTagHeader(13, (UDWORD)strlen(data.str)+2+2+data.codes.size()*((data.flags & 1) ? 2:1));
	WRITE_UWORD(data.FontID);
	out.put((unsigned char)strlen(data.str));
	out << data.str;
	out.put((unsigned char)data.flags);
	if((data.flags & 1) == 1)
	{
		for(N_STD::vector<UWORD>::iterator i = data.codes.begin(); i != data.codes.end(); i++)
		{
			WRITE_UWORD(*i);		
		}
	}
	else
	{
		for(N_STD::vector<UWORD>::iterator i = data.codes.begin(); i != data.codes.end(); i++)
		{
			out.put((char)*i);
		}
	}
	return out;
}

N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineFontInfo &data)
{
	READ_UWORD(data.FontID);
	
	int len = in.get();
	//if(i == EOF) throw;
	
	data.str = (char *)malloc(len+1);
	data.strings.push_back(data.str);
	data.str[len] = 0;
	
	unsigned char * tmp = (unsigned char *)data.str;
	
	for(int i=0; (i < len); i++)
	{
		int c = in.get();
		//if(c == EOF) throw;
		tmp[i] = c;
	}
	int flags = in.get();
	//if(flags == EOF) throw;
	
	data.flags = flags;
	
	UWORD size = data.importsize - (len+2+2);
	
	if(data.flags & 1)
	{
		for(UWORD i = 0; i < size; i+=2)
		{
			UWORD tmp;
			READ_UWORD(tmp);
			data.codes.push_back(tmp);		
		}
	}
	else
	{
		for(UWORD i = 0; i < size; i++)
		{
			int c = in.get();
			//if(c == EOF) throw;
			data.codes.push_back(c);
		}
		
	}
	return in;
}


int FlashTagDefineFont::AddShape(FlashShape &shape)
{
	shapes.push_back(shape);
	return shapes.size() - 1;
}

int FlashTagDefineFont::AddShape(FlashShape &shape, int glyphId)
{
	shapes.push_back(shape);
	int index = shapes.size() - 1;
	glyphs[glyphId] = index;
	return index;
	}

int FlashTagDefineFont::GetGlyphId(int glyph)
{	
	if(glyphs.find(glyph) == glyphs.end())
	{
		return -1;
	}
	return glyphs[glyph];
}

N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineFont &data)
{
	
	N_STD::ostrstream tmp;
		
	N_STD::vector<UWORD> offsets;

	for(N_STD::vector<FlashShape>::iterator i = data.shapes.begin(); i < data.shapes.end(); i++)
	{
		offsets.push_back(tmp.pcount());
		tmp << *i;
	}
	
	out << FlashTagHeader(10, (UDWORD)2+offsets.size()*2+tmp.pcount());
	WRITE_UWORD(data.GetID());
	
	for(N_STD::vector<UWORD>::iterator i2 = offsets.begin(); i2 < offsets.end(); i2++)
	{
		WRITE_UWORD((UWORD)(*i2+offsets.size()*2));
	}
	
	out.write(tmp.rdbuf()->str(), tmp.pcount());

	return out;
}

N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineFont &data)
{
	UWORD FontId;
	READ_UWORD(FontId);
	data.SetID(FontId);
	
	UWORD Offset1;
	READ_UWORD(Offset1);
	UWORD NumGlyphs = Offset1/2;

	N_STD::vector<UWORD> offsets;
	offsets.push_back(Offset1);

	for(int i = 0; i < NumGlyphs-1; i++)
	{
		UWORD Offset;
		READ_UWORD(Offset);
		offsets.push_back(Offset);
	}

	for(int i2 = 0; i2 < NumGlyphs; i2++)
	{
		FlashShape s;
		in >> s;		
		data.shapes.push_back(s);
	}
	
	return in;
}

void FlashTextRecordStyle::Write(N_STD::ostream &out, unsigned char bitsGlyph, unsigned char bitsAdvance)
{
	out.put((char)((char)(1 << 7)  | (char)(mhasFont << 3) | (char)(mhasFlashRGB << 2) | (char)(mhasOffsetx << 1) | (char)(mhasOffsety)));
	if(mhasFont)
	{
		WRITE_UWORD(mFontID);
	}
	if(mhasFlashRGB)
	{		
		mcolor.SetAlphaWriteMode(GetTagVersion() > 1);
		out << mcolor;
	}
	if(mhasOffsetx)
	{
		WRITE_SWORD(moffsetx);
	}
	if(mhasOffsety)
	{
		WRITE_SWORD(moffsety);
	}
	if(mhasFont)
	{
		WRITE_UWORD(mFontHeight);
	}
}

void FlashTextRecordStyle::Read(N_STD::istream &in, unsigned char bitsGlyph, unsigned char bitsAdvance)
{
	int c = in.get();

	mhasFont		= (bool)(GetIsolatedBits(c,3,4) == 1);
	mhasFlashRGB	= (bool)(GetIsolatedBits(c,2,3) == 1);
	mhasOffsetx		= (bool)(GetIsolatedBits(c,1,2) == 1);
	mhasOffsety		= (bool)(GetIsolatedBits(c,0,1) == 1);
	
	if(mhasFont)
	{
		READ_UWORD(mFontID);
	}
	if(mhasFlashRGB)
	{		
		mcolor.SetAlphaWriteMode(GetTagVersion() > 1);
		in >> mcolor;
	}
	if(mhasOffsetx)
	{
		READ_SWORD(moffsetx);
	}
	if(mhasOffsety)
	{
		READ_SWORD(moffsety);
	}
	if(mhasFont)
	{
		READ_UWORD(mFontHeight);
	}
}

bool FlashTextRecordStyle::isGlyph(void)
{
	return false;
}

void FlashTextRecordGlyph::Write(N_STD::ostream &out, unsigned char bitsGlyph, unsigned char bitsAdvance)
{
	out.put((char)v.size());
	BitStreamOut b(&out);
	for(N_STD::vector<FlashGlyphEntry>::iterator i=v.begin(); i != v.end(); i++)
	{
		b.Write((*i).first, bitsGlyph);
		b.Write(PackBitsSigned((*i).second), bitsAdvance);
	}
	b.Align();
}

void FlashTextRecordGlyph::Read(N_STD::istream &in, unsigned char bitsGlyph, unsigned char bitsAdvance)
{
	int c = in.get();
	BitStreamIn b(&in);
	
	for(int i = 0; i < c; i++)
	{
		FlashGlyphEntry tmp;
		b.Read(tmp.first, bitsGlyph);
		b.Read(tmp.second, bitsAdvance);
		v.push_back(tmp);
	}

	b.Align();

}
int FlashTextRecordGlyph::returnGlyphBits(void)
{
	int ret=0;
	for(N_STD::vector<FlashGlyphEntry>::iterator i=v.begin(); i != v.end(); i++)
	{
		if(GetBitSize((*i).first) > ret) ret = GetBitSize((*i).first);
	}
	return ret;
}

int FlashTextRecordGlyph::returnAdvBits(void)
{
		int ret=0;
	for(N_STD::vector<FlashGlyphEntry>::iterator i=v.begin(); i != v.end(); i++)
	{
		if(GetBitSizeSigned((*i).second) > ret) ret = GetBitSizeSigned((*i).second);
	}
	return ret;
}


void FlashTagDefineText::AddTextRecord(FlashTextRecord *r)
{
	records.push_back(r);
}

N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineText &data)
{
	N_STD::ostrstream tmp;
	int gbits=0;
	int abits=0;
	for(N_STD::vector<FlashTextRecord *>::iterator i = data.records.begin(); i != data.records.end(); i++)
	{
		if((*i)->isGlyph())
		{
			if((*i)->returnGlyphBits() > gbits) { gbits=(*i)->returnGlyphBits(); }
			if((*i)->returnAdvBits()   > abits) { abits=(*i)->returnAdvBits(); }
		}
	}
	

	WRITE_UWORD2(data.GetID(), tmp);
	tmp << data.rect;
	tmp << data.matrix;
	tmp << (char)gbits;
	tmp << (char)abits;
	
	for(N_STD::vector<FlashTextRecord *>::iterator i2 = data.records.begin(); i2 != data.records.end(); i2++)
	{
		(*i2)->SetTagVersion(1);
		(*i2)->Write(tmp,gbits,abits);
	}
	
	tmp << (char)0;

	out << FlashTagHeader(11, tmp.pcount());
	out.write(tmp.rdbuf()->str(), tmp.pcount());

	return out;
}

N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineText &data)
{
	UWORD id;	
	READ_UWORD(id);
	data.SetID(id);

	in >> data.rect;
	in >> data.matrix;
	int gbits = in.get();
	int abits = in.get();
	
	
	for(;;)
	{
		int c = in.get();
		if(in.eof())
		{
			break;
			//throw;
		}

		if(c == 0)
		{
			break;
		}

		if(GetIsolatedBits(c,7,8) == 1)
		{
			in.putback(c);
			FlashTextRecordStyle *s = new FlashTextRecordStyle();
			s->Read(in, gbits, abits);
			data.AddTextRecord(s);
			data.gc_records.push_back(s);
			
		}
		else
		{
			in.putback(c);
			FlashTextRecordGlyph *g = new FlashTextRecordGlyph();
			g->Read(in, gbits, abits);
			data.AddTextRecord(g);
			data.gc_records.push_back(g);
		}
	}
	return in;
}

void FlashTagDefineText2::AddTextRecord(FlashTextRecord *r)
{
	records.push_back(r);
}

N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineText2 &data)
{
	N_STD::ostrstream tmp;
	int gbits=0;
	int abits=0;
	for(N_STD::vector<FlashTextRecord *>::iterator i = data.records.begin(); i != data.records.end(); i++)
	{
		if((*i)->isGlyph())
		{
			if((*i)->returnGlyphBits() > gbits) { gbits=(*i)->returnGlyphBits(); }
			if((*i)->returnAdvBits()   > abits) { abits=(*i)->returnAdvBits(); }
		}
	}
	

	WRITE_UWORD2(data.GetID(), tmp);
	tmp << data.rect;
	tmp << data.matrix;
	tmp << (char)gbits;
	tmp << (char)abits;
	
	for(N_STD::vector<FlashTextRecord *>::iterator i2 = data.records.begin(); i2 != data.records.end(); i2++)
	{
		(*i2)->SetTagVersion(2);
		(*i2)->Write(tmp,gbits,abits);
	}
	
	tmp << (char)0;

	out << FlashTagHeader(33, tmp.pcount());
	out.write(tmp.rdbuf()->str(), tmp.pcount());

	return out;
}

N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineText2 &data)
{
	UWORD id;	
	READ_UWORD(id);
	data.SetID(id);

	in >> data.rect;	
	in >> data.matrix;
	int gbits = in.get();
	int abits = in.get();
	
	
	for(;;)
	{
		int c = in.get();
		if(in.eof())
		{
			break;
			//throw;
		}

		if(c == 0)
		{
			break;
		}

		if(GetIsolatedBits(c,7,8) == 1)
		{
			in.putback(c);
			FlashTextRecordStyle *s = new FlashTextRecordStyle();
			s->SetTagVersion(2);
			s->Read(in, gbits, abits);
			data.AddTextRecord(s);
			data.gc_records.push_back(s);
			
		}
		else
		{
			in.putback(c);
			FlashTextRecordGlyph *g = new FlashTextRecordGlyph();
			g->Read(in, gbits, abits);
			data.AddTextRecord(g);
			data.gc_records.push_back(g);
		}
	}
	return in;	
}

N_STD::ostream &operator << (N_STD::ostream &out, FlashKerningRecord &data)
{
	if(data.GetTagVersion() > 1)
	{
		WRITE_UWORD(data.code1);
		WRITE_UWORD(data.code2);
		WRITE_SWORD(data.adjustment);
	}
	else
	{
		out.put((unsigned char)data.code1);
		out.put((unsigned char)data.code2);
		WRITE_SWORD(data.adjustment);
	}
	return out;
}
N_STD::istream &operator >> (N_STD::istream &in,  FlashKerningRecord &data)
{
	if(data.GetTagVersion() > 1)
	{
		READ_UWORD(data.code1);
		READ_UWORD(data.code2);
		READ_SWORD(data.adjustment);
	}
	else
	{
		data.code1 = in.get();
		data.code2 = in.get();
		READ_SWORD(data.adjustment);
	}	
	return in;
}


// TODO: Error checking nGlyph?!
void FlashFontLayout::Write(N_STD::ostream &out)
{
	WRITE_SWORD(fontAscent);
	WRITE_SWORD(fontDescent);
	WRITE_SWORD(fontLeading);
	for(N_STD::vector<SWORD>::iterator i=fontAdvanceTable.begin(); i != fontAdvanceTable.end(); i++)
	{
		WRITE_SWORD(*i);
	}
	for(N_STD::vector<FlashRect>::iterator i2=fontBoundsTable.begin(); i2 != fontBoundsTable.end(); i2++)
	{
		out << *i2;
	}
	WRITE_UWORD((UWORD)fontKerningTable.size());
	for(N_STD::vector<FlashKerningRecord>::iterator i3=fontKerningTable.begin(); i3 != fontKerningTable.end(); i3++)
	{
		out << *i3;
	}
}

void FlashFontLayout::Read(N_STD::istream &in, UWORD NumGlyphs)
{
	READ_SWORD(fontAscent);
	READ_SWORD(fontDescent);
	READ_SWORD(fontLeading);
	
	for(int i = 0; i < NumGlyphs; i++)
	{
		SWORD tmp;
		READ_SWORD(tmp);
		fontAdvanceTable.push_back(tmp);
	}
	for(int i2 = 0; i2 < NumGlyphs; i2++)
	{
		FlashRect tmp;
		in >> tmp;
		fontBoundsTable.push_back(tmp);
	}
	UWORD size;
	READ_UWORD(size);
	
	for(int i3 = 0; i3 < NumGlyphs; i3++)
	{
		FlashKerningRecord r;
		in >> r;
		fontKerningTable.push_back(r);
	}
}

N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineFont2 &data)
{
	
	N_STD::ostringstream tmp(N_STD::ios_base::binary);
	N_STD::ostringstream tmp3(N_STD::ios_base::binary);
	
	
	tmp.put((unsigned char)strlen(data.fontname));
	tmp << data.fontname;
	WRITE_UWORD2((UWORD)data.shapes.size(),tmp);

	N_STD::ostringstream tmp2(N_STD::ios_base::binary);
	
	N_STD::vector<UDWORD> offsets;

	for(N_STD::vector<FlashShape>::iterator i = data.shapes.begin(); i != data.shapes.end(); i++)
	{
		offsets.push_back(tmp3.str().size());
		tmp3 << *i;
	}	
	
	bool longoffsets;
	
	if(offsets.size() > 0) longoffsets = offsets[offsets.size()-1] > (64*1024);
	else longoffsets = false;

	for(N_STD::vector<UDWORD>::iterator i2 = offsets.begin(); i2 != offsets.end(); i2++)
	{
		if(longoffsets)
		{
			WRITE_UDWORD2((UDWORD)*i2+offsets.size()*4+2,tmp2);
		}
		else
		{
			WRITE_UWORD2((UWORD)(*i2+offsets.size()*2+2),tmp2);
		}		
	}

				
	tmp.write(tmp2.str().c_str(), tmp2.str().size());
	WRITE_UWORD2((UWORD)(offsets.size()*2+tmp3.str().size()+2),tmp);
	
	tmp.write(tmp3.str().c_str(), tmp3.str().size());	
	
	
	
	bool longcodes = data.shapes.size() > (64*1024);
	if(longcodes)
	{
		for(N_STD::vector<UWORD>::iterator i = data.codes.begin(); i != data.codes.end(); i++)
		{			
			WRITE_UWORD2(*i,tmp);		
		}
	}
	else
	{
		for(N_STD::vector<UWORD>::iterator i = data.codes.begin(); i != data.codes.end(); i++)
		{
			tmp.put((unsigned char)*i);
		}
	}

	if(data.layout)
	{
		data.layout_data.Write(tmp);
	}
	
	out << FlashTagHeader(48,tmp.str().size()+4);
	
	WRITE_UWORD(data.GetID());
	
	out.put((char)((data.layout << 7) | (longoffsets << 3) | (longcodes << 2) | (data.flags)));
	out.put(0);
	out.write(tmp.str().c_str(), tmp.str().size());
	
	return out;
}

N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineFont2 &data)
{
	return in;
}


N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineEditBox &data)
{
	N_STD::ostrstream tmp;
	WRITE_UWORD2(data.GetID(), tmp);
	tmp << data.mr;
	tmp.put((unsigned char)(data.mflags >> 8));
	tmp.put((unsigned char)(data.mflags & 0xff));
	
	
	if(data.mflags & FTDEB_HASFONT)
	{
		WRITE_UWORD2(data.mfontID,tmp); 
		WRITE_UWORD2(data.mfontHeight,tmp);
	}
	if(data.mflags & FTDEB_HASTEXTCOLOR)
	{
		data.mcolor.SetAlphaWriteMode(true);
		tmp << data.mcolor;
	}
	if(data.mflags & FTDEB_HASMAXLENGTH)
	{
		WRITE_UWORD2(data.mmaxLength, tmp); 
	}
	if(data.mflags & FTDEB_HASLAYOUT)
	{
		tmp.put(data.malign);
		WRITE_UWORD2(data.mleftmargin, tmp); 
		WRITE_UWORD2(data.mrightmargin, tmp); 
		WRITE_UWORD2(data.mindent, tmp); 
		WRITE_UWORD2(data.mleading, tmp);	
	}
	tmp << data.mvariable; 
	tmp.put((char)0);
	if(data.mflags & FTDEB_HASTEXT)
	{
		tmp << data.minitialtext;
		tmp.put((char)0);
	}
	out << FlashTagHeader(37, tmp.pcount());
	out.write(tmp.rdbuf()->str(),tmp.pcount());
	return out;
}
N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineEditBox &data)
{
	UWORD id;
	READ_UWORD(id);
	data.SetID(id);
	
	in >> data.mr;
	
	int c = in.get();
	//if c == EOF throw
	int c2 = in.get();
	//if c2 == EOF throw

	data.mflags = ((unsigned char)c);
	data.mflags = (data.mflags << 8) | (c2 & 0xff);

	
	if(data.mflags & FTDEB_HASFONT)
	{
		READ_UWORD(data.mfontID); 
		READ_UWORD(data.mfontHeight);
	}
	if(data.mflags & FTDEB_HASTEXTCOLOR)
	{
		data.mcolor.SetAlphaWriteMode(true);
		in >> data.mcolor;
	}
	if(data.mflags & FTDEB_HASMAXLENGTH)
	{
		READ_UWORD(data.mmaxLength); 
	}
	if(data.mflags & FTDEB_HASLAYOUT)
	{
		data.malign = in.get();
		// if(data.malign == EOF) throw
		READ_UWORD(data.mleftmargin);
		READ_UWORD(data.mrightmargin);
		READ_UWORD(data.mindent);
		READ_UWORD(data.mleading);
	}
	{
		N_STD::vector<int> text;
		for(;;)
		{
			int i = in.get();
			//if(i == EOF) throw;
			text.push_back(i);
			if(i==0) break;
		}
		data.mvariable = (char *)malloc(text.size());
		data.gc.push_back(data.mvariable);
		{
			for(unsigned int i = 0; i < text.size(); i++)
			{
				unsigned char *c = (unsigned char *)data.mvariable;
				c[i] = text[i];
			}		
		}
	}

	if(data.mflags & FTDEB_HASTEXT)
	{
		N_STD::vector<int> text;
		for(;;)
		{
			int i = in.get();
			//if(i == EOF) throw;
			text.push_back(i);
			if(i==0) break;
		}
		data.minitialtext = (char *)malloc(text.size());
		data.gc.push_back(data.minitialtext);
		{
			for(unsigned int i = 0; i < text.size(); i++)
			{
				unsigned char *c = (unsigned char *)data.minitialtext;
				c[i] = text[i];
			}		
		}	
	}
	return in;
}

