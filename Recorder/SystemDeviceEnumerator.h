/**********************************************
 *  File: SystemDeviceEnumerator.h
 *  Desc: Enumerates system devices such as
 *        audio and video capture devices
 *  Author: Alberto A. Heredia (bertoso)
 *
 **********************************************/
#pragma once
#include "dshow.h"
#include <vector>
#include <algorithm>
#include <list>
class CSystemDeviceEnumerator
{
public:
	/*
	 * Name : 
	 * Desc : 
	 * In   : 
	 * Out  :
	 */
	CSystemDeviceEnumerator(void);
	/*
	 * Name : 
	 * Desc : 
	 * In   : 
	 * Out  :
	 */
	~CSystemDeviceEnumerator(void);
	/*
	 * Name : 
	 * Desc : 
	 * In   : 
	 * Out  :
	 */
	HRESULT EnumerateVideoCompressors(std::vector<CString>&);
	/*
	 * Name : 
	 * Desc : 
	 * In   : 
	 * Out  :
	 */
	HRESULT EnumerateVideoCaptureDevices(std::vector<CString>&);
	/*
	 * Name : 
	 * Desc : 
	 * In   : 
	 * Out  :
	 */
	HRESULT EnumerateAudioDevices(std::vector<CString>&);
	/*
	 * Name : 
	 * Desc : 
	 * In   : 
	 * Out  :
	 */
	HRESULT EnumerateAudioCompressors(std::vector<CString>&);
private:
	/*
	 * Name : 
	 * Desc : 
	 * In   : 
	 * Out  :
	 */
	HRESULT Enumerate(std::vector<CString>&, const IID clsidDeviceClass);
	/*
	 * Name : 
	 * Desc : 
	 * In   : 
	 * Out  :
	 */
	void CreateInstance();
	/*
	 * Name : 
	 * Desc : 
	 * In   : 
	 * Out  :
	 */
	void EnumerateDMOs(const IID clsidDMOCategory);

	ICreateDevEnum *m_pSysDevEnum;
	IMoniker *m_pMoniker;
	IEnumMoniker *m_pEnumCat;
	HRESULT m_Hr;
	GUID m_GUID;
	std::list<CString> m_listDMOs;
};

