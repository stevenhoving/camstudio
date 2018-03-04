#include "stdafx.h"
#include "Recorder.h"
#include "NewShape.h"
#include "MainFrm.h" // for maxxScreen, maxyScreen
#include <CamLib/CStudioLib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// TODO : Some Magic numbers here, put them in a haeder file or explain 200 and 150
int g_iNewShapeWidth = 200;
int g_iNewShapeHeight = 150;
CString g_strNewShapeText("Right Click to Edit Text");
CString g_strImageFilename("");
int g_iImageType = 0;

CString g_shapeName("Label_");
CString g_shapeStr;

CString proposedShapeStr;

extern void AdjustShapeName(CString &shapeName);

/////////////////////////////////////////////////////////////////////////////
// CNewShapeDlg dialog

CNewShapeDlg::CNewShapeDlg(CWnd *pParent /*=nullptr*/)
    : CDialog(CNewShapeDlg::IDD, pParent)
    , m_imageDir(GetMyVideoPath())
    , m_uImageWidth(g_iNewShapeWidth)
    , m_uImageHeight(g_iNewShapeHeight)
{
    //{{AFX_DATA_INIT(CNewShapeDlg)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

void CNewShapeDlg::DoDataExchange(CDataExchange *pDX)
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
    DDV_MinMaxUInt(pDX, m_uImageWidth, 20, maxxScreen - 1);
    DDV_MinMaxUInt(pDX, m_uImageHeight, 20, maxyScreen - 1);
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

    if (g_strImageFilename)
        g_strImageFilename = GetProgPath() + "\\DIALOG.BMP";

    m_ctrlTextFileText.SetWindowText(g_strImageFilename);

    g_shapeStr.Format(TEXT("%d"), iShapeNameInt);
    g_shapeStr = g_shapeName + g_shapeStr;
    m_ctrlEditName.SetWindowText(g_shapeStr);

    proposedShapeStr = g_shapeStr;

    // TODO: Add extra initialization here
    HICON loadFileIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON1));
    m_ctrlButtonImageFile.SetIcon(loadFileIcon);

    m_ctrlButtoBlankImage.SetCheck((0 == g_iImageType) ? BST_CHECKED : BST_UNCHECKED);
    // m_ctrlButtoImageFile.SetCheck((0 == iImageType) ? BST_UNCHECKED : BST_CHECKED);    // redundant

    m_ctrlStaticWidth.EnableWindow((0 == g_iImageType));
    m_ctrlStaticHeight.EnableWindow((0 == g_iImageType));
    m_ctrlEditWidth.EnableWindow((0 == g_iImageType));
    m_ctrlEditHeight.EnableWindow((0 == g_iImageType));

    m_ctrlTextFileText.EnableWindow((0 != g_iImageType));
    m_ctrlButtonImageFile.EnableWindow((0 != g_iImageType));

    m_ctrlEditShaepText.SetWindowText(g_strNewShapeText);

    return TRUE; // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewShapeDlg::OnClickBlankImage()
{
    // m_ctrlButtoBlankImage.SetCheck(BST_CHECKED);    // redundant
    // m_ctrlButtoImageFile.SetCheck(BST_UNCHECKED);    // redundant

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
    int oldWidth = g_iNewShapeWidth;
    int oldHeight = g_iNewShapeHeight;

    int val = m_ctrlButtoBlankImage.GetCheck();
    g_iImageType = (val) ? 0 : 1;
    if (!g_iImageType)
    {
        UpdateData();
        g_iNewShapeWidth = m_uImageWidth;
        g_iNewShapeHeight = m_uImageHeight;

        // TODO: numeric validation should be redundant due to DDV macros
        if (g_iNewShapeWidth < 20)
        {
            MessageOut(nullptr, IDS_STRINGWIDTHLESS20, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
            g_iNewShapeWidth = oldWidth;
            return;
        }

        if (maxxScreen < g_iNewShapeWidth)
        {
            // "Width cannot be larger than maxxScreen"
            MessageOut(nullptr, IDS_STRINGWIDTHLARGER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, maxxScreen);
            g_iNewShapeWidth = oldWidth;
            return;
        }

        if (g_iNewShapeHeight < 20)
        {
            // "Height cannot be less than 20"
            MessageOut(nullptr, IDS_STRINGHEIGHTLESS20, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
            g_iNewShapeHeight = oldHeight;
            return;
        }

        if (maxyScreen < g_iNewShapeHeight)
        {
            // "Height cannot be larger than maxyScreen
            MessageOut(nullptr, IDS_STRINGHEIGHTLARGER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, maxyScreen);
            g_iNewShapeHeight = oldHeight;
            return;
        }
    }

    m_ctrlTextFileText.GetWindowText(g_strImageFilename);
    g_strImageFilename.TrimLeft();
    g_strImageFilename.TrimRight();
    if (1 == g_iImageType)
    {
        if (g_strImageFilename == "")
        {
            MessageOut(nullptr, IDS_STRINGINVIMGFILE, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
            return;
        }
    }

    m_ctrlEditShaepText.GetWindowText(g_strNewShapeText);
    m_ctrlEditName.GetWindowText(g_shapeStr);

    if (proposedShapeStr != g_shapeStr)
    {
        // shape name has been changed, reset counter to 1
        g_shapeName = g_shapeStr;

        // a better method is to extract the trailing number from shapestr and use it as number for iShapeNameInt
        // iShapeNameInt = 1;
        AdjustShapeName(g_shapeName);
    }
    else
    {
        // do not exceed range.
        iShapeNameInt = (iShapeNameInt < (INT_MAX - 1)) ? iShapeNameInt++ : 1;
    }

    CDialog::OnOK();
}

void CNewShapeDlg::OnFindImageFile()
{
    const TCHAR *const pszTitle = _T("Load Picture");
    const TCHAR *const pszDefExt = _T("*.bmp; *.jpg; *.gif");
    const TCHAR *const pszFileName = _T("*.bmp; *.jpg; *.gif");
    const TCHAR *const pszFilter = _T("Picture Files (*.bmp; *.jpg; *.gif)|*.bmp; *.jpg; *.gif||");
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
