// LayoutList.cpp: implementation of the CLayoutList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Recorder.h"
#include "LayoutList.h"
#include "ScreenAnnotations.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int iCurrentLayoutID = 1;
CString strLayoutName("Layout_");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLayoutList::CLayoutList()
{

	uniqueID = iCurrentLayoutID;
	iCurrentLayoutID++;
	if (iCurrentLayoutID > 2147483647)
		iCurrentLayoutID = 0;

	//layoutName.Format("Layout_%d",uniqueID);
	//CString layoutIntStr;
	//layoutIntStr.Format("%d",iLayoutNameInt);
	//layoutName = strLayoutName + layoutIntStr;
	//iLayoutNameInt++;
	//if (iLayoutNameInt > 2147483647)
	// iLayoutNameInt = 1;

	layoutArrayPtr = NULL;

}

CLayoutList::~CLayoutList()
{

}

BOOL CLayoutList::SaveLayoutToFile(FILE *fptr)
{

	long layoutversion = 1;
	fwrite( (void *) &layoutversion, sizeof(long), 1, fptr );

	int len = layoutName.GetLength();
	fwrite( (void *) &len, sizeof(int), 1, fptr );
	fwrite( (void *) LPCTSTR(layoutName), len, 1, fptr );

	ListManager.SaveLayoutArrayToFile(layoutArrayPtr , fptr);

	return 1;

}

BOOL CLayoutList::LoadLayoutFromFile(FILE *fptr)
{

	long layoutversion = 1;
	fread( (void *) &layoutversion, sizeof(long), 1, fptr );

	int len = 0;
	fread( (void *) &len, sizeof(int), 1, fptr );
	if ((len>0) && (len<100000))
	{
		//void *buf = malloc(len);
		char *buf = (char *) malloc(len + 2);
		fread( (void *) buf, len, 1, fptr );
		buf[len] = 0;
		buf[len+1] = 0;
		layoutName = (char *) buf;
		free(buf);

	}

	if (layoutArrayPtr==NULL) {
		// TODO, Possible memory leak, where is the delete operation of the new below done?
		layoutArrayPtr = new CArray<CTransparentWnd *,CTransparentWnd *>;
	}
	ListManager.LoadLayoutArrayFromFile(layoutArrayPtr , fptr);

	if (layoutversion>1)
	{

	}

	return 1;

}