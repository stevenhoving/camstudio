#include "StdAfx.h"
#include "Recorder.h"
#include "AVI.h"
#include "vfw/VCM.h"
#include "ximage.h"
#include <windowsx.h>	// for memory operations

#define NUMSTREAMS		2

namespace {	// annonymous
BOOL CALLBACK AudioCompressCallBack(int iProgress)
{
	//Set Progress in Title Bar

	// "Compressing Audio %d%%", iProgress);
	CString fmtstr;
	fmtstr.LoadString(IDS_STRING_COMPRESSINGAUDIO);
	CString szText;
	szText.Format((LPCTSTR)fmtstr, iProgress);
	szText.Format("Compress Video %d%%", iProgress);
	AfxGetMainWnd()->SetWindowText(szText);

	return WinYield();
}
BOOL CALLBACK VideoCompressCallBack(int iProgress)
{
	//Set Progress in Title Bar
	// "Compressing Audio %d%%", iProgress);
	//CString fmtstr;
	//fmtstr.LoadString(IDS_STRING_COMPRESSINGAUDIO);
	CString szText;
	szText.Format("Compress Video %d%%", iProgress);
	AfxGetMainWnd()->SetWindowText(szText);

	return WinYield();
}
}

// Ver 1.1
// ========================================
// Merge Audio and Video File Function
// ========================================
//
// No recompression is applied to the Video File
// Optional Recompression is applied to the Audio File
// Assuming audio_recompress_format is compatible with the existing format of the audio
// file
//
// If recompress audio is set to FALSE, both audio_recompress_format and
// audio_format_size can be NULL
// ========================================
int MergeVideoAudio(CString strVideoIn, CString strAudioIn, CString strAVIOut, BOOL /*bAudioRecompress*/, sAudioFormat& rAudioFormat)
{
#define EXPERIMENTAL_CODE
#ifdef EXPERIMENTAL_CODE
	// TEST
	CamAVIFile aviFile(cVideoOpts, rAudioFormat);
	if (!aviFile.Merge(strVideoIn, strAudioIn, strAVIOut)) {
		return 5;
	}
	TRACE(_T("aviFile.Merge success!\n"));
	return 0;
	// TEST
#else

	::AVIFileInit();

	//Open Video and Audio Files
	PAVIFILE pfileVideo = NULL;
	HRESULT hr = ::AVIFileOpen(&pfileVideo, (LPCTSTR)strVideoIn, OF_READ | OF_SHARE_DENY_NONE, 0L);
	if (hr != 0) {
		//MessageBox(NULL,"Unable to open video file.","Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(NULL,IDS_STRING_NOOPENVIDEO,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}

	PAVISTREAM AviStream[NUMSTREAMS];	// the editable streams

	//Get Video Stream from Video File and Audio Stream from Audio File

	// ==========================================================
	// Important Assumption
	// Assume stream 0 is the correct stream in the files
	// ==========================================================
	if (pfileVideo) {
		PAVISTREAM pavi = 0;
		if (AVIERR_OK != ::AVIFileGetStream(pfileVideo, &pavi, streamtypeVIDEO, 0)) {
			::AVIFileRelease(pfileVideo);
			// MessageBox(NULL,"Unable to open video stream.","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL,IDS_STRING_NOOPENSREAM,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			return 1;
		}

		// Set editable stream number as 0
		if (AVIERR_OK != ::CreateEditableStream(&AviStream[0], pavi)) {
			::AVIStreamRelease(pavi);
			::AVIFileRelease(pfileVideo);

			// MessageBox(NULL,"Unable to create editable video stream.","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL, IDS_STRING_NOCREATESTREAM,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			return 1;
		}

		::AVIStreamRelease(pavi);

		::AVIFileRelease(pfileVideo);
		pfileVideo = NULL;
	}

	// =============================
	// Getting Audio Stream
	// =============================
	{
		PAVISTREAM pavi;
		if (AVIERR_OK != ::AVIStreamOpenFromFile(&pavi, strAudioIn, streamtypeAUDIO, 0, OF_READ | OF_SHARE_DENY_NONE, 0)) {
			::AVIStreamRelease(AviStream[0]);
			//MessageBox(NULL,"Unable to open audio stream.","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL,IDS_STRING_NOOPENAUDIO,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

			return 2;
		}

		//Set editable stream number as 1
		if (AVIERR_OK != ::CreateEditableStream(&AviStream[1], pavi)) {
			::AVIStreamRelease(pavi);
			::AVIStreamRelease(AviStream[0]);
			//MessageBox(NULL,"Unable to create editable audio stream.","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL,IDS_STRING_NOEDITAUDIO,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

			return 2;
		}
		::AVIStreamRelease(pavi);
	}

	// Verifying streams are of the correct type
	AVISTREAMINFO avis[NUMSTREAMS];
	::AVIStreamInfo(AviStream[0], &avis[0], sizeof(avis[0]));
	::AVIStreamInfo(AviStream[1], &avis[1], sizeof(avis[1]));

	//Assert that the streams we are going to work with are correct in our assumption
	//such that stream 0 is video and stream 1 is audio

	if (avis[0].fccType != streamtypeVIDEO) {
		//MessageBox(NULL,"Unable to verify video stream.","Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(NULL,IDS_STRING_NOVERIFYVIDEO,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

		::AVIStreamRelease(AviStream[0]);
		::AVIStreamRelease(AviStream[1]);
		return 3;
	}

	if (avis[1].fccType != streamtypeAUDIO) {
		//MessageBox(NULL,"Unable to verify audio stream.","Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(NULL,IDS_STRING_NOVERIFYAUDIO,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

		::AVIStreamRelease(AviStream[0]);
		::AVIStreamRelease(AviStream[1]);
		return 4;
	}

	// AVISaveV code takes a pointer to compression opts
	AVICOMPRESSOPTIONS gaAVIOptions[NUMSTREAMS];		// compression options
	LPAVICOMPRESSOPTIONS galpAVIOptions[NUMSTREAMS];
	galpAVIOptions[0] = &gaAVIOptions[0];
	galpAVIOptions[1] = &gaAVIOptions[1];

	// clear options structure to zeroes
	::ZeroMemory(galpAVIOptions[0], sizeof(AVICOMPRESSOPTIONS));
	::ZeroMemory(galpAVIOptions[1], sizeof(AVICOMPRESSOPTIONS));

	//=========================================
	//Set Video Stream Compress Options
	//=========================================

	//No Recompression
	galpAVIOptions[0]->fccType = streamtypeVIDEO;
	galpAVIOptions[0]->fccHandler = 0;
	galpAVIOptions[0]->dwFlags = AVICOMPRESSF_VALID
		| AVICOMPRESSF_KEYFRAMES
		| AVICOMPRESSF_DATARATE;
	if (rAudioFormat.m_bInterleaveFrames) {
		galpAVIOptions[0]->dwFlags |= AVICOMPRESSF_INTERLEAVE;
	}
	galpAVIOptions[0]->dwKeyFrameEvery = (DWORD) -1;
	galpAVIOptions[0]->dwQuality = (DWORD)ICQUALITY_DEFAULT;
	galpAVIOptions[0]->dwBytesPerSecond = 0;

	DWORD dwInterleaveEvery = rAudioFormat.m_iInterleaveFactor;;
	if (rAudioFormat.m_iInterleavePeriod == MILLISECONDS) {
		double interfloat = (((double) rAudioFormat.m_iInterleaveFactor) * ((double) cVideoOpts.m_iFramesPerSecond))/1000.0;
		dwInterleaveEvery = (interfloat < 0.0) ? 1 : interfloat;
	}
	galpAVIOptions[0]->dwInterleaveEvery = dwInterleaveEvery;

	//galpAVIOptions[0]->cbParms = 0;
	//galpAVIOptions[0]->cbFormat = 0;

	//=========================================
	//Set Audio Stream Compress Options
	//=========================================
	//Recompression may be applied
	//
	//Audio Compress Options seems to be specified by the audio format in avicompressoptions
	galpAVIOptions[1]->fccType = streamtypeAUDIO;
	galpAVIOptions[1]->fccHandler = 0;
	galpAVIOptions[1]->dwFlags = AVICOMPRESSF_VALID;
	if (rAudioFormat.m_bInterleaveFrames) {
		galpAVIOptions[1]->dwFlags |= AVICOMPRESSF_INTERLEAVE;
	}
	galpAVIOptions[1]->dwKeyFrameEvery = 0;
	galpAVIOptions[1]->dwQuality = 0;
	galpAVIOptions[1]->dwBytesPerSecond = 0;

	dwInterleaveEvery = rAudioFormat.m_iInterleaveFactor;;
	if (rAudioFormat.m_iInterleavePeriod == MILLISECONDS) {
		//back here
		double interfloat = (((double) rAudioFormat.m_iInterleaveFactor) * ((double) cVideoOpts.m_iFramesPerSecond))/1000.0;
		dwInterleaveEvery = (interfloat < 0.0) ? 1 : interfloat;
	}
	galpAVIOptions[1]->dwInterleaveEvery = dwInterleaveEvery;

	//galpAVIOptions[1]->dwInterleaveEvery = rAudioFormat.m_iInterleaveFactor;
	//galpAVIOptions[1]->cbParms = 0;

	if (bAudioRecompress) {
		galpAVIOptions[1]->cbFormat = rAudioFormat.m_dwCbwFX;
		galpAVIOptions[1]->lpFormat = GlobalAllocPtr(GHND, rAudioFormat.m_dwCbwFX);
		memcpy( (void *) galpAVIOptions[1]->lpFormat, &(rAudioFormat.AudioFormat()), rAudioFormat.m_dwCbwFX);
	} else {
		LONG lTemp = 0L;
		::AVIStreamReadFormat(AviStream[1], ::AVIStreamStart(AviStream[1]), NULL, &lTemp);
		galpAVIOptions[1]->cbFormat = lTemp;
		if (lTemp) {
			galpAVIOptions[1]->lpFormat = GlobalAllocPtr(GHND, lTemp);
		}
		// Use existing format as compress format
		if (galpAVIOptions[1]->lpFormat) {
			::AVIStreamReadFormat(AviStream[1], ::AVIStreamStart(AviStream[1]), galpAVIOptions[1]->lpFormat, &lTemp);
		}
	}

	// ============================
	// Do the work! Merging
	// ============================

	//Save fccHandlers
	DWORD fccHandler[NUMSTREAMS];
	fccHandler[0] = galpAVIOptions[0]->fccHandler;
	fccHandler[1] = galpAVIOptions[1]->fccHandler;

	hr = ::AVISaveV((LPCTSTR)strAVIOut, 0, (AVISAVECALLBACK)AudioCompressCallBack, NUMSTREAMS, AviStream, galpAVIOptions);
	//hr = AVISaveV(LPCTSTR(strAVIOut), NULL, (AVISAVECALLBACK) NULL, NUMSTREAMS, AviStream, galpAVIOptions);
	if (hr != AVIERR_OK) {
		// Error merging with audio compress options, retry merging with default audio options (no recompression)
		if (bAudioRecompress) {
			::AVISaveOptionsFree(NUMSTREAMS, galpAVIOptions);

			galpAVIOptions[0] = &gaAVIOptions[0];
			galpAVIOptions[1] = &gaAVIOptions[1];

			//Resetting Compress Options
			::ZeroMemory(galpAVIOptions[0], sizeof(AVICOMPRESSOPTIONS));
			::ZeroMemory(galpAVIOptions[1], sizeof(AVICOMPRESSOPTIONS));

			galpAVIOptions[0]->fccType = streamtypeVIDEO;
			galpAVIOptions[0]->fccHandler = 0;
			galpAVIOptions[0]->dwFlags = AVICOMPRESSF_VALID
				| AVICOMPRESSF_KEYFRAMES
				| AVICOMPRESSF_DATARATE;
			if (rAudioFormat.m_bInterleaveFrames) {
				galpAVIOptions[0]->dwFlags |= AVICOMPRESSF_INTERLEAVE;
			}
			galpAVIOptions[0]->dwKeyFrameEvery = (DWORD) -1;
			galpAVIOptions[0]->dwQuality = (DWORD)ICQUALITY_DEFAULT;
			galpAVIOptions[0]->dwBytesPerSecond = 0;
			galpAVIOptions[0]->dwInterleaveEvery = rAudioFormat.m_iInterleaveFactor;

			galpAVIOptions[1]->fccType = streamtypeAUDIO;
			galpAVIOptions[1]->fccHandler = 0;
			galpAVIOptions[1]->dwFlags = AVICOMPRESSF_VALID;
			if (rAudioFormat.m_bInterleaveFrames) {
				galpAVIOptions[1]->dwFlags |= AVICOMPRESSF_INTERLEAVE;
			}
			galpAVIOptions[1]->dwKeyFrameEvery = 0;
			galpAVIOptions[1]->dwQuality = 0;
			galpAVIOptions[1]->dwBytesPerSecond = 0;
			galpAVIOptions[1]->dwInterleaveEvery = rAudioFormat.m_iInterleaveFactor;

			//Use default audio format
			LONG lTemp;
			::AVIStreamReadFormat(AviStream[1], AVIStreamStart(AviStream[1]), NULL, &lTemp);
			galpAVIOptions[1]->cbFormat = lTemp;
			if (lTemp) {
				galpAVIOptions[1]->lpFormat = GlobalAllocPtr(GHND, lTemp);
			}
			// Use existing format as compress format
			if (galpAVIOptions[1]->lpFormat) {
				::AVIStreamReadFormat(AviStream[1], AVIStreamStart(AviStream[1]),galpAVIOptions[1]->lpFormat, &lTemp);
			}

			//Do the Work .... Merging
			hr = ::AVISaveV(LPCTSTR(strAVIOut), NULL, (AVISAVECALLBACK) NULL, NUMSTREAMS, AviStream, galpAVIOptions);
			if (hr != AVIERR_OK) {
				::AVISaveOptionsFree(NUMSTREAMS,galpAVIOptions);
				::AVIStreamRelease(AviStream[0]);
				::AVIStreamRelease(AviStream[1]);

				//MessageBox(NULL,"Unable to merge audio and video streams (1).","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(NULL,IDS_STRING_NOMERGE1,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

				return 5;
			}

			//Succesful Merging, but with no audio recompression
			//MessageBox(NULL,"Unable to apply audio compression with the selected options. Your movie is saved without audio compression.","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL,IDS_STRING_NOAUDIOCOMPRESS,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		} else {
			// if recompress audio retry
			::AVISaveOptionsFree(NUMSTREAMS, galpAVIOptions);
			::AVIStreamRelease(AviStream[0]);
			::AVIStreamRelease(AviStream[1]);
			//MessageBox(NULL,"Unable to audio and video merge streams (2).","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL,IDS_STRING_NOMERGE2,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

			return 5;
		}
	}

	// Restore fccHandlers
	galpAVIOptions[0]->fccHandler = fccHandler[0];
	galpAVIOptions[1]->fccHandler = fccHandler[1];

	::AVISaveOptionsFree(NUMSTREAMS, galpAVIOptions);

	// Free Editable Avi Streams
	for (int i = 0; i < NUMSTREAMS; i++) {
		if (AviStream[i]) {
			::AVIStreamRelease(AviStream[i]);
			AviStream[i] = NULL;
		}
	}

	::AVIFileExit();

	// Set Title Bar
	::AfxGetMainWnd()->SetWindowText(_T("CamStudio - Custom Build"));
	return 0;
#endif	// EXPERIMENTAL_CODE
}

CAVIStream::CAVIStream()
: m_pstream(0)
{
	::ZeroMemory(&m_sAVISTREAMINFO, sizeof(AVISTREAMINFO));
	::AVIFileInit();
}

CAVIStream::~CAVIStream()
{
	ULONG uRefCount = Release();
	TRACE(_T("CAVIStream::~CAVIStream: uRefCount: %d\n"), uRefCount);
	::AVIFileExit();
}

CAVIFile::CAVIFile()
: m_pfile(0)
{
	::AVIFileInit();
}

CAVIFile::~CAVIFile()
{
	// Release open file streams
	ULONG uRefCount = Release();
	TRACE(_T("CAVIFile::~CAVIFile: uRefCount: %d\n"), uRefCount);
	::AVIFileExit();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CamAVIFile::CamAVIFile()
{
}
CamAVIFile::CamAVIFile(const sVideoOpts& rVideoOpts, const sAudioFormat& rAudioFormat)
	: m_sVideoOpts(rVideoOpts)
	, m_sAudioFormat(rAudioFormat)
{
}

CamAVIFile::~CamAVIFile()
{
}

// set compressor options according to the stream
bool CamAVIFile::CompressionOpts(AVICOMPRESSOPTIONS& rOptions, CAVIStream& raviStream)
{
	bool bResult = false;

	::ZeroMemory(&rOptions, sizeof(AVICOMPRESSOPTIONS));

	AVISTREAMINFO sAVISI;
	::ZeroMemory(&sAVISI, sizeof(AVISTREAMINFO));
	HRESULT hResult = raviStream.Info(&sAVISI, sizeof(AVISTREAMINFO));
	CAVI::OnError(hResult);
	bResult = (AVIERR_OK == hResult);
	if (!bResult) {
		return bResult;
	}
	bool bVideoStream = (sAVISI.fccType == streamtypeVIDEO);
	bool bAudioStream = (sAVISI.fccType == streamtypeAUDIO);
	bResult = (bVideoStream || bAudioStream);
	if (!bResult) {
		return bResult;
	}
	if (bVideoStream) {
		rOptions.fccType	= sAVISI.fccType;
		//rOptions.fccHandler = m_sVideoOpts.m_dwCompfccHandler;
		rOptions.fccHandler = 0;	// sAVISI.fccHandler; ?
		rOptions.dwFlags = AVICOMPRESSF_VALID
			| AVICOMPRESSF_KEYFRAMES
			| AVICOMPRESSF_DATARATE;
		if (m_sAudioFormat.m_bInterleaveFrames) {
			rOptions.dwFlags |= AVICOMPRESSF_INTERLEAVE;
		}
		rOptions.dwKeyFrameEvery = (DWORD)-1;
		rOptions.dwQuality = (DWORD)ICQUALITY_DEFAULT;
		rOptions.dwBytesPerSecond = 0;

		rOptions.dwInterleaveEvery = m_sAudioFormat.m_iInterleaveFactor;;
		if (m_sAudioFormat.m_iInterleavePeriod == MILLISECONDS) {
			double interfloat = (((double) m_sAudioFormat.m_iInterleaveFactor) * ((double) m_sVideoOpts.m_iFramesPerSecond))/1000.0;
			rOptions.dwInterleaveEvery = (DWORD)((interfloat < 0.0) ? 1 : interfloat);
		}
	} else {
		ASSERT(bAudioStream);
		rOptions.fccType = streamtypeAUDIO;
		rOptions.fccHandler = 0;
		rOptions.dwFlags = AVICOMPRESSF_VALID;
		if (m_sAudioFormat.m_bInterleaveFrames) {
			rOptions.dwFlags |= AVICOMPRESSF_INTERLEAVE;
		}
		rOptions.dwKeyFrameEvery = 0;
		rOptions.dwQuality = 0;
		rOptions.dwBytesPerSecond = 0;

		rOptions.dwInterleaveEvery = m_sAudioFormat.m_iInterleaveFactor;;
		if (m_sAudioFormat.m_iInterleavePeriod == MILLISECONDS) {
			double interfloat = (((double) m_sAudioFormat.m_iInterleaveFactor) * ((double) m_sVideoOpts.m_iFramesPerSecond))/1000.0;
			rOptions.dwInterleaveEvery = (DWORD)((interfloat < 0.0) ? 1 : interfloat);
		}
		LONG lSize = 0;
		raviStream.ReadFormat(raviStream.Start(), 0, &lSize);
		rOptions.cbFormat = lSize;
		if (rOptions.cbFormat) {
			//rOptions.lpFormat = new char[rOptions.cbFormat];
			rOptions.lpFormat = GlobalAllocPtr(GHND, rOptions.cbFormat);
			// Use existing format as compress format
			if (rOptions.lpFormat) {
				raviStream.ReadFormat(raviStream.Start(), rOptions.lpFormat, &lSize);
			}
		}
	}
	return bResult;
}

bool CamAVIFile::Merge(const CString& strVideoIn, const CString& strAudioIn, const CString& strAVIOut)
{
	bool bResult = false;
	CAVIStream streamVideoEdit;
	{
		CAVIStream streamVideo;
		HRESULT hResult = streamVideo.OpenFromFile(strVideoIn, streamtypeVIDEO, 0, OF_READ | OF_SHARE_DENY_NONE, 0);
		CAVI::OnError(hResult);
		bResult = (AVIERR_OK == hResult);
		if (!bResult) {
			return bResult;
		}
		//hResult = streamVideo.CreateEditableStream(&((PAVISTREAM&)streamVideoEdit));
		hResult = streamVideo.CreateEditableStream(streamVideoEdit);
		CAVI::OnError(hResult);
		bResult = (AVIERR_OK == hResult);
		if (!bResult) {
			return bResult;
		}
		hResult = streamVideoEdit.Info();
		CAVI::OnError(hResult);
		bResult = (AVIERR_OK == hResult);
		if (!bResult) {
			return bResult;
		}
		bResult = (streamVideoEdit.getInfo().fccType == streamtypeVIDEO);
		if (!bResult) {
			return bResult;
		}
	}

	CAVIStream streamAudioEdit;
	{
		CAVIStream streamAudio;
		HRESULT hResult = streamAudio.OpenFromFile(strAudioIn, streamtypeAUDIO, 0, OF_READ | OF_SHARE_DENY_NONE, 0);
		CAVI::OnError(hResult);
		bResult = (AVIERR_OK == hResult);
		if (!bResult) {
			return bResult;
		}
		//hResult = streamAudio.CreateEditableStream(&((PAVISTREAM&)streamAudioEdit));
		hResult = streamAudio.CreateEditableStream(streamAudioEdit);
		CAVI::OnError(hResult);
		bResult = (AVIERR_OK == hResult);
		if (!bResult) {
			return bResult;
		}
		hResult = streamAudioEdit.Info();
		CAVI::OnError(hResult);
		bResult = (AVIERR_OK == hResult);
		if (!bResult) {
			return bResult;
		}
		bResult = (streamAudioEdit.getInfo().fccType == streamtypeAUDIO);
		if (!bResult) {
			return bResult;
		}
	}
	AVICOMPRESSOPTIONS AVICompressOptions[NUMSTREAMS];
	CompressionOpts(AVICompressOptions[0], streamVideoEdit);
	CompressionOpts(AVICompressOptions[1], streamAudioEdit);
	LPAVICOMPRESSOPTIONS lpAVICompressOptions[NUMSTREAMS] =
	{
		&AVICompressOptions[0],
		&AVICompressOptions[1]
	};
	// the editable streams
	PAVISTREAM AviStream[NUMSTREAMS];
	AviStream[0] = streamVideoEdit;
	AviStream[1] = streamAudioEdit;
	HRESULT hResult = streamVideoEdit.SaveV(strAVIOut, 0, AudioCompressCallBack, NUMSTREAMS, AviStream, lpAVICompressOptions);
	CAVI::OnError(hResult);
	bResult = (AVIERR_OK == hResult);

	// Set Title Bar
	::AfxGetMainWnd()->SetWindowText(_T("CamStudio - Open Source"));

	return bResult;
}

// TODO: merge audio
bool CamAVIFile::FadeOut(const CString& strAVIIn, const CString& strAVIOut)
{
	bool bResult = true;
	// open the first AVI stream from the input file
	CAVIStream streamIn;
	HRESULT hResult = streamIn.OpenFromFile(strAVIIn, streamtypeVIDEO, 0L, OF_READ | OF_SHARE_DENY_NONE, 0);
	CAVI::OnError(hResult);
	bResult = bResult && (AVIERR_OK == hResult);
	if (!bResult) {
		return bResult;
	}
	hResult = streamIn.Info();	// collect stream info
	CAVI::OnError(hResult);
	bResult = bResult && (AVIERR_OK == hResult);
	if (!bResult) {
		return bResult;
	}
	// get the first image from the file
	PGETFRAME pGetFrame = streamIn.GetFrameOpen(0);	// use default Bitmap format
	bResult = bResult && (0 != pGetFrame);
	if (!bResult) {
		return bResult;
	}
	LPVOID lpImage = streamIn.GetFrame(pGetFrame, 0L);
	LPBITMAPINFOHEADER lpBitmap = static_cast<LPBITMAPINFOHEADER>(lpImage);
	bResult = bResult && (0 != lpBitmap);
	if (!bResult) {
		return bResult;
	}

	// create the AVI output file
	CAVIFile aviOutput;
	hResult = aviOutput.Open(strAVIOut, OF_WRITE | OF_CREATE, 0);
	CAVI::OnError(hResult);
	bResult = bResult && (AVIERR_OK == hResult);
	if (!bResult) {
		return bResult;
	}
	// create output stream info
	// n.b. handler not needed yet.
	AVISTREAMINFO saviStreamInfo;
	::ZeroMemory(&saviStreamInfo, sizeof(AVISTREAMINFO));
	saviStreamInfo.fccType					= streamtypeVIDEO;
	saviStreamInfo.fccHandler				= 0;
	saviStreamInfo.dwScale					= streamIn.getInfo().dwScale;
	saviStreamInfo.dwRate					= streamIn.getInfo().dwRate;
	saviStreamInfo.dwSuggestedBufferSize	= lpBitmap->biSizeImage;
	saviStreamInfo.rcFrame					= streamIn.getInfo().rcFrame;

	// create output stream from file
	CAVIStream aviStreamOut;
	hResult = aviOutput.CreateStream(aviStreamOut, &saviStreamInfo);
	CAVI::OnError(hResult);
	bResult = bResult && (AVIERR_OK == hResult);
	if (!bResult) {
		return bResult;
	}

	// create output stream compressor options
	// n.b. now we want the handler
	AVICOMPRESSOPTIONS saviCompressorOpts;
	::ZeroMemory(&saviCompressorOpts, sizeof(AVICOMPRESSOPTIONS));
	saviCompressorOpts.fccType				= streamtypeVIDEO;
	saviCompressorOpts.fccHandler			= m_sVideoOpts.m_dwCompfccHandler;
	saviCompressorOpts.dwKeyFrameEvery		= m_sVideoOpts.m_iKeyFramesEvery;
	saviCompressorOpts.dwQuality			= m_sVideoOpts.m_iCompQuality;
	saviCompressorOpts.dwFlags				= AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES;
	saviCompressorOpts.dwBytesPerSecond		= 0;
	saviCompressorOpts.lpFormat				= m_sVideoOpts.State();
	saviCompressorOpts.cbFormat				= m_sVideoOpts.StateSize();
	saviCompressorOpts.dwInterleaveEvery	= 0;	// for non-video streams only

	// create compressed output stream from output stream
	CAVIStream aviCompressedStreamOut;
	hResult = aviStreamOut.MakeCompressedStream(aviCompressedStreamOut, &saviCompressorOpts, 0);
	CAVI::OnError(hResult);
	bResult = bResult && (AVIERR_OK == hResult);
	if (!bResult) {
		return bResult;
	}

	// set the compressed output stream format
	// size if size of header + size of color table
	ASSERT(sizeof(BITMAPINFOHEADER) == lpBitmap->biSize);
	long lOffsetImage = lpBitmap->biSize + (lpBitmap->biClrUsed * sizeof(RGBQUAD));
	hResult = aviCompressedStreamOut.SetFormat(0L, lpBitmap, lOffsetImage);
	CAVI::OnError(hResult);
	bResult = bResult && (AVIERR_OK == hResult);
	if (!bResult) {
		return bResult;
	}

	// count the keyframe.
	// Todo: GetInfo and NextKeyFrame are both memberfunction of CAVISTREAM and dwLength is indeed DWORD but NextKeyFrame expect long instead of unsigned long
	DWORD dwLength = streamIn.getInfo().dwLength;
	int iKeyFrames = 0;
	// Cast dwLength to (long) to prevent C4018 warning
	for (long lPos = 0L; lPos < (long)dwLength; lPos = streamIn.NextKeyFrame(lPos)) {
		++iKeyFrames;
	}
	// for each image in the AVI input stream
	const long MIN_BRIGHTNESS = -255;
	const double DMAX_BRIGHTNESS = 255.0;
	double dIncr = DMAX_BRIGHTNESS/(iKeyFrames - 2);
	double dBrightness = 0.0;
	CxImage cxImage;
	// Cast dwLength to (long) to prevent C4018 warning
	for (long lPos = 0L; bResult && (lPos < (long)dwLength ); lPos = streamIn.NextSample(lPos)) {
		lpImage = streamIn.GetFrame(pGetFrame, lPos);
		lpBitmap = static_cast<LPBITMAPINFOHEADER>(lpImage);
		bResult = bResult && (0 != lpBitmap);
		bResult = bResult && cxImage.Decode((LPBYTE)lpImage, lpBitmap->biSizeImage, CXIMAGE_FORMAT_UNKNOWN);
		dBrightness -= dIncr;
		long lBrightness = (long)((MIN_BRIGHTNESS < dBrightness) ? dBrightness : MIN_BRIGHTNESS);
		bResult = bResult && cxImage.Light(lBrightness);
		hResult = bResult && aviCompressedStreamOut.Write(lPos, 1L, (LPBYTE)cxImage.GetDIB() + lOffsetImage, lpBitmap->biSizeImage, 0, 0, 0);
		CAVI::OnError(hResult);
		bResult = bResult && (AVIERR_OK == hResult);
		VideoCompressCallBack((100 * lPos)/dwLength);
	}

	hResult = streamIn.GetFrameClose(pGetFrame);
	CAVI::OnError(hResult);
	bResult = bResult && (AVIERR_OK == hResult);

	// Set Title Bar
	::AfxGetMainWnd()->SetWindowText(_T("CamStudio - Open Source"));

	return bResult;
}
