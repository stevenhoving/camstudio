// RecorderDoc.h : interface of the CRecorderDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VSCAPDOC_H__8E747B04_A2E7_43C2_BFDC_34C231661E31__INCLUDED_)
#define AFX_VSCAPDOC_H__8E747B04_A2E7_43C2_BFDC_34C231661E31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

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
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRecorderDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	unsigned int FrameWidth() const					{return m_uFrameWidth;}
	unsigned int FrameWidth(unsigned int uWidth)	{return m_uFrameWidth = uWidth;}
	unsigned int FrameHeight() const				{return m_uFrameHeight;}
	unsigned int FrameHeight(unsigned int uHeight)	{return m_uFrameHeight = uHeight;}
	
protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CRecorderDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	unsigned int m_uFrameWidth;		// Capture frame width
	unsigned int m_uFrameHeight;	// capture frame height
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VSCAPDOC_H__8E747B04_A2E7_43C2_BFDC_34C231661E31__INCLUDED_)
