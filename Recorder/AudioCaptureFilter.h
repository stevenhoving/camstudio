/**********************************************
 *  File: AudioCaptureFilter.h
 *  Desc: Represents an Audio Capture device
 *  Author: Alberto A. Heredia (bertoso)
 *
 **********************************************/
#pragma once
#include "CamstudioFilter.h"
class CAudioCaptureFilter :
	public ACamstudioFilter
{
public:
	/*
	 * Name : CAudioCaptureFilter
	 * Desc : Initializes audio capture device
	 * In   : strName - Device name
	 * Out  : None
	 */
	CAudioCaptureFilter(CString strName);
	/*
	 * Name : ~CAudioCaptureFilter
	 * Desc : Frees resources used by CAudioCaptureFilter
	 * In   : None
	 * Out  : None
	 */
	~CAudioCaptureFilter(void);
};

