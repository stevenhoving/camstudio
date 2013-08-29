#pragma once
#include "dshow.h"
#include <list>
class AVICompressorFilter
{
public:
	AVICompressorFilter(void);
	~AVICompressorFilter(void);
	void EnumerateCompressors(std::list<CString>& compressors);
private:
	void CreateInstance();
	ICreateDevEnum *m_pSysDevEnum;
	IMoniker *m_pMoniker;
	IEnumMoniker *m_pEnumCat;
	HRESULT m_Hr;
};

