#pragma once

class CAudioMixer
{
public:
    CAudioMixer();
    virtual ~CAudioMixer();

    operator HMIXER() const
    {
        return m_hMixer;
    }
    bool isValid() const
    {
        return 0 != m_hMixer;
    }

    static void OnError(MMRESULT uError, const TCHAR *lpszFunction = nullptr);
    MMRESULT Close();
    MMRESULT GetControlDetails(LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails);
    MMRESULT GetDevCaps(LPMIXERCAPS pmxcaps, UINT cbmxcaps = sizeof(MIXERCAPS));
    MMRESULT GetLineControls(LPMIXERLINECONTROLS pmxlc, DWORD fdwControls);
    MMRESULT GetLineInfo(LPMIXERLINE pmxl, DWORD fdwInfo);
    MMRESULT Open(UINT uMxId, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen);
    MMRESULT SetControlDetails(LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails);

private:
    HMIXER m_hMixer;
    MIXERCAPS m_sMixerCaps;
    MIXERLINE m_sMixerLine;
    UINT m_device_count;

    MMRESULT GetID(UINT FAR *puMxId, DWORD fdwId = MIXER_OBJECTF_HMIXER);
    DWORD Message(UINT driverID, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
#ifdef _DEBUG
    bool query();
    bool queryAll();
#endif
};
