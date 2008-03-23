//-----------------------------------------------------------------------------
// Picture (Implementations) Version 1.00
//
// Routins 4 Showing Picture Files... (.BMP .DIB .EMF .GIF .ICO .JPG .WMF)
//
// Author: Dr. Yovav Gad, EMail: Sources@SuperMain.com ,Web: www.SuperMain.com
//=============================================================================
// 
// Full Story:
// ~~~~~~~~~~~
// There R Many Libraries To Handle Image Files, Anyway Most Of Them Do Not
// Include Source Files Or Just Very Complicated To Implement / Understand,
// 
// After Many Days Of Searching (And Not Finding) a Way To Load a JPG From a
// Resource And Show It On a *Dialog Based* Application, I Decided 2 Take Steps
//
// So I Created What I Call a Very *Simple* & Useful Class,
// It Can Easily Implemented By Adding It To a Project, And U Do Not Have To
// Be a Real JPEG Freak - And Invent All Header Reading From The Beginning
// (It Uses The IPicture Interface - Same Way As Internet Explorer Does)
//
// I Would Like To Thank Mr.Peter Hendrix For His Wonderful Work
// That I Found On: http://www.thecodeproject.com/bitmap/cpicture.asp
// Which I Was Inspired And Got The IPicture Interface Idea From...
//
// Guess U Will Find It Useful,
// Appreciate If U Can Mention My Name On Your Final Code,
// Please Feel Free To Send Me Any Improvements Or SaveAsJPG() Functions:
//
// (This Program Has No Bugs - Only Undocumented Solutions)
//
// Author: Dr. Yovav Gad, EMail: Sources@SuperMain.com ,Web: www.SuperMain.com
//
//--------------------------Example & Usage 4 Dummies--------------------------
//
//  U Need 2 Add "CPicture.CPP" and "CPicture.H" Into Your Project (From FileView)
//  So U Will Get Control Over The Functions In This Class,
//  Then U Can Create a Picture Object And Show It On a Device Context
//
//  CPicture m_Picture;  // Create a Picture Object (An Instance Of This Class)
//  #include "Picture.h" // Make Sure U Include This Where U Gonna Create The Object...
//
//  Load Picture Data Into The IPicture Interface (.BMP .DIB .EMF .GIF .ICO .JPG .WMF)
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	m_Picture.Load("Test.JPG"); // Load From a File - Just Load It (Show Later)
//	m_Picture.Load(IDR_TEST, "JPG"); // Load From a Resource - Just Load It (Show Later)
//  (U Must Include IDR_TEST In Your Resources Under a Custom Name, 4 Example - "JPG")
//  
//  When Using DC Object On a *Dialog Based* Application (CPaintDC dc(this);)
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	m_Picture.UpdateSizeOnDC(&dc); // Get Picture Dimentions In Pixels
//	m_Picture.Show(&dc, CPoint(0,0), CPoint(m_Picture.m_Width, m_Picture.m_Height), 0,0);
//	m_Picture.Show(&dc, CRect(0,0,100,100)); // Change Original Dimentions
//	m_Picture.ShowBitmapResource(&dc, IDB_TEST, CPoint(0,0)); // Show Bitmap Resource
//
//  OR When Using a Pointer On a "Regular" MFC Application (CDC* pDC)
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	m_Picture.UpdateSizeOnDC(pDC); // Get Picture Dimentions In Pixels
//	m_Picture.Show(pDC, CPoint(0,0), CPoint(m_Picture.m_Width, m_Picture.m_Height), 0,0);
//	m_Picture.Show(pDC, CRect(0,0,100,100)); // Change Original Dimentions
//	m_Picture.ShowBitmapResource(pDC, IDB_TEST, CPoint(0,0)); // Show Bitmap Resource
//
//  Show Picture Information
//  ~~~~~~~~~~~~~~~~~~~~~~~~
//	CString S;
//	S.Format("Size = %4d\nWidth = %4d\nHeight = %4d\nWeight = %4d\n",
//	m_Picture.m_Weight, m_Picture.m_Width, m_Picture.m_Height, m_Picture.m_Weight);
//	AfxMessageBox(S);
//
//----------------------------Cut The Bullshit Here----------------------------

#include "stdafx.h"
#include "Picture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define HIMETRIC_INCH 2540
#define ERROR_TITLE "CPicture Error" // Error Title (Related To This Class)...

extern void ErrMsg(char format[], ...);

//-----------------------------------------------------------------------------
// Does:   Constructor - Create a New CPicture Object To Hold Picture Data
// ~~~~
//
//-----------------------------------------------------------------------------
CPicture::CPicture()
//=============================================================================
{
	m_IPicture = NULL;
	m_Height = 0;
	m_Weight = 0;
	m_Width = 0;
}

//-----------------------------------------------------------------------------
// Does:   Destructor - Free Data And Information From The CPicture Object
// ~~~~
//
//-----------------------------------------------------------------------------
CPicture::~CPicture()
//=============================================================================
{
	if(m_IPicture != NULL) FreePictureData(); // Important - Avoid Leaks...
}


//-----------------------------------------------------------------------------
// Does:   Free The Allocated Memory That Holdes The IPicture Interface Data
// ~~~~    And Clear Picture Information
//
// Note:   This Might Also Be Useful If U Only Need To Show The Picture Once
// ~~~~~   Or If U Copy The Picture To The Device Context, So It Can Still
//         Remain On Screen - But IPicture Data Is Not Needed No More
//
//-----------------------------------------------------------------------------
void CPicture::FreePictureData()
//=============================================================================
{
	if(m_IPicture != NULL)
		{
		m_IPicture->Release();
		m_IPicture = NULL;
		m_Height = 0;
		m_Weight = 0;
		m_Width = 0;
		}
}

//-----------------------------------------------------------------------------
// Does:   Open a Resource And Load It Into IPicture (Interface)
// ~~~~    (.BMP .DIB .EMF .GIF .ICO .JPG .WMF)
//
// Note:   When Adding a Bitmap Resource It Would Automatically Show On "Bitmap"
// ~~~~    This NOT Good Coz We Need To Load It From a Custom Resource "BMP"
//         To Add a Custom Rresource: Import Resource -> Open As -> Custom
//         (Both .BMP And .DIB Should Be Found Under "BMP")
//
// InPut:  ResourceName - As a UINT Defined (Example: IDR_PICTURE_RESOURCE)
// ~~~~~   ResourceType - Type Name (Example: "JPG")
//
// OutPut: TRUE If Succeeded...
// ~~~~~~
//-----------------------------------------------------------------------------
BOOL CPicture::Load(UINT ResourceName, LPCSTR ResourceType)
//=============================================================================
{
	BOOL bResult = FALSE;

	HGLOBAL		hGlobal = NULL;
	HRSRC		hSource = NULL;
	LPVOID		lpVoid  = NULL;
	int			nSize   = 0;

	if(m_IPicture != NULL) FreePictureData(); // Important - Avoid Leaks...

	hSource = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(ResourceName), ResourceType);

	if(hSource == NULL)
		{
		HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
		MessageBoxEx(hWnd, "FindResource() Failed\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		return(FALSE);
		}

	hGlobal = LoadResource(AfxGetResourceHandle(), hSource);
	if(hGlobal == NULL)
		{
		HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
		MessageBoxEx(hWnd, "LoadResource() Failed\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		return(FALSE);
		}

	lpVoid = LockResource(hGlobal);
	if(lpVoid == NULL)
		{
		HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
		MessageBoxEx(hWnd, "LockResource() Failed\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		return(FALSE);
		}

	nSize = (UINT)SizeofResource(AfxGetResourceHandle(), hSource);
	if(LoadPictureData((BYTE*)hGlobal, nSize)) bResult = TRUE;

	UnlockResource(hGlobal); // 16Bit Windows Needs This
	FreeResource(hGlobal); // 16Bit Windows Needs This (32Bit - Automatic Release)

	m_Weight = nSize; // Update Picture Size Info...

	if(m_IPicture != NULL) // Do Not Try To Read From Memory That Is Not Exist...
		{ 
		m_IPicture->get_Height(&m_Height);
		m_IPicture->get_Width(&m_Width);
	    // Calculate Its Size On a "Standard" (96 DPI) Device Context
	    m_Height = MulDiv(m_Height, 96, HIMETRIC_INCH);
	    m_Width  = MulDiv(m_Width,  96, HIMETRIC_INCH);
		}
	else // Picture Data Is Not a Known Picture Type
		{
		m_Height = 0;
		m_Width = 0;
		bResult = FALSE;
		}

	return(bResult);
}

//-----------------------------------------------------------------------------
// Does:   Open a File And Load It Into IPicture (Interface)
// ~~~~    (.BMP .DIB .EMF .GIF .ICO .JPG .WMF)
//
// InPut:  sFilePathName - Path And FileName Target To Save
// ~~~~~   
//
// OutPut: TRUE If Succeeded...
// ~~~~~~
//-----------------------------------------------------------------------------
BOOL CPicture::Load(CString sFilePathName)
//=============================================================================
{
	BOOL bResult = FALSE;
	CFile PictureFile;
	CFileException e;
	int	nSize = 0;

	if(m_IPicture != NULL) FreePictureData(); // Important - Avoid Leaks...

	if(PictureFile.Open(sFilePathName, CFile::modeRead | CFile::typeBinary, &e))
		{
		nSize = PictureFile.GetLength();
		BYTE* pBuffer = new BYTE[nSize];
	
		if(PictureFile.Read(pBuffer, nSize) > 0)
			{
			if(LoadPictureData(pBuffer, nSize))	bResult = TRUE;
			}

		PictureFile.Close();
		delete [] pBuffer;
		}
	else // Open Failed...
		{
		TCHAR szCause[255];
		e.GetErrorMessage(szCause, 255, NULL);
		HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
		MessageBoxEx(hWnd, szCause, ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		bResult = FALSE;
		}

	m_Weight = nSize; // Update Picture Size Info...

	if(m_IPicture != NULL) // Do Not Try To Read From Memory That Is Not Exist...
		{ 
		m_IPicture->get_Height(&m_Height);
		m_IPicture->get_Width(&m_Width);
	    // Calculate Its Size On a "Standard" (96 DPI) Device Context
	    m_Height = MulDiv(m_Height, 96, HIMETRIC_INCH);
	    m_Width  = MulDiv(m_Width,  96, HIMETRIC_INCH);
		}
	else // Picture Data Is Not a Known Picture Type
		{
		m_Height = 0;
		m_Width = 0;
		bResult = FALSE;
		}

	return(bResult);
}

//-----------------------------------------------------------------------------
// Does:   Read The Picture Data From a Source (File / Resource)
// ~~~~    And Load It Into The Current IPicture Object In Use
//
// InPut:  Buffer Of Data Source (File / Resource) And Its Size
// ~~~~~   
//
// OutPut: Feed The IPicture Object With The Picture Data
// ~~~~~~  (Use Draw Functions To Show It On a Device Context)
//         TRUE If Succeeded...
//-----------------------------------------------------------------------------
BOOL CPicture::LoadPictureData(BYTE *pBuffer, int nSize)
//=============================================================================
{
	BOOL bResult = FALSE;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nSize);

	if(hGlobal == NULL)
		{
		HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
		MessageBoxEx(hWnd, "Can not allocate enough memory\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		return(FALSE);
		}

	void* pData = GlobalLock(hGlobal);
	memcpy(pData, pBuffer, nSize);
	GlobalUnlock(hGlobal);

	IStream* pStream = NULL;

	if(CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK)
		{
		HRESULT hr;
		if((hr = OleLoadPicture(pStream, nSize, FALSE, IID_IPicture, (LPVOID *)&m_IPicture)) == E_NOINTERFACE)
			{
			HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
			MessageBoxEx(hWnd, "IPicture interface is not supported\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
			return(FALSE);
			}
		else // S_OK
			{
			pStream->Release();
			pStream = NULL;
			bResult = TRUE;
			}
		}

	FreeResource(hGlobal); // 16Bit Windows Needs This (32Bit - Automatic Release)

	return(bResult);
}

//-----------------------------------------------------------------------------
// Does:   Draw The Loaded Picture Direct To The Client DC
// ~~~~
//
// Note:   Bigger OR Smaller Dimentions Than The Original Picture Size
// ~~~~    Will Draw The Picture Streached To Its New Given NEW Dimentions...
//
// InPut:  pDC - Given DC To Draw On
// ~~~~~   DrawRect - Dimentions Of The Picture To Draw (As a Rectangle)
//
// OutPut: TRUE If Succeeded...
// ~~~~~~
//-----------------------------------------------------------------------------
BOOL CPicture::Show(CDC *pDC, CRect DrawRect)
//=============================================================================
{
    if (pDC == NULL || m_IPicture == NULL) return FALSE;
    
    long Width  = 0;
    long Height = 0;
    m_IPicture->get_Width(&Width);
    m_IPicture->get_Height(&Height);

    HRESULT hrP = NULL;
    
    hrP = m_IPicture->Render(pDC->m_hDC,
                      DrawRect.left,                  // Left
                      DrawRect.top,                   // Top
                      DrawRect.right - DrawRect.left, // Right
                      DrawRect.bottom - DrawRect.top, // Bottom
                      0,
                      Height,
                      Width,
                      -Height,
                      &DrawRect);

    if (SUCCEEDED(hrP)) return(TRUE);

	HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
	MessageBoxEx(hWnd, "Can not allocate enough memory\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
    return(FALSE);
}

//-----------------------------------------------------------------------------
// Does:   Draw The Loaded Picture Direct To The Client DC
// ~~~~
//
// Note:   Bigger OR Smaller Dimentions Than The Original Picture Size
// ~~~~    Will Draw The Picture Streached To Its New Given Dimentions...
//
// InPut:  pDC - Given DC To Draw On
// ~~~~~   LeftTop - Opening Point To Start Drawing (Left,Top)
//         WidthHeight - Dimentions Of The Picture To Draw (Width,Height)
//         MagnifyX - Magnify Pixel Width, 0 = Default (No Magnify)
//         MagnifyY - Magnify Pixel Height, 0 = Default (No Magnify)
//
// OutPut: TRUE If Succeeded...
// ~~~~~~
//-----------------------------------------------------------------------------
BOOL CPicture::Show(CDC *pDC, CPoint LeftTop, CPoint WidthHeight, int MagnifyX, int MagnifyY)
//=============================================================================
{
    if (pDC == NULL || m_IPicture == NULL) return FALSE;
    
    long Width  = 0;
    long Height = 0;
    m_IPicture->get_Width(&Width);
    m_IPicture->get_Height(&Height);
	if(MagnifyX == NULL) MagnifyX = 0;
	if(MagnifyY == NULL) MagnifyY = 0;
	MagnifyX = int(MulDiv(Width, pDC->GetDeviceCaps(LOGPIXELSX), HIMETRIC_INCH) * MagnifyX);
	MagnifyY = int(MulDiv(Height,pDC->GetDeviceCaps(LOGPIXELSY), HIMETRIC_INCH) * MagnifyY);

	CRect DrawRect(LeftTop.x, LeftTop.y, MagnifyX, MagnifyY);

    HRESULT hrP = NULL;

    hrP = m_IPicture->Render(pDC->m_hDC,
                      LeftTop.x,               // Left
                      LeftTop.y,               // Top
                      WidthHeight.x +MagnifyX, // Width
                      WidthHeight.y +MagnifyY, // Height
                      0,
                      Height,
                      Width,
                      -Height,
                      &DrawRect);

    if(SUCCEEDED(hrP)) return(TRUE);

	HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
	MessageBoxEx(hWnd, "Can not allocate enough memory\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
    return(FALSE);
}

//-----------------------------------------------------------------------------
// Does:   Saves The Picture That Is Stored In The IPicture Object As a Bitmap
// ~~~~    (Converts From Any Known Picture Type To a Bitmap / Icon File)
//
// InPut:  sFilePathName - Path And FileName Target To Save
// ~~~~~
//
// OutPut: TRUE If Succeeded...
// ~~~~~~
//-----------------------------------------------------------------------------
BOOL CPicture::SaveAsBitmap(CString sFilePathName)
//=============================================================================
{
	BOOL bResult = FALSE;
	ILockBytes *Buffer = 0;
	IStorage   *pStorage = 0;
	IStream    *FileStream = 0;
	BYTE	   *BufferBytes;
	STATSTG		BytesStatistics;
	DWORD		OutData;
	long		OutStream;
	CFile		BitmapFile;	CFileException e;
	double		SkipFloat = 0;
	DWORD		ByteSkip = 0;
	_ULARGE_INTEGER RealData;

	CreateILockBytesOnHGlobal(NULL, TRUE, &Buffer); // Create ILockBytes Buffer

	HRESULT hr = ::StgCreateDocfileOnILockBytes(Buffer,
				 STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, &pStorage);

	hr = pStorage->CreateStream(L"PICTURE",
		 STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, 0, &FileStream);

	m_IPicture->SaveAsFile(FileStream, TRUE, &OutStream); // Copy Data Stream
	FileStream->Release();
	pStorage->Release();
	Buffer->Flush(); 

	// Get Statistics For Final Size Of Byte Array
	Buffer->Stat(&BytesStatistics, STATFLAG_NONAME);

	// Cut UnNeeded Data Coming From SaveAsFile() (Leave Only "Pure" Picture Data)
	SkipFloat = (double(OutStream) / 512); // Must Be In a 512 Blocks...
	if(SkipFloat > DWORD(SkipFloat)) ByteSkip = (DWORD)SkipFloat + 1;
	else ByteSkip = (DWORD)SkipFloat;
	ByteSkip = ByteSkip * 512; // Must Be In a 512 Blocks...
	
	// Find Difference Between The Two Values
	ByteSkip = (DWORD)(BytesStatistics.cbSize.QuadPart - ByteSkip);

	// Allocate Only The "Pure" Picture Data
	RealData.LowPart = 0;
	RealData.HighPart = 0;
	RealData.QuadPart = ByteSkip;
	BufferBytes = (BYTE*)malloc(OutStream);
	if(BufferBytes == NULL)
		{
		Buffer->Release();
		HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
		MessageBoxEx(hWnd, "Can not allocate enough memory\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		}

	Buffer->ReadAt(RealData, BufferBytes, OutStream, &OutData);

	if(BitmapFile.Open(sFilePathName, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite, &e))
		{
		BitmapFile.Write(BufferBytes, OutData);
		BitmapFile.Close();
		bResult = TRUE;
		}
	else // Write File Failed...
		{
		TCHAR szCause[255];
		e.GetErrorMessage(szCause, 255, NULL);
		HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
		MessageBoxEx(hWnd, szCause, ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		bResult = FALSE;
		}
	
	Buffer->Release();
	free(BufferBytes);

	return(bResult);
}

//-----------------------------------------------------------------------------
// Does:   Draw a Bitmap Resource To The Client DC (Using Bitblt())
// ~~~~    It Will Use The Bitmap Resource Original Size (Width And Height)
//         (.BMP .DIB)
//
// Note:   This Function Is Just Another Simple Way Of Displaying a Bitmap Resource,
// ~~~~    It Is Not Connected With The IPicture Interface And Can Be Used
//         As a StandAlone On Any Device Context (Directly)
//
// InPut:  BMPResource - Resource Name As Defined In The Resources
// ~~~~~   pDC - Given DC To Draw On
//         LeftTop - Opening Point To Start Drawing (Left,Top)
//
// OutPut: TRUE If Succeeded...
// ~~~~~~
//-----------------------------------------------------------------------------
BOOL CPicture::ShowBitmapResource(CDC *pDC, const int BMPResource, CPoint LeftTop)
//=============================================================================
{
   if (pDC == NULL) return(FALSE);

   CBitmap BMP;
   if(BMP.LoadBitmap(BMPResource))
		{
		// Get Bitmap Details
		BITMAP BMPInfo;
		BMP.GetBitmap(&BMPInfo);

		// Create An In-Memory DC Compatible With The Display DC We R Gonna Paint On
		CDC DCMemory;
		DCMemory.CreateCompatibleDC(pDC);

		// Select The Bitmap Into The In-Memory DC
		CBitmap* pOldBitmap = DCMemory.SelectObject(&BMP);

		// Copy Bits From The In-Memory DC Into The On-Screen DC
		pDC->BitBlt(LeftTop.x, LeftTop.y, BMPInfo.bmWidth, BMPInfo.bmHeight, &DCMemory, 0, 0, SRCCOPY);

		DCMemory.SelectObject(pOldBitmap); // (As Shown In MSDN Example...)
		}
	else 
		{
		TRACE0("ERROR: Can Not Find The Bitmap Resource\n");
		return(FALSE);
		}

	return(TRUE);
}

//-----------------------------------------------------------------------------
// Does:   Get The Original Picture Pixel Size (Ignor What Current DC Is Using)
// ~~~~    Pointer To a Device Context Is Needed For Pixel Calculation,
//
//         Also Updates The Class's Height And Width Properties,
//         (Coz Till Now We Had No Device Context To Work With...96 DPI Assumed)
//
// InPut:  The Client DC (Needed To Check The Size Of The Pixels)
// ~~~~~
//
// OutPut: TRUE If Succeeded...
// ~~~~~~
//-----------------------------------------------------------------------------
BOOL CPicture::UpdateSizeOnDC(CDC *pDC)
//=============================================================================
{
	if(pDC == NULL || m_IPicture == NULL) { m_Height = 0; m_Width = 0; return(FALSE); };

    m_IPicture->get_Height(&m_Height);
    m_IPicture->get_Width(&m_Width);

	// Get Current DPI - Dot Per Inch
    int CurrentDPI_X = pDC->GetDeviceCaps(LOGPIXELSX);
    int CurrentDPI_Y = pDC->GetDeviceCaps(LOGPIXELSY);

	// Use a "Standard" Print (When Printing)
    if(pDC->IsPrinting())
		{
		CurrentDPI_X = 96;
        CurrentDPI_Y = 96;
		}

    m_Height = MulDiv(m_Height, CurrentDPI_Y, HIMETRIC_INCH);
    m_Width  = MulDiv(m_Width,  CurrentDPI_X, HIMETRIC_INCH);

    return(TRUE);
}




BOOL CPicture::CopyToPicture(CPicture* dstPic,CString exchangeFile)
//=============================================================================
{

	if (m_IPicture==NULL) 
		return TRUE;
	
	if (!SaveAsBitmap(exchangeFile))
		return FALSE;
	if (!dstPic->Load(exchangeFile))
		return FALSE;
		

	/* // Crash!
	BOOL bResult = FALSE;
	ILockBytes *Buffer = 0;
	IStorage   *pStorage = 0;
	IStream    *FileStream = 0;	
	long		OutStream;	

	CreateILockBytesOnHGlobal(NULL, TRUE, &Buffer); // Create ILockBytes Buffer

	HRESULT hr = ::StgCreateDocfileOnILockBytes(Buffer,
				 STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, &pStorage);

	hr = pStorage->CreateStream(L"PICTURE",
		 STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, 0, &FileStream);

	m_IPicture->SaveAsFile(FileStream, TRUE, &OutStream); // Copy Data Stream
	
	
	if((hr = OleLoadPicture(FileStream, OutStream, FALSE, IID_IPicture, (LPVOID *)&dstPic->m_IPicture)) == E_NOINTERFACE)
	{
		HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
		MessageBoxEx(hWnd, "IPicture interface is not supported\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		bResult = FALSE;
	}
	else	
		bResult = TRUE;
		

	FileStream->Release();
	FileStream = NULL;
	
	pStorage->Release();
	Buffer->Flush();
	*/
	

	return(TRUE);
}


BOOL CPicture::SaveToFile(FILE* fptr)
//=============================================================================
{

	DWORD sizefile = 0;
	if (fptr == NULL)
		return FALSE;

	if (m_IPicture==NULL) {
	
		fwrite( (void *) &sizefile, sizeof(DWORD), 1, fptr );
		return TRUE;

	}	



	BOOL bResult = FALSE;
	ILockBytes *Buffer = 0;
	IStorage   *pStorage = 0;
	IStream    *FileStream = 0;
	BYTE	   *BufferBytes;
	STATSTG		BytesStatistics;
	DWORD		OutData;
	long		OutStream;
	CFile		BitmapFile;	CFileException e;
	double		SkipFloat = 0;
	DWORD		ByteSkip = 0;
	_ULARGE_INTEGER RealData;

	CreateILockBytesOnHGlobal(NULL, TRUE, &Buffer); // Create ILockBytes Buffer

	HRESULT hr = ::StgCreateDocfileOnILockBytes(Buffer,
				 STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, &pStorage);

	hr = pStorage->CreateStream(L"PICTURE",
		 STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, 0, &FileStream);

	m_IPicture->SaveAsFile(FileStream, TRUE, &OutStream); // Copy Data Stream
	FileStream->Release();
	pStorage->Release();
	Buffer->Flush(); 

	// Get Statistics For Final Size Of Byte Array
	Buffer->Stat(&BytesStatistics, STATFLAG_NONAME);

	// Cut UnNeeded Data Coming From SaveAsFile() (Leave Only "Pure" Picture Data)
	SkipFloat = (double(OutStream) / 512); // Must Be In a 512 Blocks...
	if(SkipFloat > DWORD(SkipFloat)) ByteSkip = (DWORD)SkipFloat + 1;
	else ByteSkip = (DWORD)SkipFloat;
	ByteSkip = ByteSkip * 512; // Must Be In a 512 Blocks...
	
	// Find Difference Between The Two Values
	ByteSkip = (DWORD)(BytesStatistics.cbSize.QuadPart - ByteSkip);

	// Allocate Only The "Pure" Picture Data
	RealData.LowPart = 0;
	RealData.HighPart = 0;
	RealData.QuadPart = ByteSkip;
	BufferBytes = (BYTE*)malloc(OutStream);
	if(BufferBytes == NULL)
		{
		Buffer->Release();
		HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
		MessageBoxEx(hWnd, "Can not allocate enough memory\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		}

	Buffer->ReadAt(RealData, BufferBytes, OutStream, &OutData);


	size_t written = fwrite( (void *) &OutData, sizeof(DWORD), 1, fptr );
	if (written<1) {
		MessageBoxEx(NULL, "Unable to write image size\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		return FALSE;
	}

	written = fwrite( (void *) BufferBytes, OutData, 1, fptr );
	if (written<1) {
		MessageBoxEx(NULL, "Unable to write image\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		return FALSE;
	}

	Buffer->Release();
	if (BufferBytes)
		free(BufferBytes);

		

	return TRUE;
	

}


BOOL CPicture::LoadFromFile(FILE* fptr)
//=============================================================================
{

	DWORD OutData;
	BYTE* BufferBytes = NULL;

	if(m_IPicture != NULL) FreePictureData(); // Important - Avoid Leaks...

	
	size_t readcount = fread( (void *) &OutData, sizeof(DWORD), 1, fptr );
	if (readcount<1) {
		MessageBoxEx(NULL, "Unable to read image size\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		return FALSE;
	}


	if (OutData <= 0)
		return TRUE;



	BufferBytes = (BYTE *) malloc(OutData+1);
	readcount = fread( (void *) BufferBytes, OutData, 1, fptr );
	if (readcount<1) {
		MessageBoxEx(NULL, "Unable to read image\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		return FALSE;
	}


	BOOL bResult = FALSE;	
	CFileException e;

	
	
	
	if (LoadPictureData(BufferBytes, OutData)) 
	{

		bResult = TRUE;
		
	}
	else // Open Failed...
	{
		TCHAR szCause[255];
		e.GetErrorMessage(szCause, 255, NULL);
		HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
		MessageBoxEx(NULL, szCause, ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		bResult = FALSE;
	}

	if (BufferBytes)
			free(BufferBytes);

	m_Weight = OutData; // Update Picture Size Info...

	if(m_IPicture != NULL) // Do Not Try To Read From Memory That Is Not Exist...
		{ 
		m_IPicture->get_Height(&m_Height);
		m_IPicture->get_Width(&m_Width);
	    // Calculate Its Size On a "Standard" (96 DPI) Device Context
	    m_Height = MulDiv(m_Height, 96, HIMETRIC_INCH);
	    m_Width  = MulDiv(m_Width,  96, HIMETRIC_INCH);
		}
	else // Picture Data Is Not a Known Picture Type
		{
		m_Height = 0;
		m_Width = 0;
		bResult = FALSE;
		}

	return(bResult);

}