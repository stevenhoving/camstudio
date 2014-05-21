// 
// alloc.cpp
//
// Implementation of IMemAllocator that uses a single contiguous
// buffer with lockable regions instead of a pool of fixed size buffers.
//
// Geraint Davies, January 2013

#include "stdafx.h"
#include "ContigBuffer.h"
//#include "logger.h"

ContigBuffer::ContigBuffer(int cSpace)
: m_pBuffer(NULL), 
  m_cSpace(0),
  m_cValid(0),
  m_bAbort(false),
  m_idxRead(0),
  m_evLocks(true)	// manual reset
{
	Allocate(cSpace);
}

ContigBuffer::~ContigBuffer()
{
	delete[] m_pBuffer;
}

HRESULT ContigBuffer::Allocate(int cSpace)
{
	if ((m_cValid > 0) || !m_locks.empty())
	{
		//LOG((TEXT("Allocate when not empty")));
		return VFW_E_WRONG_STATE;
	}
	delete[] m_pBuffer;
	m_pBuffer = 0;
	m_idxRead = 0;
	m_cSpace = cSpace;
	if (m_cSpace > 0)
	{
		m_pBuffer = new BYTE[m_cSpace];
	}
	//LOG((TEXT("Allocate %d bytes"), cSpace));
	return S_OK;
}

bool ContigBuffer::SearchLocks(int index, int indexEnd)
{
	for (lock_list_t::iterator it = m_locks.begin(); it != m_locks.end(); it++)
	{
		if ((it->first < indexEnd) && (it->second > index))
		{
			return true;
		}
	}
	return false;
}

BYTE* ContigBuffer::Append(const BYTE* p, int cBytes)
{
	for (;;)
	{
		{
			CAutoLock lock(&m_csLocks);
			if (m_bAbort)
			{
				return NULL;
			}
			m_evLocks.Reset();

			if (cBytes > m_cSpace)
			{
				//LOG((TEXT("Whole buffer too small for packet %d"), cBytes));
				return NULL;
			}
			int index = m_idxRead + m_cValid;
			if ((m_cSpace - index) < cBytes)
			{
				//LOG((TEXT("Allocator wrapping to start")));
				index = 0;
			}
			bool bLocked = true;
			if (!SearchLocks(index, index + cBytes))
			{
				bLocked = false;
				if ((index == 0) && (m_idxRead > 0))
				{
					if (m_cValid && SearchLocks(m_idxRead, m_idxRead + m_cValid))
					{
						bLocked = true;
						//LOG((TEXT("Waiting for locks on valid bytes %d"), m_cValid));
					}
					else
					{
						if (m_cValid)
						{
							MoveMemory(m_pBuffer, m_pBuffer + m_idxRead, m_cValid);
						}
						m_idxRead = 0;
					}
				}
			}
			if (!bLocked)
			{
				BYTE* pDest = m_pBuffer + m_idxRead + m_cValid;
				CopyMemory(pDest, p, cBytes);
				m_cValid += cBytes;
				return pDest;
			}
		}
		m_evLocks.Wait(2 * 1000);
		if (!m_evLocks.Check())
		{
			//LOG((TEXT("Timeout in Append")));
		}
	}
}

HRESULT ContigBuffer::Consume(const BYTE* p, int c)
{
	CAutoLock lock(&m_csLocks);
	if ((c >= m_cValid) && m_locks.empty())
	{
		//LOG((TEXT("Resetting idxRead in Consume")));
		m_idxRead = 0;
		m_cValid = 0;
	}
	else if ((p == (m_pBuffer + m_idxRead)) &&
			 (c <= m_cValid))
	{
		m_idxRead += c;
		m_cValid -= c;
	}
	else
	{
		//LOG((TEXT("Consume not contiguous")));
		return E_INVALIDARG;
	}
	return S_OK;
}

HRESULT ContigBuffer::Lock(const BYTE* p, int c)
{
	if ((p < m_pBuffer) || (p > (m_pBuffer + m_cSpace)) || (c > m_cSpace) || ((p+c) > (m_pBuffer + m_cSpace)))
	{
		//LOG((TEXT("Invalid lock region")));
		return E_INVALIDARG;
	}
	int index = int(p - m_pBuffer);
	int indexEnd = index + c;

	CAutoLock lock(&m_csLocks);
	m_locks.push_back(lock_t(index, indexEnd));

	return S_OK;
}

HRESULT ContigBuffer::Unlock(const BYTE* p, int c)
{
	if ((p < m_pBuffer) || (p > (m_pBuffer + m_cSpace)) || (c > m_cSpace) || ((p+c) > (m_pBuffer + m_cSpace)))
	{
		//LOG((TEXT("Invalid unlock region")));
		return E_INVALIDARG;
	}
	int index = int(p - m_pBuffer);
	int indexEnd = index + c;

	CAutoLock lock(&m_csLocks);
	for (lock_list_t::iterator it = m_locks.begin(); it != m_locks.end(); it++)
	{
		if ((it->first == index) && (it->second == indexEnd))
		{
			m_locks.erase(it);
			m_evLocks.Set();
#if 0
			if (!m_cValid && m_locks.empty())
			{
				LOG((TEXT("resetting idxRead on empty in Unlock")));
				m_idxRead = 0;
			}
#endif
			return S_OK;
		}
	}
	//LOG((TEXT("Lock not found in Unlock, len %d"), c));
	return E_INVALIDARG;
}

BYTE* ContigBuffer::AppendAndLock(const BYTE* p, int c)
{
	BYTE* pDest = Append(p, c);
	if (!pDest)
	{
		return pDest;
	}
	HRESULT hr = Lock(pDest, c);
	if (FAILED(hr))
	{
		return NULL;
	}
	Consume(pDest, c);
	return pDest;
}

BYTE* ContigBuffer::ValidRegion()
{
	return m_pBuffer + m_idxRead;
}

int ContigBuffer::ValidLength()
{
	return m_cValid;
}

void ContigBuffer::Abort()
{
	CAutoLock lock(&m_csLocks);

	m_bAbort = true;
	m_evLocks.Set();
}

void ContigBuffer::ResetAbort()
{
	CAutoLock lock(&m_csLocks);
	m_bAbort = false;
}
