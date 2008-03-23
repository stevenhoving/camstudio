#include <strstream>
#include "FControl.h"
#include "FSprite.h"
#include "FDisplay.h"

void FlashTagSprite::Add(FlashSpriteEnabled *tag,bool addFrames /*= true*/)
{ 
	tags.push_back(tag);
	if(tag->isFrame() && addFrames) frames++;
}

N_STD::ostream &operator << (N_STD::ostream &out, FlashTagSprite &data)
{
	N_STD::ostrstream tmp;
	for(N_STD::vector<FlashSpriteEnabled *>::iterator i=data.tags.begin(); i != data.tags.end(); i++)
	{
		(*i)->Write(tmp);
	}
	tmp << FlashTagEnd();
	out << FlashTagHeader(39, 4+tmp.pcount());
	WRITE_UWORD(data.GetID());
	WRITE_UWORD(data.frames);
	out.write(tmp.rdbuf()->str(),tmp.pcount());
	return out;
}

N_STD::istream &operator >> (N_STD::istream &in,  FlashTagSprite &data)
{
	UWORD id;
	READ_UWORD(id);
	data.SetID(id);
	READ_UWORD(data.frames);

	return in;
}

N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineMovie &data)
{
	out << FlashTagHeader(38, (UDWORD)strlen(data.str)+3);
	WRITE_UWORD(data.GetID());
	out << data.str;
	out.put((char)0);
	return out;
}
	
N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineMovie &data)
{
	UWORD id;
	READ_UWORD(id);
	data.SetID(id);
	data.str = (char *)malloc(data.importsize - 2);	
	in.read(data.str, data.importsize - 2);
	data.gc.push_back(data.str);
	return in;
}
