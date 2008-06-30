// RenderSoft CamStudio
//
// Copyright 2001 RenderSoft Software & Web Publishing
// 
//
// vscapDoc.cpp : implementation of the CVscapDoc class
//

#include "stdafx.h"
#include "vscap.h"

#include "vscapDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVscapDoc

IMPLEMENT_DYNCREATE(CVscapDoc, CDocument)

BEGIN_MESSAGE_MAP(CVscapDoc, CDocument)
	//{{AFX_MSG_MAP(CVscapDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVscapDoc construction/destruction

CVscapDoc::CVscapDoc()
{
	// TODO: add one-time construction code here

}

CVscapDoc::~CVscapDoc()
{
}

BOOL CVscapDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CVscapDoc serialization

void CVscapDoc::Serialize(CArchive& ar)
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
// CVscapDoc diagnostics

#ifdef _DEBUG
void CVscapDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVscapDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVscapDoc commands
