// RenderSoft CamStudio
//
// Copyright 2001 RenderSoft Software & Web Publishing

#include "stdafx.h"
#include "RecorderDoc.h"

IMPLEMENT_DYNCREATE(CRecorderDoc, CDocument)

BEGIN_MESSAGE_MAP(CRecorderDoc, CDocument)
END_MESSAGE_MAP()

CRecorderDoc::CRecorderDoc()
    : CDocument()
{
}

CRecorderDoc::~CRecorderDoc() = default;

BOOL CRecorderDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return false;

    return TRUE;
}

void CRecorderDoc::Serialize(CArchive & /*ar*/)
{
}

// CRecorderDoc diagnostics
#ifdef _DEBUG
void CRecorderDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CRecorderDoc::Dump(CDumpContext &dc) const
{
    CDocument::Dump(dc);
}

#endif //_DEBUG
