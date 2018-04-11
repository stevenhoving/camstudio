// Profile.h
// include file for windows profile (*ini file) classes
/*****
NEWS NEWS NEWS 10/17/2010
Config* cfg is defined in StdAfx.h . While it is disputable about it's location,
meanwhile all new necessary settings can be accessed directly
cfg->getRoot()["MySection"]["MySubsection"]["MyValue"]
see http://www.hyperrealm.com/libconfig/libconfig_manual.html
CProfile is obsolete
****/

//
// NOTA BENE: To a degree, modifications to a profile are messy. The profile
// classes help standardize the process of adding new values amd simplify
// accessing values. The classes support a number of standard types (int,
// bool, long, double, CString), composite types (LOGFONT, TextAttributes,
// ImageAttributes), and alias types (LANGID, COLORREF).
//
// The CProfile class defines the interface to a *.ini file. Entries in the
// files are composed of string-value pairs (ValuePair) which are associated
// with a key ID (IDValuePair). The IDValuePair items are collected by ID
// in a CGroupType. The CGroupType are then collected into a CSectionGroup
// for each type the section supports. CProfileSection then collectes all the
// CSectionGroup for that section and finally, the CProfileSections are
// collected in the CProfile class.
//
// Adding a profile entry for an existing type
// 1.    Define a new eProfileID value for the entry and add it to the end of
//        enumerated values for the eProfileID, leaving MAX_PROFILE_ID as the
//        last value. The value must be unique or the compiler will complain.
// 2.    Initialize the new entry in CProfile::InitSections by calling Add
//        with the eProfileID, the entry string, and the default value. You
//        can add the entry to a specific section or default to the application
//        section. The default value type will define the group within the
//        section the entry will go in.
//
// Adding a new type
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include <CamLib\CStudioLib.h>
#include "addons\TextAttributes.h"
#include "addons\ImageAttributes.h"

#include <vector>

void ReadFont(libconfig::Setting &s, LOGFONT &f);
void ReadIA(libconfig::Setting &s, ImageAttributes &iaResult);
void ReadTA(libconfig::Setting &s, TextAttributes &taResult);

void WriteFont(libconfig::Setting &s, LOGFONT &f);
void WriteIA(libconfig::Setting &s, ImageAttributes &iaResult);
void WriteTA(libconfig::Setting &s, TextAttributes &taResult);

template <class T>
void UpdateSetting(libconfig::Setting &s, const char *name, T &value, libconfig::Setting::Type t)
{
    if (s.exists(name))
    {
        s[name] = value;
    }
    else
    {
        s.add(name, t) = value;
    }
}

#define LEGACY_PROFILE_DISABLE

// video options
enum eSynchType
{
    NOSYNCH,
    VIDEOFIRST,
    AUDIOFIRST
};

// POD to hold them
struct sVideoOpts
{
    sVideoOpts() = default;

    sVideoOpts(const sVideoOpts &rhs);

    virtual ~sVideoOpts();

    sVideoOpts &operator=(const sVideoOpts &rhs);

    DWORD StateSize() const;

    // n.b. Keep LPCVOID definition; good for memory corruption check
    // LPCVOID State() const    {return m_pState;}
    LPVOID State() const;

    LPVOID State(DWORD dwStateSize);

    LPVOID State(LPCVOID pState, DWORD dwStateSize);

    // CAVEAT!!!  CAVEAT!!!  CAVEAT!!!  CAVEAT!!!
    // Caller of CopyState() must delete the allocated memory!
    // LPVOID CopyState()
    //{
    //    if (!m_pState)
    //        return m_pState;
    //    ASSERT(0L < m_dwCompressorStateSize);
    //    LPVOID lpOldState = m_pState;
    //    DWORD dwOldSize = m_dwCompressorStateSize;
    //    m_pState = 0;
    //    m_dwCompressorStateSize = 0L;
    //    VERIFY(0 != State(lpOldState, dwOldSize));
    //    return lpOldState;
    //}

    bool Read(libconfig::Setting &cProfile);

    bool Write(libconfig::Setting &cProfile);

    bool m_bRestrictVideoCodecs{false};
    bool m_bAutoAdjust{true};
    bool m_bLock{true};
    bool m_bRoundDown{false};
    int m_iValueAdjust{1};
    int m_iTimeLapse{50};
    int m_iFramesPerSecond{20};
    int m_iKeyFramesEvery{100};
    int m_iCompQuality{7000};
    int m_iSelectedCompressor{0};
    int m_iShiftType{NOSYNCH}; // NOSYNCH, VIDEOFIRST, AUDIOFIRST
    int m_iTimeShift{100};
    FOURCC m_dwCompfccHandler{0UL};
    FOURCC m_dwCompressorStateIsFor{0UL};
    CString m_cStartRecordingString{""};

protected:
    LPVOID m_pState{nullptr};
    DWORD m_dwCompressorStateSize{0UL};
};
extern sVideoOpts cVideoOpts;

enum eCaptureType
{
    CAPTURE_FIXED,
    CAPTURE_VARIABLE,
    CAPTURE_FULLSCREEN,
    CAPTURE_WINDOW,
    CAPTURE_ALLSCREENS
};

// miscellaneous options
struct sRegionOpts
{
    sRegionOpts() = default;

    sRegionOpts(const sRegionOpts &rhs)
    {
        *this = rhs;
    }

    sRegionOpts &operator=(const sRegionOpts &rhs)
    {
        if (this == &rhs)
        {
            return *this;
        }

        m_bFixed = rhs.m_bFixed;
        m_bMouseDrag = rhs.m_bMouseDrag;
        m_iCaptureMode = rhs.m_iCaptureMode;
        m_iLeft = rhs.m_iLeft;
        m_iTop = rhs.m_iTop;
        m_iWidth = rhs.m_iWidth;
        m_iHeight = rhs.m_iHeight;
        return *this;
    }

    bool Read(libconfig::Setting &cProfile)
    {
        cProfile.lookupValue("FixedCapture", m_bFixed);
        cProfile.lookupValue("SupportMouseDrag", m_bMouseDrag);
        cProfile.lookupValue("MouseCaptureMode", m_iCaptureMode);
        cProfile.lookupValue("Left", m_iLeft);
        cProfile.lookupValue("Top", m_iTop);
        cProfile.lookupValue("Width", m_iWidth);
        cProfile.lookupValue("Height", m_iHeight);

        return true;
    }

    bool Write(libconfig::Setting &cProfile)
    {
        UpdateSetting(cProfile, "FixedCapture", m_bFixed, libconfig::Setting::TypeBoolean);
        UpdateSetting(cProfile, "SupportMouseDrag", m_bMouseDrag, libconfig::Setting::TypeBoolean);
        UpdateSetting(cProfile, "MouseCaptureMode", m_iCaptureMode, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "Left", m_iLeft, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "Top", m_iTop, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "Width", m_iWidth, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "Height", m_iHeight, libconfig::Setting::TypeInt);
        return true;
    }

    bool isCaptureMode(const eCaptureType eType) const
    {
        return eType == m_iCaptureMode;
    }

    bool m_bFixed{false};
    bool m_bMouseDrag{false};
    int m_iCaptureMode{CAPTURE_FIXED};
    // TODO: should be a CRect
    int m_iLeft{100};
    int m_iTop{100};
    int m_iWidth{320};
    int m_iHeight{240};
};
extern sRegionOpts cRegionOpts;

struct sCaptionOpts
{
    sCaptionOpts() = default;

    sCaptionOpts(const sCaptionOpts &rhs)
    {
        *this = rhs;
    }

    sCaptionOpts &operator=(const sCaptionOpts &rhs)
    {
        if (this == &rhs)
        {
            return *this;
        }

        m_bAnnotation = rhs.m_bAnnotation;
        m_taCaption = rhs.m_taCaption;
        return *this;
    }

    bool Read(libconfig::Setting &cProfile)
    {
        // from Caption group
        cProfile.lookupValue("Annotation", m_bAnnotation);
        if (cProfile.exists("TextAttributes"))
        {
            ReadTA(cProfile["TextAttributes"], m_taCaption);
        }
        return true;
    }

    bool Write(libconfig::Setting &cProfile)
    {
        UpdateSetting(cProfile, "Annotation", m_bAnnotation, libconfig::Setting::TypeBoolean);
        libconfig::Setting *s;
        if (cProfile.exists("TextAttributes"))
        {
            s = &(cProfile["TextAttributes"]);
        }
        else
        {
            s = &(cProfile.add("TextAttributes", libconfig::Setting::TypeGroup));
        }
        WriteTA(*s, m_taCaption);
        return true;
    }

    bool m_bAnnotation{false};
    TextAttributes m_taCaption{TOP_LEFT};
};
extern sCaptionOpts cCaptionOpts;

struct sTimestampOpts
{
    sTimestampOpts() = default;

    sTimestampOpts(const sTimestampOpts &rhs)
    {
        *this = rhs;
    }

    sTimestampOpts &operator=(const sTimestampOpts &rhs)
    {
        if (this == &rhs)
        {
            return *this;
        }

        m_bAnnotation = rhs.m_bAnnotation;
        m_taTimestamp = rhs.m_taTimestamp;
        return *this;
    }

    bool Read(libconfig::Setting &cProfile)
    {
        // TimeStamp
        cProfile.lookupValue("Annotation", m_bAnnotation);
        if (cProfile.exists("TextAttributes"))
        {
            ReadTA(cProfile["TextAttributes"], m_taTimestamp);
        }
        return true;
    }

    bool Write(libconfig::Setting &cProfile)
    {
        UpdateSetting(cProfile, "Annotation", m_bAnnotation, libconfig::Setting::TypeBoolean);
        libconfig::Setting *s;
        if (cProfile.exists("TextAttributes"))
        {
            s = &(cProfile["TextAttributes"]);
        }
        else
        {
            s = &(cProfile.add("TextAttributes", libconfig::Setting::TypeGroup));
        }
        WriteTA(*s, m_taTimestamp);
        return true;
    }

    bool m_bAnnotation{false};
    TextAttributes m_taTimestamp{TOP_LEFT};
};
extern sTimestampOpts cTimestampOpts;


struct sWatermarkOpts
{
    sWatermarkOpts()
        : m_iaWatermark(TOP_LEFT)
    {
    }
    sWatermarkOpts(const sWatermarkOpts &rhs)
    {
        *this = rhs;
    }
    sWatermarkOpts &operator=(const sWatermarkOpts &rhs)
    {
        if (this == &rhs)
        {
            return *this;
        }

        m_bAnnotation = rhs.m_bAnnotation;
        m_iaWatermark = rhs.m_iaWatermark;
        return *this;
    }
    bool Read(libconfig::Setting &cProfile)
    {
        cProfile.lookupValue("Annotation", m_bAnnotation);
        if (cProfile.exists("ImageAttributes"))
        {
            ReadIA(cProfile["ImageAttributes"], m_iaWatermark);
        }
        return true;
    }
    bool Write(libconfig::Setting &cProfile)
    {
        UpdateSetting(cProfile, "Annotation", m_bAnnotation, libconfig::Setting::TypeBoolean);
        libconfig::Setting *s;
        if (cProfile.exists("ImageAttributes"))
        {
            s = &(cProfile["ImageAttributes"]);
        }
        else
        {
            s = &(cProfile.add("ImageAttributes", libconfig::Setting::TypeGroup));
        }
        WriteIA(*s, m_iaWatermark);
        return true;
    }

    bool m_bAnnotation{false};
    ImageAttributes m_iaWatermark;
};
extern sWatermarkOpts cWatermarkOpts;

// Audio format values
// POD to hold them
const int MILLISECONDS = 0;
const int FRAMES = 1;
enum eAudioInput
{
    NONE,
    MICROPHONE,
    SPEAKERS
};

struct sAudioFormat
{
public:
    sAudioFormat() = default;

    // \todo figure out if this constructor works the way it looks
    sAudioFormat(const sAudioFormat &rhs)
        : m_iRecordAudio(NONE)
        , m_iInterleavePeriod(MILLISECONDS)
        , m_uDeviceID(WAVE_MAPPER)
        , m_dwCbwFX(0)
        , m_dwWaveinSelected(WAVE_FORMAT_4M16)
        , m_pwfx(nullptr)

    {
        *this = rhs;
    }

    virtual ~sAudioFormat()
    {
        DeleteAudio();
    }

    sAudioFormat &operator=(const sAudioFormat &rhs);

    bool Read(libconfig::Setting &cProfile)
    {
        cProfile.lookupValue("AudioDeviceID", m_uDeviceID);
        cProfile.lookupValue("AudioCompression", m_bCompression);
        cProfile.lookupValue("useMCI", m_bUseMCI);
        cProfile.lookupValue("performAutoSearch", m_bPerformAutoSearch);
        cProfile.lookupValue("RecordAudio", m_iRecordAudio);
        cProfile.lookupValue("cbwfx", (unsigned &)m_dwCbwFX);
        cProfile.lookupValue("waveinselected", (unsigned &)m_dwWaveinSelected);
        cProfile.lookupValue("audio_bits_per_sample", m_iBitsPerSample);
        cProfile.lookupValue("audio_num_channels", m_iNumChannels);
        cProfile.lookupValue("audio_samples_per_seconds", m_iSamplesPerSeconds);
        cProfile.lookupValue("NumDev", m_iMixerDevices);
        cProfile.lookupValue("SelectedDev", m_iSelectedMixer);
        cProfile.lookupValue("CompressionFormatTag", m_wFormatTag);
        AudioFormat().wFormatTag = static_cast<WORD>(m_wFormatTag);
        cProfile.lookupValue("feedback_line", m_iFeedbackLine);
        cProfile.lookupValue("feedback_line_info", m_iFeedbackLineInfo);
        cProfile.lookupValue("InterleaveFrames", m_bInterleaveFrames);
        cProfile.lookupValue("InterleaveFactor", m_iInterleaveFactor);
        cProfile.lookupValue("InterleaveUnit", m_iInterleavePeriod);
        return true;
    }

    bool Write(libconfig::Setting &cProfile)
    {
        UpdateSetting(cProfile, "AudioDeviceID", (long &)m_uDeviceID, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "AudioCompression", m_bCompression, libconfig::Setting::TypeBoolean);
        UpdateSetting(cProfile, "useMCI", m_bUseMCI, libconfig::Setting::TypeBoolean);
        UpdateSetting(cProfile, "performAutoSearch", m_bPerformAutoSearch, libconfig::Setting::TypeBoolean);
        UpdateSetting(cProfile, "RecordAudio", m_iRecordAudio, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "cbwfx", (long &)m_dwCbwFX, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "waveinselected", (long &)m_dwWaveinSelected, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "audio_bits_per_sample", m_iBitsPerSample, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "audio_num_channels", m_iNumChannels, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "audio_samples_per_seconds", m_iSamplesPerSeconds, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "NumDev", m_iMixerDevices, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "SelectedDev", m_iSelectedMixer, libconfig::Setting::TypeInt);
        m_wFormatTag = AudioFormat().wFormatTag;
        UpdateSetting(cProfile, "CompressionFormatTag", m_wFormatTag, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "feedback_line", m_iFeedbackLine, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "feedback_line_info", m_iFeedbackLineInfo, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "InterleaveFrames", m_bInterleaveFrames, libconfig::Setting::TypeBoolean);
        UpdateSetting(cProfile, "InterleaveFactor", m_iInterleaveFactor, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "InterleaveUnit", m_iInterleavePeriod, libconfig::Setting::TypeInt);
        return true;
    }
    bool isInput(eAudioInput eInput) const
    {
        return eInput == m_iRecordAudio;
    }
    bool isAudioFormat() const
    {
        return (m_pwfx) ? true : false;
    }
    WAVEFORMATEX &AudioFormat()
    {
        if (!m_pwfx)
        {
            VERIFY(NewAudio());
        }
        return *m_pwfx;
    }

    bool DeleteAudio();
    bool WriteAudio(LPWAVEFORMATEX pwfx);
    void BuildRecordingFormat();

    bool m_bCompression{true};
    bool m_bInterleaveFrames{false};
    bool m_bUseMCI{true};
    bool m_bPerformAutoSearch{true}; // TODO: not used
    int m_iRecordAudio{NONE};
    int m_iNumChannels{2};
    int m_iBitsPerSample{16};
    int m_iSamplesPerSeconds{44100};
    int m_iInterleaveFactor{100};
    int m_iInterleavePeriod{MILLISECONDS}; // \todo convert to enum
    int m_iMixerDevices{0};
    int m_iSelectedMixer{0};
    int m_iFeedbackLine{0};
    int m_iFeedbackLineInfo{0};
    UINT m_uDeviceID{WAVE_MAPPER};
    int m_wFormatTag{0};
    DWORD m_dwCbwFX{0}; // TODO: ; can be
    DWORD m_dwWaveinSelected{WAVE_FORMAT_4M16};

private:
    bool NewAudio();
    bool CopyAudio(LPWAVEFORMATEX pwfx, DWORD dwCbwFX);
    LPWAVEFORMATEX m_pwfx{nullptr};
};
extern sAudioFormat cAudioFormat;

struct sProducerOpts
{
    sProducerOpts() = default;

    sProducerOpts(const sProducerOpts &rhs)
    {
        *this = rhs;
    }

    sProducerOpts &operator=(const sProducerOpts &rhs)
    {
        if (this == &rhs)
        {
            return *this;
        }

        m_bLaunchPropPrompt = rhs.m_bLaunchPropPrompt;
        m_bLaunchHTMLPlayer = rhs.m_bLaunchHTMLPlayer;
        m_bDeleteAVIAfterUse = rhs.m_bDeleteAVIAfterUse;

        return *this;
    }

    bool Read(libconfig::Setting &cProfile)
    {
        cProfile.lookupValue("launchPropPrompt", m_bLaunchPropPrompt);
        cProfile.lookupValue("launchHTMLPlayer", m_bLaunchHTMLPlayer);
        cProfile.lookupValue("deleteAVIAfterUse", m_bDeleteAVIAfterUse);
        return true;
    }

    bool Write(libconfig::Setting &cProfile)
    {
        UpdateSetting(cProfile, "launchPropPrompt", m_bLaunchPropPrompt, libconfig::Setting::TypeBoolean);
        UpdateSetting(cProfile, "launchHTMLPlayer", m_bLaunchHTMLPlayer, libconfig::Setting::TypeBoolean);
        UpdateSetting(cProfile, "deleteAVIAfterUse", m_bDeleteAVIAfterUse, libconfig::Setting::TypeBoolean);
        return true;
    }

    bool m_bLaunchPropPrompt{false};
    bool m_bLaunchHTMLPlayer{true};
    bool m_bDeleteAVIAfterUse{false};
};
extern sProducerOpts cProducerOpts;

enum eVideoFormat
{
    ModeAVI,
    ModeFlash,
    ModeMP4
};
enum eAVIPlay
{
    NO_PLAYER,
    CAM1_PLAYER,
    DEFAULT_PLAYER,
    CAM2_PLAYER
};

enum eViewType
{
    VIEW_NORMAL,
    VIEW_COMPACT,
    VIEW_BUTTON
};

struct sProgramOpts
{
    sProgramOpts()
        : m_iTempPathAccess(USE_INSTALLED_DIR)
        , m_iOutputPathAccess(USE_INSTALLED_DIR)
        , m_strSpecifiedDir("")
        , m_strDefaultOutDir("")
    {
    }
    sProgramOpts(const sProgramOpts &rhs)
    {
        *this = rhs;
    }
    sProgramOpts &operator=(const sProgramOpts &rhs)
    {
        if (this == &rhs)
        {
            return *this;
        }

        m_bAutoNaming = rhs.m_bAutoNaming;
        m_bCaptureTrans = rhs.m_bCaptureTrans;
        m_bFlashingRect = rhs.m_bFlashingRect;
        m_bMinimizeOnStart = rhs.m_bMinimizeOnStart;
        m_bSaveSettings = rhs.m_bSaveSettings;
        m_bAutoPan = rhs.m_bAutoPan;
        m_bRecordPreset = rhs.m_bRecordPreset;
        m_iPresetTime = rhs.m_iPresetTime;
        m_iMaxPan = rhs.m_iMaxPan;
        m_iRecordingMode = rhs.m_iRecordingMode;
        m_iLaunchPlayer = rhs.m_bSaveSettings;
        m_iTempPathAccess = rhs.m_iTempPathAccess;
        m_iOutputPathAccess = rhs.m_iOutputPathAccess;
        m_iThreadPriority = rhs.m_iThreadPriority;
        m_iViewType = rhs.m_iViewType;
        m_iSaveLen = rhs.m_iSaveLen;
        m_iCursorLen = rhs.m_iCursorLen;
        m_iShapeNameInt = rhs.m_iShapeNameInt;
        m_iLayoutNameInt = rhs.m_iLayoutNameInt;
        m_iSpecifiedDirLength = rhs.m_iSpecifiedDirLength;
        m_strSpecifiedDir = rhs.m_strSpecifiedDir;
        if (m_strSpecifiedDir.GetLength() != m_iSpecifiedDirLength)
        {
            m_iSpecifiedDirLength = m_strSpecifiedDir.GetLength();
        }
        m_strDefaultOutDir = rhs.m_strDefaultOutDir;
        return *this;
    }
    bool Read(libconfig::Setting &cProfile)
    {
        cProfile.lookupValue("AutoNaming", m_bAutoNaming);
        cProfile.lookupValue("CaptureTrans", m_bCaptureTrans);
        cProfile.lookupValue("FlashingRect", m_bFlashingRect);
        cProfile.lookupValue("MinimizeOnStart", m_bMinimizeOnStart);
        cProfile.lookupValue("RecordPreset", m_bRecordPreset);
        cProfile.lookupValue("PresetTime", m_iPresetTime);
        cProfile.lookupValue("RecordingMode", m_iRecordingMode);
        cProfile.lookupValue("LaunchPlayer", m_iLaunchPlayer);
        std::string text;
        if (cProfile.lookupValue("SaveDir", text))
        {
            m_strSpecifiedDir = text.c_str();
        }
        cProfile.lookupValue("TempPathAccess", m_iTempPathAccess);
        cProfile.lookupValue("OutputPathAccess", m_iOutputPathAccess);
        cProfile.lookupValue("ThreadPriority", m_iThreadPriority);
        cProfile.lookupValue("AutoPan", m_bAutoPan);
        cProfile.lookupValue("MaxPan", m_iMaxPan);
        cProfile.lookupValue("ViewType", m_iViewType);
        // TODO: libconfig supports arrays, so the following should be removed in the future
        cProfile.lookupValue("ShapeNameInt", m_iShapeNameInt);
        cProfile.lookupValue("LayoutNameInt", m_iLayoutNameInt);
        // TODO: check these two
        cProfile.lookupValue("LayoutNameLen", m_iSaveLen);
        cProfile.lookupValue("ShapeNameLen", m_iCursorLen);
        if (cProfile.lookupValue("DefaultOutDir", text))
        {
            m_strDefaultOutDir = text.c_str();
        }
        return true;
    }
    bool Write(libconfig::Setting &cProfile)
    {
        UpdateSetting(cProfile, "AutoNaming", m_bAutoNaming, libconfig::Setting::TypeBoolean);
        UpdateSetting(cProfile, "CaptureTrans", m_bCaptureTrans, libconfig::Setting::TypeBoolean);
        UpdateSetting(cProfile, "FlashingRect", m_bFlashingRect, libconfig::Setting::TypeBoolean);
        UpdateSetting(cProfile, "MinimizeOnStart", m_bMinimizeOnStart, libconfig::Setting::TypeBoolean);
        UpdateSetting(cProfile, "RecordPreset", m_bRecordPreset, libconfig::Setting::TypeBoolean);
        UpdateSetting(cProfile, "PresetTime", m_iPresetTime, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "RecordingMode", m_iRecordingMode, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "LaunchPlayer", m_iLaunchPlayer, libconfig::Setting::TypeInt);
        std::string text(m_strSpecifiedDir);
        UpdateSetting(cProfile, "SaveDir", text, libconfig::Setting::TypeString);
        UpdateSetting(cProfile, "TempPathAccess", m_iTempPathAccess, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "OutputPathAccess", m_iOutputPathAccess, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "ThreadPriority", m_iThreadPriority, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "AutoPan", m_bAutoPan, libconfig::Setting::TypeBoolean);
        UpdateSetting(cProfile, "MaxPan", m_iMaxPan, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "ViewType", m_iViewType, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "ShapeNameInt", m_iShapeNameInt, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "LayoutNameInt", m_iLayoutNameInt, libconfig::Setting::TypeInt);
        // TODO: check these two
        UpdateSetting(cProfile, "LayoutNameLen", m_iSaveLen, libconfig::Setting::TypeInt);
        UpdateSetting(cProfile, "ShapeNameLen", m_iCursorLen, libconfig::Setting::TypeInt);
        return true;
    }

    bool m_bAutoNaming{false};
    bool m_bCaptureTrans{true};
    bool m_bFlashingRect{true};
    bool m_bMinimizeOnStart{false};
    bool m_bSaveSettings{true};
    bool m_bAutoPan{false};
    bool m_bRecordPreset{false};
    int m_iPresetTime{60};
    int m_iMaxPan{20};
    int m_iRecordingMode{ModeAVI};
    int m_iLaunchPlayer{CAM2_PLAYER};
    int m_iTempPathAccess{USE_INSTALLED_DIR};   // \convert to enum
    int m_iOutputPathAccess{USE_INSTALLED_DIR}; // \convert to enum
    int m_iThreadPriority{THREAD_PRIORITY_NORMAL};
    int m_iViewType{VIEW_NORMAL};
    int m_iSaveLen{0};
    int m_iCursorLen{0};
    int m_iShapeNameInt{0};
    int m_iLayoutNameInt{0};
    CString m_strSpecifiedDir{""};
    CString m_strDefaultOutDir{""};

private:
    int m_iSpecifiedDirLength{0};
};
extern sProgramOpts cProgramOpts;

extern int iSaveLen;
extern int iCursorLen;
extern int iShapeNameInt;
extern int iLayoutNameInt;
extern int iFrameShift;

// extern int iShapeNameLen;    // string length
// extern int iLayoutNameLen;    // string length
