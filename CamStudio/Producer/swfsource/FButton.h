#ifndef FBUTTON_H_FILE
#define FBUTTON_H_FILE


#include <vector>

#include "FBase.h"
#include "FShape.h"
#include "FSound.h"
#include "FAction.h"

#define FBR_HIT_TEST (1<<3)
#define FBR_DOWN     (1<<2)
#define FBR_OVER     (1<<1)
#define FBR_UP       (1)

// Button State Transitions :
#define SWFSOURCE_BST_OverDownToIdle      (1<<8)
#define SWFSOURCE_BST_IdleToOverDown      (1<<7)
#define SWFSOURCE_BST_OutDownToIdle       (1<<6)
#define SWFSOURCE_BST_OutDownToOverDown   (1<<5)
#define SWFSOURCE_BST_OverDownToOutDown   (1<<4)
#define SWFSOURCE_BST_OverDownToOverUp    (1<<3)
#define SWFSOURCE_BST_OverUpToOverDown    (1<<2)
#define SWFSOURCE_BST_OverUpToIdle        (1<<1)
#define SWFSOURCE_BST_IdleToOverUp        (1)

class FlashTagDefineButton;
class FlashTagDefineButton2;
class FlashTagDefineButtonSound;

class FlashButtonRecord : public FlashVersionEnabled
{
DEFINE_RW_INTERFACE
public:
	FlashButtonRecord()  {}	
	FlashButtonRecord(UWORD _charID, UWORD _depth, char _validstates, FlashMatrix &_matrix,
		FlashColorTransform &_cfx);
	
	friend N_STD::ostream &operator<< (N_STD::ostream &out, FlashButtonRecord &data);
	friend N_STD::istream &operator>> (N_STD::istream &in,  FlashButtonRecord &data);

	char GetValidStates() { return validstates; }
	UWORD GetCharID() { return charID; }
	UWORD GetDepth()  { return depth; }

	FlashMatrix GetMatrix() { return matrix; } 
	FlashColorTransform GetColorTransform() { return cfx; }

private:
	friend N_STD::istream &operator>> (N_STD::istream &in,  FlashTagDefineButton &data);
	friend N_STD::istream &operator>> (N_STD::istream &in,  FlashTagDefineButton2 &data);

	UWORD charID;
	UWORD depth;
	char validstates;
	FlashMatrix matrix;
	FlashColorTransform cfx;
};

class FlashTagDefineButton : public FlashTag, public FlashIDEnabled
{
DEFINE_RW_INTERFACE
public:
    FlashTagDefineButton() {}
	~FlashTagDefineButton() { }
	
	void AddButtonRecord(FlashButtonRecord *r);
	void AddActionRecord(FlashActionRecord *r);

	N_STD::vector<FlashButtonRecord*> &GetButtonRecords() { return buttonrecords; }
	N_STD::vector<FlashActionRecord*> &GetActionRecords() { return actionrecords; }

private:
	friend N_STD::ostream &operator<< (N_STD::ostream &out, FlashTagDefineButton &data);
	friend N_STD::istream &operator>> (N_STD::istream &in,  FlashTagDefineButton &data);

	N_STD::vector<FlashButtonRecord*> buttonrecords;
	N_STD::vector<FlashActionRecord*> actionrecords;

	gc_vector<FlashButtonRecord*> gcbuttonrecords;
	gc_vector<FlashActionRecord*> gcactionrecords;

public:
	void DeleteButtonRecords()
	{
		for(N_STD::vector<FlashButtonRecord *>::iterator i = buttonrecords.begin(); i != buttonrecords.end(); i++)
		{
			delete *i;
		}
	}

};

class FlashTagDefineButton2 : public FlashTag, public FlashIDEnabled
{	
DEFINE_RW_INTERFACE
public:
	FlashTagDefineButton2() {}
	FlashTagDefineButton2(bool _menu) : menu(_menu) {}
	~FlashTagDefineButton2();
	void AddButtonRecord(FlashButtonRecord *r);
	void AddActionRecords(N_STD::vector<FlashActionRecord *> &r, UWORD conditionflags);	
   
	N_STD::vector<FlashButtonRecord*> &GetButtonRecords() { return buttonrecords; }
	N_STD::vector<flash_pair<N_STD::vector<FlashActionRecord*>, UWORD> > &GetActionRecords() { return actionrecords; }

private:
    friend N_STD::ostream &operator<< (N_STD::ostream &out, FlashTagDefineButton2 &data);
	friend N_STD::istream &operator>> (N_STD::istream &in,  FlashTagDefineButton2 &data);

	bool menu;
	N_STD::vector<FlashButtonRecord *> buttonrecords;
	
	N_STD::vector<flash_pair<N_STD::vector<FlashActionRecord*>, UWORD> > actionrecords;

	gc_vector<FlashButtonRecord*> gcbuttonrecords;
	gc_vector<FlashActionRecord*> gcactionrecords;

public:
	void DeleteButtonRecords()
	{
		for(N_STD::vector<FlashButtonRecord *>::iterator i = buttonrecords.begin(); i != buttonrecords.end(); i++)
		{
			delete *i;
		}
	}
    
	void DeleteActionRecords()
	{
		for(N_STD::vector<flash_pair<N_STD::vector<FlashActionRecord*>, UWORD> >::iterator i = actionrecords.begin(); i != actionrecords.end(); i++)
		{
			for(N_STD::vector<FlashActionRecord *>::iterator i2 = (*i).first.begin(); i2 != (*i).first.end(); i2++)
			{
				delete (*i2);
			}
		}
	}

};

class FlashTagDefineButtonSound : public FlashTag, public FlashIDEnabled
{	
DEFINE_RW_INTERFACE
public:
	FlashTagDefineButtonSound() {}
   ~FlashTagDefineButtonSound() {}

private:
	friend N_STD::ostream &operator<< (N_STD::ostream &out, FlashTagDefineButtonSound &data);
	friend N_STD::istream &operator>> (N_STD::istream &in,  FlashTagDefineButtonSound &data);

};


#endif
