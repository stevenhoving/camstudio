#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include <CamLib/CamError.h>
#include "CamStudioCommandLineInfo.h"
#include "Profile.h"
#include "resource.h"                   // main symbols
#include "RecorderVersionReleaseInfo.h" // Contains version and SVN release number
#include <memory>

// Multilanguage
#define ENT_LANGID _T("LanguageID")
#define ENT_LANGINI _T("LangINI")
#define SEC_SETTINGS _T("Language")
#define STANDARD_LANGID 0x09 // English

class gdi_plus;

class CRecorderApp : public CWinApp
{
public:
    CRecorderApp();
    ~CRecorderApp() override;

    LANGID LanguageID() const
    {
        return m_wCurLangID;
    }
    LANGID LanguageID(LANGID wLangID)
    {
        m_wCurLangID = wLangID;
        // FIXME:        VERIFY(m_cmSettings.Write(LANGUAGE, m_wCurLangID));
        return m_wCurLangID;
    }
    int VersionOp() const
    {
        return m_iVersionOp;
    }

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CRecorderApp)
public:
    BOOL InitInstance() override;
    int ExitInstance() override;
    //}}AFX_VIRTUAL

    // Implementation

    //{{AFX_MSG(CRecorderApp)
    afx_msg void OnAppAbout();
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
private:
    static const TCHAR *CAMSTUDIO_MUTEX;
    HANDLE m_hAppMutex;
    LANGID m_wCurLangID;
    int m_iVersionOp;
    CCamStudioCommandLineInfo m_cmdInfo;

    std::unique_ptr<gdi_plus> m_gdi;

    bool RegisterWindowClass();
    BOOL FirstInstance();
    bool LoadLanguage(LANGID LangID);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
