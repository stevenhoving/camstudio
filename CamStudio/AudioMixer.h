#pragma once

class CAudioMixer
{
public:
	CAudioMixer();
	virtual ~CAudioMixer();

	operator HMIXER() const	{return m_hMixer;}
	BOOL isValid() const	{return 0 != m_hMixer;}

	MMRESULT Open(UINT uMxId, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen);
	MMRESULT Close();
	MMRESULT GetDevCaps(LPMIXERCAPS pmxcaps, UINT cbmxcaps);
	MMRESULT GetLineInfo(LPMIXERLINE pmxl, DWORD fdwInfo);
	MMRESULT GetLineControls(LPMIXERLINECONTROLS pmxlc, DWORD fdwControls);
	MMRESULT GetControlDetails(LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails);
	MMRESULT SetControlDetails(LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails);

private:
	HMIXER m_hMixer;
public:
	static void OnError(MMRESULT uError);
};

