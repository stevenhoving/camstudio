// ScreenAnnotations.cpp : implementation file
//

#include "stdafx.h"
#include "vscap.h"
#include "ScreenAnnotations.h"
#include "ListManager.h"
#include "NewShape.h"
#include "LayoutList.h"
#include "Keyshortcuts.h"

CListManager gList;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define modeShape 0
#define modeLayout 1

int AreWindowsEdited();

int objectIsCopied  = 0;
int layoutIsCopied  = 0;
int tabMode = modeShape;
int shapeLibraryMode = 0;  //default shape library mode is 0, if >0 ==> opened library 
int layoutLibraryMode = 0; //default layout library mode is 0, if >0 ==> opened library 

int editingLabelOn = 0;
int newObjectOn = 0;

LV_ITEM itemCopied;
CTransparentWnd* wndCopied = NULL;

CLayoutList* layoutCopied = NULL;

void AdjustShapeName(CString& shapeName);
void AdjustLayoutName(CString& layoutName);

int draggingOn = 0;
int m_DragIndex = -1;
HCURSOR cursorDrag_SA = NULL;
HCURSOR cursorArrow_SA;

CImageList  saImageList;
int saImageListLoaded = 0;


extern int m_newShapeWidth;
extern int m_newShapeHeight;
extern CString m_newShapeText;
extern CString m_imageDir;
extern CString m_imageFilename;
extern int m_imagetype;
extern CString shapeStr; 

extern int shapeNameInt;
extern CString shapeName;

extern int layoutNameInt;
extern CString g_layoutName;
extern int keySCOpened;
extern CString GetProgPath();

extern void ErrMsg(char format[], ...);

extern int currentLayout;

extern int MessageOutINT(HWND hWnd,long strMsg, long strTitle, UINT mbstatus,long val);
extern int MessageOutINT2(HWND hWnd,long strMsg, long strTitle, UINT mbstatus,long val1,long val2);
extern int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus);
extern int SetAdjustHotKeys();

/////////////////////////////////////////////////////////////////////////////
// CScreenAnnotations dialog


CScreenAnnotations::CScreenAnnotations(CWnd* pParent /*=NULL*/)
	: CDialog(CScreenAnnotations::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScreenAnnotations)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	
}


void CScreenAnnotations::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScreenAnnotations)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScreenAnnotations, CDialog)
	//{{AFX_MSG_MAP(CScreenAnnotations)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST1, OnBegindragList1)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnRclickList1)
	ON_COMMAND(ID_EDITOBJ_NEWOBJECT, OnEditobjNewobject)
	ON_COMMAND(ID_EDITOBJ_EDITIMAGE, OnEditobjEditimage)
	ON_COMMAND(ID_EDITOBJ_EDITTEXT, OnEditobjEdittext)
	ON_COMMAND(ID_EDITOBJ_EDITTRANSPARENCY, OnEditobjEdittransparency)
	ON_COMMAND(ID_EDITOBJ_COPY, OnEditobjCopy)
	ON_COMMAND(ID_EDITOBJ_PASTE, OnEditobjPaste)
	ON_COMMAND(ID_EDITOBJ_REMOVE, OnEditobjRemove)
	ON_COMMAND(ID_EDITOBJ_TESTEDIT, OnEditobjTestedit)
	ON_WM_KEYDOWN()
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST1, OnEndlabeleditList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST1, OnKeydownList1)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab1)
	ON_COMMAND(ID_EDITLAYOUT_SAVELAYOUT, OnEditlayoutSavelayout)
	ON_COMMAND(ID_EDITLAYOUT_OPENLAYOUT, OnEditlayoutOpenlayout)
	ON_COMMAND(ID_EDITLAYOUT_CLOSEALLOBJECTS, OnEditlayoutCloseallobjects)
	ON_COMMAND(ID_EDITLAYOUT_LAYOUTSHORTCUTS, OnEditlayoutLayoutshortcuts)
	ON_COMMAND(ID_EDITOBJ_LIBRARY_CLOSELIBRARY, OnEditobjLibraryCloselibrary)
	ON_COMMAND(ID_EDITOBJ_LIBRARY_OPENLIBRARY, OnEditobjLibraryOpenlibrary)
	ON_COMMAND(ID_EDITOBJ_LIBRARY_NEWLIBRARY, OnEditobjLibraryNewlibrary)
	ON_COMMAND(ID_EDITLAYOUT_LIBRARY_NEWLIBRARY, OnEditlayoutLibraryNewlibrary)
	ON_COMMAND(ID_EDITLAYOUT_LIBRARY_OPENLIBRARY, OnEditlayoutLibraryOpenlibrary)
	ON_COMMAND(ID_EDITLAYOUT_LIBRARY_CLOSELIBRARY, OnEditlayoutLibraryCloselibrary)
	ON_COMMAND(ID_HELP_HELP, OnHelpHelp)
	ON_COMMAND(ID_OPTIONS_CLOSESCREENANNOTATIONS, OnOptionsClosescreenannotations)
	ON_BN_CLICKED(IDNEXT, OnNext)
	ON_COMMAND(ID_HELP_SHAPES, OnHelpShapes)
	ON_COMMAND(ID_HELP_SHAPETOPICS_INSTANTIATINGASHAPE, OnHelpShapetopicsInstantiatingashape)
	ON_COMMAND(ID_HELP_SHAPETOPICS_EDITINGASHAPE, OnHelpShapetopicsEditingashape)
	ON_COMMAND(ID_HELP_SHAPETOPICS_EDITINGTEXT, OnHelpShapetopicsEditingtext)
	ON_COMMAND(ID_HELP_SHAPETOPICS_EDITINGIMAGE, OnHelpShapetopicsEditingimage)
	ON_COMMAND(ID_HELP_SHAPETOPICS_CREATINGANEWSHAPE, OnHelpShapetopicsCreatinganewshape)
	ON_COMMAND(ID_HELP_SHAPETOPICS_EDITINGTRANSPARENCY, OnHelpShapetopicsEditingtransparency)
	ON_COMMAND(ID_HELP_SHAPETOPICS_MANAGINGSHAPES, OnHelpShapetopicsManagingshapes)
	ON_COMMAND(ID_HELP_SHAPETOPICS_RESIZINGSHAPES, OnHelpShapetopicsResizingshapes)
	ON_COMMAND(ID_HELP_LAYOUTS, OnHelpLayouts)	
	ON_COMMAND(IDR_ANN_SAVELAYOUT, OnAnnSavelayout)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_LIBRARY_OPENSHAPELIBRARY, OnLibraryOpenshapelibrary)
	ON_COMMAND(ID_LIBRARY_SAVESHAPELIBRARY, OnLibrarySaveshapelibrary)
	ON_COMMAND(ID_LIBRARY_NEWSHAPELIBRARY, OnLibraryNewshapelibrary)
	ON_COMMAND(ID_LIBRARY_NEWLAYOUTLIBRARY, OnLibraryNewlayoutlibrary)
	ON_COMMAND(ID_LIBRARY_OPENLAYOUTLIBRARY, OnLibraryOpenlayoutlibrary)
	ON_COMMAND(ID_LIBRARY_SAVELAYOUTLIBRARY, OnLibrarySavelayoutlibrary)
	ON_COMMAND(ID_OBJECTS_COPY, OnObjectsCopy)
	ON_COMMAND(ID_OBJECTS_PASTE, OnObjectsPaste)
	ON_COMMAND(ID_OBJECTS_REMOVE, OnObjectsRemove)
	ON_WM_CREATE()
	ON_COMMAND(ID_OBJECTS_EDITTEXT, OnObjectsEdittext)
	ON_COMMAND(ID_OBJECTS_EDITIMAGE, OnObjectsEditimage)
	ON_COMMAND(ID_OBJECTS_EDITNAME, OnObjectsEditname)
	ON_COMMAND(ID_OBJECTS_EDITTRANSAPRENCY, OnObjectsEdittransaprency)
	ON_COMMAND(ID_OBJECTS_CLOSEALLOBJECTS, OnObjectsCloseallobjects)
	ON_COMMAND(ID_LIBRARY_INSERTSHAPELIBARAY, OnLibraryInsertshapelibaray)
	ON_COMMAND(ID_OBJECTS_MOVEITEMUP, OnObjectsMoveitemup)
	ON_COMMAND(ID_OBJECTS_MOVEITEMDOWN, OnObjectsMoveitemdown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScreenAnnotations message handlers

void CScreenAnnotations::OnOK() 
{
	// TODO: Add extra validation here
	ShowWindow(SW_HIDE);
	
	//CDialog::OnOK();
}

BOOL CScreenAnnotations::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString shapesStr;
	CString layoutStr;

	shapesStr.LoadString(IDS_STRING_SHAPES);
	layoutStr.LoadString(IDS_STRING_LAYOUT);

	
	TC_ITEM item1, item2;
	item1.mask = TCIF_TEXT;
	//item1.pszText = "Shapes";
	item1.pszText = (char *) LPCTSTR(shapesStr);
	item2.mask = TCIF_TEXT;
	//item2.pszText = "Layout";
	item2.pszText = (char *) LPCTSTR(layoutStr);


	((CTabCtrl *) GetDlgItem (IDC_TAB1))->InsertItem(0, &item1);
	((CTabCtrl *) GetDlgItem (IDC_TAB1))->InsertItem(1, &item2);



	editingLabelOn = 0;
	draggingOn = 0;

	if (cursorDrag_SA == NULL)
		cursorDrag_SA = LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CURSORDRAG));

	if (cursorArrow_SA == NULL)
		cursorArrow_SA = LoadCursor(NULL,IDC_ARROW);	

	((CTabCtrl *) GetDlgItem (IDC_TAB1))->SetCurSel(tabMode);
	//UpdateTabCtrl(tabMode);

	if (!saImageListLoaded)
	{
		//saBitmap.LoadBitmap(IDB_SCREENANNOTATIONS_SMALL);
		//saImageList.Create(15,15, ILC_COLOR8 | ILC_MASK, 8, 8);
		//saImageList.Create( IDB_SCREENANNOTATIONS_SMALL, 16, 10, RGB(255,0, 255) );		
		saImageList.Create( IDB_SCREENANNOTATIONS_SMALL, 16, 10, RGB(255,255, 255) );		
		saImageListLoaded = 1;
		((CListCtrl *) GetDlgItem (IDC_LIST1))->SetImageList( &saImageList, LVSIL_SMALL );

	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}






void CScreenAnnotations::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	
}

void CScreenAnnotations::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{

	int nItem = -1;
	int numitems = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemCount();
	int itemstate = 0;

	for (int i=0; i < numitems; i++) {
	
		itemstate = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemState( i, LVIS_SELECTED); 
		if (itemstate & LVIS_SELECTED)  {
			nItem = i;
		}
	}


	CMenu menu;
	int menuToLoad = IDR_CONTEXTEDITOBJ;	

	POINT point;
	GetCursorPos(&point);

	
	//Layout Mode
	if (tabMode  == modeLayout) {
		
		menuToLoad = IDR_CONTEXTEDITLAYOUT;

		if (menu.LoadMenu(menuToLoad))
		{
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);

			
			if (layoutIsCopied)
			{
				pPopup->EnableMenuItem(ID_EDITOBJ_PASTE,MF_ENABLED|MF_BYCOMMAND);

			}
			else
			{
				pPopup->EnableMenuItem(ID_EDITOBJ_PASTE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);

			}

			if (nItem<0)  //item not selected
			{ 
				pPopup->EnableMenuItem(ID_EDITLAYOUT_OPENLAYOUT,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
				pPopup->EnableMenuItem(ID_EDITOBJ_REMOVE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
				pPopup->EnableMenuItem(ID_EDITOBJ_COPY,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
				pPopup->EnableMenuItem(ID_EDITOBJ_TESTEDIT, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
							
			}
			else
			{
				pPopup->EnableMenuItem(ID_EDITLAYOUT_OPENLAYOUT,MF_ENABLED|MF_BYCOMMAND);
				pPopup->EnableMenuItem(ID_EDITOBJ_REMOVE,MF_ENABLED|MF_BYCOMMAND);
				pPopup->EnableMenuItem(ID_EDITOBJ_COPY, MF_ENABLED|MF_BYCOMMAND);	
				pPopup->EnableMenuItem(ID_EDITOBJ_TESTEDIT, MF_ENABLED|MF_BYCOMMAND);
				
			}
			
			int max = gList.displayArray.GetSize();
			int isEdited = AreWindowsEdited();
			
			if ((isEdited) || (max<=0))
			{
				
				pPopup->EnableMenuItem(ID_EDITLAYOUT_SAVELAYOUT,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);

			}
			else
			{
				pPopup->EnableMenuItem(ID_EDITLAYOUT_SAVELAYOUT,MF_ENABLED|MF_BYCOMMAND);

			}
			

			if (isEdited)
				pPopup->EnableMenuItem(ID_EDITLAYOUT_OPENLAYOUT,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
	


			pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN,
							   point.x, point.y,
							   this); // route commands through main window


		} //End loadMenu Successful

		*pResult = 0;
		return;
	}
	//End Layout Mode

	

	//Shape Mode
	if (menu.LoadMenu(menuToLoad))
	{
		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);

		if ((tabMode==modeLayout) || (nItem<0) || (newObjectOn))
		{
			pPopup->EnableMenuItem(ID_EDITOBJ_EDITIMAGE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
			pPopup->EnableMenuItem(ID_EDITOBJ_EDITTEXT, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
			pPopup->EnableMenuItem(ID_EDITOBJ_EDITTRANSPARENCY, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
			pPopup->EnableMenuItem(ID_EDITOBJ_TESTEDIT, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
			
		}
		else
		{

			pPopup->EnableMenuItem(ID_EDITOBJ_EDITIMAGE,MF_ENABLED|MF_BYCOMMAND);
			pPopup->EnableMenuItem(ID_EDITOBJ_EDITTEXT, MF_ENABLED|MF_BYCOMMAND);
			pPopup->EnableMenuItem(ID_EDITOBJ_EDITTRANSPARENCY, MF_ENABLED|MF_BYCOMMAND);
			pPopup->EnableMenuItem(ID_EDITOBJ_TESTEDIT, MF_ENABLED|MF_BYCOMMAND);
		

		}

		
		if ((objectIsCopied) && (tabMode==modeShape))
		{
			pPopup->EnableMenuItem(ID_EDITOBJ_PASTE,MF_ENABLED|MF_BYCOMMAND);

		}
		else if ((layoutIsCopied) && (tabMode==modeLayout))
		{
			pPopup->EnableMenuItem(ID_EDITOBJ_PASTE,MF_ENABLED|MF_BYCOMMAND);

		}
		else
		{
			pPopup->EnableMenuItem(ID_EDITOBJ_PASTE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);

		}

		if (nItem<0)  //item not selected
		{ 
			pPopup->EnableMenuItem(ID_EDITOBJ_REMOVE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
			pPopup->EnableMenuItem(ID_EDITOBJ_COPY,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
						
		}
		else
		{
			pPopup->EnableMenuItem(ID_EDITOBJ_REMOVE,MF_ENABLED|MF_BYCOMMAND);
			pPopup->EnableMenuItem(ID_EDITOBJ_COPY, MF_ENABLED|MF_BYCOMMAND);			
			
		}

		if (newObjectOn)
		{
			pPopup->EnableMenuItem(ID_EDITOBJ_NEWOBJECT,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
			
			pPopup->EnableMenuItem(ID_EDITOBJ_REMOVE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
			pPopup->EnableMenuItem(ID_EDITOBJ_COPY,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
			pPopup->EnableMenuItem(ID_EDITOBJ_PASTE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);

			pPopup->EnableMenuItem(ID_EDITOBJ_EDITIMAGE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
			pPopup->EnableMenuItem(ID_EDITOBJ_EDITTEXT, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
			pPopup->EnableMenuItem(ID_EDITOBJ_EDITTRANSPARENCY, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
						
		}
		else 
		{
			pPopup->EnableMenuItem(ID_EDITOBJ_NEWOBJECT,MF_ENABLED|MF_BYCOMMAND);

			//pPopup->EnableMenuItem(ID_EDITOBJ_LIBRARY_NEWLIBRARY,MF_ENABLED|MF_BYCOMMAND);
			//pPopup->EnableMenuItem(ID_EDITOBJ_LIBRARY_OPENLIBRARY,MF_ENABLED|MF_BYCOMMAND);

		}


		pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN,
							   point.x, point.y,
							   this); // route commands through main window
	}



	

	*pResult = 0;

}



void CScreenAnnotations::CloseAllWindows(int wantDelete)
{

	int max = gList.displayArray.GetSize();
	CTransparentWnd * itemWnd = NULL;
	for (int i=max-1;i>=0; i--)
	{
		itemWnd = gList.displayArray[i];		
		if (itemWnd)
		{
			if ((itemWnd->trackingOn) || (itemWnd->editTransOn) || (itemWnd->editImageOn))
			{
				//Do not remove, the item is undergoing editing
				
			}
			else
			{
			
				itemWnd->ShowWindow(SW_HIDE);			
				if (wantDelete) {				
										
					gList.RemoveDisplayArray(itemWnd,1);
				
				}

			}
						
		}
	}


}

void CScreenAnnotations::OnEditobjNewobject() 
{

	newObjectOn = 1;

	
	CNewShape newshapeDlg;
	if (newshapeDlg.DoModal() == IDOK)
	{

		CTransparentWnd *newWnd;
		newWnd = new CTransparentWnd;
		
		int x = (rand() % 100) + 100;
		int y = (rand() % 100) + 100;
		CRect rect;
		if (m_imagetype==0) 
		{
						
			rect.left = x;
			rect.top = y;
			rect.right = rect.left + m_newShapeWidth - 1;
			rect.bottom = rect.top + m_newShapeHeight - 1;
			newWnd->m_textstring = m_newShapeText;
			newWnd->m_shapeStr = shapeStr;
			newWnd->CreateTransparent(shapeStr,rect,NULL);


		}		
		else 
		{

			rect.left = x;
			rect.top = y;
			rect.right = x  + 100;
			rect.bottom = y + 100;
			newWnd->m_textstring = m_newShapeText;			
			newWnd->m_shapeStr = shapeStr;
			newWnd->CreateTransparent(shapeStr,rect,m_imageFilename,1);


		}				
			
		
		if (newWnd)
		{			
			
			CTransparentWnd *cloneWnd = newWnd->Clone(0,0);			
			gList.AddDisplayArray(cloneWnd);

			cloneWnd->ShowWindow(SW_SHOW);
			cloneWnd->UpdateWindow();

			//Use fill info for items, the wnd*, the text, the icons...etc
			TabSelectShapeMode(1);
			if (tabMode == modeShape)
			{

				//A shape window can never be shown....else crash...
				gList.AddShapeArray(newWnd);

				
				CListCtrl *listctrl = ((CListCtrl *) GetDlgItem (IDC_LIST1));

				
				int nItem = listctrl->GetItemCount();
				int nItemMod = modeShape; 
				
				CString insstr = cloneWnd->m_shapeStr;
				int ins = listctrl->InsertItem( LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE, nItem, LPCTSTR(insstr), 0, 0, nItemMod, newWnd->uniqueID );
								

			}
		}
		

		
	}
	

	newObjectOn = 0;
	
}

void CScreenAnnotations::OnEditobjEditimage() 
{
	// TODO: Add your command handler code here
		// TODO: Add your command handler code here
	CTransparentWnd * editWnd  = LocateWndFromShapeList(); 
	editWnd->saveMethod = saveMethodReplace;
	editWnd->ShowWindow(SW_RESTORE);

	editWnd->EditImage();

	editWnd->saveMethod = saveMethodNew;
	SaveShapeReplace(editWnd);

	
}

void CScreenAnnotations::OnEditobjEdittext() 
{

	CTransparentWnd * editWnd  = LocateWndFromShapeList(); 
	editWnd->saveMethod = saveMethodReplace;
	editWnd->ShowWindow(SW_RESTORE);

	editWnd->EditText();

	editWnd->saveMethod = saveMethodNew;
	SaveShapeReplace(editWnd);


	
}



void CScreenAnnotations::SaveLayoutNew()
{
	//set to layout mode for tabctrl
	TabSelectLayoutMode(1);

	
	CArray<CTransparentWnd *,CTransparentWnd *> *cloneArray = gList.CloneDisplayArray();	
	if (cloneArray)
	{
		CLayoutList * newLayout = NULL;
		newLayout = new CLayoutList;
		if (newLayout) {

			newLayout->layoutArrayPtr = cloneArray;
			
			CString layoutIntStr;
			layoutIntStr.Format("%d",layoutNameInt);
			newLayout->layoutName = g_layoutName + layoutIntStr;
			layoutNameInt++ ;
			if (layoutNameInt > 2147483647)
				layoutNameInt = 1;			
			
			
			gList.AddLayoutArray(newLayout);
			
			
			CListCtrl *listctrl = ((CListCtrl *) GetDlgItem (IDC_LIST1));						
			int nItem = listctrl->GetItemCount();
			int nItemMod = modeLayout;  //refering to layout images
			CString insstr = newLayout->layoutName;
			int ins = listctrl->InsertItem( LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE, nItem, LPCTSTR(insstr), 0, 0, nItemMod, newLayout->uniqueID );			

			listctrl->EnsureVisible( nItem, FALSE );
			listctrl->SetItemState( nItem, LVIS_SELECTED, LVIS_SELECTED); 

		}

	}

}

void CScreenAnnotations::SaveShapeNew(CTransparentWnd *newWnd)
{
	//set to shape mode for tabctrl
	TabSelectShapeMode(1);

	if (newWnd)
	{
		CTransparentWnd *cloneWnd = newWnd->Clone(0,0);			
		gList.AddShapeArray(cloneWnd);				

		CListCtrl *listctrl = ((CListCtrl *) GetDlgItem (IDC_LIST1));				
		
		int nItem = listctrl->GetItemCount();
		int nItemMod = modeShape; 


		CString shapeStr;
		shapeStr.Format("%d",shapeNameInt);
		shapeNameInt++;
		if (shapeNameInt>2147483600) //assume int32
			shapeNameInt = 1;

		shapeStr = shapeName + shapeStr;	
		CString insstr = shapeStr;

		insstr.TrimLeft();
		insstr.TrimRight();
		if (insstr == "")
			insstr = cloneWnd->m_shapeStr;
		else {

			int val = insstr.Find( '\n' ); 
			if (val > 0) {
				insstr = insstr.Left(val-1);
				insstr = insstr  + " ...";
			}

		}
		
		cloneWnd->m_shapeStr = insstr;
		int ins = listctrl->InsertItem( LVIF_TEXT | LVIF_PARAM , nItem, LPCTSTR(insstr), 0, 0, nItemMod, cloneWnd->uniqueID );	

		listctrl->EnsureVisible( nItem, FALSE );
		listctrl->SetItemState( nItem, LVIS_SELECTED, LVIS_SELECTED); 
		

	}

}






void CScreenAnnotations::SaveShapeReplace(CTransparentWnd *newWnd)
{
	//set to shape mode for tabctrl
	TabSelectShapeMode(1);

	//assume the newWnd is the window that is edited
	if (newWnd)
	{			
		
		newWnd->ShowWindow(SW_HIDE);
		//listctrl item is not affected because the shapestr and position and lParam/uniqueID remains the same						
	}

}

void CScreenAnnotations::OnEditobjEdittransparency() 
{

	CTransparentWnd * editWnd  = LocateWndFromShapeList(); 
	editWnd->saveMethod = saveMethodReplace;
	editWnd->ShowWindow(SW_RESTORE);

	editWnd->EditTransparency();

	editWnd->saveMethod = saveMethodNew;
	SaveShapeReplace(editWnd);

	
}

void CScreenAnnotations::OnEditobjCopy() 
{

	if (tabMode == modeShape)
	{

		CTransparentWnd *newWnd = LocateWndFromShapeList(); 
	
		if (newWnd) {		

			if (objectIsCopied)
			{

				if (wndCopied)
				{
			
					wndCopied->ShowWindow(SW_HIDE);			
					
					wndCopied->DestroyWindow();			
					delete wndCopied;

					wndCopied = NULL;
					objectIsCopied = 0;

				}

			}
			
			wndCopied = newWnd->Clone(0,0);

			if (wndCopied) {
				objectIsCopied = 1;

			}
			else
				objectIsCopied = 0;

		}		
	}
	else if (tabMode == modeLayout)
	{

		int nItem = -1;
		int numitems = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemCount();
		int itemstate = 0;

		for (int i=0; i < numitems; i++) {
		
			//GetItemState( int nItem, LVIS_SELECTED ) 
			itemstate = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemState( i, LVIS_SELECTED); 
			if (itemstate & LVIS_SELECTED)  {
				nItem = i;
			}
		}



		CLayoutList * newLayout = NULL;
		if ((nItem>-1) && (nItem<numitems)) 
		{

			CLayoutList * itemLayout = NULL;
			itemLayout = LocateLayoutFromItem(nItem); 

			newLayout = gList.CloneLayout(itemLayout);

		}


		if (newLayout)
		{					
			if (layoutCopied)
			{
		
				gList.DestroyLayout(layoutCopied);
				layoutCopied = NULL;
				layoutIsCopied = 0;

			}

			layoutCopied = newLayout;

			if (layoutCopied) 
				layoutIsCopied = 1;		
			else
				layoutIsCopied = 0;

		}



	}


}

void CScreenAnnotations::OnEditobjPaste() 
{

	if (tabMode == modeShape)
	{		
		if (objectIsCopied)
		{
			if (wndCopied)
			{

					CTransparentWnd *cloneWnd = wndCopied->Clone(0,0);			
					gList.AddShapeArray(cloneWnd);				

					CListCtrl *listctrl = ((CListCtrl *) GetDlgItem (IDC_LIST1));				
					
					int nItem = listctrl->GetItemCount();
					int nItemMod = modeShape; 
					int ins = listctrl->InsertItem( LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE, nItem, LPCTSTR(cloneWnd->m_shapeStr), 0, 0, nItemMod, cloneWnd->uniqueID );
					listctrl->EnsureVisible( nItem, FALSE );
					listctrl->SetItemState( nItem, LVIS_SELECTED, LVIS_SELECTED); 

			}		
		}

	}
	else if (tabMode == modeLayout)
	{		
		if (layoutIsCopied)
		{
			if (layoutCopied)
			{

					CLayoutList *cloneLayout = gList.CloneLayout(layoutCopied);

					if (cloneLayout)
					{
						gList.AddLayoutArray(cloneLayout);				

						CListCtrl *listctrl = ((CListCtrl *) GetDlgItem (IDC_LIST1));	
						
						int nItem = listctrl->GetItemCount();
						int nItemMod = modeLayout; 
						int ins = listctrl->InsertItem( LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE, nItem, LPCTSTR(cloneLayout->layoutName), 0, 0, nItemMod, cloneLayout->uniqueID );
						listctrl->EnsureVisible( nItem, FALSE );
						listctrl->SetItemState( nItem, LVIS_SELECTED, LVIS_SELECTED); 

					}

			}		
		}

	}
	
}

void CScreenAnnotations::OnEditobjRemove() 
{

	int nItem = -1;
	int numitems = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemCount();
	int itemstate = 0;

	for (int i=0; i < numitems; i++) {
	
		//GetItemState( int nItem, LVIS_SELECTED ) 
		itemstate = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemState( i, LVIS_SELECTED); 
		if (itemstate & LVIS_SELECTED)  {
			nItem = i;
		}
	}

	CTransparentWnd *newWnd = NULL;
	CLayoutList *newLayout = NULL;
	if (tabMode == modeShape)
	{
	
		if ((nItem>-1) && (nItem<numitems)) {
		

			newWnd = LocateWndFromItem(nItem); 


		}

		if (newWnd) {

			gList.RemoveShapeArray(newWnd,1);
			((CListCtrl *) GetDlgItem (IDC_LIST1))->DeleteItem(  nItem );


		}	

	}
	else if (tabMode == modeLayout)
	{
	
		if ((nItem>-1) && (nItem<numitems)) {
		
			newLayout = LocateLayoutFromItem(nItem); 


		}

		if (newLayout) {

			gList.RemoveLayoutArray(newLayout,1);
			((CListCtrl *) GetDlgItem (IDC_LIST1))->DeleteItem(  nItem );


		}	

	}

}

void CScreenAnnotations::OnEditobjTestedit() 
{
	
	int nItem = -1;
	int numitems = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemCount();
	int itemstate = 0;


	for (int i=0; i < numitems; i++) {
	
		itemstate = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemState( i, LVIS_SELECTED); 
		if (itemstate & LVIS_SELECTED)  {
			nItem = i;
		}
	}



	if ((nItem>-1) && (nItem<numitems)) {	
		
		editingLabelOn = 1;
		((CListCtrl *) GetDlgItem (IDC_LIST1))->EditLabel( nItem );

	}

	
}


void CScreenAnnotations::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{

	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CScreenAnnotations::OnCancel() 
{

	
	if (editingLabelOn)	
		CDialog::OnCancel();
	else
		return;
}

void CScreenAnnotations::OnEndlabeleditList1(NMHDR* pNMHDR, LRESULT* pResult) 
{

	editingLabelOn = 0;
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if ((pDispInfo->item).pszText == NULL) {		

			pResult = FALSE;
			return;


	}
	else if (tabMode == modeShape)	
	{

			//Get Transparent Window
			//Change the shapeStr of the Transparent Window
			CTransparentWnd *newWnd = LocateWndFromShapeList(); 
	
			if (newWnd) {		

				newWnd->m_shapeStr = (pDispInfo->item).pszText;

				if (newWnd->m_shapeStr != shapeStr) 
				{
					//shape name has been changed, reset counter to 1
					shapeName = newWnd->m_shapeStr;

					//a better method is to extract the trailing number from shapestr and use it as number for shapeNameInt
					AdjustShapeName(shapeName);								

				}
				else {
					
					shapeNameInt++;
					if (shapeNameInt>2147483600) //assume int32
						shapeNameInt = 1;
				}


				*pResult = TRUE;
				return;

			}
			

			


	}
	else if (tabMode == modeLayout)	
	{

		CLayoutList *itemLayout = NULL;
		
		int nItem = -1;
		int numitems = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemCount();
		int itemstate = 0;

		for (int i=0; i < numitems; i++) {	
			
 			itemstate = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemState( i, LVIS_SELECTED); 
			if (itemstate & LVIS_SELECTED)  {			
				nItem = i;
			}
		}

		
		if ((nItem>-1) && (nItem<numitems)) {		
			itemLayout = LocateLayoutFromItem(nItem); 
		}


		if (itemLayout)
		{

			itemLayout->layoutName = (pDispInfo->item).pszText;

			if (itemLayout->layoutName != g_layoutName) 
			{
					//shape name has been changed, reset counter to 1
					g_layoutName = itemLayout->layoutName;

					//a better method is to extract the trailing number from shapestr and use it as number for layoutNameInt
					AdjustLayoutName(g_layoutName);								

			}
			else {
					
					layoutNameInt++;
					if (layoutNameInt>2147483600) //assume int32
						layoutNameInt = 1;
			}

			*pResult = TRUE;
			return;


		}

	}	

}

void CScreenAnnotations::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{

	if (tabMode == modeShape)
	{
		int x =(rand() % 200) - 100;
		int y =(rand() % 200) - 100;
		InstantiateWnd(x,y) ;
	}
	else if (tabMode == modeLayout)
	{
		InstantiateLayout(); 
	}
	
	*pResult = 0;
}

void CScreenAnnotations::InstantiateWnd(int x, int y) 
{

	if (tabMode == modeShape)
	{

		CTransparentWnd *newWnd = LocateWndFromShapeList(); 
		
		if (newWnd) {		
			
			CTransparentWnd *cloneWnd = newWnd->Clone(x,y);			
			gList.AddDisplayArray(cloneWnd);
			
			cloneWnd->InvalidateRegion();
			cloneWnd->InvalidateTransparency();
			cloneWnd->ShowWindow(SW_SHOW);
			cloneWnd->UpdateWindow();

		}
	}
	
	
}


CTransparentWnd* CScreenAnnotations::LocateWndFromShapeList() 
{
	CTransparentWnd *newWnd = NULL;

	int nItem = -1;
	int numitems = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemCount();
	int itemstate = 0;


	for (int i=0; i < numitems; i++) {	

		itemstate = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemState( i, LVIS_SELECTED); 
		if (itemstate & LVIS_SELECTED)  {
			nItem = i;
		}
	}

	
	if ((nItem>-1) && (nItem<numitems)) {
		newWnd = LocateWndFromItem(nItem); 
	}

	return newWnd;

}

CTransparentWnd* CScreenAnnotations::LocateWndFromItem(int nItem) 
{

	CTransparentWnd *newWnd = NULL;
	long WndID = -1;
	
	LV_ITEM  searchItem;
	searchItem.mask = LVIF_PARAM ;
	searchItem.iItem = nItem;
	((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItem( &searchItem); 
	
	WndID = searchItem.lParam;

	int found = 0;
	int max = gList.shapeArray.GetSize();
	CTransparentWnd * itemWnd = NULL;
	for (int i=0;i<max; i++)
	{
		itemWnd = gList.shapeArray[i];
		if (itemWnd)
		{
			
			if (itemWnd->uniqueID == WndID)
			{
				newWnd = itemWnd;
				found = 1;
				break;  //allow only one removal

			}
			
		}
	}
	

	return newWnd;


}

void CScreenAnnotations::OnKeydownList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	if (pLVKeyDow->wVKey == VK_F2) {

		OnEditobjTestedit(); 
	}

	
	
	*pResult = 0;
}

void CScreenAnnotations::OnDestroy() 
{
	CDialog::OnDestroy();
	
	if (objectIsCopied)
	{
		if (wndCopied)
		{	

			wndCopied->ShowWindow(SW_HIDE);			
			
			wndCopied->DestroyWindow();			
			delete wndCopied;

			wndCopied = NULL;
			objectIsCopied = 0;

		}
	}


	if (layoutIsCopied)
	{

		if (layoutCopied)
		{	
			gList.DestroyLayout(layoutCopied);

		}


	}	

	
}


void CScreenAnnotations::OnBegindragList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	m_DragIndex = pNMListView->iItem;    

	CListCtrl * listctrl =  ((CListCtrl *) GetDlgItem (IDC_LIST1));
	
	draggingOn = 1; 
	SetCursor(cursorDrag_SA);
	SetCapture();
	

		
	*pResult = 0;
}


void CScreenAnnotations::OnMouseMove(UINT nFlags, CPoint point) 
{

	if (draggingOn) {

		SetCursor(cursorDrag_SA);
	}

	
	CDialog::OnMouseMove(nFlags, point);
}

void CScreenAnnotations::OnLButtonUp(UINT nFlags, CPoint point) 
{
	
	if (draggingOn) {
		
		ReleaseCapture ();
		draggingOn = 0;
		SetCursor(cursorArrow_SA);	

		CPoint pt (point); //Get current mouse coordinates
	    ClientToScreen (&pt); //Convert to screen coordinates
        CWnd* pDropWnd = WindowFromPoint (pt);

		CWnd* m_pDragList = ((CListCtrl *) GetDlgItem (IDC_LIST1));

		if (tabMode == modeShape) 
		{
				if (pDropWnd != m_pDragList) {
					CTransparentWnd *newWnd = LocateWndFromShapeList(); 
			
					if (newWnd) {		
						
						GetCursorPos(&pt);

						int wx = (newWnd->m_rectWnd.Width())  / 2;
						int hx = (newWnd->m_rectWnd.Height()) / 2;
						CTransparentWnd *cloneWnd = newWnd->CloneByPos(pt.x - wx, pt.y - hx);	
						gList.AddDisplayArray(cloneWnd);
						
						cloneWnd->InvalidateRegion();
						cloneWnd->InvalidateTransparency();
						cloneWnd->ShowWindow(SW_SHOW);
						cloneWnd->UpdateWindow();

					}

				}		
				else
				{

					//Locate Selected Item
					int nItem = -1;
					int numitems = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemCount();
					int itemstate = 0;

					for (int i=0; i < numitems; i++) {					

						itemstate = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemState( i, LVIS_SELECTED); 
						if (itemstate & LVIS_SELECTED)  {
							nItem = i;
						}
					}
	
					if ((nItem>-1) && (nItem<numitems)) {

						CPoint ptx;
						GetCursorPos(&ptx);
						int hotspotx = 13;
						int hotspoty = 10;
						ptx.x += hotspotx;
						ptx.y += hotspoty;
						((CListCtrl *) GetDlgItem (IDC_LIST1))->ScreenToClient(&ptx);
						UINT lvFlags = LVHT_ONITEM; 
						int targetItem = ((CListCtrl *) GetDlgItem (IDC_LIST1))->HitTest( ptx, &lvFlags );						

						if (( targetItem >= 0) && (targetItem !=nItem))
						{
							
							int nItemMod = modeShape;

							LV_ITEM  swapItem, swapItem2;
							swapItem.mask = LVIF_PARAM;
							swapItem.iItem = nItem;
							((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItem( &swapItem); 														

							swapItem2.mask = LVIF_PARAM;
							swapItem2.iItem = targetItem;
							((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItem( &swapItem2); 

							gList.SwapShapeArray(swapItem.lParam, swapItem2.lParam);
							RefreshShapeList();						
							


						}


					}

				}
		}
		else if (tabMode == modeLayout) 
		{
				if (pDropWnd != m_pDragList) {
					
					InstantiateLayout(); 

				}		
				else
				{

					//Locate Selected Item
					int nItem = -1;
					int numitems = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemCount();
					int itemstate = 0;

					for (int i=0; i < numitems; i++) {					

						itemstate = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemState( i, LVIS_SELECTED); 
						if (itemstate & LVIS_SELECTED)  {
							nItem = i;
						}
					}
	
					if ((nItem>-1) && (nItem<numitems)) {
						

						//back here
						CPoint ptx;
						GetCursorPos(&ptx);
						int hotspotx = 13;
						int hotspoty = 10;
						ptx.x += hotspotx;
						ptx.y += hotspoty;
						((CListCtrl *) GetDlgItem (IDC_LIST1))->ScreenToClient(&ptx);
						UINT lvFlags = LVHT_ONITEM; 
						int targetItem = ((CListCtrl *) GetDlgItem (IDC_LIST1))->HitTest( ptx, &lvFlags );						


						if (( targetItem >= 0) && (targetItem !=nItem))
						{
							
							int nItemMod = modeLayout;

							LV_ITEM  swapItem, swapItem2;
							swapItem.mask = LVIF_PARAM;
							swapItem.iItem = nItem;
							((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItem( &swapItem); 

							swapItem2.mask = LVIF_PARAM;
							swapItem2.iItem = targetItem;
							((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItem( &swapItem2); 

							gList.SwapLayoutArray(swapItem.lParam, swapItem2.lParam);
							RefreshLayoutList();													

						}


					}

				}
		}



	}

	
	CDialog::OnLButtonUp(nFlags, point);
}


void CScreenAnnotations::RefreshShapeList()
{
		//tabmode must be set to shapeMode before calling this

		CListCtrl *listctrl = ((CListCtrl *) GetDlgItem (IDC_LIST1));	
		listctrl->DeleteAllItems();
		
		int max = gList.shapeArray.GetSize();
		CTransparentWnd * itemWnd = NULL;
		for (int i=0;i<max; i++)
		{
			itemWnd = gList.shapeArray[i];
			if (itemWnd)
			{	

				int nItem = listctrl->GetItemCount();
				int nItemMod = modeShape;  //refering to shape images
				CString insstr = itemWnd->m_shapeStr;
				int ins = listctrl->InsertItem( LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE, nItem, LPCTSTR(insstr), 0, 0, nItemMod, itemWnd->uniqueID );				
			
			
			}

		}


}



void CScreenAnnotations::RefreshLayoutList()
{
		//tabmode must be layoutMode before calling this
		CListCtrl *listctrl = ((CListCtrl *) GetDlgItem (IDC_LIST1));	
		listctrl->DeleteAllItems();
		
		int max = gList.layoutArray.GetSize();
		CLayoutList * itemLayout = NULL;
		for (int i=0;i<max; i++)
		{
			itemLayout = gList.layoutArray[i];
			if (itemLayout)
			{	

				int nItem = listctrl->GetItemCount();
				int nItemMod = modeLayout;  //refering to layout images
				CString insstr = itemLayout->layoutName;
				int ins = listctrl->InsertItem( LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE, nItem, LPCTSTR(insstr), 0, 0, nItemMod, itemLayout->uniqueID );				
						
			}

		}

}


void CScreenAnnotations::TabSelectShapeMode(int updateTab)
{

	if (tabMode == modeLayout) {


		RefreshShapeList();

		tabMode = modeShape;

		if (updateTab)
			((CTabCtrl *) GetDlgItem (IDC_TAB1))->SetCurSel(tabMode);		

		
	}


}


void CScreenAnnotations::TabSelectLayoutMode(int updateTab)
{


	if (tabMode == modeShape) {


		RefreshLayoutList();

		tabMode = modeLayout;
		
		if (updateTab)
			((CTabCtrl *) GetDlgItem (IDC_TAB1))->SetCurSel(tabMode);		
		
	}


	
}

void CScreenAnnotations::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult) 
{

	CTabCtrl * tabctrl  = ((CTabCtrl *) GetDlgItem (IDC_TAB1));
	int sel = tabctrl->GetCurSel( );
	UpdateTabCtrl(sel);
		
	*pResult = 0;
}


void CScreenAnnotations::UpdateTabCtrl(int sel)
{

	if (sel == modeShape)
	{
		TabSelectShapeMode(0);
	}
	else
		TabSelectLayoutMode(0);

}

void CScreenAnnotations::OnEditlayoutSavelayout() 
{

	SaveLayoutNew();
	
}

void CScreenAnnotations::OnEditlayoutOpenlayout() 
{

	InstantiateLayout(); 
	
}

void CScreenAnnotations::OnEditlayoutCloseallobjects() 
{

	CloseAllWindows(1);
	
}

void CScreenAnnotations::OnEditlayoutLayoutshortcuts() 
{

	if (!keySCOpened)
	{

		keySCOpened = 1;
		Keyshortcuts kscDlg;
		kscDlg.DoModal();
		keySCOpened = 0;
		
		SetAdjustHotKeys();

	}
	

}


int AreWindowsEdited()
{

		int isEdited = 0;
		int max = gList.displayArray.GetSize();
		CTransparentWnd * itemWnd = NULL;		
		for (int i=max-1;i>=0; i--)
		{
			itemWnd = gList.displayArray[i];		
			if (itemWnd)
			{
				if ((itemWnd->trackingOn) || (itemWnd->editTransOn) || (itemWnd->editImageOn))
				{	
					isEdited = 1;
				}				
			}
		}


		max = gList.shapeArray.GetSize();
		itemWnd = NULL;		
		for (i=max-1;i>=0; i--)
		{
			itemWnd = gList.shapeArray[i];		
			if (itemWnd)
			{
				if ((itemWnd->trackingOn) || (itemWnd->editTransOn) || (itemWnd->editImageOn))
				{	
					isEdited = 1;
				}				
			}
		}



		return isEdited;

}

CLayoutList* CScreenAnnotations::LocateLayoutFromItem(int nItem)
{

	CLayoutList* returnLayout = NULL;
	
	long LayoutID = -1;

	LV_ITEM  searchItem;
	searchItem.mask = LVIF_PARAM ;
	searchItem.iItem = nItem;
	((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItem( &searchItem); 
	LayoutID = searchItem.lParam;
	
	
	int found = 0;
	CLayoutList* itemLayout = NULL;
	int max = gList.layoutArray.GetSize();
	for (int i=max-1;i>=0; i--)
	{

		itemLayout = gList.layoutArray[i];
		if (itemLayout)
		{			
			
			if (itemLayout->uniqueID == LayoutID)
			{
				returnLayout = itemLayout;
				found = 1;
				break;  //allow only one removal

			}
			
		}

	}
	

	return returnLayout;
	
}

void CScreenAnnotations::InstantiateLayout() 
{

	if (tabMode == modeLayout)
	{
		//Locate layout
		CLayoutList *itemLayout = NULL;
		
		int nItem = -1;
		int numitems = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemCount();
		int itemstate = 0;


		for (int i=0; i < numitems; i++) {	
			
 			itemstate = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemState( i, LVIS_SELECTED); 
			if (itemstate & LVIS_SELECTED)  {			
				nItem = i;
			}
		}

		
		if ((nItem>-1) && (nItem<numitems)) {		
			itemLayout = LocateLayoutFromItem(nItem); 
		}


		if (itemLayout)
		{

			if (itemLayout->layoutArrayPtr)
			{
			
				int max = itemLayout->layoutArrayPtr->GetSize();			
				
				CTransparentWnd * itemWnd = NULL;		
				//for (int i=max-1;i>=0; i--)
				for (int i=0;i<max; i++)
				{
					itemWnd = (*(itemLayout->layoutArrayPtr))[i];		
					if (itemWnd)
					{
						CTransparentWnd *cloneWnd = itemWnd->Clone(0,0);			
						gList.AddDisplayArray(cloneWnd);

						
						cloneWnd->UpdateWindow();
						cloneWnd->InvalidateRegion();
						cloneWnd->InvalidateTransparency();						
						cloneWnd->ShowWindow(SW_SHOW);


					}
				}

			}

		}



	}


}


void AdjustShapeName(CString& shapeName)
{

			CString reconstructNum("");
			int numchar = shapeName.GetLength();
			for (int i=numchar-1; i>=0; i--)
			{
				int txchar = shapeName[i];
				if ((txchar >= '0') && (txchar <= '9'))
					reconstructNum = reconstructNum +  shapeName[i];
				else 
					break;
			}

			reconstructNum.MakeReverse();
			int xchar = reconstructNum.GetLength();
			if (xchar <= 0)
				shapeNameInt = 1;
			else {

				for (int j=0; j<xchar; j++)
				{
					if (reconstructNum[j]=='0')
						xchar--;
					else 
						break;

				}

				if ((xchar>=0) && (xchar<=numchar))
				{
					int val;
					sscanf(LPCTSTR(reconstructNum),"%d",&val);
					if ((val>=0) && (val<100000)) {
						shapeNameInt = val + 1;
						shapeName = shapeName.Left(numchar- xchar);
					}
					else
						shapeNameInt = 1;

				}
				else
					shapeNameInt = 1;
			}
}


void AdjustLayoutName(CString& layoutName)
{

			CString reconstructNum("");
			int numchar = layoutName.GetLength();
			for (int i=numchar-1; i>=0; i--)
			{
				int txchar = layoutName[i];
				if ((txchar >= '0') && (txchar <= '9'))
					reconstructNum = reconstructNum +  layoutName[i];
				else 
					break;
			}

			reconstructNum.MakeReverse();
			int xchar = reconstructNum.GetLength();
			if (xchar <= 0)
				layoutNameInt = 1;
			else {

				for (int j=0; j<xchar; j++)
				{
					if (reconstructNum[j]=='0')
						xchar--;
					else 
						break;

				}

				if ((xchar>=0) && (xchar<=numchar))
				{
					int val;
					sscanf(LPCTSTR(reconstructNum),"%d",&val);
					if ((val>=0) && (val<100000)) {
						layoutNameInt = val + 1;
						layoutName = layoutName.Left(numchar- xchar);
					}
					else
						layoutNameInt = 1;

				}
				else
					layoutNameInt = 1;
			}
}



void CScreenAnnotations::OnEditobjLibraryCloselibrary() 
{

	CString m_newfile;

	static char BASED_CODE szFilter[] =	"Shape Files (*.shapes)|*.shapes||";
	static char szTitle[]="Save Shapes";		
	
	
	CFileDialog fdlg(FALSE,"*.shapes","*.shapes",OFN_LONGNAMES | OFN_EXTENSIONDIFFERENT ,szFilter,this);	
	fdlg.m_ofn.lpstrTitle=szTitle;	
	fdlg.m_ofn.lpstrDefExt = "shapes";
	
	if(fdlg.DoModal() == IDOK)
	{
		m_newfile = fdlg.GetPathName();

		CString compareStr = GetProgPath() + "\\CamShapes.ini";
		CString teststr = m_newfile;
		compareStr.MakeLower();
		teststr.MakeLower();
		if (compareStr == teststr)
		{

			MessageOut(NULL,IDS_STRINGCANNOTSAVEO, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
			
			return;

		}

		
		gList.SaveShapeArray(m_newfile);
				
	}


	
}



void CScreenAnnotations::OnEditobjLibraryOpenlibrary() 
{
	
	CString filename;
	
	static char BASED_CODE szFilter[] =	"Shape Files (*.shapes)|*.shapes||";
	static char szTitle[]="Load Shapes";		
	
	CFileDialog fdlg(TRUE,"*.shapes","*.shapes",OFN_LONGNAMES | OFN_FILEMUSTEXIST ,szFilter,this);	
	fdlg.m_ofn.lpstrTitle=szTitle;		

	if(fdlg.DoModal() == IDOK)
	{
		filename = fdlg.GetPathName();
		gList.LoadShapeArray(filename);
		RefreshShapeList();			
		
	}
	
}

void CScreenAnnotations::OnEditobjLibraryNewlibrary() 
{	
	
	int ret = MessageOut(NULL,IDS_STRINGSAVECL, IDS_STRING_NOTE, MB_YESNOCANCEL | MB_ICONQUESTION);
	if (ret == IDYES)
	{
		OnEditobjLibraryCloselibrary();
	}
	else if (ret == IDCANCEL)
		return;
	
	gList.FreeShapeArray();
	RefreshShapeList();	
	
}

void CScreenAnnotations::OnEditlayoutLibraryNewlibrary() 
{

	int ret = MessageOut(NULL,IDS_STRINGSAVECLLA, IDS_STRING_NOTE, MB_YESNOCANCEL | MB_ICONQUESTION);
	if (ret == IDYES)
	{
		OnEditlayoutLibraryCloselibrary();
	}
	else if (ret == IDCANCEL)
		return;
	
	gList.FreeLayoutArray();
	RefreshLayoutList();		
	
}

void CScreenAnnotations::OnEditlayoutLibraryOpenlibrary() 
{

	CString filename;
	
	static char BASED_CODE szFilter[] =	"Layout Files (*.layout)|*.layout||";
	static char szTitle[]="Load Layout";		
	
	CFileDialog fdlg(TRUE,"*.layout;*.lay","*.layout;*.lay",OFN_LONGNAMES | OFN_FILEMUSTEXIST ,szFilter,this);	
	fdlg.m_ofn.lpstrTitle=szTitle;		

	if(fdlg.DoModal() == IDOK)
	{
		filename = fdlg.GetPathName();
		gList.LoadLayout(filename);
		RefreshLayoutList();			
		
	}
	
}

void CScreenAnnotations::OnEditlayoutLibraryCloselibrary() 
{

	CString m_newfile;

	static char BASED_CODE szFilter[] =	"Layout Files (*.layout)|*.layout||";
	static char szTitle[]="Save Layout";		
	
	
	CFileDialog fdlg(FALSE,"*.layout","*.layout",OFN_LONGNAMES | OFN_EXTENSIONDIFFERENT ,szFilter,this);	
	fdlg.m_ofn.lpstrTitle=szTitle;	
	fdlg.m_ofn.lpstrDefExt = "layout";
	
	if(fdlg.DoModal() == IDOK)
	{
		m_newfile = fdlg.GetPathName();

		CString compareStr = GetProgPath() + "\\CamLayout.ini";
		CString teststr = m_newfile;
		compareStr.MakeLower();
		teststr.MakeLower();
		if (compareStr == teststr)
		{		
			//MessageBox("You cannot save over the default layout file.","Note",MB_OK | MB_ICONEXCLAMATION);			
			MessageOut(NULL,IDS_STRINGcANNOTSAVEOL, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		gList.SaveLayout(m_newfile);		
				
	}
	
}

void CScreenAnnotations::OnHelpHelp() 
{

	CString progdir,helpScreenPath;
	progdir=GetProgPath();
	helpScreenPath= progdir + "\\help.htm#ScreenAnn";
	
	Openlink(helpScreenPath);

	
}

void CScreenAnnotations::OnOptionsClosescreenannotations() 
{

	OnOK(); 
	
}


BOOL CScreenAnnotations::Openlink (CString link)
{

  BOOL bSuccess = FALSE;

  //As a last resort try ShellExecuting the URL, may
  //even work on Navigator!
  if (!bSuccess)
    bSuccess = OpenUsingShellExecute (link);

  if (!bSuccess)
    bSuccess = OpenUsingRegisteredClass (link);
  return bSuccess;
}


BOOL CScreenAnnotations::OpenUsingShellExecute (CString link)
{
  LPCTSTR mode;
  mode = _T ("open");

  //HINSTANCE hRun = ShellExecute (GetParent ()->GetSafeHwnd (), mode, m_sActualLink, NULL, NULL, SW_SHOW);
  HINSTANCE hRun = ShellExecute (GetSafeHwnd (), mode, link, NULL, NULL, SW_SHOW);
  if ((int) hRun <= HINSTANCE_ERROR)
    {
      TRACE ("Failed to invoke URL using ShellExecute\n");
      return FALSE;
    }
  return TRUE;
}


BOOL CScreenAnnotations::
OpenUsingRegisteredClass (CString link)
{
  TCHAR key[MAX_PATH + MAX_PATH];
  HINSTANCE result;

  if (GetRegKey (HKEY_CLASSES_ROOT, _T (".htm"), key) == ERROR_SUCCESS)
    {
      LPCTSTR mode;
      mode = _T ("\\shell\\open\\command");
      _tcscat (key, mode);
      if (GetRegKey (HKEY_CLASSES_ROOT, key, key) == ERROR_SUCCESS)
        {
          LPTSTR pos;
          pos = _tcsstr (key, _T ("\"%1\""));
          if (pos == NULL)
            {              // No quotes found

              pos = strstr (key, _T ("%1"));   // Check for %1, without quotes

              if (pos == NULL)     // No parameter at all...

                pos = key + _tcslen (key) - 1;
              else
                *pos = _T ('\0');  // Remove the parameter

            }
          else
            *pos = _T ('\0');  // Remove the parameter

          _tcscat (pos, _T (" "));
          _tcscat (pos, link);
          result = (HINSTANCE) WinExec (key, SW_SHOW);
          if ((int) result <= HINSTANCE_ERROR)
            {
              CString str;
              switch ((int) result)
                {
                case 0:
                  str = _T ("The operating system is out\nof memory or resources.");
                  break;
                case SE_ERR_PNF:
                  str = _T ("The specified path was not found.");
                  break;
                case SE_ERR_FNF:
                  str = _T ("The specified file was not found.");
                  break;
                case ERROR_BAD_FORMAT:
                  str = _T ("The .EXE file is invalid\n(non-Win32 .EXE or error in .EXE image).");
                  break;
                case SE_ERR_ACCESSDENIED:
                  str = _T ("The operating system denied\naccess to the specified file.");
                  break;
                case SE_ERR_ASSOCINCOMPLETE:
                  str = _T ("The filename association is\nincomplete or invalid.");
                  break;
                case SE_ERR_DDEBUSY:
                  str = _T ("The DDE transaction could not\nbe completed because other DDE transactions\nwere being processed.");
                  break;
                case SE_ERR_DDEFAIL:
                  str = _T ("The DDE transaction failed.");
                  break;
                case SE_ERR_DDETIMEOUT:
                  str = _T ("The DDE transaction could not\nbe completed because the request timed out.");
                  break;
                case SE_ERR_DLLNOTFOUND:
                  str = _T ("The specified dynamic-link library was not found.");
                  break;
                case SE_ERR_NOASSOC:
                  str = _T ("There is no application associated\nwith the given filename extension.");
                  break;
                case SE_ERR_OOM:
                  str = _T ("There was not enough memory to complete the operation.");
                  break;
                case SE_ERR_SHARE:
                  str = _T ("A sharing violation occurred.");
                  break;
                default:
                  str.Format (_T ("Unknown Error (%d) occurred."), (int) result);
                }
              str = _T ("Unable to open hyperlink:\n\n") + str;
              AfxMessageBox (str, MB_ICONEXCLAMATION | MB_OK);
            }
          else
            return TRUE;
        }
    }
  return FALSE;
}

  

LONG CScreenAnnotations::GetRegKey (HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
  HKEY hkey;
  LONG retval = RegOpenKeyEx (key, subkey, 0, KEY_QUERY_VALUE, &hkey);

  if (retval == ERROR_SUCCESS)
    {
      long datasize = MAX_PATH;
      TCHAR data[MAX_PATH];
      RegQueryValue (hkey, NULL, data, &datasize);
      _tcscpy (retdata, data);
      RegCloseKey (hkey);
    }

  return retval;
}



//partial
void CScreenAnnotations::InstantiateLayout(int nItem, int makeselect) 
{
	TabSelectLayoutMode(1);

	

	if (tabMode == modeLayout)
	{
		//Locate layout
		CLayoutList *itemLayout = NULL;
		

		int numitems = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemCount();		
		

		if ((nItem>-1) && (nItem<numitems)) {		
			
			itemLayout = LocateLayoutFromItem(nItem); 		 						
			
			((CListCtrl *) GetDlgItem (IDC_LIST1))->SetItemState( nItem, LVIS_SELECTED, LVIS_SELECTED); 
			((CListCtrl *) GetDlgItem (IDC_LIST1))->EnsureVisible( nItem, FALSE );	
			
			
		}



		if (itemLayout)
		{

			//Close all others and instantiate 
			CloseAllWindows(1);
			
			
			if (itemLayout->layoutArrayPtr)
			{
			
				int max = itemLayout->layoutArrayPtr->GetSize();			
				
				CTransparentWnd * itemWnd = NULL;		
				for (int i=0;i<max; i++)
				{
					itemWnd = (*(itemLayout->layoutArrayPtr))[i];		
					
					if (itemWnd)
					{
						CTransparentWnd *cloneWnd = itemWnd->Clone(0,0);			
						gList.AddDisplayArray(cloneWnd);

						
						cloneWnd->UpdateWindow();
						cloneWnd->InvalidateRegion();
						cloneWnd->InvalidateTransparency();						
						cloneWnd->ShowWindow(SW_SHOW);


					}
				}

			}			


			

		}

	}


}


int CScreenAnnotations::GetLayoutListSelection()
{

	TabSelectLayoutMode(1);

	int numsel = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetSelectedCount();
	if (numsel<=0)
		return - 1;
						
	
	int numitems = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemCount();	
	if (numitems<=0)
		return - 1;	


	int itemstate = 0;
	if ((currentLayout>=0) && (currentLayout<numitems))
	{
		itemstate = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemState( currentLayout, LVIS_SELECTED); 
		if (itemstate)
			return currentLayout;

	}
	

	//Search thru' all
	itemstate = 0;
	int nItem = -1;
	for (int i=0; i < numitems; i++) {	
			
 		itemstate = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemState( i, LVIS_SELECTED); 
		if (itemstate & LVIS_SELECTED)  {			
				return i;
		}
	}

	return nItem;
	

}

void CScreenAnnotations::OnNext() 
{

	int max = gList.layoutArray.GetSize();
			if (max<=0) return;
	
	//Get Current selected
	int cursel  = GetLayoutListSelection();
	if (cursel == -1)
		currentLayout = 0;
	else
		currentLayout = cursel + 1;

	if (currentLayout>=max)
				currentLayout=0;	

	InstantiateLayout(currentLayout,1);
	
}

void CScreenAnnotations::OnHelpShapes() 
{

	CString progdir,helpScreenPath;
	progdir=GetProgPath();
	helpScreenPath= progdir + "\\help.htm#Shape";
	
	Openlink(helpScreenPath);
	
}

void CScreenAnnotations::OnHelpShapetopicsInstantiatingashape() 
{

	CString progdir,helpScreenPath;
	progdir=GetProgPath();
	helpScreenPath= progdir + "\\help.htm#InstantiatingShapes";
	
	Openlink(helpScreenPath);
	
}

void CScreenAnnotations::OnHelpShapetopicsEditingashape() 
{

	CString progdir,helpScreenPath;
	progdir=GetProgPath();
	helpScreenPath= progdir + "\\help.htm#EditingShapes";
	
	Openlink(helpScreenPath);
	
}

void CScreenAnnotations::OnHelpShapetopicsEditingtext() 
{

	CString progdir,helpScreenPath;
	progdir=GetProgPath();
	helpScreenPath= progdir + "\\help.htm#EditText";
	
	Openlink(helpScreenPath);
	
}

void CScreenAnnotations::OnHelpShapetopicsEditingimage() 
{

	CString progdir,helpScreenPath;
	progdir=GetProgPath();
	helpScreenPath= progdir + "\\help.htm#EditImage";
	
	Openlink(helpScreenPath);

	
}

void CScreenAnnotations::OnHelpShapetopicsCreatinganewshape() 
{

	CString progdir,helpScreenPath;
	progdir=GetProgPath();
	helpScreenPath= progdir + "\\help.htm#NewShape";
	
	Openlink(helpScreenPath);
	
}

void CScreenAnnotations::OnHelpShapetopicsEditingtransparency() 
{

	CString progdir,helpScreenPath;
	progdir=GetProgPath();
	helpScreenPath= progdir + "\\help.htm#EditTrans";
	
	Openlink(helpScreenPath);
	
}

void CScreenAnnotations::OnHelpShapetopicsManagingshapes() 
{

	CString progdir,helpScreenPath;
	progdir=GetProgPath();
	helpScreenPath= progdir + "\\help.htm#ManagingShapes";
	
	Openlink(helpScreenPath);
	
}

void CScreenAnnotations::OnHelpShapetopicsResizingshapes() 
{

	CString progdir,helpScreenPath;
	progdir=GetProgPath();
	helpScreenPath= progdir + "\\help.htm#Resize";
	
	Openlink(helpScreenPath);
	
}


void CScreenAnnotations::OnHelpLayouts() 
{

	CString progdir,helpScreenPath;
	progdir=GetProgPath();
	helpScreenPath= progdir + "\\help.htm#Layout";
	
	Openlink(helpScreenPath);
	
}


void CScreenAnnotations::OnAnnSavelayout() 
{

	SaveLayoutNew();
	
}



void CScreenAnnotations::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	
	int max = gList.displayArray.GetSize();
	
	if (max<=0) {
		pPopupMenu->EnableMenuItem(IDR_ANN_SAVELAYOUT,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
		pPopupMenu->EnableMenuItem(ID_OBJECTS_CLOSEALLOBJECTS, MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);

	}
	else
	{
		pPopupMenu->EnableMenuItem(IDR_ANN_SAVELAYOUT,MF_ENABLED|MF_BYCOMMAND);	
		pPopupMenu->EnableMenuItem(ID_OBJECTS_CLOSEALLOBJECTS,MF_ENABLED|MF_BYCOMMAND);	

	}

	int numitems = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemCount();
	int numsel = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetSelectedCount();
	if ((numitems<=0) || (numsel<=0))
	{
		pPopupMenu->EnableMenuItem(ID_OBJECTS_COPY,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
		pPopupMenu->EnableMenuItem(ID_OBJECTS_REMOVE,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);

		pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITTEXT,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
		pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITIMAGE,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
		pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITNAME,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
		pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITTRANSAPRENCY,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);

		pPopupMenu->EnableMenuItem(ID_OBJECTS_MOVEITEMUP,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
		pPopupMenu->EnableMenuItem(ID_OBJECTS_MOVEITEMDOWN,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
		
	}
	else
	{
		pPopupMenu->EnableMenuItem(ID_OBJECTS_COPY,MF_ENABLED |MF_BYCOMMAND);
		pPopupMenu->EnableMenuItem(ID_OBJECTS_REMOVE,MF_ENABLED |MF_BYCOMMAND);


		if (tabMode == modeShape)
		{

			pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITTEXT,MF_ENABLED|MF_BYCOMMAND);
			pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITIMAGE,MF_ENABLED|MF_BYCOMMAND);
			pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITNAME,MF_ENABLED|MF_BYCOMMAND);
			pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITTRANSAPRENCY,MF_ENABLED|MF_BYCOMMAND);

		}
		else
		{
			pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITTEXT,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
			pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITIMAGE,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);			
			pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITTRANSAPRENCY,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
			pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITNAME,MF_ENABLED|MF_BYCOMMAND);

		}


		//Enabling / Disabling Item Up / Down
		int nItem = -1;		
		int itemstate = 0;
		for (int i=0; i < numitems; i++) {		

			itemstate = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemState( i, LVIS_SELECTED); 
			if (itemstate & LVIS_SELECTED)  {

				nItem = i;
			}
		}

		if ((nItem>-1) && (nItem<numitems)) {

				if (nItem>0)
					pPopupMenu->EnableMenuItem(ID_OBJECTS_MOVEITEMUP,MF_ENABLED |MF_BYCOMMAND);				
				else
					pPopupMenu->EnableMenuItem(ID_OBJECTS_MOVEITEMUP,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);
					
							
				if (nItem <	numitems-1)
					pPopupMenu->EnableMenuItem(ID_OBJECTS_MOVEITEMDOWN,MF_ENABLED | MF_BYCOMMAND);
				else
					pPopupMenu->EnableMenuItem(ID_OBJECTS_MOVEITEMDOWN,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND);			
				
		}
		


	}


	if (tabMode == modeLayout)
	{
		if (layoutIsCopied)
		{
			pPopupMenu->EnableMenuItem(ID_OBJECTS_PASTE,MF_ENABLED|MF_BYCOMMAND);

		}
		else
		{
			pPopupMenu->EnableMenuItem(ID_OBJECTS_PASTE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);

		}
	}
	else  if (tabMode == modeShape)
	{

		if (objectIsCopied)
		{
			pPopupMenu->EnableMenuItem(ID_OBJECTS_PASTE,MF_ENABLED|MF_BYCOMMAND);

		}
		else
		{
			pPopupMenu->EnableMenuItem(ID_OBJECTS_PASTE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);

		}

	}
	else
		pPopupMenu->EnableMenuItem(ID_OBJECTS_PASTE,MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);



		
}

void CScreenAnnotations::OnLibraryOpenshapelibrary() 
{

	
	OnEditobjLibraryOpenlibrary(); 
	TabSelectShapeMode(1);
	
}

void CScreenAnnotations::OnLibrarySaveshapelibrary() 
{

	
	OnEditobjLibraryCloselibrary(); 

	
}

void CScreenAnnotations::OnLibraryNewshapelibrary() 
{

	
	OnEditobjLibraryNewlibrary();
	TabSelectShapeMode(1);
	
	
}

void CScreenAnnotations::OnLibraryNewlayoutlibrary() 
{
	
	OnEditlayoutLibraryNewlibrary();	
	TabSelectLayoutMode(1);
	
	
}

void CScreenAnnotations::OnLibraryOpenlayoutlibrary() 
{

	OnEditlayoutLibraryOpenlibrary();
	TabSelectLayoutMode(1);
	
}

void CScreenAnnotations::OnLibrarySavelayoutlibrary() 
{

	OnEditlayoutLibraryCloselibrary();
	
}

void CScreenAnnotations::OnObjectsCopy() 
{
	
	OnEditobjCopy(); 
	
}

void CScreenAnnotations::OnObjectsPaste() 
{
	
	OnEditobjPaste(); 
	
}

void CScreenAnnotations::OnObjectsRemove() 
{
	
	OnEditobjRemove();
	
}



int CScreenAnnotations::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	
	
	return 0;
}

void CScreenAnnotations::OnObjectsEdittext() 
{
	
	OnEditobjEdittext();
	
}

void CScreenAnnotations::OnObjectsEditimage() 
{
	
	OnEditobjEditimage();
	
}

void CScreenAnnotations::OnObjectsEditname() 
{
	
	OnEditobjTestedit();
	
}

void CScreenAnnotations::OnObjectsEdittransaprency() 
{
	
	OnEditobjEdittransparency();
	
}

void CScreenAnnotations::OnObjectsCloseallobjects() 
{
	
	OnEditlayoutCloseallobjects();
	
}

void CScreenAnnotations::OnLibraryInsertshapelibaray() 
{
	
	CString filename;
	
	static char BASED_CODE szFilter[] =	"Shape Files (*.shapes)|*.shapes||";
	static char szTitle[]="Insert Shapes From File";		
	
	CFileDialog fdlg(TRUE,"*.shapes","*.shapes",OFN_LONGNAMES | OFN_FILEMUSTEXIST ,szFilter,this);	
	fdlg.m_ofn.lpstrTitle=szTitle;		

	if(fdlg.DoModal() == IDOK)
	{
		filename = fdlg.GetPathName();		
		gList.LoadShapeArray(filename,0);
		
		RefreshShapeList();			
		
	}

	TabSelectShapeMode(1);
	
}

void CScreenAnnotations::OnObjectsMoveitemup() 
{
	
	MoveItem(0);
	
}

void CScreenAnnotations::OnObjectsMoveitemdown() 
{
	
	MoveItem(1);
	
}

void CScreenAnnotations::MoveItem(int direction)
{

	//Locate Selected Item
	int nItem = -1;
	int numitems = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemCount();
	int itemstate = 0;

	for (int i=0; i < numitems; i++) {
	

		itemstate = ((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItemState( i, LVIS_SELECTED); 
		if (itemstate & LVIS_SELECTED)  {
			nItem = i;
		}
	}


	int newitem = 0;
	if ((nItem>-1) && (nItem<numitems)) {

		if (direction == 0) //up
		{
			if (nItem>0)
			{
				 newitem = nItem - 1;
			}
			else return;
			
		}
		else if (direction == 1) //down
		{

			if (nItem <	numitems-1)
			{
				 newitem = nItem + 1;
			}
			else return;

		}


		LV_ITEM  swapItem, swapItem2;
		swapItem.mask = LVIF_PARAM;
		swapItem.iItem = nItem;
		((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItem( &swapItem); 	

		swapItem2.mask = LVIF_PARAM;
		swapItem2.iItem = newitem;
		((CListCtrl *) GetDlgItem (IDC_LIST1))->GetItem( &swapItem2); 

		if (tabMode == modeShape) 
		{	

			gList.SwapShapeArray(swapItem.lParam, swapItem2.lParam);
			RefreshShapeList();						

		}
		else if (tabMode == modeLayout) 
		{
			gList.SwapLayoutArray(swapItem.lParam, swapItem2.lParam);
			RefreshLayoutList();					

		}

		if ((newitem>-1) && (newitem<numitems)) {

			((CListCtrl *) GetDlgItem (IDC_LIST1))->EnsureVisible( newitem, FALSE );
			((CListCtrl *) GetDlgItem (IDC_LIST1))->SetItemState( newitem, LVIS_SELECTED, LVIS_SELECTED); 

		}

	}

}

