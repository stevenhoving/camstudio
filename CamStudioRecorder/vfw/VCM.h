/////////////////////////////////////////////////////////////////////////////
// VCM.h - include file for video compression manager interface class
// CHIC -    provides a thin wrappper to the installable comprression API of
//            the MS Video for Windows library.
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include <VFW.h>

// known video handlers
const FOURCC ICHANDLER_MSVC = mmioFOURCC('M', 'S', 'V', 'C');
const FOURCC ICHANDLER_MRLE = mmioFOURCC('M', 'R', 'L', 'E');
const FOURCC ICHANDLER_CVID = mmioFOURCC('C', 'V', 'I', 'D');
const FOURCC ICHANDLER_DIVX = mmioFOURCC('D', 'I', 'V', 'X');
const FOURCC ICHANDLER_VIFP = mmioFOURCC('V', 'I', 'F', 'P');
const FOURCC ICHANDLER_IV50 = mmioFOURCC('I', 'V', '5', '0');

// CVCM - simple wrapper class around a HIC and the VCM interface.
class CHIC
{
public:
    CHIC();
    virtual ~CHIC();

    operator HIC() const;
    FOURCC Handler() const;

private:
    HIC m_hIC;             // handle to a compressor or decompressor
    FOURCC m_dwfccType;    // type of compressor or decompressor
    FOURCC m_dwfccHandler; // Preferred handler of the specified type.
    ICINFO m_icInfo;
    LPVOID m_pState;         // pointer to compressor state info.
    DWORD_PTR m_ulStateSize; // size (in bytes) of memory m_pState points to.

public:
#ifdef _DEBUG
    static void TRACEFOURCC(FOURCC dwfcc, const LPCTSTR pName = 0);
    static void TRACEINFO(const ICINFO &cInfo);
#else
    static void TRACEFOURCC(FOURCC /*dwfcc*/, const LPCTSTR /*pName*/ = 0);
    static void TRACEINFO(const ICINFO & /*cInfo*/);
#endif

private:
    bool getState();

public:
    bool isOpen() const;
    bool isType(FOURCC dwfccType) const;
    bool isHandler(FOURCC dwfccHandler) const;

    // notifies a video compression driver to prepare to compress data.
    // Returns ICERR_OK if the specified compression is supported or ICERR_BADFORMAT
    // if the input or output format is not supported
    LRESULT CompressBegin(LPBITMAPINFO lpbiInput, LPBITMAPINFO lpbiOutput);
    // notifies a video compression driver to end compression and free resources
    // allocated for compression.
    // Returns ICERR_OK if successful or an error otherwise.
    LRESULT CompressEnd();

    // requests the output format of the compressed data from a video compression driver.
    LRESULT CompressGetFormat(LPBITMAPINFO lpbiInput, LPBITMAPINFO lpbiOutput);
    // requests the size of the output format of the compressed data from a video
    // compression driver.
    LRESULT CompressGetFormatSize(LPBITMAPINFO lpbiInput);
    // requests that the video compression driver supply the maximum size of one frame
    // of data when compressed into the specified output format.
    // Returns the maximum number of bytes a single compressed frame can occupy.
    LRESULT CompressGetSize(LPBITMAPINFO lpbiInput, LPBITMAPINFO lpbiOutput);

    // queries a video compression driver to determine if it has an About dialog box.
    // Returns ICERR_OK if the driver supports this message or ICERR_UNSUPPORTED otherwise.
    // n.b Actually returns unsupported state ICERR_UNSUPPORTED; otherwise supported.
    LRESULT QueryAbout();
    LRESULT About(HWND hWnd);

    LRESULT QueryConfigure();
    // notifies a video compression driver to display its configuration dialog box.
    LRESULT Configure(HWND hWnd);

    // queries a video compression driver to determine the amount of memory required
    // to retrieve the configuration information. You can use this macro or explicitly
    // call the ICM_GETSTATE message.
    // Returns the amount of memory, in bytes, required by the state information.
    DWORD GetStateSize();
    // queries a video compression driver to return its current configuration in a
    // block of memory.
    // If pv is nullptr, returns the amount of memory, in bytes, required for
    // configuration information.
    // If pv is not nullptr, returns ICERR_OK if successful or an error otherwise.
    LRESULT GetState(LPVOID pv, DWORD_PTR cb);
    LPVOID GetState();
    // notifies a video compression driver to set the state of the compressor.
    // Returns the number of bytes used by the compressor if successful or zero otherwise.
    LRESULT SetState(LPVOID pv, DWORD_PTR cb);

    // closes a compressor or decompressor.
    // Returns ICERR_OK if successful or an error otherwise;
    LRESULT Close();

    // compresses a single video image.
    // Returns ICERR_OK if successful or an error otherwise
    DWORD Compress(DWORD dwFlags, LPBITMAPINFOHEADER lpbiOutput, LPVOID lpData, LPBITMAPINFOHEADER lpbiInput,
                   LPVOID lpBits, LPDWORD lpckid, LPDWORD lpdwFlags, LONG lFrameNum, DWORD dwFrameSize, DWORD dwQuality,
                   LPBITMAPINFOHEADER lpbiPrev, LPVOID lpPrev);
    // displays a dialog box in which a user can select a compressor.
    // This function can display all registered compressors or list only the
    // compressors that support a specific format.
    // Returns TRUE if the user chooses a compressor and presses OK.
    // Returns FALSE on error or if the user presses CANCEL
    BOOL CompressorChoose(HWND hwnd, UINT uiFlags, LPVOID pvIn, LPVOID lpData, PCOMPVARS pc, LPSTR lpszTitle);
    // frees the resources in the COMPVARS structure used by other VCM functions.
    void CompressorFree(PCOMPVARS pc);

    // queries a video compression driver to determine if it supports a specific
    // input format or if it can compress a specific input format to a specific
    // output format.
    // Returns ICERR_OK if the specified compression is supported or ICERR_BADFORMAT otherwise.
    LRESULT CompressQuery(LPBITMAPINFO lpbiInput, LPBITMAPINFO lpbiOutput = 0);

    // decompresses a single video frame.
    // Returns ICERR_OK if successful or an error otherwise.
    LRESULT Decompress(DWORD dwFlags, LPBITMAPINFOHEADER lpbiFormat, LPVOID lpData, LPBITMAPINFOHEADER lpbi,
                     LPVOID lpBits);
    // notifies a video decompression driver to prepare to decompress data.
    // Returns ICERR_OK if the specified decompression is supported or ICERR_BADFORMAT otherwise
    LRESULT DecompressBegin(LPBITMAPINFO lpbiInput, LPBITMAPINFO lpbiOutput);
    // notifies a video decompression driver to end decompression and free resources
    // allocated for decompression.
    // Returns ICERR_OK if successful or an error otherwise.
    LRESULT DecompressEnd();
    // prepares a decompressor for decompressing data.
    // Returns ICERR_OK if successful or an error otherwise.
    LRESULT DecompressExBegin(DWORD dwFlags, LPBITMAPINFOHEADER lpbiSrc, LPVOID lpSrc, int xSrc, int ySrc, int dxSrc,
                              int dySrc, LPBITMAPINFOHEADER lpbiDst, LPVOID lpDst, int xDst, int yDst, int dxDst,
                              int dyDst);
    // notifies a video decompression driver to end decompression and free resources
    // allocated for decompression.
    // Returns ICERR_OK if successful or an error otherwise.
    LRESULT DecompressExEnd();

    // decompresses a single video frame.
    // Returns ICERR_OK if successful or an error otherwise.
    LRESULT DecompressEx(DWORD dwFlags, LPBITMAPINFOHEADER lpbiSrc, LPVOID lpSrc, int xSrc, int ySrc, int dxSrc,
                         int dySrc, LPBITMAPINFOHEADER lpbiDst, LPVOID lpDst, int xDst, int yDst, int dxDst, int dyDst);
    // requests the output format of the decompressed data from a video decompression
    // driver.
    LRESULT DecompressGetFormat(LPBITMAPINFO lpbiInput, LPBITMAPINFO lpbiOutput);
    // requests the size of the output format of the decompressed data from a video
    // decompression driver.
    LRESULT DecompressGetFormatSize(LPBITMAPINFO lpbiInput);
    // requests that the video decompression driver supply the color table of the
    // output BITMAPINFOHEADER structure.
    // Returns ICERR_OK if successful or an error otherwise.
    LRESULT DecompressGetPalette(LPBITMAPINFOHEADER lpbiInput, LPBITMAPINFOHEADER lpbiOutput);
    // opens a decompressor that is compatible with the specified formats.
    // Returns a handle of a decompressor if successful or zero otherwise.
    HIC DecompressOpen(FOURCC fccType, FOURCC fccHandler, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut = 0);

    // queries a video decompression driver to determine if it supports a specific input
    // format or if it can decompress a specific input format to a specific output format.
    // Returns ICERR_OK if the specified decompression is supported or ICERR_BADFORMAT
    // otherwise
    LRESULT DecompressQuery(LPBITMAPINFO lpbiInput, LPBITMAPINFO lpbiOutput);
    // specifies a palette for a video decompression driver to use if it is decompressing
    // to a format that uses a palette.
    // Returns ICERR_OK if the decompression driver can precisely decompress images to the
    // suggested palette using the set of colors as they are arranged in the palette.
    // Returns ICERR_UNSUPPORTED otherwise
    LRESULT DecompressSetPalette(LPBITMAPINFOHEADER lpbiPalette);

    // determines if a decompressor can decompress data with a specific format.
    // Returns ICERR_OK if successful or an error otherwise.
    LRESULT DecompressExQuery(DWORD dwFlags, LPBITMAPINFOHEADER lpbiSrc, LPVOID lpSrc, int xSrc, int ySrc, int dxSrc,
                              int dySrc, LPBITMAPINFOHEADER lpbiDst, LPVOID lpDst, int xDst, int yDst, int dxDst,
                              int dyDst);

    // notifies a rendering driver that the movie palette is changing.
    LRESULT DrawChangePalette(LPBITMAPINFO lpbiInput);

    // decompresses an image for drawing.
    // Returns ICERR_OK if successful or an error otherwise.
    LRESULT Draw(DWORD dwFlags, LPVOID lpFormat, LPVOID lpData, DWORD cbData, LONG lTime);
    // initializes the renderer and prepares the drawing destination for drawing.
    // Returns ICERR_OK if the renderer can decompress the data or ICERR_UNSUPPORTED otherwise.
    LRESULT DrawBegin(DWORD dwFlags, HPALETTE hpal, HWND hwnd, HDC hdc, int xDst, int yDst, int dxDst, int dyDst,
                    LPBITMAPINFOHEADER lpbi, int xSrc, int ySrc, int dxSrc, int dySrc, DWORD dwRate, DWORD dwScale);
    // notifies a rendering driver to decompress the current image to the screen and to
    // release resources allocated for decompression and drawing.
    // Returns ICERR_OK if successful or an error otherwise.
    LRESULT DrawEnd();
    // notifies a rendering driver to render the contents of any image buffers that
    // are waiting to be drawn.
    // Returns ICERR_OK if successful or an error otherwise.
    LRESULT DrawFlush();
    // requests a rendering driver that controls the timing of drawing frames to
    // return the current value of its internal clock.
    // The return value should be specified in samples.
    LRESULT DrawGetTime(LPVOID lplTime);
    // opens a driver that can draw images with the specified format.
    HIC DrawOpen(FOURCC fccType, FOURCC fccHandler, LPBITMAPINFOHEADER lpbiIn);

    // queries a rendering driver to determine if it can render data in a specific
    // format.
    // Returns ICERR_OK if the driver can render data in the specified format or
    // ICERR_BADFORMAT otherwise.
    LRESULT DrawQuery(LPBITMAPINFO lpbiInput);

    // notifies a rendering driver to realize its drawing palette while drawing.
    LRESULT DrawRealize(HDC hdc, BOOL fBackground);

    // notifies a rendering driver to draw the frames that have been passed to it.
    LRESULT DrawRenderBuffer();

    // provides synchronization information to a rendering driver that handles the
    // timing of drawing frames. The synchronization information is the sample
    // number of the frame to draw.
    LRESULT DrawSetTime(LPVOID lpTime);
    // notifies a rendering driver to start its internal clock for the timing of
    // drawing frames.
    LRESULT DrawStart();
    // provides the start and end times of a play operation to a rendering driver.
    void DrawStartPlay(DWORD lFrom, DWORD lTo);
    // notifies a rendering driver to stop its internal clock for the timing of
    // drawing frames.
    LRESULT DrawStop();
    // notifies a rendering driver when a play operation is complete.
    void DrawStopPlay();
    // notifies a rendering driver that the window specified for the ICM_DRAW_BEGIN
    // message needs to be redrawn. The window has moved or become temporarily
    // obscured.
    LRESULT DrawWindow(LPRECT prc);
    // queries a driver for the number of buffers to allocate.
    LRESULT GetBuffersWanted(LPVOID lpdwBuffers);
    // queries a video compression driver for its default (or preferred) key-frame
    // spacing. (result in global DWORD dwICValue)
    LRESULT GetDefaultKeyFrameRate();
    // queries a video compression driver to provide its default quality setting.
    LRESULT GetDefaultQuality();

    // notifies the drawing handler to suggest the input data format.
    // Returns ICERR_OK if successful or an error otherwise.
    LRESULT DrawSuggestFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut, int dxSrc, int dySrc, int dxDst,
                            int dyDst, HIC hicDecompressor);
    // determines the best format available for displaying a compressed image.
    // The function also opens a compressor if a handle of an open compressor is not
    // specified.
    // Returns a handle to a decompressor if successful or zero otherwise.
    HIC GetDisplayFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut, int BitDepth, int dx, int dy);
    // obtains information about a compressor.
    // Returns the number of bytes copied into the structure or zero if an error occurs.
    LRESULT GetInfo(ICINFO &icinfo, DWORD cb);
    // retrieves information about specific installed compressors or enumerates the
    // installed compressors.
    // Returns TRUE if successful or an error otherwise.
    static BOOL GetInfo(FOURCC fccType, FOURCC fccHandler, ICINFO &ricinfo);

    // compresses an image to a given size.
    // This function does not require initialization functions.
    // Returns a handle to a compressed DIB. The image data follows the format header.
    HANDLE ImageCompress(UINT uiFlags, LPBITMAPINFO lpbiIn, LPVOID lpBits, LPBITMAPINFO lpbiOut, LONG lQuality,
                         LONG *plSize);
    // decompresses an image without using initialization functions.
    // Returns a handle to an uncompressed DIB in the CF_DIB format if successful
    // or nullptr otherwise. Image data follows the format header
    HANDLE ImageDecompress(UINT uiFlags, LPBITMAPINFO lpbiIn, LPVOID lpBits, LPBITMAPINFO lpbiOut);
    // retrieves information about specific installed compressors or enumerates the
    // installed compressors.
    // Returns TRUE if successful or an error otherwise.
    BOOL Info(DWORD fccType, DWORD fccHandler, ICINFO &icinfo);

    // installs a new compressor or decompressor.
    // Returns ICERR_OK if successful or an error otherwise.
    BOOL Install(DWORD fccType, DWORD fccHandler, LPARAM lParam, LPSTR szDesc, UINT wFlags);
    // finds a compressor or decompressor that can handle images with the specified
    // formats, or finds a driver that can decompress an image with a specified format
    // directly to hardware.
    // Returns a handle to a compressor or decompressor if successful or zero otherwise.
    HIC Locate(DWORD fccType, DWORD fccHandler, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut, WORD wFlags);
    // opens a compressor or decompressor.
    // Returns a handle to a compressor or decompressor if successful or zero otherwise.
    HIC Open(FOURCC dwfccType, FOURCC dwfccHandler, UINT uMode);

    // opens a compressor or decompressor defined as a function.
    // Returns a handle to a compressor or decompressor if successful or zero otherwise.
    HIC OpenFunction(FOURCC dwfccType, FOURCC dwfccHandler, UINT wMode, FARPROC lpfnHandler);

    // removes an installed compressor.
    // Returns TRUE if successful or FALSE otherwise.
    BOOL Remove(DWORD fccType, DWORD fccHandler, UINT wFlags);
    // sends a message to a compressor.
    // Returns a message-specific result.
    LRESULT SendMessage(UINT wMsg, DWORD_PTR dw1, DWORD_PTR dw2);
    // compresses one frame in a sequence of frames.
    // Returns the address of the compressed bits if successful or nullptr otherwise.
    LPVOID SeqCompressFrame(PCOMPVARS pc, UINT uiFlags, LPVOID lpBits, BOOL *pfKey, LONG *plSize);
    // ends sequence compression that was initiated by using the ICSeqCompressFrameStart
    // and ICSeqCompressFrame functions.
    void SeqCompressFrameEnd(PCOMPVARS pc);
    // initializes resources for compressing a sequence of frames using the
    // ICSeqCompressFrame function.
    // Returns TRUE if successful or FALSE otherwise.
    BOOL SeqCompressFrameStart(PCOMPVARS pc, LPBITMAPINFO lpbiIn);
    // sends the address of a status callback function to a compressor.
    // The compressor calls this function during lengthy operations.
    // Returns ICERR_OK if successful or FALSE otherwise.
    LRESULT SetStatusProc(DWORD dwFlags, LONG lParam, LONG(CALLBACK *fpfnStatus)(LPARAM, UINT, LONG));
    // describes an application-defined status callback function used by the
    // ICM_SET_STATUS_PROC message and the ICSetStatusProc function.
    // Returns zero if processing should continue or a nonzero value if it should end.
    // LONG StatusProc(LPARAM lParam, UINT Message)
    //{
    //    return ::MyStatusProc(lParam, Message);
    //}
};

extern ICINFO *g_compressor_info;
extern int g_num_compressor;

extern CString GetCodecDescription(FOURCC fccHandler);
