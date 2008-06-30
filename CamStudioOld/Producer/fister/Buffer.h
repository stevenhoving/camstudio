// Buffer.h: interface for the CBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUFFER_H__B4B9CF06_B948_49A5_9145_ECB3E318BF05__INCLUDED_)
#define AFX_BUFFER_H__B4B9CF06_B948_49A5_9145_ECB3E318BF05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "errorprint.h"

class CBuffer  
{
public:
	void Erase();
	CBuffer(DWORD size, bool AutoDelete = true);
	CBuffer(void* buffer, DWORD length);
	virtual ~CBuffer();

	union PointerUnion{
		BYTE*		b;
		char*		c;
		WORD*		w;
		short int*	s;
		DWORD*		d;
		int*		i;
		float*		f;
		double*		r;	// real
		void*		v;
	}ptr;

	int	ByteLen;	// length in bytes

private:
	bool	m_bAutoDelete;
};

#endif // !defined(AFX_BUFFER_H__B4B9CF06_B948_49A5_9145_ECB3E318BF05__INCLUDED_)
