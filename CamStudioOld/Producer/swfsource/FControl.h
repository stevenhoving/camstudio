#ifndef FCONTROL_H_FILE
#define FCONTROL_H_FILE

#include "FBase.h"
#include "./md5/md5.h"
#include <vector>
#include <string>


class FlashImportHandler;

class FlashTagDefineBitsPtr : public FlashTag
{
DEFINE_RW_INTERFACE
public:
	FlashTagDefineBitsPtr(){};
	FlashTagDefineBitsPtr(UDWORD ptr) : ref(ptr){}
	UDWORD GetPtr() { return ref; }
	void SetPtr(UDWORD ptr) { ref = ptr; }

private:
	UDWORD ref;
	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineBitsPtr &data);
	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineBitsPtr &data);
};

class FlashTagProtect : public FlashTag
{
DEFINE_RW_INTERFACE
public:	
	friend class FlashImportHandler;
	FlashTagProtect() : hasPass(false){}
	FlashTagProtect(N_STD::string& password) : hasPass(false)
	{
		hasPass = true;		
		md5Pass = crypt_md5(password.c_str(), "Aa");
	}
	
	N_STD::string GetEncryptedPassword(){return md5Pass;}
	
	void SetEncryptedPassword(N_STD::string& encPassword){
		hasPass = true;
		md5Pass = encPassword;
	}
	void SetPassword(N_STD::string& password)
	{		
		hasPass = true;				
		md5Pass = crypt_md5(password.c_str(), "Aa");;
	}
	bool HasPassword() { return hasPass; }
private:
	bool hasPass;
	N_STD::string md5Pass;

	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagProtect &data);
	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagProtect &data);
};

class FlashTagEnableDebugger : public FlashTag
{
DEFINE_RW_INTERFACE
public:	
	friend class FlashImportHandler;
	FlashTagEnableDebugger() : hasPass(false){}
	FlashTagEnableDebugger(N_STD::string& password) : hasPass(false)
	{
		hasPass = true;		
		md5Pass = crypt_md5(password.c_str(), "Aa");
	}
	
	N_STD::string GetEncryptedPassword(){return md5Pass;}
	
	void SetEncryptedPassword(N_STD::string& encPassword){
		hasPass = true;
		md5Pass = encPassword;
	}
	void SetPassword(N_STD::string& password)
	{		
		hasPass = true;				
		md5Pass = crypt_md5(password.c_str(), "Aa");;
	}
	bool HasPassword() { return hasPass; }
private:
	bool hasPass;

	N_STD::string md5Pass;
	
	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagEnableDebugger &data);
	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagEnableDebugger &data);
};

class FlashTagBackgroundColor : public FlashTag
{
DEFINE_RW_INTERFACE
public:	
	FlashTagBackgroundColor() {}
	FlashTagBackgroundColor(UBYTE _r, UBYTE _g, UBYTE _b) : r(_r), g(_g), b(_b) {}
	FlashTagBackgroundColor(FlashRGB &c) { r = (UBYTE)c.GetR(); g= (UBYTE)c.GetG(); b = (UBYTE)c.GetB(); }
	~FlashTagBackgroundColor() {}

	FlashRGB GetRGB() { return FlashRGB(r,g,b); }
	
	UBYTE r;
	UBYTE g;
	UBYTE b;

	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagBackgroundColor &data);
	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagBackgroundColor &data);
};

class FlashTagLabelFrame : public FlashSpriteEnabled
{
DEFINE_RW_INTERFACE
public:
	friend class FlashImportHandler;
   FlashTagLabelFrame() : str( NULL ), namedAnchor(false) {}
   FlashTagLabelFrame(char *_str,bool _namedAnchor=false) : str(_str), namedAnchor(_namedAnchor) { gc.push_back(_str); }
   ~FlashTagLabelFrame() {}

	const char *GetLabel() { return str; }
	bool IsNamedAnchor() { return namedAnchor; }

	friend class FlashImporter;

	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagLabelFrame &data);
	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagLabelFrame &data);
	
private:
	char *str;
	bool namedAnchor;		// introduced in FlashMX

	gc_vector<char *> gc;
};

DECLARE_SIMPLE_TAG2(FlashTagEnd)

class FlashTagExportAssets : public FlashTag
{
DEFINE_RW_INTERFACE
public:
	friend class FlashImportHandler;
	FlashTagExportAssets() {}
	~FlashTagExportAssets() {}

	void AddAsset(char *str, UWORD id);

	unsigned int GetAssetNum() { return (unsigned int)assets_str.size(); }
	flash_pair<char*, UWORD> GetAsset(long num) { return(flash_pair<char*,UWORD>(assets_str[num],assets_id[num])); }
	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagExportAssets &data);
	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagExportAssets &data);

private:
	N_STD::vector <char*> assets_str;
	N_STD::vector <UWORD> assets_id;
	gc_vector<char *> gc;
};

class FlashTagImportAssets : public FlashTag
{
DEFINE_RW_INTERFACE
FlashTagImportAssets() {}
public:
	friend class FlashImportHandler;
	FlashTagImportAssets(char *_swfurl) : swfurl(_swfurl) {}
	~FlashTagImportAssets() { }

	void AddAsset(char *str, UWORD id);

	const char *GetSwfUrl() { return swfurl; }
	unsigned int GetAssetNum() { return (unsigned int)assets_str.size(); }
	flash_pair<char*, UWORD> GetAsset(long num) { return(flash_pair<char*,UWORD>(assets_str[num],assets_id[num])); }

	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagImportAssets &data);
	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagImportAssets &data);

private:
	char *swfurl;
	N_STD::vector <char*> assets_str;
	N_STD::vector <UWORD> assets_id;
    gc_vector<char *> gc;
};


//modified by CamStudio v2.27
class FlashTagFreeCharacter : public FlashTag
{
DEFINE_RW_INTERFACE
FlashTagFreeCharacter() {}
public:
	FlashTagFreeCharacter(UWORD _id) { id = _id; }
	//FlashTagFreeCharacter(UWORD _id) { id = _id; }
	//FlashTagFreeCharacter(UDWORD _id) { _id = id; }
	UDWORD GetID(void) const { return id; }

private:
	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagFreeCharacter &data);
	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagFreeCharacter &data);

	//UWORD id;
	UDWORD id;
};

#endif
