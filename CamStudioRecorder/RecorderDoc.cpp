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
    , m_uFrameWidth(0)
    , m_uFrameHeight(0)
{
}

CRecorderDoc::~CRecorderDoc()
{
}

BOOL CRecorderDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return false;

    // TODO: add reinitialization code here
    // (SDI documents will reuse this document)

    return TRUE;
}

void CRecorderDoc::Serialize(CArchive &ar)
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

unsigned int CRecorderDoc::FrameWidth(unsigned int uWidth)
{
    return m_uFrameWidth = uWidth;
}

unsigned int CRecorderDoc::FrameWidth() const
{
    return m_uFrameWidth;
}

unsigned int CRecorderDoc::FrameHeight(unsigned int uHeight)
{
    return m_uFrameHeight = uHeight;
}

unsigned int CRecorderDoc::FrameHeight() const
{
    return m_uFrameHeight;
}
