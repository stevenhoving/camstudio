// LayoutList.h: interface for the CLayoutList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LAYOUTLIST_H__46FEA864_AA1F_4655_9676_0802D031935A__INCLUDED_)
#define AFX_LAYOUTLIST_H__46FEA864_AA1F_4655_9676_0802D031935A__INCLUDED_

#include <afxtempl.h>
#include "TransparentWnd.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLayoutList  
{
public:
	CLayoutList();
	virtual ~CLayoutList();

	int uniqueID;
	CString layoutName;
	CArray<CTransparentWnd *,CTransparentWnd *>* layoutArrayPtr;
	BOOL SaveLayoutToFile(FILE *fptr);
	BOOL LoadLayoutFromFile(FILE *fptr);

};

#endif // !defined(AFX_LAYOUTLIST_H__46FEA864_AA1F_4655_9676_0802D031935A__INCLUDED_)
