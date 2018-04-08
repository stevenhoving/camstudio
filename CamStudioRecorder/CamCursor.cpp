#include "stdafx.h"
#include "CamCursor.h"

CCamCursor::CCamCursor()
    : m_clrClickMiddle(0xa000ff00)
    , m_fRingWidth(1.5)
    , m_iRingThreshold(1000)
    , m_iRingSize(20)
    , m_iSelect(ACTIVE)
    , m_hSavedCursor(nullptr)
    , m_hLoadCursor(nullptr)
    , m_hCustomCursor(nullptr)
    , m_strDir()
    , m_strFileName()
    , m_vIconID()
    , m_bRecord(true)
    , m_iCustomSel(0)
    , m_bHighlight(false)
    , m_iHighlightSize(64)
    , m_iHighlightShape(0)
    , m_clrHighlight(0xa0ffff80)
    , m_bHighlightClick(false)
    , m_clrClickLeft(0xa0ff0000)
    , m_clrClickRight(0xa00000ff)
{
}

CCamCursor::CCamCursor(const CCamCursor &rhs)
{
    *this = rhs;
}

HCURSOR CCamCursor::Load(HCURSOR hCursor)
{
    return m_hLoadCursor = hCursor;
}

HCURSOR CCamCursor::Load() const
{
    return m_hLoadCursor;
}

HCURSOR CCamCursor::Save(HCURSOR hCursor)
{
    // mlt_msk: what are we saving on? int assignment?
    return (m_hSavedCursor == hCursor) ? m_hSavedCursor : (m_hSavedCursor = hCursor);
}

HCURSOR CCamCursor::Save() const
{
    return m_hSavedCursor;
}

HCURSOR CCamCursor::Custom(HCURSOR hCursor)
{
    return m_hCustomCursor = hCursor;
}

HCURSOR CCamCursor::Custom() const
{
    return m_hCustomCursor;
}

CString CCamCursor::Dir(CString strDir)
{
    return m_strDir = strDir;
}

CString CCamCursor::Dir() const
{
    return m_strDir;
}

CString CCamCursor::FileName(CString strFileName)
{
    return m_strFileName = strFileName;
}

CString CCamCursor::FileName() const
{
    return m_strFileName;
}

int CCamCursor::Select(int iSelect)
{
    return m_iSelect = ((0 <= iSelect) && (iSelect < 3)) ? iSelect : 0;
}

int CCamCursor::Select() const
{
    return m_iSelect;
}

HCURSOR CCamCursor::Cursor(int iCursorType)
{
    switch (iCursorType)
    {
    default:
    case ACTIVE:
        m_hSavedCursor = getCursor();
        return m_hSavedCursor;
    case CUSTOM:
        return m_hCustomCursor;
    case CUSTOMFILE:
        return m_hLoadCursor;
    }
}

HCURSOR CCamCursor::Cursor()
{
    return Cursor(m_iSelect);
}

void CCamCursor::AddID(DWORD dwID)
{
    if (std::find(m_vIconID.begin(), m_vIconID.end(), dwID) == m_vIconID.end())
    {
        m_vIconID.push_back(dwID);
    }
}

size_t CCamCursor::SizeID() const
{
    return m_vIconID.size();
}

DWORD CCamCursor::GetID(size_t uIndex)
{
    return (uIndex < m_vIconID.size()) ? m_vIconID[uIndex] : 0;
}

bool CCamCursor::Record(bool bRec)
{
    return m_bRecord = bRec;
}

bool CCamCursor::Record() const
{
    return m_bRecord;
}

int CCamCursor::CustomType(int iType)
{
    return m_iCustomSel = iType;
}

int CCamCursor::CustomType() const
{
    return m_iCustomSel;
}

bool CCamCursor::Highlight(bool bHiLight)
{
    return m_bHighlight = bHiLight;
}

bool CCamCursor::Highlight() const
{
    return m_bHighlight;
}

int CCamCursor::HighlightSize(int iSize)
{
    return m_iHighlightSize = iSize;
}

int CCamCursor::HighlightSize() const
{
    return m_iHighlightSize;
}

int CCamCursor::HighlightShape(int iShape)
{
    return m_iHighlightShape = iShape;
}

int CCamCursor::HighlightShape() const
{
    return m_iHighlightShape;
}

COLORREF CCamCursor::HighlightColor(COLORREF clr)
{
    return m_clrHighlight = clr;
}

COLORREF CCamCursor::HighlightColor() const
{
    return m_clrHighlight;
}

bool CCamCursor::HighlightClick(bool bHiLight)
{
    return m_bHighlightClick = bHiLight;
}

bool CCamCursor::HighlightClick() const
{
    return m_bHighlightClick;
}

COLORREF CCamCursor::ClickLeftColor(COLORREF clr)
{
    return m_clrClickLeft = clr;
}

COLORREF CCamCursor::ClickLeftColor() const
{
    return m_clrClickLeft;
}

COLORREF CCamCursor::ClickRightColor(COLORREF clr)
{
    return m_clrClickRight = clr;
}

COLORREF CCamCursor::ClickRightColor() const
{
    return m_clrClickRight;
}

bool CCamCursor::Read(libconfig::Setting &cProfile)
{
    cProfile.lookupValue("RecordCursor", m_bRecord);
    cProfile.lookupValue("CursorType", m_iCustomSel);
    cProfile.lookupValue("CursorSel", m_iSelect);
    cProfile.lookupValue("Highlight", m_bHighlight);
    cProfile.lookupValue("HighlightSize", m_iHighlightSize);
    cProfile.lookupValue("HighlightShape", m_iHighlightShape);
    cProfile.lookupValue("HighlightColor", (int &)m_clrHighlight);
    cProfile.lookupValue("HighlightClick", m_bHighlightClick);
    cProfile.lookupValue("RingThreshold", m_iRingThreshold);
    cProfile.lookupValue("RingSize", m_iRingSize);
    cProfile.lookupValue("RingWidth", m_fRingWidth);
    cProfile.lookupValue("ClickColorLeft", (int &)m_clrClickLeft);
    cProfile.lookupValue("ClickColorMiddle", (int &)m_clrClickMiddle);
    cProfile.lookupValue("ClickColorRight", (int &)m_clrClickRight);
    std::string text;
    if (cProfile.lookupValue("CursorDir", text))
        m_strFileName = text.c_str();
    return true;
}

bool CCamCursor::Write(libconfig::Setting &cProfile)
{
    UpdateSetting(cProfile, "RecordCursor", m_bRecord, libconfig::Setting::TypeBoolean);
    UpdateSetting(cProfile, "CursorType", m_iCustomSel, libconfig::Setting::TypeInt);
    UpdateSetting(cProfile, "CursorSel", m_iSelect, libconfig::Setting::TypeInt);
    UpdateSetting(cProfile, "Highlight", m_bHighlight, libconfig::Setting::TypeBoolean);
    UpdateSetting(cProfile, "HighlightSize", m_iHighlightSize, libconfig::Setting::TypeInt);
    UpdateSetting(cProfile, "HighlightShape", m_iHighlightShape, libconfig::Setting::TypeInt);
    UpdateSetting(cProfile, "HighlightColor", (long &)m_clrHighlight, libconfig::Setting::TypeInt);
    UpdateSetting(cProfile, "RingThreshold", m_iRingThreshold, libconfig::Setting::TypeInt);
    UpdateSetting(cProfile, "RingSize", m_iRingSize, libconfig::Setting::TypeInt);
    UpdateSetting(cProfile, "RingWidth", m_fRingWidth, libconfig::Setting::TypeFloat);
    UpdateSetting(cProfile, "HighlightClick", m_bHighlightClick, libconfig::Setting::TypeBoolean);
    UpdateSetting(cProfile, "ClickColorLeft", (long &)m_clrClickLeft, libconfig::Setting::TypeInt);
    UpdateSetting(cProfile, "ClickColorMiddle", (long &)m_clrClickMiddle, libconfig::Setting::TypeInt);
    UpdateSetting(cProfile, "ClickColorRight", (long &)m_clrClickRight, libconfig::Setting::TypeInt);
    std::string text(m_strFileName);
    UpdateSetting(cProfile, "CursorDir", text, libconfig::Setting::TypeString);
    return true;
}
