/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Recorder.h"
#include "Profile.h"
#include "HotKey.h"
#include "CStudioLib.h"
#include "addons\TextAttributes.h"	// for position
#include "addons\ImageAttributes.h"
#include "addons\Camstudio4XNote.h"


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

	// TODO, Possible memory leak, where is the delete operation of the new below done?
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

void ReadIA(Setting& s, ImageAttributes& iaResult)
{
	s.lookupValue("Brightness", (int&)iaResult.m_lBrightness);
	s.lookupValue("Contrast", (int&)iaResult.m_lContrast);
	s.lookupValue("Position", (int&)iaResult.position);
	s.lookupValue("PosType", (int&)iaResult.posType);
	s.lookupValue("xPosRatio", iaResult.xPosRatio);
	s.lookupValue("yPosRatio", iaResult.yPosRatio);
	std::string text;
	if (s.lookupValue("text", text))
		iaResult.text = text.c_str();
}

void WriteIA(Setting& s, ImageAttributes& iaResult)
{
	UpdateSetting(s,"Brightness", (int&)iaResult.m_lBrightness, Setting::TypeInt);
	UpdateSetting(s,"Contrast", (int&)iaResult.m_lContrast, Setting::TypeInt);
	UpdateSetting(s,"Position", (int&)iaResult.position, Setting::TypeInt);
	UpdateSetting(s,"PosType", (int&)iaResult.posType, Setting::TypeInt);
	UpdateSetting(s,"xPosRatio", iaResult.xPosRatio, Setting::TypeInt);
	UpdateSetting(s,"yPosRatio", iaResult.yPosRatio, Setting::TypeInt);
	std::string text(iaResult.text);
	UpdateSetting(s,"text", text, Setting::TypeString);
}

void ReadTA(Setting& s, TextAttributes& taResult)
{
	s.lookupValue("position", (int&)taResult.position);
	s.lookupValue("posType", (int&)taResult.posType);
	s.lookupValue("xPosRatio", taResult.xPosRatio);
	s.lookupValue("yPosRatio", taResult.yPosRatio);
	std::string text;
	s.lookupValue("text", text);
	taResult.text = text.c_str();
	s.lookupValue("backgroundColor", (int&)taResult.backgroundColor);
	s.lookupValue("textColor", (int&)taResult.textColor);
	s.lookupValue("isFontSelected", taResult.isFontSelected);
	if (s.exists("Font"))
		ReadFont(s["Font"], taResult.logfont);
}

void WriteTA(Setting& s, TextAttributes& taResult)
{
	UpdateSetting(s,"position", (int&)taResult.position, Setting::TypeInt);
	UpdateSetting(s,"posType", (int&)taResult.posType, Setting::TypeInt);
	UpdateSetting(s,"xPosRatio", taResult.xPosRatio, Setting::TypeInt);
	UpdateSetting(s,"yPosRatio", taResult.yPosRatio, Setting::TypeInt);
	std::string text(taResult.text);
	UpdateSetting(s,"text", text, Setting::TypeString);
	UpdateSetting(s,"backgroundColor", (int&)taResult.backgroundColor, Setting::TypeInt);
	UpdateSetting(s,"textColor", (int&)taResult.textColor, Setting::TypeInt);
	UpdateSetting(s,"isFontSelected", taResult.isFontSelected, Setting::TypeInt);
	Setting* sf;
	if (s.exists("Font"))
		sf = &(s["Font"]);
	else
		sf = &(s.add("Font", Setting::TypeGroup));
	WriteFont(*sf, taResult.logfont);
}

void ReadFont(Setting& s, LOGFONT& f)
{
	unsigned tmp;
	if (s.lookupValue("CharSet", tmp))
		f.lfCharSet = tmp;
	if (s.lookupValue("ClipPrecision", tmp))
		f.lfClipPrecision = tmp;
	s.lookupValue("Escapement", (int&)f.lfEscapement);
	std::string text;
	if (s.lookupValue("FaceName", text))
		strncpy(f.lfFaceName, text.c_str(), 32);
	s.lookupValue("Height", (int&)f.lfHeight);
	if (s.lookupValue("Italic", tmp))
		f.lfItalic = tmp;
	s.lookupValue("Orientation", (int&)f.lfOrientation);
	if (s.lookupValue("OutPrecision", tmp))
		f.lfOutPrecision = tmp;
	if (s.lookupValue("PitchAndFamily", tmp))
		f.lfPitchAndFamily = tmp;
	if (s.lookupValue("Quality", tmp))
		f.lfQuality = tmp;
	if (s.lookupValue("StrikeOut", tmp))
		f.lfStrikeOut = tmp;
	if (s.lookupValue("Underline", tmp))
		f.lfUnderline = tmp;
	s.lookupValue("Weight", (int&)f.lfWeight);
	s.lookupValue("Width", (int&)f.lfWidth);
}

void WriteFont(Setting& s, LOGFONT& f)
{
	long tmp;
	tmp = f.lfCharSet;
	UpdateSetting(s,"CharSet", tmp, Setting::TypeInt);
	tmp = f.lfClipPrecision;
	UpdateSetting(s,"ClipPrecision", tmp, Setting::TypeInt);
	UpdateSetting(s,"Escapement", (long&)f.lfEscapement, Setting::TypeInt);
	std::string text;
	text.assign(f.lfFaceName, 32);
	UpdateSetting(s,"FaceName", text, Setting::TypeString);
	UpdateSetting(s,"Height", (long&)f.lfHeight, Setting::TypeInt);
	tmp = f.lfItalic;
	UpdateSetting(s,"Italic", tmp, Setting::TypeInt);
	UpdateSetting(s,"Orientation", f.lfOrientation, Setting::TypeInt);
	tmp = f.lfOutPrecision;
	UpdateSetting(s,"OutPrecision", tmp, Setting::TypeInt);
	tmp = f.lfPitchAndFamily;
	UpdateSetting(s,"PitchAndFamily", tmp, Setting::TypeInt);
	tmp = f.lfQuality;
	UpdateSetting(s,"Quality", tmp, Setting::TypeInt);
	tmp = f.lfStrikeOut;
	UpdateSetting(s,"StrikeOut", tmp, Setting::TypeInt);
	tmp = f.lfUnderline;
	UpdateSetting(s,"Underline", tmp, Setting::TypeInt);
	UpdateSetting(s,"Weight", f.lfWeight, Setting::TypeInt);
	UpdateSetting(s,"Width", f.lfWidth, Setting::TypeInt);
}
