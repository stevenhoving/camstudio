// TypeHandler.h: interface for type-specific handlers.
//
// Copyright (c) GDCL 2004-6. All Rights Reserved. 
// You are free to re-use this as the basis for your own filter development,
// provided you retain this copyright notice in the source.
// http://www.gdcl.co.uk
//////////////////////////////////////////////////////////////////////

#pragma once

class Atom;
class MovieWriter;

// directshow subtypes

class Descriptor
{
public:
    enum TagType
    {
        Invalid = 0,
        ES_Desc = 3,
        Decoder_Config = 4,
        Decoder_Specific_Info = 5,
        SL_Config = 6,
        ES_ID_Inc = 0x0e,
        ES_ID_Ref = 0x0f,
        MP4_IOD = 0x10,
        MP4_OD = 0x11,

        // Command tags
        ObjDescrUpdate = 1,
    };
    Descriptor(TagType type);

    void Append(const BYTE* pBuffer, long cBytes);
    void Append(Descriptor* pdesc);
    long Length();
    void Write(BYTE* pBuffer);
    HRESULT Write(Atom* patm);
private:
    void Reserve(long cBytes);

private:
    TagType m_type;
    long m_cBytes;
    long m_cValid;
    smart_array<BYTE> m_pBuffer;
};

class Atom;

// abstract interface and factory
class TypeHandler  
{
public:
    virtual ~TypeHandler() {}

    virtual DWORD Handler() = 0;
	virtual DWORD DataType()
	{
		return DWORD('mhlr');
	}
    virtual void WriteTREF(Atom* patm) = 0;
    virtual bool IsVideo() = 0;
    virtual bool IsAudio() = 0;
	virtual bool IsOldIndexFormat() { return false; }
	virtual bool IsNonMP4()			{ return IsOldIndexFormat(); }
    virtual void WriteDescriptor(Atom* patm, int id, int dataref, long scale) = 0;
    virtual long SampleRate() = 0;
    virtual long Scale() = 0;
	virtual long Width() = 0;
	virtual long Height() = 0;
	virtual long BlockAlign() { return 1; }
	virtual bool CanTruncate() { return false; }
	virtual bool Truncate(IMediaSample* pSample, REFERENCE_TIME tNewStart) 
	{ 
		UNREFERENCED_PARAMETER(pSample);
		UNREFERENCED_PARAMETER(tNewStart);
		return false; 
	}
	virtual LONGLONG FrameDuration() 
	{
		// default answer
		return UNITS / SampleRate();
	}

	virtual HRESULT WriteData(Atom* patm, const BYTE* pData, int cBytes, int* pcActual);
    static bool CanSupport(const CMediaType* pmt);
    static TypeHandler* Make(const CMediaType* pmt);
};

