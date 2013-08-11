#pragma once
class CMP4Converter
{
public:
	CMP4Converter(void);
	~CMP4Converter(void);
	bool ConvertAVItoMP4(
		const CString& sInputAVI, 
		const CString& sOutputMP4);
	bool Sucess();
	bool Converting();
private:

	static DWORD WINAPI ThreadProc(LPVOID lpParam);

	typedef struct ConverterData
	{
		CString *psInputFile;
		CString *psOutputFile;
		CString *psCmdLine;
		CString *psLogFile;
		CString *psExePath;
		DWORD *pdwPipeSize;
	} CONVERTERDATA, *PCONVERTERDATA;

	PCONVERTERDATA m_pData;
	HANDLE m_hThread;
	bool m_bSuccess;
	bool m_bConverting;

};

