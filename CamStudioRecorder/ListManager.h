#pragma once


#include "TransparentWnd.h"
#include "LayoutList.h"

class CListManager
{
public:
    CListManager();
    virtual ~CListManager();

    int AddShapeArray(CTransparentWnd *);
    int RemoveShapeArray(CTransparentWnd *, int wantDestroy);
    int LoadShapeArray(CString loadDir);
    int LoadShapeArray(CString loadDir, int freeExisting);
    int SaveShapeArray(CString saveDir);
    int SwapShapeArray(long uniqueID1, long uniqueID2);
    int FreeShapeArray();

    int DestroyArrayItems(CArray<CTransparentWnd *, CTransparentWnd *> *);

    int AddDisplayArray(CTransparentWnd *);
    int RemoveDisplayArray(CTransparentWnd *, int wantDestroy);
    int LoadDisplayArray(CString loadDir);
    int SaveDisplayArray(CString saveDir);
    int FreeDisplayArray();
    int SwapLayoutArray(long uniqueID1, long uniqueID2);

    CArray<CTransparentWnd *, CTransparentWnd *> *CloneDisplayArray();

    int DestroyLayout(CLayoutList *pLayout);
    int SaveLayoutArrayToFile(CArray<CTransparentWnd *, CTransparentWnd *> *layoutArrayPtr, FILE *fptr);
    int LoadLayoutArrayFromFile(CArray<CTransparentWnd *, CTransparentWnd *> *layoutArrayPtr, FILE *fptr);
    void EnsureOnTopList(CTransparentWnd *transWnd);

    int AddLayoutArray(CLayoutList *pLayout);
    int RemoveLayoutArray(CLayoutList *pLayout, int wantDestroyLayout);
    int LoadLayout(CString loadDir);
    int SaveLayout(CString loadDir);
    int FreeLayoutArray();
    CLayoutList *CloneLayout(CLayoutList *itemLayout);
    CArray<CTransparentWnd *, CTransparentWnd *> *CloneLayoutArrayPtr(CLayoutList *itemLayout);

public:
    CArray<CTransparentWnd *, CTransparentWnd *> shapeArray;
    CArray<CTransparentWnd *, CTransparentWnd *> displayArray;
    CArray<CLayoutList *, CLayoutList *> layoutArray;

private:
};

extern CListManager ListManager;


