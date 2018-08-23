#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include <CamLib/CamError.h>
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

    CRecorderApp(const CRecorderApp &) = delete;
    CRecorderApp &operator = (const CRecorderApp &) = delete;

public:
    BOOL InitInstance() override;
    int ExitInstance() override;

    // Implementation

    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
private:
    HANDLE m_hAppMutex;

    std::unique_ptr<gdi_plus> m_gdi;

    bool RegisterWindowClass();
    BOOL FirstInstance();
};
