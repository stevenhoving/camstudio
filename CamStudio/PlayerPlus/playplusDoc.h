// playplusDoc.h : interface of the CPlayplusDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYPLUSDOC_H__B34DE975_75D9_47D7_A474_5971A0F181CE__INCLUDED_)
#define AFX_PLAYPLUSDOC_H__B34DE975_75D9_47D7_A474_5971A0F181CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPlayplusDoc : public CDocument
{
protected: // create from serialization only
	CPlayplusDoc();
	DECLARE_DYNCREATE(CPlayplusDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlayplusDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPlayplusDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPlayplusDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYPLUSDOC_H__B34DE975_75D9_47D7_A474_5971A0F181CE__INCLUDED_)
