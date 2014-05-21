//
// Localization.cpp
//
// This file is part of Localization library.
// Copyright (c) Nikolay Raspopov, 2011-2012.
// E-mail: ryo.rabbit@gmail.com
// Web: http://code.google.com/p/po-localization/
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "Localization.h"

#include <atlfile.h>		// for CAtlFile, CAtlTemporaryFile
#include <windowsx.h>		// for ComboBox_*
#include <lzexpand.h>		// for LZOpenFile,LZRead,LZClose
#pragma comment(lib,"lz32")	// for LZOpenFile,LZRead,LZClose

#ifdef _DEBUG
#define LOG		ATLTRACE
#else
#define LOG		__noop
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define LZ_MAGIC	('DDZS')		// Lempel-Ziv algorithm magic number: 'SZDD' (in reverse order)
#define ORDINAL		0x80000000

static inline int hs2b(TCHAR s)
{
	return ( ( s >= '0' && s <= _T('9') ) ? ( s - _T('0') ) :
		( s >= 'a' && s <= _T('f') ) ? ( s - _T('a') + 10 ) :
		( s >= 'A' && s <= _T('F') ) ? ( s - _T('A') + 10 ) : -1 );
}

static inline CString& UnMakeSafe(CString& str)
{
	CString tmp;
	LPTSTR dst = tmp.GetBuffer( str.GetLength() + 1 );
	for ( LPCTSTR src = str; *src; src++ )
	{
		if ( *src == _T('\\') )
		{
			switch ( *(src + 1) )
			{
			case _T('\\'):
				*dst++ = _T('\\');
				src++;
				break;

			case _T('r'):
				*dst++ = _T('\r');
				src++;
				break;

			case _T('n'):
				*dst++ = _T('\n');
				src++;
				break;

			case _T('t'):
				*dst++ = _T('\t');
				src++;
				break;

			case _T('\"'):
				*dst++ = _T('\"');
				src++;
				break;

			default:
				*dst++ = *src;
			}
		}
		else
			*dst++ = *src;
	}
	*dst = 0;
	tmp.ReleaseBuffer();
	str = tmp;
	return str;
}

static inline CString Decode(LPCSTR szInput, int nInput = -1)
{
	//int nWide = MultiByteToWideChar( CP_UTF8, 0, szInput, nInput, NULL, 0 );
	CString sWide = szInput;
	//if ( nWide > 0 )
	//{
	//	MultiByteToWideChar( CP_UTF8, 0, szInput, nInput, sWide.GetBuffer( nWide ), nWide );
	//	sWide.ReleaseBuffer( nWide - ( ( nInput == -1 ) ? 1 : 0 ) );
		UnMakeSafe( sWide );
	//}
	return sWide;
}

CLocalization::CLocalization(LANGID nDefault)
	: m_nDefaultLangID	( nDefault )
	, m_nLangID			( nDefault )
{
}

CLocalization::~CLocalization()
{
	Empty();
}

void CLocalization::Empty()
{
	m_nLangID = m_nDefaultLangID;

	m_pStrings.RemoveAll();
	m_pMenus.RemoveAll();
	m_pDialogs.RemoveAll();
}

void CLocalization::Reload()
{
	LANGID nCurrent = m_nLangID;
	Empty();
	Load( m_strModule );
	Select( nCurrent );
}

CString CLocalization::LoadString(UINT nID) const
{
	CString sString;
	if ( m_pStrings.Lookup( nID, sString ) )
		return sString;

	sString.LoadString( nID );
	return sString;
}

BOOL CLocalization::Translate(HMENU hMenu, UINT nMenuID, UINT* pnOrdinal) const
{
	UINT nLocalOrdinal = 0;
	if ( pnOrdinal == NULL )
		pnOrdinal = &nLocalOrdinal;

	UINT nCount = GetMenuItemCount( hMenu );
	for ( UINT i = 0; i < nCount; ++i )
	{
		MENUITEMINFO mii =
		{
			sizeof( MENUITEMINFO ),
			MIIM_STRING | MIIM_SUBMENU | MIIM_ID
		};
		if ( ! GetMenuItemInfo( hMenu, i, TRUE, &mii ) )
			return FALSE;

		UINT nID = 0;
		if ( mii.hSubMenu )
			nID = ORDINAL | *pnOrdinal;
		else if ( ( mii.fMask & MIIM_ID ) == MIIM_ID )
			nID = mii.wID;

		if ( ( mii.fMask & MIIM_STRING ) == MIIM_STRING )
		{
			CString sItem;
			if ( m_pMenus.Lookup( ( nMenuID << 16 ) | nID, sItem ) )
			{
				mii.fMask = MIIM_STRING;
				mii.dwTypeData = (LPTSTR)(LPCTSTR)sItem;
				mii.cch = sItem.GetLength();
				if ( ! SetMenuItemInfo( hMenu, i, TRUE, &mii ) )
					return FALSE;
			}
		}

		if ( mii.hSubMenu )
		{
			(*pnOrdinal) ++;
			if ( ! Translate( mii.hSubMenu, nMenuID, pnOrdinal ) )
				return FALSE;
		}
	}

	return TRUE;
}

void CLocalization::Translate(HWND hDialog, UINT nDialogID) const
{
	// Translate dialog caption
	CString sItem;
	if ( m_pDialogs.Lookup( ( nDialogID << 16 ), sItem ) )
	{
		SetWindowText( hDialog, sItem );
	}

	// Translate all dialog controls
	UINT i = 0;
	for ( HWND hChild = GetWindow( hDialog, GW_CHILD ); hChild; hChild = GetWindow( hChild, GW_HWNDNEXT ) )
	{
		if ( UINT nControlID = GetDlgCtrlID( hChild ) & 0xffff )
		{
			if ( nControlID == 0xffff )
				nControlID = ORDINAL | i++;

			if ( m_pDialogs.Lookup( ( nDialogID << 16 ) | nControlID, sItem ) )
			{
				SetWindowText( hChild, sItem );
			}
		}
	}

	InvalidateRect( hDialog, NULL, TRUE );
	UpdateWindow( hDialog );
}

BOOL CLocalization::Add(const CString& sRef, const CString& sTranslated)
{
	int nPos = 0;
	CString sType = sRef.Tokenize( _T("."), nPos );
	if ( sType.CompareNoCase( _T("STRING") ) == 0 )
	{
		// #: STRING.{string-id}
		UINT nID = _tstol( sRef.Tokenize( _T("."), nPos ) );
		if ( nID > 0 && nID < 65536 )
		{
			m_pStrings.SetAt( nID, sTranslated );
			return TRUE;
		}
		else
			LOG( "CLocalization Error : Bad string ID: %s\n", (LPCSTR)CT2A( sRef ) );
	}
	else if ( sType.CompareNoCase( _T("MENUITEM") ) == 0 )
	{
		// #: MENUITEM.{menu-id}.{menu-item-id}
		UINT nMenuID = _tstol( sRef.Tokenize( _T("."), nPos ) );
		if ( nMenuID  > 0 && nMenuID < 32768 )
		{
			UINT nItemID = _tstol( sRef.Tokenize( _T("."), nPos ) );
			if ( nItemID  > 0 && nItemID < 65536 )
			{
				m_pMenus.SetAt( ( nMenuID << 16 ) | nItemID, sTranslated );
				return TRUE;
			}
			else
				LOG( "CLocalization Error : Bad menu item ID: %s\n", (LPCSTR)CT2A( sRef ) );
		}
		else
			LOG( "CLocalization Error : Bad menu ID: %s\n", (LPCSTR)CT2A( sRef ) );
	}
	else if ( sType.CompareNoCase( _T("MENUPOPUP") ) == 0 )
	{
		// #: MENUPOPUP.{menu-id}.{ordinal}
		UINT nMenuID = _tstol( sRef.Tokenize( _T("."), nPos ) );
		if ( nMenuID  > 0 && nMenuID < 32768 )
		{
			UINT nSubMenu = _tstol( sRef.Tokenize( _T("."), nPos ) );
			if ( nSubMenu < 65536 )
			{
				m_pMenus.SetAt( ORDINAL | ( nMenuID << 16 ) | nSubMenu, sTranslated );
				return TRUE;
			}
			else
				LOG( "CLocalization Error : Bad menu popup ordinal: %s\n", (LPCSTR)CT2A( sRef ) );
		}
		else
			LOG( "CLocalization Error : Bad menu ID: %s\n", (LPCSTR)CT2A( sRef ) );
	}
	else if ( sType.CompareNoCase( _T("DIALOGCAPTION") ) == 0 )
	{
		// #: DIALOGCAPTION.{dialog-id}
		UINT nDialogID = _tstol( sRef.Tokenize( _T("."), nPos ) );
		if ( nDialogID  > 0 && nDialogID < 65536 )
		{
			m_pDialogs.SetAt( ( nDialogID << 16 ), sTranslated );
			return TRUE;
		}
		else
			LOG( "CLocalization Error : Bad dialog ID: %s\n", (LPCSTR)CT2A( sRef ) );
	}
	else if ( sType.CompareNoCase( _T("DIALOGCONTROL") ) == 0 )
	{
		// #: DIALOGCONTROL.{dialog-id}.{control-class}.{dialog-control-id}
		UINT nDialogID = _tstol( sRef.Tokenize( _T("."), nPos ) );
		if ( nDialogID  > 0 && nDialogID < 65536 )
		{
			CString sClass = sRef.Tokenize( _T("."), nPos );
			if ( ! sClass.IsEmpty() )
			{
				UINT nControlID = (UINT)_tstoi64( sRef.Tokenize( _T("."), nPos ) );
				if ( ( nControlID & 0x7fffffff ) < 65536 )
				{
					m_pDialogs.SetAt( ( nDialogID << 16 ) | nControlID, sTranslated );
					return TRUE;
				}
				else
					LOG( "CLocalization Error : Bad dialog control ID: %s\n", (LPCSTR)CT2A( sRef ) );
			}
			else
				LOG( "CLocalization Error : Bad dialog control class: %s\n", (LPCSTR)CT2A( sRef ) );
		}
		else
			LOG( "CLocalization Error : Bad dialog ID: %s\n", (LPCSTR)CT2A( sRef ) );
	}
	else
		LOG( "CLocalization Error : Unknown reference token: %s\n", (LPCSTR)CT2A( sRef ) );

	return FALSE;
}

BOOL CLocalization::Load(LPCTSTR szModule)
{
	m_pLanguages.RemoveAll();

	CString sPath;
	if ( szModule && *szModule )
		GetFullPathName( szModule, 1024, sPath.GetBuffer( 1024 ), NULL );
	else
		GetModuleFileName( NULL, sPath.GetBuffer( 1024 ), 1024 );
	sPath.ReleaseBuffer();
	m_strModule = sPath;
	int nPos = m_strModule.ReverseFind( _T('\\') );
	BOOL bFolder = ( ( m_strModule.GetLength() - 1 ) == nPos );

	// Enumerate resources first
	if ( ! bFolder )
	{
		if ( HMODULE hModule = LoadLibraryEx( m_strModule, NULL, LOAD_LIBRARY_AS_DATAFILE ) )
		{
			EnumResourceNames( hModule, RT_PO, EnumResNameProc, (LONG_PTR)this );

			FreeLibrary( hModule );
		}
	}

	// Enumerate external files next
	WIN32_FIND_DATA wfd = {};
	HANDLE ff = FindFirstFile( m_strModule + _T("*.p?"), &wfd );
	if ( ff != INVALID_HANDLE_VALUE )
	{
		do
		{
			size_t nLen = _tcslen( wfd.cFileName );
			if ( ! ( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) && nLen >= 7 )
			{
				LPCTSTR szExt = PathFindExtension( wfd.cFileName );
				if ( _tcsicmp( szExt, _T(".po") ) == 0 ||
					 _tcsicmp( szExt, _T(".p_") ) == 0 )
				{
					int a = hs2b( wfd.cFileName [ nLen - 7 ] );
					int b = hs2b( wfd.cFileName [ nLen - 6 ] );
					int c = hs2b( wfd.cFileName [ nLen - 5 ] );
					int d = hs2b( wfd.cFileName [ nLen - 4 ] );
					if ( d >= 0 )
					{
						LANGID nLangID =
							( a >= 0 && b >= 0 && c >= 0 ? ( (LANGID)a << 12 ) : 0 ) |
							( b >= 0 && c >= 0 ? ( (LANGID)b << 8 ) : 0 ) |
							( c >= 0 ? ( (LANGID)c << 4 ) : 0 ) |
							( (LANGID)d );
						if ( nLangID != LANG_NEUTRAL && nLangID != m_nDefaultLangID )
						{
							CString sFile = m_strModule.Left( nPos + 1 ) + wfd.cFileName;
							m_pLanguages.SetAt( nLangID, sFile );
						}
					}
				}
			}
		}
		while ( FindNextFile( ff, &wfd ) );

		FindClose( ff );
	}

	return ( m_pLanguages.GetCount() > 0 );
}

BOOL CALLBACK CLocalization::EnumResNameProc(HMODULE /*hModule*/, LPCTSTR /*lpszType*/, LPTSTR lpszName, LONG_PTR lParam)
{
	CLocalization* pThis = (CLocalization*)lParam;

	if ( (UINT_PTR)lpszName < 0xffff )
	{
		pThis->m_pLanguages.SetAt( (WORD)(UINT_PTR)lpszName,
			CString( _T("$") ) + pThis->m_strModule );
	}

	return TRUE;
}

void CLocalization::FillComboBox(HWND hwndCombo) const
{
	ComboBox_ResetContent( hwndCombo );

	int index = ComboBox_AddString( hwndCombo, GetLangName( m_nDefaultLangID ) );
	ComboBox_SetItemData( hwndCombo, index, m_nDefaultLangID );
	if ( m_nDefaultLangID == m_nLangID )
		ComboBox_SetCurSel( hwndCombo, index );

	for( POSITION pos = GetStartPosition(); pos; )
	{
		LANGID nID = GetNext( pos );
		int index = ComboBox_AddString( hwndCombo, GetLangName( nID ) );
		ComboBox_SetItemData( hwndCombo, index, nID );
		if ( nID == m_nLangID )
			ComboBox_SetCurSel( hwndCombo, index );
	}
}

CString CLocalization::GetLangName(LANGID nLangID)
{
	LCID lcID = MAKELCID( nLangID, SORT_DEFAULT );

	CString strLangName;
	GetLocaleInfo( lcID, LOCALE_SENGLANGUAGE, strLangName.GetBuffer( 80 ), 80 );
	strLangName.ReleaseBuffer();

	CString strNativeLangName;
	GetLocaleInfo( lcID, LOCALE_SNATIVELANGNAME, strNativeLangName.GetBuffer( 80 ), 80 );
	strNativeLangName.ReleaseBuffer();

	CString strNativeCountry;
	GetLocaleInfo( lcID, LOCALE_SNATIVECTRYNAME, strNativeCountry.GetBuffer( 80 ), 80 );
	strNativeCountry.ReleaseBuffer();

	return strLangName + _T(" - ") + strNativeLangName + _T(" (") + strNativeCountry + _T(")");
}

BOOL CLocalization::Select(HWND hwndCombo)
{
	int index = ComboBox_GetCurSel( hwndCombo );
	return ( index != CB_ERR ) ?
		Select( (LANGID)ComboBox_GetItemData( hwndCombo, index ) ) :
		Select( m_nDefaultLangID );
}

BOOL CLocalization::Select(LANGID nLangID)
{
	if ( m_nLangID == nLangID )
		// Already loaded
		return TRUE;

	Empty();

	if ( nLangID == m_nDefaultLangID )
		// Default language requires no external file
		return TRUE;

	CString sFile;
	if ( nLangID == LANG_NEUTRAL || ! m_pLanguages.Lookup( nLangID, sFile ) )
	{
		// Using user language as second chance
		LANGID nSecondLangID = GetUserDefaultLangID();
		if ( nSecondLangID == m_nDefaultLangID )
			// Default language requires no external file
			return TRUE;
		if ( m_pLanguages.Lookup( nSecondLangID, sFile ) )
			nLangID = nSecondLangID;
		else
		{
			// ...primary user language
			nSecondLangID &= 0xff;
			if ( nSecondLangID == m_nDefaultLangID )
				// Default language requires no external file
				return TRUE;
			if ( m_pLanguages.Lookup( nSecondLangID, sFile ) )
				nLangID = nSecondLangID;
			else
			{
				// ...using system language as second chance
				nSecondLangID = GetSystemDefaultLangID();
				if ( nSecondLangID == m_nDefaultLangID )
					// Default language requires no external file
					return TRUE;
				if ( m_pLanguages.Lookup( nSecondLangID, sFile ) )
					nLangID = nSecondLangID;
				else
				{
					// ...primary system language
					nSecondLangID &= 0xff;
					if ( nSecondLangID == m_nDefaultLangID )
						// Default language requires no external file
						return TRUE;
					if ( m_pLanguages.Lookup( nSecondLangID, sFile ) )
						nLangID = nSecondLangID;
					else
					{
						LOG( "CLocalization Error : Unknown language ID %02x\n", nLangID );
						return FALSE;
					}
				}
			}
		}
	}

	if ( sFile.GetAt( 0 ) == _T('$') )
		return LoadPoFromResource( nLangID, (LPCTSTR)sFile + 1 );
	else
		return LoadPoFromFile( nLangID, sFile );
}

BOOL CLocalization::LoadPoFromFile(LANGID nLangID, LPCTSTR szFilename)
{
	Empty();

	BOOL bRet = FALSE;

	CAtlFile oFile;
	HRESULT hr = oFile.Create( szFilename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING );
	if ( SUCCEEDED( hr ) )
	{
		ULONGLONG nLength = 0;
		hr = oFile.GetSize( nLength );
		if ( SUCCEEDED( hr ) )
		{
			DWORD magic = 0;
			hr = oFile.Read( &magic, sizeof( magic ) );
			if ( magic == LZ_MAGIC )
			{
				oFile.Close();

				bRet = LoadPoFromArchive( nLangID, szFilename );
			}
			else
			{
				oFile.Seek( 0, FILE_BEGIN );
				CStringA sContent;
				hr = oFile.Read( sContent.GetBuffer( (DWORD)nLength ), (DWORD)nLength );
				sContent.ReleaseBuffer( (DWORD)nLength );
				if ( SUCCEEDED( hr ) )
				{
					bRet = LoadPoFromString( nLangID, sContent );
				}
				else
					LOG( "CLocalization Error : Can't read .po-file %02x: %s\n", nLangID, (LPCSTR)CT2A( szFilename ) );
			}
		}
		else
			LOG( "CLocalization Error : Can't get size of .po-file %02x: %s\n", nLangID, (LPCSTR)CT2A( szFilename ) );
	}
	else
		LOG( "CLocalization Error : Can't open .po-file %02x: %s\n", nLangID, (LPCSTR)CT2A( szFilename ) );

	return bRet;
}

BOOL CLocalization::LoadPoFromResource(LANGID nLangID, LPCTSTR szFilename)
{
	Empty();

	BOOL bRet = FALSE;

	if ( HMODULE hModule = LoadLibraryEx( szFilename, NULL, LOAD_LIBRARY_AS_DATAFILE ) )
	{
		if ( HRSRC hResInfo = FindResource( hModule, MAKEINTRESOURCE( nLangID ), RT_PO ) )
		{
			DWORD nSize = SizeofResource( hModule, hResInfo );
			if ( HGLOBAL hResData = LoadResource( hModule, hResInfo ) )
			{
				if ( const char* hData = (const char*)LockResource( hResData ) )
				{
					DWORD magic = *(DWORD*)hData;
					if ( magic == LZ_MAGIC )
					{
						CAtlTemporaryFile oFile;
						HRESULT hr = oFile.Create( NULL, GENERIC_WRITE );
						if ( SUCCEEDED( hr ) )
						{
							CString sTempFilename = oFile.TempFileName();

							hr = oFile.Write( hData, nSize );
							if ( SUCCEEDED( hr ) )
							{
								oFile.HandsOff();

								bRet = LoadPoFromArchive( nLangID, sTempFilename );

								oFile.HandsOn();
							}
							else
								LOG( "CLocalization Error : Can't write temporary file: %s\n", (LPCSTR)CT2A( sTempFilename ) );
						}
						else
							LOG( "CLocalization Error : Can't create temporary file: %s\n", (LPCSTR)CT2A( szFilename ) );
					}
					else
						bRet = LoadPoFromString( nLangID, CStringA( hData, nSize ) );
				}
				else
					LOG( "CLocalization Error : Can't lock .po-resource %02x: %s\n", nLangID, (LPCSTR)CT2A( szFilename ) );
			}
			else
				LOG( "CLocalization Error : Can't load .po-resource %02x: %s\n", nLangID, (LPCSTR)CT2A( szFilename ) );
		}
		else
			LOG( "CLocalization Error : Can't find .po-resource %02x: %s\n", nLangID, (LPCSTR)CT2A( szFilename ) );

		FreeLibrary( hModule );
	}
	else
		LOG( "CLocalization Error : Can't open .po-resource %02x: %s\n", nLangID, (LPCSTR)CT2A( szFilename ) );

	return bRet;
}

BOOL CLocalization::LoadPoFromArchive(LANGID nLangID, LPCTSTR szFilename)
{
	Empty();

	BOOL bRet = FALSE;

	OFSTRUCT of = { sizeof ( OFSTRUCT ) };
	INT hCompressed = LZOpenFile( (LPTSTR)szFilename, &of, OF_READ | OF_SHARE_DENY_WRITE );
	if ( hCompressed > 0 ) // Status: < 0 - error, == 0 - uncompressed, > 0 - compressed
	{
		CStringA sUncompressed;
		const INT nChunk = 1024;
		for (;;)
		{
			INT nLength = sUncompressed.GetLength();
			LPSTR pBuf = sUncompressed.GetBuffer( nLength + nChunk );
			INT nRead = LZRead( hCompressed, pBuf + nLength, nChunk );
			if ( nRead == 0 )
			{
				// EOF
				sUncompressed.ReleaseBuffer( nLength );
				bRet = LoadPoFromString( nLangID, sUncompressed );
				break;
			}
			else if ( nRead < 0 )
			{
				// Decompression error
				LOG( "CLocalization Error : Can't decompress file: %s\n", (LPCSTR)CT2A( szFilename ) );
				sUncompressed.ReleaseBuffer( nLength );
				break;
			}
			sUncompressed.ReleaseBuffer( nLength + nRead );
		}
		LZClose( hCompressed );
	}
	else
		LOG( "CLocalization Error : Can't open file: %s\n", (LPCSTR)CT2A( szFilename ) );

	return bRet;
}

BOOL CLocalization::LoadPoFromString(LANGID nLangID, const CStringA& sContent)
{
	Empty();

	BOOL bRet = TRUE;

	m_nLangID = nLangID;

	CString sRef;			// #:
	CString sID;			// msgid

	CStringA sString, sOriginalLine;
	enum
	{
		mode_start, mode_ref, mode_msgid, mode_msgstr
	}
	mode = mode_start;

	int nLine = 1;
	int curPos = 0;
	CStringA sLine = sContent.Tokenize( "\n", curPos );
	while ( bRet && ! sLine.IsEmpty() )
	{
		sOriginalLine = sLine;
		sLine.Trim();

		switch ( sLine[ 0 ] )
		{
		case '#':
			if ( mode != mode_ref && mode != mode_start && mode != mode_msgstr )
			{
				LOG( "CLocalization Error : Invalid .po-line #%d: %s\n", nLine, sOriginalLine );
				bRet = FALSE;
				break;
			}
			if ( sLine[ 1 ] == ':' )
			{
				// Ref
				if ( mode == mode_msgstr )
				{
					// Save previous string
					if ( ! sRef.IsEmpty() && ! sString.IsEmpty() )
						Add( sRef, Decode( sString ) );

					sString.Empty();
					sRef.Empty();
					sID.Empty();

					mode = mode_ref;
				}

				if ( ! sString.IsEmpty() )
					sString += " ";
				sString += sLine.Mid( 2 ).Trim();
			}
			// else Comments
			break;

		case 'm':
			if ( sLine.Mid( 0, 7 ) == "msgid \"" )
			{
				// ID
				if ( mode != mode_start && mode != mode_ref )
				{
					LOG( "CLocalization Error : Invalid .po-line #%d: %s\n", nLine, sOriginalLine );
					bRet = FALSE;
					break;
				}

				sRef = sString.Trim();

				sString.Empty();

				sLine = sLine.Mid( 6, sLine.GetLength() - 6 );
				mode = mode_msgid;
			}
			else if ( sLine.Mid( 0, 8 ) == "msgstr \"" )
			{
				// Translation
				if ( mode != mode_msgid )
					sRef = sString.Trim();
				else
					sID = Decode( sString );

				sString.Empty();

				sLine = sLine.Mid( 7, sLine.GetLength() - 7 );
				mode = mode_msgstr;
			}
			else
			{
				// Unknown string
				LOG( "CLocalization Error : Invalid .po-line #%d: %s\n", nLine, sOriginalLine );
				bRet = FALSE;
				break;
			}

		case '\"':
			if ( mode != mode_msgid && mode != mode_msgstr )
			{
				LOG( "CLocalization Error : Invalid .po-line #%d: %s\n", nLine, sOriginalLine );
				bRet = FALSE;
				break;
			}
			if ( sLine[ sLine.GetLength() - 1 ] == '\"' )
			{
				// String continue
				sString += sLine.Mid( 1, sLine.GetLength() - 2 );
			}
			else
			{
				// Unknown string
				LOG( "CLocalization Error : Invalid .po-line #%d: %s\n", nLine, sOriginalLine );
				bRet = FALSE;
				break;
			}
			break;

		default:
			// Empty or unknown lines
			;
		}

		// Get next line
		sLine = sContent.Tokenize( "\n", curPos );
		nLine++;
	} // while

	if ( bRet && mode != mode_msgstr )
	{
		// Unknown string
		LOG( "CLocalization Error : Invalid .po-line #%d: %s\n", nLine, sOriginalLine );
		bRet = FALSE;
	}

	if ( bRet )
	{
		// Save last string
		if ( ! sRef.IsEmpty() && ! sString.IsEmpty() )
			Add( sRef, Decode( sString ) );
	}
	else
	{
		// Clean partial translation
		Empty();
	}

	return bRet;
}