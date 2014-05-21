// ParseBuffer.h
//
// Dynamically-growable buffer for parsing
//
// Copyright (c) GDCL 2002-2009

#include "StdAfx.h"
#include "ParseBuffer.h"



// parse buffer utility class -----------------------

ParseBuffer::ParseBuffer()
: m_pData(NULL),
  m_cSpace(0),
  m_cValid(0)
{}

ParseBuffer::~ParseBuffer()
{
	delete[] m_pData;
}

bool
ParseBuffer::FillFromFile(HANDLE hFile)
{
	if (m_cSpace == 0)
	{
		m_cSpace = 64 * 1024;
		m_pData = new BYTE[m_cSpace];
	}
	DWORD cActual = 0;
	ReadFile(hFile, m_pData + m_cValid, m_cSpace - m_cValid, &cActual, NULL);
	m_cValid += cActual;
	return (cActual > 0) ? true : false;
}

bool 
ParseBuffer::Append(const BYTE* pData, long cBytes)
{
	if (m_cSpace < (m_cValid + cBytes))
	{
		int cNew = m_cSpace;
		while (cNew < (m_cValid + cBytes))
		{
			cNew += 64 * 1024;
		}
		BYTE* pNew = new BYTE[cNew];
		if (pNew == NULL)
		{
			return false;
		}
		if (m_cValid)
		{
			CopyMemory(pNew, m_pData, m_cValid);
			delete[] m_pData;
		}
		m_pData = pNew;
		m_cSpace = cNew;
	}
	CopyMemory(&m_pData[m_cValid], pData, cBytes);
	m_cValid += cBytes;
	return true;
}

void 
ParseBuffer::Consume(long cBytes)
{
	m_cValid -= cBytes;
	if (m_cValid > 0)
	{
		MoveMemory(m_pData, &m_pData[cBytes], m_cValid);
	}
	else
	{
		m_cValid = 0;
	}
}
void 
ParseBuffer::Done()
{
	delete[] m_pData;
	m_pData = NULL;
	m_cValid = 0;
	m_cSpace = 0;
}
