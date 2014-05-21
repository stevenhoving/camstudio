// ParseBuffer.h
//
// Dynamically-growable buffer for parsing
//
// Copyright (c) GDCL 2002-2009

#pragma once

// used by parsers to manage accumulation memory
class ParseBuffer
{
public:
	ParseBuffer();
	~ParseBuffer();

	ParseBuffer(const ParseBuffer& r)
	: m_pData(NULL)
	{
		Assign(r);
	}
	const ParseBuffer& operator=(const ParseBuffer& r)
	{
		Done();
		Assign(r);
		return *this;
	}
	void Assign(const ParseBuffer& r)
	{
		m_cSpace = r.m_cSpace;
		m_cValid = r.m_cValid;
		m_pData = NULL;
		if (m_cSpace)
		{
			m_pData = new BYTE[m_cSpace];
		}
		if (m_cValid)
		{
			CopyMemory(m_pData, r.m_pData, m_cValid);
		}
	}

	bool Append(const BYTE* pData, long cBytes);
	bool FillFromFile(HANDLE hFile);
	void Consume(long cBytes);
	long Size()
	{
		return m_cValid;
	}
	const BYTE* Data()
	{
		if (m_cValid)
		{
			return m_pData;
		}
		return NULL;
	}
	void Done();
	
private:
	long m_cSpace;
	long m_cValid;
	BYTE* m_pData;
};

