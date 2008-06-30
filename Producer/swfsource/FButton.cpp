#include <strstream>
#include "FButton.h"

FlashButtonRecord::FlashButtonRecord(UWORD _charID, UWORD _depth, char _validstates, 
									 FlashMatrix &_matrix, FlashColorTransform &_cfx) :
									 charID(_charID), depth(_depth), validstates(_validstates),
										 matrix(_matrix), cfx(_cfx)
{

}

N_STD::ostream &operator<< (N_STD::ostream &out, FlashButtonRecord &data)
{
	out.put(data.validstates);
	WRITE_UWORD(data.charID);
	WRITE_UWORD(data.depth);
	out << data.matrix;
	if(data.GetTagVersion() > 0)
	{
		data.cfx.SetTagVersion(data.GetTagVersion());
		out << data.cfx;
	}
	
	return out;
}

N_STD::istream &operator>> (N_STD::istream &in,  FlashButtonRecord &data)
{
	data.validstates = in.get();
	READ_UWORD(data.charID);
	READ_UWORD(data.depth);
	in >> data.matrix;
	data.cfx.SetTagVersion(data.GetTagVersion());
	if(data.GetTagVersion() > 1) in >> data.cfx;
	return in;
}

void FlashTagDefineButton::AddButtonRecord(FlashButtonRecord *r)
{
	buttonrecords.push_back(r);
}
void FlashTagDefineButton::AddActionRecord(FlashActionRecord *r)
{
	actionrecords.push_back(r);
}

N_STD::ostream &operator<< (N_STD::ostream &out, FlashTagDefineButton &data)
{
	
	N_STD::ostrstream tmp;
	
	for(N_STD::vector<FlashButtonRecord*>::iterator ib=data.buttonrecords.begin();
		ib != data.buttonrecords.end(); ib++)
		{
			(*ib)->SetTagVersion(0);
			(*ib)->Write(tmp);
		}
		tmp.put( (char) 0);
	
	for(N_STD::vector<FlashActionRecord*>::iterator ir=data.actionrecords.begin();
		ir != data.actionrecords.end(); ir++)
		{
			(*ir)->Write(tmp);
		}
		tmp.put( (char) 0);
	
		
	out << FlashTagHeader(7,tmp.pcount()+2);
	WRITE_UWORD(data.GetID());
	
	out.write(tmp.rdbuf()->str(), tmp.pcount());
	
	return out;
}

N_STD::istream &operator>> (N_STD::istream &in,  FlashTagDefineButton &data)
{
	
	UWORD d;
	READ_UWORD(d);
	data.SetID(d);
	
	int i;
	int count = 0;
	for(i = in.get(); i != 0; i=in.get())
	{
		in.putback(i);
		data.buttonrecords.push_back(new FlashButtonRecord());
		data.gcbuttonrecords.push_back(data.buttonrecords[count]);
		data.buttonrecords[count]->SetTagVersion(0);
		in >> *data.buttonrecords[count];
		count ++;
	}
	count = 0;

	FlashActionVectorImporter import;
	import.Import(in, data.actionrecords, data.gcactionrecords);
	
	return in;
}

FlashTagDefineButton2::~FlashTagDefineButton2() 
{ 	
}
void FlashTagDefineButton2::AddButtonRecord(FlashButtonRecord *r)
{
	buttonrecords.push_back(r);
}
void FlashTagDefineButton2::AddActionRecords(N_STD::vector<FlashActionRecord *> &r, UWORD conditionflags)
{
	actionrecords.push_back(flash_pair<N_STD::vector<FlashActionRecord *>, UWORD>(r,conditionflags));
}
N_STD::ostream &operator<< (N_STD::ostream &out, FlashTagDefineButton2 &data)
{
	N_STD::ostrstream tmp;
	for(N_STD::vector<FlashButtonRecord*>::iterator ib=data.buttonrecords.begin();
		ib != data.buttonrecords.end(); ib++)
		{
			(*ib)->SetTagVersion(2);
			(*ib)->Write(tmp); // tmp << *(*ib);
		}
		tmp.put((char)0);
	
	UWORD offset = tmp.pcount();
		
	for(N_STD::vector<flash_pair<N_STD::vector<FlashActionRecord *>,UWORD> >::iterator ir
			= data.actionrecords.begin();
		ir != data.actionrecords.end(); ir++)
		{
			N_STD::ostrstream tmp2;
			WRITE_UWORD2((*ir).second,tmp2);
			
			for(N_STD::vector<FlashActionRecord *>::iterator i = (*ir).first.begin(); 
				i != (*ir).first.end();
				i++)
				{
					(*i)->Write(tmp2);			
				}
			tmp2.put((char)0);

			N_STD::vector<flash_pair<N_STD::vector<FlashActionRecord *>,UWORD> >::iterator irt = ir;
			irt++;
			if(irt != (data.actionrecords.end()))
			{
				WRITE_UWORD2(tmp2.pcount(), tmp);
			}
			else 
			{
				WRITE_UWORD2(0, tmp);
			}
			tmp.write(tmp2.rdbuf()->str(), tmp2.pcount());
		}		
	out << FlashTagHeader(34,tmp.pcount()+5);	
	WRITE_UWORD(data.GetID());
	out.put((char)data.menu);
	WRITE_UWORD(offset+2);
	out.write(tmp.rdbuf()->str(), tmp.pcount());
	return out;
}

N_STD::istream &operator>> (N_STD::istream &in,  FlashTagDefineButton2 &data)
{
	N_STD::ostrstream tmp;
	N_STD::streampos start = in.tellg();

	UWORD d, offset;
	READ_UWORD(d);
	data.SetID(d);
	data.menu = (in.get()==1);
	READ_UWORD(offset);
	int i;
	int count = 0;
	for(i = in.get(); i != 0; i=in.get())
	{
		in.putback(i);
		data.buttonrecords.push_back(new FlashButtonRecord());
		data.gcbuttonrecords.push_back(data.buttonrecords[count]);
		data.buttonrecords[count]->SetTagVersion(2);
		in >> *data.buttonrecords[count];
		count ++;
	}

	if(data.importsize + start == (UDWORD)in.tellg())
      return in;

	int count2 = 0;
	do
	{
		READ_UWORD(d);
		UWORD d2;
		READ_UWORD(d2);
		
		N_STD::vector<FlashActionRecord *> x;
		flash_pair<N_STD::vector<FlashActionRecord *>,UWORD> tmp(x,d2);
		data.actionrecords.push_back(tmp);
		
		FlashActionVectorImporter import;
		import.Import(in, data.actionrecords[count2].first, data.gcactionrecords);
		
		count2++;

	} while (d != 0);

	return in;
}

N_STD::ostream &operator<< (N_STD::ostream &out, FlashTagDefineButtonSound &data)
{
// TODO
	return out;
}

N_STD::istream &operator>> (N_STD::istream &in,  FlashTagDefineButtonSound &data)
{
// TODO - to complete
	FlashSoundInfo    soundInfoChar0,soundInfoChar1,soundInfoChar2,soundInfoChar3;
	UWORD d;
	READ_UWORD(d);
	data.SetID(d);

	READ_UWORD(d);    // SoundChar0 Id
	if(d>0)
		in >> soundInfoChar0;

	READ_UWORD(d);    // SoundChar1 Id
	if(d>0)
		in >> soundInfoChar1;

	READ_UWORD(d);    // SoundChar2 Id
	if(d>0)
		in >> soundInfoChar2;

	READ_UWORD(d);    // SoundChar3 Id
	if(d>0)
		in >> soundInfoChar3;
	
	return in;
}
