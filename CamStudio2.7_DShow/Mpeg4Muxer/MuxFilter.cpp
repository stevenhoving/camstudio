//
// MuxFilter.cpp
//
// Implementation of classes for DirectShow MPEG-4 Multiplexor filter
//
// Geraint Davies, May 2004
//
// Copyright (c) GDCL 2004-6. All Rights Reserved. 
// You are free to re-use this as the basis for your own filter development,
// provided you retain this copyright notice in the source.
// http://www.gdcl.co.uk
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MuxFilter.h"
#include "MuxInputPin.h"
#include "MuxOutputPin.h"
#include <sstream>

//#include "logger.h"
//Logger theLogger(TEXT("MP4Mux.txt"));



// ---- construction/destruction and COM support -------------

// the class factory calls this to create the filter
//static 
CUnknown* WINAPI 
Mpeg4Mux::CreateInstance(LPUNKNOWN pUnk, HRESULT* phr)
{
    return new Mpeg4Mux(pUnk, phr);
}


Mpeg4Mux::Mpeg4Mux(LPUNKNOWN pUnk, HRESULT* phr)
: CBaseFilter(NAME("Mpeg4Mux"), pUnk, &m_csFilter, *m_sudFilter.clsID),
  m_tWritten(0)
{
    // create output pin and one free input
    m_pOutput = new MuxOutput(this, &m_csFilter, phr);
    CreateInput();
}

Mpeg4Mux::~Mpeg4Mux()
{
    delete m_pOutput;
    for (UINT i = 0; i < m_pInputs.size(); i++)
    {
        m_pInputs[i]->Release();
    }
}


STDMETHODIMP 
Mpeg4Mux::NonDelegatingQueryInterface(REFIID iid, void** ppv)
{
    if (iid == IID_IMediaSeeking)
    {
        return GetInterface((IMediaSeeking*) this, ppv);
    }

    return CBaseFilter::NonDelegatingQueryInterface(iid, ppv);
}

int 
Mpeg4Mux::GetPinCount()
{
    // one output, plus variable set of inputs
    return (int) (m_pInputs.size() + 1);
}

CBasePin *
Mpeg4Mux::GetPin(int n)
{
    // list all inputs first, then one output

    if (n == (int)m_pInputs.size())
    {
        return m_pOutput;
    } else if (n < (int)m_pInputs.size())
    {
        return m_pInputs[n];
    }
    return NULL;
}

void
Mpeg4Mux::CreateInput()
{
	ostringstream strm;
	strm << "Input " << m_pInputs.size() + 1;
	_bstr_t str = strm.str().c_str();

	HRESULT hr = S_OK;
    MuxInput* pPin = new MuxInput(this, &m_csFilter, &hr, str, (int) (m_pInputs.size()));
    pPin->AddRef();
    m_pInputs.push_back(pPin);

    // ensure enumerator is refreshed
    IncrementPinVersion();
}

void 
Mpeg4Mux::OnDisconnect(int index)
{
    // if index is the last but one, and
    // the last one is still unconnected, then
    // remove the last one
    if ((index == (int)(m_pInputs.size() - 2)) &&
        (!m_pInputs[m_pInputs.size()-1]->IsConnected()))
    {
        m_pInputs[m_pInputs.size()-1]->Release();
        m_pInputs.pop_back();

        // refresh enumerator
        IncrementPinVersion();
    }
}

void 
Mpeg4Mux::OnConnect(int index)
{
    // if this is the last one, make a new one
    if (index == (int)(m_pInputs.size()-1))
    {
        CreateInput();
    }
}

bool 
Mpeg4Mux::CanReceive(const CMediaType* pmt)
{
    return TypeHandler::CanSupport(pmt);
}

TrackWriter* 
Mpeg4Mux::MakeTrack(int index, const CMediaType* pmt)
{
    CAutoLock lock(&m_csTracks);
    UNREFERENCED_PARAMETER(index);
    return m_pMovie->MakeTrack(pmt);
}

void 
Mpeg4Mux::OnEOS()
{
    // all tracks are now written
    m_pOutput->DeliverEndOfStream();
}

STDMETHODIMP 
Mpeg4Mux::Stop()
{
    HRESULT hr = S_OK;
    if (m_State != State_Stopped)
    {
		// ensure that queue-writing is stopped
        if (m_pMovie)
        {
            m_pMovie->Stop();

			// switch to IStream for post-run fixup.
			m_pOutput->UseIStream();
        }

        // stop all input pins
        hr = CBaseFilter::Stop();

        if (m_pMovie)
        {
			// write all queued data
			m_pMovie->WriteOnStop();

            // write all metadata
            hr = m_pMovie->Close(&m_tWritten);
            m_pMovie = NULL;

            // fill remaining file space
            m_pOutput->FillSpace();
        }
    }
    return hr;
}
    

STDMETHODIMP 
Mpeg4Mux::Pause()
{
    if (m_State == State_Stopped)
    {
        m_pOutput->Reset();
        m_pMovie = new MovieWriter(m_pOutput);
    }
    return CBaseFilter::Pause();
}
// ------- input pin -------------------------------------------------------

MuxInput::MuxInput(Mpeg4Mux* pFilter, CCritSec* pLock, HRESULT* phr, LPCWSTR pName, int index)
: m_pMux(pFilter),
  m_index(index),
  m_pTrack(NULL),
  m_pCopyAlloc(NULL),
  CBaseInputPin(NAME("MuxInput"), pFilter, pLock, phr, pName)
{
	ZeroMemory(&m_StreamInfo, sizeof(m_StreamInfo));
}

MuxInput::~MuxInput()
{
	if (m_pCopyAlloc)
	{
		m_pCopyAlloc->Release();
	}
}

HRESULT 
MuxInput::CheckMediaType(const CMediaType* pmt)
{
    if (m_pMux->CanReceive(pmt))
    {
        return S_OK;
    }
    return VFW_E_TYPE_NOT_ACCEPTED;
}

HRESULT 
MuxInput::GetMediaType(int iPosition, CMediaType* pmt)
{
    UNREFERENCED_PARAMETER(iPosition);
    UNREFERENCED_PARAMETER(pmt);
    return VFW_S_NO_MORE_ITEMS;
}

STDMETHODIMP 
MuxInput::Receive(IMediaSample* pSample)
{
    HRESULT hr = CBaseInputPin::Receive(pSample);
    if (hr != S_OK)
    {
        return hr;
    }
    if (!m_pTrack)
    {
        return E_FAIL;
    }

	if (ShouldDiscard(pSample))
	{
		return S_OK;
	}
	if (m_pCopyAlloc)
	{
		BYTE* pSrc;
		pSample->GetPointer(&pSrc);
		IMediaSamplePtr pOurs;
		hr = m_pCopyAlloc->AppendAndWrap(pSrc, pSample->GetActualDataLength(), &pOurs);
		if (SUCCEEDED(hr))
		{
			hr = CopySampleProps(pSample, pOurs);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pTrack->Add(pOurs);
		}
		return hr;
	}

    return m_pTrack->Add(pSample);
}

// copy the input buffer to the output
HRESULT
MuxInput::CopySampleProps(IMediaSample* pIn, IMediaSample* pOut)
{
    REFERENCE_TIME tStart, tEnd;
    if (SUCCEEDED(pIn->GetTime(&tStart, &tEnd)))
    {
        pOut->SetTime(&tStart, &tEnd);
    }

	if (SUCCEEDED(pIn->GetMediaTime(&tStart, &tEnd)))
	{
		pOut->SetMediaTime(&tStart, &tEnd);
	}

    if (pIn->IsSyncPoint() == S_OK)
    {
        pOut->SetSyncPoint(true);
    }
    if (pIn->IsDiscontinuity() == S_OK)
    {
        pOut->SetDiscontinuity(true);
    }
    if (pIn->IsPreroll() == S_OK)
    {
        pOut->SetPreroll(true);
    }
	return S_OK;
}

STDMETHODIMP 
MuxInput::EndOfStream()
{
    if ((m_pTrack != NULL) && (m_pTrack->OnEOS()))
    {
        // we are the last -- can forward now
        m_pMux->OnEOS();
    }
    return S_OK;
}

STDMETHODIMP 
MuxInput::BeginFlush()
{
    // ensure no more data accepted, and queued
    // data is discarded, so no threads are blocking
    if (m_pTrack)
    {
        m_pTrack->Stop(true);
    }
    return S_OK;
}

STDMETHODIMP 
MuxInput::EndFlush()
{
    // we don't re-enable writing -- we support only
    // one contiguous sequence in a file.
    return S_OK;
}

HRESULT 
MuxInput::Active()
{
    HRESULT hr = CBaseInputPin::Active();
    if (SUCCEEDED(hr))
    {
		if (m_pCopyAlloc)
		{
			m_CopyBuffer.ResetAbort();
		}

        m_pTrack = m_pMux->MakeTrack(m_index, &m_mt);
    }
    return hr;
}

HRESULT 
MuxInput::Inactive()
{
    // ensure that there are no more writes and no blocking threads
	if (m_pTrack)
	{
		m_pTrack->Stop(false);
		m_pTrack = NULL;
	}
    HRESULT hr = CBaseInputPin::Inactive();
	if (m_pCopyAlloc)
	{
		m_CopyBuffer.Abort();
	}
	return hr;
}

HRESULT 
MuxInput::BreakConnect()
{
    HRESULT hr = CBaseInputPin::BreakConnect();
    m_pMux->OnDisconnect(m_index);
    return hr;
}


HRESULT 
MuxInput::CompleteConnect(IPin *pReceivePin)
{
    HRESULT hr = CBaseInputPin::CompleteConnect(pReceivePin);
    if (SUCCEEDED(hr))
    {
        m_pMux->OnConnect(m_index);
    }
    return hr;
}

STDMETHODIMP 
MuxInput::GetAllocator(IMemAllocator** ppAllocator)
{
    CAutoLock lock(m_pLock);

    HRESULT hr = S_OK;

	// we supply our own allocator whose only purpose is to increase the requested number of
	// buffers. if possible, we supply a sanity-check max buffer size
	long cMaxBuffer = 0;
	TypeHandler* ph = TypeHandler::Make(&m_mt);
	if (ph)
	{
		if (ph->IsVideo())
		{
			cMaxBuffer = ph->Height() * ph->Width() * 4;
		}
		delete ph;
	}

	MuxAllocator* pAlloc = new MuxAllocator(NULL, &hr, cMaxBuffer);
    if (!pAlloc)
    {
        return E_OUTOFMEMORY;
    }
    pAlloc->QueryInterface(IID_IMemAllocator, (void**)&m_pAllocator);
    return pAlloc->QueryInterface(IID_IMemAllocator, (void**)ppAllocator);
}

	
STDMETHODIMP 
MuxInput::NotifyAllocator(IMemAllocator* pAlloc, BOOL bReadOnly)
{
	ALLOCATOR_PROPERTIES propAlloc;
	pAlloc->GetProperties(&propAlloc);

	if (propAlloc.cBuffers < 20)
	{
		// too few buffers -- we need to copy
		// -- base the buffer size on 100 x buffers, but
		// restrict to 200MB max
		int cSpace = propAlloc.cbBuffer * 100;
		cSpace = min((200 * 1024 * 1024), cSpace);
		HRESULT hr = m_CopyBuffer.Allocate(cSpace);
		if (SUCCEEDED(hr))
		{
			m_pCopyAlloc = new Suballocator(&m_CopyBuffer, &hr);
			m_pCopyAlloc->AddRef();
		}
	}
	else
	{
		if (m_pCopyAlloc)
		{
			m_pCopyAlloc->Release();
			m_pCopyAlloc = NULL;
		}
	}
	return __super::NotifyAllocator(pAlloc, bReadOnly);
}
	
STDMETHODIMP
MuxInput::StartAt(const REFERENCE_TIME* ptStart, DWORD dwCookie)
{
	if (ptStart == NULL)
	{
		m_StreamInfo.dwFlags &= ~AM_STREAM_INFO_DISCARDING;
	}
	else if (*ptStart == MAXLONGLONG) 
	{
		// cancels a start request (but does not stop)
		m_StreamInfo.dwFlags &= ~(AM_STREAM_INFO_START_DEFINED);

		// if running, and stop pending, then by some wierd overloading of the spec, this means start now
		if (m_pMux->IsActive() && (m_StreamInfo.dwFlags & AM_STREAM_INFO_STOP_DEFINED))
		{
			m_StreamInfo.dwFlags &= ~AM_STREAM_INFO_DISCARDING;
		}
	}
	else
	{
		m_StreamInfo.dwFlags |= (AM_STREAM_INFO_START_DEFINED | AM_STREAM_INFO_DISCARDING);
		m_StreamInfo.dwStartCookie = dwCookie;
		m_StreamInfo.tStart = *ptStart;
		m_pTrack->SetStartAt(m_StreamInfo.tStart);
		DbgLog((LOG_TRACE, 0, "Mux StartAt %d ms", long(m_StreamInfo.tStart/10000)));
	}
	return S_OK;
	
}

STDMETHODIMP
MuxInput::StopAt(const REFERENCE_TIME* ptStop, BOOL bSendExtra, DWORD dwCookie)
{
	CAutoLock lock(&m_csStreamControl);

	if (ptStop == NULL)
	{
		m_StreamInfo.dwFlags |= AM_STREAM_INFO_DISCARDING;
	}
	else if (*ptStop == MAXLONGLONG) 
	{
		m_StreamInfo.dwFlags &= ~(AM_STREAM_INFO_STOP_DEFINED);
	}
	else
	{
		m_StreamInfo.dwFlags |= AM_STREAM_INFO_STOP_DEFINED | (bSendExtra ? AM_STREAM_INFO_STOP_SEND_EXTRA : 0);
		m_StreamInfo.dwStopCookie = dwCookie;
		m_StreamInfo.tStop = *ptStop;
		DbgLog((LOG_TRACE, 0, "Mux StopAt %d ms", long(m_StreamInfo.tStop/10000)));
	}
	return S_OK;
}

STDMETHODIMP
MuxInput::GetInfo(AM_STREAM_INFO* pInfo)
{
	CAutoLock lock(&m_csStreamControl);

	*pInfo = m_StreamInfo;
	return S_OK;
}

	
bool 
MuxInput::ShouldDiscard(IMediaSample* pSample)
{
	CAutoLock lock(&m_csStreamControl);
	if (m_StreamInfo.dwFlags & AM_STREAM_INFO_DISCARDING)
	{
		if (m_StreamInfo.dwFlags & AM_STREAM_INFO_START_DEFINED)
		{
			REFERENCE_TIME tStart, tStop;
			if ((SUCCEEDED(pSample->GetTime(&tStart, &tStop))) &&
				(tStop > m_StreamInfo.tStart))
			{
				m_StreamInfo.dwFlags &= ~(AM_STREAM_INFO_DISCARDING | AM_STREAM_INFO_START_DEFINED);
				if (m_StreamInfo.dwStartCookie)
				{
					m_pMux->NotifyEvent(EC_STREAM_CONTROL_STARTED, (LONG_PTR) this, m_StreamInfo.dwStartCookie);
					m_StreamInfo.dwStartCookie = 0;
				}
				if ((tStart < m_StreamInfo.tStart) && m_pTrack->Handler()->CanTruncate())
				{
					m_pTrack->Handler()->Truncate(pSample, m_StreamInfo.tStart);
				}
			}
		}
	}
	else
	{
		if (m_StreamInfo.dwFlags & AM_STREAM_INFO_STOP_DEFINED)
		{
			REFERENCE_TIME tStart, tStop;
			if (SUCCEEDED(pSample->GetTime(&tStart, &tStop)))
			{
				DbgLog((LOG_TRACE, 0, "Pending stop %d ms, sample %d", long(m_StreamInfo.tStop/10000), long(tStart/10000)));

				if (tStart >= m_StreamInfo.tStop)
				{
					m_StreamInfo.dwFlags |= AM_STREAM_INFO_DISCARDING;
					m_StreamInfo.dwFlags &= ~(AM_STREAM_INFO_STOP_DEFINED);
					if (m_StreamInfo.dwStopCookie)
					{
						m_pMux->NotifyEvent(EC_STREAM_CONTROL_STOPPED, (LONG_PTR) this, m_StreamInfo.dwStopCookie);
						m_StreamInfo.dwStopCookie = 0;
					}
				}
			}
		}
	}
	return (m_StreamInfo.dwFlags & AM_STREAM_INFO_DISCARDING) ? true : false;
}

// ----------------------


MuxAllocator::MuxAllocator(LPUNKNOWN pUnk, HRESULT* phr, long cMaxBuffer)
: CMemAllocator(NAME("MuxAllocator"), pUnk, phr),
  m_cMaxBuffer(cMaxBuffer)
{
}

// we override this just to increase the requested buffer count
STDMETHODIMP 
MuxAllocator::SetProperties(
        ALLOCATOR_PROPERTIES* pRequest,
        ALLOCATOR_PROPERTIES* pActual)
{
    // !! base buffer count on media type size?

    ALLOCATOR_PROPERTIES prop = *pRequest;

	// some encoders request excessively large output buffers and if we
	// ask for 100 of those, we will use up the system memory
	if ((m_cMaxBuffer == 0) || (prop.cbBuffer <= m_cMaxBuffer))
	{
		if (prop.cBuffers < 100)
		{
			prop.cBuffers = 100;
		}
	}
    return CMemAllocator::SetProperties(&prop, pActual);
}

// --- output --------------------------------------------------------

MuxOutput::MuxOutput(Mpeg4Mux* pFilter, CCritSec* pLock, HRESULT* phr)
: m_pMux(pFilter),
  m_llBytes(0),
  m_bUseIStream(true),		// use IStream always
  CBaseOutputPin(NAME("MuxOutput"), pFilter, pLock, phr, L"Output")
{
}

// CBaseOutputPin overrides
HRESULT 
MuxOutput::CheckMediaType(const CMediaType* pmt)
{
    if (*pmt->Type() == MEDIATYPE_Stream)
    {
        return S_OK;
    }
    return VFW_E_TYPE_NOT_ACCEPTED;
}

HRESULT 
MuxOutput::GetMediaType(int iPosition, CMediaType* pmt)
{
    if (iPosition != 0)
    {
        return VFW_S_NO_MORE_ITEMS;
    }
    pmt->InitMediaType();
    pmt->SetType(&MEDIATYPE_Stream);
    pmt->SetSubtype(&MEDIASUBTYPE_NULL);
    return S_OK;
}

HRESULT 
MuxOutput::DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pprop)
{
    // we can break up large write into multiple buffers, so we are 
    // better off using a few small buffers
    pprop->cbBuffer = 4 * 1024;
    pprop->cBuffers = 20;
    pprop->cbAlign = 1;
    ALLOCATOR_PROPERTIES propActual;
    return pAlloc->SetProperties(pprop, &propActual);
}

HRESULT 
MuxOutput::CompleteConnect(IPin *pReceivePin)
{
    // make sure that this is the file writer, supporting
    // IStream, or we will not be able to write out the metadata
    // at stop time
    IStreamPtr pIStream = pReceivePin;
    if (pIStream == NULL)
    {
        return E_NOINTERFACE;
    }
    return CBaseOutputPin::CompleteConnect(pReceivePin);
}
	
HRESULT 
MuxOutput::BreakConnect()
{
	return __super::BreakConnect();
}

void 
MuxOutput::Reset()
{
    CAutoLock lock(&m_csWrite);
    m_llBytes = 0;
    m_bUseIStream = true;		// always use IStream, so we don't fail when downstream filter is stopped first
}

void
MuxOutput::UseIStream()
{
    CAutoLock lock(&m_csWrite);
    m_bUseIStream = true;
}

LONGLONG 
MuxOutput::Position()
{
    // start of this container in absolute byte position
    return 0;
}

LONGLONG
MuxOutput::Length()
{
    // length of this atom container (ie location of next atom)
    return m_llBytes;
}

HRESULT 
MuxOutput::Append(const BYTE* pBuffer, long cBytes)
{
    HRESULT hr = Replace(m_llBytes, pBuffer, cBytes);
    m_llBytes += cBytes;
    return hr;
}

HRESULT 
MuxOutput::Replace(LONGLONG pos, const BYTE* pBuffer, long cBytes)
{
    // all media content is written when the graph is running,
    // using IMemInputPin. On stop (during our stop, but after the
    // file writer has stopped), we switch to IStream for the metadata.
    // The in-memory index is updated after a successful call to this function, so
    // any data not written on completion of Stop will not be in the index.
    CAutoLock lock(&m_csWrite);

    HRESULT hr = S_OK;
    if (m_bUseIStream)
    {
        IStreamPtr pStream = GetConnected();
        if (pStream == NULL)
        {
            hr = E_NOINTERFACE;
        } else {
            LARGE_INTEGER liTo;
            liTo.QuadPart = pos;
            ULARGE_INTEGER uliUnused;
            hr = pStream->Seek(liTo, STREAM_SEEK_SET, &uliUnused);
            if (SUCCEEDED(hr))
            {
                ULONG cActual;
                hr = pStream->Write(pBuffer, cBytes, &cActual);
                if (SUCCEEDED(hr) && ((long)cActual != cBytes))
                {
                    hr = E_FAIL;
                }
            }
        }
    } else {
        // where the buffer boundaries lie is not important in this 
        // case, so break writes up into the buffers.
        while (cBytes && (hr == S_OK))
        {
            IMediaSamplePtr pSample;
            hr = GetDeliveryBuffer(&pSample, NULL, NULL, 0);
            if (SUCCEEDED(hr))
            {
                long cThis = min(pSample->GetSize(), cBytes);
                BYTE* pDest;
                pSample->GetPointer(&pDest);
                CopyMemory(pDest, pBuffer,  cThis);
                pSample->SetActualDataLength(cThis);
    
                // time stamps indicate file position in bytes
                LONGLONG tStart = pos;
                LONGLONG tEnd = pos + cThis;
                pSample->SetTime(&tStart, &tEnd);
                hr = Deliver(pSample);
                if (SUCCEEDED(hr))
                {
                    pBuffer += cThis;
                    cBytes -= cThis;
                    pos += cThis;
                }
            }
        }
    }
    return hr;
}
    
void 
MuxOutput::FillSpace()
{
    IStreamPtr pStream = GetConnected();
    if (pStream != NULL)
    {
        LARGE_INTEGER li0;
        li0.QuadPart = 0;
        ULARGE_INTEGER uliEnd;
        HRESULT hr = pStream->Seek(li0, STREAM_SEEK_END, &uliEnd);
        if (SUCCEEDED(hr))
        {
            if (uliEnd.QuadPart > (ULONGLONG) m_llBytes)
            {
                LONGLONG free = uliEnd.QuadPart - m_llBytes;
                if ((free < 0x7fffffff) && (free >= 8))
                {
                    // create a free chunk
                    BYTE b[8];
                    WriteLong(long(free), b);
                    WriteLong(DWORD('free'), b+4);
                    Append(b, 8);
                }
            }
        }
    }
}

// ---- seeking support ------------------------------------------------

SeekingAggregator::SeekingAggregator(CBaseFilter* pFilter, bool bSetTimeFormat)
: m_bSetTimeFormat(bSetTimeFormat)
{
    // collect all pins that support seeking.
    // if bSet is true, collect only those that
    // allow SetTimeFormat -- this will be one per splitter
    for (int i = 0; i < pFilter->GetPinCount(); i++)
    {
        CBasePin* pPin = pFilter->GetPin(i);
        PIN_DIRECTION pindir;
        pPin->QueryDirection(&pindir);
        if (pindir == PINDIR_INPUT)
        {
            IMediaSeekingPtr pSeek = pPin->GetConnected();
            if (pSeek != NULL)
            {
                HRESULT hr = S_OK;
                if (m_bSetTimeFormat)
                {
                    hr = pSeek->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);
                }
                if (SUCCEEDED(hr))
                {
                    m_Pins.push_back(pSeek.Detach());
                }
            }
        }
    }
}

SeekingAggregator::~SeekingAggregator()
{
    for (iterator i = Begin(); i != End(); i++)
    {
        IMediaSeekingPtr pSeek = *i;
        // undo the SetTimeFormat call
        if (m_bSetTimeFormat)
        {
            pSeek->SetTimeFormat(&TIME_FORMAT_NONE);
        }
        pSeek->Release();
    }
}

STDMETHODIMP 
Mpeg4Mux::GetCurrentPosition(LONGLONG *pCurrent)
{
    if (m_pMovie == NULL)
    {
        // return previous total (after Stop)
        *pCurrent = m_tWritten;
    } else {
        // this is not passed upstream -- we report the 
        // position of the mux. Report the earliest write
        // time of any pin
        REFERENCE_TIME tCur = m_pMovie->CurrentPosition();
        *pCurrent = tCur;
    }

    return S_OK;
}

STDMETHODIMP 
Mpeg4Mux::GetCapabilities(DWORD * pCapabilities )
{
    // OR together all the pins' capabilities, together with our own
    DWORD caps = AM_SEEKING_CanGetCurrentPos;
    SeekingAggregator pins(this);
    for (SeekingAggregator::iterator i = pins.Begin(); i != pins.End(); i++)
    {
        IMediaSeekingPtr pSeek = *i;
        DWORD dwThis;
        HRESULT hr = pSeek->GetCapabilities(&dwThis);
        if (SUCCEEDED(hr))
        {
            caps |= dwThis;
        }
    }
    *pCapabilities = caps;
    return S_OK;
}

STDMETHODIMP 
Mpeg4Mux::CheckCapabilities(DWORD * pCapabilities )
{
    DWORD dwActual;
    GetCapabilities(&dwActual);
    if (*pCapabilities & (~dwActual)) {
        return S_FALSE;
    }
    return S_OK;
}

STDMETHODIMP 
Mpeg4Mux::IsFormatSupported(const GUID * pFormat)
{
    if (*pFormat == TIME_FORMAT_MEDIA_TIME)
    {
        return S_OK;
    }
    return S_FALSE;
}

STDMETHODIMP 
Mpeg4Mux::QueryPreferredFormat(GUID * pFormat)
{
    *pFormat = TIME_FORMAT_MEDIA_TIME;
    return S_OK;
}

STDMETHODIMP 
Mpeg4Mux::GetTimeFormat(GUID *pFormat)
{
    *pFormat = TIME_FORMAT_MEDIA_TIME;
    return S_OK;
}

STDMETHODIMP 
Mpeg4Mux::IsUsingTimeFormat(const GUID * pFormat)
{
    if (*pFormat == TIME_FORMAT_MEDIA_TIME)
    {
        return S_OK;
    }
    return S_FALSE;
}

STDMETHODIMP 
Mpeg4Mux::SetTimeFormat(const GUID * pFormat)
{
    if ((*pFormat == TIME_FORMAT_MEDIA_TIME) ||
        (*pFormat == TIME_FORMAT_NONE))
    {
        return S_OK;
    }
    return VFW_E_NO_TIME_FORMAT;
}

STDMETHODIMP 
Mpeg4Mux::GetDuration(LONGLONG *pDuration)
{
    // find the longest of all input durations
    SeekingAggregator pins(this);
    REFERENCE_TIME tMax = 0;
    for (SeekingAggregator::iterator i = pins.Begin(); i != pins.End(); i++)
    {
        IMediaSeekingPtr pSeek = *i;
        REFERENCE_TIME t;
        pSeek->GetDuration(&t);
        if (t > tMax)
        {
            tMax = t;
        }
    }
    *pDuration = tMax;
    return S_OK;
}

STDMETHODIMP 
Mpeg4Mux::GetStopPosition(LONGLONG *pStop)
{
    // return the first valid entry
    SeekingAggregator pins(this);
    if (pins.Begin() == pins.End())
    {
        return E_NOINTERFACE;
    }
    IMediaSeekingPtr pSeek = *pins.Begin();
    return pSeek->GetStopPosition(pStop);
}

STDMETHODIMP 
Mpeg4Mux::ConvertTimeFormat(LONGLONG * pTarget, const GUID * pTargetFormat,
                          LONGLONG    Source, const GUID * pSourceFormat )
{
    if (((pTargetFormat == 0) || (*pTargetFormat == TIME_FORMAT_MEDIA_TIME)) &&
        ((pSourceFormat == 0) || (*pSourceFormat == TIME_FORMAT_MEDIA_TIME)))
    {
        *pTarget = Source;
        return S_OK;
    }
    return VFW_E_NO_TIME_FORMAT;
}

STDMETHODIMP 
Mpeg4Mux::SetPositions(LONGLONG * pCurrent, DWORD dwCurrentFlags
        , LONGLONG * pStop, DWORD dwStopFlags )
{
    // must be passed to all valid inputs -- all must succeed
    SeekingAggregator pins(this, true);
    HRESULT hr = S_OK;
    for (SeekingAggregator::iterator i = pins.Begin(); i != pins.End(); i++)
    {
        IMediaSeekingPtr pSeek = *i;
        HRESULT hrThis = pSeek->SetPositions(pCurrent, dwCurrentFlags, pStop, dwStopFlags);
        if (FAILED(hrThis) && SUCCEEDED(hr))
        {
            hr = hrThis;
        }
    }
    return hr;
}

STDMETHODIMP 
Mpeg4Mux::GetPositions(LONGLONG * pCurrent,
                          LONGLONG * pStop )
{
    // return first valid input
    SeekingAggregator pins(this);
    if (pins.Begin() == pins.End())
    {
        return E_NOINTERFACE;
    }
    IMediaSeekingPtr pSeek = *pins.Begin();
    return pSeek->GetPositions(pCurrent, pStop);
}

STDMETHODIMP 
Mpeg4Mux::GetAvailable(LONGLONG * pEarliest, LONGLONG * pLatest )
{
    // the available section is the area for which any
    // data is available -- and here it is not very important whether
    // it is actually available
    *pEarliest = 0;
    return GetDuration(pLatest);
}

STDMETHODIMP 
Mpeg4Mux::SetRate(double dRate)
{
    // must be passed to all valid inputs -- all must succeed
    SeekingAggregator pins(this, true);
    HRESULT hr = S_OK;
    for (SeekingAggregator::iterator i = pins.Begin(); i != pins.End(); i++)
    {
        IMediaSeekingPtr pSeek = *i;
        HRESULT hrThis = pSeek->SetRate(dRate);
        if (FAILED(hrThis) && SUCCEEDED(hr))
        {
            hr = hrThis;
        }
    }
    return hr;
}

STDMETHODIMP 
Mpeg4Mux::GetRate(double * pdRate)
{
    // return first valid input
    SeekingAggregator pins(this);
    if (pins.Begin() == pins.End())
    {
        return E_NOINTERFACE;
    }
    IMediaSeekingPtr pSeek = *pins.Begin();
    return pSeek->GetRate(pdRate);
}

STDMETHODIMP 
Mpeg4Mux::GetPreroll(LONGLONG * pllPreroll)
{
    // preroll time needed is the longest of any
    SeekingAggregator pins(this);
    REFERENCE_TIME tMax = 0;
    for (SeekingAggregator::iterator i = pins.Begin(); i != pins.End(); i++)
    {
        IMediaSeekingPtr pSeek = *i;
        REFERENCE_TIME t;
        HRESULT hr = pSeek->GetPreroll(&t);

        if ((hr == S_OK) && (t > tMax))
        {
            tMax = t;
        }
    }
    *pllPreroll = tMax;
    return S_OK;
}

