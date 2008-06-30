// File Format Conversion Routines
#define APP_MAX_FILE_PATH_CHARS     144
#define APP_MAX_FILE_PATH_BYTES     (APP_MAX_FILE_PATH_CHARS * sizeof(TCHAR))
#define APP_MAX_FILE_TITLE_CHARS    APP_MAX_FILE_PATH_CHARS
#define APP_MAX_FILE_TITLE_BYTES    (APP_MAX_FILE_TITLE_CHARS * sizeof(TCHAR))


typedef struct tAACONVERTDESC
{
    HACMDRIVERID        hadid;
    HACMDRIVER          had;
    HACMSTREAM          has;
    DWORD               fdwOpen;

    HMMIO               hmmioSrc;
    HMMIO               hmmioDst;

    MMCKINFO            ckDst;
    MMCKINFO            ckDstRIFF;

    UINT                uBufferTimePerConvert;

    TCHAR               szFilePathSrc[APP_MAX_FILE_PATH_CHARS];
    LPWAVEFORMATEX      pwfxSrc;
    LPBYTE            pbSrc;
	//char *              pbSrc;
    DWORD               dwSrcSamples;
    DWORD               cbSrcData;
    DWORD               cbSrcReadSize;
    TCHAR               szSrcFormatTag[ACMFORMATTAGDETAILS_FORMATTAG_CHARS];
    TCHAR               szSrcFormat[ACMFORMATDETAILS_FORMAT_CHARS];

    TCHAR               szFilePathDst[APP_MAX_FILE_PATH_CHARS];
    LPWAVEFORMATEX      pwfxDst;
    LPBYTE            pbDst;
	//char *				pbDst;
    DWORD               cbDstBufSize;
    TCHAR               szDstFormatTag[ACMFORMATTAGDETAILS_FORMATTAG_CHARS];
    TCHAR               szDstFormat[ACMFORMATDETAILS_FORMAT_CHARS];

    BOOL                fApplyFilter;
    LPWAVEFILTER        pwfltr;
    TCHAR               szFilterTag[ACMFILTERTAGDETAILS_FILTERTAG_CHARS];
    TCHAR               szFilter[ACMFILTERDETAILS_FILTER_CHARS];

    ACMSTREAMHEADER     ash;

    DWORD               cTotalConverts;
    DWORD               dwTimeTotal;
    DWORD               dwTimeShortest;
    DWORD               dwShortestConvert;
    DWORD               dwTimeLongest;
    DWORD               dwLongestConvert;

} AACONVERTDESC, *PAACONVERTDESC;

typedef struct tACMAPPFILEDESC
{
    DWORD           fdwState;

    TCHAR           szFileTitle[APP_MAX_FILE_TITLE_CHARS];
    TCHAR           szFilePath[APP_MAX_FILE_PATH_CHARS];
    
    DWORD           cbFileSize;
    UINT            uDosChangeDate;
    UINT            uDosChangeTime;
    DWORD           fdwFileAttributes;

    LPWAVEFORMATEX  pwfx;
    UINT            cbwfx;

    DWORD           dwDataBytes;
    DWORD           dwDataSamples;

} ACMAPPFILEDESC, *PACMAPPFILEDESC;



#define PCM_TO_PCM_TYPE 0
#define NONPCM_TO_PCM_TYPE  1

BOOL GetTitle( const char* pszFilePath, PTSTR pszFileTitle, int bufsize);
BOOL OpenAcmFile(PACMAPPFILEDESC paafd);
BOOL PerformConversion(PACMAPPFILEDESC paafd, const char* SaveFile, int ConversionType, LPWAVEFORMATEX dstFormat, int sizeDstFormat);

BOOL AcmAppFileOpen(HWND hwnd, PACMAPPFILEDESC paafd);

extern BOOL AcmAppConvertEnd(HWND hdlg,PAACONVERTDESC paacd);
extern BOOL AcmAppConvertBegin(HWND hdlg,PAACONVERTDESC paacd);
extern BOOL AcmAppConvertConvert(HWND hdlg,PAACONVERTDESC paacd);
int ConvertFile(const char* filename, const char* SaveFile, LPWAVEFORMATEX dstFormat, int sizeDstFormat);
int MultiStepConvertToPCM(CString filename, CString SaveFile, LPWAVEFORMATEX dstFormat, int sizeDstFormat);
char* GetTempIntermediatePath();


BOOL AppGetFileTitle(PTSTR pszFilePath,PTSTR pszFileTitle);
BOOL AcmAppFileOpen(HWND hwnd, PACMAPPFILEDESC paafd);
BOOL AcmAppGetErrorString(MMRESULT mmr, LPTSTR pszError);
void AppDlgYield(HWND  hdlg);

#define SIZEOF_WAVEFORMATEX(pwfx)   ((WAVE_FORMAT_PCM==(pwfx)->wFormatTag)?sizeof(PCMWAVEFORMAT):(sizeof(WAVEFORMATEX)+(pwfx)->cbSize))
#define RIFFAPI  FAR PASCAL
BOOL RIFFAPI riffCopyChunk(HMMIO hmmioSrc, HMMIO hmmioDst, const LPMMCKINFO lpck);

#define WM_CONVERT_BEGIN        (WM_USER + 100)
#define WM_CONVERT_END          (WM_USER + 101)
#define BeginConvert(hwnd, paacd)   PostMessage(hwnd, WM_CONVERT_BEGIN, 0, (LPARAM)(UINT)paacd)
#define EndConvert(hwnd, f, paacd)  PostMessage(hwnd, WM_CONVERT_END, (WPARAM)f, (LPARAM)(UINT)paacd)