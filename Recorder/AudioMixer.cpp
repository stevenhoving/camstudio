#include "stdafx.h"
#include "Recorder.h"
#include "AudioMixer.h"

CAudioMixer::CAudioMixer()
:	m_hMixer(0)
{
}

CAudioMixer::~CAudioMixer()
{
	if (isValid())
		Close();
}

void CAudioMixer::OnError(MMRESULT uError)
{
	if (MMSYSERR_NOERROR != uError)
		TRACE("CAudioMixer::OnError: ");

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
	default:
		TRACE("Unknown %d\n", uError);
		break;
	}
}

MMRESULT CAudioMixer::Open(UINT uMxId, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen)
{
	MMRESULT uResult = ::mixerOpen(&m_hMixer, uMxId, dwCallback, dwInstance, fdwOpen);
	OnError(uResult);
	TRACE("CAudioMixer::Open: %s\n", isValid() ? "OK" : "FAIL");
	return uResult;
}

MMRESULT CAudioMixer::Close()
{
	if (!isValid()) {
		OnError(MMSYSERR_INVALHANDLE);
		return MMSYSERR_INVALHANDLE;
	}
	MMRESULT uResult = ::mixerClose(m_hMixer);
	m_hMixer = (MMSYSERR_NOERROR == uResult) ? 0 : m_hMixer;
	OnError(uResult);
	TRACE("CAudioMixer::Close: %s\n", !isValid() ? "OK" : "FAIL");
	return uResult;
}

MMRESULT CAudioMixer::GetDevCaps(LPMIXERCAPS pmxcaps, UINT cbmxcaps)
{
	MMRESULT uResult = ::mixerGetDevCaps(reinterpret_cast<UINT_PTR>(m_hMixer), pmxcaps, cbmxcaps);
	OnError(uResult);
	return uResult;
}

MMRESULT CAudioMixer::GetLineInfo(LPMIXERLINE pmxl, DWORD fdwInfo)
{
	//MMRESULT uResult = ::mixerGetLineInfo(reinterpret_cast<HMIXEROBJ>(m_hMixer), pmxl, fdwInfo);
	MMRESULT uResult = ::mixerGetLineInfo((HMIXEROBJ)(m_hMixer), pmxl, fdwInfo);
	OnError(uResult);
	return uResult;
}

MMRESULT CAudioMixer::GetLineControls(LPMIXERLINECONTROLS pmxlc, DWORD fdwControls)
{
	MMRESULT uResult = ::mixerGetLineControls(reinterpret_cast<HMIXEROBJ>(m_hMixer), pmxlc, fdwControls);
	OnError(uResult);
	return uResult;
}

MMRESULT CAudioMixer::GetControlDetails(LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails)
{
	MMRESULT uResult = ::mixerGetControlDetails(reinterpret_cast<HMIXEROBJ>(m_hMixer), pmxcd, fdwDetails);
	OnError(uResult);
	return uResult;
}

MMRESULT CAudioMixer::SetControlDetails(LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails)
{
	MMRESULT uResult = ::mixerSetControlDetails(reinterpret_cast<HMIXEROBJ>(m_hMixer), pmxcd, fdwDetails);
	OnError(uResult);
	return uResult;
}
