// ListManager.cpp: implementation of the CListManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vscap.h"
#include "ListManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern void ErrMsg(char format[], ...);
extern int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CListManager::CListManager()
{

	shapeArray.RemoveAll( );
	displayArray.RemoveAll( );
	layoutArray.RemoveAll( );


}

CListManager::~CListManager()
{

}


int CListManager::AddShapeArray(CTransparentWnd * newWnd)
{

	shapeArray.Add(newWnd);
	return 1;
}


//Note: The window is only detached from the array, it is not destroyed
int CListManager::RemoveShapeArray(CTransparentWnd * removeWnd, int wantDestroy)
{

	int found = 0;

	int max = shapeArray.GetSize();
	CTransparentWnd * itemWnd = NULL;
	for (int i=0;i<max; i++)
	{
		itemWnd = shapeArray[i];
		if (itemWnd == removeWnd)
		{
			shapeArray.RemoveAt( i, 1);
			found = 1;
			break;  //allow only one removal
			
		}
	}

	if ((found) && (wantDestroy)) {

		if (removeWnd) {
			removeWnd->DestroyWindow();
			delete removeWnd;
		}

	}

	return found;
}

int CListManager::LoadShapeArray(CString loadDir)
{
	return LoadShapeArray(loadDir, 1);

}

int CListManager::LoadShapeArray(CString loadDir, int freeExisting)
{

	int ret = TRUE;
	FILE* fptr = fopen(LPCTSTR(loadDir),"rb");
	if (fptr != NULL) {

				

				long fileversion = 0;
				fread( (void *) &fileversion, sizeof(long), 1, fptr );		

				int max = 0;
				fread( (void *) &max, sizeof(int), 1, fptr ); //Number of Shapes

				int reserve[100];
				for (int j=0;j<100;j++)
					fread( (void *) &reserve[j], sizeof(int), 1, fptr ); //Reserve Bytes

				//Testing
				int failtest = 0;
				if (reserve[0]!='c')  failtest=1;
				if (reserve[1]!='s')  failtest=1;
				if (reserve[2]!='h')  failtest=1;
				if (reserve[3]!='a')  failtest=1;
				if (reserve[4]!='p')  failtest=1;
				if (reserve[5]!='e')  failtest=1;

				if (failtest)
				{
					//MessageBox(NULL,"Invalid shape file CamShapes.ini","Note",MB_OK | MB_ICONEXCLAMATION);
					MessageOut(NULL,IDS_STRINGINSHAPEFILE,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
					fclose(fptr);
					return  FALSE;

				}
				
				if ((max>0) && (max<100000))
				{
					if (freeExisting)
						FreeShapeArray();
					for (int i=0; i<max ; i++)
					{

						CTransparentWnd * itemWnd = new CTransparentWnd;
						if (itemWnd)
						{
							if (!itemWnd->LoadShape(fptr))
							{
								ret = FALSE;
								break;
							}
							else
							{

								CString pTitle(itemWnd->m_shapeStr);
								itemWnd->CreateEx(	WS_EX_TOPMOST ,	
													AfxRegisterWndClass(0),
													LPCTSTR(pTitle),
													WS_POPUP | WS_SYSMENU,
													itemWnd->m_rectWnd,
													NULL,
													NULL,
													NULL );	

								AddShapeArray(itemWnd);
							}

						}

					}


				}
	

		fclose(fptr);

	}
	else 
	{

		//Silent Mode
		//MessageBox(NULL,"Error opening shape file for loading","Note",MB_OK | MB_ICONEXCLAMATION);

	}


	return ret;
}


int CListManager::SaveShapeArray(CString saveDir)
{

	int ret = TRUE;
	FILE* fptr = fopen(LPCTSTR(saveDir),"wb");
	if (fptr != NULL) {

			
			long fileversion = 100;
			fwrite( (void *) &fileversion, sizeof(long), 1, fptr );		

			int max = shapeArray.GetSize();
			fwrite( (void *) &max, sizeof(int), 1, fptr ); //Number of Shapes

			int reserve[100];
			reserve[0]='c';
			reserve[1]='s';
			reserve[2]='h';
			reserve[3]='a';
			reserve[4]='p';
			reserve[5]='e';

			for (int j=0;j<100;j++)
				fwrite( (void *) &reserve[j], sizeof(int), 1, fptr ); //Reserve Bytes
			
			
			CTransparentWnd * itemWnd = NULL;
			for (int i=0;i<max; i++)
			{
				itemWnd = shapeArray[i];
				if (itemWnd)
				{
					if (!itemWnd->SaveShape(fptr))
						ret = FALSE;

				}
			}	

		fclose(fptr);

	}
	else 
		//MessageBox(NULL,"Error opening shape file for saving","Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(NULL,IDS_STRING_EOSFS,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

	return ret;
}

int CListManager::AddDisplayArray(CTransparentWnd * newWnd)
{

	displayArray.Add(newWnd);
	return 1;
}

int CListManager::RemoveDisplayArray(CTransparentWnd *removeWnd, int wantDestroy)
{

	int found = 0;

	int max = displayArray.GetSize();
	CTransparentWnd * itemWnd = NULL;
	for (int i=0;i<max; i++)
	{
		itemWnd = displayArray[i];
		if (itemWnd == removeWnd)
		{
			displayArray.RemoveAt( i, 1);
			found = 1;
			break;  //allow only one removal

		}
	}

	if ((found) && (wantDestroy)) {

		if (removeWnd) {
			removeWnd->DestroyWindow();
			delete removeWnd;
		}

	}

	return found;
}

int CListManager::LoadDisplayArray(CString loadDir)
{

	return 1;
}

int CListManager::SaveDisplayArray(CString saveDir)
{

	return 1;
}

int CListManager::AddLayoutArray(CLayoutList* pLayout)
{

	layoutArray.Add(pLayout);
	return 1;
}


int CListManager::RemoveLayoutArray(CLayoutList* pLayout,int wantDestroyLayout)
{

	
	int found = 0;

	int max = layoutArray.GetSize();
	CLayoutList * itemLayout = NULL;
	for (int i=max-1;i>=0; i--)
	{
		itemLayout = layoutArray[i];
		if (itemLayout == pLayout)
		{
			layoutArray.RemoveAt( i, 1);
			found = 1;
			break;  //allow only one removal			

		}
	}

	if ((found) && (wantDestroyLayout)) {

		if (itemLayout) {

			DestroyLayout(itemLayout);
			
		}

	}

	return found;
	
	
}

int CListManager::DestroyLayout(CLayoutList* pLayout)
{

	CArray<CTransparentWnd *,CTransparentWnd *> *removeArray = pLayout->layoutArrayPtr;

	DestroyArrayItems(removeArray);
	delete (pLayout->layoutArrayPtr);
	pLayout->layoutArrayPtr = NULL;
	
	delete pLayout;
	pLayout = NULL;

	return 1;

}


int CListManager::DestroyArrayItems(CArray<CTransparentWnd *,CTransparentWnd *> *removeArray)
{

	int max = removeArray->GetSize();
	CTransparentWnd * itemWnd = NULL;
	for (int i=max-1;i>=0; i--)
	{
		itemWnd = (*removeArray)[i];
		if (itemWnd)
		{

			if ((itemWnd->trackingOn) || (itemWnd->editTransOn) || (itemWnd->editImageOn))
			{
				//Do not remove, the item is undergoing editing

			}
			else
			{			
				itemWnd->ShowWindow(SW_HIDE);			

			
				//need to clone the display array				
				removeArray->RemoveAt( i, 1);
				itemWnd->DestroyWindow();
				delete itemWnd;

			}
	
		}

	}

	return 1;

}

CArray<CTransparentWnd *,CTransparentWnd *> * CListManager::CloneDisplayArray()
{

	CArray<CTransparentWnd *,CTransparentWnd *> *cloneArray = new CArray<CTransparentWnd *,CTransparentWnd *>;

	int max = displayArray.GetSize();
	CTransparentWnd * itemWnd = NULL;
	for (int i=0;i<max; i++)
	{
		itemWnd = displayArray[i];
		if (itemWnd)
		{
			//need to clone the display array
			CTransparentWnd *newWnd = itemWnd->Clone(0,0);
			if (newWnd)
				cloneArray->Add(newWnd);
		}

	}

	return cloneArray;

}



int CListManager::FreeShapeArray()
{

	DestroyArrayItems(&shapeArray);
	
	return 1;
}

int CListManager::FreeDisplayArray()
{
	
	DestroyArrayItems(&displayArray);	
	return 1;
}

int CListManager::FreeLayoutArray()
{

	//Free Multiple Lists		
	int max = layoutArray.GetSize();
	CLayoutList * pLayout = NULL;
	for (int i=max-1;i>=0; i--)
	{
		pLayout = layoutArray[i];
		if (pLayout)
		{

			RemoveLayoutArray(pLayout,1);				
		}

	}	
	
	return 1;
}


CArray<CTransparentWnd *,CTransparentWnd *> * CListManager::CloneLayoutArrayPtr(CLayoutList* itemLayout)
{

	CArray<CTransparentWnd *,CTransparentWnd *> * layoutArrayPtr = itemLayout->layoutArrayPtr;

	CArray<CTransparentWnd *,CTransparentWnd *> *cloneArray = new CArray<CTransparentWnd *,CTransparentWnd *>;

	int max = layoutArrayPtr->GetSize();
	CTransparentWnd * itemWnd = NULL;
	for (int i=0;i<max; i++)
	{
		itemWnd = (*layoutArrayPtr)[i];
		if (itemWnd)
		{
			//need to clone the display array
			CTransparentWnd *newWnd = itemWnd->Clone(0,0);
			if (newWnd)
				cloneArray->Add(newWnd);
		}

	}

	return cloneArray;


}



CLayoutList * CListManager::CloneLayout(CLayoutList * itemLayout)
{

		CLayoutList * newLayout = NULL;
		if (itemLayout)
		{			
			CArray<CTransparentWnd *,CTransparentWnd *> *cloneArray = CloneLayoutArrayPtr(itemLayout);
			if (cloneArray)
			{
				
				newLayout = new CLayoutList;
				if (newLayout) {

					newLayout->layoutArrayPtr = cloneArray;
					//gList.AddLayoutArray(newLayout);
					newLayout->layoutName = itemLayout->layoutName;	

				}

			}

		}

		return newLayout;

}


int CListManager::SwapShapeArray(long uniqueID1, long uniqueID2)
{

	CTransparentWnd * tempWnd = NULL;
	int swapItem1 = -1;
	int swapItem2 = -1;

	int max = shapeArray.GetSize();
	CTransparentWnd * itemWnd = NULL;
	for (int i=0;i<max; i++)
	{
		itemWnd = shapeArray[i];
		if (itemWnd)
		{

			if (itemWnd->uniqueID == uniqueID1)
			{
				swapItem1 = i;
			}
			else if (itemWnd->uniqueID == uniqueID2)
			{
				swapItem2 = i;
			}
	
		}
	}


	//Perform the swap
	if ((swapItem1>-1) && (swapItem2>-1))
	{
		tempWnd = shapeArray[swapItem1];
		shapeArray[swapItem1] = shapeArray[swapItem2];
		shapeArray[swapItem2] = tempWnd;
		return 1;

	}
	
	return 0;
}



int CListManager::SwapLayoutArray(long uniqueID1, long uniqueID2)
{

	CLayoutList * tempLayout = NULL;
	int swapItem1 = -1;
	int swapItem2 = -1;

	int max = layoutArray.GetSize();
	CLayoutList * itemLayout = NULL;
	for (int i=0;i<max; i++)
	{
		itemLayout = layoutArray[i];
		if (itemLayout)
		{

			if (itemLayout->uniqueID == uniqueID1)
			{
				swapItem1 = i;
			}
			else if (itemLayout->uniqueID == uniqueID2)
			{
				swapItem2 = i;
			}
	
		}
	}


	//Perform the swap
	if ((swapItem1>-1) && (swapItem2>-1))
	{
		tempLayout = layoutArray[swapItem1];
		layoutArray[swapItem1] = layoutArray[swapItem2];
		layoutArray[swapItem2] = tempLayout;
		return 1;

	}
	
	return 0;
}


int CListManager::LoadLayout(CString loadDir)
{


	int ret = TRUE;
	FILE* fptr = fopen(LPCTSTR(loadDir),"rb");
	if (fptr != NULL) {
			

				long fileversion = 0;
				fread( (void *) &fileversion, sizeof(long), 1, fptr );		

				int max = 0;
				fread( (void *) &max, sizeof(int), 1, fptr ); //Number of Layouts

				int reserve[100];
				for (int j=0;j<100;j++)
					fread( (void *) &reserve[j], sizeof(int), 1, fptr ); //Reserve Bytes


				//Testing
				int failtest = 0;
				if (reserve[0]!='c')  failtest=1;
				if (reserve[1]!='l')  failtest=1;
				if (reserve[2]!='a')  failtest=1;
				if (reserve[3]!='y')  failtest=1;
				if (reserve[4]!='o')  failtest=1;
				if (reserve[5]!='u')  failtest=1;
				if (reserve[6]!='t')  failtest=1;

				if (failtest)
				{
					//MessageBox(NULL,"Invalid layout file CamLayout.ini","Note",MB_OK | MB_ICONEXCLAMATION);
					MessageOut(NULL,IDS_STRINGINVLAYOUTFILE,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

					fclose(fptr);
					return  FALSE;
				}

				
								
				if ((max>0) && (max<100000))
				{

					FreeLayoutArray();
					for (int i=0; i<max ; i++)
					{

						CLayoutList * itemLayout = new CLayoutList;
						if (itemLayout)
						{
							if (!itemLayout->LoadLayoutFromFile(fptr))
							{
								ret = FALSE;
								break;
							}
							
							AddLayoutArray(itemLayout);

						}

					}


				}
	

		fclose(fptr);

	}
	else 
	{
		//Silent Mode
		//MessageBox(NULL,"Error opening layout file for loading","Note",MB_OK | MB_ICONEXCLAMATION);

	}


	return ret;
}

int CListManager::SaveLayout(CString saveDir)
{

	int ret = TRUE;
	FILE* fptr = fopen(LPCTSTR(saveDir),"wb");
	if (fptr != NULL) {


			
			long fileversion = 100;
			fwrite( (void *) &fileversion, sizeof(long), 1, fptr );		

			int max = layoutArray.GetSize();
			fwrite( (void *) &max, sizeof(int), 1, fptr ); //Number of Shapes


			int reserve[100];
			reserve[0]='c';
			reserve[1]='l';
			reserve[2]='a';
			reserve[3]='y';
			reserve[4]='o';
			reserve[5]='u';
			reserve[6]='t';

			for (int j=0;j<100;j++)
				fwrite( (void *) &reserve[j], sizeof(int), 1, fptr ); //Reserve Bytes
			
			
			CLayoutList * itemLayout = NULL;
			for (int i=0;i<max; i++)
			{
				itemLayout = layoutArray[i];
				if (itemLayout)
				{
					if (!itemLayout->SaveLayoutToFile(fptr))
						ret = FALSE;

				}
			}	

		fclose(fptr);

	}
	else 
		//MessageBox(NULL,"Error opening layout file for saving","Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(NULL,IDS_STRING_EOLFS,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

	return ret;
}


int CListManager::SaveLayoutArrayToFile(CArray<CTransparentWnd *,CTransparentWnd *> * layoutArrayPtr , FILE* fptr)
{

			int ret = TRUE;
			long shapearrayversion = 100;
			fwrite( (void *) &shapearrayversion, sizeof(long), 1, fptr );		

			int max = layoutArrayPtr->GetSize();
			fwrite( (void *) &max, sizeof(int), 1, fptr ); //Number of Shapes

		
			
			CTransparentWnd * itemWnd = NULL;
			for (int i=0;i<max; i++)
			{
				itemWnd = (*layoutArrayPtr)[i];
				if (itemWnd)
				{
					if (!itemWnd->SaveShape(fptr))
						ret = FALSE;

				}
			}	




	return ret;

}

//Ensure window is on the top of displaylist
void CListManager::EnsureOnTopList(CTransparentWnd* transWnd ) 
{

	int max = displayArray.GetSize();
	if (max<=0) return;
	if (transWnd == displayArray[max-1])  //at botton of list (i.e on top..no chnage)
	{
		//no change
		return;

	}
	int found = RemoveDisplayArray(transWnd, 0);
	if (found)
		AddDisplayArray(transWnd);
	
	return;
}

int CListManager::LoadLayoutArrayFromFile(CArray<CTransparentWnd *,CTransparentWnd *> * layoutArrayPtr , FILE* fptr)
{
			int ret = TRUE;

			if (layoutArrayPtr==NULL)
				return FALSE;

			long layoutarrayversion = 0;
			fread( (void *) &layoutarrayversion, sizeof(long), 1, fptr );		

			int max = 0;
			fread( (void *) &max, sizeof(int), 1, fptr ); //Number of Shapes


			if ((max>0) && (max<10000))
			{
				
				for (int i=0; i<max ; i++)
				{

					CTransparentWnd * itemWnd = new CTransparentWnd;
					if (itemWnd)
					{
						if (!itemWnd->LoadShape(fptr))
						{
							ret = FALSE;
							break;
						}
						else
						{

							CString pTitle(itemWnd->m_shapeStr);
							itemWnd->CreateEx(	WS_EX_TOPMOST ,	
												AfxRegisterWndClass(0),
												LPCTSTR(pTitle),
												WS_POPUP | WS_SYSMENU,
												itemWnd->m_rectWnd,
												NULL,
												NULL,
												NULL );	

							if (layoutArrayPtr)
								layoutArrayPtr->Add(itemWnd);
														
							
						}

					}

				}

			
			}


			//Future Enhancements
			if (layoutarrayversion>100)
			{



			}

	return ret;

}