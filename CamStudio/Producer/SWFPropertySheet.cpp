// SWFPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "playplus.h"
#include "SWFPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SWFPropertySheet

IMPLEMENT_DYNAMIC(SWFPropertySheet, CPropertySheet)

SWFPropertySheet::SWFPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

SWFPropertySheet::SWFPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

SWFPropertySheet::~SWFPropertySheet()
{
}


BEGIN_MESSAGE_MAP(SWFPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(SWFPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SWFPropertySheet message handlers
