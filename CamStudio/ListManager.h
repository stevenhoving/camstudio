// ListManager.h: interface for the CListManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTMANAGER_H__0F5DD254_C4A6_4E45_BCBD_8FA13F605F2A__INCLUDED_)
#define AFX_LISTMANAGER_H__0F5DD254_C4A6_4E45_BCBD_8FA13F605F2A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include "TransparentWnd.h"
#include "LayoutList.h"


class CListManager  
{
public:
	CListManager();
	virtual ~CListManager();

	CArray<CTransparentWnd *,CTransparentWnd *> shapeArray;
	CArray<CTransparentWnd *,CTransparentWnd *> displayArray;
	CArray<CLayoutList* ,CLayoutList*> layoutArray;

	int AddShapeArray(CTransparentWnd *);
	int RemoveShapeArray(CTransparentWnd *,int wantDestroy);
	int LoadShapeArray(CString loadDir);
	int LoadShapeArray(CString loadDir, int freeExisting);
	int SaveShapeArray(CString saveDir);
	int SwapShapeArray(long uniqueID1, long uniqueID2);
	int FreeShapeArray();

	int DestroyArrayItems(CArray<CTransparentWnd *,CTransparentWnd *> *);


	int AddDisplayArray(CTransparentWnd *);
	int RemoveDisplayArray(CTransparentWnd *,int wantDestroy);
	int LoadDisplayArray(CString loadDir);
	int SaveDisplayArray(CString saveDir);
	int FreeDisplayArray();
	int SwapLayoutArray(long uniqueID1, long uniqueID2);
	CArray<CTransparentWnd *,CTransparentWnd *> * CloneDisplayArray();
	int DestroyLayout(CLayoutList* pLayout);	
	int SaveLayoutArrayToFile(CArray<CTransparentWnd *,CTransparentWnd *> * layoutArrayPtr , FILE* fptr);
	int LoadLayoutArrayFromFile(CArray<CTransparentWnd *,CTransparentWnd *> * layoutArrayPtr , FILE* fptr);
	void EnsureOnTopList(CTransparentWnd* transWnd ); 

	int AddLayoutArray(CLayoutList* pLayout);
	int RemoveLayoutArray(CLayoutList* pLayout, int wantDestroyLayout);
	int LoadLayout(CString loadDir);
	int SaveLayout(CString loadDir);
	int FreeLayoutArray();
	CArray<CTransparentWnd *,CTransparentWnd *> * CloneLayoutArrayPtr(CLayoutList* itemLayout);
	CLayoutList* CloneLayout(CLayoutList * itemLayout);

};

#endif // !defined(AFX_LISTMANAGER_H__0F5DD254_C4A6_4E45_BCBD_8FA13F605F2A__INCLUDED_)


