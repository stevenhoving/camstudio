/**********************************************
 *  File: AmVfwCompressDialgs.h
 *  Desc: Displays a dialog box provided by a Video for Windows (VFW) codec
 *  Author: Alberto A. Heredia (bertoso)
 *
 **********************************************/
#pragma once
#include "dshow.h"
#include "VideoCompressorFilter.h"
#include "ScreenCaptureFilter.h"

class CAMVfwCompressDialogs
{
public:

	/*
	 * Name : CAMVfwCompressDialogs
	 * Desc : Constructor
	 * In   : hwnd - Handle of the owner window
	 * Out  :
	 */
	CAMVfwCompressDialogs(HWND hWnd);
	/*
	 * Name : ~CAMVfwCompressDialogs
	 * Desc : Destructor
	 * In   : None
	 */
	~CAMVfwCompressDialogs(void);
	/* Name : CreateInstance
	 * Desc : Creates instance of filter that supports IAMVfwCompressDialog
	 * Input: HRESULT
	 */
	HRESULT CreateInstance(const IID clsid);
	/*
	 * Name : HasAboutDialog
	 * Desc : Queries if a filter has About dialog box
	 * In   : pFilter - Pointer to ACamstudioFilter
	 * Out  : true/false
	 */
	bool HasAboutDialog(const ACamstudioFilter *pFilter);
	/*
	 * Name : HasAboutDialog
	 * Desc : Queries if a filter has Configure dialog box
	 * In   : pFilter - Pointer to ACamstudioFilter
	 * Out  : true/false
	 */
	bool HasConfigDialog(const ACamstudioFilter *pFilter);
	/*
	 * Name : DisplayAboutDialog
	 * Desc : Displays a filter's About dialog box
	 * In   : pFilter - Pointer to ACamstudioFilter
	 * Out  : None
	 */
	void DisplayAboutDialog(const ACamstudioFilter *pFilter);
	/*
	 * Name : DisplayConfigDialog
	 * Desc : Pointer to ACamstudioFilter
	 * In   : Pointer to ACamstudioFilter
	 * Out  : None
	 */
	void DisplayConfigDialog(const ACamstudioFilter *pFilter);
private:
	/*
	 * Name : GetPropertyWindow
	 * Desc : Perform the actual task of displaying/querying if a filter has About/Configure dialog box
	 * In   : iDialog - enum
	 *		  pFilter - Pointer to ACamstudioFilter
	 * Out  :
	 */
	HRESULT GetPropertyWindow(VfwCompressDialogs iDialog, const ACamstudioFilter *pFilter);
private:
	ICreateDevEnum *m_pSysDevEnum;
	IMoniker *m_pMoniker;
	IEnumMoniker *m_pEnum;
	HWND m_hWnd;
	GUID m_GUID;
};

