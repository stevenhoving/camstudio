#pragma once

// output pin, writes multiplexed data downstream
// using IMemOutputPin while running, and then writes 
// metadata using IStream::Write when stopping.
class MuxOutput 
: public CBaseOutputPin,
  public AtomWriter
{
public:
    MuxOutput(Mpeg4Mux* pFilter, CCritSec* pLock, HRESULT* phr);

    // CBaseOutputPin overrides
    HRESULT CheckMediaType(const CMediaType* pmt);
    HRESULT GetMediaType(int iPosition, CMediaType* pmt);
    HRESULT DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pprop);
    HRESULT CompleteConnect(IPin *pReceivePin);
	HRESULT BreakConnect();

    // called from filter
    void Reset();
    void UseIStream();
    void FillSpace();

    // AtomWriter methods
    LONGLONG Length();
    LONGLONG Position();
    HRESULT Replace(LONGLONG pos, const BYTE* pBuffer, long cBytes);
    HRESULT Append(const BYTE* pBuffer, long cBytes);
private:
    Mpeg4Mux* m_pMux;
    CCritSec m_csWrite;
    bool m_bUseIStream;
    LONGLONG m_llBytes;
};