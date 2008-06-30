#include "FlashFontObj.h"
#include "FFont.h"
#include "FDisplay.h"
#include <iostream>
#include <windows.h>
#include <math.h>
#include <hash_map>

SWORD SWORDFromFixed(FIXED f)
{
	SWORD ret = f.value;
	if(f.fract > 0x8000) ret+=1;
	return (SWORD)ret;
}

bool FlashFontFactory::GetGlyphShape(const char *font, UWORD charindex, FlashShape & s, bool bold, bool italic, bool uLine)
{

	unsigned int fWeight=400;
    if(bold==true)fWeight=700;

	FlashShape &r=s;
	FlashShapeRecordChange c;
	c.ChangeFillStyle0(1);
	c.ChangeLineStyle(0);
	s.AddRecord(c);
	HDC tmphdc = CreateDC("DISPLAY", NULL, NULL, NULL); 
	HDC hdcScreen = CreateCompatibleDC(tmphdc); 
    	
	long base = 1300;
	HFONT myfont = CreateFont(-base,0,0,0,fWeight,italic ? TRUE : FALSE,uLine ? TRUE : FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH,font);	
	HGDIOBJ obj = SelectObject(hdcScreen,myfont);
	
	unsigned int c2=charindex;
	
	GLYPHMETRICS gm;
	DWORD size;
	MAT2 matrix;
	FIXED f1;
	f1.value = 1;
	f1.fract = 0;
	FIXED f0;
	f0.value = 0;
	f0.fract = 0;
	matrix.eM11 = f1;
	matrix.eM12 = f0;
	matrix.eM21 = f0;
	matrix.eM22 = f1;
	
	SWORD lastx=0;
	SWORD lasty=0;
	if(c2 != ' ')
	{
		if((size = GetGlyphOutline(hdcScreen,c2,GGO_NATIVE,&gm,0,NULL,&matrix)) != GDI_ERROR)
		{ 
			
			char *buffer = new char[size+1];
			if((size = GetGlyphOutline(hdcScreen,c2,GGO_NATIVE,&gm,size,buffer,&matrix)) != GDI_ERROR)
			{
				if(uLine==true)
				{
					INT charWidth=0;
					GetCharWidth(hdcScreen,charindex,charindex,&charWidth);
          
					r.AddRecord(FlashShapeRecordChange(0,1536));
					r.AddRecord(FlashShapeRecordStraight(charWidth,0));
					r.AddRecord(FlashShapeRecordStraight(0,-100));
					r.AddRecord(FlashShapeRecordStraight(-charWidth,0));
					r.AddRecord(FlashShapeRecordStraight(0,100)); 
					r.AddRecord(FlashShapeRecordChange(0,-1536));       
				}
				
				char *pos = (char*)buffer;
				while(pos < buffer+size)
				{
					TTPOLYGONHEADER *header = (TTPOLYGONHEADER *)pos;
					pos+=sizeof(TTPOLYGONHEADER);
	
					SWORD hx = SWORDFromFixed(header->pfxStart.x);
					SWORD hy = 1024-SWORDFromFixed(header->pfxStart.y);
					r.AddRecord(FlashShapeRecordChange(hx,hy));
					lastx = hx;
					lasty = hy;
					while(pos < (char*)header+(header->cb))
					{			
						
						TTPOLYCURVE *curve = (TTPOLYCURVE *)pos;
						// handle polylines
						if(curve->wType == TT_PRIM_LINE)
						{

							for(WORD i=0; i < curve->cpfx; i++)
							{
								SWORD x = SWORDFromFixed(curve->apfx[i].x);
								SWORD y = 1024-SWORDFromFixed(curve->apfx[i].y);
								r.AddRecord(FlashShapeRecordStraight(x-lastx,y-lasty));
								lastx += x-lastx;
								lasty += y-lasty;
							}
						}
						
						// handle lines
						else
						{
							POINTFX pfxB,pfxC;
							for (int u = 0; u < curve->cpfx - 1; u++) // Walk through points in spline 
							{
								pfxB = curve->apfx[u];				// B is always the current point
							   
							   SWORD cx_;
							   SWORD cy_;
							   SWORD ax_;
							   SWORD ay_;

							   cx_ = SWORDFromFixed(pfxB.x);
							   cy_ = SWORDFromFixed(pfxB.y);

							   if (u < curve->cpfx - 2) 		   // If not on last spline, compute C
							   {					  
								  ax_ = (cx_ + SWORDFromFixed(curve->apfx[u+1].x)) / 2;  // x midpoint
								  ay_ = (cy_ + SWORDFromFixed(curve->apfx[u+1].y)) / 2;  // y midpoint
							   }
							   else 
							   {
								  pfxC = curve->apfx[u+1];
								  
								  ax_ = SWORDFromFixed(pfxC.x);
								  ay_ = SWORDFromFixed(pfxC.y);
							   }

							   cy_=1024-cy_;
							   ay_=1024-ay_;
							   SWORD cx = cx_-lastx;
							   SWORD cy = cy_-lasty;
							   SWORD rx = ax_-cx_;
							   SWORD ry = ay_-cy_;
							   lastx += ax_-lastx;
							   lasty += ay_-lasty;
							   r.AddRecord(FlashShapeRecordCurved(cx,cy,rx,ry));
						   }
						}
						pos+=(sizeof(WORD)*2) + (sizeof(POINTFX)*curve->cpfx);
					}
					r.AddRecord(FlashShapeRecordStraight(hx-lastx, hy-lasty));
					lastx = hx;
					lasty = hy;
				}			
			}
			else 
			{
				return false; // ERROR
			}
			delete[] buffer;
		}
		else 
		{
			return false; // ERROR
		} 
	}

	SelectObject(hdcScreen,obj);
	DeleteObject(myfont);
	DeleteObject(hdcScreen);
	return true;
}


int FlashFontFactory::GetGlyphAdvance(const char *fontname, UWORD charindex, int pointsize)
{
	HDC tmphdc = CreateDC("DISPLAY", NULL, NULL, NULL); 
	HDC hdcScreen = CreateCompatibleDC(tmphdc); 
 	
	OUTLINETEXTMETRIC om;
	GetOutlineTextMetrics(hdcScreen,sizeof(om),&om);

	HFONT myfont = CreateFont(-1300/50*pointsize,0,0,0,FW_REGULAR,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,fontname);
	HGDIOBJ obj = SelectObject(hdcScreen,myfont);

	INT i=0;
	BOOL ret=::GetCharWidth(hdcScreen,charindex,charindex,&i);

	SelectObject(hdcScreen,obj);
	DeleteObject(myfont);
	DeleteObject(hdcScreen);

	if(ret == 0) return -1;
	else return i;

}

UWORD FlashFontFactory::WriteText(std::ostream &out, const char *fontname, const char *text, int x, int y, FlashRGB color, int pointsize, int depth, int extraspacing, bool bold, bool italic, bool uLine)
{
	FlashRect textBounds;

	std::hash_map<int,FlashShape> shapes;
	std::hash_map<int,int> shapes_lookup;
	
	FlashTagDefineFont df;
		
	FlashTextRecordStyle* trs1 = new FlashTextRecordStyle(1,1,1,1,df.GetID(), pointsize*20, color, x, y);
	FlashTextRecordGlyph* trs2 = new FlashTextRecordGlyph();

	FlashRect bounds(x,y,x,y);
	int index = 0;
	int width = 0;

	UWORD uCode;
	bool bDBCS;
	textBounds.SetRect(x,y,x,y);
	for(int i=0; text[i] != 0; )
	{
		//Added=2001.7.16
		//if (text[i] & 0x80) {
		if (IsDBCSLeadByte(text[i])) {
			bDBCS = true;
			uCode = ((text[i]<<8)&0xff00) | (text[i+1]&0x00ff);
		}
		else {
			bDBCS = false;
			uCode = text[i];
		}
		
		if(shapes.find(uCode) == shapes.end())
		{
			shapes[uCode] = FlashShape();
			if(!GetGlyphShape(fontname,(UWORD)uCode,shapes[uCode],bold,italic,uLine)) return false; 
			df.AddShape(shapes[uCode]);
			shapes_lookup[uCode]=index;
			index++;
		}		
		int advance = GetGlyphAdvance(fontname,uCode,pointsize)+extraspacing;
		trs2->AddGlyph(FlashGlyphEntry(shapes_lookup[uCode],advance));

		FlashRect r = shapes[uCode].GetBounds();
		FlashRect b(r.GetX1() / 50 * pointsize+width+x+extraspacing*i, 
					r.GetY1() / 50 * pointsize+y, 
					r.GetX2() / 50 * pointsize+width+x+extraspacing*i,
					r.GetY2() / 50 * pointsize+y);

		if(index == 0) bounds = b;
		else bounds.BoundWith(b);
		width+=advance;	

		//cout << "index " << index << " ----------------\n";
		//cout << bounds.GetX1() << "   " << bounds.GetY1() << "   " << bounds.GetX2() << "   " << bounds.GetY2() << "\n";

		//Added=2001.7.16
		if (bDBCS) i += 2;
		else i++;
		textBounds.SetRect(x,y,
						textBounds.GetX2()+r.GetX2()-r.GetX1(), 
						max(textBounds.GetY2(),y+r.GetY2()-r.GetY1()));
	}
	//FlashTagDefineText dt(bounds,FlashMatrix());
	//Modified=2001.7.16
	FlashTagDefineText dt(textBounds,FlashMatrix());
	
	dt.AddTextRecord(trs1);
	dt.AddTextRecord(trs2);
	
	out << df;
	out << dt;
    
	delete trs1;
	delete trs2;

	out << FlashTagPlaceObject2(depth,dt.GetID());

	return dt.GetID();
}

UWORD FlashFontFactory::WriteText(std::ostream &out, const char *fontname, const char *text, int x, int y, FlashRGB color, int pointsize, int depth, FlashRect& textBounds, int extraspacing, bool bold, bool italic, bool uLine)
{
	std::hash_map<int,FlashShape> shapes;
	std::hash_map<int,int> shapes_lookup;
	
	FlashTagDefineFont df;
		
	FlashTextRecordStyle* trs1 = new FlashTextRecordStyle(1,1,1,1,df.GetID(), pointsize*20, color, x, y);
	FlashTextRecordGlyph* trs2 = new FlashTextRecordGlyph();

	FlashRect bounds(x,y,x,y);
	int index = 0;
	int width = 0;

	UWORD uCode;
	bool bDBCS;
	textBounds.SetRect(x,y,x,y);
	for(int i=0; text[i] != 0; )
	{
		//Added=2001.7.16
		//if (text[i] & 0x80) {
		if (IsDBCSLeadByte(text[i])) {
			bDBCS = true;
			uCode = ((text[i]<<8)&0xff00) | (text[i+1]&0x00ff);
		}
		else {
			bDBCS = false;
			uCode = text[i];
		}
		
		if(shapes.find(uCode) == shapes.end())
		{
			shapes[uCode] = FlashShape();
			if(!GetGlyphShape(fontname,(UWORD)uCode,shapes[uCode],bold,italic,uLine)) return false;
			df.AddShape(shapes[uCode]);
			shapes_lookup[uCode]=index;
			index++;
		}		
		int advance = GetGlyphAdvance(fontname,uCode,pointsize)+extraspacing;
		trs2->AddGlyph(FlashGlyphEntry(shapes_lookup[uCode],advance));

		FlashRect r = shapes[uCode].GetBounds();
		FlashRect b(r.GetX1() / 50 * pointsize+width+x+extraspacing*i, 
					r.GetY1() / 50 * pointsize+y, 
					r.GetX2() / 50 * pointsize+width+x+extraspacing*i,
					r.GetY2() / 50 * pointsize+y);

		if(index == 0) bounds = b;
		else bounds.BoundWith(b);
		width+=advance;	

		//cout << "index " << index << " ----------------\n";
		//cout << bounds.GetX1() << "   " << bounds.GetY1() << "   " << bounds.GetX2() << "   " << bounds.GetY2() << "\n";

		//Added=2001.7.16
		if (bDBCS) i += 2;
		else i++;
		textBounds.SetRect(x,y,
						textBounds.GetX2()+r.GetX2()-r.GetX1(), 
						max(textBounds.GetY2(),y+r.GetY2()-r.GetY1()));
	}
	//FlashTagDefineText dt(bounds,FlashMatrix());
	//Modified=2001.7.16
	FlashTagDefineText dt(textBounds,FlashMatrix());
	
	dt.AddTextRecord(trs1);
	dt.AddTextRecord(trs2);
	
	out << df;
	out << dt;
    
	delete trs1;
	delete trs2;

	out << FlashTagPlaceObject2(depth,dt.GetID());

	return dt.GetID();
}



UWORD FlashFontFactory::GetBounds(std::ostream &out, const char *fontname, const char *text, int x, int y, FlashRGB color, int pointsize, int depth, FlashRect& textBounds, int extraspacing, bool bold, bool italic, bool uLine)
{
	std::hash_map<int,FlashShape> shapes;
	std::hash_map<int,int> shapes_lookup;
	
	FlashTagDefineFont df;
		
	FlashTextRecordStyle* trs1 = new FlashTextRecordStyle(1,1,1,1,df.GetID(), pointsize*20, color, x, y);
	FlashTextRecordGlyph* trs2 = new FlashTextRecordGlyph();

	FlashRect bounds(x,y,x,y);
	int index = 0;
	int width = 0;

	UWORD uCode;
	bool bDBCS;
	textBounds.SetRect(x,y,x,y);
	for(int i=0; text[i] != 0; )
	{
		//Added=2001.7.16
		//if (text[i] & 0x80) {
		if (IsDBCSLeadByte(text[i])) {
			bDBCS = true;
			uCode = ((text[i]<<8)&0xff00) | (text[i+1]&0x00ff);
		}
		else {
			bDBCS = false;
			uCode = text[i];
		}
		
		if(shapes.find(uCode) == shapes.end())
		{
			shapes[uCode] = FlashShape();
			if(!GetGlyphShape(fontname,(UWORD)uCode,shapes[uCode],bold,italic,uLine)) return false;
			df.AddShape(shapes[uCode]);
			shapes_lookup[uCode]=index;
			index++;
		}		
		int advance = GetGlyphAdvance(fontname,uCode,pointsize)+extraspacing;
		trs2->AddGlyph(FlashGlyphEntry(shapes_lookup[uCode],advance));

		FlashRect r = shapes[uCode].GetBounds();
		FlashRect b(r.GetX1() / 50 * pointsize+width+x+extraspacing*i, 
					r.GetY1() / 50 * pointsize+y, 
					r.GetX2() / 50 * pointsize+width+x+extraspacing*i,
					r.GetY2() / 50 * pointsize+y);

		if(index == 0) bounds = b;
		else bounds.BoundWith(b);
		width+=advance;	

		//cout << "index " << index << " ----------------\n";
		//cout << bounds.GetX1() << "   " << bounds.GetY1() << "   " << bounds.GetX2() << "   " << bounds.GetY2() << "\n";

		//Added=2001.7.16
		if (bDBCS) i += 2;
		else i++;
		textBounds.SetRect(x,y,
						textBounds.GetX2()+r.GetX2()-r.GetX1(), 
						max(textBounds.GetY2(),y+r.GetY2()-r.GetY1()));
	}
	//FlashTagDefineText dt(bounds,FlashMatrix());
	//Modified=2001.7.16
	
	delete trs1;
	delete trs2;
	
	return 0;
}
