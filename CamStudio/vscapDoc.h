// vscapDoc.h : interface of the CVscapDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VSCAPDOC_H__8E747B04_A2E7_43C2_BFDC_34C231661E31__INCLUDED_)
#define AFX_VSCAPDOC_H__8E747B04_A2E7_43C2_BFDC_34C231661E31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CVscapDoc : public CDocument
{
protected: // create from serialization only
	CVscapDoc();
	DECLARE_DYNCREATE(CVscapDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVscapDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVscapDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CVscapDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VSCAPDOC_H__8E747B04_A2E7_43C2_BFDC_34C231661E31__INCLUDED_)
