// Modifies Timestamp, Caption, and Watermark
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "../Recorder.h"
#include "AnnotationEffectsOptions.h"

// CAnnotationEffectsOptionsDlg dialog

IMPLEMENT_DYNAMIC(CAnnotationEffectsOptionsDlg, CDialog)

CAnnotationEffectsOptionsDlg::CAnnotationEffectsOptionsDlg(CWnd *pParent /*=nullptr*/)
    : CDialog(CAnnotationEffectsOptionsDlg::IDD, pParent)
    , m_timestamp()
    , m_caption()
    , m_image()
    , m_buttonImagePath()
    , m_ctlImagePath()
    , m_FormatTimestampPreview()
{
    // TRACE("## CAnnotationEffectsOptionsDlg::CAnnotationEffectsOptionsDlg() \n");
    // Todo: Update preview fields.
}

CAnnotationEffectsOptionsDlg::~CAnnotationEffectsOptionsDlg()
{
    // TRACE("## CAnnotationEffectsOptionsDlg::~CAnnotationEffectsOptionsDlg() \n");
}

///////////////////////////////////////////////////
// Connencted (passed) value are defined and set with CRecorderView::OnEffectsOptions()
///////////////////////////////////////////////////
void CAnnotationEffectsOptionsDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_IMAGE_PATH, m_buttonImagePath);
    DDX_Text(pDX, IDC_EDIT_IMAGE_PATH, m_image.text);
    DDX_Control(pDX, IDC_EDIT_IMAGE_PATH, m_ctlImagePath);
    DDX_Text(pDX, IDC_EDIT_TIMESTAMP_FORMAT, m_timestamp.text);

    DDX_Text(pDX, IDC_EDIT_CAPTION_TEXT, m_caption.text);
    DDX_Control(pDX, IDC_FORMATTIMESTAMPPREVIEW, m_FormatTimestampPreview);

    // TRACE("## CAnnotationEffectsOptionsDlg::DoDataExchange\n" );
    // TRACE("## -----------------------------------------------\n" );
    // TRACE("## m_CheckBoxXnoteDisplayCameraDelayMode=[%d]\n", m_CheckBoxXnoteDisplayCameraDelayMode.GetCheck() );
    // TRACE("## m_bXnoteDisplayCameraDelayMode=[%d]\n", m_bXnoteDisplayCameraDelayMode);
    // TRACE("## m_ulXnoteRecordDurationLimitInMilliSec=[%d]\n", m_ulXnoteRecordDurationLimitInMilliSec);
    // TRACE("## -----------------------------------------------\n" );
    // TRACE("## m_CheckBoxXnoteRecordDurationLimitMode=[%d]\n", m_CheckBoxXnoteRecordDurationLimitMode.GetCheck()  );
    // TRACE("## m_bXnoteRecordDurationLimitMode=[%d]\n", m_bXnoteRecordDurationLimitMode);
    // TRACE("## m_ulXnoteCameraDelayInMilliSec=[%lu]\n", m_ulXnoteCameraDelayInMilliSec);
    // TRACE("## -----------------------------------------------\n" );


    // TRACE("## m_CheckBoxXnoteDisplayCameraDelayMode=[%d]\n", m_CheckBoxXnoteDisplayCameraDelayMode.GetCheck() );
    // TRACE("## m_CheckBoxXnoteRecordDurationLimitMode=[%d]\n", m_CheckBoxXnoteRecordDurationLimitMode.GetCheck()  );
    // TRACE("## -----------------------------------------------\n" );
}

BEGIN_MESSAGE_MAP(CAnnotationEffectsOptionsDlg, CDialog)
ON_EN_CHANGE(IDC_EDIT_TIMESTAMP_FORMAT, &CAnnotationEffectsOptionsDlg::OnEnChangeEditTimestampFormat)
ON_BN_CLICKED(IDC_BUTTON_TIMESTAMP_FORMAT_OPTIONS,
              &CAnnotationEffectsOptionsDlg::OnBnClickedButtonTimestampFormatOptions)


ON_BN_CLICKED(IDC_BUTTON_CAPTION_OPTIONS, OnBnClickedButtonCaptionOptions)

ON_BN_CLICKED(IDC_BUTTON_WATERMARK_OPTIONS, OnBnClickedButtonWatermarkOptions)

ON_BN_CLICKED(IDC_BUTTON_IMAGE_PATH, OnBnClickedButtonImagePath)
ON_BN_CLICKED(IDOK, &CAnnotationEffectsOptionsDlg::OnBnClickedOk)
ON_WM_CREATE()

ON_EN_CHANGE(IDC_FORMATTIMESTAMPPREVIEW, &CAnnotationEffectsOptionsDlg::OnEnChangeFormatpreview)
END_MESSAGE_MAP()

// CAnnotationEffectsOptionsDlg message handlers

void CAnnotationEffectsOptionsDlg::OnBnClickedButtonImagePath()
{
    const TCHAR *szFilter = _T("Bitmap Files (*.bmp)|*.bmp|GIF Files (*.gif)|*.gif|JPEG Files (*.jpg;*.jpeg)|*.jpg; "
                                 "*.jpeg|All Files (*.*)|*.*||");
    CFileDialog dlg(TRUE, nullptr, m_image.text, OFN_HIDEREADONLY, szFilter);
    if (dlg.DoModal() == IDOK)
    {
        m_image.text = dlg.GetPathName();
        m_ctlImagePath.SetWindowText(m_image.text);
        // AfxMessageBox(m_imagePath);
    }
}

void CAnnotationEffectsOptionsDlg::OnBnClickedButtonCaptionOptions()
{
    // TODO: Add your control notification handler code here
    CEffectsOptionsDlg optDlg;
    optDlg.m_params = m_caption;
    if (optDlg.DoModal() == IDOK)
    {
        m_caption = optDlg.m_params;
    }
}

void CAnnotationEffectsOptionsDlg::OnBnClickedButtonTimestampFormatOptions()
{
    // TODO: Add your control notification handler code here
    CEffectsOptionsDlg optDlg;
    optDlg.m_params = m_timestamp;
    if (optDlg.DoModal() == IDOK)
    {
        m_timestamp = optDlg.m_params;
    }
}

void CAnnotationEffectsOptionsDlg::OnBnClickedButtonWatermarkOptions()
{
    // TODO: Add your control notification handler code here
    CEffectsOptions2Dlg optDlg;
    optDlg.m_params = m_image;
    if (optDlg.DoModal() == IDOK)
    {
        m_image = optDlg.m_params;
    }
}

void CAnnotationEffectsOptionsDlg::OnEnChangeEditTimestampFormat()
{
    // Update Timestamp formatted preview presentation field
    CAnnotationEffectsOptionsDlg::OnEnChangeFormatpreview();
}

void CAnnotationEffectsOptionsDlg::OnBnClickedOk()
{
    CString str;
    GetDlgItem(IDC_EDIT_TIMESTAMP_FORMAT)->GetWindowText(str);
    // \todo figure out if we need to fix this
    //if (!IsStrftimeSafe(str.GetBuffer()))
    //{
        //MessageBox(_T("There is an error in the timestamp format!"), _T("Error!"), MB_OK);
        //return;
    //}

    Invalidate();
    OnOK();
}

int CAnnotationEffectsOptionsDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialog::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }

    /* TODO: Is this the most appropriate place to put defaults? Probably not ...
     * Also, theres probably a better check than if the text is zero weather
     * or not the data has been initialized.*/

    /*
     * Comment Jun2010: Indeed there is a better place for initialization.
     * Connected (passed) value are defined and set with CRecorderView::OnEffectsOptions()
     */

    if (m_timestamp.text.GetLength() == 0)
    {
        m_timestamp.text = CString("Time: %H:%M:%S");
        m_timestamp.backgroundColor = RGB(255, 255, 255);
        m_timestamp.textColor = RGB(0, 0, 0);
        m_timestamp.logfont.lfHeight = 12;
    }

    // At first I believed that m_cXnoteDisplayFormatString had not any added value here.
    // But it appeared that the length of this filed determines the visible tect in annotation.
    // Code below determine the string length of the filed to use.

    return 0;
}

bool CAnnotationEffectsOptionsDlg::IsStrftimeSafe(char *pbuffer)
{
    pbuffer = ::strchr(pbuffer, '%');
    while (pbuffer)
    {
        char nxt = *(++pbuffer);
        if (nxt != 'a' && nxt != 'A' && nxt != 'b' && nxt != 'B' && nxt != 'c' && nxt != 'd' && nxt != 'H' &&
            nxt != 'I' && nxt != 'j' && nxt != 'm' && nxt != 'M' && nxt != 'p' && nxt != 'S' && nxt != 'U' &&
            nxt != 'w' && nxt != 'W' && nxt != 'x' && nxt != 'X' && nxt != 'y' && nxt != 'Y' && nxt != 'Z' &&
            nxt != '%')
        {
            return false;
        }

        pbuffer = ::strchr(++pbuffer, '%');
    }

    return true;
}

void CAnnotationEffectsOptionsDlg::OnEnChangeFormatpreview()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TRACE("CAnnotationEffectsOptionsDlg::OnEnChangeFormatpreview() \n");
    CString str;
    GetDlgItem(IDC_EDIT_TIMESTAMP_FORMAT)->GetWindowText(str);

    //if (IsStrftimeSafe(str.GetBuffer()))
    {
        time_t szClock;
        time(&szClock);
        struct tm newTime = {};
        localtime_s(&newTime, &szClock);
        TCHAR TimeBuff[256];
        wcsftime(TimeBuff, sizeof(TimeBuff), str, &newTime);
        m_FormatTimestampPreview.SetWindowText(TimeBuff);
    }
    //else
    //{
    //    m_FormatTimestampPreview.SetWindowText(_T("Error in timestamp format!"));
    //}
}
