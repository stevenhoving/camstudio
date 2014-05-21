/**********************************************
 *  File: AudioCompressorFilter.h
 *  Desc: Represents an Audio Compressor filter
 *  Author: Alberto A. Heredia (bertoso)
 *
 **********************************************/
#pragma once
#include "CamstudioFilter.h"
class CAudioCompressorFilter : public ACamstudioFilter
{
public:
	/*
	 * Name : CAudioCompressorFilter
	 * Desc : Initializes audio codec
	 * In   : strName - Codec name
	 * Out  : None
	 */
	CAudioCompressorFilter(CString strName);

	/*
	 * Name : ~CAudioCompressorFilter
	 * Desc : Frees resources used by CAudioCompressorFilter
	 * In   : None
	 * Out  : None
	 */
	~CAudioCompressorFilter(void);
};

