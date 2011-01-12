// NewShape.cpp : implementation file
//

#include "stdafx.h"
#include "Recorder.h"
#include "NewShape.h"
#include "MainFrm.h"			// for maxxScreen, maxyScreen
#include "CStudioLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// TODO : Some Magic numbers here, put them in a haeder file or explain 200 and 150
int iNewShapeWidth = 200;
int iNewShapeHeight = 150;
CString strNewShapeText("Right Click to Edit Text");
CString strImageFilename("");
int iImageType = 0;

CString shapeName("Label_");
CString shapeStr;

CString proposedShapeStr;

extern void AdjustShapeName(CString& shapeName);

/////////////////////////////////////////////////////////////////////////////
// CNewShapeDlg dialog

CNewShapeDlg::CNewShapeDlg(CWnd* pParent /*=NULL*/)
: CDialog(CNewShapeDlg::IDD, pParent)
, m_imageDir(GetMyVideoPath())
, m_uImageWidth(iNewShapeWidth)
, m_uImageHeight(iNewShapeHeight)
{
	//{{AFX_DATA_INIT(CNewShapeDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CNewShapeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewShapeDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_NAME, m_ctrlEditName);
	DDX_Control(pDX, IDC_IMAGEFILETEXT, m_ctrlTextFileText);
	DDX_Control(pDX, IDC_RADIO1, m_ctrlButtoBlankImage);
	DDX_Control(pDX, IDC_RADIO2, m_ctrlButtoImageFile);
	DDX_Control(pDX, IDC_BUTTON1, m_ctrlButtonImageFile);
	DDX_Control(pDX, IDC_STATICWIDTH, m_ctrlStaticWidth);
	DDX_Control(pDX, IDC_STATICHEIGHT, m_ctrlStaticHeight);
	DDX_Control(pDX, IDC_WIDTH, m_ctrlEditWidth);
	DDX_Control(pDX, IDC_HEIGHT, m_ctrlEditHeight);
	DDX_Control(pDX, IDC_EDIT3, m_ctrlEditShaepText);
	DDX_Text(pDX, IDC_WIDTH, m_uImageWidth);
	DDX_Text(pDX, IDC_HEIGHT, m_uImageHeight);
	DDV_MinMaxUInt(pDX, m_uImageWidth, 20, maxxScreen-1);
	DDV_MinMaxUInt(pDX, m_uImageHeight, 20, maxyScreen-1);
}

BEGIN_MESSAGE_MAP(CNewShapeDlg, CDialog)
	//{{AFX_MSG_MAP(CNewShapeDlg)
	ON_BN_CLICKED(IDC_RADIO1, OnClickBlankImage)
	ON_BN_CLICKED(IDC_RADIO2, OnClickImageFile)
	ON_BN_CLICKED(IDC_BUTTON1, OnFindImageFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewShapeDlg message handlers

BOOL CNewShapeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (strImageFilename)
		strImageFilename = GetProgPath() + "\\DIALOG.BMP";

	m_ctrlTextFileText.SetWindowText(strImageFilename);

	shapeStr.Format(TEXT("%d"), iShapeNameInt);
	shapeStr = shapeName + shapeStr;
	m_ctrlEditName.SetWindowText(shapeStr);

	proposedShapeStr = shapeStr;

	// TODO: Add extra initialization here
	HICON loadFileIcon= LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON1));
	m_ctrlButtonImageFile.SetIcon(loadFileIcon);

	m_ctrlButtoBlankImage.SetCheck((0 == iImageType) ? BST_CHECKED : BST_UNCHECKED);
	//m_ctrlButtoImageFile.SetCheck((0 == iImageType) ? BST_UNCHECKED : BST_CHECKED);	// redundant

	m_ctrlStaticWidth.EnableWindow((0 == iImageType));
	m_ctrlStaticHeight.EnableWindow((0 == iImageType));
	m_ctrlEditWidth.EnableWindow((0 == iImageType));
	m_ctrlEditHeight.EnableWindow((0 == iImageType));

	m_ctrlTextFileText.EnableWindow((0 != iImageType));
	m_ctrlButtonImageFile.EnableWindow((0 != iImageType));

	m_ctrlEditShaepText.SetWindowText(strNewShapeText);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CNewShapeDlg::OnClickBlankImage()
{
	//m_ctrlButtoBlankImage.SetCheck(BST_CHECKED);	// redundant
	//m_ctrlButtoImageFile.SetCheck(BST_UNCHECKED);	// redundant

	m_ctrlStaticWidth.EnableWindow(TRUE);
	m_ctrlStaticHeight.EnableWindow(TRUE);
	m_ctrlEditWidth.EnableWindow(TRUE);
	m_ctrlEditHeight.EnableWindow(TRUE);

	m_ctrlButtonImageFile.EnableWindow(FALSE);
	m_ctrlTextFileText.EnableWindow(FALSE);
}

void CNewShapeDlg::OnClickImageFile()
{
	m_ctrlStaticWidth.EnableWindow(FALSE);
	m_ctrlStaticHeight.EnableWindow(FALSE);
	m_ctrlEditWidth.EnableWindow(FALSE);
	m_ctrlEditHeight.EnableWindow(FALSE);

	m_ctrlButtonImageFile.EnableWindow(TRUE);
	m_ctrlTextFileText.EnableWindow(TRUE);
}

void CNewShapeDlg::OnOK()
{
	int oldWidth = iNewShapeWidth;
	int oldHeight = iNewShapeHeight;

	int val = m_ctrlButtoBlankImage.GetCheck();
	iImageType = (val) ? 0 : 1;
	if (!iImageType) {
		UpdateData();
		iNewShapeWidth = m_uImageWidth;
		iNewShapeHeight = m_uImageHeight;

		// TODO: numeric validation should be redundant due to DDV macros
		if (iNewShapeWidth < 20) {
			MessageOut(NULL,IDS_STRINGWIDTHLESS20,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			iNewShapeWidth = oldWidth;
			return;
		}

		if (maxxScreen < iNewShapeWidth) {
			// "Width cannot be larger than maxxScreen"
			MessageOut(NULL,IDS_STRINGWIDTHLARGER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, maxxScreen);
			iNewShapeWidth = oldWidth;
			return;
		}

		if (iNewShapeHeight < 20) {
			// "Height cannot be less than 20"
			MessageOut(NULL,IDS_STRINGHEIGHTLESS20,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			iNewShapeHeight = oldHeight;
			return;
		}

		if (maxyScreen < iNewShapeHeight) {
			// "Height cannot be larger than maxyScreen
			MessageOut(NULL,IDS_STRINGHEIGHTLARGER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION,maxyScreen);
			iNewShapeHeight = oldHeight;
			return;
		}
	}

	m_ctrlTextFileText.GetWindowText(strImageFilename);
	strImageFilename.TrimLeft();
	strImageFilename.TrimRight();
	if (1 == iImageType) {
		if (strImageFilename == "") {
			MessageOut(NULL,IDS_STRINGINVIMGFILE,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			return;
		}
	}

	m_ctrlEditShaepText.GetWindowText(strNewShapeText);
	m_ctrlEditName.GetWindowText(shapeStr);

	if (proposedShapeStr != shapeStr) {
		//shape name has been changed, reset counter to 1
		shapeName = shapeStr;

		//a better method is to extract the trailing number from shapestr and use it as number for iShapeNameInt
		//iShapeNameInt = 1;
		AdjustShapeName(shapeName);
	} else {
		// do not exceed range.
		iShapeNameInt = (iShapeNameInt < (INT_MAX - 1))
			? iShapeNameInt++
			: 1;
	}

	CDialog::OnOK();
}

void CNewShapeDlg::OnFindImageFile()
{
	const TCHAR * const pszTitle = _T("Load Picture");
	const TCHAR * const pszDefExt = _T("*.bmp; *.jpg; *.gif");
	const TCHAR * const pszFileName = _T("*.bmp; *.jpg; *.gif");
	const TCHAR * const pszFilter = _T("Picture Files (*.bmp; *.jpg; *.gif)|*.bmp; *.jpg; *.gif||");
	CFileDialog fdlg(TRUE, pszDefExt, pszFileName, OFN_LONGNAMES | OFN_FILEMUSTEXIST, pszFilter, this);
	fdlg.m_ofn.lpstrTitle = pszTitle;
	fdlg.m_ofn.lpstrInitialDir = m_imageDir;
	if (IDOK == fdlg.DoModal())
	{
		CString strNewFileTitle = fdlg.GetPathName();
		m_ctrlTextFileText.SetWindowText(strNewFileTitle);

		strNewFileTitle = strNewFileTitle.Left(strNewFileTitle.ReverseFind('\\'));
		m_imageDir = strNewFileTitle;
	}
}
