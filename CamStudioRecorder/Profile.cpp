#include "StdAfx.h"
#include "Recorder.h"
#include "Profile.h"
#include "HotKey.h"
#include <CamLib/CStudioLib.h>
#include "addons\TextAttributes.h" // for position
#include "addons\ImageAttributes.h"

bool bRecordPreset = false;

int iSaveLen = 0;   // dir len?
int iCursorLen = 0; // dir len?
int iShapeNameInt = 1;
int iLayoutNameInt = 1;
int iFrameShift = 0;
int iPresetTime = 60;

sAudioFormat &sAudioFormat::operator=(const sAudioFormat &rhs)
{
    if (this == &rhs)
        return *this;

    m_uDeviceID = rhs.m_uDeviceID;
    m_bCompression = rhs.m_bCompression;
    m_bInterleaveFrames = rhs.m_bInterleaveFrames;
    m_bUseMCI = rhs.m_bUseMCI;
    m_bPerformAutoSearch = rhs.m_bPerformAutoSearch;
    m_iRecordAudio = rhs.m_iRecordAudio;
    m_iNumChannels = rhs.m_iNumChannels;
    m_iBitsPerSample = rhs.m_iBitsPerSample;
    m_iSamplesPerSeconds = rhs.m_iSamplesPerSeconds;
    m_iInterleaveFactor = rhs.m_iInterleaveFactor;
    m_iInterleavePeriod = rhs.m_iInterleavePeriod;
    m_iMixerDevices = rhs.m_iMixerDevices;
    m_iSelectedMixer = rhs.m_iSelectedMixer;
    m_iFeedbackLine = rhs.m_iFeedbackLine;
    m_iFeedbackLineInfo = rhs.m_iFeedbackLineInfo;
    m_dwWaveinSelected = rhs.m_dwWaveinSelected;
    m_wFormatTag = rhs.m_wFormatTag;

    DeleteAudio();
    if (rhs.m_pwfx && rhs.m_dwCbwFX)
    {
        if (NewAudio())
        {
            ASSERT(m_dwCbwFX == rhs.m_dwCbwFX);
            ASSERT(m_dwCbwFX == (sizeof(WAVEFORMATEX) + m_pwfx->cbSize));
            ::CopyMemory(m_pwfx, rhs.m_pwfx, m_dwCbwFX);
            if (m_pwfx->wFormatTag != m_wFormatTag)
            {
                m_pwfx->wFormatTag = static_cast<WORD>(m_wFormatTag);
            }
        }
    }
    else
    {
        TRACE(_T("error! pointer and size mismatch!\n"));
    }
    return *this;
}

// special memory allocator for sAudioFormat
bool sAudioFormat::DeleteAudio()
{
    if (m_pwfx)
    {
        delete[] m_pwfx;
        m_pwfx = 0;
    }
    m_dwCbwFX = 0;
    return true;
}

bool sAudioFormat::NewAudio()
{
    bool bResult = DeleteAudio();
    if (!bResult)
    {
        return bResult;
    }
    MMRESULT mmresult = ::acmMetrics(nullptr, ACM_METRIC_MAX_SIZE_FORMAT, &m_dwCbwFX);
    bResult = (MMSYSERR_NOERROR == mmresult);
    if (!bResult)
    {
        return bResult;
    }

    // TODO, Possible memory leak, where is the delete operation of the new below done?
    m_pwfx = (LPWAVEFORMATEX) new char[m_dwCbwFX];
    bResult = (0 != m_pwfx);
    if (!bResult)
    {
        m_dwCbwFX = 0;
    }
    else
    {
        ::ZeroMemory(m_pwfx, m_dwCbwFX);

        // Prevent C4244 warning and take some predictions for unwanted truncations
        // m_pwfx->cbSize = m_dwCbwFX - sizeof(WAVEFORMATEX);  -> C4244
        DWORD tmpDWord = m_dwCbwFX - sizeof(WAVEFORMATEX);
        if (tmpDWord > static_cast<WORD>(tmpDWord))
        {
            TRACE("Error: Casted value [%i] is not the same as uncasted [%l] ..!\n", static_cast<WORD>(tmpDWord),
                  tmpDWord);
        }
        m_pwfx->cbSize = static_cast<WORD>(tmpDWord);
    }
    return bResult;
}

bool sAudioFormat::CopyAudio(LPWAVEFORMATEX pwfx, DWORD dwCbwFX)
{
    bool bResult = (0 != pwfx);
    if (!bResult)
    {
        return bResult;
    }
    bResult = (m_dwCbwFX == dwCbwFX);
    if (!bResult)
    {
        return bResult;
    }
    ::CopyMemory(pwfx, m_pwfx, m_dwCbwFX);

    return bResult;
}

bool sAudioFormat::WriteAudio(const LPWAVEFORMATEX pwfx)
{
    bool bResult = (0 != pwfx);
    if (!bResult)
    {
        return bResult;
    }
    bResult = NewAudio();
    if (!bResult)
    {
        return bResult;
    }
    ::CopyMemory(m_pwfx, pwfx, m_dwCbwFX);
    m_wFormatTag = m_pwfx->wFormatTag;

    return bResult;
}

void sAudioFormat::BuildRecordingFormat()
{
    WAVEFORMATEX &rFormat = AudioFormat();

    rFormat.wFormatTag = WAVE_FORMAT_PCM;
    rFormat.wBitsPerSample = static_cast<WORD>(m_iBitsPerSample);
    rFormat.nSamplesPerSec = m_iSamplesPerSeconds;
    rFormat.nChannels = static_cast<WORD>(m_iNumChannels);

    rFormat.nBlockAlign = rFormat.nChannels * (rFormat.wBitsPerSample / 8);
    rFormat.nAvgBytesPerSec = rFormat.nSamplesPerSec * rFormat.nBlockAlign;
    rFormat.cbSize = 0;
}

/////////////////////////////////////////////////////////////////////////////

void ReadIA(libconfig::Setting &s, ImageAttributes &iaResult)
{
    s.lookupValue("Brightness", (int &)iaResult.m_lBrightness);
    s.lookupValue("Contrast", (int &)iaResult.m_lContrast);
    s.lookupValue("Position", (int &)iaResult.position);
    s.lookupValue("PosType", (int &)iaResult.posType);
    s.lookupValue("xPosRatio", iaResult.xPosRatio);
    s.lookupValue("yPosRatio", iaResult.yPosRatio);
    std::string text;
    if (s.lookupValue("text", text))
        iaResult.text = text.c_str();
}

void WriteIA(libconfig::Setting &s, ImageAttributes &iaResult)
{
    UpdateSetting(s, "Brightness", (int &)iaResult.m_lBrightness, libconfig::Setting::TypeInt);
    UpdateSetting(s, "Contrast", (int &)iaResult.m_lContrast, libconfig::Setting::TypeInt);
    UpdateSetting(s, "Position", (int &)iaResult.position, libconfig::Setting::TypeInt);
    UpdateSetting(s, "PosType", (int &)iaResult.posType, libconfig::Setting::TypeInt);
    UpdateSetting(s, "xPosRatio", iaResult.xPosRatio, libconfig::Setting::TypeInt);
    UpdateSetting(s, "yPosRatio", iaResult.yPosRatio, libconfig::Setting::TypeInt);
    std::string text(iaResult.text);
    UpdateSetting(s, "text", text, libconfig::Setting::TypeString);
}

void ReadTA(libconfig::Setting &s, TextAttributes &taResult)
{
    s.lookupValue("position", (int &)taResult.position);
    s.lookupValue("posType", (int &)taResult.posType);
    s.lookupValue("xPosRatio", taResult.xPosRatio);
    s.lookupValue("yPosRatio", taResult.yPosRatio);
    std::string text;
    s.lookupValue("text", text);
    taResult.text = text.c_str();
    s.lookupValue("backgroundColor", (int &)taResult.backgroundColor);
    s.lookupValue("textColor", (int &)taResult.textColor);
    s.lookupValue("isFontSelected", taResult.isFontSelected);
    if (s.exists("Font"))
        ReadFont(s["Font"], taResult.logfont);
}

void WriteTA(libconfig::Setting &s, TextAttributes &taResult)
{
    UpdateSetting(s, "position", (int &)taResult.position, libconfig::Setting::TypeInt);
    UpdateSetting(s, "posType", (int &)taResult.posType, libconfig::Setting::TypeInt);
    UpdateSetting(s, "xPosRatio", taResult.xPosRatio, libconfig::Setting::TypeInt);
    UpdateSetting(s, "yPosRatio", taResult.yPosRatio, libconfig::Setting::TypeInt);
    std::string text(taResult.text);
    UpdateSetting(s, "text", text, libconfig::Setting::TypeString);
    UpdateSetting(s, "backgroundColor", (int &)taResult.backgroundColor, libconfig::Setting::TypeInt);
    UpdateSetting(s, "textColor", (int &)taResult.textColor, libconfig::Setting::TypeInt);
    UpdateSetting(s, "isFontSelected", taResult.isFontSelected, libconfig::Setting::TypeInt);
    libconfig::Setting *sf;
    if (s.exists("Font"))
        sf = &(s["Font"]);
    else
        sf = &(s.add("Font", libconfig::Setting::TypeGroup));
    WriteFont(*sf, taResult.logfont);
}

void ReadFont(libconfig::Setting &s, LOGFONT &f)
{
    unsigned tmp;

    if (s.lookupValue("CharSet", tmp))
        f.lfCharSet = tmp;
    if (s.lookupValue("ClipPrecision", tmp))
        f.lfClipPrecision = tmp;
    s.lookupValue("Escapement", (int &)f.lfEscapement);
    std::string text;
    if (s.lookupValue("FaceName", text))
        strncpy_s(f.lfFaceName, text.c_str(), 32);
    s.lookupValue("Height", (int &)f.lfHeight);
    if (s.lookupValue("Italic", tmp))
        f.lfItalic = tmp;
    s.lookupValue("Orientation", (int &)f.lfOrientation);
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
    s.lookupValue("Weight", (int &)f.lfWeight);
    s.lookupValue("Width", (int &)f.lfWidth);
}

void WriteFont(libconfig::Setting &s, LOGFONT &f)
{
    long tmp;
    tmp = f.lfCharSet;
    UpdateSetting(s, "CharSet", tmp, libconfig::Setting::TypeInt);
    tmp = f.lfClipPrecision;
    UpdateSetting(s, "ClipPrecision", tmp, libconfig::Setting::TypeInt);
    UpdateSetting(s, "Escapement", (long &)f.lfEscapement, libconfig::Setting::TypeInt);
    std::string text;
    text.assign(f.lfFaceName, 32);
    UpdateSetting(s, "FaceName", text, libconfig::Setting::TypeString);
    UpdateSetting(s, "Height", (long &)f.lfHeight, libconfig::Setting::TypeInt);
    tmp = f.lfItalic;
    UpdateSetting(s, "Italic", tmp, libconfig::Setting::TypeInt);
    UpdateSetting(s, "Orientation", f.lfOrientation, libconfig::Setting::TypeInt);
    tmp = f.lfOutPrecision;
    UpdateSetting(s, "OutPrecision", tmp, libconfig::Setting::TypeInt);
    tmp = f.lfPitchAndFamily;
    UpdateSetting(s, "PitchAndFamily", tmp, libconfig::Setting::TypeInt);
    tmp = f.lfQuality;
    UpdateSetting(s, "Quality", tmp, libconfig::Setting::TypeInt);
    tmp = f.lfStrikeOut;
    UpdateSetting(s, "StrikeOut", tmp, libconfig::Setting::TypeInt);
    tmp = f.lfUnderline;
    UpdateSetting(s, "Underline", tmp, libconfig::Setting::TypeInt);
    UpdateSetting(s, "Weight", f.lfWeight, libconfig::Setting::TypeInt);
    UpdateSetting(s, "Width", f.lfWidth, libconfig::Setting::TypeInt);
}

sVideoOpts &sVideoOpts::operator=(const sVideoOpts &rhs)
{
    if (this == &rhs)
    {
        return *this;
    }

    m_bRestrictVideoCodecs = rhs.m_bRestrictVideoCodecs;
    m_bAutoAdjust = rhs.m_bAutoAdjust;
    m_bLock = rhs.m_bLock;
    m_bRoundDown = rhs.m_bRoundDown;
    m_iValueAdjust = rhs.m_iValueAdjust;
    m_iTimeLapse = rhs.m_iTimeLapse;
    m_iFramesPerSecond = rhs.m_iFramesPerSecond;
    m_iKeyFramesEvery = rhs.m_iKeyFramesEvery;
    m_iCompQuality = rhs.m_iCompQuality;
    m_iSelectedCompressor = rhs.m_iSelectedCompressor;
    m_iShiftType = rhs.m_iShiftType;
    m_iTimeShift = rhs.m_iTimeShift;
    m_dwCompfccHandler = rhs.m_dwCompfccHandler;
    m_dwCompressorStateIsFor = rhs.m_dwCompressorStateIsFor;

    State(rhs.m_pState, rhs.m_dwCompressorStateSize);
    m_cStartRecordingString = rhs.m_cStartRecordingString;

    return *this;
}

sVideoOpts::sVideoOpts(const sVideoOpts &rhs)
    : m_iShiftType(NOSYNCH)
    , m_pState(nullptr)
    , m_cStartRecordingString("")
{
    *this = rhs;
}

sVideoOpts::~sVideoOpts()
{
    delete[] m_pState;
}

DWORD sVideoOpts::StateSize() const
{
    return m_dwCompressorStateSize;
}

LPVOID sVideoOpts::State(LPCVOID pState, DWORD dwStateSize)
{
    State(pState ? dwStateSize : 0L);
    if (!pState || (dwStateSize < 1L))
    {
        return m_pState;
    }

    ::_memccpy(m_pState, pState, 1, m_dwCompressorStateSize);

    return m_pState;
}

LPVOID sVideoOpts::State(DWORD dwStateSize)
{
    if (m_pState)
    {
        delete[] m_pState;
        m_pState = nullptr;
        m_dwCompressorStateSize = 0L;
    }

    ASSERT(0L == m_dwCompressorStateSize);
    if (dwStateSize < 1L)
    {
        return m_pState;
    }
    m_dwCompressorStateSize = dwStateSize;
    // TODO: , Possible memory leak, where is the delete operation of the new below done?
    m_pState = new char[m_dwCompressorStateSize];
    return m_pState;
}

LPVOID sVideoOpts::State() const
{
    return m_pState;
}

bool sVideoOpts::Read(libconfig::Setting &cProfile)
{
    cProfile.lookupValue("restrictVideoCodecs", m_bRestrictVideoCodecs);
    cProfile.lookupValue("AutoAdjust", m_bAutoAdjust);
    cProfile.lookupValue("LockCaptureAndPlayback", m_bLock);
    cProfile.lookupValue("RoundDown", m_bRoundDown);
    cProfile.lookupValue("ValueAdjust", m_iValueAdjust);
    cProfile.lookupValue("TimeLapse", m_iTimeLapse);
    cProfile.lookupValue("fps", m_iFramesPerSecond);
    cProfile.lookupValue("KeyFramesEvery", m_iKeyFramesEvery);
    cProfile.lookupValue("CompQuality", m_iCompQuality);
    cProfile.lookupValue("shiftType", m_iShiftType);
    cProfile.lookupValue("timeshift", m_iTimeShift);
    cProfile.lookupValue("CompFCCHandler", (unsigned &)m_dwCompfccHandler);
    cProfile.lookupValue("CompressorStateIsFor", (unsigned &)m_dwCompressorStateIsFor);
    DWORD dwSize = 0UL;
    cProfile.lookupValue("CompressorStateSize", (unsigned &)dwSize);
    State(dwSize);
    // CString  m_cStartRecordingString = "";
    m_cStartRecordingString = "";
    return true;
}

bool sVideoOpts::Write(libconfig::Setting &cProfile)
{
    UpdateSetting(cProfile, "restrictVideoCodecs", m_bRestrictVideoCodecs, libconfig::Setting::TypeBoolean);
    UpdateSetting(cProfile, "AutoAdjust", m_bAutoAdjust, libconfig::Setting::TypeBoolean);
    UpdateSetting(cProfile, "LockCaptureAndPlayback", m_bLock, libconfig::Setting::TypeBoolean);
    UpdateSetting(cProfile, "RoundDown", m_bRoundDown, libconfig::Setting::TypeBoolean);
    UpdateSetting(cProfile, "ValueAdjust", m_iValueAdjust, libconfig::Setting::TypeInt);
    UpdateSetting(cProfile, "TimeLapse", m_iTimeLapse, libconfig::Setting::TypeInt);
    UpdateSetting(cProfile, "fps", m_iFramesPerSecond, libconfig::Setting::TypeInt);
    UpdateSetting(cProfile, "KeyFramesEvery", m_iKeyFramesEvery, libconfig::Setting::TypeInt);
    UpdateSetting(cProfile, "CompQuality", m_iCompQuality, libconfig::Setting::TypeInt);
    UpdateSetting(cProfile, "shiftType", m_iShiftType, libconfig::Setting::TypeInt);
    UpdateSetting(cProfile, "timeshift", m_iTimeShift, libconfig::Setting::TypeInt);
    UpdateSetting(cProfile, "CompFCCHandler", (long &)m_dwCompfccHandler, libconfig::Setting::TypeInt);
    UpdateSetting(cProfile, "CompressorStateIsFor", (long &)m_dwCompressorStateIsFor, libconfig::Setting::TypeInt);
    UpdateSetting(cProfile, "CompressorStateSize", (long &)m_dwCompressorStateSize, libconfig::Setting::TypeInt);
    return true;
}
