#include <iostream>
#include <sstream>
#include <strstream>
#include "FShape.h"

N_STD::ostream &operator<< (N_STD::ostream &out, FlashFillStyle &data)
{
	data.Write(out);
	return out;
}
N_STD::istream &operator>> (N_STD::istream &in,  FlashFillStyle &data)
{
	data.Read(in);
	return in;
}

void FlashFillStyleSolid::Write(N_STD::ostream &out) 
{
	out.put((char)0);
	if(GetTagVersion() > 2) r1.SetAlphaWriteMode(true);
	else					r1.SetAlphaWriteMode(false);
	out << r1;
}
void FlashFillStyleSolid::Read(N_STD::istream &in) 
{
	int c = in.get();
	if(c==EOF || c != 0)
	{
		//throw
	}
	if(GetTagVersion() > 2) r1.SetAlphaWriteMode(true);
	else					r1.SetAlphaWriteMode(false);
	in >> r1;
}
void FlashFillStyleBitmap::Write(N_STD::ostream &out) 
{
	if(tiled) out.put((char)0x40);
	else	  out.put((char)0x41);
	WRITE_UWORD(bitmapID);
	out << matrix;

}
void FlashFillStyleBitmap::Read(N_STD::istream &in) 
{
	int c = in.get();
	if(c == EOF)
	{
		//throw
	}
	else if(c == 0x40)
	{
		tiled = true;
		type = 0x40;
	}
	else if(c == 0x41)
	{
		tiled = false;
		type = 0x41;
	}
	else
	{
		//throw
	}
	READ_UWORD(bitmapID);

	in >> matrix;
}


N_STD::ostream &operator<< (N_STD::ostream &out, FlashGradientRecord &data)
{
	out.put((unsigned char)data.ratios.size());
	for(unsigned int i = 0; i < data.ratios.size(); i++)
	{
		out.put(data.ratios[i]);
		
		if(data.GetTagVersion() > 2) data.colors[i].SetAlphaWriteMode(true);
		else						 data.colors[i].SetAlphaWriteMode(false);

		out << data.colors[i];
	}
	return out;
}

N_STD::istream &operator>> (N_STD::istream &in,  FlashGradientRecord &data)
{
	int c = in.get();
	//if(i == EOF) throw;
	for(int i = 0; i < c; i++)
	{
		int c2 = in.get();
		//if(c2 == EOF) throw;

		data.ratios.push_back(c2);
		data.colors.push_back(FlashRGB(0,0,0));

		if(data.GetTagVersion() > 2) data.colors[i].SetAlphaWriteMode(true);
		else						 data.colors[i].SetAlphaWriteMode(false);

		in >> data.colors[i];
	}
	return in;
}


void FlashFillStyleGradient::Write(N_STD::ostream &out)
{
	out.put((char)type);
	gradient.SetTagVersion(GetTagVersion());
	out << matrix;
	out << gradient;
}

void FlashFillStyleGradient::Read(N_STD::istream &in)
{
	int c = in.get();
	//if(c == EOF) throw;
	//if(c != 0x10 || c != 0x12) throw;
	
	type = c;
	
	gradient.SetTagVersion(GetTagVersion());
	in >> matrix;
	in >> gradient;
}

FlashFillStyleArray::FlashFillStyleArray( const FlashFillStyleArray &that )
{
	// Copy list contents
	for(N_STD::vector<FlashFillStyle*>::const_iterator i = that.styles.begin(); 
		i != that.styles.end(); i++ )
	{
		FlashFillStyle *style = (*i)->Copy();
		gc.push_back( style );
		styles.push_back( style );
	}
}

FlashLineStyleArray::FlashLineStyleArray( const FlashLineStyleArray &that )
{
	// Copy list contents
	for(N_STD::vector<FlashLineStyle*>::const_iterator i = that.styles.begin(); 
		i != that.styles.end(); i++ )
	{
		FlashLineStyle *style = (*i)->Copy();
		gc.push_back( style );
		styles.push_back( style );
	}
}

N_STD::ostream &operator<< (N_STD::ostream &out, FlashFillStyleArray &data)
{
    if( (data.styles.size() >= 0xff) && (data.GetTagVersion() > 1) )
    {
        out.put( (unsigned char)0xff);
        WRITE_SWORD( data.styles.size() );
    }
    else
        out.put((unsigned char)data.styles.size());

	for(N_STD::vector<FlashFillStyle*>::iterator i=data.styles.begin(); i!=data.styles.end(); i++)
	{
		FlashFillStyle *style = *i;
		(*style).SetTagVersion(data.GetTagVersion());
		out << **i;
	}
	return out;
}
N_STD::istream &operator>> (N_STD::istream &in,  FlashFillStyleArray &data)
{
	SWORD c2 = in.get();
	if((c2 == 0xff) && (data.GetTagVersion() > 1))
	{
		READ_SWORD(c2);		
	}
	//if(c == EOF) throw;
	for(int i = 0; i < c2; i++)
	{
		//if(c == EOF) throw;
		int c = in.get();	
		/* FILL TYPES
		0x00 - solid
		0x10 - linear gradient
		0x12 - radial gradient
		0x40 - tiled bitmap
		0x41 - clipped bitmap
		*/
		FlashFillStyle *style = NULL;
		if(c == 0x00)
		{
			in.putback(c);
			FlashFillStyleSolid *tmp = new FlashFillStyleSolid();
			data.gc.push_back(tmp);
			tmp->SetTagVersion(data.GetTagVersion());
			in >> *tmp;
			style = tmp;

		}
		else if(c == 0x10 || c == 0x12)
		{
			in.putback(c);
			FlashFillStyleGradient *tmp = new FlashFillStyleGradient();
			data.gc.push_back(tmp);
			tmp->SetTagVersion(data.GetTagVersion());
			in >> *tmp;
			style = tmp;
		}
		else if(c == 0x40 || c == 0x41)
		{			
			in.putback(c);
			FlashFillStyleBitmap *tmp = new FlashFillStyleBitmap();
			data.gc.push_back(tmp);
			tmp->SetTagVersion(data.GetTagVersion());
			in >> *tmp;
			style = tmp;
		}
		else
		{
			//throw
		}
		if ( style != NULL )
			data.styles.push_back(style);
	}
	return in;
}

int FlashFillStyleArray::GetNBits()
{
	return GetBitSize(styles.size());
}

N_STD::ostream &operator<< (N_STD::ostream &out, FlashLineStyle &data)
{
	WRITE_UWORD(data.width);

	if(data.GetTagVersion() > 2) data.color.SetAlphaWriteMode(true);
	else					data.color.SetAlphaWriteMode(false);

	out << data.color;
	return out;
}
N_STD::istream &operator>> (N_STD::istream &in,  FlashLineStyle &data)
{
	READ_UWORD(data.width);

	if(data.GetTagVersion() > 2) data.color.SetAlphaWriteMode(true);
	else						 data.color.SetAlphaWriteMode(false);

	
	in >> data.color;
	return in;
}

int FlashLineStyleArray::GetNBits()
{
	return GetBitSize(styles.size());
}
N_STD::ostream &operator<< (N_STD::ostream &out, FlashLineStyleArray &data)
{
    if( (data.styles.size() >= 0xff) && (data.GetTagVersion() > 1) )
    {
        out.put( (unsigned char)0xff);
        WRITE_SWORD( data.styles.size() );
    }
    else
        out.put((unsigned char)data.styles.size());
	for(N_STD::vector<FlashLineStyle*>::iterator i=data.styles.begin(); i!=data.styles.end(); i++)
	{
		(**i).SetTagVersion(data.GetTagVersion());
		out << **i;
	}
	return out;
}
N_STD::istream &operator>> (N_STD::istream &in,  FlashLineStyleArray &data)
{
	SWORD c = in.get();
	if((c == 0xff) && (data.GetTagVersion() > 1))
	{
		READ_SWORD(c);		
	}
	//if(c == EOF) throw;
	for(int i = 0; i < c; i++)
	{
		FlashLineStyle *style = new FlashLineStyle();
		data.gc.push_back(style);
		style->SetTagVersion(data.GetTagVersion());		
		in >> *style;
		data.styles.push_back(style);		
	}
	return in;
}


void FlashShapeRecordStraight::Write(BitBuffer &out, FlashShapeCommon &data)
{
	int bits = fbase_max(GetBitSizeSigned(dx),GetBitSizeSigned(dy))-2;
	if(bits < 1) bits = 1;
	
	BitBuffer &b=out;
	b.Write(1,1);
	b.Write(1,1);
	
	b.Write(bits,4);
	if(dx==0)
	{
		b.Write(0,1);
		b.Write(1,1);
		b.Write(PackBitsSigned(dy),bits+2);
	}
	else if(dy==0)
	{
		b.Write(0,1);
		b.Write(0,1);
		b.Write(PackBitsSigned(dx),bits+2);
	}
	else
	{
		b.Write(1,1);
		b.Write(PackBitsSigned(dx),bits+2);
		b.Write(PackBitsSigned(dy),bits+2);
	}
}
void FlashShapeRecordStraight::Read(BitStreamIn &in, FlashShapeCommon &data)
{
	int bits;	
	in.Read(bits,4);

	bits+=2;

	int type;
	in.Read(type, 1);
	if(type == 0)
	{
		int type2;
		in.Read(type2, 1);

		if(type2 == 1)
		{
			in.Read(dy,bits);
			dy = UnPackBitsSigned(dy, bits);
			dx = 0;
		}
		else if(type2 == 0)
		{
			in.Read(dx,bits);
			dx = UnPackBitsSigned(dx, bits);
			dy = 0;
		}
	}
	else
	{
		in.Read(dx,bits);
		dx = UnPackBitsSigned(dx, bits);
		in.Read(dy,bits);
		dy = UnPackBitsSigned(dy, bits);
	}
}

void FlashShapeRecordCurved::Write(BitBuffer &out, FlashShapeCommon &data)
{
	BitBuffer &b=out;
	int bits = (fbase_max(fbase_max(GetBitSizeSigned(dxa),GetBitSizeSigned(dya)), 
					fbase_max(GetBitSizeSigned(dxc),GetBitSizeSigned(dyc))
					)
				)-2;
	if(bits < 1) bits = 1;


	b.Write(1,1);
	b.Write(0,1);
	
	b.Write(bits,4);
	b.Write(PackBitsSigned(dxc),bits+2);
	b.Write(PackBitsSigned(dyc),bits+2);
	b.Write(PackBitsSigned(dxa),bits+2);
	b.Write(PackBitsSigned(dya),bits+2);
}
void FlashShapeRecordCurved::Read(BitStreamIn &in, FlashShapeCommon &data)
{
	BitStreamIn &b=in;
	int bits;
	b.Read(bits,4);
	b.Read(dxc,bits+2);
	dxc = UnPackBitsSigned(dxc, bits+2);
	b.Read(dyc,bits+2);
	dyc = UnPackBitsSigned(dyc, bits+2);
	b.Read(dxa,bits+2);
	dxa = UnPackBitsSigned(dxa, bits+2);
	b.Read(dya,bits+2);
	dya = UnPackBitsSigned(dya, bits+2);
}
FlashShapeRecordChange::FlashShapeRecordChange()
{
	newstyles=false;
	linestyle=false;
	fillstyle1=false;
	fillstyle0=false;
	moveto=false;
	stylefill0 = 0;
	stylefill1 = 0;
	styleline = 0;
}

FlashShapeRecordChange::FlashShapeRecordChange(SDWORD _dx, SDWORD _dy)
{
	newstyles=false;
	linestyle=false;
	fillstyle1=false;
	fillstyle0=false; 
	moveto=true;

	dx=_dx;
	dy=_dy;
	stylefill0 = 0;
	stylefill1 = 0;
	styleline = 0;
}
	
void FlashShapeRecordChange::NewFillStyles(FlashFillStyleArray &a, FlashLineStyleArray &c)
{
	newstyles=true;
	fillstyles=a;
	linestyles=c;
}

void FlashShapeRecordChange::ChangeFillStyle1(UWORD style)
{
	fillstyle1=true;
	stylefill1=style;
}
void FlashShapeRecordChange::ChangeFillStyle0(UWORD style)
{
	fillstyle0=true;
	stylefill0=style;
}

void FlashShapeRecordChange::ChangeLineStyle(UWORD style)
{
	linestyle=true;
	styleline=style;
}
void FlashShapeRecordChange::Write(BitBuffer &out, FlashShapeCommon &data)
{
	BitBuffer &b=out;
	b.Write(0,1);
	b.Write(newstyles,1);
	b.Write(linestyle,1);
	b.Write(fillstyle1,1);
	b.Write(fillstyle0,1);
	b.Write(moveto,1);
	if(moveto)
	{
		int m = fbase_max(GetBitSizeSigned(dx),GetBitSizeSigned(dy));
		b.Write(m,5);
		b.Write(PackBitsSigned(dx),m);
		b.Write(PackBitsSigned(dy),m);
	}
	if(fillstyle0)
	{
		b.Write(stylefill0, data.NBitsFillStyle);
	}
	if(fillstyle1)
	{
		b.Write(stylefill1, data.NBitsFillStyle);
	}
	if(linestyle)
	{
		b.Write(styleline,  data.NBitsLineStyle);
	}
	if(newstyles)
	{
		N_STD::ostrstream str;
		fillstyles.SetTagVersion(GetTagVersion());
		linestyles.SetTagVersion(GetTagVersion());
		str << fillstyles;
		str << linestyles;
		BitBuffer b2;
		b2.Write(fillstyles.GetNBits(),4);
		b2.Write(linestyles.GetNBits(),4);
		str << b2;
				
		b.Align();
		b.WriteBytes(str.rdbuf()->str(),str.pcount());
		
		data.NBitsFillStyle = fillstyles.GetNBits();
		data.NBitsLineStyle = linestyles.GetNBits();		
	}
}
void FlashShapeRecordChange::Read(BitStreamIn &in, FlashShapeCommon &data)
{
	BitStreamIn &b=in;

	
	newstyles = (data.flags & 16) == 16;
	linestyle = (data.flags & 8) == 8;
	fillstyle1 = (data.flags & 4) == 4;
	fillstyle0 = (data.flags & 2) == 2;
	moveto = (data.flags & 1) == 1;

	if(moveto)
	{
		int m;
		b.Read(m,5);
		b.Read(dx,m);
		dx = UnPackBitsSigned(dx,m);
		b.Read(dy,m);
		dy = UnPackBitsSigned(dy,m);
	}
	if(fillstyle0)
	{
		b.Read(stylefill0, data.NBitsFillStyle);
	}
	if(fillstyle1)
	{
		b.Read(stylefill1, data.NBitsFillStyle);
	}
	if(linestyle)
	{
		b.Read(styleline,  data.NBitsLineStyle);
	}
	if(newstyles)
	{
		b.Align();
		
		fillstyles.SetTagVersion(GetTagVersion());
		linestyles.SetTagVersion(GetTagVersion());

		b.GetStream() >> fillstyles;
		b.GetStream() >> linestyles;
				
		b.Read(data.NBitsFillStyle,4);
		b.Read(data.NBitsLineStyle,4);		
	}

}

void FlashShapeRecordEnd::Write(BitBuffer &out, FlashShapeCommon &data)
{
    BitBuffer &b=out;
	b.Write(0,1);
	b.Write(0,5);
}
void FlashShapeRecordEnd::Read(BitStreamIn &in, FlashShapeCommon &data)
{
}

FlashRect FlashShape::GetBounds()
{
    FlashRect prep;
	SDWORD curx=0;
	SDWORD cury=0;
	
	N_STD::vector<flash_pair<SDWORD, SDWORD> > coords;
	
	for(N_STD::vector<flash_pair<int,long> >::iterator i = record_sequencer.begin(); i != record_sequencer.end(); i++)
	{
		if((*i).first==0)
		{
			FlashShapeRecordChange &rec = record_change[(*i).second];
			if(rec.moveto)
			{
				coords.push_back(flash_pair<SDWORD,SDWORD>(rec.dx,rec.dy));
				curx = rec.dx;
				cury = rec.dy;				
			}

		}
		else if((*i).first==1)
		{
			FlashShapeRecordStraight &rec =record_straight[(*i).second];
			curx+=rec.dx;
			cury+=rec.dy;
			coords.push_back(flash_pair<SDWORD,SDWORD>(curx, cury));

		}
		else if((*i).first==2)
		{
			FlashShapeRecordCurved &rec =record_curved[(*i).second];
			curx+=rec.dxc;
			cury+=rec.dyc;
			coords.push_back(flash_pair<SDWORD,SDWORD>(curx, cury));
			curx+=rec.dxa;
			cury+=rec.dya;
			coords.push_back(flash_pair<SDWORD,SDWORD>(curx, cury));
		}
	}
	
	if(coords.size() > 0)
	{
		prep=FlashRect(coords[0].first,coords[0].second, coords[0].first, coords[0].second);
		for(N_STD::vector<flash_pair< SDWORD, SDWORD> >::iterator i=coords.begin(); i != coords.end(); i++)
		{
			prep.BoundWith(FlashRect((*i).first,(*i).second,(*i).first,(*i).second));
		}
	}

	prep.SetRect(prep.GetX1() - 100, prep.GetY1() - 100, prep.GetX2()+100, prep.GetY2()+100);
	
	return prep;
}
N_STD::ostream &operator<< (N_STD::ostream &out,  FlashShape &data)
{
	BitBuffer b;
	FlashShapeCommon c;
	c.NBitsFillStyle = 1;
	c.NBitsLineStyle = 1;
	b.Write(1,4);
	b.Write(1,4);
	for(N_STD::vector<flash_pair<int,long> >::iterator i = data.record_sequencer.begin(); i != data.record_sequencer.end(); i++)
	{
		if((*i).first==0)
		{
			data.record_change[(*i).second].Write(b,c);
		}
		else if((*i).first==1)
		{
			data.record_straight[(*i).second].Write(b,c);
		}
		else if((*i).first==2)
		{
			data.record_curved[(*i).second].Write(b,c);
		}
	}
	FlashShapeRecordEnd e;
	e.Write(b,c);
	out << b;
	return out;
}
N_STD::istream &operator>> (N_STD::istream &in,   FlashShape &data)
{
	BitStreamIn b(&in);
	FlashShapeCommon c;
	c.NBitsFillStyle = 1;
	c.NBitsLineStyle = 1;
	int tmp;
	b.Read(tmp,4);
	b.Read(tmp,4);
	
	for(;;)
	{
		int type;
		b.Read(type,1);
		if(type == 0)
		{
			int flags;
			b.Read(flags,5);
			if(flags == 0)
				break;
			else
			{
				FlashShapeRecordChange r;
				c.flags = flags;
				r.Read(b, c);
				data.AddRecord(r);
			}
		}
		if(type == 1)
		{
			int type2;
			b.Read(type2,1);
			if(type2 == 1)
			{
				FlashShapeRecordStraight r;
				r.Read(b, c);
				data.AddRecord(r);
			}
			if(type2 == 0)
			{
				FlashShapeRecordCurved r;
				r.Read(b, c);
				data.AddRecord(r);
			}			
		}
	}
	return in;
}

N_STD::ostream &operator<< (N_STD::ostream &out,  FlashShapeWithStyle &data)
{
	data.fillstyles.SetTagVersion(data.GetTagVersion());
	data.linestyles.SetTagVersion(data.GetTagVersion());
	out << data.fillstyles;
	out << data.linestyles;
	BitBuffer b;
	data.fillstyles.SetTagVersion(data.GetTagVersion());
	b.Write(data.fillstyles.GetNBits(),4);
	b.Write(data.linestyles.GetNBits(),4);
	FlashShapeCommon c;
	c.NBitsFillStyle = data.fillstyles.GetNBits();
	c.NBitsLineStyle = data.linestyles.GetNBits();
	c.DefineTagVersion = data.GetTagVersion();
	for(N_STD::vector<flash_pair<int,long> >::iterator i = data.record_sequencer.begin(); i != data.record_sequencer.end(); i++)
	{
		if((*i).first==0)
		{			
			data.record_change[(*i).second].SetTagVersion(data.GetTagVersion());
			data.record_change[(*i).second].Write(b,c);
		}
		else if((*i).first==1)
		{
			data.record_straight[(*i).second].SetTagVersion(data.GetTagVersion());
			data.record_straight[(*i).second].Write(b,c);
		}
		else if((*i).first==2)
		{
			data.record_curved[(*i).second].SetTagVersion(data.GetTagVersion());
			data.record_curved[(*i).second].Write(b,c);
		}
	}
	FlashShapeRecordEnd e;
	e.Write(b,c);
	out << b;
	return out;
}
N_STD::istream &operator>> (N_STD::istream &in, FlashShapeWithStyle  &data)
{
	data.fillstyles.SetTagVersion(data.GetTagVersion());
	data.linestyles.SetTagVersion(data.GetTagVersion());
	in >> data.fillstyles;
	in >> data.linestyles;
	BitStreamIn b(&in);
	FlashShapeCommon c;
	b.Read(c.NBitsFillStyle,4);
	b.Read(c.NBitsLineStyle,4);

	c.DefineTagVersion = data.GetTagVersion();
	
	int i = 0;
	for( ; !b.GetStream().fail(); )
	{
		i++;
		int type;
		b.Read(type,1);
		if(type == 0)
		{
			int flags;
			b.Read(flags,5);
			
			if(flags == 0)
			{
				break;
			}
			else
			{
				FlashShapeRecordChange r;
				c.flags = flags;
				r.SetTagVersion(data.GetTagVersion());
				r.Read(b, c);
				data.AddRecord(r);
			}
		}
		if(type == 1)
		{
			int type2;
			b.Read(type2,1);
			if(type2 == 1)
			{
				FlashShapeRecordStraight r;
				r.SetTagVersion(data.GetTagVersion());
				r.Read(b, c);				
				data.AddRecord(r);
			}
			else if(type2 == 0)
			{
				FlashShapeRecordCurved r;
				r.SetTagVersion(data.GetTagVersion());
				r.Read(b, c);
				data.AddRecord(r);
			}			
		}
	}
	return in;
}
N_STD::ostream &operator<< (N_STD::ostream &out, FlashTagDefineShapeBase &data)
{
	N_STD::ostringstream str(N_STD::ios_base::binary);
	WRITE_UWORD2(data.GetID(),str);
	str << data.shapes.GetBounds();
	data.shapes.SetTagVersion(data.version);
	str << data.shapes;
	out << FlashTagHeader(data.header,str.str().size());
	out.write(str.str().c_str(),str.str().size());
	return out;
}
N_STD::istream &operator>> (N_STD::istream &in,  FlashTagDefineShapeBase &data)
{
	UWORD id;
	READ_UWORD(id);
	data.SetID(id);	
	in >> data.rimport;
	data.shapes.SetTagVersion(data.version);
	in >> data.shapes;
	return in;
}
