// playplusDoc.cpp : implementation of the CPlayplusDoc class
//

#include "stdafx.h"
#include "playplus.h"

#include "playplusDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlayplusDoc

IMPLEMENT_DYNCREATE(CPlayplusDoc, CDocument)

BEGIN_MESSAGE_MAP(CPlayplusDoc, CDocument)
	//{{AFX_MSG_MAP(CPlayplusDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlayplusDoc construction/destruction

CPlayplusDoc::CPlayplusDoc()
{
	// TODO: add one-time construction code here

}

CPlayplusDoc::~CPlayplusDoc()
{
}

BOOL CPlayplusDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CPlayplusDoc serialization

void CPlayplusDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPlayplusDoc diagnostics

#ifdef _DEBUG
void CPlayplusDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPlayplusDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlayplusDoc commands
