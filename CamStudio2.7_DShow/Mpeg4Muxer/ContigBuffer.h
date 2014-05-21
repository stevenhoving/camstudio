// 
// alloc.h
//
// Implementation of IMemAllocator that uses a single contiguous
// buffer with lockable regions instead of a pool of fixed size buffers.
//
// Geraint Davies, January 2013

#pragma once

#include <utility>
typedef std::pair<int, int> lock_t;
typedef std::list<lock_t> lock_list_t;

class ContigBuffer
{
public:
	ContigBuffer(int cSpace = 0);
	~ContigBuffer();
	HRESULT Allocate(int cSpace);

	BYTE* Append(const BYTE* p, int c);
	HRESULT Consume(const BYTE* p, int c);

	HRESULT Lock(const BYTE* p, int c);
	HRESULT Unlock(const BYTE* p, int c);

	BYTE* AppendAndLock(const BYTE* p, int c);

	BYTE* ValidRegion();
	int ValidLength();

	void Abort();
	void ResetAbort();
private:
	bool SearchLocks(int index, int indexEnd);

private:
	CCritSec m_csLocks;
	CAMEvent m_evLocks;
	lock_list_t m_locks;
	bool m_bAbort;

	int m_idxRead;
	int m_cValid;
	int m_cSpace;
	BYTE* m_pBuffer;
};

// creates IMediaSample objects that
// are subsamples of another buffer
class Suballocator : public CBaseAllocator
{
public:
	Suballocator(ContigBuffer* pSource, HRESULT* phr)
	: m_pSource(pSource),
      CBaseAllocator(NAME("Suballocator"), NULL, phr)
	{}

    // call this to append data to the buffer and create a sample object
	HRESULT AppendAndWrap(BYTE* pBuffer, int cBytes, IMediaSample** ppSample)
	{
		BYTE* pDest = m_pSource->AppendAndLock(pBuffer, cBytes);
		if (!pDest)
		{
			return E_FAIL;
		}
		HRESULT hr = S_OK;
		CMediaSample* pS = new CMediaSample(NAME("CMediaSample"), this, &hr);
		pS->SetPointer(pDest, cBytes);

		IMediaSamplePtr pSample = pS;
		*ppSample = pSample.Detach();
		return S_OK;
	}

    // IMemAllocator methods
	STDMETHODIMP SetProperties(ALLOCATOR_PROPERTIES* pRequest, ALLOCATOR_PROPERTIES* pActual)
	{
		UNREFERENCED_PARAMETER(pRequest);
		UNREFERENCED_PARAMETER(pActual);
		return S_OK;
	}
    STDMETHODIMP GetProperties(ALLOCATOR_PROPERTIES* pProps)
	{
		ZeroMemory(pProps, sizeof(ALLOCATOR_PROPERTIES));
		pProps->cBuffers = 100;
		pProps->cbBuffer = 64 * 1024;
		return S_OK;
	}

    STDMETHODIMP Commit()
	{
		return S_OK;
	}
    STDMETHODIMP Decommit()
	{
		return S_OK;
	}
    STDMETHODIMP GetBuffer(IMediaSample **ppBuffer, REFERENCE_TIME * pStartTime,
                           REFERENCE_TIME * pEndTime,
                           DWORD dwFlags)
	{
		UNREFERENCED_PARAMETER(ppBuffer);
		UNREFERENCED_PARAMETER(pStartTime);
		UNREFERENCED_PARAMETER(pEndTime);
		UNREFERENCED_PARAMETER(dwFlags);

		return E_NOTIMPL;
	}
    // called by CMediaSample on last release
    STDMETHODIMP ReleaseBuffer(IMediaSample *pBuffer)
	{
		BYTE* pBuf;
		pBuffer->GetPointer(&pBuf);
		m_pSource->Unlock(pBuf, pBuffer->GetActualDataLength());

		CMediaSample* pS = static_cast<CMediaSample*>(pBuffer);
		delete pS;
		return S_OK;
	}

private:
	void Free() {}

private:
	ContigBuffer* m_pSource;
};
