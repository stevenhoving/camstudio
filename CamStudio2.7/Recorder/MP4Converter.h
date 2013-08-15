#pragma once

enum ConversionResult
{
	SUCCESS = 0,
	CANCELLED = 1,
	FAILED = 2
};
class CMP4Converter
{
public:
	CMP4Converter(void);
	~CMP4Converter(void);
	bool ConvertAVItoMP4(
		const CString& sInputAVI, 
		const CString& sOutputMP4);
	bool Success();
	bool Converting();
	void CancelConversion();
	ConversionResult Status();

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
	//static bool m_bSuccess;
	bool m_bConverting;
	static ConversionResult m_ConvRes;
};

