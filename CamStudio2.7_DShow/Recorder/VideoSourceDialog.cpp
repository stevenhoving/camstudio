// VideoSourceDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Recorder.h"
#include "VideoSourceDialog.h"
#include "afxdialogex.h"


// CVideoSourceDialog dialog

IMPLEMENT_DYNAMIC(CVideoSourceDialog, CDialogEx)

CVideoSourceDialog::CVideoSourceDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVideoSourceDialog::IDD, pParent)
{

}

CVideoSourceDialog::~CVideoSourceDialog()
{
}

void CVideoSourceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CVideoSourceDialog, CDialogEx)
END_MESSAGE_MAP()


// CVideoSourceDialog message handlers
