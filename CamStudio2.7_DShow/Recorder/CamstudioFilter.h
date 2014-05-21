/**********************************************
 *  File: CamstudioFilter.h
 *  Desc: Base class for all directshow filters loaded by Camstudio
 *  Author: Alberto A. Heredia (bertoso)
 *
 **********************************************/
#pragma once
#include "dshow.h"
#include "ks.h"
class ACamstudioFilter
{
public:
	/*
	 * Name : ACamstudioFilter
	 * Desc : Initialize instance of this class
	 * In   : None
	 * Out  : None
	 */
	ACamstudioFilter();
	/*
	 * Name : ~ACamstudioFilter
	 * Desc : Frees resources used by ACamstudioFilter
	 * In   : None
	 * Out  : None
	 */
	virtual ~ACamstudioFilter(void);
	/*
	 * Name : GetFilter
	 * Desc : Returns the pointer of the underlying filter represented by this class
	 * In   : None
	 * Out  : IBaseFilter - Pointer of the filter
	 */
	IBaseFilter *GetFilter()const;
	/*
	 * Name : FilterName
	 * Desc : Returns the friendly name of the underlying filter
	 * In   : None
	 * Out  : CString - Name of the filter
	 */
	const CString& FilterName()const;
	/*
	 * Name : FilterNameW
	 * Desc : Returns the wide version friendly name of the underlying filter
	 * In   : None
	 * Out  : CStringW - Name of the filter
	 */
	const CStringW& FilterNameW()const;
protected:
	/*
	 * Name : LoadFilter
	 * Desc : Performs the actual loading of the filter (audio/video capture device or audio/video codec)
	 * In   : clsid	  - GUID of the filter
	 *		  strName - Friendly name of the filter
	 * Out  :
	 */
	HRESULT LoadFilter(const GUID clsid, CString strName);
private:
	IBaseFilter *m_pFilter;
	CString m_sName;
	CStringW m_sNameW;
};

