#pragma once
#include "CamstudioFilter.h"
class CStreamingConfig
{
public:
	CStreamingConfig(void);
	~CStreamingConfig(void);
	HRESULT GetStreamingCaps(ACamstudioFilter *pFilter);
};

