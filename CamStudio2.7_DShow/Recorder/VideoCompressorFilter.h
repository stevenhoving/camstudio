#pragma once
#include "CamstudioFilter.h"
class CVideoCompressorFilter : public ACamstudioFilter
{
public:
	CVideoCompressorFilter(CString strName);
	~CVideoCompressorFilter(void);
};

