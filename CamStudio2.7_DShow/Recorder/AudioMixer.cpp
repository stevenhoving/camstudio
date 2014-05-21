#include "stdafx.h"
#include "Recorder.h"
#include "AudioMixer.h"

UINT CAudioMixer::m_uDevices = ::mixerGetNumDevs();

CAudioMixer::CAudioMixer()
:	m_hMixer(0)
{
	m_sMixerLine.cbStruct = sizeof(m_sMixerLine);
	ASSERT(queryAll());
}

CAudioMixer::~CAudioMixer()
{
	if (isValid())
		Close();
}

void CAudioMixer::OnError(MMRESULT uError, LPTSTR lpszFunction)
{
	if (!lpszFunction)
		lpszFunction = _T("CAudioMixer::OnError");
	if (MMSYSERR_NOERROR != uError) {
		TRACE("CAudioMixer::OnError : %s : ", lpszFunction);
	}

	switch (uError)
	{
	case MMSYSERR_NOERROR:		break;
	case MMSYSERR_ERROR:		TRACE("unspecified error\n"); break;
	case MMSYSERR_BADDEVICEID:	TRACE("The uMxId parameter specifies an invalid device identifier.\n"); break;
	case MMSYSERR_NOTENABLED:	TRACE("driver failed enable\n"); break;
	case MMSYSERR_ALLOCATED:	TRACE("The specified resource is already allocated by the maximum number of clients possible.\n"); break;
	case MMSYSERR_INVALHANDLE:	TRACE("The uMxId parameter specifies an invalid handle.\n"); break;
	case MMSYSERR_NODRIVER:		TRACE("No mixer device is available for the object specified by uMxId. Note that the location referenced by uMxId will also contain the value  - 1.\n"); break;
	case MMSYSERR_NOMEM:		TRACE("Unable to allocate resources.\n"); break;
	case MMSYSERR_NOTSUPPORTED:	TRACE("function isn't supported\n"); break;
	case MMSYSERR_BADERRNUM:	TRACE("error value out of range\n"); break;
	case MMSYSERR_INVALFLAG:	TRACE("One or more flags are invalid.\n"); break;
	case MMSYSERR_INVALPARAM:	TRACE("One or more parameters are invalid.\n"); break;
	case MMSYSERR_HANDLEBUSY:	TRACE("handle being used simultaneously on another thread (eg callback)\n"); break;
	case MMSYSERR_INVALIDALIAS:	TRACE("specified alias not found\n"); break;
	case MMSYSERR_BADDB:		TRACE("bad registry database\n"); break;
	case MMSYSERR_KEYNOTFOUND:	TRACE("registry key not found\n"); break;
	case MMSYSERR_READERROR:	TRACE("registry read error\n"); break;
	case MMSYSERR_WRITEERROR:	TRACE("registry write error\n"); break;
	case MMSYSERR_DELETEERROR:	TRACE("registry delete error\n"); break;
	case MMSYSERR_VALNOTFOUND:	TRACE("registry value not found\n"); break;
	case MMSYSERR_NODRIVERCB:	TRACE("driver does not call DriverCallback\n"); break;
	case MMSYSERR_MOREDATA:		TRACE("more data to be returned\n"); break;
	case MIXERR_INVALLINE:		TRACE("The audio line reference is invalid.\n"); break;
	case MIXERR_INVALCONTROL:	TRACE("The control reference is invalid.\n"); break;
	case MIXERR_INVALVALUE:		TRACE("The parameter value is invalid.\n"); break;
	default:
		{
			TCHAR pszText[MAXERRORLENGTH];
			MMRESULT mmCode = ::waveInGetErrorText(uError, pszText, MAXERRORLENGTH);
			if (MMSYSERR_NOERROR == mmCode) {
				TRACE("Error (%d) : %s\n", uError, pszText);
			} else {
				TRACE("Unknown (%d)\n", uError);
			}
		}
		break;
	}
	if (MMSYSERR_NOERROR != uError) {
		::OnError(lpszFunction);
	}
}

MMRESULT CAudioMixer::Open(UINT uMxId, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen)
{
	MMRESULT uResult = ::mixerOpen(&m_hMixer, uMxId, dwCallback, dwInstance, fdwOpen);
	OnError(uResult, _T("CAudioMixer::Open"));
	TRACE("CAudioMixer::Open: %s\n", isValid() ? "OK" : "FAIL");
	return uResult;
}

MMRESULT CAudioMixer::Close()
{
	if (!isValid()) {
		OnError(MMSYSERR_INVALHANDLE, _T("CAudioMixer::Close"));
		return MMSYSERR_INVALHANDLE;
	}
	MMRESULT uResult = ::mixerClose(m_hMixer);
	m_hMixer = (MMSYSERR_NOERROR == uResult) ? 0 : m_hMixer;
	OnError(uResult, _T("CAudioMixer::Close"));
	TRACE("CAudioMixer::Close: %s\n", !isValid() ? "OK" : "FAIL");
	return uResult;
}

MMRESULT CAudioMixer::GetDevCaps(LPMIXERCAPS pmxcaps, UINT cbmxcaps)
{
	MMRESULT uResult = ::mixerGetDevCaps(reinterpret_cast<UINT_PTR>(m_hMixer), pmxcaps, cbmxcaps);
	OnError(uResult, _T("CAudioMixer::GetDevCaps"));
	if (MMSYSERR_NOERROR == uResult) {
		m_sMixerCaps = *pmxcaps;
	}
	return uResult;
}

// The mixerGetLineInfo function retrieves information about a specific line
// of a mixer device.
MMRESULT CAudioMixer::GetLineInfo(LPMIXERLINE pmxl, DWORD fdwInfo)
{
	// The cbStruct member must always be initialized to be the size,
	// in bytes, of the MIXERLINE structure.
	ASSERT(sizeof(MIXERLINE) == pmxl->cbStruct);
	//ASSERT((0UL <= pmxl->dwDestination) && (pmxl->dwDestination < m_sMixerCaps.cDestinations));
	MMRESULT uResult = ::mixerGetLineInfo(reinterpret_cast<HMIXEROBJ>(m_hMixer), pmxl, fdwInfo);
	OnError(uResult, _T("CAudioMixer::GetLineInfo"));
	return uResult;
}

MMRESULT CAudioMixer::GetLineControls(LPMIXERLINECONTROLS pmxlc, DWORD fdwControls)
{
	MMRESULT uResult = ::mixerGetLineControls(reinterpret_cast<HMIXEROBJ>(m_hMixer), pmxlc, fdwControls);
	OnError(uResult, _T("CAudioMixer::GetLineControls"));
	return uResult;
}

MMRESULT CAudioMixer::GetControlDetails(LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails)
{
	MMRESULT uResult = ::mixerGetControlDetails(reinterpret_cast<HMIXEROBJ>(m_hMixer), pmxcd, fdwDetails);
	OnError(uResult, _T("CAudioMixer::GetControlDetails"));
	return uResult;
}

MMRESULT CAudioMixer::SetControlDetails(LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails)
{
	MMRESULT uResult = ::mixerSetControlDetails(reinterpret_cast<HMIXEROBJ>(m_hMixer), pmxcd, fdwDetails);
	OnError(uResult, _T("CAudioMixer::SetControlDetails"));
	return uResult;
}

MMRESULT CAudioMixer::GetID(UINT * puMxId, DWORD fdwId)
{
	MMRESULT uResult = ::mixerGetID(reinterpret_cast<HMIXEROBJ>(m_hMixer), puMxId, fdwId);
	OnError(uResult, _T("CAudioMixer::GetID"));
	return uResult;
}
DWORD CAudioMixer::Message(UINT driverID, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	DWORD dwResult = ::mixerMessage(reinterpret_cast<HMIXER>(driverID), uMsg, dwParam1, dwParam2);
	OnError(dwResult, _T("CAudioMixer::Message"));
	return dwResult;
}
#ifdef _DEBUG
bool CAudioMixer::query()
{
	bool bResult = isValid();
	if (!bResult)
		return !bResult;	// only test valid devices

#define EXPERIMENTAL_CODE
#ifdef EXPERIMENTAL_CODE
#endif	//EXPERIMENTAL_CODE
#undef EXPERIMENTAL_CODE

	VERIFY(MMSYSERR_NOERROR == GetDevCaps(&m_sMixerCaps));
	TRACE("Mixer: %s\nVersion: %d.%d\nDestinations: %d\n",
		m_sMixerCaps.szPname,
		HIBYTE(m_sMixerCaps.vDriverVersion),
		LOBYTE(m_sMixerCaps.vDriverVersion),
		m_sMixerCaps.cDestinations);

	std::vector <DWORD> vDestComponentType;
	vDestComponentType.push_back(MIXERLINE_COMPONENTTYPE_DST_DIGITAL);
	vDestComponentType.push_back(MIXERLINE_COMPONENTTYPE_DST_LINE);
	vDestComponentType.push_back(MIXERLINE_COMPONENTTYPE_DST_MONITOR);
	vDestComponentType.push_back(MIXERLINE_COMPONENTTYPE_DST_SPEAKERS);
	vDestComponentType.push_back(MIXERLINE_COMPONENTTYPE_DST_HEADPHONES);
	vDestComponentType.push_back(MIXERLINE_COMPONENTTYPE_DST_TELEPHONE);
	vDestComponentType.push_back(MIXERLINE_COMPONENTTYPE_DST_WAVEIN);
	vDestComponentType.push_back(MIXERLINE_COMPONENTTYPE_DST_VOICEIN);	// optional

	std::vector <DWORD> vSrcComponentType;
	vSrcComponentType.push_back(MIXERLINE_COMPONENTTYPE_SRC_DIGITAL);
	vSrcComponentType.push_back(MIXERLINE_COMPONENTTYPE_SRC_LINE);
	vSrcComponentType.push_back(MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE);
	vSrcComponentType.push_back(MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER);
	vSrcComponentType.push_back(MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC);
	vSrcComponentType.push_back(MIXERLINE_COMPONENTTYPE_SRC_TELEPHONE);
	vSrcComponentType.push_back(MIXERLINE_COMPONENTTYPE_SRC_PCSPEAKER);
	vSrcComponentType.push_back(MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT);
	vSrcComponentType.push_back(MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY);
	vSrcComponentType.push_back(MIXERLINE_COMPONENTTYPE_SRC_ANALOG);

	for (std::vector<DWORD>::iterator iter = vDestComponentType.begin(); iter != vDestComponentType.end(); ++iter) {
		::ZeroMemory(&m_sMixerLine, sizeof(MIXERLINE));
		m_sMixerLine.cbStruct = sizeof(MIXERLINE);
		m_sMixerLine.dwSource = 0UL;
		m_sMixerLine.dwDestination = 0UL;
		if (MMSYSERR_NOERROR == GetLineInfo(&m_sMixerLine, MIXER_GETLINEINFOF_COMPONENTTYPE)) {
			TRACE("Device : %s\nLine : %d\nDest : %d\nSource : %d\n"
				, m_sMixerLine.szName
				, m_sMixerLine.dwLineID
				, m_sMixerLine.dwDestination
				, m_sMixerLine.dwSource);
		} else {
			DWORD dwError = ::GetLastError();
			if (ERROR_SUCCESS != dwError) {
				::SetLastError(ERROR_SUCCESS);	// reset the error
			}
		}
	}
	for (std::vector<DWORD>::iterator iter = vSrcComponentType.begin(); iter != vSrcComponentType.end(); ++iter) {
		::ZeroMemory(&m_sMixerLine, sizeof(m_sMixerLine));
		m_sMixerLine.cbStruct = sizeof(m_sMixerLine);
		m_sMixerLine.dwComponentType = *iter;
		if (MMSYSERR_NOERROR == GetLineInfo(&m_sMixerLine, MIXER_GETLINEINFOF_COMPONENTTYPE)) {
			TRACE("Device : %s\nLine : %d\nDest : %d\nSource : %d\n"
				, m_sMixerLine.szName
				, m_sMixerLine.dwLineID
				, m_sMixerLine.dwDestination
				, m_sMixerLine.dwSource);
		} else {
			DWORD dwError = ::GetLastError();
			if (ERROR_SUCCESS != dwError) {
				::SetLastError(ERROR_SUCCESS);	// reset the error
			}
		}
	}
	return bResult;
}

bool CAudioMixer::queryAll()
{
#define EXPERIMENTAL_CODE
#ifdef EXPERIMENTAL_CODE
#endif	//EXPERIMENTAL_CODE
#undef EXPERIMENTAL_CODE

	bool bResult = isValid();
	if (bResult)
		return !bResult;	// only test when no valid devices

	// open each mixer ID
	for (UINT uID = 0; uID < m_uDevices; ++uID) {
		MMRESULT uResult = Open(uID, 0, 0, MIXER_OBJECTF_MIXER);
		if (MMSYSERR_NOERROR != uResult) {
			continue;
		}
		query();

		Close();
	}
	return true;
}

#endif	// _DEBUG

