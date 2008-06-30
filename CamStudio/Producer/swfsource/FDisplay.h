#ifndef FDISPLAY_H_FILE
#define FDISPLAY_H_FILE

#include <string>
#include "FBase.h"
#include "FShape.h"

class FlashTagPlaceObject : public FlashSpriteEnabled
{
DEFINE_RW_INTERFACE
public:
	FlashTagPlaceObject() {}
	FlashTagPlaceObject(UWORD _depth, UWORD _charID, FlashMatrix &m);
	FlashTagPlaceObject(UWORD _depth, UWORD _charID, FlashMatrix &m, FlashColorTransform &c);

	bool HasColorTransform() { return (hascfx); }

	UWORD GetCharID() { return characterID; }
	UWORD GetDepth()  { return depth; }
	FlashMatrix GetMatrix() { return matrix; } 
	FlashColorTransform GetColorTransform() { return cfx; }

private:
	friend N_STD::ostream& operator<<(N_STD::ostream& out, FlashTagPlaceObject &data);
	friend N_STD::istream& operator>>(N_STD::istream& in, FlashTagPlaceObject &data);

        UWORD characterID;
        UWORD depth;
	FlashMatrix matrix;
        bool hascfx;
	FlashColorTransform cfx;

};

//TODO: Find out Clip Actions Data Format
class FlashTagPlaceObject2 : public FlashSpriteEnabled
{
DEFINE_RW_INTERFACE
public:
	FlashTagPlaceObject2()
	{
		hasName = false;
		hasCharID = false;
		hasMove = false;
		hasMatrix = false;
		hasColorTransform = false;
		hasRatio = false;
		hasClipDepth = false;
		depth = 0;
		charID = 0;
		ratio = 0;
		clipDepth = 0;
// Shouldn't a *transformation* matrix be initialized to Identity?  Currently, scaling is 0.0...
	}
	FlashTagPlaceObject2(UWORD _depth) : hasName(false), hasCharID(false), hasMove(false), hasMatrix(false), hasColorTransform(false), hasRatio(false), hasClipDepth(false), depth(_depth)
	{
		ratio = 0;
		clipDepth = 0;
	}
	FlashTagPlaceObject2(UWORD _depth, UWORD _charID) : hasName(false), hasCharID(true), hasMove(false), hasMatrix(false), hasColorTransform(false), hasRatio(false), hasClipDepth(false), depth(_depth), charID(_charID)
	{
		ratio = 0;
		clipDepth = 0;
	}
	FlashTagPlaceObject2(UWORD _depth, UWORD _charID, FlashMatrix& _matrix) : hasName(false), hasCharID(true), hasMove(false), hasMatrix(true), hasColorTransform(false), hasRatio(false), hasClipDepth(false), depth(_depth)
	{
		charID = _charID;
		matrix = _matrix;
		ratio = 0;
		clipDepth = 0;
	}
	FlashTagPlaceObject2(UWORD _depth, UWORD _charID, FlashMatrix& _matrix, FlashColorTransform& _cfx) : hasName(false), hasCharID(true), hasMove(false), hasMatrix(true), hasColorTransform(true), hasRatio(false), hasClipDepth(false), depth(_depth)
	{
		charID = _charID;
		matrix = _matrix;
		cfx = _cfx;
		ratio = 0;
		clipDepth = 0;
	}
	FlashTagPlaceObject2(UWORD _depth, FlashMatrix& _matrix, FlashColorTransform& _cfx) : hasName(false), hasCharID(false), hasMove(false), hasMatrix(true), hasColorTransform(true), hasRatio(false), hasClipDepth(false), depth(_depth)
	{	
		matrix = _matrix;
		cfx = _cfx;
		ratio = 0;
		clipDepth = 0;
	}
	~FlashTagPlaceObject2(){}

	void SetName(N_STD::string& _name)
	{
		hasName = true;
		name = _name;
	}

	void SetCharID(UWORD _charID)
	{
		hasCharID = true;
		charID = _charID;
	}

	void SetMove(bool _move)
	{
		hasMove = _move;
	}

	void SetMatrix(FlashMatrix& _matrix)
	{
		hasMatrix = true;
		matrix = _matrix;
	}

	void SetColorTransform(FlashColorTransform& _cfx)
	{
		hasColorTransform = true;
		cfx = _cfx;
	}

	void SetRatio(UWORD _ratio) 
	{ 
		hasRatio = true;
		ratio=_ratio; 
	}

	void SetClipDepth(UWORD _clipDepth) 
	{
		hasClipDepth = true;
		clipDepth = _clipDepth;
	}
	void SetDepth( UWORD d )
	{
		depth = d;
	}

	bool HasName() { return hasName; }
	bool HasCharID() { return hasCharID; }
	bool HasMove() { return hasMove; }
	bool HasMatrix() { return hasMatrix; }
	bool HasColorTransform() { return hasColorTransform; }
	bool HasRatio() { return hasRatio; }
	bool HasClipDepth() { return hasClipDepth; }

	UWORD GetDepth()  { return depth; }
	const char* GetName() { return name.c_str(); }
	UWORD GetCharID() { return charID; }	
	FlashMatrix GetMatrix() { return matrix; } 
	FlashColorTransform GetColorTransform() { return cfx; }
	UWORD GetRatio() { return ((UWORD)ratio); }
	UWORD GetClipDepth(){ return clipDepth; }
	

private:

	friend N_STD::ostream& operator<<(N_STD::ostream& out, FlashTagPlaceObject2 &data);
	friend N_STD::istream& operator>>(N_STD::istream& in, FlashTagPlaceObject2 &data);
	

	bool hasName;
	bool hasCharID;
	bool hasMove;
	bool hasMatrix;
	bool hasColorTransform;
	bool hasRatio;
	bool hasClipDepth;
	
	UWORD depth;
	N_STD::string name;
	UWORD charID;
	FlashMatrix matrix;
	FlashColorTransform cfx;	
	UWORD ratio;
	UWORD clipDepth;
};

class FlashTagRemoveObject : public FlashSpriteEnabled
{
DEFINE_RW_INTERFACE
public:
	FlashTagRemoveObject() {}
	FlashTagRemoveObject(UWORD _depth, UWORD _charID) : charID(_charID), depth(_depth){};
	~FlashTagRemoveObject() {}

	UWORD GetCharID() { return charID; }
	UWORD GetDepth()  { return depth; }

private:
	friend N_STD::ostream& operator<<(N_STD::ostream& out, FlashTagRemoveObject &data);
	friend N_STD::istream& operator>>(N_STD::istream& in, FlashTagRemoveObject &data);

	UWORD charID;
	UWORD depth;
};
class FlashTagRemoveObject2 : public FlashSpriteEnabled
{
DEFINE_RW_INTERFACE
public:
	FlashTagRemoveObject2() {}
	FlashTagRemoveObject2(UWORD _depth) : depth(_depth){};
	~FlashTagRemoveObject2() {}

    void SetDepth(UWORD _depth)  { depth = _depth; }
	UWORD GetDepth()  { return depth; }

private:
	friend N_STD::ostream& operator<<(N_STD::ostream& out, FlashTagRemoveObject2 &data);
	friend N_STD::istream& operator>>(N_STD::istream& in, FlashTagRemoveObject2 &data);

	UWORD depth;
};
DECLARE_SIMPLE_TAG3(FlashTagShowFrame)
#endif 
