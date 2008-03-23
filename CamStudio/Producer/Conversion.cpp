#include "stdafx.h"
#include <vfw.h>
#include <windowsx.h>

#include "./Conversion/WAVEIO.H"
#include "Conversion.h"

extern CString GetTempPathEx(CString fileName, CString fxstr, CString exstr);

//ver 2.25
#include "resource.h"
extern int ErrMsg (LPSTR sz,...);
extern int runmode;
BOOL        gfCancelConvert = FALSE;
int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus);


//if return -1, or 0, no new file is created
//if return 1 or 2, the converted file is saved in SaveFile
int MultiStepConvertToPCM(CString filename, CString SaveFile, LPWAVEFORMATEX dstFormat, int sizeDstFormat)
{

	CString intermediatePath;
	int retval = 0;
	LPWAVEFORMATEX intermediateFormat = NULL;

	
	if (dstFormat) {
		if ((dstFormat->wFormatTag)!=WAVE_FORMAT_PCM) {

			if ((runmode==0) || (runmode==1))
				//MessageBox(NULL,"Destination Format not PCM","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(NULL,IDS_DESTNOTPCM, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);

			return -1;
		}

	}

		
	//First step conversion,
	intermediateFormat = (LPWAVEFORMATEX) malloc(sizeDstFormat);
	memcpy( intermediateFormat, dstFormat, sizeDstFormat);

		
	//warning : Convert File will change the intermediateFormat (to a format compatible with original) after conversion
	retval = ConvertFile(LPCTSTR(filename), LPCTSTR(SaveFile), intermediateFormat, sizeDstFormat);	
	
	if (intermediateFormat) free(intermediateFormat);
	


	if (retval == 0) {
	
		//MessageBox(NULL, "0","Note",MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}
	else if (retval == 2)
	{
		//MessageBox(NULL, "2","Note",MB_OK | MB_ICONEXCLAMATION);
		return 2;
	}
	else if (retval == 1) {

		//need 2nd Step Conversion				
		//intermediatePath= GetTempIntermediatePath();				
		intermediatePath =  GetTempPathEx("\\~intermediate001.wav", "\\~intermediate", ".wav");

		
		//rename
		if (!MoveFile( SaveFile,intermediatePath)) {
			
				//Although there is error copying, the temp file still remains in the temp directory and is not deleted, in case user wants a manual recover
				if ((runmode==0) || (runmode==1))
					//MessageBox(NULL, "File Creation Error. Unable to rename/copy file.","Note",MB_OK | MB_ICONEXCLAMATION);
					MessageOut(NULL,IDS_FILECRERR, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
				return -1;	
			
		}
		
		
		retval = ConvertFile(LPCTSTR(intermediatePath), LPCTSTR(SaveFile), dstFormat, sizeDstFormat);		

		if (retval == 0)
		{
			//actually no 2nd step conversion is performed
			//but a new file SaveFile is created in the 1st step conversion			
			
			//move back
			MoveFile( intermediatePath, SaveFile);			
			return 1;

		}				
				
		if (retval>=0)		
			DeleteFile(intermediatePath);

		

		//retval = ConvertFile(SaveFile,intermediatePath, dstFormat, sizeDstFormat);
		return retval;		

	}
	
	return 0;

}






//return -1 if error
//return 0 if no conversion
//return 1 if nonPCM to PCM conversion
//return 2 if PCM to PCM conversion
int ConvertFile(const char* filename, const char* SaveFile, LPWAVEFORMATEX dstFormat, int sizeDstFormat)
{

    ACMAPPFILEDESC  gaafd;
    TCHAR               fileTitle[APP_MAX_FILE_TITLE_CHARS];
    BOOL                f; 
	MMRESULT mmr;
	
	GetTitle( filename,  fileTitle, APP_MAX_FILE_TITLE_CHARS);
	

	lstrcpy(gaafd.szFilePath, filename);
    lstrcpy(gaafd.szFileTitle, fileTitle);
    
	//get info into gaafd
	f = AcmAppFileOpen(NULL,  &gaafd);
    if (!f)
    {
		//MessageBox(NULL,"Error Opening File","Note",MB_OK | MB_ICONEXCLAMATION);
		if ((runmode==0) || (runmode==1))
			//MessageBox(NULL,"Error Opening Audio File for Conversion","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL,IDS_ERROPAFIC, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
		return -1;
    }


		
	if (((gaafd.pwfx)->wFormatTag)!=WAVE_FORMAT_PCM) {
		dstFormat->wFormatTag = WAVE_FORMAT_PCM;		
		mmr = acmFormatSuggest(NULL, gaafd.pwfx, dstFormat, sizeDstFormat, ACM_FORMATSUGGESTF_WFORMATTAG);	
		PerformConversion(&gaafd, SaveFile, NONPCM_TO_PCM_TYPE , dstFormat, sizeDstFormat);
		return 1;
	}

	//if (gaafd.pwfx)->wFormatTag) ==WAVE_FORMAT_PCM
	if ((dstFormat->wFormatTag)==WAVE_FORMAT_PCM) {
	
		//if PCM rates not similar
		if ((gaafd.pwfx->nChannels != dstFormat->nChannels) ||
			(gaafd.pwfx->nSamplesPerSec != dstFormat->nSamplesPerSec) ||
			(gaafd.pwfx->nAvgBytesPerSec != dstFormat->nAvgBytesPerSec) ||
			(gaafd.pwfx->nBlockAlign != dstFormat->nBlockAlign) ||
			(gaafd.pwfx->wBitsPerSample != dstFormat->wBitsPerSample) ||
			(gaafd.pwfx->cbSize != dstFormat->cbSize))
		{
			mmr = acmFormatSuggest(NULL, gaafd.pwfx, dstFormat, sizeDstFormat, ACM_FORMATSUGGESTF_NCHANNELS  | ACM_FORMATSUGGESTF_NSAMPLESPERSEC | ACM_FORMATSUGGESTF_WBITSPERSAMPLE | ACM_FORMATSUGGESTF_WFORMATTAG);			
			PerformConversion(&gaafd, SaveFile, PCM_TO_PCM_TYPE , dstFormat, sizeDstFormat);			
			return 2;
		}	

	}

	return 0;    

}


BOOL GetTitle( const char* pszFilePath, PTSTR pszFileTitle, int bufsize)
{

	//GetFileTitle( pszFilePath, pszFileTitle, bufsize); 
 
	

    #define IS_SLASH(c)     ('/' == (c) || '\\' == (c))

    //PTSTR       pch;
	const char * pch;

    //
    //  scan to the end of the file path string..
    //
    for (pch = pszFilePath; '\0' != *pch; pch++)
        ;

    //
    //  now scan back toward the beginning of the string until a slash (\),
    //  colon, or start of the string is encountered.
    //
    while ((pch >= pszFilePath) && !IS_SLASH(*pch) && (':' != *pch))
    {
        pch--;
    }

    //
    //  finally, copy the 'title' into the destination buffer.. skip ahead
    //  one char since the above loop steps back one too many chars...
    //
    lstrcpy(pszFileTitle, ++pch);
	

    return (TRUE);
}





BOOL PerformConversion(PACMAPPFILEDESC paafd, const char* SaveFile, int ConversionType, LPWAVEFORMATEX dstFormat, int sizeDstFormat)
{
    //BOOL                f;
    DWORD               nAvgBytesPerSec;
    DWORD               nBlockAlign;
    DWORD               dwTimeAverage;
    PAACONVERTDESC      paacd;

    paacd = (PAACONVERTDESC)LocalAlloc(LPTR, sizeof(*paacd));
    if (NULL == paacd)
    {
        return (FALSE);
    }


    //
    //
    //
    paacd->hmmioSrc      = NULL;
    paacd->hmmioDst      = NULL;

    //
    //  default to 1 second per convert buffer..
    //
    paacd->uBufferTimePerConvert = 1000;

    paacd->dwSrcSamples  = paafd->dwDataSamples;


    //
    //  compute source bytes to read (round down to nearest block for
    //  one second of data)
    //
    nAvgBytesPerSec     = paafd->pwfx->nAvgBytesPerSec;
    nBlockAlign         = paafd->pwfx->nBlockAlign;
    paacd->cbSrcReadSize = nAvgBytesPerSec - (nAvgBytesPerSec % nBlockAlign);

    paacd->cbDstBufSize  = 0L;
    //paacd->fdwOpen       = 0L;
	paacd->fdwOpen       = ACM_STREAMOPENF_NONREALTIME;

    lstrcpy(paacd->szFilePathSrc, paafd->szFilePath);
    paacd->pwfxSrc       = paafd->pwfx;
    paacd->pbSrc         = NULL;

    paacd->cbSrcData     = paafd->dwDataBytes;

    lstrcpy(paacd->szFilePathDst, SaveFile);
    paacd->pwfxDst       = NULL;
    paacd->pbDst         = NULL;

    paacd->fApplyFilter  = FALSE;
    paacd->pwfltr        = NULL;


    paacd->cTotalConverts     = 0L;
    paacd->dwTimeTotal        = 0L;
    paacd->dwTimeShortest     = (DWORD)-1L;
    paacd->dwShortestConvert  = (DWORD)-1L;
    paacd->dwTimeLongest      = 0L;
    paacd->dwLongestConvert   = (DWORD)-1L;

    //f = DialogBoxParam(ghinst,
    //                   DLG_AACHOOSER,
    //                   hwnd,
    //                   AcmAppDlgProcChooser,
    //                  (LPARAM)(UINT)paacd);

	//Not sure
	paacd->pwfxDst = dstFormat;



    if (AcmAppConvertBegin(NULL, paacd))
    {
        AcmAppConvertConvert(NULL, paacd);
    }

    AcmAppConvertEnd(NULL, paacd);
	
 
	
	//if (f)
    {
		//this is needed only if a dialog appears
        //lstrcpy(SaveFile, paacd->szFilePathDst);

        //
        //
        //
        //f = DialogBoxParam(ghinst,
        //                    DLG_AACONVERT,
        //                    hwnd,
        //                    AcmAppConvertDlgProc,
        //                   (LPARAM)(UINT)paacd);
        //if (!f)
        //{
            //AppMsgBox(hwnd, MB_OK | MB_ICONEXCLAMATION,
            //           TEXT("Conversion aborted--destination file is corrupt!"));
        //}

		

        if (paacd->cTotalConverts > 1)
        {
            dwTimeAverage  = paacd->dwTimeTotal;
            dwTimeAverage -= paacd->dwTimeShortest;

            dwTimeAverage /= (paacd->cTotalConverts - 1);
        }
        else
        {
            dwTimeAverage = paacd->dwTimeTotal;
        }

		/*
        AppMsgBox(hwnd, MB_OK | MB_ICONEXCLAMATION,
                    TEXT("Conversion Statistics:\n\nTotal Time:\t%lu ms\nTotal Converts:\t%lu\nShortest Time:\t%lu ms (on %lu)\nLongest Time:\t%lu ms (on %lu)\n\nAverage Time:\t%lu ms"),
                    paacd->dwTimeTotal,
                    paacd->cTotalConverts,
                    paacd->dwTimeShortest,
                    paacd->dwShortestConvert,
                    paacd->dwTimeLongest,
                    paacd->dwLongestConvert,
                    dwTimeAverage);
					*/

        //if (f)
        //{
            //AcmAppOpenInstance(hwnd, paacd->szFilePathDst, FALSE);
        //}
    }


    //
    //
    //
    if (NULL != paacd->pwfxDst)
    {
        GlobalFreePtr(paacd->pwfxDst);
        paacd->pwfxDst = NULL;
    }

    if (NULL != paacd->pwfltr)
    {
        GlobalFreePtr(paacd->pwfltr);
        paacd->pwfltr = NULL;
    }

    paacd->pwfxSrc = NULL;


    LocalFree((HLOCAL)paacd);

    return (0);
} // AcmAppFileConvert()



BOOL AcmAppFileOpen(HWND hwnd, PACMAPPFILEDESC paafd)
{
    WAVEIOCB    wio;
    WIOERR      werr;


    HANDLE      hf;

    DWORD       cbFileSize;
    BOOL        fReturn;



    //
    //  blow previous stuff...
    //
    if (NULL != paafd->pwfx)
    {
        GlobalFreePtr(paafd->pwfx);
        paafd->pwfx  = NULL;
        paafd->cbwfx = 0;
    }

    paafd->fdwState          = 0L;
    paafd->cbFileSize        = 0L;
    paafd->uDosChangeDate    = 0;
    paafd->uDosChangeTime    = 0;
    paafd->fdwFileAttributes = 0L;
    paafd->dwDataBytes       = 0L;
    paafd->dwDataSamples     = 0L;


    //
    //  open the file for reading..
    //

    hf = CreateFile(paafd->szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL,
                    OPEN_EXISTING, 0, 0);
    if (INVALID_HANDLE_VALUE == hf)
        return (FALSE);


    //
    //  assume the worst
    //
    fReturn = FALSE;

    //
    //  determine the length in _bytes_ of the file
    //

    cbFileSize = GetFileSize((HANDLE)hf, NULL);


    //
    //
    //
    //
    paafd->cbFileSize        = cbFileSize;


{
    BY_HANDLE_FILE_INFORMATION  bhfi;
    WORD                        wDosChangeDate;
    WORD                        wDosChangeTime;

    GetFileInformationByHandle(hf, &bhfi);

    paafd->fdwFileAttributes = bhfi.dwFileAttributes;

    FileTimeToDosDateTime(&bhfi.ftLastWriteTime,
                          &wDosChangeDate, &wDosChangeTime);

    paafd->uDosChangeDate = (UINT)wDosChangeDate;
    paafd->uDosChangeTime = (UINT)wDosChangeTime;
}



    //
    //  now return the fully qualified path and title for the file
    //

    AppGetFileTitle(paafd->szFilePath, paafd->szFileTitle);


    CloseHandle(hf);



    //
    //
    //
    //
    werr = wioFileOpen(&wio, paafd->szFilePath, 0L);
    if (WIOERR_NOERROR == werr)
    {
        UINT        cbwfx;

        cbwfx = SIZEOF_WAVEFORMATEX(wio.pwfx);

        paafd->pwfx = (LPWAVEFORMATEX)GlobalAllocPtr(GHND, cbwfx);
        if (NULL != paafd->pwfx)
        {
            _fmemcpy(paafd->pwfx, wio.pwfx, cbwfx);

            paafd->cbwfx         = cbwfx;

            paafd->dwDataBytes   = wio.dwDataBytes;
            paafd->dwDataSamples = wio.dwDataSamples;

            fReturn = TRUE;
        }

        wioFileClose(&wio, 0L);
    }
    else
    {
        ErrMsg(TEXT("The file '%s' cannot be loaded as a wave file (wio error=%u)."),(LPTSTR)paafd->szFilePath, werr);
    }


    //
    //  !!! before returning, we really should try to display a error
    //      message... memory error, etc..
    //
    return (fReturn);
} // AcmAppFileOpen()


BOOL AppGetFileTitle
(
    PTSTR                   pszFilePath,
    PTSTR                   pszFileTitle
)
{
    #define IS_SLASH(c)     ('/' == (c) || '\\' == (c))

    PTSTR       pch;

    //
    //  scan to the end of the file path string..
    //
    for (pch = pszFilePath; '\0' != *pch; pch++)
        ;

    //
    //  now scan back toward the beginning of the string until a slash (\),
    //  colon, or start of the string is encountered.
    //
    while ((pch >= pszFilePath) && !IS_SLASH(*pch) && (':' != *pch))
    {
        pch--;
    }

    //
    //  finally, copy the 'title' into the destination buffer.. skip ahead
    //  one char since the above loop steps back one too many chars...
    //
    lstrcpy(pszFileTitle, ++pch);

    return (TRUE);
} // AppGetFileTitle()




BOOL AcmAppConvertEnd
(
    HWND                hdlg,
    PAACONVERTDESC      paacd
)
{
    MMRESULT            mmr;
    LPACMSTREAMHEADER   pash;


    //
    //
    //
    //
    if (NULL != paacd->hmmioSrc)
    {
        mmioClose(paacd->hmmioSrc, 0);
        paacd->hmmioSrc = NULL;
    }

    if (NULL != paacd->hmmioDst)
    {
        mmioAscend(paacd->hmmioDst, &paacd->ckDst, 0);
        mmioAscend(paacd->hmmioDst, &paacd->ckDstRIFF, 0);

        mmioClose(paacd->hmmioDst, 0);
        paacd->hmmioDst = NULL;
    }


    //
    //
    //
    //
    if (NULL != paacd->has)
    {
        pash = &paacd->ash;

        if (ACMSTREAMHEADER_STATUSF_PREPARED & pash->fdwStatus)
        {
            pash->cbSrcLength = paacd->cbSrcReadSize;
            pash->cbDstLength = paacd->cbDstBufSize;

            mmr = acmStreamUnprepareHeader(paacd->has, &paacd->ash, 0L);
            if (MMSYSERR_NOERROR != mmr)
            {
                ErrMsg(TEXT("acmStreamUnprepareHeader() failed with error = %u!"), mmr);
            }
        }

        //
        //
        //
        acmStreamClose(paacd->has, 0L);
        paacd->has = NULL;

        if (NULL != paacd->had)
        {
            acmDriverClose(paacd->had, 0L);
            paacd->had = NULL;
        }
    }


    //
    //
    //
    //
    if (NULL != paacd->pbSrc)
    {
        GlobalFreePtr(paacd->pbSrc);
        paacd->pbSrc = NULL;
    }
    
    if (NULL != paacd->pbDst)
    {
        GlobalFreePtr(paacd->pbDst);
        paacd->pbDst = NULL;
    }


    return (TRUE);
} // AcmAppConvertEnd()


//--------------------------------------------------------------------------;
//  
//  BOOL AcmAppConvertBegin
//  
//  Description:
//  
//  
//  Arguments:
//      HWND hdlg:
//  
//      PAACONVERTDESC paacd:
//  
//  Return (BOOL):
//  
//  
//--------------------------------------------------------------------------;

BOOL AcmAppConvertBegin
(
    HWND                    hdlg,
    PAACONVERTDESC          paacd
)
{
    TCHAR               ach[40];
    MMRESULT            mmr;
    MMCKINFO            ckSrcRIFF;
    MMCKINFO            ck;
    DWORD               dw;
    LPACMSTREAMHEADER   pash;
    LPWAVEFILTER        pwfltr;


    //
    //
    //
    if (NULL != paacd->hadid)
    {
        mmr = acmDriverOpen(&paacd->had, paacd->hadid, 0L);
        if (MMSYSERR_NOERROR != mmr)
        {
            AcmAppGetErrorString(mmr, ach);
            ErrMsg(TEXT("The selected driver (hadid=%.04Xh) cannot be opened. %s (%u)"), paacd->hadid, (LPSTR)ach, mmr);
            return (FALSE);
        }
    }


    //
    //
    //
    //
    pwfltr = paacd->fApplyFilter ? paacd->pwfltr : (LPWAVEFILTER)NULL;

    mmr = acmStreamOpen(&paacd->has,
                        paacd->had,
                        paacd->pwfxSrc,
                        paacd->pwfxDst,
                        pwfltr,
                        0L,
                        0L,
                        paacd->fdwOpen);

    if (MMSYSERR_NOERROR != mmr)
    {
        ErrMsg(TEXT("acmStreamOpen() failed with error = %u!"), mmr);

        return (FALSE);
    }


    //
    //
    //
    mmr = acmStreamSize(paacd->has,
                        paacd->cbSrcReadSize,
                        &paacd->cbDstBufSize,
                        ACM_STREAMSIZEF_SOURCE);

    if (MMSYSERR_NOERROR != mmr)
    {
        ErrMsg(TEXT("acmStreamSize() failed with error = %u!"), mmr);

        return (FALSE);
    }



    //
    //  first try to open the file, etc.. open the given file for reading
    //  using buffered I/O
    //
    paacd->hmmioSrc = mmioOpen(paacd->szFilePathSrc,
                               NULL,
                               MMIO_READ | MMIO_DENYWRITE | MMIO_ALLOCBUF);
    if (NULL == paacd->hmmioSrc)
        goto aacb_Error;

    //
    //
    //
    paacd->hmmioDst = mmioOpen(paacd->szFilePathDst,
                               NULL,
                               MMIO_CREATE | MMIO_WRITE | MMIO_EXCLUSIVE | MMIO_ALLOCBUF);
    if (NULL == paacd->hmmioDst)
        goto aacb_Error;



    //
    //
    //
    //
    pash = &paacd->ash;
    pash->fdwStatus = 0L;


    //
    //  allocate the src and dst buffers for reading/converting data
    //
    paacd->pbSrc = (LPBYTE)GlobalAllocPtr(GHND, paacd->cbSrcReadSize);
    if (NULL == paacd->pbSrc)
        goto aacb_Error;
    
    paacd->pbDst = (LPBYTE)GlobalAllocPtr(GHND, paacd->cbDstBufSize);
    if (NULL == paacd->pbDst)
        goto aacb_Error;


    //
    //
    //
    //
    pash->cbStruct          = sizeof(*pash);
    pash->fdwStatus         = 0L;
    pash->dwUser            = 0L;
    pash->pbSrc             = paacd->pbSrc;
    pash->cbSrcLength       = paacd->cbSrcReadSize;
    pash->cbSrcLengthUsed   = 0L;
    pash->dwSrcUser         = paacd->cbSrcReadSize;
    pash->pbDst             = paacd->pbDst;
    pash->cbDstLength       = paacd->cbDstBufSize;
    pash->cbDstLengthUsed   = 0L;
    pash->dwDstUser         = paacd->cbDstBufSize;

    mmr = acmStreamPrepareHeader(paacd->has, pash, 0L);
    if (MMSYSERR_NOERROR != mmr)
    {
        ErrMsg(TEXT("acmStreamPrepareHeader() failed with error = %u!"), mmr);
        goto aacb_Error;
    }                          



    //
    //  create the RIFF chunk of form type 'WAVE'
    //
    //
    paacd->ckDstRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    paacd->ckDstRIFF.cksize  = 0L;
    if (mmioCreateChunk(paacd->hmmioDst, &paacd->ckDstRIFF, MMIO_CREATERIFF))
        goto aacb_Error;

    //
    //  locate a 'WAVE' form type in a 'RIFF' thing...
    //
    ckSrcRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    if (mmioDescend(paacd->hmmioSrc, (LPMMCKINFO)&ckSrcRIFF, NULL, MMIO_FINDRIFF))
        goto aacb_Error;

    //
    //  we found a WAVE chunk--now go through and get all subchunks that
    //  we know how to deal with...
    //
    while (mmioDescend(paacd->hmmioSrc, &ck, &ckSrcRIFF, 0) == 0)
    {
        //
        //  quickly check for corrupt RIFF file--don't ascend past end!
        //
        if ((ck.dwDataOffset + ck.cksize) > (ckSrcRIFF.dwDataOffset + ckSrcRIFF.cksize))
            goto aacb_Error;

        switch (ck.ckid)
        {
            //
            //  explicitly skip these...
            //
            //
            //
            case mmioFOURCC('f', 'm', 't', ' '):
                break;

            case mmioFOURCC('d', 'a', 't', 'a'):
                break;

            case mmioFOURCC('f', 'a', 'c', 't'):
                break;

            case mmioFOURCC('J', 'U', 'N', 'K'):
                break;

            case mmioFOURCC('P', 'A', 'D', ' '):
                break;

            case mmioFOURCC('c', 'u', 'e', ' '):
                break;


            //
            //  copy chunks that are OK to copy
            //
            //
            //
            case mmioFOURCC('p', 'l', 's', 't'):
                // although without the 'cue' chunk, it doesn't make much sense
                riffCopyChunk(paacd->hmmioSrc, paacd->hmmioDst, &ck);
                break;

            case mmioFOURCC('D', 'I', 'S', 'P'):
                riffCopyChunk(paacd->hmmioSrc, paacd->hmmioDst, &ck);
                break;

                
            //
            //  don't copy unknown chunks
            //
            //
            //
            default:
                break;
        }

        //
        //  step up to prepare for next chunk..
        //
        mmioAscend(paacd->hmmioSrc, &ck, 0);
    }

#if 0
    //
    //  now write out possibly editted chunks...
    //
    if (riffWriteINFO(paacd->hmmioDst, (glpwio->pInfo)))
    {
        goto aacb_Error;
    }
#endif

    //
    // go back to beginning of data portion of WAVE chunk
    //
    if (-1 == mmioSeek(paacd->hmmioSrc, ckSrcRIFF.dwDataOffset + sizeof(FOURCC), SEEK_SET))
        goto aacb_Error;

    ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
    mmioDescend(paacd->hmmioSrc, &ck, &ckSrcRIFF, MMIO_FINDCHUNK);


    //
    //  now create the destination fmt, fact, and data chunks _in that order_
    //
    //
    //
    //  hmmio is now descended into the 'RIFF' chunk--create the format chunk
    //  and write the format header into it
    //
    dw = SIZEOF_WAVEFORMATEX(paacd->pwfxDst);

    paacd->ckDst.ckid   = mmioFOURCC('f', 'm', 't', ' ');
    paacd->ckDst.cksize = dw;
    if (mmioCreateChunk(paacd->hmmioDst, &paacd->ckDst, 0))
        goto aacb_Error;

    if (mmioWrite(paacd->hmmioDst, (HPSTR)paacd->pwfxDst, dw) != (LONG)dw)
        goto aacb_Error;

    if (mmioAscend(paacd->hmmioDst, &paacd->ckDst, 0) != 0)
        goto aacb_Error;

    //
    //  create the 'fact' chunk (not necessary for PCM--but is nice to have)
    //  since we are not writing any data to this file (yet), we set the
    //  samples contained in the file to 0..
    //
    paacd->ckDst.ckid   = mmioFOURCC('f', 'a', 'c', 't');
    paacd->ckDst.cksize = 0L;
    if (mmioCreateChunk(paacd->hmmioDst, &paacd->ckDst, 0))
        goto aacb_Error;

    if (mmioWrite(paacd->hmmioDst, (HPSTR)&paacd->dwSrcSamples, sizeof(DWORD)) != sizeof(DWORD))
        goto aacb_Error;

    if (mmioAscend(paacd->hmmioDst, &paacd->ckDst, 0) != 0)
        goto aacb_Error;


    //
    //  create the data chunk AND STAY DESCENDED... for reasons that will
    //  become apparent later..
    //
    paacd->ckDst.ckid   = mmioFOURCC('d', 'a', 't', 'a');
    paacd->ckDst.cksize = 0L;
    if (mmioCreateChunk(paacd->hmmioDst, &paacd->ckDst, 0))
        goto aacb_Error;

    //
    //  at this point, BOTH the src and dst files are sitting at the very
    //  beginning of their data chunks--so we can READ from the source,
    //  CONVERT the data, then WRITE it to the destination file...
    //
    return (TRUE);


    //
    //
    //
    //
aacb_Error:

    AcmAppConvertEnd(hdlg, paacd);

    return (FALSE);
} // AcmAppConvertBegin()


//--------------------------------------------------------------------------;
//  
//  BOOL AcmAppConvertConvert
//  
//  Description:
//  
//  
//  Arguments:
//      HWND hdlg:
//  
//      PAACONVERTDESC paacd:
//  
//  Return (BOOL):
//  
//  
//--------------------------------------------------------------------------;

BOOL AcmAppConvertConvert
(
    HWND                hdlg,
    PAACONVERTDESC     paacd
)
{
    MMRESULT            mmr;
    TCHAR               ach[40];
    DWORD               dw;
    WORD                w;
    DWORD               dwCurrent;
    WORD                wCurPercent;
    LPACMSTREAMHEADER   pash;
    DWORD               cbRead;
    DWORD               dwTime;


    wCurPercent = (WORD)-1;

    paacd->cTotalConverts    = 0L;
    paacd->dwTimeTotal       = 0L;
    paacd->dwTimeLongest     = 0L;
    if (0 == paacd->cbSrcData)
    {
        paacd->dwTimeShortest    = 0L;
        paacd->dwShortestConvert = 0L;
        paacd->dwLongestConvert  = 0L;
    }
    else
    {
        paacd->dwTimeShortest    = (DWORD)-1L;
        paacd->dwShortestConvert = (DWORD)-1L;
        paacd->dwLongestConvert  = (DWORD)-1L;
    }

    pash = &paacd->ash;

    for (dwCurrent = 0; dwCurrent < paacd->cbSrcData; )
    {
        w = (WORD)((dwCurrent * 100) / paacd->cbSrcData);
        if (w != wCurPercent)
        {
            wCurPercent = w;
            wsprintf(ach, TEXT("%u%%"), wCurPercent);

            //if (hdlg)
            //    SetDlgItemText(hdlg, IDD_AACONVERT_TXT_STATUS, ach);
        }

        AppDlgYield(hdlg);

        if (gfCancelConvert)
            goto aacc_Error;

        //
        //
        //
        cbRead = min(paacd->cbSrcReadSize, paacd->cbSrcData - dwCurrent);
        dw = mmioRead(paacd->hmmioSrc, (char *) paacd->pbSrc, cbRead);
        if (0L == dw)
            break;


        AppDlgYield(hdlg);
        if (gfCancelConvert)
            goto aacc_Error;

             

        //
        //
        //
        pash->cbSrcLength     = dw;
        pash->cbDstLengthUsed = 0L;



        dwTime = timeGetTime();

        mmr = acmStreamConvert(paacd->has,
                               &paacd->ash,
                               ACM_STREAMCONVERTF_BLOCKALIGN);

        if (MMSYSERR_NOERROR != mmr)
        {
            ErrMsg(TEXT("acmStreamConvert() failed with error = %u!"), mmr);
            goto aacc_Error;
        }

        while (0 == (ACMSTREAMHEADER_STATUSF_DONE & ((AACONVERTDESC volatile *)paacd)->ash.fdwStatus))
            ;

        dwTime = timeGetTime() - dwTime;


        paacd->dwTimeTotal += dwTime;

        if (dwTime < paacd->dwTimeShortest)
        {
            paacd->dwTimeShortest    = dwTime;
            paacd->dwShortestConvert = paacd->cTotalConverts;
        }

        if (dwTime > paacd->dwTimeLongest)
        {
            paacd->dwTimeLongest     = dwTime;
            paacd->dwLongestConvert  = paacd->cTotalConverts;
        }

        paacd->cTotalConverts++;


        AppDlgYield(hdlg);
        if (gfCancelConvert)
            goto aacc_Error;


        //
        //
        //
        dw = (cbRead - pash->cbSrcLengthUsed);
        if (0L != dw)
        {
            mmioSeek(paacd->hmmioSrc, -(LONG)dw, SEEK_CUR);
        }

        dwCurrent += pash->cbSrcLengthUsed;


        //
        //
        //
        dw = pash->cbDstLengthUsed;
        if (0L == dw)
            break;
          
        if (mmioWrite(paacd->hmmioDst, (char *) paacd->pbDst, dw) != (LONG)dw)
            goto aacc_Error;
    }


    //
    //
    //
    //
    //
    //
    wCurPercent = (WORD)-1;

    for (;paacd->cbSrcData;)
    {
        w = (WORD)((dwCurrent * 100) / paacd->cbSrcData);
        if (w != wCurPercent)
        {
            wCurPercent = w;
            wsprintf(ach, TEXT("Cleanup Pass -- %u%%"), wCurPercent);

            //if (hdlg)
            //    SetDlgItemText(hdlg, IDD_AACONVERT_TXT_STATUS, ach);
        }

        AppDlgYield(hdlg);
        if (gfCancelConvert)
            goto aacc_Error;


        //
        //
        //
        dw = 0L;
        cbRead = min(paacd->cbSrcReadSize, paacd->cbSrcData - dwCurrent);
        if (0L != cbRead)
        {
            dw = mmioRead(paacd->hmmioSrc, (char *) paacd->pbSrc, cbRead);
            if (0L == dw)
                break;
        }


        AppDlgYield(hdlg);
        if (gfCancelConvert)
            goto aacc_Error;

             

        //
        //
        //
        pash->cbSrcLength     = dw;
        pash->cbDstLengthUsed = 0L;



        dwTime = timeGetTime();

        mmr = acmStreamConvert(paacd->has,
                               &paacd->ash,
                               ACM_STREAMCONVERTF_BLOCKALIGN |
                               ACM_STREAMCONVERTF_END);

        if (MMSYSERR_NOERROR != mmr)
        {
            ErrMsg(TEXT("acmStreamConvert() failed with error = %u!"), mmr);
            goto aacc_Error;
        }

        while (0 == (ACMSTREAMHEADER_STATUSF_DONE & ((AACONVERTDESC volatile *)paacd)->ash.fdwStatus))
            ;

        dwTime = timeGetTime() - dwTime;


        paacd->dwTimeTotal += dwTime;

        if (dwTime < paacd->dwTimeShortest)
        {
            paacd->dwTimeShortest    = dwTime;
            paacd->dwShortestConvert = paacd->cTotalConverts;
        }

        if (dwTime > paacd->dwTimeLongest)
        {
            paacd->dwTimeLongest     = dwTime;
            paacd->dwLongestConvert  = paacd->cTotalConverts;
        }

        paacd->cTotalConverts++;


        AppDlgYield(hdlg);
        if (gfCancelConvert)
            goto aacc_Error;

        //
        //
        //
        dw = pash->cbDstLengthUsed;
        if (0L == dw)
        {
            pash->cbDstLengthUsed = 0L;

            //
            //  BUGBUG BOBSTER
            //  What if the last conversion ate up some of the source bytes?
            //  It's possible - the codec could cache some of the data
            //  without actually converting it, right?  The pbSrc pointer
            //  might have to be incremented, and cbSrcLength might have to
            //  to be decreased by cbSrcLengthUsed.  This probably wouldn't
            //  happen with most of our codecs though...?
            //
            mmr = acmStreamConvert(paacd->has,
                                   &paacd->ash,
                                   ACM_STREAMCONVERTF_END);

            if (MMSYSERR_NOERROR == mmr)
            {
                while (0 == (ACMSTREAMHEADER_STATUSF_DONE & ((AACONVERTDESC volatile *)paacd)->ash.fdwStatus))
                    ;
            }

            dw = pash->cbDstLengthUsed;
            if (0L == dw)
                break;
        }
          
        if (mmioWrite(paacd->hmmioDst,(char *)  paacd->pbDst, dw) != (LONG)dw)
            goto aacc_Error;

        //
        //
        //
        dw = (cbRead - pash->cbSrcLengthUsed);
        if (0L != dw)
        {
            mmioSeek(paacd->hmmioSrc, -(LONG)dw, SEEK_CUR);
        }

        dwCurrent += pash->cbSrcLengthUsed;

        if (0L == pash->cbDstLengthUsed)
            break;
    }

    //if (hdlg)
    //    EndConvert(hdlg, !gfCancelConvert, paacd);

    return (!gfCancelConvert);


aacc_Error:

    //if (hdlg)
    //    EndConvert(hdlg, FALSE, paacd);

    return (FALSE);
} // AcmAppConvertConvert()



BOOL AcmAppGetErrorString(MMRESULT   mmr, LPTSTR pszError)
{
    PTSTR               psz;

    switch (mmr)
    {
        case MMSYSERR_NOERROR:
            psz = TEXT("MMSYSERR_NOERROR");
            break;

        case MMSYSERR_ERROR:
            psz = TEXT("MMSYSERR_ERROR");
            break;

        case MMSYSERR_BADDEVICEID:
            psz = TEXT("MMSYSERR_BADDEVICEID");
            break;

        case MMSYSERR_NOTENABLED:
            psz = TEXT("MMSYSERR_NOTENABLED");
            break;

        case MMSYSERR_ALLOCATED:
            psz = TEXT("MMSYSERR_ALLOCATED");
            break;

        case MMSYSERR_INVALHANDLE:
            psz = TEXT("MMSYSERR_INVALHANDLE");
            break;

        case MMSYSERR_NODRIVER:
            psz = TEXT("MMSYSERR_NODRIVER");
            break;

        case MMSYSERR_NOMEM:
            psz = TEXT("MMSYSERR_NOMEM");
            break;

        case MMSYSERR_NOTSUPPORTED:
            psz = TEXT("MMSYSERR_NOTSUPPORTED");
            break;

        case MMSYSERR_BADERRNUM:
            psz = TEXT("MMSYSERR_BADERRNUM");
            break;

        case MMSYSERR_INVALFLAG:
            psz = TEXT("MMSYSERR_INVALFLAG");
            break;

        case MMSYSERR_INVALPARAM:
            psz = TEXT("MMSYSERR_INVALPARAM");
            break;


        case WAVERR_BADFORMAT:
            psz = TEXT("WAVERR_BADFORMAT");
            break;

        case WAVERR_STILLPLAYING:
            psz = TEXT("WAVERR_STILLPLAYING");
            break;

        case WAVERR_UNPREPARED:
            psz = TEXT("WAVERR_UNPREPARED");
            break;

        case WAVERR_SYNC:
            psz = TEXT("WAVERR_SYNC");
            break;


        case ACMERR_NOTPOSSIBLE:
            psz = TEXT("ACMERR_NOTPOSSIBLE");
            break;

        case ACMERR_BUSY:
            psz = TEXT("ACMERR_BUSY");
            break;

        case ACMERR_UNPREPARED:
            psz = TEXT("ACMERR_UNPREPARED");
            break;

        case ACMERR_CANCELED:
            psz = TEXT("ACMERR_CANCELED");
            break;


        default:
            lstrcpy(pszError, TEXT("(unknown)"));
            return (FALSE);
    }

    lstrcpy(pszError, psz);
    return (TRUE);
} // AcmAppGetErrorString()


void AppDlgYield(HWND  hdlg)
{
    MSG     msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if ((hdlg == NULL) || !IsDialogMessage(hdlg, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
} // AppDlgYield()


BOOL RIFFAPI riffCopyChunk(HMMIO hmmioSrc, HMMIO hmmioDst, const LPMMCKINFO lpck)
{
    MMCKINFO    ck;
    HPSTR       hpBuf;

    //
    //
    //
    hpBuf = (HPSTR)GlobalAllocPtr(GHND, lpck->cksize);
    if (!hpBuf)
        return (FALSE);

    ck.ckid   = lpck->ckid;
    ck.cksize = lpck->cksize;
    if (mmioCreateChunk(hmmioDst, &ck, 0))
        goto rscc_Error;
        
    if (mmioRead(hmmioSrc, hpBuf, lpck->cksize) != (LONG)lpck->cksize)
        goto rscc_Error;

    if (mmioWrite(hmmioDst, hpBuf, lpck->cksize) != (LONG)lpck->cksize)
        goto rscc_Error;

    if (mmioAscend(hmmioDst, &ck, 0))
        goto rscc_Error;

    if (hpBuf)
        GlobalFreePtr(hpBuf);

    return (TRUE);

rscc_Error:

    if (hpBuf)
        GlobalFreePtr(hpBuf);

    return (FALSE);
} /* RIFFSupCopyChunk() */


