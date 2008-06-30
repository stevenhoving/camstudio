#ifndef FSPRITE_H_FILE
#define FSPRITE_H_FILE
#include "FBase.h"

class FlashTagSprite : public FlashIDEnabled, public FlashTag
{
	DEFINE_RW_INTERFACE
public:
	FlashTagSprite() : frames(0) {}
	~FlashTagSprite() {}

	void Add(FlashSpriteEnabled *tag,bool addFrames = true); 

	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagSprite &data);
	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagSprite &data);

	void DeleteChildren()
	{
		for(N_STD::vector<FlashSpriteEnabled *>::iterator i = tags.begin(); i != tags.end(); i++)
		{
			delete *i;
		}
	}
	N_STD::vector<FlashSpriteEnabled *> &GetTags() { return tags; }
	UWORD GetFrameCount(void) { return frames; }

private:

	N_STD::vector<FlashSpriteEnabled *> tags;
	
	FlashIDFactory idFactory;

	gc_vector<FlashSpriteEnabled *> gc;
	UWORD frames;
};

class FlashTagDefineMovie : public FlashIDEnabled, public FlashTag
{
	DEFINE_RW_INTERFACE
public:
	FlashTagDefineMovie() : str(NULL) {}
	FlashTagDefineMovie(char *url) : str(url) { gc.push_back(url); }
	~FlashTagDefineMovie() {}

	const char *GetMovieName() { return str; }

	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineMovie &data);
	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineMovie &data);

private:
	gc_vector<char *> gc;
	char *str;
};

// TODO: Find out about Flash MX tag 60 & tag 61 (embedded Video - Sorensen Video tags) data format

#endif
