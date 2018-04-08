#pragma once

#include "TransparentWnd.h"

class CLayoutList
{
public:
    CLayoutList();
    virtual ~CLayoutList();

    int uniqueID;
    CString layoutName;
    CArray<CTransparentWnd *, CTransparentWnd *> *layoutArrayPtr;
    BOOL SaveLayoutToFile(FILE *fptr);
    BOOL LoadLayoutFromFile(FILE *fptr);
};

