#include "StdAfx.h"
#include "VCM.h"

ICINFO *g_compressor_info = nullptr;
int g_num_compressor = 0;

CString GetCodecDescription(FOURCC fccHandler)
{
    // ICINFO compinfo;
    //::ZeroMemory(&compinfo, sizeof(compinfo));
    // compinfo.dwSize = sizeof(ICINFO);
    // HIC hic = ICOpen(ICTYPE_VIDEO, fccHandler, ICMODE_QUERY);
    // if (hic) {
    //    ICGetInfo(hic, &compinfo, sizeof(ICINFO));
    //    ICClose(hic);
    //}
    CHIC Hic;
    if (Hic.Open(ICTYPE_VIDEO, fccHandler, ICMODE_QUERY))
    {
        ICINFO sICInfo;
        LRESULT lResult = Hic.GetInfo(sICInfo, sizeof(ICINFO));
        Hic.Close();
        return (0L < lResult) ? CString(sICInfo.szDescription) : CString(_T(""));
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
    if (isOpen())
    {
        VERIFY(ICERR_OK == Close());
        VERIFY(!isOpen());
    }
    if (m_pState)
    {
        ASSERT(0 < m_ulStateSize);
        delete[] m_pState;
        m_ulStateSize = 0L;
    }
}

CHIC::operator HIC() const
{
    return m_hIC;
}

FOURCC CHIC::Handler() const
{
    return m_dwfccHandler;
}

#ifdef _DEBUG


void CHIC::TRACEINFO(const ICINFO &cInfo)
{
    CString strSupport("Flags:\n");
    if (cInfo.dwFlags & VIDCF_QUALITY)
        strSupport += TEXT("VIDCF_QUALITY : supports quality\n");
    if (cInfo.dwFlags & VIDCF_CRUNCH)
        strSupport += TEXT("VIDCF_CRUNCH : supports crunching to a frame size\n");
    if (cInfo.dwFlags & VIDCF_TEMPORAL)
        strSupport += TEXT("VIDCF_TEMPORAL : supports inter-frame compress\n");
    if (cInfo.dwFlags & VIDCF_COMPRESSFRAMES)
        strSupport += TEXT("VIDCF_COMPRESSFRAMES : wants the compress all frames message\n");
    if (cInfo.dwFlags & VIDCF_DRAW)
        strSupport += TEXT("VIDCF_DRAW : supports drawing\n");
    if (cInfo.dwFlags & VIDCF_FASTTEMPORALC)
        strSupport += TEXT("VIDCF_FASTTEMPORALC : does not need prev frame on compress\n");
    if (cInfo.dwFlags & VIDCF_FASTTEMPORALD)
        strSupport += TEXT("VIDCF_FASTTEMPORALD : does not need prev frame on decompress\n");
    TRACE(TEXT("%s"), (LPCTSTR)strSupport);
}

void CHIC::TRACEFOURCC(FOURCC dwfcc, const LPCTSTR pName /*= 0*/)
{
    CString csFourCC;
    csFourCC += (BYTE)(0xFF & dwfcc);
    csFourCC += (BYTE)(0xFF & (dwfcc >> 8));
    csFourCC += (BYTE)(0xFF & (dwfcc >> 16));
    csFourCC += (BYTE)(0xFF & (dwfcc >> 24));
    TRACE(TEXT("FourCC %s: %s\n"), pName ? pName : TEXT(""), (LPCTSTR)csFourCC);
}

#else

void CHIC::TRACEINFO(const ICINFO & /*cInfo*/)
{
}


void CHIC::TRACEFOURCC(FOURCC /*dwfcc*/, const LPCTSTR /*pName*/ /*= 0*/)
{
}
#endif

bool CHIC::getState()
{
    bool bResult = isOpen();
    if (!bResult)
    {
        TRACE("CHIC::getState: %s Not open\n", bResult ? "OK" : "FAIL");
        return bResult;
    }

    try
    {
        if (m_pState)
        {
            ASSERT(m_ulStateSize > 0);
            delete[] m_pState;
            m_ulStateSize = 0L;
        }
        ASSERT(m_ulStateSize == 0L);
        m_ulStateSize = GetStateSize();
        bResult = m_ulStateSize > 0;
        if (!bResult)
        {
            TRACE("CHIC::getState: %s bad size\n", bResult ? "OK" : "FAIL");
            return bResult;
        }

        // TODO, Possible memory leak, where is the delete operation of the new below done although there is a delete in
        // catch
        m_pState = new char[m_ulStateSize];
        LRESULT lResult = ::ICGetState(m_hIC, m_pState, m_ulStateSize);
        // bResult = (lResult == m_ulStateSize );  ==> C4389 Warning, type mismatch
        bResult = (lResult - m_ulStateSize == 0); // Save
        if (!bResult)
            throw "ICGetState failed";
    }
    catch (...)
    {
        bResult = false;
        if (m_pState)
        {
            ASSERT(0 < m_ulStateSize);
            delete[] m_pState, m_pState = 0;
            m_ulStateSize = 0L;
        }
        ASSERT(0L == m_ulStateSize);
    }
    TRACE("CHIC::getState: %s\n", bResult ? "OK" : "FAIL");
    return bResult;
}

bool CHIC::isOpen() const
{
    return (0 != m_hIC);
}

bool CHIC::isType(FOURCC dwfccType) const
{
    return dwfccType == m_dwfccType;
}

bool CHIC::isHandler(FOURCC dwfccHandler) const
{
    return dwfccHandler == m_dwfccHandler;
}

LRESULT CHIC::CompressBegin(LPBITMAPINFO lpbiInput, LPBITMAPINFO lpbiOutput)
{
    return ICCompressBegin(m_hIC, lpbiInput, lpbiOutput);
}

LRESULT CHIC::CompressEnd()
{
    return ICCompressEnd(m_hIC);
}

LRESULT CHIC::CompressGetFormat(LPBITMAPINFO lpbiInput, LPBITMAPINFO lpbiOutput)
{
    return ICCompressGetFormat(m_hIC, lpbiInput, lpbiOutput);
}

LRESULT CHIC::CompressGetFormatSize(LPBITMAPINFO lpbiInput)
{
    return ICCompressGetFormatSize(m_hIC, lpbiInput);
}

LRESULT CHIC::CompressGetSize(LPBITMAPINFO lpbiInput, LPBITMAPINFO lpbiOutput)
{
    return ICCompressGetSize(m_hIC, lpbiInput, lpbiOutput);
}

// queries a video compression driver to determine if it has an About dialog box.
// Returns ICERR_OK if the driver supports this message or ICERR_UNSUPPORTED otherwise.
// n.b Actually returns unsupported state ICERR_UNSUPPORTED; otherwise supported.
LRESULT CHIC::QueryAbout()
{
    LRESULT dwResult = ICQueryAbout(m_hIC);
    // TRACE("CHIC::QueryAbout : %s About\n", (ICERR_UNSUPPORTED == dwResult) ? "No" : "Have");
    TRACE("CHIC::QueryAbout : %s About\n", (dwResult) ? "Have" : "No");
    return dwResult;
}

LRESULT CHIC::About(HWND hWnd)
{
    LRESULT dwResult = QueryAbout();
    // return (ICERR_UNSUPPORTED == dwResult) ? dwResult : ICAbout(m_hIC, hWnd);
    return (dwResult) ? ICAbout(m_hIC, hWnd) : dwResult;
}

LRESULT CHIC::QueryConfigure()
{
    return ICQueryConfigure(m_hIC);
}

LRESULT CHIC::Configure(HWND hWnd)
{
    LRESULT dwResult = QueryConfigure();
    // return (ICERR_OK == dwResult) ? ICConfigure(m_hIC, hWnd) : dwResult;
    return (dwResult) ? ICConfigure(m_hIC, hWnd) : dwResult;
}

DWORD CHIC::GetStateSize()
{
    return ICGetStateSize(m_hIC);
}

LPVOID CHIC::GetState()
{
    VERIFY(getState());
    return m_pState;
}

LRESULT CHIC::GetState(LPVOID pv, DWORD_PTR cb)
{
    VERIFY(getState());
    return ICGetState(m_hIC, pv, cb);
}

LRESULT CHIC::SetState(LPVOID pv, DWORD_PTR cb)
{
    return ICSetState(m_hIC, pv, cb);
}

// closes a compressor or decompressor.
// Returns ICERR_OK if successful or an error otherwise;
LRESULT CHIC::Close()
{
    LRESULT lResult = ::ICClose(m_hIC);
    if (ICERR_OK == lResult)
    {
        m_hIC = 0;
    }
    TRACE("CHIC::Close: %s\n", (ICERR_OK == lResult) ? "OK" : "FAIL");
    return lResult;
}

DWORD CHIC::Compress(DWORD dwFlags, LPBITMAPINFOHEADER lpbiOutput, LPVOID lpData, LPBITMAPINFOHEADER lpbiInput,
                     LPVOID lpBits, LPDWORD lpckid, LPDWORD lpdwFlags, LONG lFrameNum, DWORD dwFrameSize,
                     DWORD dwQuality, LPBITMAPINFOHEADER lpbiPrev, LPVOID lpPrev)
{
    return ::ICCompress(m_hIC, dwFlags, lpbiOutput, lpData, lpbiInput, lpBits, lpckid, lpdwFlags, lFrameNum,
                        dwFrameSize, dwQuality, lpbiPrev, lpPrev);
}

LRESULT CHIC::DrawRealize(HDC hdc, BOOL fBackground)
{
    return ICDrawRealize(m_hIC, hdc, fBackground);
}

LRESULT CHIC::DrawRenderBuffer()
{
    return ICDrawRenderBuffer(m_hIC);
}

LRESULT CHIC::DrawSetTime(LPVOID lpTime)
{
    return ICDrawSetTime(m_hIC, lpTime);
}

LRESULT CHIC::DrawStart()
{
    return ICDrawStart(m_hIC);
}

void CHIC::DrawStartPlay(DWORD lFrom, DWORD lTo)
{
    ICDrawStartPlay(m_hIC, lFrom, lTo);
}

LRESULT CHIC::DrawStop()
{
    return ICDrawStop(m_hIC);
}

void CHIC::DrawStopPlay()
{
    ICDrawStopPlay(m_hIC);
}

LRESULT CHIC::DrawWindow(LPRECT prc)
{
    return ICDrawWindow(m_hIC, prc);
}

LRESULT CHIC::GetBuffersWanted(LPVOID lpdwBuffers)
{
    return ICGetBuffersWanted(m_hIC, lpdwBuffers);
}

LRESULT CHIC::GetDefaultKeyFrameRate()
{
    return ICGetDefaultKeyFrameRate(m_hIC);
}

LRESULT CHIC::GetDefaultQuality()
{
    return ICGetDefaultQuality(m_hIC);
}

LRESULT CHIC::DrawSuggestFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut, int dxSrc, int dySrc, int dxDst,
                              int dyDst, HIC hicDecompressor)
{
    return ::ICDrawSuggestFormat(m_hIC, lpbiIn, lpbiOut, dxSrc, dySrc, dxDst, dyDst, hicDecompressor);
}

HIC CHIC::GetDisplayFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut, int BitDepth, int dx, int dy)
{
    return ::ICGetDisplayFormat(m_hIC, lpbiIn, lpbiOut, BitDepth, dx, dy);
}

BOOL CHIC::GetInfo(FOURCC fccType, FOURCC fccHandler, ICINFO &ricinfo)
{
    return ::ICInfo(fccType, fccHandler, &ricinfo);
}

LRESULT CHIC::GetInfo(ICINFO &icinfo, DWORD cb)
{
    return ::ICGetInfo(m_hIC, &icinfo, cb);
}

HANDLE CHIC::ImageCompress(UINT uiFlags, LPBITMAPINFO lpbiIn, LPVOID lpBits, LPBITMAPINFO lpbiOut, LONG lQuality,
                           LONG *plSize)
{
    return ::ICImageCompress(m_hIC, uiFlags, lpbiIn, lpBits, lpbiOut, lQuality, plSize);
}

HANDLE CHIC::ImageDecompress(UINT uiFlags, LPBITMAPINFO lpbiIn, LPVOID lpBits, LPBITMAPINFO lpbiOut)
{
    return ::ICImageDecompress(m_hIC, uiFlags, lpbiIn, lpBits, lpbiOut);
}

// retrieves information about specific installed compressors or enumerates the
// installed compressors.
// Returns TRUE if successful or an error otherwise.
BOOL CHIC::Info(DWORD fccType, DWORD fccHandler, ICINFO &icinfo)
{
    BOOL bResult = ::ICInfo(fccType, fccHandler, &icinfo);
    if (bResult)
    {
        // n.b TRACE won't display szDriver properly; ???
        TRACE("CGIC::Info:\nDescription: %s\nName: %s\nDriver: %s\n", icinfo.szDescription, icinfo.szName,
              (LPCTSTR)CString(icinfo.szDriver));
    }
    return bResult;
}

BOOL CHIC::Install(DWORD fccType, DWORD fccHandler, LPARAM lParam, LPSTR szDesc, UINT wFlags)
{
    return ::ICInstall(fccType, fccHandler, lParam, szDesc, wFlags);
}

HIC CHIC::Locate(DWORD fccType, DWORD fccHandler, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut, WORD wFlags)
{
    return ::ICLocate(fccType, fccHandler, lpbiIn, lpbiOut, wFlags);
}

// opens a compressor or decompressor.
// Returns a handle to a compressor or decompressor if successful or zero otherwise.
HIC CHIC::Open(FOURCC dwfccType, FOURCC dwfccHandler, UINT uMode)
{
    ASSERT(!isOpen());
    m_hIC = ::ICOpen(dwfccType, dwfccHandler, uMode);
    if (isOpen())
    {
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
    if (isOpen())
    {
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

BOOL CHIC::Remove(DWORD fccType, DWORD fccHandler, UINT wFlags)
{
    return ::ICRemove(fccType, fccHandler, wFlags);
}

LRESULT CHIC::SendMessage(UINT wMsg, DWORD_PTR dw1, DWORD_PTR dw2)
{
    return ::ICSendMessage(m_hIC, wMsg, dw1, dw2);
}

LPVOID CHIC::SeqCompressFrame(PCOMPVARS pc, UINT uiFlags, LPVOID lpBits, BOOL *pfKey, LONG *plSize)
{
    return ::ICSeqCompressFrame(pc, uiFlags, lpBits, pfKey, plSize);
}

void CHIC::SeqCompressFrameEnd(PCOMPVARS pc)
{
    ::ICSeqCompressFrameEnd(pc);
}

BOOL CHIC::SeqCompressFrameStart(PCOMPVARS pc, LPBITMAPINFO lpbiIn)
{
    return ::ICSeqCompressFrameStart(pc, lpbiIn);
}

LRESULT CHIC::SetStatusProc(DWORD dwFlags, LONG lParam, LONG(CALLBACK *fpfnStatus)(LPARAM, UINT, LONG))
{
    return ::ICSetStatusProc(m_hIC, dwFlags, lParam, fpfnStatus);
}

BOOL CHIC::CompressorChoose(HWND hwnd, UINT uiFlags, LPVOID pvIn, LPVOID lpData, PCOMPVARS pc, LPSTR lpszTitle)
{
    return ::ICCompressorChoose(hwnd, uiFlags, pvIn, lpData, pc, lpszTitle);
}

void CHIC::CompressorFree(PCOMPVARS pc)
{
    ::ICCompressorFree(pc);
}

LRESULT CHIC::CompressQuery(LPBITMAPINFO lpbiInput, LPBITMAPINFO lpbiOutput /*= 0*/)
{
    LRESULT dwResult = ICCompressQuery(m_hIC, lpbiInput, lpbiOutput);
    return dwResult;
}

LRESULT CHIC::Decompress(DWORD dwFlags, LPBITMAPINFOHEADER lpbiFormat, LPVOID lpData, LPBITMAPINFOHEADER lpbi,
                       LPVOID lpBits)
{
    return ::ICDecompress(m_hIC, dwFlags, lpbiFormat, lpData, lpbi, lpBits);
}

LRESULT CHIC::DecompressBegin(LPBITMAPINFO lpbiInput, LPBITMAPINFO lpbiOutput)
{
    return ICDecompressBegin(m_hIC, lpbiInput, lpbiOutput);
}

LRESULT CHIC::DecompressEnd()
{
    return ICDecompressEnd(m_hIC);
}

LRESULT CHIC::DecompressExBegin(DWORD dwFlags, LPBITMAPINFOHEADER lpbiSrc, LPVOID lpSrc, int xSrc, int ySrc, int dxSrc,
                                int dySrc, LPBITMAPINFOHEADER lpbiDst, LPVOID lpDst, int xDst, int yDst, int dxDst,
                                int dyDst)
{
    return ::ICDecompressExBegin(m_hIC, dwFlags, lpbiSrc, lpSrc, xSrc, ySrc, dxSrc, dySrc, lpbiDst, lpDst, xDst, yDst,
                                 dxDst, dyDst);
}

LRESULT CHIC::DecompressExEnd()
{
    return ICDecompressExEnd(m_hIC);
}

LRESULT CHIC::DecompressEx(DWORD dwFlags, LPBITMAPINFOHEADER lpbiSrc, LPVOID lpSrc, int xSrc, int ySrc, int dxSrc,
                           int dySrc, LPBITMAPINFOHEADER lpbiDst, LPVOID lpDst, int xDst, int yDst, int dxDst,
                           int dyDst)
{
    return ::ICDecompressEx(m_hIC, dwFlags, lpbiSrc, lpSrc, xSrc, ySrc, dxSrc, dySrc, lpbiDst, lpDst, xDst, yDst, dxDst,
                            dyDst);
}

LRESULT CHIC::DecompressGetFormat(LPBITMAPINFO lpbiInput, LPBITMAPINFO lpbiOutput)
{
    return ICDecompressGetFormat(m_hIC, lpbiInput, lpbiOutput);
}

LRESULT CHIC::DecompressGetFormatSize(LPBITMAPINFO lpbiInput)
{
    return ICDecompressGetFormatSize(m_hIC, lpbiInput);
}

LRESULT CHIC::DecompressGetPalette(LPBITMAPINFOHEADER lpbiInput, LPBITMAPINFOHEADER lpbiOutput)
{
    return ICDecompressGetPalette(m_hIC, lpbiInput, lpbiOutput);
}

// opens a decompressor that is compatible with the specified formats.
// Returns a handle of a decompressor if successful or zero otherwise.
HIC CHIC::DecompressOpen(FOURCC dwfccType, FOURCC dwfccHandler, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    ASSERT(!isOpen());
    m_hIC = ICDecompressOpen(dwfccType, dwfccHandler, lpbiIn, lpbiOut);
    if (isOpen())
    {
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

LRESULT CHIC::DecompressQuery(LPBITMAPINFO lpbiInput, LPBITMAPINFO lpbiOutput)
{
    return ICDecompressQuery(m_hIC, lpbiInput, lpbiOutput);
}

LRESULT CHIC::DecompressSetPalette(LPBITMAPINFOHEADER lpbiPalette)
{
    return ICDecompressSetPalette(m_hIC, lpbiPalette);
}

LRESULT CHIC::DecompressExQuery(DWORD dwFlags, LPBITMAPINFOHEADER lpbiSrc, LPVOID lpSrc, int xSrc, int ySrc, int dxSrc,
                                int dySrc, LPBITMAPINFOHEADER lpbiDst, LPVOID lpDst, int xDst, int yDst, int dxDst,
                                int dyDst)
{
    return ::ICDecompressExQuery(m_hIC, dwFlags, lpbiSrc, lpSrc, xSrc, ySrc, dxSrc, dySrc, lpbiDst, lpDst, xDst, yDst,
                                 dxDst, dyDst);
}

LRESULT CHIC::DrawChangePalette(LPBITMAPINFO lpbiInput)
{
    return ICDrawChangePalette(m_hIC, lpbiInput);
}

LRESULT CHIC::Draw(DWORD dwFlags, LPVOID lpFormat, LPVOID lpData, DWORD cbData, LONG lTime)
{
    return ::ICDraw(m_hIC, dwFlags, lpFormat, lpData, cbData, lTime);
}

LRESULT CHIC::DrawBegin(DWORD dwFlags, HPALETTE hpal, HWND hwnd, HDC hdc, int xDst, int yDst, int dxDst, int dyDst,
                      LPBITMAPINFOHEADER lpbi, int xSrc, int ySrc, int dxSrc, int dySrc, DWORD dwRate, DWORD dwScale)
{
    return ::ICDrawBegin(m_hIC, dwFlags, hpal, hwnd, hdc, xDst, yDst, dxDst, dyDst, lpbi, xSrc, ySrc, dxSrc, dySrc,
                         dwRate, dwScale);
}

LRESULT CHIC::DrawEnd()
{
    return ICDrawEnd(m_hIC);
}

LRESULT CHIC::DrawFlush()
{
    return ICDrawFlush(m_hIC);
}

LRESULT CHIC::DrawGetTime(LPVOID lplTime)
{
    return ICDrawGetTime(m_hIC, lplTime);
}

// opens a driver that can draw images with the specified format.
// Returns a handle of a driver if successful or zero otherwise.
HIC CHIC::DrawOpen(FOURCC dwfccType, FOURCC dwfccHandler, LPBITMAPINFOHEADER lpbiIn)
{
    ASSERT(!isOpen());
    m_hIC = ICDrawOpen(dwfccType, dwfccHandler, lpbiIn);
    if (isOpen())
    {
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

LRESULT CHIC::DrawQuery(LPBITMAPINFO lpbiInput)
{
    return ICDrawQuery(m_hIC, lpbiInput);
}
