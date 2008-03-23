// AudioFormat.cpp : implementation file
//

#include "stdafx.h"
#include "vscap.h"

#include <mmsystem.h>
#include <vfw.h>
#include <windowsx.h>

#include "AudioFormat.h"

#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//  ==========================
//  Ver 1.1
//  ==========================

//External Variables
extern DWORD waveinselected;
extern int audio_bits_per_sample;
extern int audio_num_channels;
extern int audio_samples_per_seconds;
extern BOOL bAudioCompression;

extern LPWAVEFORMATEX      pwfx;
extern DWORD               cbwfx;

extern UINT AudioDeviceID;

extern BOOL interleaveFrames;
extern int  interleaveFactor;
extern int  interleaveUnit;
extern void BuildRecordingFormat();
extern void AllocCompressFormat();

//Local Variables
LPWAVEFORMATEX      pwfxLocal = NULL;
DWORD               cbwfxLocal = 0;

int audio_bits_per_sampleLocal;
int audio_num_channelsLocal;
int audio_samples_per_secondsLocal;
WAVEFORMATEX m_FormatLocal;
BOOL bAudioCompressionLocal = TRUE;

void AllocLocalCompressFormat();
void SuggestLocalCompressFormat();
void BuildLocalRecordingFormat();


// Get Textual Description of WAVEFORMAT
BOOL GetFormatDescription ( LPWAVEFORMATEX  pwformat,  LPTSTR  pszFormatTag,  LPTSTR  pszFormat) ; 
DWORD formatmap[15];
int numformat=0;

UINT devicemap[15];
int numdevice=0;

#define  MILLISECONDS 0
#define  FRAMES 1

extern int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus);


//ver 1.8
int useMCI = 0;
extern CString GetProgPath();


#if !defined(AVE_FORMAT_MPEGLAYER3)
#define  WAVE_FORMAT_MPEGLAYER3 0x0055  
#endif


/////////////////////////////////////////////////////////////////////////////
// AudioFormat dialog


AudioFormat::AudioFormat(CWnd* pParent /*=NULL*/)
	: CDialog(AudioFormat::IDD, pParent)
{
	//{{AFX_DATA_INIT(AudioFormat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AudioFormat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AudioFormat)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AudioFormat, CDialog)
	//{{AFX_MSG_MAP(AudioFormat)
	ON_BN_CLICKED(IDC_CHOOSE, OnChoose)
	ON_CBN_SELCHANGE(IDC_RECORDFORMAT, OnSelchangeRecordformat)
	ON_BN_CLICKED(IDC_INTERLEAVE, OnInterleave)
	ON_BN_CLICKED(IDVOLUME, OnVolume)
	ON_CBN_SELCHANGE(IDC_INPUTDEVICE, OnSelchangeInputdevice)
	ON_BN_CLICKED(IDC_INTERLEAVEFRAMES, OnInterleaveframes)
	ON_BN_CLICKED(IDC_INTERLEAVESECONDS, OnInterleaveseconds)
	ON_BN_CLICKED(IDC_SYSTEMRECORD, OnSystemrecord)
	ON_BN_CLICKED(IDC_BUTTON1, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AudioFormat message handlers


void AudioFormat::OnOK() 
{	
	
	CString interleaveFactorStr;
	int ifactornum;
	
	((CEdit *) (GetDlgItem(IDC_IFACTOR)))->GetWindowText(interleaveFactorStr);		
	sscanf(LPCTSTR(interleaveFactorStr),"%d",&ifactornum);
	if (ifactornum<=0) {

		
		MessageOut(this->m_hWnd,IDS_STRING_INTERLEAVE1, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
		//MessageBox("Interleave factor must greater than 0","Note",MB_OK | MB_ICONEXCLAMATION);
		return;

	}
	
	//if (ifactornum<1000)  {

	//	MessageBox("Interleave factor must be smaller than 1000","Note",MB_OK | MB_ICONEXCLAMATION);
	//	return;

	//}

	interleaveFactor = ifactornum;

	BOOL binteleave =((CButton *) (GetDlgItem(IDC_INTERLEAVE)))->GetCheck();		
	if (binteleave) 
		interleaveFrames = TRUE;
	else
		interleaveFrames = FALSE;

	BOOL interleave_unit= ((CButton *) (GetDlgItem(IDC_INTERLEAVEFRAMES)))->GetCheck();
	if (interleave_unit)
		interleaveUnit = FRAMES;
	else
		interleaveUnit = MILLISECONDS;
	

	//The Recording format, Compressed format and device must be valid before
	//data from the Audio Options Dialog can be updated to the external variables
	if (numformat>0)  {
	
		int sel =((CComboBox *) (GetDlgItem(IDC_COMBO1)))->GetCurSel();
		if (sel>=0) {

			if  (pwfxLocal) {				 

				//Ver 1.2
				int getdevice = ((CComboBox *) (GetDlgItem(IDC_INPUTDEVICE)))->GetCurSel();
				if (getdevice < numdevice) 
				{						

					if (pwfx == NULL) 
						AllocCompressFormat(); //Allocate external format in order to return values 
							
					if (cbwfx >= cbwfxLocal) { //All checks cleared, update external values


						//Updating to external variables

						AudioDeviceID = devicemap[getdevice];
						
						bAudioCompression = bAudioCompressionLocal;

						//Update the external pwfx (compressed format) ;
						cbwfx = cbwfxLocal;
						memcpy( (void *) pwfx, (void *) pwfxLocal, cbwfxLocal );

						
						//Update the external m_Format (Recording format) and related variables;
						waveinselected = formatmap[sel];
						audio_bits_per_sample = audio_bits_per_sampleLocal;
						audio_num_channels = audio_num_channelsLocal;
						audio_samples_per_seconds = audio_samples_per_secondsLocal;
						
						BuildRecordingFormat();								

					}	


				}

			}	


		}		
	}



	if (pwfxLocal) {
		
		GlobalFreePtr(pwfxLocal);
		pwfxLocal = NULL;

	}

	//ver 1.8
	int val = ((CButton *) (GetDlgItem(IDC_SYSTEMRECORD)))->GetCheck();
	if (val)	
		useMCI = 1;
	else 
		useMCI = 0;

	CDialog::OnOK();
}


BOOL AudioFormat::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	//Interleave
	((CButton *) (GetDlgItem(IDC_INTERLEAVE)))->SetCheck(interleaveFrames);		
	CString interleaveFactorStr;
	interleaveFactorStr.Format("%d",interleaveFactor);
	((CEdit *) (GetDlgItem(IDC_IFACTOR)))->SetWindowText(interleaveFactorStr);		
	
	if (interleaveFrames) {
		((CButton *) (GetDlgItem(IDC_IFACTOR)))->EnableWindow(TRUE);
		((CButton *) (GetDlgItem(IDC_INTERLEAVEFRAMES)))->EnableWindow(TRUE);
		((CButton *) (GetDlgItem(IDC_INTERLEAVESECONDS)))->EnableWindow(TRUE);

	}
	else {
		((CButton *) (GetDlgItem(IDC_IFACTOR)))->EnableWindow(FALSE);
		((CButton *) (GetDlgItem(IDC_INTERLEAVEFRAMES)))->EnableWindow(FALSE);
		((CButton *) (GetDlgItem(IDC_INTERLEAVESECONDS)))->EnableWindow(FALSE);

	}

		
	if (interleaveUnit == FRAMES) {
		((CButton *) (GetDlgItem(IDC_INTERLEAVEFRAMES)))->SetCheck(TRUE);
		((CButton *) (GetDlgItem(IDC_INTERLEAVESECONDS)))->SetCheck(FALSE);
	}
	else {
		((CButton *) (GetDlgItem(IDC_INTERLEAVEFRAMES)))->SetCheck(FALSE);
		((CButton *) (GetDlgItem(IDC_INTERLEAVESECONDS)))->SetCheck(TRUE);
	}
	

	audio_bits_per_sampleLocal = audio_bits_per_sample;
	audio_num_channelsLocal = audio_num_channels;
	audio_samples_per_secondsLocal = audio_samples_per_seconds;		
	
	bAudioCompressionLocal = bAudioCompression;

	
	//Ver 1.2

	//Generate device list
	numdevice = 0;
	devicemap[numdevice] = WAVE_MAPPER;
	numdevice ++ ;
	
	((CComboBox *) (GetDlgItem(IDC_INPUTDEVICE)))->ResetContent( );
	((CComboBox *) (GetDlgItem(IDC_INPUTDEVICE)))->AddString("Default input device");		
	
	int numdevs=waveInGetNumDevs();		
	for (int i=0; i<numdevs; i++)
	{ 		
		WAVEINCAPS wicaps; 		
		MMRESULT mmr=waveInGetDevCaps(i,&wicaps,sizeof(WAVEINCAPS));
		if (mmr==MMSYSERR_NOERROR)
		{ 			

				((CComboBox *) (GetDlgItem(IDC_INPUTDEVICE)))->AddString(wicaps.szPname);
				devicemap[numdevice] = i;
				numdevice ++ ;				
				
		}
		
	}

	//Select the device combo box
	int deviceIsSelected= 0; 
	int selectedDevice = WAVE_MAPPER;
	for (i=0; i<numdevice; i++) {

		if (AudioDeviceID == devicemap[i]) {
			((CComboBox *) (GetDlgItem(IDC_INPUTDEVICE)))->SetCurSel(i);
			selectedDevice = devicemap[i];
			deviceIsSelected = 1; 
		}

	}
	if (!deviceIsSelected) {
		if (numdevice)
			((CComboBox *) (GetDlgItem(IDC_INPUTDEVICE)))->SetCurSel(0);

	}


	//Ver 1.2 
	WAVEINCAPS pwic;
	MMRESULT mmr = waveInGetDevCaps( AudioDeviceID ,  &pwic, sizeof(pwic) ); 

	int selected_cindex=-1; //selected index of combo box	
	numformat=0; //counter, number of format

		
	//This code works on the assumption (when filling in values for the user - interfaces)
	//that the  m_Format and pwfx formats (external variables) are already chosen correctly and compatibile with each other
	int devID;
	devID = ((CComboBox *) (GetDlgItem(IDC_INPUTDEVICE)))->GetCurSel();
	if (devID < numdevice) {
		
		UpdateDeviceData(selectedDevice,waveinselected,pwfx);
		
	}

	//ver 1.8
	((CButton *) (GetDlgItem(IDC_SYSTEMRECORD)))->SetCheck(useMCI);
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void AudioFormat::OnChoose() 
{
	// TODO: Add your control notification handler code here
	ACMFORMATCHOOSE     acmfc;
	MMRESULT            mmresult;

	if (pwfxLocal==NULL)
		SuggestLocalCompressFormat();

	//
    //  initialize the ACMFORMATCHOOSE members
    //	
    memset(&acmfc, 0, sizeof(acmfc));

    acmfc.cbStruct        = sizeof(acmfc);    
	acmfc.hwndOwner       = m_hWnd;
	acmfc.pwfx            = pwfxLocal;

	    
	//if (initial_audiosetup)
	//	acmfc.fdwStyle        = 0;
	//else
		acmfc.fdwStyle        = ACMFORMATCHOOSE_STYLEF_INITTOWFXSTRUCT;
	
	acmfc.cbwfx           = cbwfxLocal;
    acmfc.pszTitle        = TEXT("Audio Compression Format");
    acmfc.szFormatTag[0]  = '\0';
    acmfc.szFormat[0]     = '\0';
    acmfc.pszName         = NULL;
    acmfc.cchName         = 0;
    acmfc.fdwEnum         = 0;
    acmfc.pwfxEnum        = NULL;
    acmfc.hInstance       = NULL;
     acmfc.pszTemplateName = NULL;
    acmfc.lCustData       = 0L;
    acmfc.pfnHook         = NULL;

	/////////////////////////////
	// Valid formats for saving
	/////////////////////////////
	BuildLocalRecordingFormat();
	
	acmfc.pwfxEnum = &m_FormatLocal;
	acmfc.fdwEnum = ACM_FORMATENUMF_SUGGEST ;

	//
    //
    //
    mmresult = acmFormatChoose(&acmfc);	
    if (MMSYSERR_NOERROR != mmresult)
    {
        if (ACMERR_CANCELED != mmresult)
        {

			CString msgstr;
			CString tstr;
			tstr.LoadString(IDS_STRING_NOTE);
			msgstr.Format("FormatChoose() failed with error = %u!", mmresult);
            MessageBox(msgstr,tstr, MB_OK | MB_ICONEXCLAMATION);
			
        }        
        
        return ;
    }


	TRACE("\nWAVEFORMAT:");
	TRACE("\nwFormatTag = %d",pwfxLocal->wFormatTag);
	TRACE("\nnChannels = %d",pwfxLocal->nChannels);
	TRACE("\nnSamplesPerSec = %d",pwfxLocal->nSamplesPerSec);
	TRACE("\nnAvgBytesPerSec = %d",pwfxLocal->nAvgBytesPerSec);
	TRACE("\nnBlockAlign = %d",pwfxLocal->nBlockAlign);
	TRACE("\ncbSize = %d",pwfxLocal->cbSize);	 


	UpdateLocalCompressFormatInterface();
	
	
	
}


void AudioFormat::UpdateLocalCompressFormatInterface() {

	
	if (bAudioCompressionLocal==0) {

		((CEdit *) (GetDlgItem(IDC_COMPRESSEDFORMATTAG)))->SetWindowText("None Available");		
		((CEdit *) (GetDlgItem(IDC_COMPRESSEDFORMAT)))->SetWindowText(" ");		
		return;

	}
	
	char  pszFormat[200];
	char  pszFormatTag[200];			
	BOOL res = GetFormatDescription ( pwfxLocal,  pszFormatTag,  pszFormat);
	
	if (res) {
		
		((CComboBox *) (GetDlgItem(IDC_COMPRESSEDFORMATTAG)))->SetWindowText(pszFormatTag);		
		((CComboBox *) (GetDlgItem(IDC_COMPRESSEDFORMAT)))->SetWindowText(pszFormat);		
		
	}

}

BOOL GetFormatDescription ( LPWAVEFORMATEX  pwformat,  LPTSTR  pszFormatTag,  LPTSTR  pszFormat)  
{ 
    MMRESULT      mmr; 
 
    // Retrieve the descriptive name for the FormatTag in pwformat. 
    if (NULL != pszFormatTag) { 
        ACMFORMATTAGDETAILS aftd; 
 
        
        memset(&aftd, 0, sizeof(aftd)); 
 
        // Fill in the required members FormatTAG query. 
        aftd.cbStruct = sizeof(aftd); 
        aftd.dwFormatTag = pwformat->wFormatTag; 
 

        // Ask ACM to find first available driver that supports the specified Format tag. 
        mmr = acmFormatTagDetails(NULL, &aftd, 
            ACM_FORMATTAGDETAILSF_FORMATTAG); 
        if (MMSYSERR_NOERROR != mmr) { 
            
            return (FALSE); 
        } 
 
        // Copy the Format tag name into the calling application's 
        // buffer. 
        lstrcpy(pszFormatTag, aftd.szFormatTag); 
    } 	

	CString formatstr;
	CString str_samples_per_second;
	CString str_bits_per_sample;
	CString str_avg_bytes_per_second;
	CString str_nchannels;

	str_samples_per_second.Format("%d Hz",pwformat->nSamplesPerSec);
	str_bits_per_sample.Format("%d Bit",pwformat->wBitsPerSample);
	str_avg_bytes_per_second.Format("%d Bytes/sec",pwformat->nAvgBytesPerSec);
	if (pwformat->nChannels==1)
		str_nchannels.Format("Mono");
	else
		str_nchannels.Format("Stereo");

	formatstr = str_samples_per_second + ", ";
	if ((pwformat->wBitsPerSample)>0) formatstr = formatstr  + str_bits_per_sample + ", ";
	formatstr = formatstr + str_nchannels + "      " + str_avg_bytes_per_second;
	lstrcpy(pszFormat, LPCTSTR(formatstr)); 
	
 
    return (TRUE); 
} 
 


void AudioFormat::OnSelchangeRecordformat() 
{

	if (numformat<=0) 
		return; //no format to choose from

	int sel =((CComboBox *) (GetDlgItem(IDC_COMBO1)))->GetCurSel();
	if (sel<0) return;


	if (formatmap[sel] == WAVE_FORMAT_1M08) {

		audio_bits_per_sampleLocal = 8;
		audio_num_channelsLocal = 1;
		audio_samples_per_secondsLocal = 11025;		
	
	}

	if (formatmap[sel] ==  WAVE_FORMAT_1M16) {

		audio_bits_per_sampleLocal = 16;
		audio_num_channelsLocal = 1;
		audio_samples_per_secondsLocal = 11025;				

	}
	
	if (formatmap[sel] ==  WAVE_FORMAT_1S08) {

		audio_bits_per_sampleLocal = 8;
		audio_num_channelsLocal = 2;
		audio_samples_per_secondsLocal = 11025;		
		
		
	}

	if (formatmap[sel] ==  WAVE_FORMAT_1S16) { 

		audio_bits_per_sampleLocal = 16;
		audio_num_channelsLocal = 2;
		audio_samples_per_secondsLocal = 11025;				

	}
	
	if (formatmap[sel] ==  WAVE_FORMAT_2M08) { 

		audio_bits_per_sampleLocal = 8;
		audio_num_channelsLocal = 1;
		audio_samples_per_secondsLocal = 22050;			
		
	}

	if (formatmap[sel] ==  WAVE_FORMAT_2M16) { 

		audio_bits_per_sampleLocal = 16;
		audio_num_channelsLocal = 1;
		audio_samples_per_secondsLocal = 22050;			
		
	}

	if (formatmap[sel] ==  WAVE_FORMAT_2S08)  {

		audio_bits_per_sampleLocal = 8;
		audio_num_channelsLocal = 2;
		audio_samples_per_secondsLocal = 22050;			

	}

	if (formatmap[sel] ==  WAVE_FORMAT_2S16) { 

		audio_bits_per_sampleLocal = 16;
		audio_num_channelsLocal = 2;
		audio_samples_per_secondsLocal = 22050;			
	
		
	}

	if (formatmap[sel] ==  WAVE_FORMAT_4M08) { 

		audio_bits_per_sampleLocal = 8;
		audio_num_channelsLocal = 1;
		audio_samples_per_secondsLocal = 44100;			
		
	}

	if (formatmap[sel] ==  WAVE_FORMAT_4M16) {

		audio_bits_per_sampleLocal = 16;
		audio_num_channelsLocal = 1;
		audio_samples_per_secondsLocal = 44100;			
		
	}

	if (formatmap[sel] ==  WAVE_FORMAT_4S08) { 

		audio_bits_per_sampleLocal = 8;
		audio_num_channelsLocal = 2;
		audio_samples_per_secondsLocal = 44100;			
		
		
	}

	if (formatmap[sel] ==  WAVE_FORMAT_4S16) {

		audio_bits_per_sampleLocal = 16;
		audio_num_channelsLocal = 2;
		audio_samples_per_secondsLocal = 44100;			
	
		
	}

	BuildLocalRecordingFormat();
	SuggestLocalCompressFormat();
	UpdateLocalCompressFormatInterface();

	
}

void AudioFormat::OnInterleave() 
{
	// TODO: Add your control notification handler code here
	BOOL binteleave =((CButton *) (GetDlgItem(IDC_INTERLEAVE)))->GetCheck();		
	if (binteleave) {
		
		((CButton *) (GetDlgItem(IDC_IFACTOR)))->EnableWindow(TRUE);
		((CButton *) (GetDlgItem(IDC_INTERLEAVEFRAMES)))->EnableWindow(TRUE);
		((CButton *) (GetDlgItem(IDC_INTERLEAVESECONDS)))->EnableWindow(TRUE);

	}
	else {
		
		((CButton *) (GetDlgItem(IDC_IFACTOR)))->EnableWindow(FALSE);
		((CButton *) (GetDlgItem(IDC_INTERLEAVEFRAMES)))->EnableWindow(FALSE);
		((CButton *) (GetDlgItem(IDC_INTERLEAVESECONDS)))->EnableWindow(FALSE);

	}
	
}


void BuildLocalRecordingFormat() {
	
	m_FormatLocal.wFormatTag	= WAVE_FORMAT_PCM;		
	m_FormatLocal.wBitsPerSample = audio_bits_per_sampleLocal;
	m_FormatLocal.nSamplesPerSec = audio_samples_per_secondsLocal;
	m_FormatLocal.nChannels = audio_num_channelsLocal;
	m_FormatLocal.nBlockAlign = m_FormatLocal.nChannels * (m_FormatLocal.wBitsPerSample/8);
	m_FormatLocal.nAvgBytesPerSec = m_FormatLocal.nSamplesPerSec * m_FormatLocal.nBlockAlign;
	m_FormatLocal.cbSize = 0;

}


void SuggestLocalCompressFormat() {

	bAudioCompressionLocal = TRUE;

	AllocLocalCompressFormat();
	
	//1st try MPEGLAYER3
	BuildLocalRecordingFormat();		
	MMRESULT mmr;
	if ((m_FormatLocal.nSamplesPerSec == 22050) && (m_FormatLocal.nChannels==2) && (m_FormatLocal.wBitsPerSample <= 16)) {
		
		pwfxLocal->wFormatTag = WAVE_FORMAT_MPEGLAYER3;
		mmr = acmFormatSuggest(NULL, &m_FormatLocal,  pwfxLocal, cbwfxLocal, ACM_FORMATSUGGESTF_WFORMATTAG);	

	}
	
	if (mmr!=0) {
		
		//ver 1.6, use PCM if MP3 not available
		
		//Then try ADPCM
		//BuildLocalRecordingFormat();
		//pwfxLocal->wFormatTag = WAVE_FORMAT_ADPCM;
		//MMRESULT mmr = acmFormatSuggest(NULL, &m_FormatLocal,  pwfxLocal, cbwfxLocal, ACM_FORMATSUGGESTF_WFORMATTAG);


		if (mmr!=0) {

			//Use the PCM as default
			BuildLocalRecordingFormat();
			pwfxLocal->wFormatTag = WAVE_FORMAT_PCM;
			MMRESULT mmr = acmFormatSuggest(NULL, &m_FormatLocal,  pwfxLocal, cbwfxLocal, ACM_FORMATSUGGESTF_WFORMATTAG);


			if (mmr!=0) {

				bAudioCompressionLocal = FALSE;


			}


		}

	}

}


void AllocLocalCompressFormat() {

	
	if (pwfxLocal) {

		//Do nothing....already allocated

	}
	else {

		MMRESULT mmresult = acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &cbwfxLocal);
		if (MMSYSERR_NOERROR != mmresult)
		{

			CString msgstr;
			CString tstr;
			tstr.LoadString(IDS_STRING_NOTE);
			msgstr.Format("Metrics failed mmresult=%u!", mmresult);
            ::MessageBox(NULL,msgstr,tstr, MB_OK | MB_ICONEXCLAMATION);			
			return ;
		}

		if (cbwfxLocal < cbwfx) cbwfxLocal = cbwfx;


		pwfxLocal = (LPWAVEFORMATEX)GlobalAllocPtr(GHND, cbwfxLocal);
		if (NULL == pwfxLocal)
		{

			CString msgstr;
			CString tstr;
			tstr.LoadString(IDS_STRING_NOTE);
			msgstr.Format("GlobalAllocPtr(%lu) failed!", cbwfxLocal);
            ::MessageBox(NULL,msgstr,tstr, MB_OK | MB_ICONEXCLAMATION);			
			return ;
		}

		
	}

}



void AudioFormat::OnCancel() 
{
	// TODO: Add extra cleanup here
	if (pwfxLocal) {
		
		GlobalFreePtr(pwfxLocal);
		pwfxLocal = NULL;

	}
	
	CDialog::OnCancel();
}

void AudioFormat::OnVolume() 
{
	// Ver 1.1
	if (waveInGetNumDevs() == 0) {

		//CString msgstr;
		//msgstr.Format("Unable to detect audio input device. You need a sound card with microphone input.");
        //MessageBox(msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);
		MessageOut(this->m_hWnd,IDS_STRING_NOINPUT1 ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

		return;

	}
	
	CString launchPath("");
	CString testLaunchPath;
	CString exeFileName("\\sndvol32.exe");

	char dirx[300];
	GetWindowsDirectory(dirx,300);
	CString Windir(dirx);
	CString AppDir;
	CString SubDir;


	//Test Windows\sndvol32.exe
	AppDir = Windir;			
	SubDir = "";
	testLaunchPath=AppDir+SubDir+exeFileName;
	if (launchPath=="") {

		//Verify sndvol32.exe exists	
		OFSTRUCT ofs;
		HFILE hdir = OpenFile(testLaunchPath, &ofs,OF_EXIST);
		if (hdir != HFILE_ERROR) {

				launchPath=testLaunchPath;				
			
		}	
		
	}

	//Test Windows\system32\sndvol32.exe
	AppDir = Windir;			
	SubDir = "\\system32";
	testLaunchPath=AppDir+SubDir+exeFileName;
	if (launchPath=="") {

		//Verify sndvol32.exe exists	
		OFSTRUCT ofs;
		HFILE hdir = OpenFile(testLaunchPath, &ofs,OF_EXIST);
		if (hdir != HFILE_ERROR) {

				launchPath=testLaunchPath;
			
				//need CloseHandle ?
				//BOOL ret = CloseHandle((HANDLE) hdir);
				//if (!ret) MessageBox("Close handle Fails","Note",MB_OK | MB_ICONEXCLAMATION);		
			
		}	
		
	}

	
	//Test Windows\system\sndvol32.exe
	AppDir = Windir;			
	SubDir = "\\system32";
	testLaunchPath=AppDir+SubDir+exeFileName;
	if (launchPath=="") {

		//Verify sndvol32.exe exists	
		OFSTRUCT ofs;
		HFILE hdir = OpenFile(testLaunchPath, &ofs,OF_EXIST);
		if (hdir != HFILE_ERROR) {

				launchPath=testLaunchPath;			
		
		}	
		
	}


	if (launchPath!="") { //launch Volume Control	

		//not sure
		launchPath = launchPath + " /r /rec /record";

		if (WinExec(launchPath,SW_SHOW)!=0) {

				
		}
		else {
			//MessageBox("Error launching Volume Control!","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(this->m_hWnd,IDS_STRING_ERRVOLCTRL1 ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

		}	

	}

}

void AudioFormat::OnSelchangeInputdevice() 
{
	// TODO: Add your control notification handler code here


	int devID;
	devID = ((CComboBox *) (GetDlgItem(IDC_INPUTDEVICE)))->GetCurSel();
	if (devID < numdevice) {
		
		UpdateDeviceData(devicemap[devID],waveinselected,NULL);
		
	}
	
}


// ver 1.2
// =====================================
// UpdateDeviceData
//
// Update the user - interface based on the device data
//	
// If the third parameter (compressed format) is not null, we assume it is compatibile with the 2nd parameter (recording format)
//
// =====================================
void AudioFormat::UpdateDeviceData(UINT deviceID, DWORD curr_sel_rec_format, LPWAVEFORMATEX curr_sel_pwfx) {


	WAVEINCAPS pwic;
	MMRESULT mmr = waveInGetDevCaps( deviceID ,  &pwic, sizeof(pwic) ); 

	int selected_cindex=-1; //selected index of combo box	
	numformat=0; //counter, number of format


	//Reset Recording Format Combo Box and format map
	((CComboBox *) (GetDlgItem(IDC_COMBO1)))->ResetContent();
	numformat = 0;

	
	//This code works on the assumption (when filling in values for the interfaces)
	//that the  m_Format and pwfx formats (external variables) are already chosen correctly and compatibile with each other
	if ((pwic.dwFormats) & WAVE_FORMAT_1M08) {

		((CComboBox *) (GetDlgItem(IDC_COMBO1)))->AddString("11.025 kHz, mono, 8-bit");		
		

		formatmap[numformat]=WAVE_FORMAT_1M08;
		numformat++;
		
	}

	if ((pwic.dwFormats) & WAVE_FORMAT_1M16) {
		
		((CComboBox *) (GetDlgItem(IDC_COMBO1)))->AddString("11.025 kHz, mono, 16-bit");


		formatmap[numformat]=WAVE_FORMAT_1M16;
		numformat++;		


	}
	
	if ((pwic.dwFormats) & WAVE_FORMAT_1S08) {
		
		((CComboBox *) (GetDlgItem(IDC_COMBO1)))->AddString("11.025 kHz, stereo, 8-bit");

		
		formatmap[numformat]=WAVE_FORMAT_1S08;
		numformat++;		

	}

	if ((pwic.dwFormats) & WAVE_FORMAT_1S16) { 
	
		((CComboBox *) (GetDlgItem(IDC_COMBO1)))->AddString("11.025 kHz, stereo, 16-bit");
		
		formatmap[numformat]=WAVE_FORMAT_1S16;
		numformat++;		


	}


	if ((pwic.dwFormats) & WAVE_FORMAT_2M08) { 
	
		((CComboBox *) (GetDlgItem(IDC_COMBO1)))->AddString("22.05 kHz, mono, 8-bit");

		formatmap[numformat]=WAVE_FORMAT_2M08;
		numformat++;		

	}

	if ((pwic.dwFormats) & WAVE_FORMAT_2M16) { 
		
		((CComboBox *) (GetDlgItem(IDC_COMBO1)))->AddString("22.05 kHz, mono, 16-bit");


		formatmap[numformat]=WAVE_FORMAT_2M16;
		numformat++;		

	}

	
	if ((pwic.dwFormats) & WAVE_FORMAT_2S08)  {		

	
		((CComboBox *) (GetDlgItem(IDC_COMBO1)))->AddString("22.05 kHz, stereo, 8-bit");


		formatmap[numformat]=WAVE_FORMAT_2S08;
		numformat++;		


	}

	if ((pwic.dwFormats) & WAVE_FORMAT_2S16) { 
	
		((CComboBox *) (GetDlgItem(IDC_COMBO1)))->AddString("22.05 kHz, stereo, 16-bit");


		formatmap[numformat]=WAVE_FORMAT_2S16;
		numformat++;		

	}

	if ((pwic.dwFormats) & WAVE_FORMAT_4M08) { 
		
		((CComboBox *) (GetDlgItem(IDC_COMBO1)))->AddString("44.1 kHz, mono, 8-bit");


		formatmap[numformat]=WAVE_FORMAT_4M08;
		numformat++;		


	}

	if ((pwic.dwFormats) & WAVE_FORMAT_4M16) {
		
		((CComboBox *) (GetDlgItem(IDC_COMBO1)))->AddString("44.1 kHz, mono, 16-bit");


		formatmap[numformat]=WAVE_FORMAT_4M16;
		numformat++;		

	}

 	if ((pwic.dwFormats) & WAVE_FORMAT_4S08) { 
		
		((CComboBox *) (GetDlgItem(IDC_COMBO1)))->AddString("44.1 kHz, stereo, 8-bit");

		formatmap[numformat]=WAVE_FORMAT_4S08;
		numformat++;		

	}

	if ((pwic.dwFormats) & WAVE_FORMAT_4S16) {
	
		((CComboBox *) (GetDlgItem(IDC_COMBO1)))->AddString("44.1 kHz, stereo, 16-bit");

		formatmap[numformat]=WAVE_FORMAT_4S16;
		numformat++;		

	}

	
	if (numformat<=0) {
		
		((CComboBox *) (GetDlgItem(IDC_COMBO1)))->AddString("None Available");
		((CComboBox *) (GetDlgItem(IDC_COMBO1)))->SetCurSel(0);

		
		((CEdit *) (GetDlgItem(IDC_COMPRESSEDFORMATTAG)))->SetWindowText("None Available");		
		((CEdit *) (GetDlgItem(IDC_COMPRESSEDFORMAT)))->SetWindowText(" ");		

		((CButton *) (GetDlgItem(IDC_CHOOSE)))->EnableWindow(FALSE);

		//For this case, where no recording format, compressed format is available
		//is handled by OnOk (no external formats is updated) when the user press the OK button.		
		return;

	}
	else {

		((CButton *) (GetDlgItem(IDC_CHOOSE)))->EnableWindow(TRUE);

	}


	for (int k=0;k<numformat;k++) {
	
		if (curr_sel_rec_format == formatmap[k])
			selected_cindex=k;

	}
		
	
	//If can reach here ==> numformat > 0
	if ((selected_cindex==-1) && (numformat>0)) {	//selected recording format not found

		//force selection to one that is compatible
		selected_cindex=0;
		
		((CComboBox *) (GetDlgItem(IDC_COMBO1)))->SetCurSel(selected_cindex);
		
		//force selection of compress format
		OnSelchangeRecordformat() ;
	

	}
	else {

		//Compressed or Save format
		AllocLocalCompressFormat();
		if (curr_sel_pwfx==NULL) {			
		
			SuggestLocalCompressFormat();
		}
		else {		
			memcpy( (void *) pwfxLocal, (void *) curr_sel_pwfx, cbwfx );
		
		}

		UpdateLocalCompressFormatInterface();

		//will this following line call OnSelchangeRecordformat() ?
		if (selected_cindex>=0)
			((CComboBox *) (GetDlgItem(IDC_COMBO1)))->SetCurSel(selected_cindex);


	}

}

void AudioFormat::OnInterleaveframes() 
{
	// TODO: Add your control notification handler code here
	((CButton *) (GetDlgItem(IDC_INTERLEAVEFRAMES)))->SetCheck(TRUE);
	((CButton *) (GetDlgItem(IDC_INTERLEAVESECONDS)))->SetCheck(FALSE);
	
}

void AudioFormat::OnInterleaveseconds() 
{
	// TODO: Add your control notification handler code here
	((CButton *) (GetDlgItem(IDC_INTERLEAVEFRAMES)))->SetCheck(FALSE);
	((CButton *) (GetDlgItem(IDC_INTERLEAVESECONDS)))->SetCheck(TRUE);
	
}

void AudioFormat::OnSystemrecord() 
{
	// TODO: Add your control notification handler code here

	
}

void AudioFormat::OnHelp() 
{
	// TODO: Add your control notification handler code here
	CString progdir,helppath;
	progdir=GetProgPath();
	helppath= progdir + "\\help.htm#Helpmci";
	
	Openlink(helppath);

	
}


BOOL AudioFormat::Openlink (CString link)
{

  BOOL bSuccess = FALSE;

  //As a last resort try ShellExecuting the URL, may
  //even work on Navigator!
  if (!bSuccess)
    bSuccess = OpenUsingShellExecute (link);

  if (!bSuccess)
    bSuccess = OpenUsingRegisteredClass (link);
  return bSuccess;
}


BOOL AudioFormat::OpenUsingShellExecute (CString link)
{
  LPCTSTR mode;
  mode = _T ("open");

  //HINSTANCE hRun = ShellExecute (GetParent ()->GetSafeHwnd (), mode, m_sActualLink, NULL, NULL, SW_SHOW);
  HINSTANCE hRun = ShellExecute (GetSafeHwnd (), mode, link, NULL, NULL, SW_SHOW);
  if ((int) hRun <= HINSTANCE_ERROR)
    {
      TRACE ("Failed to invoke URL using ShellExecute\n");
      return FALSE;
    }
  return TRUE;
}


BOOL AudioFormat::OpenUsingRegisteredClass (CString link)
{
  TCHAR key[MAX_PATH + MAX_PATH];
  HINSTANCE result;

  if (GetRegKey (HKEY_CLASSES_ROOT, _T (".htm"), key) == ERROR_SUCCESS)
    {
      LPCTSTR mode;
      mode = _T ("\\shell\\open\\command");
      _tcscat (key, mode);
      if (GetRegKey (HKEY_CLASSES_ROOT, key, key) == ERROR_SUCCESS)
        {
          LPTSTR pos;
          pos = _tcsstr (key, _T ("\"%1\""));
          if (pos == NULL)
            {              // No quotes found

              pos = strstr (key, _T ("%1"));   // Check for %1, without quotes

              if (pos == NULL)     // No parameter at all...

                pos = key + _tcslen (key) - 1;
              else
                *pos = _T ('\0');  // Remove the parameter

            }
          else
            *pos = _T ('\0');  // Remove the parameter

          _tcscat (pos, _T (" "));
          _tcscat (pos, link);
          result = (HINSTANCE) WinExec (key, SW_SHOW);
          if ((int) result <= HINSTANCE_ERROR)
            {
              CString str;
              switch ((int) result)
                {
                case 0:
                  str = _T ("The operating system is out\nof memory or resources.");
                  break;
                case SE_ERR_PNF:
                  str = _T ("The specified path was not found.");
                  break;
                case SE_ERR_FNF:
                  str = _T ("The specified file was not found.");
                  break;
                case ERROR_BAD_FORMAT:
                  str = _T ("The .EXE file is invalid\n(non-Win32 .EXE or error in .EXE image).");
                  break;
                case SE_ERR_ACCESSDENIED:
                  str = _T ("The operating system denied\naccess to the specified file.");
                  break;
                case SE_ERR_ASSOCINCOMPLETE:
                  str = _T ("The filename association is\nincomplete or invalid.");
                  break;
                case SE_ERR_DDEBUSY:
                  str = _T ("The DDE transaction could not\nbe completed because other DDE transactions\nwere being processed.");
                  break;
                case SE_ERR_DDEFAIL:
                  str = _T ("The DDE transaction failed.");
                  break;
                case SE_ERR_DDETIMEOUT:
                  str = _T ("The DDE transaction could not\nbe completed because the request timed out.");
                  break;
                case SE_ERR_DLLNOTFOUND:
                  str = _T ("The specified dynamic-link library was not found.");
                  break;
                case SE_ERR_NOASSOC:
                  str = _T ("There is no application associated\nwith the given filename extension.");
                  break;
                case SE_ERR_OOM:
                  str = _T ("There was not enough memory to complete the operation.");
                  break;
                case SE_ERR_SHARE:
                  str = _T ("A sharing violation occurred.");
                  break;
                default:
                  str.Format (_T ("Unknown Error (%d) occurred."), (int) result);
                }
              str = _T ("Unable to open hyperlink:\n\n") + str;
              AfxMessageBox (str, MB_ICONEXCLAMATION | MB_OK);
            }
          else
            return TRUE;
        }
    }
  return FALSE;
}

  

LONG AudioFormat::GetRegKey (HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
  HKEY hkey;
  LONG retval = RegOpenKeyEx (key, subkey, 0, KEY_QUERY_VALUE, &hkey);

  if (retval == ERROR_SUCCESS)
    {
      long datasize = MAX_PATH;
      TCHAR data[MAX_PATH];
      RegQueryValue (hkey, NULL, data, &datasize);
      _tcscpy (retdata, data);
      RegCloseKey (hkey);
    }

  return retval;
}

