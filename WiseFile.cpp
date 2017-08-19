// Visual File Information
// Copyright (c) Microsoft Corporation
// All rights reserved. 
// 
// MIT License
// 
// Permission is hereby granted, free of charge, to any person obtaining 
// a copy of this software and associated documentation files (the ""Software""), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom 
// the Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED *AS IS*, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
// IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "stdafx.h"
#include "resource.h"
#include "globals.h"
#include "WiseFile.h"
#include "Imagehlp.h"
#include "strlib.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static CHAR THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC (CWiseFile, CObject)

__forceinline void lstrinit(LPWSTR psz)
{
	*psz = '\0';
}

// CWiseFile::CWiseFile
CWiseFile::CWiseFile()
{
	SetState(FWFS_INVALID);

	Init();

	SetState(FWFS_VALID);
}

CWiseFile::CWiseFile(const CWiseFile& rwf)
{
	Copy(rwf);
}

// CWiseFile::CWiseFile
// Param:   LPCWSTR pszFileSpec
CWiseFile::CWiseFile(LPCWSTR pszFileSpec)
{
	SetState(FWFS_INVALID);

	Init();

	if (!Attach(pszFileSpec))
	{
		Init();
	}

	SetState(FWFS_VALID);
}

// CWiseFile::~CWiseFile
CWiseFile::~CWiseFile()
{
	SetState(FWFS_INVALID);

	//Init();
}

// debug helper
#ifdef _DEBUG
void CWiseFile::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
	dc  << "FileSpec: " << m_szFullPath << "\n";
}
#endif//_DEBUG

// CWiseFile::Attach
// Returns: bool 
// Param:   LPCWSTR pszFileSpec
int CWiseFile::Attach(LPCWSTR pszFileSpec)
{
	if (lstrcch(pszFileSpec) < 1 || (lstrcch(pszFileSpec) > MAX_PATH))
	{
		return FWF_ERR_BADPARAM;
	}

	if (0 == StrCmpN(pszFileSpec, L"\\\\?\\", 4))
	{
		return FWF_ERR_SPECIALPATH;
	}

	WIN32_FIND_DATA fd;
	zero(fd);

	HANDLE hff = FindFirstFile(pszFileSpec, &fd);
	if (INVALID_HANDLE_VALUE == hff)
	{
		//FindClose(hff);
		return FWF_ERR_BADPARAM;
	}

	m_dwAttribs = fd.dwFileAttributes;
	if (m_dwAttribs & FILE_ATTRIBUTE_DIRECTORY)
	{
		FindClose(hff);
		return FWF_ERR_FOLDER;
	}

	m_qwSize = GetFileSize64(pszFileSpec);

	FILETIME ft;
	::FileTimeToSystemTime( &(fd.ftCreationTime), &m_stUTCCreation );
	::FileTimeToLocalFileTime( &(fd.ftCreationTime), &ft );
	::FileTimeToSystemTime( &ft, &m_stLocalCreation );

	::FileTimeToSystemTime( &(fd.ftLastAccessTime), &m_stUTCLastAccess);
	::FileTimeToLocalFileTime( &(fd.ftLastAccessTime), &ft);
	::FileTimeToSystemTime( &ft, &m_stLocalLastAccess );

	::FileTimeToSystemTime( &(fd.ftLastWriteTime), &m_stUTCLastWrite);
	::FileTimeToLocalFileTime( &(fd.ftLastWriteTime), &ft);
	::FileTimeToSystemTime( &ft, &m_stLocalLastWrite );

	WCHAR szDrive[_MAX_DRIVE];
	WCHAR szDir[_MAX_DIR];
	LPWSTR pszDot;
	_wsplitpath_s(pszFileSpec, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, m_szName, _MAX_PATH, m_szExt, _MAX_PATH);

	pszDot = CharNext(m_szExt);
	lstrcpy(m_szExt, pszDot);
	
	lstrcpy(m_szFullPath, pszFileSpec);
	lstrcpy(m_szPath, szDrive);
	lstrcat(m_szPath, szDir);
	lstrcpy(m_szShortName, fd.cAlternateFileName);

	if (0 != FindNextFile(hff, &fd))
	{
		FindClose(hff);
		return FWF_ERR_WILDCARD;
	}

	FindClose(hff);
	SetState(FWFS_ATTACHED);
	return FWF_SUCCESS;
}

int CWiseFile::Attach()
{
	return Attach( GetFullPath() );
}

const CWiseFile& CWiseFile::Copy(const CWiseFile& rwf)
{
	m_dwAttribs = rwf.m_dwAttribs;
	m_dwCRC = rwf.m_dwCRC;
	m_dwFlags = rwf.m_dwFlags;
	m_dwOS = rwf.m_dwOS;
	m_dwType = rwf.m_dwType;
	m_qwFileVersion = rwf.m_qwFileVersion;
	m_qwProductVersion = rwf.m_qwProductVersion;
	m_qwSize = rwf.m_qwSize;
	m_stLocalCreation = rwf.m_stLocalCreation;
	m_stLocalLastAccess = rwf.m_stLocalLastAccess;
	m_stLocalLastWrite = rwf.m_stLocalLastWrite;
	m_stUTCCreation = rwf.m_stUTCCreation;
	m_stUTCLastAccess = rwf.m_stUTCLastAccess;
	m_stUTCLastWrite = rwf.m_stUTCLastWrite;
	lstrcpy(m_szAttribs, rwf.m_szAttribs);
	lstrcpy(m_szExt, rwf.m_szExt);
	lstrcpy(m_szFlags, rwf.m_szFlags);
	lstrcpy(m_szFullPath, rwf.m_szFullPath);
	lstrcpy(m_szShortName, rwf.m_szShortName);
	m_CodePage = rwf.m_CodePage;
	m_wFileState = rwf.m_wFileState;
	m_wLanguage = rwf.m_wLanguage;
	
	return *this;
}

const CWiseFile& CWiseFile::operator=(const CWiseFile& rwf)
{
	return Copy(rwf);
}

bool CWiseFile::operator==(const CWiseFile& rwf)
{
	if (CheckState(FWFS_CRC_COMPLETE))
	{
		return m_dwCRC == rwf.m_dwCRC;
	}

	if (CheckState(FWFS_VERSION))
	{
		return ( (0 == lstrcmpi(m_szFullPath, rwf.m_szFullPath))
			&& (m_qwFileVersion == rwf.m_qwFileVersion ));
	}

	if (CheckState(FWFS_ATTACHED))
	{
		return (0 == lstrcmpi(m_szFullPath, rwf.m_szFullPath));
	}

	return false;
}

// CWiseFile::Detach
// Returns: int
int CWiseFile::Detach()
{
	// does nothing
	return FWF_SUCCESS;
}

int CWiseFile::GetSize(LPWSTR pszText, bool bHex)
{
	if (!CheckState(FWFS_ATTACHED))
	{
		lstrinit(pszText);
		return FWF_ERR_INVALID;
	}

	if (bHex)
	{
		wsprintf(pszText, L"%08x", LODWORD(m_qwSize));
		return FWF_SUCCESS;
	}
	else
	{
		// Not using StrFormatByteSize because it wordifies everything
		if (int2str(pszText, LODWORD(m_qwSize)))
		{
			return FWF_SUCCESS;
		}
		else
		{
			return FWF_ERR_OTHER;
		}
	}
}

int CWiseFile::GetSize64(LPWSTR pszText, bool bHex)
{
	if (!CheckState(FWFS_ATTACHED))
	{
		lstrinit(pszText);
		return FWF_ERR_INVALID;
	}

	if (bHex)
	{
		wsprintf(pszText, L"%16x", m_qwSize);
		return FWF_SUCCESS;
	}
	else
	{
		// Not using StrFormatByteSize because it wordifies everything
		if (int2str(pszText, m_qwSize))
		{
			return FWF_SUCCESS;
		}
		else
		{
			return FWF_ERR_OTHER;
		}
	}
}

int CWiseFile::GetCRC(LPWSTR pszText, bool bHex)
{
	if (!CheckState(FWFS_ATTACHED))
	{
		lstrinit(pszText);
		return FWF_ERR_INVALID;
	}

	if (CheckState(FWFS_CRC_COMPLETE))
	{
		if (bHex)
		{
			wsprintf( pszText, L"%08x", m_dwCRC );
		}
		else
		{
			int2str(pszText, m_dwCRC);
		}
		return FWF_SUCCESS;
	}

	WCHAR szBuf[64];
	if (CheckState(FWFS_CRC_ERROR))
	{
		LoadString(AfxGetResourceHandle(), STR_CRC_ERROR, szBuf, 64);
		wsprintf( pszText, L"%s", szBuf);
		return FWF_SUCCESS;
	}

	if (CheckState(FWFS_CRC_PENDING))
	{
		LoadString(AfxGetResourceHandle(), STR_CRC_PENDING, szBuf, 64);
		wsprintf( pszText, L"%s", szBuf);
		return FWF_SUCCESS;
	}

	if (CheckState(FWFS_CRC_WORKING))
	{
		LoadString(AfxGetResourceHandle(), STR_CRC_WORKING, szBuf, 64);
		wsprintf( pszText, L"%s", szBuf);
		return FWF_SUCCESS;
	}

	lstrinit(pszText);
	return FWF_ERR_OTHER;
}

int CWiseFile::GetFileVersion(LPWSTR pszText)
{
	if (!CheckState(FWFS_VERSION))
	{
		lstrinit(pszText);
		return FWF_ERR_INVALID;
	}
	
	if (!m_fHasVersion)
	{
		lstrinit(pszText);
		return FWF_SUCCESS;
	}

	WCHAR szDec[2];
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDec, 2);

	wsprintf( pszText, L"%d%s%02d%s%04d%s%04d",
		(int)HIWORD(HIDWORD(m_qwFileVersion)),
		szDec,
		(int)LOWORD(HIDWORD(m_qwFileVersion)),
		szDec,
		(int)HIWORD(LODWORD(m_qwFileVersion)),
		szDec,
		(int)LOWORD(LODWORD(m_qwFileVersion)) );
	return FWF_SUCCESS;
}

int CWiseFile::GetFileVersion(LPDWORD pdwMS, LPDWORD pdwLS)
{
	if (!CheckState(FWFS_VERSION))
		return FWF_ERR_INVALID;

	if(pdwMS)
		*pdwMS = HIDWORD(m_qwFileVersion);

	if(pdwLS)
		*pdwLS  = LODWORD(m_qwFileVersion);

	return FWF_SUCCESS;
}

int CWiseFile::GetFileVersion(LPWORD pwHighMS, LPWORD pwLowMS, LPWORD pwHighLS, LPWORD pwLowLS)
{
	if (!CheckState(FWFS_VERSION))
		return FWF_ERR_INVALID;

	if (pwHighMS)
		*pwHighMS = HIWORD(HIDWORD(m_qwFileVersion));

	if (pwLowMS)
		*pwLowMS  = LOWORD(HIDWORD(m_qwFileVersion));

	if (pwHighLS)
		*pwHighLS = HIWORD(LODWORD(m_qwFileVersion));

	if (pwLowLS)
		*pwLowLS  = LOWORD(LODWORD(m_qwFileVersion));

	return FWF_SUCCESS;
}

int CWiseFile::GetProductVersion(LPWSTR pszText)
{
	if (!CheckState(FWFS_VERSION))
	{
		lstrinit(pszText);
		return FWF_ERR_INVALID;
	}
	
	if (!m_fHasVersion)
	{
		lstrinit(pszText);
		return FWF_SUCCESS;
	}

	WCHAR szDec[2];
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDec, 2);

	wsprintf( pszText, L"%2d%s%02d%s%04d%s%04d",
		(int)HIWORD(HIDWORD(m_qwProductVersion)),
		szDec,
		(int)LOWORD(HIDWORD(m_qwProductVersion)),
		szDec,
		(int)HIWORD(LODWORD(m_qwProductVersion)),
		szDec,
		(int)LOWORD(LODWORD(m_qwProductVersion)) );
	return FWF_SUCCESS;
}

int CWiseFile::GetProductVersion(LPDWORD pdwMS, LPDWORD pdwLS)
{
	if (!CheckState(FWFS_VERSION))
		return FWF_ERR_INVALID;

	if(pdwMS)
		*pdwMS = HIDWORD(m_qwProductVersion);

	if(pdwLS)
		*pdwLS  = LODWORD(m_qwProductVersion);

	return FWF_SUCCESS;
}

int CWiseFile::GetProductVersion(LPWORD pwHighMS, LPWORD pwLowMS, LPWORD pwHighLS, LPWORD pwLowLS)
{
	if (!CheckState(FWFS_VERSION))
		return FWF_ERR_INVALID;

	if (pwHighMS)
		*pwHighMS = HIWORD(HIDWORD(m_qwProductVersion));

	if (pwLowMS)
		*pwLowMS  = LOWORD(HIDWORD(m_qwProductVersion));

	if (pwHighLS)
		*pwHighLS = HIWORD(LODWORD(m_qwProductVersion));

	if (pwLowLS)
		*pwLowLS  = LOWORD(LODWORD(m_qwProductVersion));

	return FWF_SUCCESS;
}

void CWiseFile::GetTypeString()
{
	if (!CheckState(FWFS_VERSION))
		return;
	
	if (!m_fHasVersion)
		return;

	switch (m_dwType)
	{
		case VFT_UNKNOWN: wsprintf(m_szType, L"VFT_UNKNOWN: 0x%08x", m_dwType);
			break;
		case VFT_APP: lstrcpy(m_szType, L"VFT_APP");
			break;
		case VFT_DLL: lstrcpy(m_szType, L"VFT_DLL");
			break;
		case VFT_DRV: lstrcpy(m_szType, L"VFT_DRV");
			break;
		case VFT_FONT: lstrcpy(m_szType, L"VFT_FONT");
			break;
		case VFT_VXD: lstrcpy(m_szType, L"VFT_VXD");
			break;
		case VFT_STATIC_LIB: lstrcpy(m_szType, L"VFT_STATIC_LIB");
			break;
		default:
		{
			wsprintf(m_szType, L"Reserved: 0x%08x",m_dwType );
		}
	}
	m_fszType = true;
}

int CWiseFile::GetTypeString(LPWSTR pszText)
{
	if (!CheckState(FWFS_VERSION))
	{
		lstrinit(pszText);
		return FWF_ERR_INVALID;
	}
	
	if (!m_fHasVersion)
		return FWF_ERR_INVALID;

	if (!m_fszType)
	{
		GetTypeString();
	}
	
	if (m_fszType)
	{
		return ( (NULL==lstrcpy( pszText, m_szType )) ? lstrcb(m_szType) : FWF_SUCCESS );
	}

	return FWF_SUCCESS;
}

void CWiseFile::GetOSString()
{
	if (!CheckState(FWFS_VERSION))
		return;

	if (!m_fHasVersion)
		return;

	switch(m_dwOS)
	{
		case VOS_UNKNOWN: wsprintf(m_szOS, L"VOS_UNKNOWN: 0x%08x", m_dwOS);
			break;
		case VOS_DOS: lstrcpy(m_szOS, L"VOS_DOS");
			break;
		case VOS_OS216: lstrcpy(m_szOS, L"VOS_OS216");
			break;
		case VOS_OS232: lstrcpy(m_szOS, L"VOS_OS232");
			break;
		case VOS_NT: lstrcpy(m_szOS, L"VOS_NT");
			break;
		case VOS__WINDOWS16: lstrcpy(m_szOS, L"VOS__WINDOWS16");
			break;
		case VOS__PM16: lstrcpy(m_szOS, L"VOS__PM16");
			break;
		case VOS__PM32: lstrcpy(m_szOS, L"VOS__PM32");
			break;
		case VOS__WINDOWS32: lstrcpy(m_szOS, L"VOS__WINDOWS32");
			break;
		case VOS_OS216_PM16: lstrcpy(m_szOS, L"VOS_OS216_PM16");
			break;
		case VOS_OS232_PM32: lstrcpy(m_szOS, L"VOS_OS232_PM32");
			break;
		case VOS_DOS_WINDOWS16: lstrcpy(m_szOS, L"VOS_DOS_WINDOWS16");
			break;
		case VOS_DOS_WINDOWS32: lstrcpy(m_szOS, L"VOS_DOS_WINDOWS32");
			break;
		case VOS_NT_WINDOWS32: lstrcpy(m_szOS, L"VOS_NT_WINDOWS32");
			break;
		default:
		{
			wsprintf(m_szOS, L"Reserved: 0x%08x",m_dwOS );
		}
	}
	m_fszOS = true;
}

int CWiseFile::GetOSString(LPWSTR pszText)
{
	if (!CheckState(FWFS_VERSION))
	{
		lstrinit(pszText);
		return FWF_ERR_INVALID;
	}

	if (!m_fHasVersion)
	{
		*pszText = 0;
		return FWF_SUCCESS;
	}

	if (!m_fszOS)
	{
		GetOSString();
	}

	if (m_fszOS)
	{
		return ( (NULL==lstrcpy( pszText, m_szOS )) ? lstrcb(m_szOS) : FWF_SUCCESS );
	}

	return FWF_SUCCESS;
}

int CWiseFile::GetLanguage(LPWSTR pszText, bool bNumeric)
{
	if (!CheckState(FWFS_VERSION))
	{
		lstrinit(pszText);
		return FWF_ERR_INVALID;
	}

	if (!m_fHasVersion)
	{
		*pszText = 0;
		return FWF_SUCCESS;
	}

	if (bNumeric)
	{
		wsprintf( pszText, L"%04x", (int)m_wLanguage);
	}
	else
	{
		GetLanguageName( m_wLanguage, pszText);
	}
	return FWF_SUCCESS;
}

int CWiseFile::GetCodePage(LPWSTR pszText, bool bNumeric)
{
	if (!CheckState(FWFS_VERSION))
	{
		lstrinit(pszText);
		return FWF_ERR_INVALID;
	}

	if (!m_fHasVersion)
	{
		lstrinit(pszText);
		return FWF_SUCCESS;
	}

	if (bNumeric)
	{
		wsprintf( pszText, L"%04x", (WORD)m_CodePage);
	}
	else
	{
			m_CodePage.GetCodePageName(LANGIDFROMLCID(GetThreadLocale()), m_CodePage, pszText);
	}
	return FWF_SUCCESS;
}

int CWiseFile::GetFullLanguage(LPWSTR pszText, bool bNumeric)
{
	if (!CheckState(FWFS_VERSION))
	{
		lstrinit(pszText);
		return FWF_ERR_INVALID;
	}

	if (!m_fHasVersion)
	{
		lstrinit(pszText);
		return FWF_SUCCESS;
	}

	if (bNumeric)
	{
		wsprintf( pszText, L"%04x, %04x",(int)m_wLanguage,(WORD)m_CodePage );
	}
	else
	{
		WCHAR szTemp1[255];
		WCHAR szSep[3];

		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLIST, szSep, 2);
		lstrcat(szSep, L" ");

		GetLanguageName( m_wLanguage, szTemp1 );
		wsprintf( pszText, L"%s%s%s\0",szTemp1, szSep, (LPCWSTR)m_CodePage);
	}
	return FWF_SUCCESS;
}

int CWiseFile::ReadVersionInfo()
{
	#ifdef TEST
		if (CheckState(FWFS_VERSION))
		{
			TRACE(L"CWiseFile::ReadVersionInfo() warning: already called.\n");
		}
	#endif

	CHAR szPath[MAX_PATH];
	lw2mb(m_szFullPath, szPath, MAX_PATH);

	PLOADED_IMAGE pli;
	pli = ImageLoad(szPath, NULL);
	if (NULL != pli)
	{
		if (pli->Characteristics & IMAGE_FILE_DEBUG_STRIPPED)
		{
			// debug info is stripped
			m_fDebugStripped = true;
		}
		else
		{
			m_fDebugStripped = false;
		}
		ImageUnload(pli);
	}

	DWORD	dwVerSize = 0;
	DWORD	dwVerHandle = 0;
	LPVOID	lpVerBuffer = NULL;
	LPVOID	lpVerData = NULL;
	UINT	cbVerData = 0;

	WCHAR	szShortPath[MAX_PATH];
	if (!GetShortPath(szShortPath, MAX_PATH))
	{
		return FWF_ERR_NOVERSION;
	}

	// this takes a long time to call, max size I've seen is 5476
	dwVerSize = ::GetFileVersionInfoSize( szShortPath, &dwVerHandle);

	if (dwVerSize < 1)
		return FWF_ERR_NOVERSION;

	lpVerBuffer = (LPVOID) new BYTE[dwVerSize];

	if (NULL == lpVerBuffer )
	{
		delete [] lpVerBuffer;
		return FWF_ERR_LOWMEMORY;
	}

	ZeroMemory(lpVerBuffer, dwVerSize);
	if (! ::GetFileVersionInfo( szShortPath, dwVerHandle, dwVerSize, lpVerBuffer))
	{
		delete [] lpVerBuffer;
		return FWF_ERR_NOVERSION;
	}

	if (! ::VerQueryValue(lpVerBuffer, L"\\", &lpVerData, &cbVerData))
	{
		delete [] lpVerBuffer;
		return FWF_ERR_NOVERSION;
	}

	m_fHasVersion = true;
	#define pVerFixedInfo ((VS_FIXEDFILEINFO FAR*)lpVerData)
	m_qwFileVersion = MAKEQWORD(pVerFixedInfo->dwFileVersionLS, pVerFixedInfo->dwFileVersionMS);
	m_qwProductVersion = MAKEQWORD(pVerFixedInfo->dwProductVersionLS, pVerFixedInfo->dwProductVersionMS);
	m_dwType = pVerFixedInfo->dwFileType;
	m_dwOS = pVerFixedInfo->dwFileOS;
	m_dwFlags = pVerFixedInfo->dwFileFlags;
	#undef pVerFixedInfo

	if (!VerQueryValue(lpVerBuffer, L"\\VarFileInfo\\Translation", &lpVerData, &cbVerData))
	{
		delete [] lpVerBuffer;
		return FWF_ERR_NOVERSION;
	}

	m_wLanguage = LOWORD(*(LPDWORD)lpVerData);
	m_CodePage = HIWORD(*(LPDWORD)lpVerData);
	delete [] lpVerBuffer;
	
	OrState(FWFS_VERSION);

	return FWF_SUCCESS;
}

int CWiseFile::GetDateCreation (LPWSTR pszText, bool fLocal)
{
	if (!CheckState(FWFS_ATTACHED))
	{
		lstrinit(pszText);
		return FWF_ERR_INVALID;
	}

	WCHAR szDate[64];
	SYSTEMTIME* pst = NULL;
	if (fLocal)
	{
		pst = &m_stLocalCreation;
	}
	else
	{
		pst = &m_stUTCCreation;
	}

	if (0 == ::GetDateFormat(::GetThreadLocale(), DATE_SHORTDATE, pst, NULL, szDate, 64 ))
	{
		return FWF_ERR_OTHER;
	}
	else
	{
		return ( (NULL==lstrcpy( pszText, szDate )) ? lstrcb(szDate) : FWF_SUCCESS );
	}
}

int CWiseFile::GetDateLastAccess (LPWSTR pszText, bool fLocal)
{
	if (!CheckState(FWFS_ATTACHED))
	{
		lstrinit(pszText);
		return FWF_ERR_INVALID;
	}

	WCHAR szDate[64];
	SYSTEMTIME* pst;
	if (fLocal)
	{
		pst = &m_stLocalLastAccess;
	}
	else
	{
		pst = &m_stUTCLastAccess;
	}

	if (0 == ::GetDateFormat(::GetThreadLocale(), DATE_SHORTDATE, pst, NULL, szDate, 64 ))
	{
		return FWF_ERR_OTHER;
	}
	else
	{
		return ( (NULL==lstrcpy( pszText, szDate )) ? lstrcb(szDate) : FWF_SUCCESS );
	}
}

int CWiseFile::GetDateLastWrite (LPWSTR pszText, bool fLocal)
{
	if (!CheckState(FWFS_ATTACHED))
	{
		lstrinit(pszText);
		return FWF_ERR_INVALID;
	}

	WCHAR szDate[64];
	SYSTEMTIME* pst;
	if (fLocal)
	{
		pst = &m_stLocalLastWrite;
	}
	else
	{
		pst = &m_stUTCLastWrite;
	}

	if (0 == ::GetDateFormat(::GetThreadLocale(), DATE_SHORTDATE, pst, NULL, szDate, 64 ))
	{
		return FWF_ERR_OTHER;
	}
	else
	{
		return ( (NULL == lstrcpy( pszText, szDate )) ? lstrcb(szDate) : FWF_SUCCESS );
	}
}

LPSYSTEMTIME CWiseFile::GetDateCreation( bool fLocal )
{
	if (fLocal)
	{
		return &m_stLocalCreation;
	}
	return &m_stUTCCreation;
}

LPSYSTEMTIME CWiseFile::GetTimeCreation( bool fLocal )
{
	if (fLocal)
	{
		return &m_stLocalCreation;
	}
	return &m_stUTCCreation;
}

int CWiseFile::GetTimeCreation (LPWSTR pszText, bool fLocal)
{
	if (!CheckState(FWFS_ATTACHED))
	{
		lstrinit(pszText);
		return FWF_ERR_INVALID;
	}

	WCHAR szTime[64];
	SYSTEMTIME* pst;
	if (fLocal)
	{
		pst = &m_stLocalCreation;
	}
	else
	{
		pst = &m_stUTCCreation;
	}

	if (0 == ::GetTimeFormat(::GetThreadLocale(), TIME_NOSECONDS, pst, NULL, szTime, 64 ))
	{
			return FWF_ERR_OTHER;
	}
	else
	{
		return ( (NULL==lstrcpy( pszText, szTime )) ? lstrcb(szTime) : FWF_SUCCESS );
	}
}

int CWiseFile::GetTimeLastAccess (LPWSTR pszText, bool fLocal)
{
	if (!CheckState(FWFS_ATTACHED))
	{
		lstrinit(pszText);
		return FWF_ERR_INVALID;
	}

	WCHAR szTime[64];
	SYSTEMTIME* pst;
	if (fLocal)
	{
		pst = &m_stLocalLastAccess;
	}
	else
	{
		pst = &m_stUTCLastAccess;
	}

	if (0 == ::GetTimeFormat(::GetThreadLocale(), TIME_NOSECONDS, pst, NULL, szTime, 64 ))
	{
		return FWF_ERR_OTHER;
	}
	else
	{
		return ( (NULL==lstrcpy( pszText, szTime )) ? lstrcb(szTime) : FWF_SUCCESS );
	}
}

int CWiseFile::GetTimeLastWrite (LPWSTR pszText, bool fLocal)
{
	if (!CheckState(FWFS_ATTACHED))
	{
		lstrinit(pszText);
		return FWF_ERR_INVALID;
	}

	WCHAR szTime[64];
	SYSTEMTIME* pst;
	if (fLocal)
	{
		pst = &m_stLocalLastWrite;
	}
	else
	{
		pst = &m_stUTCLastWrite;
	}

	if (0 == ::GetTimeFormat(::GetThreadLocale(), TIME_NOSECONDS, pst, NULL, szTime, 64 ))
	{
		return FWF_ERR_OTHER;

	}
	else
	{
		return ( (NULL==lstrcpy( pszText, szTime )) ? lstrcb(szTime) : FWF_SUCCESS );
	}
}

int CWiseFile::TouchFileTime(FILETIME* lpTime)
{
	if (!CheckState(FWFS_ATTACHED))
		return FWF_ERR_INVALID;

	if (NULL == lpTime)
		return FWF_ERR_BADPARAM;

	DWORD dwOldAttribs = ::GetFileAttributes( m_szFullPath );
	if (FALSE == ::SetFileAttributes( m_szFullPath, FILE_ATTRIBUTE_NORMAL))
	{
		::SetFileAttributes( m_szFullPath, dwOldAttribs);
		return FWF_ERR_OTHER;
	}

	HANDLE hFile = ::CreateFile( m_szFullPath,
					   GENERIC_WRITE, 
					   FILE_SHARE_READ | FILE_SHARE_WRITE, 
					   NULL, 
					   OPEN_EXISTING, 
					   FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING, 
					   NULL);

	if(INVALID_HANDLE_VALUE == hFile)
	{
		::SetFileAttributes( m_szFullPath, dwOldAttribs);
		::CloseHandle(hFile);
		return FWF_ERR_OTHER;
	} 

	if (FALSE == ::SetFileTime(hFile, lpTime, lpTime, lpTime))
	{
		::SetFileAttributes( m_szFullPath, dwOldAttribs);
		::CloseHandle(hFile);
		return FWF_ERR_OTHER;
	}

	::SetFileAttributes( m_szFullPath, dwOldAttribs);
	::CloseHandle(hFile);

	FILETIME ft;
	::FileTimeToSystemTime( lpTime, &m_stUTCCreation );
	::FileTimeToLocalFileTime( lpTime, &ft );
	::FileTimeToSystemTime( &ft, &m_stLocalCreation );

	::FileTimeToSystemTime( lpTime, &m_stUTCLastAccess);
	::FileTimeToLocalFileTime( lpTime, &ft);
	::FileTimeToSystemTime( &ft, &m_stLocalLastAccess );

	::FileTimeToSystemTime( lpTime, &m_stUTCLastWrite);
	::FileTimeToLocalFileTime( lpTime, &ft);
	::FileTimeToSystemTime( &ft, &m_stLocalLastWrite );

	return true;
}

int CWiseFile::GetAttribs( LPWSTR pszText )
{
	if (!CheckState(FWFS_ATTACHED))
	{
		lstrcpy(m_szAttribs, L"        ");
		return FWF_ERR_INVALID;
	}

	if (m_fszAttribs)
	{
		// the string is already built, return it
		return ( (NULL==lstrcpy( pszText, m_szAttribs )) ? lstrcb(m_szAttribs) : FWF_SUCCESS );
	}
	m_fszAttribs = true;

	if (m_dwAttribs & FILE_ATTRIBUTE_NORMAL)
	{
		lstrcpy(m_szAttribs, L"        ");
		return ( (NULL==lstrcpy( pszText, m_szAttribs )) ? lstrcb(m_szAttribs) : FWF_SUCCESS );
	}

	wsprintf(m_szAttribs, L"%c%c%c%c%c%c%c%c",
		((m_dwAttribs & FILE_ATTRIBUTE_ARCHIVE)		? 'A' : ' '),
		((m_dwAttribs & FILE_ATTRIBUTE_HIDDEN)		? 'H' : ' '),
		((m_dwAttribs & FILE_ATTRIBUTE_READONLY)	? 'R' : ' '),
		((m_dwAttribs & FILE_ATTRIBUTE_SYSTEM)		? 'S' : ' '),
		((m_dwAttribs & FILE_ATTRIBUTE_ENCRYPTED)	? 'E' : ' '),
		((m_dwAttribs & FILE_ATTRIBUTE_COMPRESSED)	? 'C' : ' '),
		((m_dwAttribs & FILE_ATTRIBUTE_TEMPORARY)	? 'T' : ' '),
		((m_dwAttribs & FILE_ATTRIBUTE_OFFLINE)		? 'O' : ' '));
	return ( (NULL==lstrcpy( pszText, m_szAttribs )) ? lstrcb(m_szAttribs) : FWF_SUCCESS );
}

int CWiseFile::GetFlags(LPWSTR pszText)
{
	if ( !CheckState(FWFS_VERSION) )
	{
		lstrinit(pszText);
		return FWF_ERR_INVALID;
	}

	if (!m_fHasVersion)
	{
		lstrinit(pszText);
		return FWF_SUCCESS;
	}

	if (m_fszFlags)
	{
		// the string is already built, return it
		return ( (NULL == lstrcpy( pszText, m_szFlags )) ? lstrcb(m_szFlags) : FWF_SUCCESS );
	}
	m_fszFlags = true;

	// list seperator
	WCHAR szSep[3];
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLIST, szSep, 2);
	lstrcat(szSep, L" ");

	CString str;
	if (m_dwFlags & VS_FF_DEBUG)
	{
		if (m_fDebugStripped)
		{
			str.LoadString(STR_FLAG_DEBUG_STRIPPED);
		}
		else
		{
			str.LoadString(STR_FLAG_DEBUG);
		}
		lstrcat(m_szFlags, str);
		lstrcat(m_szFlags, szSep);
	}
	if (m_dwFlags & VS_FF_PRERELEASE)
	{
		str.LoadString(STR_FLAG_PRERELEASE);
		lstrcat(m_szFlags, str);
		lstrcat(m_szFlags, szSep);
	}
	if (m_dwFlags & VS_FF_PATCHED)
	{
		str.LoadString(STR_FLAG_PATCHED);
		lstrcat(m_szFlags, str);
		lstrcat(m_szFlags, szSep);
	}
	if (m_dwFlags & VS_FF_PRIVATEBUILD)
	{
		str.LoadString(STR_FLAG_PRIVATEBUILD);
		lstrcat(m_szFlags, str);
		lstrcat(m_szFlags, szSep);
	}
	if (m_dwFlags & VS_FF_INFOINFERRED)
	{
		str.LoadString(STR_FLAG_INFOINFERRED);
		lstrcat(m_szFlags, str);
		lstrcat(m_szFlags, szSep);
	}
	if (m_dwFlags & VS_FF_SPECIALBUILD)
	{
		str.LoadString(STR_FLAG_SPECIALBUILD);
		lstrcat(m_szFlags, str);
		lstrcat(m_szFlags, szSep);
	}

	LPWSTR pEnd = lstrrchr(m_szFlags, NULL, szSep[0]);
	if (pEnd)
	{
		lstrinit(pEnd);
	}

	// if we've looked at it, and we still don't have a string for it
	// put a default one in (unless the flags are 00000000)
	if ((lstrcch(m_szFlags) < 3) && m_dwFlags != 0)
	{
		wsprintf(m_szFlags, L"%08x", m_dwFlags);;
	}

	return ( (NULL==lstrcpy( pszText, m_szFlags )) ? lstrcb(m_szFlags) : FWF_SUCCESS );
}

bool GetLanguageName(UINT Language, LPWSTR pszBuf)
{
	if (NULL == pszBuf)
		return false;
	
	WCHAR szTemp[255];

	if (0 == Language)
	{
		VerLanguageName(Language, szTemp, 255);
		wsprintf(pszBuf, L"%lu %s", Language, szTemp);
		return true;
	}

	if (0 == GetLocaleInfo( Language, LOCALE_SLANGUAGE, szTemp, 255))
	{
		VerLanguageName(Language, szTemp, 255);
	}

	wsprintf(pszBuf, L"%lu %s", Language, szTemp);
	return true;
}

bool CWiseFile::GetFieldString(LPWSTR pszBuf, int iField, bool fOptions)
{
	switch (iField)
	{
		case  0: return (FWF_SUCCESS == GetPath(pszBuf));
		break;
		
		case  1: return (FWF_SUCCESS == GetName(pszBuf));
		break;
		
		case  2: return (FWF_SUCCESS == GetExt(pszBuf));
		break;
		
		case  3: return (FWF_SUCCESS == GetSize64(pszBuf));
		break;
		
		case  4: return (FWF_SUCCESS == GetDateCreation(pszBuf));
		break;
	
		case  5: return (FWF_SUCCESS == GetTimeCreation(pszBuf));
		break;
	
		case  6: return (FWF_SUCCESS == GetDateLastWrite(pszBuf));
		break;
	
		case  7: return (FWF_SUCCESS == GetTimeLastWrite(pszBuf));
		break;
	
		case  8: return (FWF_SUCCESS == GetDateLastAccess(pszBuf));
		break;

		case  9: return (FWF_SUCCESS == GetTimeLastAccess(pszBuf));
		break;

		case 10: return (FWF_SUCCESS == GetAttribs(pszBuf));
		break;

		case 11: return (FWF_SUCCESS == GetFileVersion(pszBuf));
		break;
	
		case 12: return (FWF_SUCCESS == GetProductVersion(pszBuf));
		break;
	
		case 13: return (FWF_SUCCESS == GetLanguage(pszBuf));
		break;
	
		case 14: return (FWF_SUCCESS == GetCodePage(pszBuf));
		break;

		case 15: return (FWF_SUCCESS == GetOSString(pszBuf));
		break;
	
		case 16: return (FWF_SUCCESS == GetTypeString(pszBuf));
		break;
	
		case 17: return (FWF_SUCCESS == GetFlags(pszBuf));
		break;

		case 18: return (FWF_SUCCESS == GetCRC(pszBuf));
		break;

		fOptions;
		default:	return false;
	}
}

bool CWiseFile::GetShortPath(LPWSTR pszBuf, int cchBuf)
{
	if (0 == ::GetShortPathName(m_szFullPath, pszBuf, cchBuf))
	{
		return false;
	}
	return true;
}

int CWiseFile::ReadVersionInfoEx()
{
    HINSTANCE h = ::LoadLibraryEx(m_szFullPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if (NULL == h)
	{
		return FWF_ERR_NOVERSION;
	}
	
	HRSRC hrsrc = ::FindResource(h, MAKEINTRESOURCE(1), RT_VERSION);
	if (NULL == hrsrc)
	{
		FreeLibrary(h);
		return FWF_ERR_NOVERSION;
	}

	UINT cb = ::SizeofResource(h, hrsrc);
	if (0 == cb)
	{
		FreeLibrary(h);
		return FWF_ERR_NOVERSION;
	}
	UINT xcb = ::GetFileVersionInfoSize(m_szFullPath, 0);
	ASSERT(cb == xcb);

	LPBYTE pb = (BYTE*) ::LockResource(::LoadResource(h, hrsrc));
	if (NULL == pb)
	{
		FreeLibrary(h);
		return FWF_ERR_NOVERSION;
	}

	VS_FIXEDFILEINFO* pvi = new VS_FIXEDFILEINFO;
	LPBYTE pVerBuffer = pb;

	// Usually before you get here, you call GetFileVersionInfoSize to figure out cb
	// to get that size manually, you have to walk thru umpteen fields
	// then you call get file version info, which supposedly gives you VS_VERSION_INFO
	// but pb points to the VS_VERSION_INFO, so we'll VerQueryValue directly

	// next line causes access violation
	if (! ::VerQueryValue(pb, L"\\", (void**) &pvi, &cb))
	{
		return FWF_ERR_NOVERSION;
	}

	if (pvi->dwSignature != 0xFEEF04BD)
	{
		FreeLibrary(h);
		return FWF_ERR_NOVERSION;
	}

	m_qwFileVersion = MAKEQWORD(pvi->dwFileVersionLS, pvi->dwFileVersionMS);
	m_qwProductVersion = MAKEQWORD(pvi->dwProductVersionLS, pvi->dwProductVersionMS);
	m_dwType = pvi->dwFileType;
	m_dwOS = pvi->dwFileOS;
	m_dwFlags = pvi->dwFileFlags;

	LPVOID pVerData = NULL;
	UINT cbVerData = 0;
	if (!::VerQueryValue(pVerBuffer, L"\\VarFileInfo\\Translation", &pVerData, &cbVerData))
	{
		return FWF_ERR_NOVERSION;
	}

	m_wLanguage = LOWORD(*(LPDWORD)pVerBuffer);
	m_CodePage = HIWORD(*(LPDWORD)pVerBuffer);

	FreeLibrary(h);
	return FWF_SUCCESS;
}

bool CWiseFile::Init()
{
	lstrinit(m_szFullPath);
	lstrinit(m_szPath);
	lstrinit(m_szName);
	lstrinit(m_szExt);
	lstrinit(m_szShortName);
	lstrinit(m_szAttribs);
	lstrinit(m_szFlags);
	lstrinit(m_szOS);
	lstrinit(m_szType);

	zero(m_stLocalCreation);
	zero(m_stLocalLastAccess);
	zero(m_stLocalLastWrite);

	zero(m_stUTCCreation);
	zero(m_stUTCLastAccess);
	zero(m_stUTCLastWrite);
	
	m_dwAttribs = 0;
	m_qwSize = 0;
	m_qwFileVersion = 0;
	m_qwProductVersion = 0;
	m_wLanguage = 0;
	m_CodePage = 0;
	m_dwOS = 0;
	m_dwType = 0;
	m_dwFlags = 0;
	m_dwCRC = 0;

	m_fDebugStripped = false;
	m_fHasVersion = false;
	m_fszFlags = false;	
	m_fszAttribs = false;
	m_fszOS = false;
	m_fszType = false;

	return true;
}