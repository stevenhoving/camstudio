#include <stdarg.h>
#include "FAction.h"

void FlashActionRecord::WriteHeader(N_STD::ostream &out, UWORD size)
{
	if(m_code > 0x80)
	{
		out.put( (char)m_code);
		WRITE_UWORD(size);
	}
	else
	{
		out.put( (char)m_code);			
	}

}

void FlashActionPush::Write(N_STD::ostream &out)
{
	WriteHeader(out,len+1);
	out.put((char)type);
	out.write(data,len);
}
void FlashActionPush::Read(N_STD::istream &in)
{
	ReadHeader(in);
	type = in.get();
	len = m_size-1;
	data = (char *)malloc(m_size);
	gcstrings.push_back(data);
	in.read(data,m_size-1);
}


FlashActionDefineFunction::FlashActionDefineFunction( char *function ...)
{
		
	m_functionName=function;

	va_list ap;
	va_start(ap, function);

	for(;;)
	{
		char *p=va_arg(ap,char*);
		if(p == 0) break;
		m_paramNames.push_back(p);
	}
	va_end(ap);

	m_numParams = m_paramNames.size();
	
}

FlashActionDefineFunction::~FlashActionDefineFunction()
{
}

void FlashActionDefineFunction::AddAction(FlashActionRecord *r)
{
	m_actions.push_back(r);
}

void FlashActionDefineFunction::Write(N_STD::ostream &out)
{

	out.put((char)0x9B); // FActionDefineFunction tag
	
	int len=0;
	for(N_STD::vector<char*>::iterator i=m_paramNames.begin(); i != m_paramNames.end(); i++)
	{
		len+=strlen(*i)+1;
	}

	WRITE_UWORD(strlen(m_functionName)+1+len+4);
	
	out << m_functionName;
	out.put((char)0);
	
	WRITE_UWORD(m_numParams);
	
	for(N_STD::vector<char*>::iterator is=m_paramNames.begin(); is != m_paramNames.end(); is++)
	{
		out << (*is);
		out.put((char)0);
	}
	N_STD::ostrstream tmp;
	for(N_STD::vector<FlashActionRecord *>::iterator ir = m_actions.begin(); ir != m_actions.end(); ir++)
	{
		(*ir)->Write(tmp);
	}
	WRITE_UWORD(tmp.pcount());
	out.write(tmp.rdbuf()->str(),tmp.pcount());
}
void FlashActionDefineFunction::Read(N_STD::istream &in)
{
	ReadHeader(in);
	
	{
		N_STD::vector<char> str;
		unsigned int i;
		while((i = in.get()) != 0)
		{
			str.push_back((char)i);
		}
		m_functionName = (char*)malloc(str.size()+1);
		m_gcstrings.push_back(m_functionName);
		i=0;
	
		for(;i < str.size(); i++)
		{
			(m_functionName)[i]=str[i];
		}
		(m_functionName)[i]=0;
	}
	
	READ_UWORD(m_numParams);
	
	for(UDWORD it = 0; it < (UDWORD)m_numParams; it++)
	{
		N_STD::vector<char> str;
		unsigned int i;
		while((i = in.get()) != 0)
		{
			str.push_back((char)i);
		}
		char *s = (char*)malloc(str.size()+1);
		m_paramNames.push_back(s);
		m_gcstrings.push_back(s);
		i=0;
	
		for(;i < str.size(); i++)
		{
			(m_paramNames[it])[i]=str[i];
		}
		(m_paramNames[it])[i]=0;
	}

	UWORD actionsBlockSize;
	READ_UWORD(actionsBlockSize);
	
	FlashActionVectorImporter i;
	i.Import(in,m_actions, records_delete, actionsBlockSize);
}

FlashActionConstantPool::FlashActionConstantPool(char *c ...)
{
	va_list ap;
	va_start(ap,c);

	m_length=0;

	char *p=c;
	m_strings.push_back(p);
	m_length+=strlen(p)+1;
	
	for(;;)
	{
		char *p=va_arg(ap,char*);
		if(p == 0) break;
		m_strings.push_back(p);
		m_length+=strlen(p)+1;
	}
	va_end(ap);
	
}
void FlashActionConstantPool::Write(N_STD::ostream &out)
{
	out.put((char)0x88);
	WRITE_UWORD(m_length+2);
	WRITE_UWORD(m_strings.size());
	
	for(N_STD::vector<char*>::iterator is=m_strings.begin(); is != m_strings.end(); is++)
	{
		out << (*is);
		out.put((char)0);
	}

}

void FlashActionConstantPool::Read(N_STD::istream &in)
{
	ReadHeader(in);
	
	UWORD size;
	m_length = m_size - 2;
	READ_UWORD(size);
	
	for(UDWORD it = 0; it < (UDWORD)size; it++)
	{
		N_STD::vector<char> str;
		unsigned int i;
		while((i = in.get()) != 0)
		{
			str.push_back((char)i);
		}
		char *s = (char*)malloc(str.size()+1);
		m_strings.push_back(s);
		m_gcstrings.push_back(s);
		
		i=0;
	
		for(;i < str.size(); i++)
		{
			(m_strings[it])[i]=str[i];
		}
		(m_strings[it])[i]=0;
	}
}

void FlashActionWith::AddAction(FlashActionRecord *r)
{
	m_actions.push_back(r);
}

void FlashActionWith::Write(N_STD::ostream &out)
{
	out.put((char)0x94);		
	N_STD::ostrstream tmp;
	for(N_STD::vector<FlashActionRecord *>::iterator ir = m_actions.begin(); ir != m_actions.end(); ir++)
	{
		(*ir)->Write(tmp);
	}
	WRITE_UWORD(tmp.pcount());
	out.write(tmp.rdbuf()->str(),tmp.pcount());
}
void FlashActionWith::Read(N_STD::istream &in)
{	
	ReadHeader(in);
	FlashActionVectorImporter i;
	i.Import(in,m_actions, records_delete);
}
void FlashTagDoAction::AddAction(FlashActionRecord *r)
{
	records.push_back(r);
}
FlashTagDoAction::~FlashTagDoAction() 
{
}
N_STD::ostream &operator<< (N_STD::ostream &out, FlashTagDoAction &data)
{
	N_STD::ostrstream tmp;
	for(N_STD::vector<FlashActionRecord*>::iterator i=data.records.begin(); i < data.records.end(); i++)
	{
		(*i)->Write(tmp);
	}
	
	out << FlashTagHeader(12, tmp.pcount()+1);
	out.write(tmp.rdbuf()->str(),tmp.pcount());		
	out << (char)0;
	return out;
}
N_STD::istream &operator>> (N_STD::istream &in,  FlashTagDoAction &data)
{
	FlashActionVectorImporter i;
	i.Import(in,data.records, data.records_delete);
	return in;
}
#define IMPORT_ACTION_IF(n,x)					\
		if(i == x)								\
		{										\
			FlashActionRecord *p = new n();     \
			v.push_back(p);				        \
			d.push_back(p);                     \
			(*v[count]).Read(in);				\
		}
#define IMPORT_ACTION_ELSE_IF(n,x)				\
		else IMPORT_ACTION_IF(n,x)

void FlashActionVectorImporter::Import(N_STD::istream &in, N_STD::vector<FlashActionRecord *> &v, gc_vector<FlashActionRecord*> &d)
{
	int i;
	int count = 0;

	for(i = in.get(); i != 0; i=in.get())
	{
		in.putback(i);
		IMPORT_ACTION_IF(FlashActionNextFrame,0x04)
		IMPORT_ACTION_ELSE_IF(FlashActionPreviousFrame,0x05)
		IMPORT_ACTION_ELSE_IF(FlashActionPlay,0x06)
		IMPORT_ACTION_ELSE_IF(FlashActionStop,0x07)
		IMPORT_ACTION_ELSE_IF(FlashActionToggleQuality,0x08)
		IMPORT_ACTION_ELSE_IF(FlashActionStopSounds,0x09)
		IMPORT_ACTION_ELSE_IF(FlashActionGotoFrame,0x81)
		IMPORT_ACTION_ELSE_IF(FlashActionSetTarget,0x8B)
		IMPORT_ACTION_ELSE_IF(FlashActionGotoLabel,0x8C)

		IMPORT_ACTION_ELSE_IF(FlashActionGetURL, 0x83)
		IMPORT_ACTION_ELSE_IF(FlashActionWaitForFrame, 0x8A)
				
		IMPORT_ACTION_ELSE_IF(FlashActionAdd,0x0A)
		IMPORT_ACTION_ELSE_IF(FlashActionSubtract,0x0B)
		IMPORT_ACTION_ELSE_IF(FlashActionMultiply,0x0C)
		IMPORT_ACTION_ELSE_IF(FlashActionDivide,0x0D)
		IMPORT_ACTION_ELSE_IF(FlashActionEquals,0x0E)
		IMPORT_ACTION_ELSE_IF(FlashActionLess,0x0F)
		IMPORT_ACTION_ELSE_IF(FlashActionAnd,0x10)
		IMPORT_ACTION_ELSE_IF(FlashActionOr,0x11)
		IMPORT_ACTION_ELSE_IF(FlashActionNot,0x12)
		IMPORT_ACTION_ELSE_IF(FlashActionStringEquals,0x13)
		IMPORT_ACTION_ELSE_IF(FlashActionStringLength,0x14)
		IMPORT_ACTION_ELSE_IF(FlashActionStringExtract,0x15)
		IMPORT_ACTION_ELSE_IF(FlashActionPop,0x17)
		IMPORT_ACTION_ELSE_IF(FlashActionToInteger,0x18)
		IMPORT_ACTION_ELSE_IF(FlashActionGetVariable,0x1C)
		IMPORT_ACTION_ELSE_IF(FlashActionSetVariable,0x1D)
		IMPORT_ACTION_ELSE_IF(FlashActionSetTarget2,0x20)
		IMPORT_ACTION_ELSE_IF(FlashActionGetProperty,0x22)
		IMPORT_ACTION_ELSE_IF(FlashActionSetProperty,0x23)
		IMPORT_ACTION_ELSE_IF(FlashActionCloneSprite,0x24)
		IMPORT_ACTION_ELSE_IF(FlashActionRemoveSprite,0x25)
		IMPORT_ACTION_ELSE_IF(FlashActionTrace,0x26)
		IMPORT_ACTION_ELSE_IF(FlashActionStartDrag,0x27)
		IMPORT_ACTION_ELSE_IF(FlashActionEndDrag,0x28)
		IMPORT_ACTION_ELSE_IF(FlashActionStringAdd,0x21)
		IMPORT_ACTION_ELSE_IF(FlashActionStringLess,0x29)
		IMPORT_ACTION_ELSE_IF(FlashActionRandomNumber,0x30)
		IMPORT_ACTION_ELSE_IF(FlashActionMBStringLength,0x31)
		IMPORT_ACTION_ELSE_IF(FlashActionCharToAscii,0x32)
		IMPORT_ACTION_ELSE_IF(FlashActionAsciiToChar,0x33)
		IMPORT_ACTION_ELSE_IF(FlashActionGetTime,0x34)
		IMPORT_ACTION_ELSE_IF(FlashActionMBCharToAscii,0x36)
		IMPORT_ACTION_ELSE_IF(FlashActionMBStringExtract,0x35)
		IMPORT_ACTION_ELSE_IF(FlashActionMBAsciToChar,0x37)
		IMPORT_ACTION_ELSE_IF(FlashActionWaitForFrame2,0x8D)
		IMPORT_ACTION_ELSE_IF(FlashActionJump,0x99)
		IMPORT_ACTION_ELSE_IF(FlashActionGetURL2,0x9A)
		IMPORT_ACTION_ELSE_IF(FlashActionIf,0x9D)
		IMPORT_ACTION_ELSE_IF(FlashActionCall,0x9E)
		IMPORT_ACTION_ELSE_IF(FlashActionGotoFrame2,0x9F)

		IMPORT_ACTION_ELSE_IF(FlashActionPush, 0x96) // handle alternate instances??
		
		IMPORT_ACTION_ELSE_IF(FlashActionDelete,0x3a)
		IMPORT_ACTION_ELSE_IF(FlashActionDelete2,0x3b)
		IMPORT_ACTION_ELSE_IF(FlashActionDefineLocal,0x3c)
		IMPORT_ACTION_ELSE_IF(FlashActionCallFunction,0x3d)
		IMPORT_ACTION_ELSE_IF(FlashActionReturn,0x3e)
		IMPORT_ACTION_ELSE_IF(FlashActionModulo,0x3f)
		IMPORT_ACTION_ELSE_IF(FlashActionNewObject,0x40)
		IMPORT_ACTION_ELSE_IF(FlashActionDefineLocal2,0x41)
		IMPORT_ACTION_ELSE_IF(FlashActionInitArray,0x42)
		IMPORT_ACTION_ELSE_IF(FlashActionInitObject,0x4)
		IMPORT_ACTION_ELSE_IF(FlashActionTypeOf,0x44)
		IMPORT_ACTION_ELSE_IF(FlashActionTargetPath,0x45)
		IMPORT_ACTION_ELSE_IF(FlashActionEnumerate,0x46)
		IMPORT_ACTION_ELSE_IF(FlashActionAdd2,0x47)
		IMPORT_ACTION_ELSE_IF(FlashActionLess2,0x48)
		IMPORT_ACTION_ELSE_IF(FlashActionEquals2,0x49)
		IMPORT_ACTION_ELSE_IF(FlashActionToNumber,0x4a)
		IMPORT_ACTION_ELSE_IF(FlashActionToString,0x4b)
		IMPORT_ACTION_ELSE_IF(FlashActionPushDuplicate,0x4c)
		IMPORT_ACTION_ELSE_IF(FlashActionStackSwap,0x4d)
		IMPORT_ACTION_ELSE_IF(FlashActionGetMember,0x4e)
		IMPORT_ACTION_ELSE_IF(FlashActionSetMember,0x4f)
		IMPORT_ACTION_ELSE_IF(FlashActionIncrement,0x50)
		IMPORT_ACTION_ELSE_IF(FlashActionDecrement,0x51)
		IMPORT_ACTION_ELSE_IF(FlashActionCallMethod,0x52)
		IMPORT_ACTION_ELSE_IF(FlashActionNewMethod,0x53)
		IMPORT_ACTION_ELSE_IF(FlashActionBitAnd,0x60)
		IMPORT_ACTION_ELSE_IF(FlashActionBitOr,0x61)
		IMPORT_ACTION_ELSE_IF(FlashActionBitXor,0x62)
		IMPORT_ACTION_ELSE_IF(FlashActionBitLShift,0x63)
		IMPORT_ACTION_ELSE_IF(FlashActionBitRShift,0x64)
		IMPORT_ACTION_ELSE_IF(FlashActionBitURShift,0x65)
		IMPORT_ACTION_ELSE_IF(FlashActionStoreRegister,0x87)

		IMPORT_ACTION_ELSE_IF(FlashActionDefineFunction, 0x9B)
		IMPORT_ACTION_ELSE_IF(FlashActionConstantPool, 0x88)
		IMPORT_ACTION_ELSE_IF(FlashActionWith, 0x94)
		else
		{
			FlashActionRecord *p = new FlashActionRecord();
			v.push_back(p);
			d.push_back(p);
			(*v[count]).Read(in);
		}
		count ++;
	}
	count = 0;
}

void FlashActionVectorImporter::Import(N_STD::istream &in, N_STD::vector<FlashActionRecord *> &v, gc_vector<FlashActionRecord*> &d, UDWORD actionsBlockSize)
{
	int i;
	int count = 0;
	N_STD::streampos start = in.tellg();

	while( (actionsBlockSize + start) > (UDWORD)in.tellg() )
	{
		i = in.get();
		in.putback(i);
		IMPORT_ACTION_IF(FlashActionNextFrame,0x04)
		IMPORT_ACTION_ELSE_IF(FlashActionPreviousFrame,0x05)
		IMPORT_ACTION_ELSE_IF(FlashActionPlay,0x06)
		IMPORT_ACTION_ELSE_IF(FlashActionStop,0x07)
		IMPORT_ACTION_ELSE_IF(FlashActionToggleQuality,0x08)
		IMPORT_ACTION_ELSE_IF(FlashActionStopSounds,0x09)
		IMPORT_ACTION_ELSE_IF(FlashActionGotoFrame,0x81)
		IMPORT_ACTION_ELSE_IF(FlashActionSetTarget,0x8B)
		IMPORT_ACTION_ELSE_IF(FlashActionGotoLabel,0x8C)

		IMPORT_ACTION_ELSE_IF(FlashActionGetURL, 0x83)
		IMPORT_ACTION_ELSE_IF(FlashActionWaitForFrame, 0x8A)
				
		IMPORT_ACTION_ELSE_IF(FlashActionAdd,0x0A)
		IMPORT_ACTION_ELSE_IF(FlashActionSubtract,0x0B)
		IMPORT_ACTION_ELSE_IF(FlashActionMultiply,0x0C)
		IMPORT_ACTION_ELSE_IF(FlashActionDivide,0x0D)
		IMPORT_ACTION_ELSE_IF(FlashActionEquals,0x0E)
		IMPORT_ACTION_ELSE_IF(FlashActionLess,0x0F)
		IMPORT_ACTION_ELSE_IF(FlashActionAnd,0x10)
		IMPORT_ACTION_ELSE_IF(FlashActionOr,0x11)
		IMPORT_ACTION_ELSE_IF(FlashActionNot,0x12)
		IMPORT_ACTION_ELSE_IF(FlashActionStringEquals,0x13)
		IMPORT_ACTION_ELSE_IF(FlashActionStringLength,0x14)
		IMPORT_ACTION_ELSE_IF(FlashActionStringExtract,0x15)
		IMPORT_ACTION_ELSE_IF(FlashActionPop,0x17)
		IMPORT_ACTION_ELSE_IF(FlashActionToInteger,0x18)
		IMPORT_ACTION_ELSE_IF(FlashActionGetVariable,0x1C)
		IMPORT_ACTION_ELSE_IF(FlashActionSetVariable,0x1D)
		IMPORT_ACTION_ELSE_IF(FlashActionSetTarget2,0x20)
		IMPORT_ACTION_ELSE_IF(FlashActionGetProperty,0x22)
		IMPORT_ACTION_ELSE_IF(FlashActionSetProperty,0x23)
		IMPORT_ACTION_ELSE_IF(FlashActionCloneSprite,0x24)
		IMPORT_ACTION_ELSE_IF(FlashActionRemoveSprite,0x25)
		IMPORT_ACTION_ELSE_IF(FlashActionTrace,0x26)
		IMPORT_ACTION_ELSE_IF(FlashActionStartDrag,0x27)
		IMPORT_ACTION_ELSE_IF(FlashActionEndDrag,0x28)
		IMPORT_ACTION_ELSE_IF(FlashActionStringAdd,0x21)
		IMPORT_ACTION_ELSE_IF(FlashActionStringLess,0x29)
		IMPORT_ACTION_ELSE_IF(FlashActionRandomNumber,0x30)
		IMPORT_ACTION_ELSE_IF(FlashActionMBStringLength,0x31)
		IMPORT_ACTION_ELSE_IF(FlashActionCharToAscii,0x32)
		IMPORT_ACTION_ELSE_IF(FlashActionAsciiToChar,0x33)
		IMPORT_ACTION_ELSE_IF(FlashActionGetTime,0x34)
		IMPORT_ACTION_ELSE_IF(FlashActionMBCharToAscii,0x36)
		IMPORT_ACTION_ELSE_IF(FlashActionMBStringExtract,0x35)
		IMPORT_ACTION_ELSE_IF(FlashActionMBAsciToChar,0x37)
		IMPORT_ACTION_ELSE_IF(FlashActionWaitForFrame2,0x8D)
		IMPORT_ACTION_ELSE_IF(FlashActionJump,0x99)
		IMPORT_ACTION_ELSE_IF(FlashActionGetURL2,0x9A)
		IMPORT_ACTION_ELSE_IF(FlashActionIf,0x9D)
		IMPORT_ACTION_ELSE_IF(FlashActionCall,0x9E)
		IMPORT_ACTION_ELSE_IF(FlashActionGotoFrame2,0x9F)

		IMPORT_ACTION_ELSE_IF(FlashActionPush, 0x96) // Not Exactly....handle alternate instances
		
		IMPORT_ACTION_ELSE_IF(FlashActionDelete,0x3a)
		IMPORT_ACTION_ELSE_IF(FlashActionDelete2,0x3b)
		IMPORT_ACTION_ELSE_IF(FlashActionDefineLocal,0x3c)
		IMPORT_ACTION_ELSE_IF(FlashActionCallFunction,0x3d)
		IMPORT_ACTION_ELSE_IF(FlashActionReturn,0x3e)
		IMPORT_ACTION_ELSE_IF(FlashActionModulo,0x3f)
		IMPORT_ACTION_ELSE_IF(FlashActionNewObject,0x40)
		IMPORT_ACTION_ELSE_IF(FlashActionDefineLocal2,0x41)
		IMPORT_ACTION_ELSE_IF(FlashActionInitArray,0x42)
		IMPORT_ACTION_ELSE_IF(FlashActionInitObject,0x4)
		IMPORT_ACTION_ELSE_IF(FlashActionTypeOf,0x44)
		IMPORT_ACTION_ELSE_IF(FlashActionTargetPath,0x45)
		IMPORT_ACTION_ELSE_IF(FlashActionEnumerate,0x46)
		IMPORT_ACTION_ELSE_IF(FlashActionAdd2,0x47)
		IMPORT_ACTION_ELSE_IF(FlashActionLess2,0x48)
		IMPORT_ACTION_ELSE_IF(FlashActionEquals2,0x49)
		IMPORT_ACTION_ELSE_IF(FlashActionToNumber,0x4a)
		IMPORT_ACTION_ELSE_IF(FlashActionToString,0x4b)
		IMPORT_ACTION_ELSE_IF(FlashActionPushDuplicate,0x4c)
		IMPORT_ACTION_ELSE_IF(FlashActionStackSwap,0x4d)
		IMPORT_ACTION_ELSE_IF(FlashActionGetMember,0x4e)
		IMPORT_ACTION_ELSE_IF(FlashActionSetMember,0x4f)
		IMPORT_ACTION_ELSE_IF(FlashActionIncrement,0x50)
		IMPORT_ACTION_ELSE_IF(FlashActionDecrement,0x51)
		IMPORT_ACTION_ELSE_IF(FlashActionCallMethod,0x52)
		IMPORT_ACTION_ELSE_IF(FlashActionNewMethod,0x53)
		IMPORT_ACTION_ELSE_IF(FlashActionBitAnd,0x60)
		IMPORT_ACTION_ELSE_IF(FlashActionBitOr,0x61)
		IMPORT_ACTION_ELSE_IF(FlashActionBitXor,0x62)
		IMPORT_ACTION_ELSE_IF(FlashActionBitLShift,0x63)
		IMPORT_ACTION_ELSE_IF(FlashActionBitRShift,0x64)
		IMPORT_ACTION_ELSE_IF(FlashActionBitURShift,0x65)
		IMPORT_ACTION_ELSE_IF(FlashActionStoreRegister,0x87)

		IMPORT_ACTION_ELSE_IF(FlashActionDefineFunction, 0x9B)
		IMPORT_ACTION_ELSE_IF(FlashActionConstantPool, 0x88)
		IMPORT_ACTION_ELSE_IF(FlashActionWith, 0x94)
		else
		{
			FlashActionRecord *p = new FlashActionRecord();
			v.push_back(p);
			d.push_back(p);
			(*v[count]).Read(in);
		}
		count ++;
	}
	if((actionsBlockSize + start) < (UDWORD)in.tellg())
	{
		UDWORD difference = (UDWORD)in.tellg() - (UDWORD)(actionsBlockSize + start);
		std::cout << "ActionBlock Size Discrepancy: -" << difference << "\n";
//		return false;
	}
	count = 0;
}
