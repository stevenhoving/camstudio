#ifndef FACTION_H_FILE
#define FACTION_H_FILE

#include <iostream>
#include <strstream>
#include <vector>

#include "FBase.h"

class FlashActionRecord
{
public:
	FlashActionRecord() : m_size(0), unknowndata(NULL) {}
        virtual ~FlashActionRecord() { }

	virtual void Write(N_STD::ostream &out) { WriteHeader(out,m_size); out.write(unknowndata,m_size); }
	
	virtual void Read(N_STD::istream &in) { ReadHeader(in); if(m_size != 0) { unknowndata = (char *)malloc(m_size+1); gcstrings.push_back(unknowndata); in.read(unknowndata,m_size); } }

	UWORD GetRecordSize()
	{
		N_STD::strstream tmp;
		Write(tmp);
		return tmp.pcount();
	}

	UWORD GetActionCode(void) const { return (m_code); }

protected:
	void WriteHeader(N_STD::ostream &out, UWORD size=0);
	void ReadHeader(N_STD::istream &in)  { m_code = in.get(); if(m_code < 0x80) return; READ_UWORD(m_size); }
	
	char *unknowndata;
	gc_vector<char*> gcstrings;
	
	UWORD m_code;
	UWORD m_size;
};



//ACTION WITH NO PARAMS
#define DECLARE_SIMPLE_ACTION_CLASS(ActionName, ActionID)		\
class ActionName : public FlashActionRecord						\
{																\
public:															\
	ActionName() { m_code=ActionID; }							\
        virtual ~ActionName() {}                                                                                        \
	virtual void Write(N_STD::ostream &out) { WriteHeader(out); }	\
private:														\
};
//ACTION WITH WORD PARAM
#define DECLARE_SIMPLE_ACTION_CLASS2(ActionName, ActionID, pName)	\
class ActionName : public FlashActionRecord							\
{																	\
public:																\
	ActionName(SWORD pName) : data(pName) { m_code=ActionID; }		\
        virtual ~ActionName() {}                                                                                                \
	virtual void Write(N_STD::ostream &out) { WriteHeader(out,2);		\
											WRITE_SWORD(data); }	\
	SWORD Get##pName() { return data; }								\
	virtual void Read(N_STD::istream &in) { ReadHeader(in); READ_SWORD(data); } \
private:															\
	ActionName() {}													\
	friend class FlashActionVectorImporter;						\
	SWORD data;														\
};

//ACTION WITH STRING PARAM
#define DECLARE_SIMPLE_ACTION_CLASS3(ActionName, ActionID, pName)					\
class ActionName : public FlashActionRecord											\
{																					\
public:																				\
        ActionName(char *pName) : data(pName) { m_code=ActionID; }                               \
        virtual ~ActionName() {  }                                                                                                                                \
	virtual void Write(N_STD::ostream &out) { WriteHeader(out,(UWORD)(strlen(data)+1));		\
											out << data; out.put((char)0); }			\
	const char *Get##pName() { return data; }								\
        virtual void Read(N_STD::istream &in) { ReadHeader(in); N_STD::vector<char> str; unsigned int i;     while((i = in.get()) != 0)      { str.push_back((char)i); } data = (char*)malloc(str.size()+1); garbage.push_back(data); i=0; for(;i < str.size(); i++) { data[i]=str[i]; }      data[i]=0; } \
private:																			\
        ActionName() {}                                                                                                     \
	friend class FlashActionVectorImporter;						\
	char *data;																\
        gc_vector<char *> garbage; \
};
//ACTION WITH BYTE PARAM
#define DECLARE_SIMPLE_ACTION_CLASS4(ActionName, ActionID, pName)	\
class ActionName : public FlashActionRecord							\
{																	\
public:																\
	ActionName(unsigned char pName) : data(pName) { m_code=ActionID; }		\
        virtual ~ActionName() {}                                                                                                \
	virtual void Write(N_STD::ostream &out) { WriteHeader(out,1);		\
											out.put(data); }			\
	char Get##pName() { return data; }								\
	virtual void Read(N_STD::istream &in) { ReadHeader(in); data=in.get(); } \
private:															\
	ActionName() {}													\
	friend class FlashActionVectorImporter;						\
	unsigned char data;														\
};

/* FLASH 3.0 SIMPLE ACTIONS */
DECLARE_SIMPLE_ACTION_CLASS(FlashActionNextFrame,0x04);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionPreviousFrame,0x05);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionPlay,0x06);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionStop,0x07);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionToggleQuality,0x08);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionStopSounds,0x09);
DECLARE_SIMPLE_ACTION_CLASS2(FlashActionGotoFrame,0x81,frame);
DECLARE_SIMPLE_ACTION_CLASS3(FlashActionSetTarget,0x8B,target);
DECLARE_SIMPLE_ACTION_CLASS3(FlashActionGotoLabel,0x8C,label);

/* FLASH 3.0 COMPLEX ACTIONS */
class FlashActionGetURL : public FlashActionRecord											
{																					
public:
	FlashActionGetURL(char *url, char *target) : data1(url), data2(target) { m_code=0x83; }
        virtual ~FlashActionGetURL() 
	{ 
	}

	
	virtual void Write(N_STD::ostream &out) { WriteHeader(out,strlen(data1)+strlen(data2)+2);
											out << data1; out.put((char)0);
											out << data2; out.put((char)0);}
	
	const char *GetURL(void) { return data1; }
	const char *GetTarget(void) { return data2; }

	virtual void Read(N_STD::istream &in) 
	{	
		ReadHeader(in);
		N_STD::vector<char> str; 
		unsigned int i;	
		while((i  = in.get()) != 0)	
		{ 
			str.push_back((char)i); 
		} 
		data1 = (char*)malloc(str.size()+1); 
		i=0; 
		for(;i < str.size(); i++) 
		{ 
			data1[i]=str[i]; 
		} 	
		data1[i]=0; 
		
		N_STD::vector<char> str2; 
		unsigned int i2;
		while((i2 = in.get()) != 0)	
		{ 
			str2.push_back((char)i2); 
		} 
		data2 = (char*)malloc(str2.size()+1); 
		i2=0; 
		for(;i2 < str2.size(); i2++) 
		{ 
			data2[i2]=str2[i2]; 
		} 	
		data2[i2]=0;
		gcstrings.push_back(data1);
		gcstrings.push_back(data2);
	}
private:
	gc_vector<char *> gcstrings;
	FlashActionGetURL() {}													
	friend class FlashActionVectorImporter;						
	char *data1;
	char *data2;
};

class FlashActionWaitForFrame : public FlashActionRecord											
{																					
public:
	FlashActionWaitForFrame(SWORD frame, char skipcount) : data1(frame), data2(skipcount) { m_code=0x8A; }					
        virtual ~FlashActionWaitForFrame() {}                                                                                                                            
	virtual void Write(N_STD::ostream &out) { WriteHeader(out,3);
											WRITE_SWORD(data1);
											out.put((char)data2);}
	virtual void Read(N_STD::istream &in) { ReadHeader(in); READ_SWORD(data1); data2 = in.get(); }
	SWORD GetFrame(void) { return data1; }
	char GetSkipCount(void) { return data2; }
private:	
	FlashActionWaitForFrame() {}
	friend class FlashActionVectorImporter;

	SWORD data1;
	char data2;
};

/* FLASH 4.0 SIMPLE ACTIONS */
DECLARE_SIMPLE_ACTION_CLASS(FlashActionAdd,0x0A);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionSubtract,0x0B);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionMultiply,0x0C);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionDivide,0x0D);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionEquals,0x0E);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionLess,0x0F);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionAnd,0x10);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionOr,0x11);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionNot,0x12);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionStringEquals,0x13);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionStringLength,0x14);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionStringExtract,0x15);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionPop,0x17);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionToInteger,0x18);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionGetVariable,0x1C);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionSetVariable,0x1D);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionSetTarget2,0x20);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionGetProperty,0x22);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionSetProperty,0x23);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionCloneSprite,0x24);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionRemoveSprite,0x25);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionTrace,0x26);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionStartDrag,0x27);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionEndDrag,0x28);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionStringAdd,0x21);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionStringLess,0x29);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionRandomNumber,0x30);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionMBStringLength,0x31);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionCharToAscii,0x32);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionAsciiToChar,0x33);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionGetTime,0x34);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionMBCharToAscii,0x36);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionMBStringExtract,0x35);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionMBAsciToChar,0x37);
DECLARE_SIMPLE_ACTION_CLASS4(FlashActionWaitForFrame2,0x8D, skipcount);
DECLARE_SIMPLE_ACTION_CLASS2(FlashActionJump,0x99,offset);
DECLARE_SIMPLE_ACTION_CLASS4(FlashActionGetURL2,0x9A,method);
DECLARE_SIMPLE_ACTION_CLASS2(FlashActionIf,0x9D,offset);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionCall,0x9E);
DECLARE_SIMPLE_ACTION_CLASS4(FlashActionGotoFrame2,0x9F,play);

/* FLASH 4.0 COMPLEX ACTIONS */
class FlashActionPush : public FlashActionRecord
{
public:
	FlashActionPush(char _type, char *_data, UWORD _len) : type(_type), data(_data), len(_len) { m_code = 0x96;}
        virtual ~FlashActionPush() { }

	virtual void Write(N_STD::ostream &out);
	virtual void Read(N_STD::istream &in);

	char GetType() { return type; }
	virtual UWORD GetDataSize() { return len; }
	virtual const char *GetData() { return data; }
protected:
	FlashActionPush() {};
private:
	friend class FlashActionVectorImporter;
	gc_vector<char*> gcstrings;
	char type;
	char *data;
	UWORD len;
};


/* FLASH 5.0 SIMPLE ACTIONS */
DECLARE_SIMPLE_ACTION_CLASS(FlashActionDelete,0x3a);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionDelete2,0x3b);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionDefineLocal,0x3c);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionCallFunction,0x3d);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionReturn,0x3e);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionModulo,0x3f);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionNewObject,0x40);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionDefineLocal2,0x41);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionInitArray,0x42);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionInitObject,0x42);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionTypeOf,0x44);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionTargetPath,0x45);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionEnumerate,0x46);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionAdd2,0x47);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionLess2,0x48);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionEquals2,0x49);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionToNumber,0x4a);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionToString,0x4b);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionPushDuplicate,0x4c);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionStackSwap,0x4d);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionGetMember,0x4e);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionSetMember,0x4f);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionIncrement,0x50);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionDecrement,0x51);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionCallMethod,0x52);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionNewMethod,0x53);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionBitAnd,0x60);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionBitOr,0x61);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionBitXor,0x62);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionBitLShift,0x63);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionBitRShift,0x64);
DECLARE_SIMPLE_ACTION_CLASS(FlashActionBitURShift,0x65);
DECLARE_SIMPLE_ACTION_CLASS4(FlashActionStoreRegister,0x87,registernumber);

/* FLASH 5.0 COMPLEX ACTIONS */

class FlashActionDefineFunction : public FlashActionRecord
{
public:	
	FlashActionDefineFunction(char *function ...); 
		// Function Name, Arg1, Arg2, ... Arg n
        virtual ~FlashActionDefineFunction();
	
	virtual void Write(N_STD::ostream &out);
	virtual void Read(N_STD::istream &in);

	void AddAction(FlashActionRecord *r);

	char* GetFunctionName() { return m_functionName; }
	UWORD GetNumParams() { return m_numParams; }
	char* GetParam(UWORD pos) { return m_paramNames[pos]; }
	UWORD GetNumActions() { return m_actions.size(); }
	FlashActionRecord *GetAction(UWORD pos) { return m_actions[pos]; }

private:
	FlashActionDefineFunction() {}

	friend class FlashActionVectorImporter;
	char *m_functionName;
	UWORD m_numParams;	
	N_STD::vector<char *> m_paramNames;
	N_STD::vector<FlashActionRecord *> m_actions;
	gc_vector<char *> m_gcstrings;
	gc_vector<FlashActionRecord *> records_delete;
};

class FlashActionWith : public FlashActionRecord
{
public:	
	FlashActionWith() {}
        virtual ~FlashActionWith() { }
	
	virtual void Write(N_STD::ostream &out);
	virtual void Read(N_STD::istream &in);

	void AddAction(FlashActionRecord *r);
private:
	N_STD::vector<FlashActionRecord *> m_actions;
	gc_vector<FlashActionRecord *> records_delete;
};

class FlashActionConstantPool : public FlashActionRecord
{
public:
	FlashActionConstantPool(char *c ...);
        virtual ~FlashActionConstantPool() {}

	virtual void Write(N_STD::ostream &out);
	virtual void Read(N_STD::istream &in);

private:
	FlashActionConstantPool() {}
	friend class FlashActionVectorImporter;
	int m_length;
	N_STD::vector<char *>m_strings;
	gc_vector<char *>m_gcstrings;
};

/* Do Action Tag */
class FlashTagDoAction : public FlashSpriteEnabled
{
DEFINE_RW_INTERFACE
public:
	FlashTagDoAction() {}
        virtual ~FlashTagDoAction();

	UWORD GetNumActions() { return records.size(); }
	FlashActionRecord *GetAction(UWORD pos) { return records[pos]; }

	void AddAction(FlashActionRecord *r);

	friend N_STD::ostream &operator<< (N_STD::ostream &out, FlashTagDoAction &data);
	friend N_STD::istream &operator>> (N_STD::istream &in,  FlashTagDoAction &data);

private:
	N_STD::vector<FlashActionRecord *> records;
	gc_vector<FlashActionRecord *> records_delete;
};

class FlashActionVectorImporter
{
public:
	FlashActionVectorImporter() {}
        virtual ~FlashActionVectorImporter() {}

	void Import(N_STD::istream &in, N_STD::vector<FlashActionRecord *> &v, gc_vector<FlashActionRecord*> &d);
	void Import(N_STD::istream &in, N_STD::vector<FlashActionRecord *> &v, gc_vector<FlashActionRecord*> &d, UDWORD actionsBlockSize);
private:
};
#endif  // FACTION_H_FILE

