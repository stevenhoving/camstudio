#pragma once

#include "Profile.h"

#include <vector>
#include <algorithm>
// TODO: reorganize code and move bodies to cpp. Even better to merge CamCursor with CursorOptionsDlg class.
#include <CamHook/CamHook.h>

/////////////////////////////////////////////////////////////////////////////
// CCamCursor
/////////////////////////////////////////////////////////////////////////////
enum eCursorType
{
    ACTIVE,
    CUSTOM,
    CUSTOMFILE
};

class CCamCursor
{
public:
    CCamCursor();

    CCamCursor(const CCamCursor &rhs);

    virtual ~CCamCursor() = default;

    HCURSOR Load() const;
    HCURSOR Load(HCURSOR hCursor);
    HCURSOR Save() const;
    HCURSOR Save(HCURSOR hCursor);
    HCURSOR Custom() const;
    HCURSOR Custom(HCURSOR hCursor);
    CString Dir() const;
    CString Dir(CString strDir);
    CString FileName() const;
    CString FileName(CString strFileName);
    int Select() const;

    int Select(int iSelect);

    HCURSOR Cursor();

    HCURSOR Cursor(int iCursorType);

    void AddID(DWORD dwID);

    size_t SizeID() const;

    DWORD GetID(size_t uIndex);

    bool Record() const;

    bool Record(bool bRec);

    int CustomType() const;

    int CustomType(int iType);

    bool Highlight() const;

    bool Highlight(bool bHiLight);

    int HighlightSize() const;
    int HighlightSize(int iSize);

    int HighlightShape() const;
    int HighlightShape(int iShape);
    COLORREF HighlightColor() const;
    COLORREF HighlightColor(COLORREF clr);
    bool HighlightClick() const;
    bool HighlightClick(bool bHiLight);
    COLORREF ClickLeftColor() const;
    COLORREF ClickLeftColor(COLORREF clr);
    COLORREF ClickRightColor() const;
    COLORREF ClickRightColor(COLORREF clr);

    bool Read(libconfig::Setting &cProfile);

    bool Write(libconfig::Setting &cProfile);

    // why do we ever want to make everything private and use getter/setter even for plain types with no onchange
    // events??? let's keep it simple
    DWORD m_clrClickMiddle; // wheel click color ARGB
    float m_fRingWidth;     // ring width for click events, Gdiplus::REAL
    int m_iRingThreshold;   // time in ms for ring to grow till m_iRingSize
    int m_iRingSize;        // maximum ring size

private:
    // Cursor variables
    int m_iSelect;           // selected (active) cursor [0..2]
    HCURSOR m_hSavedCursor;  // active screen cursor
    HCURSOR m_hLoadCursor;   // resource cursor
    HCURSOR m_hCustomCursor; // file cursor
    CString m_strDir;        // directory to load icon images
    CString m_strFileName;
    std::vector<DWORD> m_vIconID;

    bool m_bRecord;           // record screen cursor
    int m_iCustomSel;         // cursor type (actual, predef, custom)
    bool m_bHighlight;        // highlight screen cursor
    int m_iHighlightSize;     // cursor highlilght size
    int m_iHighlightShape;    // cursor highlilght shape
    COLORREF m_clrHighlight;  // cursor highlilght color
    bool m_bHighlightClick;   // highlight cursor buttons
    COLORREF m_clrClickLeft;  // left click color
    COLORREF m_clrClickRight; // right click color
};

extern CCamCursor CamCursor; // cursors

