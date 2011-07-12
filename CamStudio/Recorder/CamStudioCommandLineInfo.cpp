//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Recorder.h"
#include "CamStudioCommandLineInfo.h"
#include <tchar.h>

CCamStudioCommandLineInfo::CCamStudioCommandLineInfo(void)
: m_iCodecID(0)
, m_csOutfile(_T(""))
, m_iSeconds(0)
, m_bHelp(false)
, m_bStealth(false)
, m_bStart(false)
, m_bStop(false)
, m_bCodecs(false)
, m_iX(-1)
, m_iY(-1)
, m_iWidth(-1)
, m_iHeight(-1)
{
}

CCamStudioCommandLineInfo::~CCamStudioCommandLineInfo(void)
{
}

// CamStudio [-codec <codec ID>][-outfile <filename>][-seconds <seconds>][-help]
//
//"-codec <ID>: which codec to use"
//"-outfile <filename>: .avi file to write to"
//"-seconds <seconds>: how many seconds to record for ('0' means to record until a key is pressed)"
//"-help: this screen"
//
void CCamStudioCommandLineInfo::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
	static bool bIsArg = false;
	static int nArgIndex = 0;
	if (!bFlag && bIsArg) {
		CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
		return;
	}

	if (bFlag) {
		CString csArg(pszParam);
		if (csArg == _T("codec")) {
			TRACE("CCamStudioCommandLineInfo::ParseParam: -codec\n");
			nArgIndex = 1;
		} else if (csArg == _T("outfile")) {
			TRACE("CCamStudioCommandLineInfo::ParseParam: -outfile\n");
			nArgIndex = 2;
		} else if (csArg == _T("seconds")) {
			TRACE("CCamStudioCommandLineInfo::ParseParam: -seconds\n");
			nArgIndex = 3;
 		} else if (csArg == _T("x")) {
 			TRACE("CCamStudioCommandLineInfo::ParseParam: -x\n");
 			nArgIndex = 4;
 		} else if (csArg == _T("y")) {
 			TRACE("CCamStudioCommandLineInfo::ParseParam: -y\n");
 			nArgIndex = 5;
 		} else if (csArg == _T("width")) {
 			TRACE("CCamStudioCommandLineInfo::ParseParam: -width\n");
 			nArgIndex = 6;
 		} else if (csArg == _T("height")) {
 			TRACE("CCamStudioCommandLineInfo::ParseParam: -height\n");
 			nArgIndex = 7;
 		} else if (csArg == _T("stealth")) {
 			TRACE("CCamStudioCommandLineInfo::ParseParam: -stealth\n");
 			m_bStealth = true;
 			nArgIndex = 0;	// takes no arguments
 		} else if (csArg == _T("start")) {
 			TRACE("CCamStudioCommandLineInfo::ParseParam: -start\n");
 			m_bStart = true;
 			nArgIndex = 0;	// takes no arguments
 		} else if (csArg == _T("stop")) {
 			TRACE("CCamStudioCommandLineInfo::ParseParam: -stop\n");
 			m_bStop = true;
 			nArgIndex = 0;	// takes no arguments
 		} else if (csArg == _T("codecs")) {
 			TRACE("CCamStudioCommandLineInfo::ParseParam: -codecs\n");
 			m_bCodecs = true;
 			nArgIndex = 0;	// takes no arguments
 		} else if (csArg == _T("cfgfile")) {
 			TRACE("CCamStudioCommandLineInfo::ParseParam: -cfgfile\n");
 			nArgIndex = 8;
 		} else if (csArg == _T("loadcodec")) {
 			TRACE("CCamStudioCommandLineInfo::ParseParam: -loadcodec\n");
 			nArgIndex = 9;
		} else if (csArg == _T("help")) {
			TRACE("CCamStudioCommandLineInfo::ParseParam: -help\n");
			m_bHelp = true;
			nArgIndex = 0;	// takes no arguments
		} else {
			nArgIndex = 0;	// not our flag
			CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
		}
	} else {
		// process argument
		// TODO: add validation where needed
		switch (nArgIndex) {
		default:
		case 0:
			// not our argument
			CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
			break;
		case 1:	// [-codec <codec ID>]
			// integer argument
			m_iCodecID = ::_tstoi(pszParam);
			break;
		case 2:	// [-outfile <filename>]
			// string argument
			m_csOutfile = pszParam;
			break;
		case 3:	// [-seconds <seconds>]
			// integer argument
			m_iSeconds = ::_tstoi(pszParam);
			break;
		case 4:
 			// integer argument
 			m_iX = ::_tstoi(pszParam);
 			break;
 		case 5:
 			// integer argument
 			m_iY = ::_tstoi(pszParam);
 			break;
 		case 6:
 			// integer argument
 			m_iWidth = ::_tstoi(pszParam);
 			break;
 		case 7:
 			// integer argument
 			m_iHeight = ::_tstoi(pszParam);
 			break;
 		case 8:	// [-cfgfile <config file>]
 			// string argument
 			m_csCfgfile = pszParam;
 			break;
 		case 9:	// [-loadcodec <codec dll>]
 			// string argument
 			m_csLoadcodec = pszParam;
 			break;
		}
	}
}