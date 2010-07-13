/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Recorder.h"
#include "Profile.h"
#include "TextAttributes.h"	// for position
#include "ImageAttributes.h"
#include "HotKey.h"
#include "CStudioLib.h"
#include "Camstudio4XNote.h"

bool bRecordPreset = false;

int iSaveLen = 0;	// dir len?
int iCursorLen = 0;	// dir len?
int iShapeNameInt = 1;
int iLayoutNameInt = 1;
int iFrameShift = 0;
int iPresetTime = 60;

sAudioFormat& sAudioFormat::operator=(const sAudioFormat& rhs)
{
	if (this == &rhs)
		return *this;

	m_uDeviceID				= rhs.m_uDeviceID;
	m_bCompression			= rhs.m_bCompression;
	m_bInterleaveFrames		= rhs.m_bInterleaveFrames;
	m_bUseMCI				= rhs.m_bUseMCI;
	m_bPerformAutoSearch	= rhs.m_bPerformAutoSearch;
	m_iRecordAudio			= rhs.m_iRecordAudio;
	m_iNumChannels			= rhs.m_iNumChannels;
	m_iBitsPerSample		= rhs.m_iBitsPerSample;
	m_iSamplesPerSeconds	= rhs.m_iSamplesPerSeconds;
	m_iInterleaveFactor		= rhs.m_iInterleaveFactor;
	m_iInterleavePeriod		= rhs.m_iInterleavePeriod;
	m_iMixerDevices			= rhs.m_iMixerDevices;
	m_iSelectedMixer		= rhs.m_iSelectedMixer;
	m_iFeedbackLine			= rhs.m_iFeedbackLine;
	m_iFeedbackLineInfo		= rhs.m_iFeedbackLineInfo;
	m_dwWaveinSelected		= rhs.m_dwWaveinSelected;
	m_wFormatTag			= rhs.m_wFormatTag;

	DeleteAudio();
	if (rhs.m_pwfx && rhs.m_dwCbwFX) {
		if (NewAudio()) {
			ASSERT(m_dwCbwFX == rhs.m_dwCbwFX);
			ASSERT(m_dwCbwFX == (sizeof(WAVEFORMATEX) + m_pwfx->cbSize));
			::CopyMemory(m_pwfx, rhs.m_pwfx, m_dwCbwFX);
			if (m_pwfx->wFormatTag != m_wFormatTag) {
				m_pwfx->wFormatTag = static_cast<WORD>(m_wFormatTag);
			}
		}
	} else {
		TRACE(_T("error! pointer and size mismatch!\n"));
	}
	return *this;
}

// special memory allocator for sAudioFormat
bool sAudioFormat::DeleteAudio()
{
	if (m_pwfx) {
		delete [] m_pwfx, m_pwfx = 0;
	}
	m_dwCbwFX = 0;
	return true;
}

bool sAudioFormat::NewAudio()
{
	bool bResult = DeleteAudio();
	if (!bResult)  {
		return bResult;
	}
	MMRESULT mmresult = ::acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &m_dwCbwFX);
	bResult = (MMSYSERR_NOERROR == mmresult);
	if (!bResult)  {
		return bResult;
	}

	m_pwfx = (LPWAVEFORMATEX)new char[m_dwCbwFX];
	bResult = (0 != m_pwfx);
	if (!bResult) {
		m_dwCbwFX = 0;
	} else {
		::ZeroMemory(m_pwfx, m_dwCbwFX);

		// Prevent C4244 warning and take some predictions for unwanted truncations
	    // m_pwfx->cbSize = m_dwCbwFX - sizeof(WAVEFORMATEX);  -> C4244
		DWORD tmpDWord = m_dwCbwFX - sizeof(WAVEFORMATEX);
		if ( tmpDWord > static_cast<WORD>(tmpDWord) ) {
			TRACE("Error: Casted value [%i] is not the same as uncasted [%l] ..!\n", static_cast<WORD>(tmpDWord),tmpDWord);
		}
		m_pwfx->cbSize = static_cast<WORD>(tmpDWord);
	}
	return bResult;
}

bool sAudioFormat::CopyAudio(LPWAVEFORMATEX pwfx, DWORD dwCbwFX)
{
	bool bResult = (0 != pwfx);
	if (!bResult)  {
		return bResult;
	}
	bResult = (m_dwCbwFX == dwCbwFX);
	if (!bResult)  {
		return bResult;
	}
	::CopyMemory(pwfx, m_pwfx, m_dwCbwFX);

	return bResult;
}

bool sAudioFormat::WriteAudio(const LPWAVEFORMATEX pwfx)
{
	bool bResult = (0 != pwfx);
	if (!bResult)  {
		return bResult;
	}
	bResult = NewAudio();
	if (!bResult)  {
		return bResult;
	}
	::CopyMemory(m_pwfx, pwfx, m_dwCbwFX);
	m_wFormatTag = m_pwfx->wFormatTag;

	return bResult;
}

void sAudioFormat::BuildRecordingFormat()
{
	WAVEFORMATEX& rFormat = AudioFormat();

	rFormat.wFormatTag		= WAVE_FORMAT_PCM;
	rFormat.wBitsPerSample	= static_cast<WORD>(m_iBitsPerSample);
	rFormat.nSamplesPerSec	= m_iSamplesPerSeconds;
	rFormat.nChannels		= static_cast<WORD>(m_iNumChannels);

	rFormat.nBlockAlign		= rFormat.nChannels * (rFormat.wBitsPerSample/8);
	rFormat.nAvgBytesPerSec	= rFormat.nSamplesPerSec * rFormat.nBlockAlign;
	rFormat.cbSize			= 0;
}

/////////////////////////////////////////////////////////////////////////////
namespace baseprofile {

template <>
CString ReadEntry(CString strFilename, CString strSection, CString strKeyName, const CString& DefValue)
{
	//TRACE("ReadEntry(CString): %s,\nSection: %s\nKey: %s\nValue: %s\n", strFilename, strSection, strKeyName, DefValue);
	const int BUFSIZE = 260;
	TCHAR szBuf[BUFSIZE];
	DWORD dwLen = ::GetPrivateProfileString(strSection, strKeyName, DefValue, szBuf, BUFSIZE, strFilename);
	return (dwLen) ? szBuf : TEXT("");
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const CString& Value)
{
	//TRACE("WriteEntry(CString): %s\nSection: %s\nKey: %s\nValue: %s\n", strFilename, strSection, strKeyName, Value);
	bool bDeleteSection = strKeyName.IsEmpty();
	bool bDeleteKey = Value.IsEmpty();
	bool bResult = (bDeleteSection || bDeleteKey);
	if (!bResult) {
		bResult = ::WritePrivateProfileString(strSection, strKeyName, Value, strFilename)
			? true : false;
	} else if (bDeleteSection) {
		bResult = ::WritePrivateProfileString(strSection, 0, 0, strFilename)
			? true : false;
	} else {
		bResult = ::WritePrivateProfileString(strSection, strKeyName, 0, strFilename)
			? true : false;
	}
	return bResult;
}

// int Read/Write
template <>
int ReadEntry(CString strFilename, CString strSection, CString strKeyName, const int& DefValue)
{
	//TRACE("ReadEntry(int): %s\nSection: %s\nKey: %s\nValue: %d\n", strFilename, strSection, strKeyName, DefValue);
	return ::GetPrivateProfileInt(strSection, strKeyName, DefValue, strFilename);
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const int& Value)
{
	//TRACE("WriteEntry(int): %s\nSection: %s\nKey: %s\nValue: %d\n", strFilename, strSection, strKeyName, Value);
	CString strValue;
	strValue.Format(TEXT("%d"), Value);
	return WriteEntry(strFilename, strSection, strKeyName, strValue);
}

// bool Read/Write
template <>
bool ReadEntry(CString strFilename, CString strSection, CString strKeyName, const bool& DefValue)
{
	//TRACE("ReadEntry(bool): %s\nSection: %s\nKey: %s\nValue: %d\n", strFilename, strSection, strKeyName, DefValue);
	return ::GetPrivateProfileInt(strSection, strKeyName, DefValue, strFilename)
		? true : false;
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const bool& Value)
{
	//TRACE("WriteEntry(bool): %s\nSection: %s\nKey: %s\nValue: %d\n", strFilename, strSection, strKeyName, Value);
	CString strValue;
	strValue.Format(TEXT("%d"), Value ? 1 : 0);
	return WriteEntry(strFilename, strSection, strKeyName, strValue);
}

////////////////////////////////////////////////////////////////////////////
// long Read/Write
////////////////////////////////////////////////////////////////////////////
template <>
long ReadEntry(CString strFilename, CString strSection, CString strKeyName, const long& DefValue)
{
	//TRACE("ReadEntry(long): %s,\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, DefValue);
	const int BUFSIZE = 128;
	TCHAR szBuf[BUFSIZE];
	DWORD dwLen = ::GetPrivateProfileString(strSection, strKeyName, _T(""), szBuf, BUFSIZE, strFilename);
	return (dwLen) ? ::_tstol(szBuf) : DefValue;
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const long& Value)
{
	//TRACE("WriteEntry(long): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, Value);
	CString strValue;
	strValue.Format(TEXT("%ld"), Value);
	return WriteEntry(strFilename, strSection, strKeyName, strValue);
}

////////////////////////////////////////////////////////////////////////////
// DWORD Read/Write
////////////////////////////////////////////////////////////////////////////
template <>
DWORD ReadEntry(CString strFilename, CString strSection, CString strKeyName, const DWORD& DefValue)
{
	//TRACE("ReadEntry(DWORD): %s,\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, DefValue);
	const int BUFSIZE = 128;
	TCHAR szBuf[BUFSIZE];
	DWORD dwLen = ::GetPrivateProfileString(strSection, strKeyName, _T(""), szBuf, BUFSIZE, strFilename);
	return (dwLen) ? ::_tstol(szBuf) : DefValue;
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const DWORD& Value)
{
	//TRACE("WriteEntry(DWORD): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, Value);
	CString strValue;
	strValue.Format(TEXT("%ld"), Value);
	return WriteEntry(strFilename, strSection, strKeyName, strValue);
}

////////////////////////////////////////////////////////////////////////////
// double Read/Write
////////////////////////////////////////////////////////////////////////////
template <>
double ReadEntry(CString strFilename, CString strSection, CString strKeyName, const double& DefValue)
{
	//TRACE("ReadEntry(double): %s,\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, DefValue);
	const int BUFSIZE = 128;
	TCHAR szBuf[BUFSIZE];
	DWORD dwLen = ::GetPrivateProfileString(strSection, strKeyName, _T(""), szBuf, BUFSIZE, strFilename);
	return (dwLen) ? ::_tstof(szBuf) : DefValue;
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const double& Value)
{
	//TRACE("WriteEntry(double): %s\nSection: %s\nKey: %s\nValue: %f\n", strFilename, strSection, strKeyName, Value);
	CString strValue;
	strValue.Format(TEXT("%f"), Value);
	return WriteEntry(strFilename, strSection, strKeyName, strValue);
}

////////////////////////////////////////////////////////////////////////////
// BYTE Read/Write
////////////////////////////////////////////////////////////////////////////
template <>
BYTE ReadEntry(CString strFilename, CString strSection, CString strKeyName, const BYTE& DefValue)
{
	//TRACE("ReadEntry(BYTE): %s,\nSection: %s\nKey: %s\nValue: %u\n", strFilename, strSection, strKeyName, DefValue);

	// Prevent C4244 warning and take some predictions for unwanted truncations
	// BYTE iVal = ::GetPrivateProfileInt(strSection, strKeyName, DefValue, strFilename); // C4244 Warning

	UINT tmpUINTValue = ::GetPrivateProfileInt(strSection, strKeyName, DefValue, strFilename);
	// Check if data is losted (Test only)
	if ( tmpUINTValue > static_cast<BYTE>(tmpUINTValue) ) {
		TRACE("Error: iVal, Casted value [%i] is not the same as uncasted [%l] ..!\n", static_cast<BYTE>(tmpUINTValue),tmpUINTValue);
	}
	// Assign casted value (check tracelog to validate if cast is aleways allowed)
	BYTE iVal = static_cast<BYTE>(tmpUINTValue);

	return iVal;
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const BYTE& Value)
{
	//TRACE("WriteEntry(BYTE): %s\nSection: %s\nKey: %s\nValue: %u\n", strFilename, strSection, strKeyName, Value);
	CString strValue;
	strValue.Format(TEXT("%u"), Value);
	return WriteEntry(strFilename, strSection, strKeyName, strValue);
}

////////////////////////////////////////////////////////////////////////////
// UINT Read/Write
////////////////////////////////////////////////////////////////////////////
template <>
UINT ReadEntry(CString strFilename, CString strSection, CString strKeyName, const UINT& DefValue)
{
	//TRACE("ReadEntry(UINT): %s,\nSection: %s\nKey: %s\nValue: %u\n", strFilename, strSection, strKeyName, DefValue);
	UINT iVal = ::GetPrivateProfileInt(strSection, strKeyName, DefValue, strFilename);
	return iVal;
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const UINT& Value)
{
	//TRACE("WriteEntry(BYTE): %s\nSection: %s\nKey: %s\nValue: %u\n", strFilename, strSection, strKeyName, Value);
	CString strValue;
	strValue.Format(TEXT("%u"), Value);
	return WriteEntry(strFilename, strSection, strKeyName, strValue);
}

////////////////////////////////////////////////////////////////////////////
// LANGID Read/Write
////////////////////////////////////////////////////////////////////////////
template <>
LANGID ReadEntry(CString strFilename, CString strSection, CString strKeyName, const LANGID& DefValue)
{
	//TRACE("ReadEntry(LANGID): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, DefValue);
	int iDefValue = DefValue;
	return LANGID(ReadEntry(strFilename, strSection, strKeyName, iDefValue));
}

template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const LANGID& Value)
{
	//TRACE("WriteEntry(LANGID): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, Value);
	int iDefValue = Value;
	return WriteEntry(strFilename, strSection, strKeyName, iDefValue);
}

////////////////////////////////////////////////////////////////////////////
// ePosition Read/Write
////////////////////////////////////////////////////////////////////////////
template <>
ePosition ReadEntry(CString strFilename, CString strSection, CString strKeyName, const ePosition& DefValue)
{
	//TRACE("ReadEntry(ePosition): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, DefValue);
	int iDefValue = DefValue;
	return ePosition(ReadEntry(strFilename, strSection, strKeyName, iDefValue));
}

template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const ePosition& Value)
{
	//TRACE("WriteEntry(ePosition): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, Value);
	int iDefValue = Value;
	return WriteEntry(strFilename, strSection, strKeyName, iDefValue);
}

////////////////////////////////////////////////////////////////////////////
// ePosType Read/Write
////////////////////////////////////////////////////////////////////////////
template <>
ePosType ReadEntry(CString strFilename, CString strSection, CString strKeyName, const ePosType& DefValue)
{
	//TRACE("ReadEntry(ePosType): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, DefValue);
	int iDefValue = DefValue;
	return ePosType(ReadEntry(strFilename, strSection, strKeyName, iDefValue));
}

template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const ePosType& Value)
{
	//TRACE("WriteEntry(ePosType): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, Value);
	int iDefValue = Value;
	return WriteEntry(strFilename, strSection, strKeyName, iDefValue);
}

////////////////////////////////////////////////////////////////////////////
// LOGFONT Read/Write
////////////////////////////////////////////////////////////////////////////
template <>
LOGFONT ReadEntry(CString strFilename, CString strSection, CString strKeyName, const LOGFONT& DefValue)
{
	//TRACE("ReadEntry(LOGFONT): %s,\nSection: %s\nKey: %s\nValue: \n", strFilename, strSection, strKeyName);

	LOGFONT lfResult = DefValue;
	CString strKeyNameEx;
	strKeyNameEx = strKeyName + _T("lfHeight");
	lfResult.lfHeight = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfHeight);
	strKeyNameEx = strKeyName + _T("lfWidth");
	lfResult.lfWidth = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfWidth);
	strKeyNameEx = strKeyName + _T("lfEscapement");
	lfResult.lfEscapement = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfEscapement);
	strKeyNameEx = strKeyName + _T("lfOrientation");
	lfResult.lfOrientation = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfOrientation);
	strKeyNameEx = strKeyName + _T("lfWeight");
	lfResult.lfWeight = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfWeight);
	strKeyNameEx = strKeyName + _T("lfItalic");
	lfResult.lfItalic = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfItalic);
	strKeyNameEx = strKeyName + _T("lfUnderline");
	lfResult.lfUnderline = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfUnderline);
	strKeyNameEx = strKeyName + _T("lfStrikeOut");
	lfResult.lfStrikeOut = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfStrikeOut);
	strKeyNameEx = strKeyName + _T("lfCharSet");
	lfResult.lfCharSet = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfCharSet);
	strKeyNameEx = strKeyName + _T("lfOutPrecision");
	lfResult.lfOutPrecision = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfOutPrecision);
	strKeyNameEx = strKeyName + _T("lfClipPrecision");
	lfResult.lfClipPrecision = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfClipPrecision);
	strKeyNameEx = strKeyName + _T("lfQuality");
	lfResult.lfQuality = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfQuality);
	strKeyNameEx = strKeyName + _T("lfPitchAndFamily");
	lfResult.lfPitchAndFamily = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfPitchAndFamily);
	strKeyNameEx = strKeyName + _T("lfFaceName");
	CString strFaceName(DefValue.lfFaceName);
	// TEST
	strFaceName = _T("Arial");
	strFaceName = ReadEntry(strFilename, strSection, strKeyNameEx, strFaceName);
	_tcscpy(lfResult.lfFaceName, strFaceName);
	//wcscpy_s(lfResult.lfFaceName, LF_FACESIZE, strFaceName);

	return lfResult;
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const LOGFONT& Value)
{
	//TRACE("WriteEntry(LOGFONT): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, Value);
	CString strKeyNameEx;
	strKeyNameEx = strKeyName + "lfHeight";
	bool bResult = WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfHeight);
	strKeyNameEx = strKeyName + "lfWidth";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfWidth);
	strKeyNameEx = strKeyName + "lfEscapement";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfEscapement);
	strKeyNameEx = strKeyName + "lfOrientation";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfOrientation);
	strKeyNameEx = strKeyName + "lfWeight";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfWeight);
	strKeyNameEx = strKeyName + "lfItalic";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfItalic);
	strKeyNameEx = strKeyName + "lfUnderline";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfUnderline);
	strKeyNameEx = strKeyName + "lfStrikeOut";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfStrikeOut);
	strKeyNameEx = strKeyName + "lfCharSet";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfCharSet);
	strKeyNameEx = strKeyName + "lfOutPrecision";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfOutPrecision);
	strKeyNameEx = strKeyName + "lfClipPrecision";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfClipPrecision);
	strKeyNameEx = strKeyName + "lfQuality";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfQuality);
	strKeyNameEx = strKeyName + "lfPitchAndFamily";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfPitchAndFamily);
	strKeyNameEx = strKeyName + "lfFaceName";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, CString(Value.lfFaceName));

	return bResult;
}

////////////////////////////////////////////////////////////////////////////
// TextAttributes Read/Write
////////////////////////////////////////////////////////////////////////////
template <>
TextAttributes ReadEntry(CString strFilename, CString strSection, CString strKeyName, const TextAttributes& DefValue)
{
	//TRACE("ReadEntry(TextAttributes): %s,\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, DefValue);
	TextAttributes taResult = DefValue;
	CString strKeyNameEx;
	strKeyNameEx = strKeyName + "position";
	taResult.position = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.position);

	strKeyNameEx = strKeyName + "posType";
	taResult.posType = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.posType);
	strKeyNameEx = strKeyName + "xPosRatio";
	taResult.xPosRatio = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.xPosRatio);
	strKeyNameEx = strKeyName + "yPosratio";
	taResult.yPosRatio = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.yPosRatio);

	strKeyNameEx = strKeyName + "text";
	taResult.text = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.text);
	strKeyNameEx = strKeyName + "backgroundColor";
	taResult.backgroundColor = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.backgroundColor);
	strKeyNameEx = strKeyName + "textColor";
	taResult.textColor = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.textColor);
	strKeyNameEx = strKeyName + "isFontSelected";
	taResult.isFontSelected = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.isFontSelected);
	strKeyNameEx = strKeyName + "logfont";
	taResult.logfont = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.logfont);

	return taResult;
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const TextAttributes& Value)
{
	//TRACE("WriteEntry(TextAttributes): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, Value);
	CString strKeyNameEx;
	strKeyNameEx = strKeyName + "position";
	bool bResult = WriteEntry(strFilename, strSection, strKeyNameEx, Value.position);

	strKeyNameEx = strKeyName + "posType";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.posType);
	strKeyNameEx = strKeyName + "xPosRatio";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.xPosRatio);
	strKeyNameEx = strKeyName + "yPosRatio";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.yPosRatio);

	strKeyNameEx = strKeyName + "text";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.text);
	strKeyNameEx = strKeyName + "backgroundColor";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.backgroundColor);
	strKeyNameEx = strKeyName + "textColor";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.textColor);
	strKeyNameEx = strKeyName + "isFontSelected";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.isFontSelected);
	strKeyNameEx = strKeyName + "logfont";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.logfont);
	return bResult;
}

////////////////////////////////////////////////////////////////////////////
// ImageAttributes Read/Write
////////////////////////////////////////////////////////////////////////////
template <>
ImageAttributes ReadEntry(CString strFilename, CString strSection, CString strKeyName, const ImageAttributes& DefValue)
{
	//TRACE("ReadEntry(ImageAttributes): %s,\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, DefValue);
	ImageAttributes iaResult = DefValue;
	CString strKeyNameEx;
	strKeyNameEx = strKeyName + "position";
	iaResult.position = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.position);

	strKeyNameEx = strKeyName + "posType";
	iaResult.posType = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.posType);
	strKeyNameEx = strKeyName + "xPosRatio";
	iaResult.xPosRatio = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.xPosRatio);
	strKeyNameEx = strKeyName + "yPosratio";
	iaResult.yPosRatio = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.yPosRatio);

	strKeyNameEx = strKeyName + "text";
	iaResult.text = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.text);
	return iaResult;
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const ImageAttributes& Value)
{
	//TRACE("WriteEntry(ImageAttributes): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, Value);
	CString strKeyNameEx;
	strKeyNameEx = strKeyName + "position";
	bool bResult = WriteEntry(strFilename, strSection, strKeyNameEx, Value.position);

	strKeyNameEx = strKeyName + "posType";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.posType);
	strKeyNameEx = strKeyName + "xPosRatio";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.xPosRatio);
	strKeyNameEx = strKeyName + "yPosRatio";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.yPosRatio);

	strKeyNameEx = strKeyName + "text";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.text);
	return bResult;
}

}	// namespace baseprofile

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CProfileSection::CProfileSection(const CString strSectionName)
: m_strSectionName(strSectionName)
{
}

CProfileSection::~CProfileSection()
{
}

// add item to specific section group
template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const CString& Value)
{
	return m_grpStrings.Add(iID, strName, Value);
}
template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const int& Value)
{
	return m_grpIntegers.Add(iID, strName, Value);
}
template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const bool& Value)
{
	return m_grpBools.Add(iID, strName, Value);
}
template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const long& Value)
{
	return m_grpLongs.Add(iID, strName, Value);
}
template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const DWORD& Value)
{
	return m_grpDWORD.Add(iID, strName, Value);
}
template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const double& Value)
{
	return m_grpDoubles.Add(iID, strName, Value);
}

template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const LANGID& Value)
{
	return m_grpLANGID.Add(iID, strName, Value);
}

template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const LOGFONT& Value)
{
	return m_grpLogFont.Add(iID, strName, Value);
}
template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const TextAttributes& Value)
{
	return m_grpTextAttribs.Add(iID, strName, Value);
}
template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const ImageAttributes& Value)
{
	return m_grpImageAttribs.Add(iID, strName, Value);
}

template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const UINT& Value)
{
	return m_grpUINT.Add(iID, strName, Value);
}

// read items from section groups
bool CProfileSection::Read(const CString strFile)
{
	//TRACE("## CProfileSection::Read\n");
	bool bResult = m_grpStrings.Read(strFile, m_strSectionName);
	bResult = bResult && m_grpIntegers.Read(strFile, m_strSectionName);
	bResult = bResult && m_grpBools.Read(strFile, m_strSectionName);
	bResult = bResult && m_grpUINT.Read(strFile, m_strSectionName);
	bResult = bResult && m_grpLongs.Read(strFile, m_strSectionName);
	bResult = bResult && m_grpDWORD.Read(strFile, m_strSectionName);
	bResult = bResult && m_grpDoubles.Read(strFile, m_strSectionName);
	bResult = bResult && m_grpLANGID.Read(strFile, m_strSectionName);
	bResult = bResult && m_grpLogFont.Read(strFile, m_strSectionName);
	bResult = bResult && m_grpTextAttribs.Read(strFile, m_strSectionName);
	bResult = bResult && m_grpImageAttribs.Read(strFile, m_strSectionName);
	return bResult;
}
// write items from section groups
bool CProfileSection::Write(const CString strFile)
{
	bool bResult = m_grpStrings.Write(strFile, m_strSectionName);
	bResult = bResult && m_grpIntegers.Write(strFile, m_strSectionName);
	bResult = bResult && m_grpUINT.Write(strFile, m_strSectionName);
	bResult = bResult && m_grpBools.Write(strFile, m_strSectionName);
	bResult = bResult && m_grpLongs.Write(strFile, m_strSectionName);
	bResult = bResult && m_grpDWORD.Write(strFile, m_strSectionName);
	bResult = bResult && m_grpDoubles.Write(strFile, m_strSectionName);
	bResult = bResult && m_grpLANGID.Write(strFile, m_strSectionName);
	bResult = bResult && m_grpLogFont.Write(strFile, m_strSectionName);
	bResult = bResult && m_grpTextAttribs.Write(strFile, m_strSectionName);
	bResult = bResult && m_grpImageAttribs.Write(strFile, m_strSectionName);
	return bResult;
}

// CGroupItem
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
const TCHAR * const LEGACY_SECTION = _T(" CamStudio Settings ver2.50 -- Please do not edit ");
const TCHAR * const APP_SECTION = _T("CamStudio");
const TCHAR * const VIDEO_SECTION = _T("Video");
const TCHAR * const AUDIO_SECTION = _T("Audio");
const TCHAR * const PROGRAMOPTIONS_SECTION = _T("ProgramOptions");
const TCHAR * const CURSOR_SECTION = _T("Cursor");
const TCHAR * const HOTKEY_SECTION = _T("HotKeys");
const TCHAR * const REGION_SECTION = _T("Region");
const TCHAR * const TIMESTAMP_SECTION = _T("TimeStamp");
const TCHAR * const XNOTE_SECTION = _T("XNote");
const TCHAR * const CAPTION_SECTION = _T("Caption");
const TCHAR * const WATERMARK_SECTION = _T("Watermark");

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CProfile::CProfile(const CString strFileName)
: m_strFileName(strFileName)
, m_SectionLegacy(LEGACY_SECTION)
, m_SectionApp(APP_SECTION)
, m_SectionVideo(VIDEO_SECTION)
, m_SectionAudio(AUDIO_SECTION)
, m_SectionCursor(CURSOR_SECTION)
, m_SectionHotkeys(HOTKEY_SECTION)
, m_SectionRegion(REGION_SECTION)
, m_SectionProgram(PROGRAMOPTIONS_SECTION)
, m_SectionTimeStamp(TIMESTAMP_SECTION)
, m_SectionXNote(XNOTE_SECTION)
, m_SectionCaption(CAPTION_SECTION)
, m_SectionWatermark(WATERMARK_SECTION)
{
	InitSections();
	InitLegacySection();

	//This push the current settings to the init file
	m_vAllSections.push_back(m_SectionLegacy);
	m_vAllSections.push_back(m_SectionApp);
	m_vAllSections.push_back(m_SectionProgram);
	m_vAllSections.push_back(m_SectionVideo);
	m_vAllSections.push_back(m_SectionAudio);
	m_vAllSections.push_back(m_SectionHotkeys);
	m_vAllSections.push_back(m_SectionCursor);
	m_vAllSections.push_back(m_SectionRegion);
	m_vAllSections.push_back(m_SectionTimeStamp);
	m_vAllSections.push_back(m_SectionXNote);
	m_vAllSections.push_back(m_SectionCaption);
	m_vAllSections.push_back(m_SectionWatermark);
}

CProfile::~CProfile()
{
	Write();
	// delete the legacy section
	//m_SectionLegacy.Delete(m_strFileName);
}

void CProfile::InitSections()
{
	//TRACE("## CProfile::InitSections()\n");
	Add(LANGUAGE, "language", LANGID(STANDARD_LANGID));
	Add(FRAMESHIFT, "frameshift", 0);	// TODO: obsolete

	Add(m_SectionAudio, AUDIODEVICEID, "AudioDeviceID", 0U);
	Add(m_SectionAudio, CBWFX, "cbwfx", 50UL);
	Add(m_SectionAudio, RECORDAUDIO, "recordaudio", 0);
	Add(m_SectionAudio, WAVEINSELECTED, "waveinselected", 128UL);
	Add(m_SectionAudio, AUDIO_BITS_PER_SAMPLE, "audio_bits_per_sample", 16);
	Add(m_SectionAudio, AUDIO_NUM_CHANNELS, "audio_num_channels", 2);
	Add(m_SectionAudio, AUDIO_SAMPLES_PER_SECONDS, "audio_samples_per_seconds", 22050);
	Add(m_SectionAudio, INTERLEAVEFRAMES, "interleaveFrames", true);
	Add(m_SectionAudio, INTERLEAVEFACTOR, "interleaveFactor", 100);
	Add(m_SectionAudio, INTERLEAVEUNIT, "interleaveUnit", 1);
	Add(m_SectionAudio, USEMCI, "useMCI", false);
	Add(m_SectionAudio, NUMDEV, "NumDev", 0);
	Add(m_SectionAudio, SELECTEDDEV, "SelectedDev", 0);
	Add(m_SectionAudio, COMPRESSFORMATTAG, "CompressionFormatTag", 0);
	Add(m_SectionAudio, FEEDBACK_LINE, "feedback_line", 0);
	Add(m_SectionAudio, FEEDBACK_LINE_INFO, "feedback_line_info", 0);
	Add(m_SectionAudio, BAUDIOCOMPRESSION, "bAudioCompression", true);
	Add(m_SectionAudio, PERFORMAUTOSEARCH, "performAutoSearch", true);

	// producer - left in general app
	Add(LAUNCHPROPPROMPT, "launchPropPrompt", false);
	Add(LAUNCHHTMLPLAYER, "launchHTMLPlayer", true);
	Add(DELETEAVIAFTERUSE, "deleteAVIAfterUse", true);

	// region
	Add(m_SectionRegion, FIXEDCAPTURE, "FixedCapture", false);
	Add(m_SectionRegion, SUPPORTMOUSEDRAG, "SupportMouseDrag", true);
	Add(m_SectionRegion, CAPTURELEFT, "CaptureLeft", 100);
	Add(m_SectionRegion, CAPTURETOP, "CaptureTop", 100);
	Add(m_SectionRegion, CAPTUREWIDTH, "CaptureWidth", 320);
	Add(m_SectionRegion, CAPTUREHEIGHT, "CaptureHeight", 240);
	Add(m_SectionRegion, MOUSECAPTUREMODE, "MouseCaptureMode", 0);

	// hotkeys
	Add(m_SectionHotkeys, KEYRECORDSTART, "keyRecordStart", (UINT)VK_F8);
	Add(m_SectionHotkeys, KEYRECORDSTARTCTRL, "keyRecordStartCtrl", false);
	Add(m_SectionHotkeys, KEYRECORDSTARTALT, "keyRecordStartAlt", false);
	Add(m_SectionHotkeys, KEYRECORDSTARTSHIFT, "keyRecordStartShift", false);

	Add(m_SectionHotkeys, KEYRECORDEND, "keyRecordEnd", (UINT)VK_F9);
	Add(m_SectionHotkeys, KEYRECORDENDCTRL, "keyRecordEndCtrl", false);
	Add(m_SectionHotkeys, KEYRECORDENDALT, "keyRecordEndAlt", false);
	Add(m_SectionHotkeys, KEYRECORDENDSHIFT, "keyRecordEndShift", false);

	Add(m_SectionHotkeys, KEYRECORDCANCEL, "uKeyRecordCancel", (UINT)VK_F10);
	Add(m_SectionHotkeys, KEYRECORDCANCELCTRL, "keyRecordCancelCtrl", false);
	Add(m_SectionHotkeys, KEYRECORDCANCELALT, "keyRecordCancelAlt", false);
	Add(m_SectionHotkeys, KEYRECORDCANCELSHIFT, "keyRecordCancelShift", false);

	Add(m_SectionHotkeys, KEYNEXT, "keyNext", (UINT)VK_F11);
	Add(m_SectionHotkeys, KEYNEXTCTRL, "keyNextCtrl", false);
	Add(m_SectionHotkeys, KEYNEXTALT, "keyNextAlt", false);
	Add(m_SectionHotkeys, KEYNEXTSHIFT, "keyNextShift", false);

	Add(m_SectionHotkeys, KEYPREV, "keyPrev", (UINT)VK_UNDEFINED);
	Add(m_SectionHotkeys, KEYPREVCTRL, "keyPrevCtrl", false);
	Add(m_SectionHotkeys, KEYPREVALT, "keyPrevAlt", false);
	Add(m_SectionHotkeys, KEYPREVSHIFT, "keyPrevShift", false);

	Add(m_SectionHotkeys, KEYSHOWLAYOUT, "keyShowLayout", (UINT)VK_UNDEFINED);
	Add(m_SectionHotkeys, KEYSHOWLAYOUTCTRL, "keyShowLayoutCtrl", false);
	Add(m_SectionHotkeys, KEYSHOWLAYOUTALT, "keyShowLayoutAlt", false);
	Add(m_SectionHotkeys, KEYSHOWLAYOUTSHIFT, "keyShowLayoutShift", false);

	// program options
	Add(m_SectionProgram, AUTONAMING, "AutoNaming", false);
	Add(m_SectionProgram, CAPTURETRANS, "CaptureTrans", true);
	Add(m_SectionProgram, FLASHINGRECT, "FlashingRect", true);
	Add(m_SectionProgram, MINIMIZEONSTART, "MinimizeOnStart", false);
	Add(m_SectionProgram, PRESETTIME, "presettime", 0);
	Add(m_SectionProgram, RECORDPRESET, "recordpreset", false);
	Add(m_SectionProgram, RECORDINGMODE, "RecordingMode", 0);
	Add(m_SectionProgram, LAUNCHPLAYER, "LaunchPlayer", 3);
	Add(m_SectionProgram, SPECIFIEDDIR, "SpecifiedDir", CString(_T("")));
	Add(m_SectionProgram, TEMPPATH_ACCESS, "TempPathAccess", 0);
	Add(m_SectionProgram, THREADPRIORITY, "ThreadPriority", 0);
	Add(m_SectionProgram, AUTOPAN, "autopan", false);
	Add(m_SectionProgram, MAXPAN, "maxpan", 0);
	Add(m_SectionProgram, SHAPENAMEINT, "shapeNameInt", 0);
	Add(m_SectionProgram, SHAPENAMELEN, "shapeNameLen", 6);
	Add(m_SectionProgram, LAYOUTNAMEINT, "layoutNameInt", 0);
	Add(m_SectionProgram, LAYOUTNAMELEN, "g_layoutNameLen", 7);
	Add(m_SectionProgram, VIEWTYPE, "viewtype", 0);

	// video options
	Add(m_SectionVideo, RESTRICTVIDEOCODECS, "restrictVideoCodecs", false);
	Add(m_SectionVideo, AUTOADJUST, "AutoAdjust", true);
	Add(m_SectionVideo, VALUEADJUST, "ValueAdjust", 0);
	Add(m_SectionVideo, TIMELAPSE, "TimeLapse", 0);
	Add(m_SectionVideo, FRAMES_PER_SECOND, "FramesPerSecond", 0);
	Add(m_SectionVideo, KEYFRAMEINTERVAL, "KeyFramesEvery", 0);
	Add(m_SectionVideo, COMPQUALITY, "CompQuality", 0);
	Add(m_SectionVideo, COMPFCCHANDLER, "CompFCCHandler", 0UL);
	Add(m_SectionVideo, COMPRESSORSTATEISFOR, "CompressorStateIsFor", 0UL);
	Add(m_SectionVideo, COMPRESSORSTATESIZE, "CompressorStateSize", 0UL);
	Add(m_SectionVideo, SHIFTTYPE, "shiftType", 0);
	Add(m_SectionVideo, TIMESHIFT, "timeshift", 0);

	Add(m_SectionCursor, RECORDCURSOR, "RecordCursor", true);
	Add(m_SectionCursor, CURSORTYPE, "CursorType", 0);
	Add(m_SectionCursor, CUSTOMSEL, "CustomSel", 0);
	Add(m_SectionCursor, HIGHLIGHTCURSOR, "HighlightCursor", false);
	Add(m_SectionCursor, HIGHLIGHTSIZE, "HighlightSize", 0);
	Add(m_SectionCursor, HIGHLIGHTSHAPE, "HighlightShape", 0);
	Add(m_SectionCursor, HIGHLIGHTCOLOR, "HighlightColor", RGB(255, 255, 125));
	Add(m_SectionCursor, HIGHLIGHTCLICK, "HighlightClick", false);
	Add(m_SectionCursor, HIGHLIGHTCLICKCOLORLEFT, "ClickColorLeft", RGB(255, 0, 0));
	Add(m_SectionCursor, HIGHLIGHTCLICKCOLORRIGHT, "ClickColorRight", RGB(0, 0, 255));
	Add(m_SectionCursor, SAVEDIR, "SaveDir", 25);		// TODO: should be string
	Add(m_SectionCursor, CURSORDIR, "CursorDir", 18);	// TODO: should be string

	Add(m_SectionTimeStamp, TIMESTAMPANNOTATION, "Annotation", false);
	Add(m_SectionTimeStamp, TIMESTAMPTEXTATTRIBUTES, "TextAttributes", TextAttributes(TOP_LEFT));

	// Xnote Stopwatch 
#ifdef CAMSTUDIO4XNOTE
	Add(m_SectionXNote, XNOTEANNOTATION, "xnoteAnnotation", true);
	Add(m_SectionXNote, XNOTEREMOTECONTROL, "xnoteRemoteControl", true);		
#else
	Add(m_SectionXNote, XNOTEANNOTATION, "xnoteAnnotation", false);
	Add(m_SectionXNote, XNOTEREMOTECONTROL, "xnoteRemoteControl", false);		
#endif
	Add(m_SectionXNote, XNOTEDISPLAYCAMERADELAY, "xnoteDisplayCameraDelay", true);		
	Add(m_SectionXNote, XNOTECAMERADELAYINMILLISEC, "xnoteCameraDelayInMilliSec", 175UL);		
	Add(m_SectionXNote, XNOTEDISPLAYFORMATSTRING, "xnoteDisplayFormatString", CString(_T("(0000)  00:00:00.000")) );   //String required, defines length printable area. Empty String is No Annotationbox   // hh:mm:ss.ttt
	Add(m_SectionXNote, XNOTERECORDDURATIONLIMITMODE, "xnoteRecordDurationLimitMode", true );		
	Add(m_SectionXNote, XNOTERECORDDURATIONLIMITINMILLISEC, "xnoteRecordDurationLimitInMilliSec", 1750UL  ); // Default timevalue required, otherwise recording stops direct when it is started
	Add(m_SectionXNote, XNOTETEXTATTRIBUTES, "TextAttributes", TextAttributes(BOTTOM_LEFT));

	Add(m_SectionCaption, CAPTIONANNOTATION, "Annotation", false);
	Add(m_SectionCaption, CAPTIONTEXTATTRIBUTES, "TextAttributes", TextAttributes(TOP_LEFT));

	Add(m_SectionWatermark, WATERMARKANNOTATION, "watermarkAnnotation", false);
	Add(m_SectionWatermark, WATERMARKIMAGEATTRIBUTES, "WatermarkImageAttributes", ImageAttributes(TOP_LEFT));
}

void CProfile::InitLegacySection()
{
	VERIFY(Add(m_SectionLegacy, FLASHINGRECT, "flashingRect", false));
	VERIFY(Add(m_SectionLegacy, LAUNCHPLAYER, "launchPlayer", 0));
	VERIFY(Add(m_SectionLegacy, MINIMIZEONSTART, "minimizeOnStart", false));
	VERIFY(Add(m_SectionLegacy, MOUSECAPTUREMODE, "MouseCaptureMode", 0));
	VERIFY(Add(m_SectionLegacy, CAPTUREWIDTH, "capturewidth", 0));
	VERIFY(Add(m_SectionLegacy, CAPTUREHEIGHT, "captureheight", 0));
	VERIFY(Add(m_SectionLegacy, TIMELAPSE, "timelapse", 0));
	VERIFY(Add(m_SectionLegacy, FRAMES_PER_SECOND, "frames_per_second", 0));
	VERIFY(Add(m_SectionLegacy, KEYFRAMEINTERVAL, "keyFramesEvery", 0));
	VERIFY(Add(m_SectionLegacy, COMPQUALITY, "compquality", 0));
	VERIFY(Add(m_SectionLegacy, COMPFCCHANDLER, "compfccHandler", 0UL));
	VERIFY(Add(m_SectionLegacy, COMPRESSORSTATEISFOR, "CompressorStateIsFor", 0UL));
	VERIFY(Add(m_SectionLegacy, COMPRESSORSTATESIZE, "CompressorStateSize", 0UL));
	VERIFY(Add(m_SectionLegacy, RECORDCURSOR, "g_recordcursor", true));
	VERIFY(Add(m_SectionLegacy, CUSTOMSEL, "g_customsel", 0));
	VERIFY(Add(m_SectionLegacy, CURSORTYPE, "g_cursortype", 0));
	VERIFY(Add(m_SectionLegacy, HIGHLIGHTCURSOR, "g_highlightcursor", false));
	VERIFY(Add(m_SectionLegacy, HIGHLIGHTSIZE, "g_highlightsize", 0));
	VERIFY(Add(m_SectionLegacy, HIGHLIGHTSHAPE, "g_highlightshape", 0));
	VERIFY(Add(m_SectionLegacy, HIGHLIGHTCLICK, "g_highlightclick", false));
	VERIFY(Add(m_SectionLegacy, HIGHLIGHTCOLORR, "g_highlightcolorR", 255));
	VERIFY(Add(m_SectionLegacy, HIGHLIGHTCOLORG, "g_highlightcolorG", 255));
	VERIFY(Add(m_SectionLegacy, HIGHLIGHTCOLORB, "g_highlightcolorB", 125));
	VERIFY(Add(m_SectionLegacy, HIGHLIGHTCLICKCOLORLEFTR, "g_highlightclickcolorleftR", 255));
	VERIFY(Add(m_SectionLegacy, HIGHLIGHTCLICKCOLORLEFTG, "g_highlightclickcolorleftG", 0));
	VERIFY(Add(m_SectionLegacy, HIGHLIGHTCLICKCOLORLEFTB, "g_highlightclickcolorleftB", 0));
	VERIFY(Add(m_SectionLegacy, HIGHLIGHTCLICKCOLORRIGHTR, "g_highlightclickcolorrightR", 0));
	VERIFY(Add(m_SectionLegacy, HIGHLIGHTCLICKCOLORRIGHTG, "g_highlightclickcolorrightG", 0));
	VERIFY(Add(m_SectionLegacy, HIGHLIGHTCLICKCOLORRIGHTB, "g_highlightclickcolorrightB", 255));
	VERIFY(Add(m_SectionLegacy, AUTOPAN, "autopan", false));
	VERIFY(Add(m_SectionLegacy, MAXPAN, "maxpan", 0));
	VERIFY(Add(m_SectionLegacy, AUDIODEVICEID, "AudioDeviceID", 0U));
	VERIFY(Add(m_SectionLegacy, CBWFX, "cbwfx", 50UL));
	VERIFY(Add(m_SectionLegacy, RECORDAUDIO, "recordaudio", 0));
	VERIFY(Add(m_SectionLegacy, WAVEINSELECTED, "waveinselected", 128UL));
	VERIFY(Add(m_SectionLegacy, AUDIO_BITS_PER_SAMPLE, "audio_bits_per_sample", 16));
	VERIFY(Add(m_SectionLegacy, AUDIO_NUM_CHANNELS, "audio_num_channels", 2));
	VERIFY(Add(m_SectionLegacy, AUDIO_SAMPLES_PER_SECONDS, "audio_samples_per_seconds", 22050));
	VERIFY(Add(m_SectionLegacy, BAUDIOCOMPRESSION, "bAudioCompression", true));
	VERIFY(Add(m_SectionLegacy, INTERLEAVEFRAMES, "interleaveFrames", true));
	VERIFY(Add(m_SectionLegacy, INTERLEAVEFACTOR, "interleaveFactor", 100));
	VERIFY(Add(m_SectionLegacy, KEYRECORDSTART, "keyRecordStart", (UINT)0));
	VERIFY(Add(m_SectionLegacy, KEYRECORDEND, "keyRecordEnd", (UINT)VK_UNDEFINED));
	VERIFY(Add(m_SectionLegacy, KEYRECORDCANCEL, "keyRecordCancel", (UINT)VK_UNDEFINED));
	VERIFY(Add(m_SectionLegacy, VIEWTYPE, "viewtype", 0));
	VERIFY(Add(m_SectionLegacy, AUTOADJUST, "g_autoadjust", true));
	VERIFY(Add(m_SectionLegacy, VALUEADJUST, "g_valueadjust", 0));
	VERIFY(Add(m_SectionLegacy, SAVEDIR, "savedir", 25));
	VERIFY(Add(m_SectionLegacy, CURSORDIR, "cursordir", 18));
	VERIFY(Add(m_SectionLegacy, THREADPRIORITY, "threadPriority", 0));
	VERIFY(Add(m_SectionLegacy, CAPTURELEFT, "captureleft", 0));
	VERIFY(Add(m_SectionLegacy, CAPTURETOP, "capturetop", 0));
	VERIFY(Add(m_SectionLegacy, FIXEDCAPTURE, "fixedcapture", false));
	VERIFY(Add(m_SectionLegacy, INTERLEAVEUNIT, "interleaveUnit", 1));
	VERIFY(Add(m_SectionLegacy, TEMPPATH_ACCESS, "tempPath_Access", 0));
	VERIFY(Add(m_SectionLegacy, CAPTURETRANS, "captureTrans", true));
	VERIFY(Add(m_SectionLegacy, SPECIFIEDDIR, "specifieddir", CString(_T(""))));
	VERIFY(Add(m_SectionLegacy, NUMDEV, "NumDev", 0));
	VERIFY(Add(m_SectionLegacy, SELECTEDDEV, "SelectedDev", 0));
	VERIFY(Add(m_SectionLegacy, FEEDBACK_LINE, "feedback_line", 0));
	VERIFY(Add(m_SectionLegacy, FEEDBACK_LINE_INFO, "feedback_line_info", 0));
	VERIFY(Add(m_SectionLegacy, PERFORMAUTOSEARCH, "performAutoSearch", true));
	VERIFY(Add(m_SectionLegacy, SUPPORTMOUSEDRAG, "supportMouseDrag", true));
	VERIFY(Add(m_SectionLegacy, KEYRECORDSTARTCTRL, "keyRecordStartCtrl", false));
	VERIFY(Add(m_SectionLegacy, KEYRECORDENDCTRL, "keyRecordEndCtrl", false));
	VERIFY(Add(m_SectionLegacy, KEYRECORDCANCELCTRL, "keyRecordCancelCtrl", false));
	VERIFY(Add(m_SectionLegacy, KEYRECORDSTARTALT, "keyRecordStartAlt", false));
	VERIFY(Add(m_SectionLegacy, KEYRECORDENDALT, "keyRecordEndAlt", false));
	VERIFY(Add(m_SectionLegacy, KEYRECORDCANCELALT, "keyRecordCancelAlt", false));
	VERIFY(Add(m_SectionLegacy, KEYRECORDSTARTSHIFT, "keyRecordStartShift", false));
	VERIFY(Add(m_SectionLegacy, KEYRECORDENDSHIFT, "keyRecordEndShift", false));
	VERIFY(Add(m_SectionLegacy, KEYRECORDCANCELSHIFT, "keyRecordCancelShift", false));
	VERIFY(Add(m_SectionLegacy, KEYNEXT, "keyNext", (UINT)VK_UNDEFINED));
	VERIFY(Add(m_SectionLegacy, KEYPREV, "keyPrev", (UINT)VK_UNDEFINED));
	VERIFY(Add(m_SectionLegacy, KEYSHOWLAYOUT, "keyShowLayout", (UINT)VK_UNDEFINED));
	VERIFY(Add(m_SectionLegacy, KEYNEXTCTRL, "keyNextCtrl", false));
	VERIFY(Add(m_SectionLegacy, KEYPREVCTRL, "keyPrevCtrl", false));
	VERIFY(Add(m_SectionLegacy, KEYSHOWLAYOUTCTRL, "keyShowLayoutCtrl", false));
	VERIFY(Add(m_SectionLegacy, KEYNEXTALT, "keyNextAlt", false));
	VERIFY(Add(m_SectionLegacy, KEYPREVALT, "keyPrevAlt", false));
	VERIFY(Add(m_SectionLegacy, KEYSHOWLAYOUTALT, "keyShowLayoutAlt", false));
	VERIFY(Add(m_SectionLegacy, KEYNEXTSHIFT, "keyNextShift", false));
	VERIFY(Add(m_SectionLegacy, KEYPREVSHIFT, "keyPrevShift", false));
	VERIFY(Add(m_SectionLegacy, KEYSHOWLAYOUTSHIFT, "keyShowLayoutShift", false));
	VERIFY(Add(m_SectionLegacy, SHAPENAMEINT, "shapeNameInt", 0));
	VERIFY(Add(m_SectionLegacy, SHAPENAMELEN, "shapeNameLen", 6));
	VERIFY(Add(m_SectionLegacy, LAYOUTNAMEINT, "layoutNameInt", 0));
	VERIFY(Add(m_SectionLegacy, LAYOUTNAMELEN, "g_layoutNameLen", 7));
	VERIFY(Add(m_SectionLegacy, USEMCI, "useMCI", false));
	VERIFY(Add(m_SectionLegacy, SHIFTTYPE, "shiftType", 0));
	VERIFY(Add(m_SectionLegacy, TIMESHIFT, "timeshift", 0));
	VERIFY(Add(m_SectionLegacy, FRAMESHIFT, "frameshift", 0));
	VERIFY(Add(m_SectionLegacy, LAUNCHPROPPROMPT, "launchPropPrompt", false));
	VERIFY(Add(m_SectionLegacy, LAUNCHHTMLPLAYER, "launchHTMLPlayer", true));
	VERIFY(Add(m_SectionLegacy, DELETEAVIAFTERUSE, "deleteAVIAfterUse", true));
	VERIFY(Add(m_SectionLegacy, RECORDINGMODE, "RecordingMode", 0));
	VERIFY(Add(m_SectionLegacy, AUTONAMING, "autonaming", false));
	VERIFY(Add(m_SectionLegacy, RESTRICTVIDEOCODECS, "restrictVideoCodecs", false));
	VERIFY(Add(m_SectionLegacy, PRESETTIME, "presettime", 0));
	VERIFY(Add(m_SectionLegacy, RECORDPRESET, "recordpreset", false));
	VERIFY(Add(m_SectionLegacy, LANGUAGE, "language", LANGID(STANDARD_LANGID)));

	VERIFY(Add(m_SectionLegacy, TIMESTAMPANNOTATION, "timestampAnnotation", false));
	VERIFY(Add(m_SectionLegacy, TIMESTAMPBACKCOLOR, "timestampBackColor", 0));
	VERIFY(Add(m_SectionLegacy, TIMESTAMPSELECTED, "timestampSelected", 0));
	VERIFY(Add(m_SectionLegacy, TIMESTAMPPOSITION, "timestampPosition", 0));
	VERIFY(Add(m_SectionLegacy, TIMESTAMPTEXTCOLOR, "timestampTextColor", (COLORREF)(16777215)));
	VERIFY(Add(m_SectionLegacy, TIMESTAMPTEXTWEIGHT, "timestampTextWeight", 0));
	VERIFY(Add(m_SectionLegacy, TIMESTAMPTEXTHEIGHT, "timestampTextHeight", 0));
	VERIFY(Add(m_SectionLegacy, TIMESTAMPTEXTWIDTH, "timestampTextWidth", 0));
	//VERIFY(Add(m_SectionLegacy, TIMESTAMPTEXTFONT, CString(_T("timestampTextFont")), CString(_T("Arial"))));
	VERIFY(Add(m_SectionLegacy, TIMESTAMPTEXTFONT, "timestampTextFont", CString(_T("Arial"))));

	VERIFY(Add(m_SectionLegacy, CAPTIONANNOTATION, "captionAnnotation", false));
	VERIFY(Add(m_SectionLegacy, CAPTIONBACKCOLOR, "captionBackColor", 0));
	VERIFY(Add(m_SectionLegacy, CAPTIONSELECTED, "captionSelected", 0));
	VERIFY(Add(m_SectionLegacy, CAPTIONPOSITION, "captionPosition", 0));
	VERIFY(Add(m_SectionLegacy, CAPTIONTEXTCOLOR, "captionTextColor", (COLORREF)(16777215)));
	VERIFY(Add(m_SectionLegacy, CAPTIONTEXTWEIGHT, "captionTextWeight", 0));
	VERIFY(Add(m_SectionLegacy, CAPTIONTEXTHEIGHT, "captionTextHeight", 0));
	VERIFY(Add(m_SectionLegacy, CAPTIONTEXTWIDTH, "captionTextWidth", 0));
	//VERIFY(Add(m_SectionLegacy, CAPTIONTEXTFONT, CString(_T("captionTextFont")), CString(_T("Arial"))));
	VERIFY(Add(m_SectionLegacy, CAPTIONTEXTFONT, "captionTextFont", CString(_T("Arial"))));
	
	VERIFY(Add(m_SectionLegacy, WATERMARKANNOTATION, "watermarkAnnotation", false));
}

bool CProfile::Convert()
{
	//TRACE("## CProfile::Convert\n");
	// copy conversion
	VERIFY(Convert(m_SectionLegacy, FLASHINGRECT, false));
	VERIFY(Convert(m_SectionLegacy, LAUNCHPLAYER, 0));
	VERIFY(Convert(m_SectionLegacy, MINIMIZEONSTART, false));
	VERIFY(Convert(m_SectionLegacy, MOUSECAPTUREMODE, 0));
	VERIFY(Convert(m_SectionLegacy, CAPTUREWIDTH, 0));
	VERIFY(Convert(m_SectionLegacy, CAPTUREHEIGHT, 0));
	VERIFY(Convert(m_SectionLegacy, TIMELAPSE, 0));
	VERIFY(Convert(m_SectionLegacy, FRAMES_PER_SECOND, 0));
	VERIFY(Convert(m_SectionLegacy, KEYFRAMEINTERVAL, 0));
	VERIFY(Convert(m_SectionLegacy, COMPQUALITY, 0));
	VERIFY(Convert(m_SectionLegacy, COMPFCCHANDLER, 0UL));
	VERIFY(Convert(m_SectionLegacy, COMPRESSORSTATEISFOR, 0UL));
	VERIFY(Convert(m_SectionLegacy, COMPRESSORSTATESIZE, 0UL));
	VERIFY(Convert(m_SectionLegacy, RECORDCURSOR, true));
	VERIFY(Convert(m_SectionLegacy, CUSTOMSEL, 0));
	VERIFY(Convert(m_SectionLegacy, CURSORTYPE, 0));
	VERIFY(Convert(m_SectionLegacy, HIGHLIGHTCURSOR, false));
	VERIFY(Convert(m_SectionLegacy, HIGHLIGHTSIZE, 0));
	VERIFY(Convert(m_SectionLegacy, HIGHLIGHTSHAPE, 0));
	VERIFY(Convert(m_SectionLegacy, HIGHLIGHTCLICK, false));
	VERIFY(Convert(m_SectionLegacy, AUTOPAN, false));
	VERIFY(Convert(m_SectionLegacy, MAXPAN, 0));
	VERIFY(Convert(m_SectionLegacy, AUDIODEVICEID, 0U));
	VERIFY(Convert(m_SectionLegacy, CBWFX, 50UL));
	VERIFY(Convert(m_SectionLegacy, RECORDAUDIO, 0));
	VERIFY(Convert(m_SectionLegacy, WAVEINSELECTED, 128UL));
	VERIFY(Convert(m_SectionLegacy, AUDIO_BITS_PER_SAMPLE, 16));
	VERIFY(Convert(m_SectionLegacy, AUDIO_NUM_CHANNELS, 2));
	VERIFY(Convert(m_SectionLegacy, AUDIO_SAMPLES_PER_SECONDS, 22050));
	VERIFY(Convert(m_SectionLegacy, BAUDIOCOMPRESSION, true));
	VERIFY(Convert(m_SectionLegacy, INTERLEAVEFRAMES, true));
	VERIFY(Convert(m_SectionLegacy, INTERLEAVEFACTOR, 100));
	VERIFY(Convert(m_SectionLegacy, KEYRECORDSTART, (UINT)0));
	VERIFY(Convert(m_SectionLegacy, KEYRECORDEND, (UINT)VK_UNDEFINED));
	VERIFY(Convert(m_SectionLegacy, KEYRECORDCANCEL, (UINT)VK_UNDEFINED));
	VERIFY(Convert(m_SectionLegacy, VIEWTYPE, 0));
	VERIFY(Convert(m_SectionLegacy, AUTOADJUST, true));
	VERIFY(Convert(m_SectionLegacy, VALUEADJUST, 0));
	VERIFY(Convert(m_SectionLegacy, SAVEDIR, 25));
	VERIFY(Convert(m_SectionLegacy, CURSORDIR, 18));
	VERIFY(Convert(m_SectionLegacy, THREADPRIORITY, 0));
	VERIFY(Convert(m_SectionLegacy, CAPTURELEFT, 0));
	VERIFY(Convert(m_SectionLegacy, CAPTURETOP, 0));
	VERIFY(Convert(m_SectionLegacy, FIXEDCAPTURE, false));
	VERIFY(Convert(m_SectionLegacy, INTERLEAVEUNIT, 1));
	VERIFY(Convert(m_SectionLegacy, TEMPPATH_ACCESS, 0));
	VERIFY(Convert(m_SectionLegacy, CAPTURETRANS, true));
	VERIFY(Convert(m_SectionLegacy, SPECIFIEDDIR, CString(_T(""))));
	VERIFY(Convert(m_SectionLegacy, NUMDEV, 0));
	VERIFY(Convert(m_SectionLegacy, SELECTEDDEV, 0));
	VERIFY(Convert(m_SectionLegacy, FEEDBACK_LINE, 0));
	VERIFY(Convert(m_SectionLegacy, FEEDBACK_LINE_INFO, 0));
	VERIFY(Convert(m_SectionLegacy, PERFORMAUTOSEARCH, true));
	VERIFY(Convert(m_SectionLegacy, SUPPORTMOUSEDRAG, true));
	VERIFY(Convert(m_SectionLegacy, KEYRECORDSTARTCTRL, false));
	VERIFY(Convert(m_SectionLegacy, KEYRECORDENDCTRL, false));
	VERIFY(Convert(m_SectionLegacy, KEYRECORDCANCELCTRL, false));
	VERIFY(Convert(m_SectionLegacy, KEYRECORDSTARTALT, false));
	VERIFY(Convert(m_SectionLegacy, KEYRECORDENDALT, false));
	VERIFY(Convert(m_SectionLegacy, KEYRECORDCANCELALT, false));
	VERIFY(Convert(m_SectionLegacy, KEYRECORDSTARTSHIFT, false));
	VERIFY(Convert(m_SectionLegacy, KEYRECORDENDSHIFT, false));
	VERIFY(Convert(m_SectionLegacy, KEYRECORDCANCELSHIFT, false));
	VERIFY(Convert(m_SectionLegacy, KEYNEXT, (UINT)VK_UNDEFINED));
	VERIFY(Convert(m_SectionLegacy, KEYPREV, (UINT)VK_UNDEFINED));
	VERIFY(Convert(m_SectionLegacy, KEYSHOWLAYOUT, (UINT)VK_UNDEFINED));
	VERIFY(Convert(m_SectionLegacy, KEYNEXTCTRL, false));
	VERIFY(Convert(m_SectionLegacy, KEYPREVCTRL, false));
	VERIFY(Convert(m_SectionLegacy, KEYSHOWLAYOUTCTRL, false));
	VERIFY(Convert(m_SectionLegacy, KEYNEXTALT, false));
	VERIFY(Convert(m_SectionLegacy, KEYPREVALT, false));
	VERIFY(Convert(m_SectionLegacy, KEYSHOWLAYOUTALT, false));
	VERIFY(Convert(m_SectionLegacy, KEYNEXTSHIFT, false));
	VERIFY(Convert(m_SectionLegacy, KEYPREVSHIFT, false));
	VERIFY(Convert(m_SectionLegacy, KEYSHOWLAYOUTSHIFT, false));
	VERIFY(Convert(m_SectionLegacy, SHAPENAMEINT, 0));
	VERIFY(Convert(m_SectionLegacy, SHAPENAMELEN, 6));
	VERIFY(Convert(m_SectionLegacy, LAYOUTNAMEINT, 0));
	VERIFY(Convert(m_SectionLegacy, LAYOUTNAMELEN, 7));
	VERIFY(Convert(m_SectionLegacy, USEMCI, false));
	VERIFY(Convert(m_SectionLegacy, SHIFTTYPE, 0));
	VERIFY(Convert(m_SectionLegacy, TIMESHIFT, 0));
	VERIFY(Convert(m_SectionLegacy, FRAMESHIFT, 0));
	VERIFY(Convert(m_SectionLegacy, LAUNCHPROPPROMPT, false));
	VERIFY(Convert(m_SectionLegacy, LAUNCHHTMLPLAYER, true));
	VERIFY(Convert(m_SectionLegacy, DELETEAVIAFTERUSE, true));
	VERIFY(Convert(m_SectionLegacy, RECORDINGMODE, 0));
	VERIFY(Convert(m_SectionLegacy, AUTONAMING, false));
	VERIFY(Convert(m_SectionLegacy, RESTRICTVIDEOCODECS, false));
	VERIFY(Convert(m_SectionLegacy, PRESETTIME, 0));
	VERIFY(Convert(m_SectionLegacy, RECORDPRESET, false));
	VERIFY(Convert(m_SectionLegacy, LANGUAGE, LANGID(STANDARD_LANGID)));

	// custom conversions
	// Red, Green, Blue entries => COLORREF
	{
		int iRed = 0;
		int iGreen = 0;
		int iBlue = 0;
		VERIFY(m_SectionLegacy.Read(HIGHLIGHTCOLORR, iRed));
		VERIFY(m_SectionLegacy.Read(HIGHLIGHTCOLORG, iGreen));
		VERIFY(m_SectionLegacy.Read(HIGHLIGHTCOLORB, iBlue));
		COLORREF clrColor = RGB(iRed, iGreen, iBlue);
		VERIFY(Write(HIGHLIGHTCOLOR, clrColor));

		VERIFY(m_SectionLegacy.Read(HIGHLIGHTCLICKCOLORLEFTR, iRed));
		VERIFY(m_SectionLegacy.Read(HIGHLIGHTCLICKCOLORLEFTG, iGreen));
		VERIFY(m_SectionLegacy.Read(HIGHLIGHTCLICKCOLORLEFTB, iBlue));
		clrColor = RGB(iRed, iGreen, iBlue);
		VERIFY(Write(HIGHLIGHTCLICKCOLORLEFT, clrColor));

		VERIFY(m_SectionLegacy.Read(HIGHLIGHTCLICKCOLORRIGHTR, iRed));
		VERIFY(m_SectionLegacy.Read(HIGHLIGHTCLICKCOLORRIGHTG, iGreen));
		VERIFY(m_SectionLegacy.Read(HIGHLIGHTCLICKCOLORRIGHTB, iBlue));
		clrColor = RGB(iRed, iGreen, iBlue);
		VERIFY(Write(HIGHLIGHTCLICKCOLORRIGHT, clrColor));
	}

	// combined entries
	// Caption, TimeStamp, XNote, Watermark
	
	CString strFaceName;

	if (0)
	{
		sTimestampOpts cTimestamp;
		VERIFY(m_SectionLegacy.Read(TIMESTAMPANNOTATION, cTimestamp.m_bAnnotation));
		VERIFY(m_SectionLegacy.Read(TIMESTAMPBACKCOLOR, cTimestamp.m_taTimestamp.backgroundColor));
		VERIFY(m_SectionLegacy.Read(TIMESTAMPSELECTED, cTimestamp.m_taTimestamp.isFontSelected));
		VERIFY(m_SectionLegacy.Read(TIMESTAMPPOSITION, cTimestamp.m_taTimestamp.position));
		VERIFY(m_SectionLegacy.Read(TIMESTAMPTEXTCOLOR, cTimestamp.m_taTimestamp.textColor));
		VERIFY(m_SectionLegacy.Read(TIMESTAMPTEXTWEIGHT, cTimestamp.m_taTimestamp.logfont.lfWeight));
		VERIFY(m_SectionLegacy.Read(TIMESTAMPTEXTHEIGHT, cTimestamp.m_taTimestamp.logfont.lfHeight));
		VERIFY(m_SectionLegacy.Read(TIMESTAMPTEXTWIDTH, cTimestamp.m_taTimestamp.logfont.lfWidth));
		VERIFY(m_SectionLegacy.Read(TIMESTAMPTEXTFONT, strFaceName));
		::strncpy_s(cTimestamp.m_taTimestamp.logfont.lfFaceName, LF_FACESIZE, strFaceName, LF_FACESIZE);
		//wcscpy_s(cTimestamp.m_taTimestamp.logfont.lfFaceName, LF_FACESIZE, strFaceName);
	}
	
	if (0)
	{
		sCaptionOpts cCaption;
		VERIFY(m_SectionLegacy.Read(CAPTIONANNOTATION, cCaption.m_bAnnotation));
		VERIFY(m_SectionLegacy.Read(CAPTIONBACKCOLOR, cCaption.m_taCaption.backgroundColor));
		VERIFY(m_SectionLegacy.Read(CAPTIONSELECTED, cCaption.m_taCaption.isFontSelected));
		VERIFY(m_SectionLegacy.Read(CAPTIONPOSITION, cCaption.m_taCaption.position));
		VERIFY(m_SectionLegacy.Read(CAPTIONTEXTCOLOR, cCaption.m_taCaption.textColor));
		VERIFY(m_SectionLegacy.Read(CAPTIONTEXTWEIGHT, cCaption.m_taCaption.logfont.lfWeight));
		VERIFY(m_SectionLegacy.Read(CAPTIONTEXTHEIGHT, cCaption.m_taCaption.logfont.lfHeight));
		VERIFY(m_SectionLegacy.Read(CAPTIONTEXTWIDTH, cCaption.m_taCaption.logfont.lfWidth));
		VERIFY(m_SectionLegacy.Read(CAPTIONTEXTFONT, strFaceName));
		::strncpy_s(cCaption.m_taCaption.logfont.lfFaceName, LF_FACESIZE, strFaceName, LF_FACESIZE);
		//wcscpy_s(cCaption.m_taCaption.logfont.lfFaceName, LF_FACESIZE, strFaceName);
	}
	
	if (0)
	{
		sWatermarkOpts cWatermark;
		VERIFY(Convert(m_SectionLegacy, WATERMARKANNOTATION, cWatermark.m_bAnnotation));
	}

	return false;
}

// read the entire file
bool CProfile::Read()
{
	//TRACE("## CProfile::Read\n");
	bool bResult = false;
	// Read and convert legacy section
	//VERIFY(m_SectionLegacy.Read(m_strFileName));
	//VERIFY(Convert());
	// now read updated values
	for (std::vector <CProfileSection>::iterator iter = m_vAllSections.begin(); iter != m_vAllSections.end(); ++iter)
	{
		bResult |= iter->Read(m_strFileName);
	}
	return bResult;
}
// write the entire file
bool CProfile::Write()
{
	//TRACE("## CProfile::Write\n");
	bool bResult = false;
	for (std::vector <CProfileSection>::iterator iter = m_vAllSections.begin(); iter != m_vAllSections.end(); ++iter)
	{
		bResult |= iter->Write(m_strFileName);
	}
	return bResult;
}
