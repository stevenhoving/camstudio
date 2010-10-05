/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "VCM.h"

ICINFO * pCompressorInfo = 0;
int num_compressor = 0;

CString GetCodecDescription(FOURCC fccHandler)
{
	//ICINFO compinfo;
	//::ZeroMemory(&compinfo, sizeof(compinfo));
	//compinfo.dwSize = sizeof(ICINFO);
	//HIC hic = ICOpen(ICTYPE_VIDEO, fccHandler, ICMODE_QUERY);
	//if (hic) {
	//	ICGetInfo(hic, &compinfo, sizeof(ICINFO));
	//	ICClose(hic);
	//}
	CHIC Hic;
	if (Hic.Open(ICTYPE_VIDEO, fccHandler, ICMODE_QUERY)) {
		ICINFO sICInfo;
		LRESULT lResult = Hic.GetInfo(sICInfo, sizeof(ICINFO));
		Hic.Close();
		return (0L < lResult)
			? CString(sICInfo.szDescription)
			: CString(_T(""));
	}

	return CString("");
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CHIC::CHIC()
: m_hIC(0)
, m_dwfccType(0UL)
, m_dwfccHandler(0UL)
, m_pState(0)
, m_ulStateSize(0UL)
{
	::ZeroMemory(&m_icInfo, sizeof(m_icInfo));
	m_icInfo.dwSize = sizeof(ICINFO);
}

CHIC::~CHIC()
{
	if (isOpen()) {
		VERIFY(ICERR_OK == Close());
		VERIFY(!isOpen());
	}
	if (m_pState) {
		ASSERT(0 < m_ulStateSize);
		delete [] m_pState;
		m_ulStateSize = 0L;
	}
}

bool CHIC::getState()
{
	bool bResult = isOpen();
	if (!bResult) {
		TRACE("CHIC::getState: %s Not open\n", bResult ? "OK" : "FAIL");
		return bResult;
	}
	try
	{
		if (m_pState) {
			ASSERT(0 < m_ulStateSize);
			delete [] m_pState;
			m_ulStateSize = 0L;
		}
		ASSERT(0L == m_ulStateSize);
		m_ulStateSize = GetStateSize();
		bResult = (0 < m_ulStateSize);
		if (!bResult) {
			TRACE("CHIC::getState: %s bad size\n", bResult ? "OK" : "FAIL");
			return bResult;
		}

		// TODO, Possible memory leak, where is the delete operation of the new below done although there is a delete in catch
		m_pState = new char[m_ulStateSize];
		LRESULT lResult = ::ICGetState(m_hIC, m_pState, m_ulStateSize);
		// bResult = (lResult == m_ulStateSize );  ==> C4389 Warning, type mismatch
		bResult = ( lResult - m_ulStateSize == 0 );  // Save 
		if (!bResult)
			throw "ICGetState failed";
	}
	catch(...)
	{
		bResult = false;
		if (m_pState) {
			ASSERT(0 < m_ulStateSize);
			delete [] m_pState, m_pState = 0;
			m_ulStateSize = 0L;
		}
		ASSERT(0L == m_ulStateSize);
	}
	TRACE("CHIC::getState: %s\n", bResult ? "OK" : "FAIL");
	return bResult;
}

// queries a video compression driver to determine if it has an About dialog box.
// Returns ICERR_OK if the driver supports this message or ICERR_UNSUPPORTED otherwise.
// n.b Actually returns unsupported state ICERR_UNSUPPORTED; otherwise supported.
DWORD CHIC::QueryAbout()
{
	DWORD dwResult = ICQueryAbout(m_hIC);
	//TRACE("CHIC::QueryAbout : %s About\n", (ICERR_UNSUPPORTED == dwResult) ? "No" : "Have");
	TRACE("CHIC::QueryAbout : %s About\n", (dwResult) ? "Have" : "No");
	return dwResult;
}

DWORD CHIC::About(HWND hWnd)
{
	DWORD dwResult = QueryAbout();
	//return (ICERR_UNSUPPORTED == dwResult) ? dwResult : ICAbout(m_hIC, hWnd);
	return (dwResult) ? ICAbout(m_hIC, hWnd) : dwResult;
}

// closes a compressor or decompressor.
// Returns ICERR_OK if successful or an error otherwise;
LRESULT CHIC::Close()
{
	LRESULT lResult = ::ICClose(m_hIC);
	if (ICERR_OK == lResult) {
		m_hIC = 0;
	}
	TRACE("CHIC::Close: %s\n", (ICERR_OK == lResult) ? "OK" : "FAIL");
	return lResult;
}

// retrieves information about specific installed compressors or enumerates the
// installed compressors.
// Returns TRUE if successful or an error otherwise.
BOOL CHIC::Info(DWORD fccType, DWORD fccHandler, ICINFO& icinfo)
{
	BOOL bResult = ::ICInfo(fccType, fccHandler, &icinfo);
	if (bResult) {
		// n.b TRACE won't display szDriver properly; ???
		TRACE("CGIC::Info:\nDescription: %s\nName: %s\nDriver: %s\n", icinfo.szDescription, icinfo.szName, (LPCTSTR)CString(icinfo.szDriver));
	}
	return bResult;
}

// opens a compressor or decompressor.
// Returns a handle to a compressor or decompressor if successful or zero otherwise.
HIC CHIC::Open(FOURCC dwfccType, FOURCC dwfccHandler, UINT uMode)
{
	ASSERT(!isOpen());
	m_hIC = ::ICOpen(dwfccType, dwfccHandler, uMode);
	if (isOpen()) {
		m_dwfccType = dwfccType;
		m_dwfccHandler = dwfccHandler;
		TRACEFOURCC(m_dwfccType, TEXT("Type"));
		TRACEFOURCC(m_dwfccHandler, TEXT("Handler"));
		VERIFY(Info(m_dwfccType, m_dwfccHandler, m_icInfo));
		VERIFY(getState());
	}
	TRACE("CHIC::Open: %s\n", isOpen() ? "OK" : "FAIL");
	return m_hIC;
}
// opens a compressor or decompressor defined as a function.
// Returns a handle to a compressor or decompressor if successful or zero otherwise.
HIC CHIC::OpenFunction(FOURCC dwfccType, FOURCC dwfccHandler, UINT wMode, FARPROC lpfnHandler)
{
	ASSERT(!isOpen());
	m_hIC = ::ICOpenFunction(dwfccType, dwfccHandler, wMode, lpfnHandler);
	if (isOpen()) {
		m_dwfccType = dwfccType;
		m_dwfccHandler = dwfccHandler;
		TRACEFOURCC(m_dwfccType, TEXT("Type"));
		TRACEFOURCC(m_dwfccHandler, TEXT("Handler"));
		VERIFY(Info(m_dwfccType, m_dwfccHandler, m_icInfo));
		VERIFY(getState());
	}
	TRACE("CHIC::OpenFunction: %s\n", isOpen() ? "OK" : "FAIL");
	return m_hIC;
}

// opens a decompressor that is compatible with the specified formats.
// Returns a handle of a decompressor if successful or zero otherwise.
HIC CHIC::DecompressOpen(FOURCC dwfccType, FOURCC dwfccHandler, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
	ASSERT(!isOpen());
	m_hIC = ICDecompressOpen(dwfccType, dwfccHandler, lpbiIn, lpbiOut);
	if (isOpen()) {
		m_dwfccType = dwfccType;
		m_dwfccHandler = dwfccHandler;
		TRACEFOURCC(m_dwfccType, TEXT("Type"));
		TRACEFOURCC(m_dwfccHandler, TEXT("Handler"));
		VERIFY(Info(m_dwfccType, m_dwfccHandler, m_icInfo));
		VERIFY(getState());
	}
	TRACE("CHIC::DecompressOpen: %s\n", isOpen() ? "OK" : "FAIL");
	return m_hIC;
}

// opens a driver that can draw images with the specified format.
// Returns a handle of a driver if successful or zero otherwise.
HIC CHIC::DrawOpen(FOURCC dwfccType, FOURCC dwfccHandler, LPBITMAPINFOHEADER lpbiIn)
{
	ASSERT(!isOpen());
	m_hIC = ICDrawOpen(dwfccType, dwfccHandler, lpbiIn);
	if (isOpen()) {
		m_dwfccType = dwfccType;
		m_dwfccHandler = dwfccHandler;
		TRACEFOURCC(m_dwfccType, TEXT("Type"));
		TRACEFOURCC(m_dwfccHandler, TEXT("Handler"));
		VERIFY(Info(m_dwfccType, m_dwfccHandler, m_icInfo));
		VERIFY(getState());
	}
	TRACE("CHIC::DrawOpen: %s\n", isOpen() ? "OK" : "FAIL");
	return m_hIC;
}
