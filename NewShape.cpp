// NewShape.cpp : implementation file
//

#include "stdafx.h"
#include "vscap.h"
#include "NewShape.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
  
int m_newShapeWidth = 200;
int m_newShapeHeight = 150;
CString m_newShapeText("Right Click to Edit Text");
CString m_imageDir("");
CString m_imageFilename("");
int m_imagetype = 0;

CString shapeName("Label_");
int shapeNameInt = 1;
CString shapeStr; 

CString proposedShapeStr; 

extern int maxxScreen;
extern int maxyScreen;
extern CString GetProgPath();
extern void AdjustShapeName(CString& shapeName);

extern int MessageOutINT(HWND hWnd,long strMsg, long strTitle, UINT mbstatus,long val);
extern int MessageOutINT2(HWND hWnd,long strMsg, long strTitle, UINT mbstatus,long val1,long val2);
extern int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus);


/////////////////////////////////////////////////////////////////////////////
// CNewShape dialog


CNewShape::CNewShape(CWnd* pParent /*=NULL*/)
	: CDialog(CNewShape::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewShape)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CNewShape::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewShape)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewShape, CDialog)
	//{{AFX_MSG_MAP(CNewShape)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewShape message handlers

BOOL CNewShape::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (m_imageDir=="")
		m_imageDir = GetProgPath();

	if (m_imageFilename)
		m_imageFilename = GetProgPath() + "\\DIALOG.BMP";

	((CEdit *) GetDlgItem(IDC_IMAGEFILETEXT))->SetWindowText(m_imageFilename);

	
	shapeStr.Format("%d",shapeNameInt);
	shapeStr = shapeName + shapeStr;	
	((CEdit *) GetDlgItem(IDC_NAME))->SetWindowText(shapeStr);

	proposedShapeStr = shapeStr;
	//((CEdit *) GetDlgItem(IDC_NAME))->SetSel( 0, 0, FALSE );
	//((CEdit *) GetDlgItem(IDC_NAME))->SetFocus();
	
	// TODO: Add extra initialization here
	HICON loadFileIcon= LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON1));
	((CButton *) GetDlgItem(IDC_BUTTON1))->SetIcon(loadFileIcon);
	
	if (m_imagetype == 0) {

		((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(1);
		((CButton *) GetDlgItem(IDC_RADIO2))->SetCheck(0);

		((CStatic *) GetDlgItem(IDC_STATICWIDTH))->EnableWindow(TRUE);
		((CStatic *) GetDlgItem(IDC_STATICHEIGHT))->EnableWindow(TRUE);
		((CEdit *) GetDlgItem(IDC_WIDTH))->EnableWindow(TRUE);
		((CEdit *) GetDlgItem(IDC_HEIGHT))->EnableWindow(TRUE);

		((CEdit *) GetDlgItem(IDC_IMAGEFILETEXT))->EnableWindow(FALSE);
		((CEdit *) GetDlgItem(IDC_BUTTON1))->EnableWindow(FALSE);

	}
	else 
	{

		((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(0);
		((CButton *) GetDlgItem(IDC_RADIO2))->SetCheck(1);

		((CStatic *) GetDlgItem(IDC_STATICWIDTH))->EnableWindow(FALSE);
		((CStatic *) GetDlgItem(IDC_STATICHEIGHT))->EnableWindow(FALSE);
		((CEdit *) GetDlgItem(IDC_WIDTH))->EnableWindow(FALSE);
		((CEdit *) GetDlgItem(IDC_HEIGHT))->EnableWindow(FALSE);

		((CEdit *) GetDlgItem(IDC_IMAGEFILETEXT))->EnableWindow(TRUE);
		((CEdit *) GetDlgItem(IDC_BUTTON1))->EnableWindow(TRUE);


	}

	CString widthStr, heightStr;
	widthStr.Format("%d",m_newShapeWidth);
	heightStr.Format("%d",m_newShapeHeight);
	((CEdit *) GetDlgItem(IDC_WIDTH))->SetWindowText(widthStr);
	((CEdit *) GetDlgItem(IDC_HEIGHT))->SetWindowText(heightStr);
	((CEdit *) GetDlgItem(IDC_EDIT3))->SetWindowText(m_newShapeText);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewShape::OnRadio1() 
{
	// TODO: Add your control notification handler code here
	((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(1);
	((CButton *) GetDlgItem(IDC_RADIO2))->SetCheck(0);

	((CStatic *) GetDlgItem(IDC_STATICWIDTH))->EnableWindow(TRUE);
	((CStatic *) GetDlgItem(IDC_STATICHEIGHT))->EnableWindow(TRUE);
	((CEdit *) GetDlgItem(IDC_WIDTH))->EnableWindow(TRUE);
	((CEdit *) GetDlgItem(IDC_HEIGHT))->EnableWindow(TRUE);

	((CEdit *) GetDlgItem(IDC_IMAGEFILETEXT))->EnableWindow(FALSE);
	((CEdit *) GetDlgItem(IDC_BUTTON1))->EnableWindow(FALSE);

	
}

void CNewShape::OnRadio2() 
{
	// TODO: Add your control notification handler code here
	((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(0);
	((CButton *) GetDlgItem(IDC_RADIO2))->SetCheck(1);

	((CStatic *) GetDlgItem(IDC_STATICWIDTH))->EnableWindow(FALSE);
	((CStatic *) GetDlgItem(IDC_STATICHEIGHT))->EnableWindow(FALSE);
	((CEdit *) GetDlgItem(IDC_WIDTH))->EnableWindow(FALSE);
	((CEdit *) GetDlgItem(IDC_HEIGHT))->EnableWindow(FALSE);

	((CEdit *) GetDlgItem(IDC_IMAGEFILETEXT))->EnableWindow(TRUE);
	((CEdit *) GetDlgItem(IDC_BUTTON1))->EnableWindow(TRUE);
	
}


void CNewShape::OnOK() 
{
	// TODO: Add extra validation here

	int oldWidth = m_newShapeWidth;
	int oldHeight = m_newShapeHeight;

	int val = ((CButton *) GetDlgItem(IDC_RADIO1))->GetCheck();
	if (val)
		m_imagetype = 0;
	else
		m_imagetype = 1;

	if (m_imagetype==0) {

			CString widthStr, heightStr;

			((CEdit *) GetDlgItem(IDC_WIDTH))->GetWindowText(widthStr);
			((CEdit *) GetDlgItem(IDC_HEIGHT))->GetWindowText(heightStr);
			sscanf(LPCTSTR(widthStr),"%d",&m_newShapeWidth);
			sscanf(LPCTSTR(heightStr),"%d",&m_newShapeHeight);	


			if (m_newShapeWidth<20) {
				MessageOut(NULL,IDS_STRINGWIDTHLESS20,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);				
				m_newShapeWidth  = oldWidth; 
				return;

			}

			if (m_newShapeWidth>maxxScreen) {
				
				//CString msgstr;
				//msgstr.Format("Width cannot be larger than %d",maxxScreen);
				//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOutINT(NULL,IDS_STRINGWIDTHLARGER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION,maxxScreen);
				m_newShapeWidth  = oldWidth; 
				return;

			}

			if (m_newShapeHeight<20) {

				//MessageBox("Height cannot be less than 20","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(NULL,IDS_STRINGHEIGHTLESS20,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);				
				m_newShapeHeight  = oldHeight; 
				return;

			}

			if (m_newShapeHeight>maxyScreen) {
				
				//CString msgstr;
				//msgstr.Format("Height cannot be larger than %d",maxyScreen);
				//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
				
				MessageOutINT(NULL,IDS_STRINGHEIGHTLARGER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION,maxyScreen);
				m_newShapeHeight  = oldHeight; 
				return;

			}

	}
	


	((CEdit *) GetDlgItem(IDC_IMAGEFILETEXT))->GetWindowText(m_imageFilename);
	m_imageFilename.TrimLeft();
	m_imageFilename.TrimRight();
	
	if (m_imagetype == 1) {

		if (m_imageFilename == "") 
		{

			MessageOut(NULL,IDS_STRINGINVIMGFILE,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			return;

		}

	}
	
	((CEdit *) GetDlgItem(IDC_EDIT3))->GetWindowText(m_newShapeText);	
	//m_newShapeText.TrimLeft();
	//m_newShapeText.TrimRight();
	//if (m_imageFilename=="") {

	//	MessageBox("Shape name is empty,"Note",MB_OK | MB_ICONEXCLAMATION);
	//	return;

	//}


	
	
	((CEdit *) GetDlgItem(IDC_NAME))->GetWindowText(shapeStr);

	if (proposedShapeStr != shapeStr) 
	{
		//shape name has been changed, reset counter to 1
		shapeName = shapeStr;

		//a better method is to extract the trailing number from shapestr and use it as number for shapeNameInt
		//shapeNameInt = 1;
		AdjustShapeName(shapeName);


	}
	else {
		
		shapeNameInt++;
		if (shapeNameInt>2147483600) //assume int32
			shapeNameInt = 1;
	}


	

	
	CDialog::OnOK();
}

void CNewShape::OnButton1() 
{
	// TODO: Add your control notification handler code here
	CString widthStr, heightStr;
	((CEdit *) GetDlgItem(IDC_WIDTH))->GetWindowText(widthStr);
	((CEdit *) GetDlgItem(IDC_HEIGHT))->GetWindowText(heightStr);
	
	if (m_imageDir == "")
		m_imageDir = GetProgPath();	

	static char BASED_CODE szFilter[] =	"Picture Files (*.bmp; *.jpg; *.gif)|*.bmp; *.jpg; *.gif||";
	static char szTitle[]="Load Picture";		
	
	CFileDialog fdlg(TRUE,"*.bmp; *.jpg; *.gif","*.bmp; *.jpg; *.gif",OFN_LONGNAMES | OFN_FILEMUSTEXIST ,szFilter,this);	
	fdlg.m_ofn.lpstrTitle=szTitle;		
	fdlg.m_ofn.lpstrInitialDir = m_imageDir;

	CString m_newfileTitle;
	if(fdlg.DoModal() == IDOK)
	{	
		//m_imageFilename = fdlg.GetPathName();
		m_newfileTitle = fdlg.GetPathName();		
		((CEdit *) GetDlgItem(IDC_IMAGEFILETEXT))->SetWindowText(m_newfileTitle);


		m_newfileTitle=m_newfileTitle.Left(m_newfileTitle.ReverseFind('\\'));		
		m_imageDir = m_newfileTitle;	
		
				
	}
	
}
