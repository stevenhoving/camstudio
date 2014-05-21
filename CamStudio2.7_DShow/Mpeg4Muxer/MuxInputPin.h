#pragma once
#include "ContigBuffer.h"
// input pin, receives data corresponding to one
// media track in the file.
// Pins are created and deleted dynamically to 
// ensure that there is always one unconnected pin.
class MuxInput 
: public CBaseInputPin,
  public IAMStreamControl
{
public:
    MuxInput(Mpeg4Mux* pFilter, CCritSec* pLock, HRESULT* phr, LPCWSTR pName, int index);
	~MuxInput();

    // lifetime management for pins is normally delegated to the filter, but
    // we need to be able to create and delete them independently, so keep 
    // a separate refcount.
    STDMETHODIMP_(ULONG) NonDelegatingRelease()
    {
        return CUnknown::NonDelegatingRelease();
    }
    STDMETHODIMP_(ULONG) NonDelegatingAddRef()
    {
        return CUnknown::NonDelegatingAddRef();
    }

    DECLARE_IUNKNOWN
    STDMETHODIMP NonDelegatingQueryInterface(REFIID iid, void** ppv)
	{
		if (iid == IID_IAMStreamControl)
		{
			return GetInterface((IAMStreamControl*) this, ppv);
		}
		return __super::NonDelegatingQueryInterface(iid, ppv);
	}

    // CBasePin overrides
    HRESULT CheckMediaType(const CMediaType* pmt);
    HRESULT GetMediaType(int iPosition, CMediaType* pmt);
    
    // input
    STDMETHODIMP Receive(IMediaSample* pSample);
    STDMETHODIMP EndOfStream();
    STDMETHODIMP BeginFlush();
    STDMETHODIMP EndFlush();

    // state change
    HRESULT Active();
    HRESULT Inactive();

    // connection management -- used to maintain one free pin
    HRESULT BreakConnect();
    HRESULT CompleteConnect(IPin *pReceivePin);

    // support custom allocator
    STDMETHODIMP GetAllocator(IMemAllocator** ppAllocator);
	STDMETHODIMP NotifyAllocator(IMemAllocator* pAlloc, BOOL bReadOnly);

	// IAMStreamControl methods
	STDMETHOD(StartAt)(const REFERENCE_TIME* ptStart, DWORD dwCookie);
	STDMETHOD(StopAt)(const REFERENCE_TIME* ptStop, BOOL bSendExtra, DWORD dwCookie);
	STDMETHOD(GetInfo)(AM_STREAM_INFO* pInfo);

private:
	bool ShouldDiscard(IMediaSample* pSample);
	HRESULT CopySampleProps(IMediaSample* pIn, IMediaSample* pOut);

private:
    Mpeg4Mux* m_pMux;
    int m_index;
    TrackWriter* m_pTrack;

	CCritSec m_csStreamControl;
	AM_STREAM_INFO m_StreamInfo;

	ContigBuffer m_CopyBuffer;
	Suballocator* m_pCopyAlloc;
};
