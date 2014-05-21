/**********************************************
 *  File: AudioCompressorFormats.h
 *  Desc: Encapsulates all compression formats supported by an Audio Codec
 *  Author: Alberto A. Heredia (bertoso)
 *
 **********************************************/
#pragma once
#include "dshow.h"
#include <vector>
#include <algorithm>
class CAudioCompressorFormats
{
public:
	/*
	 * Name : CAudioCompressorFormats
	 * Desc : Initializes CAudioCompressorFormats
	 * In   : sAudComp - Audio codec name
	 * Out  : None
	 */
	CAudioCompressorFormats(CString sAudComp);
	/*
	 * Name : ~CAudioCompressorFormats
	 * Desc : Frees resources used by CAudioCompressorFormats
	 * In   : None
	 * Out  : None
	 */
	~CAudioCompressorFormats(void);
	/*
	 * Name : GetSupportedFormats
	 * Desc : Retrieves all compression formats supported by a codec
	 * In   : listFormats - container where compression formats are placed
	 * Out  : None
	 */
	HRESULT GetSupportedFormats(std::vector<WAVEFORMATEX*>& listFormats);
private:
	/*
	 * Name : FreeMediaType
	 * Desc : Frees resources used while loading compression formats
	 * In   : mt - Address of AM_MEDIA_TYPE (encapsulates each format loaded)
	 * Out  : None
	 */
	void FreeMediaType(AM_MEDIA_TYPE& mt);
	
	// Instance variables

	ICreateDevEnum *m_pSysDevEnum;
	IMoniker *m_pMoniker;
	IEnumMoniker *m_pEnumCat;
	CStringW m_sAudComp;

};

