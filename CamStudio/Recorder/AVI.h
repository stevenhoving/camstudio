#pragma once

// Video/Audio Merging Functions and Definitions
extern int Merge_Video_And_Sound_File(CString input_video_path, CString input_audio_path, CString output_avi_path, BOOL recompress_audio, LPWAVEFORMATEX audio_recompress_format, DWORD audio_format_size, BOOL bInterleave, int interleave_factor, int interleave_unit);

class CAVI
{
public:
	static void OnError(HRESULT hrErr)
	{
		switch (hrErr)
		{
		case AVIERR_UNSUPPORTED:
			TRACE("CAVI::Error: AVIERR_UNSUPPORTED\n");
			break;
		case AVIERR_BADFORMAT:
			TRACE("CAVI::Error: AVIERR_BADFORMAT\n");
			break;
		case AVIERR_MEMORY:
			TRACE("CAVI::Error: AVIERR_MEMORY\n");
			break;
		case AVIERR_INTERNAL:
			TRACE("CAVI::Error: AVIERR_INTERNAL\n");
			break;
		case AVIERR_BADFLAGS:
			TRACE("CAVI::Error: AVIERR_BADFLAGS\n");
			break;
		case AVIERR_BADPARAM:
			TRACE("CAVI::Error: AVIERR_BADPARAM\n");
			break;
		case AVIERR_BADSIZE:
			TRACE("CAVI::Error: AVIERR_BADSIZE\n");
			break;
		case AVIERR_BADHANDLE:
			TRACE("CAVI::Error: AVIERR_BADHANDLE\n");
			break;
		case AVIERR_FILEREAD:
			TRACE("CAVI::Error: AVIERR_FILEREAD\n");
			break;
		case AVIERR_FILEWRITE:
			TRACE("CAVI::Error: AVIERR_FILEWRITE\n");
			break;
		case AVIERR_FILEOPEN:
			TRACE("CAVI::Error: AVIERR_FILEOPEN\n");
			break;
		case AVIERR_COMPRESSOR:
			TRACE("CAVI::Error: AVIERR_COMPRESSOR\n");
			break;
		case AVIERR_NOCOMPRESSOR:
			TRACE("CAVI::Error: AVIERR_NOCOMPRESSOR\n");
			break;
		case AVIERR_READONLY:
			TRACE("CAVI::Error: AVIERR_READONLY\n");
			break;
		case AVIERR_NODATA:
			TRACE("CAVI::Error: AVIERR_NODATA\n");
			break;
		case AVIERR_BUFFERTOOSMALL:
			TRACE("CAVI::Error: AVIERR_BUFFERTOOSMALL\n");
			break;
		case AVIERR_CANTCOMPRESS:
			TRACE("CAVI::Error: AVIERR_CANTCOMPRESS\n");
			break;
		case AVIERR_USERABORT:
			TRACE("CAVI::Error: AVIERR_USERABORT\n");
			break;
		default:
		case AVIERR_ERROR:
			TRACE("CAVI::Error: AVIERR_ERROR\n");
			break;
		}
	}

protected:
	// AVIFile Library Operations
	void FileInit()		{::AVIFileInit();}
	void FileExit()		{::AVIFileExit();}
};

class CAVIFile :
	public CAVI
{
public:
	CAVIFile();
	~CAVIFile();

	operator PAVIFILE()		{return m_pfile;}

private:
	PAVIFILE m_pfile;

protected:
	// Opening and Closing AVI Files
	void Open(LPCTSTR szFile, UINT uMode, LPCLSID lpHandler = 0)
	{
		::AVIFileOpen(&m_pfile, szFile, uMode, lpHandler);
	}
	ULONG AddRef()	{return ::AVIFileAddRef(m_pfile);}
	ULONG Release()	{return ::AVIFileRelease(m_pfile);}
	// '_' to avoid macro substitution
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
	HRESULT GetStream(PAVISTREAM * ppavi, FOURCC fccType, LONG lParam)
	{
		return ::AVIFileGetStream(m_pfile, ppavi, fccType, lParam);
	}

	// Writing Individual Streams
	HRESULT CreateStream(PAVISTREAM * ppavi, AVISTREAMINFO * psi)
	{
		return ::AVIFileCreateStream(m_pfile, ppavi, psi);
	}
};

class CAVIStream :
	public CAVI
{
public:
	CAVIStream();
	~CAVIStream();

	operator PAVISTREAM()		{return m_pavi;}

private:
	PAVISTREAM m_pavi;

protected:
	HRESULT OpenFromFile(LPCTSTR szFile, FOURCC fccType, LONG lParam, UINT mode, CLSID * pclsidHandler)
	{
		return ::AVIStreamOpenFromFile(&m_pavi, szFile, fccType, lParam, mode, pclsidHandler);
	}
	LONG AddRef()
	{
		return ::AVIStreamAddRef(m_pavi);
	}
	// This function supersedes the obsolete AVIStreamClose function.
	LONG Release()
	{
		return ::AVIStreamRelease(m_pavi);
	}
	
	// Reading Stream Information
	HRESULT ReadData(DWORD ckid, LPVOID lpData, LONG * lpcbData)
	{
		return ::AVIStreamReadData(m_pavi, ckid, lpData, lpcbData);
	}
	HRESULT DataSize(FOURCC fcc, LONG * plSize)
	{
		return ReadData(fcc, 0, plSize);
	}
	HRESULT ReadFormat(LONG lPos, LPVOID lpFormat, LONG * lpcbFormat)
	{
		return ::AVIStreamReadFormat(m_pavi, lPos, lpFormat, lpcbFormat);
	}
	HRESULT FormatSize(LONG lPos, LONG * lpcbFormat)
	{
		return ::AVIStreamReadFormat(m_pavi, lPos, 0, lpcbFormat);
	}
	HRESULT Read(LONG lStart, LONG lSamples, LPVOID lpBuffer, LONG cbBuffer, LONG * plBytes, LONG * plSamples)
	{
		return ::AVIStreamRead(m_pavi, lStart, lSamples, lpBuffer, cbBuffer, plBytes, plSamples);
	}
	HRESULT SampleSize(LONG lPos, LONG * plSize)
	{		
		return Read(lPos, 1, 0, 0, plSize, 0);
	}
	HRESULT BeginStreaming(LONG lStart, LONG lEnd, LONG lRate)
	{
		return ::AVIStreamBeginStreaming(m_pavi, lStart, lEnd, lRate);
	}
	HRESULT EndStreaming()
	{
		return ::AVIStreamEndStreaming(m_pavi);
	}

	// Decompressing Video Data in a Stream
	PGETFRAME GetFrameOpen(LPBITMAPINFOHEADER lpbiWanted)
	{
		return ::AVIStreamGetFrameOpen(m_pavi, lpbiWanted);
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

	HRESULT SaveV(LPCTSTR szFile, CLSID * pclsidHandler, AVISAVECALLBACK lpfnCallback, int nStreams, LPAVICOMPRESSOPTIONS * plpOptions)
	{
		return ::AVISaveV(szFile, pclsidHandler, lpfnCallback, nStreams, &m_pavi, plpOptions);
	}

	BOOL SaveOptions(HWND hwnd, UINT uiFlags, int nStreams, LPAVICOMPRESSOPTIONS * plpOptions)
	{
		return ::AVISaveOptions(hwnd, uiFlags, nStreams, &m_pavi, plpOptions);
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
		return ::AVIStreamSetFormat(m_pavi, lPos, lpFormat, cbFormat);
	}
	HRESULT Write(LONG lStart, LONG lSamples, LPVOID lpBuffer, LONG cbBuffer, DWORD dwFlags, LONG * plSampWritten, LONG * plBytesWritten)
	{
		return ::AVIStreamWrite(m_pavi, lStart, lSamples, lpBuffer, cbBuffer, dwFlags, plSampWritten, plBytesWritten);
	}
	HRESULT WriteData(DWORD ckid, LPVOID lpData, LONG cbData)
	{
		return ::AVIStreamWriteData(m_pavi, ckid, lpData, cbData);
	}

	// Finding the Starting Position in a Stream
	LONG Start()
	{
		return ::AVIStreamStart(m_pavi);
	}
	LONG StartTime()
	{
		return SampleToTime(Start());
	}
	LONG Length()
	{
		return ::AVIStreamLength(m_pavi);
	}
	LONG LengthTime()
	{
		return SampleToTime(Length());
	}
	// This function supersedes the obsolete AVIStreamFindKeyFrame function.
	LONG FindSample(LONG lPos, LONG lFlags)
	{
		return ::AVIStreamFindSample(m_pavi, lPos, lFlags);
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
		return ::AVIStreamSampleToTime(m_pavi, lSample);
	}
	LONG TimeToSample(LONG lTime)
	{
		return ::AVIStreamTimeToSample(m_pavi, lTime);
	}

	// Creating Temporary Streams
	HRESULT Create(LONG lParam1, LONG lParam2, CLSID * pclsidHandler)
	{
		return ::AVIStreamCreate(&m_pavi, lParam1, lParam2, pclsidHandler);
	}
	HRESULT MakeCompressedStream(PAVISTREAM * ppsCompressed, AVICOMPRESSOPTIONS * lpOptions, CLSID * pclsidHandler)
	{
		return ::AVIMakeCompressedStream(ppsCompressed, m_pavi, lpOptions, pclsidHandler);
	}

	// Editing AVI Streams
	HRESULT CreateEditableStream(PAVISTREAM * ppsEditable)
	{
		return ::CreateEditableStream(ppsEditable, m_pavi);
	}
	HRESULT EditStreamCut(LONG * plStart, LONG * plLength, PAVISTREAM * ppResult)
	{
		return ::EditStreamCut(m_pavi, plStart, plLength, ppResult);
	}
	HRESULT EditStreamCopy(LONG * plStart, LONG * plLength, PAVISTREAM * ppResult)
	{
		return ::EditStreamCopy(m_pavi, plStart, plLength, ppResult);
	}
	HRESULT EditStreamPaste(LONG * plPos, LONG * plLength, PAVISTREAM pstream, LONG lStart, LONG lLength)
	{
		return ::EditStreamPaste(m_pavi, plPos, plLength, pstream, lStart, lLength);
	}
	HRESULT EditStreamClone(PAVISTREAM * ppResult)
	{
		return ::EditStreamClone(m_pavi, ppResult);
	}
	HRESULT EditStreamSetInfo(AVISTREAMINFO * lpInfo, LONG cbInfo)
	{
		return ::EditStreamSetInfo(m_pavi, lpInfo, cbInfo);
	}
	HRESULT EditStreamSetName(LPCTSTR lpszName)
	{
		return ::EditStreamSetName(m_pavi, lpszName);
	}
};
