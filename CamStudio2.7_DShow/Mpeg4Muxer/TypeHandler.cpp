// TypeHandler.cpp: implementation of type-specific handler classes.
//
// Copyright (c) GDCL 2004-6. All Rights Reserved. 
// You are free to re-use this as the basis for your own filter development,
// provided you retain this copyright notice in the source.
// http://www.gdcl.co.uk
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MovieWriter.h"
#include <dvdmedia.h>
#include <mmreg.h>  // for a-law and u-law G.711 audio types

#include "nalunit.h"
#include "ParseBuffer.h"

void WriteVariable(ULONG val, BYTE* pDest, int cBytes)
{
	for (int i = 0; i < cBytes; i++)
	{
		pDest[i] = BYTE((val >> (8 * (cBytes - (i+1)))) & 0xff);
	}
}

class DivxHandler : public TypeHandler
{
public:
    DivxHandler(const CMediaType* pmt);

    DWORD Handler() 
    {
        return 'vide';
    }
    void WriteTREF(Atom* patm) { UNREFERENCED_PARAMETER(patm); }
    bool IsVideo() 
    {
        return true;
    }
    bool IsAudio()
    { 
        return false;
    }
    long SampleRate()
    {
        // an approximation is sufficient
        return 30;
    }
    // use 90Khz except for audio
    long Scale()
    {
        return 90000;
    }
	long Width();
	long Height();

    void WriteDescriptor(Atom* patm, int id, int dataref, long scale);
	HRESULT WriteData(Atom* patm, const BYTE* pData, int cBytes, int* pcActual);

private:
    CMediaType m_mt;
	smart_array<BYTE> m_pConfig;
	long m_cConfig;
};

class H264Handler : public TypeHandler
{
public:
    H264Handler(const CMediaType* pmt)
    : m_mt(*pmt)
    {}

    DWORD Handler() 
    {
        return 'vide';
    }
    void WriteTREF(Atom* patm) {UNREFERENCED_PARAMETER(patm);}
    bool IsVideo() 
    {
        return true;
    }
    bool IsAudio()
    { 
        return false;
    }
    long SampleRate()
    {
        // an approximation is sufficient
        return 30;
    }
    // use 90Khz except for audio
    long Scale()
    {
        return 90000;
    }
	long Width();
	long Height();

    void WriteDescriptor(Atom* patm, int id, int dataref, long scale);
	LONGLONG FrameDuration();

protected:
    CMediaType m_mt;
};

class H264ByteStreamHandler : public H264Handler
{
public:
	H264ByteStreamHandler(const CMediaType* pmt);

    void WriteDescriptor(Atom* patm, int id, int dataref, long scale);
	LONGLONG FrameDuration();
	HRESULT WriteData(Atom* patm, const BYTE* pData, int cBytes, int* pcActual);

	long Width()	{ return m_cx; }
	long Height()	{ return m_cy; }

	enum { nalunit_length_field = 4 };
private:
	REFERENCE_TIME m_tFrame;
	long m_cx;
	long m_cy;

	ParseBuffer m_ParamSets;		// stores param sets for WriteDescriptor
	bool m_bSPS;
	bool m_bPPS;
};

class FOURCCVideoHandler : public TypeHandler
{
public:
    FOURCCVideoHandler(const CMediaType* pmt)
    : m_mt(*pmt),
	  m_bMJPG(false),
	  m_bProcessMJPG(false)
	{
		FOURCCMap MJPG(DWORD('GPJM'));
		FOURCCMap mjpg(DWORD('gpjm'));
		if ((*pmt->Subtype() == MJPG) ||
			(*pmt->Subtype() == mjpg))
		{
			m_bMJPG = true;
			m_bProcessMJPG = true;
		}
	}

    DWORD Handler() 
    {
        return 'vide';
    }
    void WriteTREF(Atom* patm) { UNREFERENCED_PARAMETER(patm); }
    bool IsVideo() 
    {
        return true;
    }
    bool IsAudio()
    { 
        return false;
    }
    long SampleRate()
    {
        // an approximation is sufficient
        return 30;
    }
    // use 90Khz except for audio
    long Scale()
    {
        return 90000;
    }
	long Width();
	long Height();
    void WriteDescriptor(Atom* patm, int id, int dataref, long scale);
	HRESULT WriteData(Atom* patm, const BYTE* pData, int cBytes, int* pcActual);

	bool IsNonMP4()			{ return m_bMJPG; }

private:
	bool m_bMJPG;
	bool m_bProcessMJPG;	// false if already has APP1
    CMediaType m_mt;
};

class AACHandler : public TypeHandler
{
public:
    AACHandler(const CMediaType* pmt);

    DWORD Handler() 
    {
        return 'soun';
    }
    void WriteTREF(Atom* patm) { UNREFERENCED_PARAMETER(patm); }
    bool IsVideo() 
    {
        return false;
    }
    bool IsAudio()
    { 
        return true;
    }
    long SampleRate()
    {
        return 50;
    }
    long Scale();
	long Width()	{ return 0; }
	long Height()	{ return 0; }
    void WriteDescriptor(Atom* patm, int id, int dataref, long scale);
	HRESULT WriteData(Atom* patm, const BYTE* pData, int cBytes, int* pcActual);

private:
    CMediaType m_mt;
};


// handles some standard WAVEFORMATEX wave formats
class WaveHandler : public TypeHandler
{
public:
    WaveHandler(const CMediaType* pmt)
    : m_mt(*pmt)
    {}

    DWORD Handler() 
    {
        return 'soun';
    }
    void WriteTREF(Atom* patm) {UNREFERENCED_PARAMETER(patm);}
    bool IsVideo() 
    {
        return false;
    }
    bool IsAudio()
    { 
        return true;
    }
    long SampleRate()
    {
        return 50;
    }
	bool CanTruncate();
	bool Truncate(IMediaSample* pSample, REFERENCE_TIME tNewStart);

    long Scale();
	long Width()	{ return 0; }
	long Height()	{ return 0; }
    void WriteDescriptor(Atom* patm, int id, int dataref, long scale);
	bool IsOldIndexFormat();
	long BlockAlign();
private:
    CMediaType m_mt;
};

// CC 608 closed-caption data as a private stream in byte-pair format
class CC608Handler : public TypeHandler
{
public:
    CC608Handler(const CMediaType* pmt)
    : m_mt(*pmt)
    {}

	DWORD DataType()
	{
		return 'dhlr';
	}

    DWORD Handler() 
    {
        return 'text';
    }
    void WriteTREF(Atom* patm) {UNREFERENCED_PARAMETER(patm);}
    bool IsVideo() 
    {
        return false;
    }
    bool IsAudio()
    { 
        return false;
    }
    long SampleRate()
    {
        return 30;
    }
	long Scale()
	{
		return 90000;
	}
	long Width()	{ return 0; }
	long Height()	{ return 0; }
    void WriteDescriptor(Atom* patm, int id, int dataref, long scale);
private:
    CMediaType m_mt;
};


// -----------------------------------------------------------------------------------------

const int WAVE_FORMAT_AAC = 0x00ff;
const int WAVE_FORMAT_AACEncoder = 0x1234;

// Broadcom/Cyberlink Byte-Stream H264 subtype
// CLSID_H264
class DECLSPEC_UUID("8D2D71CB-243F-45E3-B2D8-5FD7967EC09B") CLSID_H264_BSF;

//static 
bool
TypeHandler::CanSupport(const CMediaType* pmt)
{
    if (*pmt->Type() == MEDIATYPE_Video)
    {
        // divx
        FOURCCMap divx(DWORD('xvid'));
        FOURCCMap xvidCaps(DWORD('XVID'));
        FOURCCMap divxCaps(DWORD('DIVX'));
        FOURCCMap dx50(DWORD('05XD'));
        if (((*pmt->Subtype() == divx) || 
			(*pmt->Subtype() == divxCaps) ||
			(*pmt->Subtype() == xvidCaps) ||
			(*pmt->Subtype() == dx50)) 
			    &&
				(*pmt->FormatType() == FORMAT_VideoInfo))
        {
            return true;
        }

		FOURCCMap x264(DWORD('462x'));
		FOURCCMap H264(DWORD('462H'));
		FOURCCMap h264(DWORD('462h'));
		FOURCCMap avc1(DWORD('1CVA'));

		// H264 BSF
		if ((*pmt->Subtype() == x264) || 
			(*pmt->Subtype() == H264) ||
			(*pmt->Subtype() == h264) ||
			(*pmt->Subtype() == avc1) ||
			(*pmt->Subtype() == __uuidof(CLSID_H264_BSF)))
		{
			// BSF
			if ((*pmt->FormatType() == FORMAT_VideoInfo) || (*pmt->FormatType() == FORMAT_VideoInfo2))
			{
				return true;
			}
			// length-prepended
			if (*pmt->FormatType() == FORMAT_MPEG2Video)
			{
				return true;
			}
		}

		// uncompressed
		// it would be nice to select any uncompressed type eg by checking that
		// the bitcount and biSize match up with the dimensions, but that
		// also works for ffdshow encoder outputs, so I'm returning to an 
		// explicit list. 
		FOURCCMap fcc(pmt->subtype.Data1);
		if ((fcc == *pmt->Subtype()) && (*pmt->FormatType() == FORMAT_VideoInfo))
		{
			VIDEOINFOHEADER* pvi = (VIDEOINFOHEADER*)pmt->Format();
			if ((pvi->bmiHeader.biBitCount > 0) && (DIBSIZE(pvi->bmiHeader) == pmt->GetSampleSize()))
			{
				FOURCCMap yuy2(DWORD('2YUY'));
				FOURCCMap uyvy(DWORD('YVYU'));
				FOURCCMap yv12(DWORD('21VY'));
				FOURCCMap nv12(DWORD('21VN'));
				FOURCCMap i420(DWORD('024I'));
				if ((*pmt->Subtype() == yuy2) ||
					(*pmt->Subtype() == uyvy) ||
					(*pmt->Subtype() == yv12) ||
					(*pmt->Subtype() == nv12) ||
//					(*pmt->Subtype() == MEDIASUBTYPE_RGB32) ||
//					(*pmt->Subtype() == MEDIASUBTYPE_RGB24) ||
					(*pmt->Subtype() == i420)
					)
				{
					return true;
				}
			}
			FOURCCMap MJPG(DWORD('GPJM'));
			FOURCCMap jpeg(DWORD('gepj'));
			FOURCCMap mjpg(DWORD('gpjm'));
			if ((*pmt->Subtype() == MJPG) ||
				(*pmt->Subtype() == jpeg) ||
				(*pmt->Subtype() == mjpg))
			{
				return true;
			}
		}
    } else if (*pmt->Type() == MEDIATYPE_Audio)
    {
        // rely on format tag to identify formats -- for 
        // this, subtype adds nothing

        if (*pmt->FormatType() == FORMAT_WaveFormatEx)
        {
            // CoreAAC decoder
            WAVEFORMATEX* pwfx = (WAVEFORMATEX*)pmt->Format();
            if ((pwfx->wFormatTag == WAVE_FORMAT_AAC) || 
                (pwfx->wFormatTag == WAVE_FORMAT_AACEncoder))
            {
                return true;
            }

            if ((pwfx->wFormatTag == WAVE_FORMAT_PCM) ||
                (pwfx->wFormatTag == WAVE_FORMAT_ALAW) ||
                (pwfx->wFormatTag == WAVE_FORMAT_MULAW))
            {
                return true;
            }

			// Intel Media SDK uses the 0xFF- aac subtype guid, but
			// the wFormatTag does not match
			FOURCCMap aac(WAVE_FORMAT_AAC);
			if (*pmt->Subtype() == aac)
			{
				return true;
			}
        }
    }
	else if ((*pmt->Type() == MEDIATYPE_AUXLine21Data) &&
		(*pmt->Subtype() == MEDIASUBTYPE_Line21_BytePair))
	{
		return true;
	}
    return false;
}

//static 
TypeHandler* 
TypeHandler::Make(const CMediaType* pmt)
{
    if (!CanSupport(pmt))
    {
        return NULL;
    }
    if (*pmt->Type() == MEDIATYPE_Video)
    {
        // divx
        FOURCCMap divx(DWORD('xvid'));
        FOURCCMap xvidCaps(DWORD('XVID'));
        FOURCCMap divxCaps(DWORD('DIVX'));
        FOURCCMap dx50(DWORD('05XD'));
        if ((*pmt->Subtype() == divx) || 
			(*pmt->Subtype() == divxCaps) ||
			(*pmt->Subtype() == xvidCaps) ||
			(*pmt->Subtype() == dx50)) 
        {
            return new DivxHandler(pmt);
        }

		FOURCCMap x264(DWORD('462x'));
		FOURCCMap H264(DWORD('462H'));
		FOURCCMap h264(DWORD('462h'));
		FOURCCMap avc1(DWORD('1CVA'));

		// H264
		if ((*pmt->Subtype() == x264) || 
			(*pmt->Subtype() == H264) ||
			(*pmt->Subtype() == h264) ||
			(*pmt->Subtype() == avc1) ||
			(*pmt->Subtype() == __uuidof(CLSID_H264_BSF)))
		{
			// BSF
			if ((*pmt->FormatType() == FORMAT_VideoInfo) || (*pmt->FormatType() == FORMAT_VideoInfo2))
			{
				return new H264ByteStreamHandler(pmt);
			}
			// length-prepended
			if (*pmt->FormatType() == FORMAT_MPEG2Video)
			{
				// check that the length field is 1-4. This is stored in dwFlags
				MPEG2VIDEOINFO* pvi = (MPEG2VIDEOINFO*)pmt->Format();
				if ((pvi->dwFlags < 1) || (pvi->dwFlags > 4))
				{
					// this is not MP4 format. 
					return new H264ByteStreamHandler(pmt);
				}
	            return new H264Handler(pmt);
			}
		}

		// other: uncompressed (checked in CanSupport)
		FOURCCMap fcc(pmt->subtype.Data1);
		if ((fcc == *pmt->Subtype()) && (*pmt->FormatType() == FORMAT_VideoInfo))
		{
			VIDEOINFOHEADER* pvi = (VIDEOINFOHEADER*)pmt->Format();
			if ((pvi->bmiHeader.biBitCount > 0) && (DIBSIZE(pvi->bmiHeader) == pmt->GetSampleSize()))
			{
				return new FOURCCVideoHandler(pmt);
			}
			FOURCCMap MJPG(DWORD('GPJM'));
			FOURCCMap jpeg(DWORD('gepj'));
			FOURCCMap mjpg(DWORD('gpjm'));
			if ((*pmt->Subtype() == MJPG) ||
				(*pmt->Subtype() == jpeg) ||
				(*pmt->Subtype() == mjpg))
			{
				return new FOURCCVideoHandler(pmt);
			}
		}

    } else if (*pmt->Type() == MEDIATYPE_Audio)
    {
        // rely on format tag to identify formats -- for 
        // this, subtype adds nothing

        if (*pmt->FormatType() == FORMAT_WaveFormatEx)
        {
            // CoreAAC decoder
            WAVEFORMATEX* pwfx = (WAVEFORMATEX*)pmt->Format();
            if ((pwfx->wFormatTag == WAVE_FORMAT_AAC) || 
                (pwfx->wFormatTag == WAVE_FORMAT_AACEncoder))
            {
                return new AACHandler(pmt);
            }

            if ((pwfx->wFormatTag == WAVE_FORMAT_PCM) ||
                (pwfx->wFormatTag == WAVE_FORMAT_ALAW) ||
                (pwfx->wFormatTag == WAVE_FORMAT_MULAW))
            {
                return new WaveHandler(pmt);
            }
			// Intel Media SDK uses the 0xFF- aac subtype guid, but
			// the wFormatTag does not match
			FOURCCMap aac(WAVE_FORMAT_AAC);
			if (*pmt->Subtype() == aac)
			{
				return new AACHandler(pmt);
			}
        }
    }
	else if ((*pmt->Type() == MEDIATYPE_AUXLine21Data) &&
		(*pmt->Subtype() == MEDIASUBTYPE_Line21_BytePair))
	{
		return new CC608Handler(pmt);
	}
    return NULL;
}

HRESULT 
TypeHandler::WriteData(Atom* patm, const BYTE* pData, int cBytes, int* pcActual)
{
	*pcActual = cBytes;
	return patm->Append(pData, cBytes);
}


// -------------------------------------------
DivxHandler::DivxHandler(const CMediaType* pmt)
: m_mt(*pmt),
  m_cConfig(0)
{
	if ((*m_mt.FormatType() == FORMAT_VideoInfo) && 
		(m_mt.FormatLength() > sizeof(VIDEOINFOHEADER)))
	{
		m_cConfig = m_mt.FormatLength() - sizeof(VIDEOINFOHEADER);
		m_pConfig = new BYTE[m_cConfig];
		const BYTE* pExtra = m_mt.Format() + sizeof(VIDEOINFOHEADER);
		CopyMemory(m_pConfig, pExtra, m_cConfig);
	}
}

long 
DivxHandler::Width()
{
    VIDEOINFOHEADER* pvi = (VIDEOINFOHEADER*)m_mt.Format();
	return pvi->bmiHeader.biWidth;
}

long 
DivxHandler::Height()
{
    VIDEOINFOHEADER* pvi = (VIDEOINFOHEADER*)m_mt.Format();
	return abs(pvi->bmiHeader.biHeight);
}

void 
DivxHandler::WriteDescriptor(Atom* patm, int id, int dataref, long scale)
{
    UNREFERENCED_PARAMETER(scale);
    smart_ptr<Atom> psd = patm->CreateAtom('mp4v');

    VIDEOINFOHEADER* pvi = (VIDEOINFOHEADER*)m_mt.Format();
    int width = pvi->bmiHeader.biWidth;
    int height = abs(pvi->bmiHeader.biHeight);

    BYTE b[78];
    ZeroMemory(b, 78);
    WriteShort(dataref, b+6);
    WriteShort(width, b+24);
    WriteShort(height, b+26);
    b[29] = 0x48;
    b[33] = 0x48;
    b[41] = 1;
    b[75] = 24;
    WriteShort(-1, b+76);
    psd->Append(b, 78);

    smart_ptr<Atom> pesd = psd->CreateAtom('esds');
    WriteLong(0, b);        // ver/flags
    pesd->Append(b, 4);

    // es descr
    //      decoder config
    //          <objtype/stream type/bitrates>
    //          decoder specific info desc
    //      sl descriptor
    Descriptor es(Descriptor::ES_Desc);
    WriteShort(id, b);
    b[2] = 0;
    es.Append(b, 3);
    Descriptor dcfg(Descriptor::Decoder_Config);
    b[0] = 0x20;    //mpeg-4 video
    b[1] = (4 << 2) | 1;    // video stream

    // buffer size 15000
    b[2] = 0;
    b[3] = 0x3a;
    b[4] = 0x98;
    WriteLong(1500000, b+5);    // max bitrate
    WriteLong(0, b+9);          // avg bitrate 0 = variable
    dcfg.Append(b, 13);
    Descriptor dsi(Descriptor::Decoder_Specific_Info);

	dsi.Append(m_pConfig, m_cConfig);
    dcfg.Append(&dsi);
    es.Append(&dcfg);
    Descriptor sl(Descriptor::SL_Config);
    b[0] = 2;
    sl.Append(b, 2);
    es.Append(&sl);
    es.Write(pesd);
    pesd->Close();

    psd->Close();
}

inline bool NextStartCode(const BYTE*&pBuffer, long& cBytes)
{
    while ((cBytes >= 4) &&
           (*(UNALIGNED DWORD *)pBuffer & 0x00FFFFFF) != 0x00010000) {
        cBytes--;
        pBuffer++;
    }
    return cBytes >= 4;
}

HRESULT 
DivxHandler::WriteData(Atom* patm, const BYTE* pData, int cBytes, int* pcActual)
{
	if (m_cConfig == 0)
	{
		const BYTE* p = pData;
		long c = cBytes;
		const BYTE* pVOL = NULL;
		while (NextStartCode(p, c))
		{
			if (pVOL == NULL)
			{
				if (p[3] == 0x20)
				{
					pVOL = p;
				}
			}
			else
			{
				m_cConfig = long(p - pVOL);
				m_pConfig = new BYTE[m_cConfig];
				CopyMemory(m_pConfig, pVOL, m_cConfig);
				break;
			}
			p += 4;
			c -= 4;
		}
	}
	return __super::WriteData(patm, pData, cBytes, pcActual);
}

long 
AACHandler::Scale()
{
    // for audio, the scale should be the sampling rate but
    // must not exceed 65535
    WAVEFORMATEX* pwfx = (WAVEFORMATEX*)m_mt.Format();
    if (pwfx->nSamplesPerSec > 65535)
    {
        return 45000;
    }
    else
    {
        return pwfx->nSamplesPerSec;
    }
}

AACHandler::AACHandler(const CMediaType* pmt)
: m_mt(*pmt)
{
	// the Intel encoder uses a tag that doesn't match the subtype
	FOURCCMap aac(WAVE_FORMAT_AAC);
	if ((*m_mt.Subtype() == aac) &&
		(*m_mt.FormatType() == FORMAT_WaveFormatEx))
	{
		WAVEFORMATEX* pwfx = (WAVEFORMATEX*)m_mt.Format();
		pwfx->wFormatTag = WAVE_FORMAT_AAC;
	}
}

HRESULT 
AACHandler::WriteData(Atom* patm, const BYTE* pData, int cBytes, int* pcActual)
{
	if ((cBytes > 7) && (pData[0] == 0xff) && ((pData[1] & 0xF0) == 0xF0))
	{
		if (m_mt.FormatLength() == sizeof(WAVEFORMATEX))
		{
			int len = ((pData[3] & 0x3) << 11) + (pData[4] << 3) + ((pData[5] >> 5) & 0x7);
			if (len == cBytes)
			{
				int header = 7;
				if ((pData[1] & 1) == 0) 
				{
					header = 9;
				}
				pData += header;
				cBytes -= header;
			}
		}
	}
	return __super::WriteData(patm, pData, cBytes, pcActual);
}

const DWORD AACSamplingFrequencies[] = 
{
    96000, 
	88200, 
	64000, 
	48000, 
	44100, 
	32000,
    24000, 
	22050, 
	16000, 
	12000, 
	11025, 
	8000, 
	7350,
	0,
	0,
	0,
};
#ifndef SIZEOF_ARRAY
#define SIZEOF_ARRAY(a) (sizeof(a) / sizeof(a[0]))
#endif

void 
AACHandler::WriteDescriptor(Atom* patm, int id, int dataref, long scale)
{
    smart_ptr<Atom> psd = patm->CreateAtom('mp4a');

    BYTE b[28];
    ZeroMemory(b, 28);
    WriteShort(dataref, b+6);
    WriteShort(2, b+16);
    WriteShort(16, b+18);
    WriteShort(unsigned short(scale), b+24);    // this is what forces us to use short audio scales
    psd->Append(b, 28);

    smart_ptr<Atom> pesd = psd->CreateAtom('esds');
    WriteLong(0, b);        // ver/flags
    pesd->Append(b, 4);
    // es descr
    //      decoder config
    //          <objtype/stream type/bitrates>
    //          decoder specific info desc
    //      sl descriptor
    Descriptor es(Descriptor::ES_Desc);
    WriteShort(id, b);
    b[2] = 0;
    es.Append(b, 3);
    Descriptor dcfg(Descriptor::Decoder_Config);
    b[0] = 0x40;    // AAC audio
    b[1] = (5 << 2) | 1;    // audio stream

    // buffer size 15000
    b[2] = 0;
    b[3] = 0x3a;
    b[4] = 0x98;
    WriteLong(1500000, b+5);    // max bitrate
    WriteLong(0, b+9);          // avg bitrate 0 = variable
    dcfg.Append(b, 13);
    Descriptor dsi(Descriptor::Decoder_Specific_Info);
    BYTE* pExtra = m_mt.Format() + sizeof(WAVEFORMATEX);
    long cExtra = m_mt.FormatLength() - sizeof(WAVEFORMATEX);
	if (cExtra == 0)
	{
		long ObjectType = 2;
		WAVEFORMATEX* pwfx = (WAVEFORMATEX*)m_mt.Format();
		long ChannelIndex = pwfx->nChannels;
		if (ChannelIndex == 8)
		{
			ChannelIndex = 7;
		}
		long RateIndex = 0;
		for (long i =0; i < SIZEOF_ARRAY(AACSamplingFrequencies); i++)
		{
			if (AACSamplingFrequencies[i] == pwfx->nSamplesPerSec)
			{
				RateIndex = i;
				break;
			}
		}
		BYTE b[2];
		b[0] = (BYTE) ((ObjectType << 3) | ((RateIndex >> 1) & 7));
		b[1] = (BYTE) (((RateIndex & 1) << 7) | (ChannelIndex << 3));
		dsi.Append(b, 2);
	}
    if (cExtra > 0)
    {
        dsi.Append(pExtra, cExtra);
	}
	dcfg.Append(&dsi);
    es.Append(&dcfg);
    Descriptor sl(Descriptor::SL_Config);
    b[0] = 2;
    sl.Append(b, 2);
    es.Append(&sl);
    es.Write(pesd);
    pesd->Close();
    psd->Close();
}
	
LONGLONG 
H264Handler::FrameDuration()
{
	MPEG2VIDEOINFO* pvi = (MPEG2VIDEOINFO*)m_mt.Format();
	return pvi->hdr.AvgTimePerFrame;
}

long 
H264Handler::Width()
{
	MPEG2VIDEOINFO* pvi = (MPEG2VIDEOINFO*)m_mt.Format();
	return pvi->hdr.bmiHeader.biWidth;
}

long 
H264Handler::Height()
{
	MPEG2VIDEOINFO* pvi = (MPEG2VIDEOINFO*)m_mt.Format();
	return abs(pvi->hdr.bmiHeader.biHeight);
}

void 
H264Handler::WriteDescriptor(Atom* patm, int id, int dataref, long scale)
{
    UNREFERENCED_PARAMETER(scale);
    UNREFERENCED_PARAMETER(id);
    smart_ptr<Atom> psd = patm->CreateAtom('avc1');

	MPEG2VIDEOINFO* pvi = (MPEG2VIDEOINFO*)m_mt.Format();
    int width = pvi->hdr.bmiHeader.biWidth;
    int height = abs(pvi->hdr.bmiHeader.biHeight);


    BYTE b[78];
    ZeroMemory(b, 78);
    WriteShort(dataref, b+6);
    WriteShort(width, b+24);
    WriteShort(height, b+26);
    b[29] = 0x48;
    b[33] = 0x48;
    b[41] = 1;
    b[75] = 24;
    WriteShort(-1, b+76);
    psd->Append(b, 78);

    smart_ptr<Atom> pesd = psd->CreateAtom('avcC');
    b[0] = 1;           // version 1
    b[1] = (BYTE)pvi->dwProfile;
    b[2] = 0;
    b[3] = (BYTE)pvi->dwLevel;
    // length of length-preceded nalus
    b[4] = BYTE(0xfC | (pvi->dwFlags - 1));
    b[5] = 0xe1;        // 1 SPS

    // SPS
    const BYTE* p = (const BYTE*)&pvi->dwSequenceHeader;
    const BYTE* pEnd = p + pvi->cbSequenceHeader;
    int c = (p[0] << 8) | p[1];
    // extract profile/level compat from SPS
    b[2] = p[4];
    pesd->Append(b, 6);
    pesd->Append(p, 2+c);
    int type = p[2] & 0x1f;
    while ((p < pEnd) && (type != 8))
    {
        p += 2+c;
        c = (p[0] << 8) | p[1];
        type = p[2] & 0x1f;
    }
    if ((type == 8) && ((p+2+c) <= pEnd))
    {
        b[0] = 1;   // 1 PPS
        pesd->Append(b, 1);
        pesd->Append(p, 2+c);
    }
    pesd->Close();

    psd->Close();
}

#pragma pack(push, 1)
struct QTVideo 
{
	BYTE	reserved1[6];		// 0
	USHORT	dataref;
	
	USHORT	version;			// 0
	USHORT	revision;			// 0
	ULONG	vendor;

	ULONG	temporal_compression;
	ULONG	spatial_compression;

	USHORT	width;
	USHORT	height;
	
	ULONG	horz_resolution;	// 00 48 00 00
	ULONG	vert_resolution;	// 00 48 00 00
	ULONG	reserved2;			// 0
	USHORT	frames_per_sample;	// 1
	BYTE	codec_name[32];		// pascal string - ascii, first byte is char count
	USHORT	bit_depth;
	USHORT	colour_table_id;		// ff ff
};

inline USHORT Swap2Bytes(int x)
{
	return (USHORT) (((x & 0xff) << 8) | ((x >> 8) & 0xff));
}
inline DWORD Swap4Bytes(DWORD x)
{
	return ((x & 0xff) << 24) |
		   ((x & 0xff00) << 8) |
		   ((x & 0xff0000) >> 8) |
		   ((x >> 24) & 0xff);
}

long 
FOURCCVideoHandler::Width()
{
	if (*m_mt.FormatType() == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER* pvi = (VIDEOINFOHEADER*)m_mt.Format();
		return pvi->bmiHeader.biWidth;
	}
	else if (*m_mt.FormatType() == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2* pvi = (VIDEOINFOHEADER2*)m_mt.Format();
		return pvi->bmiHeader.biWidth;
	}
	else
	{
		return 0;
	}
}

long 
FOURCCVideoHandler::Height()
{
	if (*m_mt.FormatType() == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER* pvi = (VIDEOINFOHEADER*)m_mt.Format();
		return abs(pvi->bmiHeader.biHeight);
	}
	else if (*m_mt.FormatType() == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2* pvi = (VIDEOINFOHEADER2*)m_mt.Format();
		return abs(pvi->bmiHeader.biHeight);
	}
	else
	{
		return 0;
	}
}

BYTE DefaultHuffTable[] = 
{
    0xff, 0xc4, 0x01, 0xa2,

    0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,

    0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,

    0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7d,
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
    0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa,

    0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77,
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
    0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
    0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
    0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa
};
const int DefaultHuffSize = sizeof(DefaultHuffTable);

void
FOURCCVideoHandler::WriteDescriptor(Atom* patm, int id, int dataref, long scale)
{
	UNREFERENCED_PARAMETER(scale);
	UNREFERENCED_PARAMETER(dataref);
	UNREFERENCED_PARAMETER(id);

	FOURCCMap fcc = m_mt.Subtype();
	DWORD codec = Swap4Bytes(fcc.GetFOURCC());
	if (m_bMJPG)
	{
		codec = DWORD('mjpa');
	}
	else if (codec == DWORD('MJPG'))
	{
		// we didn't need the APP1 insertion,
		// so call it Photo JPEG.
		codec = DWORD('jpeg');
	}

	smart_ptr<Atom> psd = patm->CreateAtom(codec);

	int cx, cy, depth;
	if (*m_mt.FormatType() == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER* pvi = (VIDEOINFOHEADER*)m_mt.Format();
		cx = pvi->bmiHeader.biWidth;
		cy = abs(pvi->bmiHeader.biHeight);
		depth = pvi->bmiHeader.biBitCount;
	}
	else if (*m_mt.FormatType() == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2* pvi = (VIDEOINFOHEADER2*)m_mt.Format();
		cx = pvi->bmiHeader.biWidth;
		cy = abs(pvi->bmiHeader.biHeight);
		depth = pvi->bmiHeader.biBitCount;
	}
	else
	{
		return;
	}

	QTVideo fmt;
	ZeroMemory(&fmt, sizeof(fmt));
	// remember we must byte-swap all data
	fmt.width = Swap2Bytes(cx);
	fmt.height = Swap2Bytes(cy);
	fmt.bit_depth = Swap2Bytes(depth);

	fmt.dataref = Swap2Bytes(1);
	fmt.vert_resolution = fmt.horz_resolution = Swap4Bytes(0x00480000);
	fmt.frames_per_sample  = Swap2Bytes(1);
	fmt.colour_table_id = 0xffff;

	// pascal string codec name
	const char* pName;
	if (m_bMJPG)
	{
		pName = "Motion JPEG";
		fmt.spatial_compression = Swap4Bytes(512);
	}
	else if (codec == DWORD('jpeg'))
	{
		pName = "Photo JPEG";
	}
	else if (codec == 0)
	{
		pName = "RGB Video";
	}
	else
	{
		pName = "YUV Video";
	}
	int cch = lstrlenA(pName);
	CopyMemory(&fmt.codec_name[1], pName, cch);
	fmt.codec_name[0] = (BYTE)cch;

	psd->Append((const BYTE*)&fmt, sizeof(fmt));
	if (m_bMJPG)
	{
		smart_ptr<Atom> pfiel = psd->CreateAtom(DWORD('fiel'));
		BYTE b[] = {2, 1};
		pfiel->Append(b, sizeof(b));
		pfiel->Close();
	}

	psd->Close();
}

// Quicktime and FCP require an APP1 marker for dual-field.
// Blackmagic and Microsoft decoders require APP0. 
// The Blackmagic decoder creates APP0.
// We will insert both and fix up one if present.
// If neither are present, finding the end of the image is slow.

// big-endian values.
struct APP1
{
	WORD	marker;
	WORD	length;
	DWORD	reserved;
	DWORD	tag;
	DWORD	fieldsize;
	DWORD	paddedsize;
	DWORD	nextfield;
	DWORD	quantoffset;
	DWORD	huffoffset;
	DWORD	sofoffset;
	DWORD	sosoffset;
	DWORD	dataoffset;
};

struct APP0
{
	WORD marker;
	WORD length;
	DWORD tag;
	BYTE polarity;
	BYTE reserved;
	DWORD paddedsize;
	DWORD fieldsize;
};

HRESULT FOURCCVideoHandler::WriteData(Atom* patm, const BYTE* pData, int cBytes, int* pcActual)
{
	if (!m_bProcessMJPG)
	{
		return __super::WriteData(patm, pData, cBytes, pcActual);
	}
	if ((cBytes < 2) || (pData[0] != 0xff) || (pData[1] != 0xD8))
	{
		return VFW_E_INVALIDMEDIATYPE;
	}
	
	APP1 header;
	ZeroMemory(&header, sizeof(header));
	header.marker = Swap2Bytes(0xffe1);
	header.length = Swap2Bytes(sizeof(APP1) - 2);
	header.tag = Swap4Bytes(DWORD('mjpg'));

	APP0 app0;
	ZeroMemory(&app0, sizeof(app0));
	app0.marker = Swap2Bytes(0xffe0);
	app0.length = Swap2Bytes(sizeof(app0) - 2);
	app0.tag = Swap4Bytes(DWORD('AVI1'));
	app0.polarity = 1;
	
	int total = 0;
	const BYTE* fieldstart = pData;
	int cBytesTotal = cBytes;

	// point at which we insert APP0 and/or APP1.
	const BYTE* pInsertBefore = 0;

	// we can use the alternate variant to find the EOI, 
	// and in any case we need to adjust for insertion
	APP0* pAPP0 = NULL;
	APP1* pAPP1 = NULL;

	while (cBytes > 0)
	{
		int markerlen = 1;
		if (*pData == 0xFF)
		{
			if (cBytes < 2)
			{
				return VFW_E_BUFFER_UNDERFLOW;
			} 
			if (pData[1] == 0xff)
			{
				markerlen = 1;
			}
			else if (pData[1] == 0)
			{
				markerlen = 2;
			}
			else
			{
				BYTE m = pData[1];
				markerlen = 2;
				if ((m < 0xD0) || (m > 0xD9))
				{
					if (cBytes < 4)
					{
						return VFW_E_BUFFER_UNDERFLOW;
					}
					markerlen += (pData[2] << 8) | pData[3];
				}

				// if there's an existing APP0 or APP1,
				// we may need to fix it up.
				if (m == 0xE0)
				{
					pInsertBefore = pData + markerlen;
					if (memcmp(pData+4, "AVI1", 4) == 0)
					{
						pAPP0 = (APP0*) pData;
					}
				}
				if (m == 0xE1)
				{
					pInsertBefore = pData;
					if (memcmp(pData + 8, "mjpg", 4) == 0)
					{
						pAPP1 = (APP1*) pData;
					}
				}
				
				int offset = int(pData - fieldstart);
				if (m == 0xC0)
				{
					header.sofoffset = Swap4Bytes(offset);
				}
				else if (m == 0xC4)
				{
					header.huffoffset = Swap4Bytes(offset);
				}
				else if (m == 0xDB)
				{
					header.quantoffset = Swap4Bytes(offset);
				}
				else if (m == 0xDA)
				{
					header.sosoffset = Swap4Bytes(offset);
					header.dataoffset = Swap4Bytes(offset + markerlen);
					// if either APPx header is present, we don't need to
					// slowly scan the entire frame
					if (pAPP0 != NULL)
					{
						DWORD oldlen = Swap4Bytes(pAPP0->fieldsize);
						const BYTE* EOI = fieldstart + oldlen - 2;
						markerlen = int(EOI - pData);
					}
					else if (pAPP1 != NULL)
					{
						DWORD oldlen = Swap4Bytes(pAPP1->fieldsize);
						const BYTE* EOI = fieldstart + oldlen - 2;
						markerlen = int(EOI - pData);
					}
				}
				else if (m == 0xD9)
				{
					header.fieldsize = Swap4Bytes(offset + markerlen);
				}
				if ((m == 0xD8) || (cBytes == markerlen))
				{
					if (pData != fieldstart)
					{
						bool bInsertHuff = false;
						int bytesInserted = 0;
						if (header.huffoffset == 0)
						{
							// will need to insert a default
							bInsertHuff = true;
							bytesInserted = DefaultHuffSize;
						}
						if (!pAPP0)
						{
							bytesInserted += sizeof(APP0);
						}
						if (!pAPP1)
						{
							bytesInserted += sizeof(APP1);
						}

						// any insertions needed?
						if (bytesInserted)
						{
							offset += bytesInserted;
							// move existing pointers down
							header.fieldsize = Swap4Bytes(Swap4Bytes(header.fieldsize) + bytesInserted);
							header.sofoffset = Swap4Bytes(Swap4Bytes(header.sofoffset) + bytesInserted);
							header.quantoffset = Swap4Bytes(Swap4Bytes(header.quantoffset) + bytesInserted);
							header.sosoffset = Swap4Bytes(Swap4Bytes(header.sosoffset) + bytesInserted);
							header.dataoffset = Swap4Bytes(Swap4Bytes(header.dataoffset) + bytesInserted);
						}
						else
						{
							// no modification necessary (still MJPEG but no processing needed)
							m_bProcessMJPG = false;
							return __super::WriteData(patm, fieldstart, cBytesTotal, pcActual);
						}
						const BYTE* pNextFrame = pData;
						if (m == 0xd8)
						{
							// marker is start of next field
							header.paddedsize = Swap4Bytes(offset);
							header.nextfield = header.paddedsize;
						}
						else
						{
							// single field?
							if (total == 0)
							{
								// no modification necessary (treat as Photo JPEG not MJPEG)
								m_bMJPG = false;
								m_bProcessMJPG = false;
								return __super::WriteData(patm, fieldstart, cBytesTotal, pcActual);
							}
							// marker is last in present field
							header.paddedsize = Swap4Bytes(offset + markerlen);
							pNextFrame += markerlen;
						}

						// APP0 is before the insertion point, if already present, so fix up before
						// writing it out
						if (pAPP0)
						{
							// need to update field size in alternate header
							pAPP0->fieldsize = Swap4Bytes(Swap4Bytes(pAPP0->fieldsize) + bytesInserted);
							pAPP0->paddedsize = Swap4Bytes(Swap4Bytes(pAPP0->paddedsize) + bytesInserted);
						}

						// find insertion point and write out data that comes before that
						// Then we can fix up the huffman location if needed
						if (pInsertBefore == NULL)
						{
							// insert immediately after SOI
							pInsertBefore = fieldstart + 2;
						}
						int len = int(pInsertBefore - fieldstart);
						HRESULT hr = patm->Append(fieldstart, len);
						if (FAILED(hr))
						{
							return hr;
						}
						total += len;
						if (bInsertHuff)
						{
							int hufloc = len;
							if (!pAPP0)
							{
								hufloc += sizeof(APP0);
							}
							if (!pAPP1)
							{
								hufloc += sizeof(APP1);
							}
							header.huffoffset = Swap4Bytes(hufloc);
						}

						if (pAPP1)
						{
							pAPP1->fieldsize = Swap4Bytes(Swap4Bytes(pAPP1->fieldsize) + bytesInserted);
							pAPP1->paddedsize = Swap4Bytes(Swap4Bytes(pAPP1->paddedsize) + bytesInserted);
							pAPP1->quantoffset = Swap4Bytes(Swap4Bytes(pAPP1->quantoffset) + bytesInserted);
							if (pAPP1->huffoffset)
							{
								pAPP1->huffoffset = Swap4Bytes(Swap4Bytes(pAPP1->huffoffset) + bytesInserted);
							}
							else if (bInsertHuff)
							{
								pAPP1->huffoffset = header.huffoffset;
							}

							pAPP1->sofoffset = Swap4Bytes(Swap4Bytes(pAPP1->sofoffset) + bytesInserted);
							pAPP1->sosoffset = Swap4Bytes(Swap4Bytes(pAPP1->sosoffset) + bytesInserted);
							pAPP1->dataoffset = Swap4Bytes(Swap4Bytes(pAPP1->dataoffset) + bytesInserted);
							if (pAPP1->nextfield)
							{
								pAPP1->nextfield = Swap4Bytes(Swap4Bytes(pAPP1->nextfield) + bytesInserted);
							}
						}

						// APP0
						if (!pAPP0)
						{
							app0.fieldsize = header.fieldsize;
							app0.paddedsize = header.paddedsize;

							hr = patm->Append((const BYTE*)&app0, sizeof(APP0));
							if (FAILED(hr))
							{
								return hr;
							}
							total += sizeof(APP0);
						}


						// APP1
						if (!pAPP1)
						{
							hr = patm->Append((const BYTE*)&header, sizeof(header));
							if (FAILED(hr))
							{
								return hr;
							}
							total += sizeof(header);
						}

						if (bInsertHuff)
						{
							hr = patm->Append(DefaultHuffTable, DefaultHuffSize);
							if (FAILED(hr))
							{
								return hr;
							}
							total += DefaultHuffSize;
						}

						// rest of field
						len = int(pNextFrame - pInsertBefore);
						hr = patm->Append(pInsertBefore, len);
						total += len;
						if (FAILED(hr))
						{
							return hr;
						}

						// set up for next field

						fieldstart = pNextFrame;
						pInsertBefore = NULL;
						pAPP0 = NULL;
						pAPP1 = NULL;
						ZeroMemory(&header, sizeof(header));
						header.marker = Swap2Bytes(0xffe1);
						header.length = Swap2Bytes(sizeof(APP1) - 2);
						header.tag = Swap4Bytes(DWORD('mjpg'));
					}
				}
			}
		}
		pData += markerlen;
		cBytes -= markerlen;
	}
	*pcActual = total;
	return S_OK;
}

#pragma pack(pop)

long 
WaveHandler::Scale()
{
    // for audio, the scale should be the sampling rate but
    // must not exceed 65535
    WAVEFORMATEX* pwfx = (WAVEFORMATEX*)m_mt.Format();
    if (pwfx->nSamplesPerSec > 65535)
    {
        return 45000;
    }
    else
    {
        return pwfx->nSamplesPerSec;
    }
}

long WaveHandler::BlockAlign()
{
	WAVEFORMATEX* pwfx = (WAVEFORMATEX*)m_mt.Format();
	return pwfx->nBlockAlign;
}

bool WaveHandler::IsOldIndexFormat() 
{
	WAVEFORMATEX* pwfx = (WAVEFORMATEX*)m_mt.Format();

	if ((pwfx->wFormatTag == WAVE_FORMAT_PCM) && (pwfx->nSamplesPerSec < 65536) && (pwfx->nChannels <= 2))
	{
		return true;
	}
	return false;
}

void 
WaveHandler::WriteDescriptor(Atom* patm, int id, int dataref, long scale)
{
	WAVEFORMATEX* pwfx = (WAVEFORMATEX*)m_mt.Format();

	if (IsOldIndexFormat())
	{
	    smart_ptr<Atom> psd = patm->CreateAtom(DWORD('sowt'));
		BYTE b[44];
		ZeroMemory(b, 44);
		WriteShort(dataref, b+6);
		WriteShort(1, b+8);		// ver 1 of sound sample desc
		WriteShort(pwfx->nChannels, b+16);
		short bits = (pwfx->wBitsPerSample == 8) ? 8 : 16;
		WriteShort(bits, b+18);
		WriteShort(0xffff, b+20);
		WriteShort(pwfx->nSamplesPerSec, b+24);    // this is what forces us to use short audio scales

		short bytesperchan = pwfx->wBitsPerSample / 8;
		WriteLong(1, b+28);
		WriteLong(bytesperchan, b+32);
		WriteLong(bytesperchan * pwfx->nChannels, b+36);
		WriteLong(2, b+40);

		psd->Append(b, 44);
	    psd->Close();
	}
	else
	{
		DWORD dwAtom = 0;
		if (pwfx->wFormatTag == WAVE_FORMAT_PCM)
		{
			dwAtom = 'lpcm';
		} else if (pwfx->wFormatTag == WAVE_FORMAT_MULAW)
		{
			dwAtom = 'ulaw';
		} else if (pwfx->wFormatTag == WAVE_FORMAT_ALAW)
		{
			dwAtom = 'alaw';
		}
	    smart_ptr<Atom> psd = patm->CreateAtom(dwAtom);
		BYTE b[28];
		ZeroMemory(b, 28);
		WriteShort(dataref, b+6);
		WriteShort(2, b+16);
		WriteShort(16, b+18);
		WriteShort(unsigned short(scale), b+24);    // this is what forces us to use short audio scales
		psd->Append(b, 28);

		smart_ptr<Atom> pesd = psd->CreateAtom('esds');
		WriteLong(0, b);        // ver/flags
		pesd->Append(b, 4);
		// es descr
		//      decoder config
		//          <objtype/stream type/bitrates>
		//          decoder specific info desc
		//      sl descriptor
		Descriptor es(Descriptor::ES_Desc);
		WriteShort(id, b);
		b[2] = 0;
		es.Append(b, 3);
		Descriptor dcfg(Descriptor::Decoder_Config);
		b[0] = 0xC0;    // custom object type
		b[1] = (5 << 2) | 1;    // audio stream

		// buffer size 15000
		b[2] = 0;
		b[3] = 0x3a;
		b[4] = 0x98;
		WriteLong(1500000, b+5);    // max bitrate
		WriteLong(0, b+9);          // avg bitrate 0 = variable
		dcfg.Append(b, 13);
		Descriptor dsi(Descriptor::Decoder_Specific_Info);

		// write whole WAVEFORMATEX as decoder specific info
		int cLen = pwfx->cbSize + sizeof(WAVEFORMATEX);
		dsi.Append((const BYTE*)pwfx, cLen);
		dcfg.Append(&dsi);
		es.Append(&dcfg);
		Descriptor sl(Descriptor::SL_Config);
		b[0] = 2;
		sl.Append(b, 2);
		es.Append(&sl);
		es.Write(pesd);
		pesd->Close();
	    psd->Close();
	}
}

bool 
WaveHandler::CanTruncate()
{
    WAVEFORMATEX* pwfx = (WAVEFORMATEX*)m_mt.Format();
    if (pwfx->wFormatTag == WAVE_FORMAT_PCM)
    {
		return true;
	}
	return false;
}

bool 
WaveHandler::Truncate(IMediaSample* pSample, REFERENCE_TIME tNewStart)
{
	if (!CanTruncate())
	{
		return false;
	}
	REFERENCE_TIME tStart, tEnd;
	if (pSample->GetTime(&tStart, &tEnd) != S_OK)
	{
		// VFW_S_NO_STOP_TIME: if the demux is not able to work out the stop time, then we will not truncate.
		return false;
	}
    WAVEFORMATEX* pwfx = (WAVEFORMATEX*)m_mt.Format();
	LONGLONG tDiff = tNewStart - tStart;
	long cBytesExcess = long (tDiff * pwfx->nSamplesPerSec / UNITS) * pwfx->nBlockAlign;
	long cData = pSample->GetActualDataLength();
	BYTE* pBuffer;
	pSample->GetPointer(&pBuffer);
	MoveMemory(pBuffer, pBuffer+cBytesExcess, cData - cBytesExcess);
	pSample->SetActualDataLength(cData - cBytesExcess);
	pSample->SetTime(&tNewStart, &tEnd);
	return true;

}

// ---- descriptor ------------------------

Descriptor::Descriptor(TagType type)
: m_type(type),
  m_cBytes(0),
  m_cValid(0)
{
}

void
Descriptor::Append(const BYTE* pBuffer, long cBytes)
{
    Reserve(cBytes);
    CopyMemory(m_pBuffer+m_cValid, pBuffer, cBytes);
    m_cValid += cBytes;
}

void
Descriptor::Reserve(long cBytes)
{
    if ((m_cValid + cBytes) > m_cBytes)
    {
        // increment memory in 128 byte chunks
        long inc = ((cBytes+127)/128) * 128;
        smart_array<BYTE> pNew = new BYTE[m_cBytes + inc];
        if (m_cValid > 0)
        {
            CopyMemory(pNew, m_pBuffer, m_cValid);
        }
        m_pBuffer = pNew;
        m_cBytes += inc;
    }
}

void
Descriptor::Append(Descriptor* pdesc)
{
    long cBytes = pdesc->Length();
    Reserve(cBytes);
    pdesc->Write(m_pBuffer + m_cValid);
    m_cValid += cBytes;
}

long 
Descriptor::Length()
{
    long cHdr = 2;
    long cBody = m_cValid;
    while (cBody > 0x7f)
    {
        cHdr++;
        cBody >>= 7;
    }
    return cHdr + m_cValid;

}

void 
Descriptor::Write(BYTE* pBuffer)
{
    int idx = 0;
    pBuffer[idx++] = (BYTE) m_type;
	if (m_cValid == 0)
	{
		pBuffer[idx++] = 0;
	}
	else
	{
		long cBody = m_cValid;
		while (cBody)
		{
			BYTE b = BYTE(cBody & 0x7f);
			if (cBody > 0x7f)
			{
				b |= 0x80;
			}
			pBuffer[idx++] = b;
			cBody >>= 7;
		}
	}
	CopyMemory(pBuffer + idx, m_pBuffer, m_cValid);
}

HRESULT 
Descriptor::Write(Atom* patm)
{
    long cBytes = Length();
    smart_array<BYTE> ptemp = new BYTE[cBytes];
    Write(ptemp);
    return patm->Append(ptemp, cBytes);
}

// --- H264 BSF support --------------
H264ByteStreamHandler::H264ByteStreamHandler(const CMediaType* pmt)
: H264Handler(pmt),
  m_bSPS(false),
  m_bPPS(false)
{
	if (*m_mt.FormatType() == FORMAT_MPEG2Video)
	{
		MPEG2VIDEOINFO* pvi = (MPEG2VIDEOINFO*)m_mt.Format();
		m_tFrame = pvi->hdr.AvgTimePerFrame;
		m_cx = pvi->hdr.bmiHeader.biWidth;
		m_cy = pvi->hdr.bmiHeader.biHeight;
	}
	else if (*m_mt.FormatType() == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER* pvi = (VIDEOINFOHEADER*)m_mt.Format();
		m_tFrame = pvi->AvgTimePerFrame;
		m_cx = pvi->bmiHeader.biWidth;
		m_cy = pvi->bmiHeader.biHeight;
	}
	else if (*m_mt.FormatType() == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2* pvi = (VIDEOINFOHEADER2*)m_mt.Format();
		m_tFrame = pvi->AvgTimePerFrame;
		m_cx = pvi->bmiHeader.biWidth;
		m_cy = pvi->bmiHeader.biHeight;
	}
	else
	{
		m_tFrame = m_cx = m_cy = 0;
	}
}

void 
H264ByteStreamHandler::WriteDescriptor(Atom* patm, int id, int dataref, long scale)
{
    UNREFERENCED_PARAMETER(scale);
    UNREFERENCED_PARAMETER(id);
    smart_ptr<Atom> psd = patm->CreateAtom('avc1');

	// locate param sets in parse buffer
	NALUnit sps, pps;
	NALUnit nal;
	const BYTE* pBuffer = m_ParamSets.Data();
	long cBytes = m_ParamSets.Size();
	while (nal.Parse(pBuffer, cBytes, nalunit_length_field, true))
	{
		if (nal.Type() == NALUnit::NAL_Sequence_Params)
		{
			sps = nal;
		}
		else if (nal.Type() == NALUnit::NAL_Picture_Params)
		{
			pps = nal;
		}
		const BYTE* pNext = nal.Start() + nal.Length();
		cBytes-= long(pNext - pBuffer);
		pBuffer = pNext;
	}

	SeqParamSet seq;
	seq.Parse(&sps);

    BYTE b[78];
    ZeroMemory(b, 78);
    WriteShort(dataref, b+6);
    WriteShort(m_cx, b+24);
    WriteShort(m_cy, b+26);
    b[29] = 0x48;
    b[33] = 0x48;
    b[41] = 1;
    b[75] = 24;
    WriteShort(-1, b+76);
    psd->Append(b, 78);

    smart_ptr<Atom> pesd = psd->CreateAtom('avcC');
    b[0] = 1;           // version 1
    b[1] = (BYTE)seq.Profile();
    b[2] = seq.Compat();
    b[3] = (BYTE)seq.Level();
    // length of length-preceded nalus
    b[4] = BYTE(0xfC | (nalunit_length_field - 1));

    b[5] = 0xe1;        // 1 SPS

	// in the descriptor, the length field for param set nalus is always 2
	pesd->Append(b, 6);
	WriteVariable(sps.Length(), b, 2);
	pesd->Append(b, 2);
	pesd->Append(sps.Start(), sps.Length());

    b[0] = 1;   // 1 PPS
	WriteVariable(pps.Length(), b+1, 2);
	pesd->Append(b, 3);
	pesd->Append(pps.Start(), pps.Length());

    pesd->Close();
    psd->Close();
}

LONGLONG 
H264ByteStreamHandler::FrameDuration()
{
	return m_tFrame;
}

HRESULT 
H264ByteStreamHandler::WriteData(Atom* patm, const BYTE* pData, int cBytes, int* pcActual)
{
	int cActual = 0;

	NALUnit nal;
	while(nal.Parse(pData, cBytes, 0, true))
	{
		const BYTE* pNext = nal.Start() + nal.Length();
		cBytes-= long(pNext - pData);
		pData = pNext;

		// convert length to correct byte order
		BYTE length[nalunit_length_field];
		WriteVariable(nal.Length(), length, nalunit_length_field);

		if (!m_bSPS && (nal.Type() == NALUnit::NAL_Sequence_Params))
		{
			// store in length-preceded format for use in WriteDescriptor
			m_bSPS = true;
			m_ParamSets.Append(length, nalunit_length_field);
			m_ParamSets.Append(nal.Start(), nal.Length());
		}
		else if (!m_bPPS && (nal.Type() == NALUnit::NAL_Picture_Params))
		{
			// store in length-preceded format for use in WriteDescriptor
			m_bPPS = true;
			m_ParamSets.Append(length, nalunit_length_field);
			m_ParamSets.Append(nal.Start(), nal.Length());
		}

		// write length and data to file
		patm->Append(length, nalunit_length_field);
		patm->Append(nal.Start(), nal.Length());
		cActual += nalunit_length_field + nal.Length();
	}

	*pcActual = cActual;
	return S_OK;
}

void 
CC608Handler::WriteDescriptor(Atom* patm, int id, int dataref, long scale)
{
    UNREFERENCED_PARAMETER(scale);
    UNREFERENCED_PARAMETER(dataref);
    UNREFERENCED_PARAMETER(id);

    smart_ptr<Atom> psd = patm->CreateAtom('c608');
	psd->Close();
}
