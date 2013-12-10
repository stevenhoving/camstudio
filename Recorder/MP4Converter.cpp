#include "StdAfx.h"
#include "MP4Converter.h"
#include "CamFile.h"

#define BUF_SIZE 4096

HANDLE m_gHandles[2];
ConversionResult CMP4Converter::m_ConvRes;

CMP4Converter::CMP4Converter(void) :
m_pData(NULL),
m_bConverting(true),
m_hThread(INVALID_HANDLE_VALUE)
{
	m_gHandles[0] = CreateEvent( 
		NULL,   // default security attributes
		FALSE,  // auto-reset event object
		FALSE,  // initial state is nonsignaled
		NULL);  // unnamed object

}

CMP4Converter::~CMP4Converter(void)
{
	if(m_pData)
	{
		if(m_pData->pdwPipeSize)
			delete m_pData->pdwPipeSize;
		if(m_pData->psCmdLine)
			delete m_pData->psCmdLine;
		if(m_pData->psExePath)
			delete m_pData->psExePath;
		if(m_pData->psInputFile)
			delete m_pData->psInputFile;
		if(m_pData->psLogFile)
			delete m_pData->psLogFile;
		if(m_pData->psOutputFile)
			delete m_pData->psOutputFile;
	}
}
bool CMP4Converter::ConvertAVItoMP4(
	const CString& sInputAVI, 
	const CString& sOutputMP4)
{
	bool bRet = false;
	DWORD dwThreadId;
	m_pData = (PCONVERTERDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CONVERTERDATA));
	if(m_pData)
	{
		m_pData->psInputFile = new CString(sInputAVI);
		m_pData->psOutputFile = new CString(sOutputMP4);
		CString sCmd;
		sCmd.Format(" -i \"%s\" -c:v libx264 -preset slow -crf 22 -c:a mp2 -b:a 128k -y \"%s\" -loglevel quiet", sInputAVI, sOutputMP4);
		m_pData->psCmdLine = new CString(sCmd);
		m_pData->pdwPipeSize = new DWORD(1048576); // 1mb
		m_pData->psExePath = new CString(GetProgPath() + "\\ffmpeg.exe");
		m_pData->psLogFile = new CString(GetProgPath() + "\\ffmpeg.log");

		m_hThread = CreateThread( 
				NULL,					// default security attributes
				0,						// use default stack size  
				ThreadProc,				// thread function name
				m_pData,				// argument to thread function 
				0,						// use default creation flags 
				&dwThreadId);			// returns the thread identifier 
		if(m_hThread)
			bRet = true;
	}
	return bRet;
}
 DWORD WINAPI CMP4Converter::ThreadProc(LPVOID lpParam)
{
	void *rdstdout = 0;
	void *wrstdout = 0;

	bool bRet = false;

	ConverterData* m_pData = (ConverterData*) lpParam;
	// Create a pipe for the child process's STDOUT. 
	SECURITY_ATTRIBUTES sa;

	// Set up the security attributes struct.
	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	if ( ! CreatePipe(&rdstdout, &wrstdout, &sa, *m_pData->pdwPipeSize)) 
		MessageBox(NULL,"CreatePipe failed", "Warning", MB_ICONEXCLAMATION); 

	PROCESS_INFORMATION pi;
	ZeroMemory( &pi, sizeof(pi) );
	
	STARTUPINFO si;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	si.hStdInput  = wrstdout;
	si.hStdOutput = wrstdout;
	si.hStdError  = wrstdout;
	si.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
 	// Start the FFMpeg process. 
	if( !CreateProcess(
		*m_pData->psExePath,			//module name
		m_pData->psCmdLine->GetBuffer(),// Command line
		NULL,							// Process handle not inheritable
		NULL,							// Thread handle not inheritable
		TRUE,							// Set handle inheritance to FALSE
		NULL,							// No console
		NULL,							// Use parent's environment block
		NULL,							// Use parent's starting directory 
		&si,							// Pointer to STARTUPINFO structure
		&pi )							// Pointer to PROCESS_INFORMATION structure
		) 
	{
		MessageBox(NULL, "Failed to start MP4 Converter" ,"Note",MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}
	DWORD dwRead; 
	CHAR buffer[BUF_SIZE];
	BOOL bSuccess = FALSE;
 
	BOOL bFlag;

	m_gHandles[1] = pi.hProcess;
	// Wait until FFMpeg process exits or cancel event is fired
	
	DWORD dWIndex = WaitForMultipleObjects(sizeof(m_gHandles) / sizeof(m_gHandles[0]), m_gHandles, FALSE, INFINITE );

	switch (dWIndex) 
	{

		case WAIT_OBJECT_0 + 0:
			{
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pi.dwProcessId);
				if(hProcess)
				{
					if(!TerminateProcess(hProcess, 0))
					{

					}
					m_ConvRes = CANCELLED;
				}
			}
			break; 

		case WAIT_OBJECT_0 + 1:
			{
				FILE *fp;
				int nSize = BUF_SIZE-1;

				fopen_s(&fp, *m_pData->psLogFile, "w");

				if (!CloseHandle(wrstdout))
					MessageBox(NULL,"Failed to close the write stdout pipe", "Warning", 0);

				bool bContinue = true;
				while(bContinue)
				{
					bFlag = ReadFile (rdstdout, buffer, nSize, &dwRead, NULL);
					if (bFlag == FALSE) 
					{
						bContinue = false;
					}
					else
					{
						buffer[dwRead] = 0;
						fwrite(buffer, 1, dwRead, fp);
					}
				}
				fclose(fp);
				m_ConvRes = SUCCESS;
			}
			break;
		default:
			m_ConvRes = FAILED;
			break;
	}
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	CloseHandle( rdstdout);
	return 0;
}

 bool CMP4Converter::Converting()
 {
	 DWORD dwExitCode;
	 GetExitCodeThread(m_hThread, &dwExitCode);
	 if(dwExitCode == STILL_ACTIVE)
		 return true;
	 else
		 return false;
 }

 void CMP4Converter::CancelConversion()
 {
	 SetEvent(m_gHandles[0]);
 }

 ConversionResult CMP4Converter::Status()
 {
	 return m_ConvRes;
 }

