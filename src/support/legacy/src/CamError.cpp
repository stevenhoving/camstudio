#include "stdafx.h"
#include <CamLib/CamError.h>

int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus)
{
    CString tstr("");
    CString mstr("");
    VERIFY(tstr.LoadString((UINT)strTitle));
    VERIFY(mstr.LoadString((UINT)strMsg));

    return ::MessageBox(hWnd, mstr, tstr, mbstatus);
}

int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val)
{
    CString tstr("");
    CString mstr("");
    CString fstr("");
    VERIFY(tstr.LoadString((UINT)strTitle));
    VERIFY(mstr.LoadString((UINT)strMsg));
    fstr.Format(mstr, val);

    return ::MessageBox(hWnd, fstr, tstr, mbstatus);
}

int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val1, long val2)
{
    CString tstr("");
    CString mstr("");
    CString fstr("");
    VERIFY(tstr.LoadString((UINT)strTitle));
    VERIFY(mstr.LoadString((UINT)strMsg));
    fstr.Format(mstr, val1, val2);

    return ::MessageBox(hWnd, fstr, tstr, mbstatus);
}
