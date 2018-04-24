#pragma once

class CRecorderDoc : public CDocument
{
protected: // create from serialization only
    CRecorderDoc();
    DECLARE_DYNCREATE(CRecorderDoc)

    // Attributes
public:
    // Operations
public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CRecorderDoc)
public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive &ar);
    //}}AFX_VIRTUAL

    // Implementation
public:
    ~CRecorderDoc() override;
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext &dc) const;
#endif

    unsigned int FrameWidth() const;
    unsigned int FrameWidth(unsigned int uWidth);
    unsigned int FrameHeight() const;
    unsigned int FrameHeight(unsigned int uHeight);

protected:
    DECLARE_MESSAGE_MAP()
private:
    unsigned int m_uFrameWidth;  // Capture frame width
    unsigned int m_uFrameHeight; // capture frame height
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

