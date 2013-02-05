/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <mmsystem.h>
#include <MMREG.H>
#include <MSAcm.h>

extern void SuggestCompressFormat();
extern void AttemptCompressFormat();
extern void AttemptRecordingFormat();
extern void SuggestRecordingFormat();

/////////////////////////////////////////////////////////////////////////////
class CACM
{
public:
	CACM();
	virtual ~CACM();

	//returns the version number of the ACM.
	// The version number is returned as a hexadecimal number of the form 0xAABBCCCC,
	// where
	// AA is the major version number,
	// BB is the minor version number,
	// and CCCC is the build number.
	DWORD Version()
	{
		return ::acmGetVersion();
	}
	//returns various metrics for the ACM or related ACM objects.
	MMRESULT Metrics(HACMOBJ hao, UINT uMetric, LPVOID pMetric)
	{
		return ::acmMetrics(hao, uMetric, pMetric);
	}
	MMRESULT MaxFormatSize(DWORD& rdwMetric)
	{
		return Metrics(0, ACM_METRIC_MAX_SIZE_FORMAT, &rdwMetric);
	}
};

class CACMFormat :
	public CACM
{
public:
	CACMFormat();
	~CACMFormat();
	
	MMRESULT FormatEnum(HACMDRIVER had, LPACMFORMATDETAILS pafd, ACMFORMATENUMCB fnCallback, DWORD_PTR dwInstance, DWORD fdwEnum)
	{
		return ::acmFormatEnum(had, pafd, fnCallback, dwInstance, fdwEnum);
	}
	MMRESULT FormatEnum(HACMDRIVER had, LPACMFORMATDETAILS pafd, DWORD_PTR dwInstance, DWORD fdwEnum)
	{
		return ::acmFormatEnum(had, pafd, FormatEnumCallback, dwInstance, fdwEnum);
	}
private:
	// callback with object access 
	BOOL FormatEnumCallback(HACMDRIVERID /*hadid*/, LPACMFORMATDETAILS /*pafd*/, DWORD /*fdwSupport*/)
	{
		return TRUE;
	}
	// callback without object access.
	// (note calling convention)
	static BOOL CALLBACK FormatEnumCallback(HACMDRIVERID hadid, LPACMFORMATDETAILS pafd, DWORD_PTR dwInstance, DWORD fdwSupport)
	{
		CACMFormat * pxACMFormat = reinterpret_cast<CACMFormat *>(dwInstance);
		BOOL bResult = (pxACMFormat)
			? pxACMFormat->FormatEnumCallback(hadid, pafd, fdwSupport)
			: FALSE;
		return bResult;
	}
};

