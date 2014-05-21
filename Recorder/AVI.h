/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Profile.h"

// Video/Audio Merging Functions and Definitions
extern int MergeVideoAudio(CString strVideoIn, CString strAudioIn, CString strAVIOut, BOOL bAudioRecompress, sAudioFormat& rAudioFormat);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class CAVI
{
public:
	static void OnError(HRESULT hrErr)
	{
		switch (hrErr)
		{
		case AVIERR_OK:
			break;
		default:					TRACE("CAVI::Error: Unknown\n"); break;
		case AVIERR_UNSUPPORTED:	TRACE("CAVI::Error: AVIERR_UNSUPPORTED\n"); break;
		case AVIERR_BADFORMAT:		TRACE("CAVI::Error: AVIERR_BADFORMAT\n"); break;
		case AVIERR_MEMORY:			TRACE("CAVI::Error: AVIERR_MEMORY\n"); break;
		case AVIERR_INTERNAL:		TRACE("CAVI::Error: AVIERR_INTERNAL\n"); break;
		case AVIERR_BADFLAGS:		TRACE("CAVI::Error: AVIERR_BADFLAGS\n"); break;
		case AVIERR_BADPARAM:		TRACE("CAVI::Error: AVIERR_BADPARAM\n"); break;
		case AVIERR_BADSIZE:		TRACE("CAVI::Error: AVIERR_BADSIZE\n"); break;
		case AVIERR_BADHANDLE:		TRACE("CAVI::Error: AVIERR_BADHANDLE\n"); break;
		case AVIERR_FILEREAD:		TRACE("CAVI::Error: AVIERR_FILEREAD\n"); break;
		case AVIERR_FILEWRITE:		TRACE("CAVI::Error: AVIERR_FILEWRITE\n"); break;
		case AVIERR_FILEOPEN:		TRACE("CAVI::Error: AVIERR_FILEOPEN\n"); break;
		case AVIERR_COMPRESSOR:		TRACE("CAVI::Error: AVIERR_COMPRESSOR\n"); break;
		case AVIERR_NOCOMPRESSOR:	TRACE("CAVI::Error: AVIERR_NOCOMPRESSOR\n"); break;
		case AVIERR_READONLY:		TRACE("CAVI::Error: AVIERR_READONLY\n"); break;
		case AVIERR_NODATA:			TRACE("CAVI::Error: AVIERR_NODATA\n"); break;
		case AVIERR_BUFFERTOOSMALL:	TRACE("CAVI::Error: AVIERR_BUFFERTOOSMALL\n"); break;
		case AVIERR_CANTCOMPRESS:	TRACE("CAVI::Error: AVIERR_CANTCOMPRESS\n"); break;
		case AVIERR_USERABORT:		TRACE("CAVI::Error: AVIERR_USERABORT\n"); break;
		case AVIERR_ERROR:			TRACE("CAVI::Error: AVIERR_ERROR\n"); break;
		}
	}

protected:
	// AVIFile Library Operations
	void FileInit()		{::AVIFileInit();}
	void FileExit()		{::AVIFileExit();}
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class CAVIStream : public CAVI
{
public:
	CAVIStream();
	~CAVIStream();

	operator PAVISTREAM()			{return m_pstream;}
	//operator PAVISTREAM&()		{return m_pstream;}
	operator PAVISTREAM*()			{return &m_pstream;}
	const AVISTREAMINFO& getInfo()	{return m_sAVISTREAMINFO;}

private:
	PAVISTREAM m_pstream;
	AVISTREAMINFO m_sAVISTREAMINFO;

//protected:
public:
	HRESULT OpenFromFile(LPCTSTR szFile, FOURCC fccType, LONG lParam, UINT mode, CLSID * pclsidHandler)
	{
		return ::AVIStreamOpenFromFile(&m_pstream, szFile, fccType, lParam, mode, pclsidHandler);
	}
	LONG AddRef()
	{
		return ::AVIStreamAddRef(m_pstream);
	}
	// This function supersedes the obsolete AVIStreamClose function.
	LONG Release()
	{
		return (m_pstream) ? ::AVIStreamRelease(m_pstream) : 0L;
	}
	HRESULT Info(AVISTREAMINFO * psi, LONG lSize)
	{
		return ::AVIStreamInfo(m_pstream, psi, lSize);
	}
	HRESULT Info()
	{
		return ::AVIStreamInfo(m_pstream, &m_sAVISTREAMINFO, sizeof(AVISTREAMINFO));
	}
	
	// Reading Stream Information
	HRESULT ReadData(DWORD ckid, LPVOID lpData, LONG * lpcbData)
	{
		return ::AVIStreamReadData(m_pstream, ckid, lpData, lpcbData);
	}
	HRESULT DataSize(FOURCC fcc, LONG * plSize)
	{
		return ReadData(fcc, 0, plSize);
	}
	HRESULT ReadFormat(LONG lPos, LPVOID lpFormat, LONG * lpcbFormat)
	{
		return ::AVIStreamReadFormat(m_pstream, lPos, lpFormat, lpcbFormat);
	}
	HRESULT FormatSize(LONG lPos, LONG * lpcbFormat)
	{
		return ::AVIStreamReadFormat(m_pstream, lPos, 0, lpcbFormat);
	}
	HRESULT Read(LONG lStart, LONG lSamples, LPVOID lpBuffer, LONG cbBuffer, LONG * plBytes, LONG * plSamples)
	{
		return ::AVIStreamRead(m_pstream, lStart, lSamples, lpBuffer, cbBuffer, plBytes, plSamples);
	}
	HRESULT SampleSize(LONG lPos, LONG * plSize)
	{		
		return Read(lPos, 1, 0, 0, plSize, 0);
	}
	HRESULT BeginStreaming(LONG lStart, LONG lEnd, LONG lRate)
	{
		return ::AVIStreamBeginStreaming(m_pstream, lStart, lEnd, lRate);
	}
	HRESULT EndStreaming()
	{
		return ::AVIStreamEndStreaming(m_pstream);
	}

	// Decompressing Video Data in a Stream
	PGETFRAME GetFrameOpen(LPBITMAPINFOHEADER lpbiWanted)
	{
		return ::AVIStreamGetFrameOpen(m_pstream, lpbiWanted);
	}
	LPVOID GetFrame(PGETFRAME pgf, LONG lPos)
	{
		return ::AVIStreamGetFrame(pgf, lPos);
	}
	HRESULT GetFrameClose(PGETFRAME pget)
	{
		return ::AVIStreamGetFrameClose(pget);
	}

	// Creating a File from Existing Streams
	// TODO: vararg version
	//HRESULT Save(LPCTSTR szFile, CLSID * pclsidHandler, AVISAVECALLBACK lpfnCallback, int nStreams, PAVISTREAM pavi, LPAVICOMPRESSOPTIONS lpOptions,...)
	//{
	//	return ::AVISave(szFile, pclsidHandler, lpfnCallback, nStreams, pavi, lpOptions,...);
	//}

	// TODO: Not safe! Do not use.
	HRESULT SaveV(LPCTSTR szFile, CLSID * pclsidHandler, AVISAVECALLBACK lpfnCallback, int nStreams, PAVISTREAM * pavi, LPAVICOMPRESSOPTIONS * plpOptions)
	{
		return ::AVISaveV(szFile, pclsidHandler, lpfnCallback, nStreams, pavi, plpOptions);
	}

	BOOL SaveOptions(HWND hwnd, UINT uiFlags, int nStreams, LPAVICOMPRESSOPTIONS * plpOptions)
	{
		return ::AVISaveOptions(hwnd, uiFlags, nStreams, &m_pstream, plpOptions);
	}
	// n.b. '_' used to avoid macro substitution
	BOOL GetSaveFileName_Preview(LPOPENFILENAME lpofn)
	{
		return ::GetSaveFileNamePreview(lpofn);
	}
	HRESULT MakeFileFromStreams(PAVIFILE * ppfile, int nStreams, PAVISTREAM * papStreams)
	{
		return ::AVIMakeFileFromStreams(ppfile, nStreams, papStreams);
	}

	// Writing Individual Streams
	HRESULT SetFormat(LONG lPos, LPVOID lpFormat, LONG cbFormat)
	{
		return ::AVIStreamSetFormat(m_pstream, lPos, lpFormat, cbFormat);
	}
	HRESULT Write(LONG lStart, LONG lSamples, LPVOID lpBuffer, LONG cbBuffer, DWORD dwFlags, LONG * plSampWritten, LONG * plBytesWritten)
	{
		return ::AVIStreamWrite(m_pstream, lStart, lSamples, lpBuffer, cbBuffer, dwFlags, plSampWritten, plBytesWritten);
	}
	HRESULT WriteData(DWORD ckid, LPVOID lpData, LONG cbData)
	{
		return ::AVIStreamWriteData(m_pstream, ckid, lpData, cbData);
	}

	// Finding the Starting Position in a Stream
	LONG Start()
	{
		return ::AVIStreamStart(m_pstream);
	}
	LONG StartTime()
	{
		return SampleToTime(Start());
	}
	LONG Length()
	{
		return ::AVIStreamLength(m_pstream);
	}
	LONG LengthTime()
	{
		return SampleToTime(Length());
	}
	// This function supersedes the obsolete AVIStreamFindKeyFrame function.
	LONG FindSample(LONG lPos, LONG lFlags)
	{
		return ::AVIStreamFindSample(m_pstream, lPos, lFlags);
	}

	// TODO: AVI 2.0 issue?
	LONG End()
	{
		return Start() + Length();
	}
	LONG EndTime()
	{
		return SampleToTime(End());
	}

	// Finding Sample and Key Frames
	
	BOOL IsKeyFrame(LONG lPos)
	{
		return (NearestKeyFrame(lPos) == 1L);
	}
	LONG NearestKeyFrame(LONG lPos)
	{
		return FindSample(lPos, FIND_PREV | FIND_KEY);
	}
	LONG NearestKeyFrameTime(LONG lTime)
	{
		return SampleToTime(NearestKeyFrame(TimeToSample(lTime))); 
	}
	LONG NearestSample(LONG lPos)
	{
		return FindSample(lPos, FIND_PREV | FIND_ANY);
	}
	LONG NearestSampleTime(LONG lTime)
	{
		return SampleToTime(NearestSample(TimeToSample(lTime)));
	}
	LONG NextKeyFrame(LONG lPos)
	{
		return FindSample(lPos + 1, FIND_NEXT | FIND_KEY);
	}
	LONG NextKeyFrameTime(LONG lTime)
	{
		return SampleToTime(NextKeyFrame(TimeToSample(lTime))); 
	}
	LONG NextSample(LONG lPos)
	{
		return FindSample(lPos + 1, FIND_NEXT | FIND_ANY);
	}
	LONG NextSampleTime(LONG lTime)
	{
		return SampleToTime(NextSample(TimeToSample(lTime)));
	}
	LONG PrevKeyFrame(LONG lPos)
	{
		return FindSample(lPos - 1, FIND_PREV | FIND_KEY);
	}
	LONG PrevKeyFrameTime(LONG lTime)
	{
		return SampleToTime(PrevKeyFrame(TimeToSample(lTime)));
	}
	LONG PrevSample(LONG lPos)
	{
		return FindSample(lPos - 1, FIND_PREV | FIND_ANY);
	}
	LONG PrevSampleTime(LONG lTime)
	{
		return SampleToTime(PrevSample(TimeToSample(lTime)));
	}
	LONG SampleToSample(PAVISTREAM pavi1, PAVISTREAM pavi2, LONG lSample)
	{
		return ::AVIStreamTimeToSample(pavi1, ::AVIStreamSampleToTime(pavi2, lSample));
	}

	// Switching Between Samples and Time
	LONG SampleToTime(LONG lSample)
	{
		return ::AVIStreamSampleToTime(m_pstream, lSample);
	}
	LONG TimeToSample(LONG lTime)
	{
		return ::AVIStreamTimeToSample(m_pstream, lTime);
	}

	// Creating Temporary Streams
	HRESULT Create(LONG lParam1, LONG lParam2, CLSID * pclsidHandler)
	{
		return ::AVIStreamCreate(&m_pstream, lParam1, lParam2, pclsidHandler);
	}
	HRESULT MakeCompressedStream(PAVISTREAM * ppsCompressed, AVICOMPRESSOPTIONS * lpOptions, CLSID * pclsidHandler)
	{
		return ::AVIMakeCompressedStream(ppsCompressed, m_pstream, lpOptions, pclsidHandler);
	}

	// Editing AVI Streams
	HRESULT CreateEditableStream(PAVISTREAM * ppsEditable)
	{
		return ::CreateEditableStream(ppsEditable, m_pstream);
	}
	HRESULT EditStreamCut(LONG * plStart, LONG * plLength, PAVISTREAM * ppResult)
	{
		return ::EditStreamCut(m_pstream, plStart, plLength, ppResult);
	}
	HRESULT EditStreamCopy(LONG * plStart, LONG * plLength, PAVISTREAM * ppResult)
	{
		return ::EditStreamCopy(m_pstream, plStart, plLength, ppResult);
	}
	HRESULT EditStreamPaste(LONG * plPos, LONG * plLength, PAVISTREAM pstream, LONG lStart, LONG lLength)
	{
		return ::EditStreamPaste(m_pstream, plPos, plLength, pstream, lStart, lLength);
	}
	HRESULT EditStreamClone(PAVISTREAM * ppResult)
	{
		return ::EditStreamClone(m_pstream, ppResult);
	}
	HRESULT EditStreamSetInfo(AVISTREAMINFO * lpInfo, LONG cbInfo)
	{
		return ::EditStreamSetInfo(m_pstream, lpInfo, cbInfo);
	}
	HRESULT EditStreamSetName(LPCTSTR lpszName)
	{
		return ::EditStreamSetName(m_pstream, lpszName);
	}
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class CAVIFile : public CAVI
{
public:
	CAVIFile();
	~CAVIFile();

	operator PAVIFILE()		{return m_pfile;}

private:
	PAVIFILE m_pfile;

protected:
public:
	// Opening and Closing AVI Files
	HRESULT Open(LPCTSTR szFile, UINT uMode, LPCLSID lpHandler = 0)
	{
		return ::AVIFileOpen(&m_pfile, szFile, uMode, lpHandler);
	}
	ULONG AddRef()	{return (m_pfile) ? ::AVIFileAddRef(m_pfile) : 0UL;}
	ULONG Release()	{return (m_pfile) ? ::AVIFileRelease(m_pfile) : 0UL;}
	
	// '_' to avoid macro substitution
	// n.b. This appears to be obsolete on Vista
	BOOL GetOpenFileName_Preview(LPOPENFILENAME lpofn)
	{
		::GetOpenFileNamePreview(lpofn);
	}

	// Reading from a File
	HRESULT Info(AVIFILEINFO * pfi, LONG lSize)
	{
		return ::AVIFileInfo(m_pfile, pfi, lSize);
	}
	HRESULT ReadData(DWORD ckid, LPVOID lpData, LONG * lpcbData)
	{
		return ::AVIFileReadData(m_pfile, ckid, lpData, lpcbData);
	}

	// Writing to a File
	HRESULT WriteData(DWORD ckid, LPVOID lpData, LONG cbData)
	{
		return ::AVIFileWriteData(m_pfile, ckid, lpData, cbData);
	}

	// Using the Clipboard
	HRESULT PutFileOnClipboard()
	{
		return ::AVIPutFileOnClipboard(m_pfile);
	}
	HRESULT GetFromClipboard()
	{
		return ::AVIGetFromClipboard(&m_pfile);
	}

	// Opening and Closing Streams
	HRESULT GetStream(PAVISTREAM * ppavi, FOURCC fccType = streamtypeVIDEO, LONG lParam = 0L)
	{
		return ::AVIFileGetStream(m_pfile, ppavi, fccType, lParam);
	}

	// Writing Individual Streams
	HRESULT CreateStream(PAVISTREAM * ppavi, AVISTREAMINFO * psi)
	{
		return ::AVIFileCreateStream(m_pfile, ppavi, psi);
	}
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class CamAVIFile : public CAVIFile
{
	CamAVIFile();
public:
	CamAVIFile(const sVideoOpts& rVideoOpts, const sAudioFormat& rAudioFormat);
	~CamAVIFile();

	bool Merge(const CString& strVideoIn, const CString& strAudioIn, const CString& strAVIOut);
	bool FadeOut(const CString& strAVIIn, const CString& strAVIOut);

private:
	CString m_strAVIFile;
	sVideoOpts m_sVideoOpts;
	sAudioFormat m_sAudioFormat;
	bool CompressionOpts(AVICOMPRESSOPTIONS& rOptions, CAVIStream& raviStream);
public:
};

