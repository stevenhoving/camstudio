#ifndef FBITMAP_H_FILE
#define FBITMAP_H_FILE

#include "FBase.h"
#include <iostream>
#include <vector>

class FlashTagDefineBitsJPEG1  : public FlashTag, public FlashIDEnabled
{
DEFINE_RW_INTERFACE
public:
	FlashTagDefineBitsJPEG1() : FlashIDEnabled(false) {}
	FlashTagDefineBitsJPEG1(unsigned char *_data, UDWORD _len) : data(_data), len(_len) {}

	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineBitsJPEG1 &data);
	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineBitsJPEG1 &data);

	const unsigned char *GetJPEGData() { return data; }
	UDWORD GetJPEGDataSize() { return len; }

private:	
	gc_vector<unsigned char *> datagc;
	unsigned char *data;
	UDWORD len;
};

class FlashTagJPEGTables  : public FlashTag
{
DEFINE_RW_INTERFACE
public:
	FlashTagJPEGTables() {}	
	FlashTagJPEGTables(unsigned char *_data, UDWORD _len) : data(_data), len(_len) {}

	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagJPEGTables &data);
	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagJPEGTables &data);

	const unsigned char *GetJPEGData() { return data; }
	UDWORD GetJPEGDataSize() { return len; }

private:	
	unsigned char *data;
	gc_vector<unsigned char *> datagc;
	UDWORD len;
};

class FlashTagDefineBitsJPEG2  : public FlashTag, public FlashIDEnabled
{
DEFINE_RW_INTERFACE
public:
	FlashTagDefineBitsJPEG2() : FlashIDEnabled(false) {}
	FlashTagDefineBitsJPEG2(unsigned char *_JPEGdata, UDWORD _len) : data(_JPEGdata), len(_len) {}

	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineBitsJPEG2 &data);
	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineBitsJPEG2 &data);

	const unsigned char *GetJPEGData() { return data; }
	UDWORD GetJPEGDataSize() { return len; }

private:	
	unsigned char *data;
	UDWORD len;
	gc_vector<unsigned char *> gc;
};

class FlashTagDefineBitsJPEG3  : public FlashTag, public FlashIDEnabled
{
DEFINE_RW_INTERFACE
public:
	FlashTagDefineBitsJPEG3() : FlashIDEnabled(false) {}
	FlashTagDefineBitsJPEG3(unsigned char *_JPEGdata, UDWORD _len, unsigned char *_TRANSdata2, UDWORD _len2) : data(_JPEGdata), len(_len), data2(_TRANSdata2), len2(_len2) {}

	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineBitsJPEG3 &data);
	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineBitsJPEG3 &data);

	const unsigned char *GetJPEGData() { return data; }
	UDWORD GetJPEGDataSize() { return len; }

	const unsigned char *GetAlphaData() { return data2; }
	UDWORD GetAlphaDataSize() { return len2; }

private:	
	unsigned char *data;
	UDWORD len;
	unsigned char *data2;
	UDWORD len2;

	gc_vector<unsigned char *> gc;
};

class FlashZLibBitmapData
{
public:	
	FlashZLibBitmapData() {}
	FlashZLibBitmapData(unsigned char *_data, UDWORD _len) : withalpha(false), data(_data), len(_len) {}
	
	void AddColor(FlashRGB &r) { colors.push_back(r); }
	void SetAlpha(bool a) { withalpha = a; }
	bool GetAlpha() { return withalpha; }
	UDWORD GetSize() { return colors.size(); }

	void Write(N_STD::ostream &out, char format);
	void Read(N_STD::istream &in, char format, UDWORD input_buffer_size, UDWORD size_buffer);

	const unsigned char *GetData() { return data; }
	UDWORD GetDataSize() { return len; }
private:
	friend class FlashTagDefineBitsLossless;
	friend class FlashTagDefineBitsLossless2;
	gc_vector<unsigned char *> gc;
	bool withalpha;
	N_STD::vector<FlashRGB> colors;
	
	unsigned char *data;
        UDWORD len;
};

class FlashTagDefineBitsLossless : public FlashTag, public FlashIDEnabled
{
DEFINE_RW_INTERFACE
public:
	FlashTagDefineBitsLossless() : FlashIDEnabled(false) {}
	FlashTagDefineBitsLossless(char _format, UWORD _width, UWORD _height, FlashZLibBitmapData &_d)
		:	format(_format), width(_width), height(_height),  d(_d) {}

	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineBitsLossless &data);
	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineBitsLossless &data);

	int GetFormat() { return format; }
	UWORD GetWidth() { return width; }
	UWORD GetHeight() { return height; }
	FlashZLibBitmapData &GetData() { return d; }

private:
	char format;
	UWORD width;
	UWORD height;
	FlashZLibBitmapData d;
  
};

class FlashTagDefineBitsLossless2 : public FlashTag, public FlashIDEnabled
{
DEFINE_RW_INTERFACE
public:
	FlashTagDefineBitsLossless2() : FlashIDEnabled(false) {}
	FlashTagDefineBitsLossless2(char _format, UWORD _width, UWORD _height, FlashZLibBitmapData &_d)
		:	format(_format), width(_width), height(_height), d(_d) {}

	friend N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineBitsLossless2 &data);
	friend N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineBitsLossless2 &data);


	int GetFormat() { return format; }
	UWORD GetWidth() { return width; }
	UWORD GetHeight() { return height; }
    FlashZLibBitmapData &GetData() { return d; }

private:
	char format;
	UWORD width;
	UWORD height;
	FlashZLibBitmapData d;
};

#endif
