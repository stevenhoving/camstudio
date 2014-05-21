#pragma once
#include "CamstudioFilter.h"
class CVideoCaptureDevice : public ACamstudioFilter
{
public:
	CVideoCaptureDevice(CString strName);
	~CVideoCaptureDevice(void);
};

