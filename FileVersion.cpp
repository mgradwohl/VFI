// FileVersion.cpp: implementation of the CFileVersion class.
//
//////////////////////////////////////////////////////////////////////

#include "filelib.h"
#include "strlib.h"

//#include "../stdafx.h"
#include "FileVersion.h"

#pragma comment(lib, "version.lib")

#ifdef _DEBUG
	#undef THIS_FILE
	static CHAR THIS_FILE[]=__FILE__;
	#ifdef DEBUG_NEW
		#define new DEBUG_NEW
	#endif
#endif

CFileVersion::CFileVersion()
{
	m_pszFile = NULL;
	m_fRead = false;
	m_V1 = m_V2 = m_V3 = m_V4 = 0;
}

CFileVersion::~CFileVersion()
{
	m_V1 = m_V2 = m_V3 = m_V4 = 0;

	delete [] m_pszFile;
	m_pszFile = NULL;
	m_fRead = false;
}

bool CFileVersion::Open(LPCWSTR pszFile)
{
	if(NULL == pszFile)
	{
		return false;
	}

	WCHAR szTemp[MAX_PATH];
	if (!DoesFileExist(pszFile))
	{
		// look in the Module Directory
		GetModuleFolder(GetModuleHandle(NULL), szTemp);
		lstrcat(szTemp, pszFile);
		if (!DoesFileExist(szTemp))
		{
			// look in the Windows Directory
			if (0 == GetWindowsDirectory(szTemp, MAX_PATH))
			{
				return false;
			}
			PathAppend(szTemp, pszFile);
			if (!DoesFileExist(szTemp))
			{
				// look in the System Directory
				GetSystemDirectory(szTemp, MAX_PATH);
				PathAppend(szTemp, pszFile);
				if (!DoesFileExist(szTemp))
				{
					return false;
				}
			}
		}
	}
	else
	{
		lstrcpy(szTemp, pszFile);
	}

	if (NULL != m_pszFile)
	{
		delete [] m_pszFile;
		m_pszFile = NULL;
	}

	m_pszFile = new WCHAR[lstrcb(szTemp) + 1];
	if(NULL == m_pszFile)
	{
		return false;
	}

	lstrcpy(m_pszFile, szTemp);
	return true;
}

bool CFileVersion::GetVersion4(LPWSTR pszVersion)
{
	if ((NULL == pszVersion) || ('\0' == *pszVersion))
	{
		return false;
	}

	if (!m_fRead)
	{
		if(!ReadVersionInfo())
		{
			return false;
		}
	}

	WCHAR szDec[2];
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDec, 2);
	wsprintf(pszVersion, L"%d%s%02d%s%02d%s%04d", m_V1, szDec, m_V2, szDec, m_V3, szDec, m_V4);
	return true;
}


bool CFileVersion::GetVersion3(LPWSTR pszVersion)
{
	if (NULL == pszVersion)
	{
		return false;
	}

	if (!m_fRead)
	{
		if(!ReadVersionInfo())
		{
			return false;
		}
	}

	WCHAR szDec[2];
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDec, 2);
	wsprintf(pszVersion, L"%d%s%02d%s%04d", m_V1, szDec, m_V2, szDec, m_V3);
	return true;
}

bool CFileVersion::ReadVersionInfo()
{
	LPVOID pVerInfoBlock = NULL;
	VS_FIXEDFILEINFO* pVerInfo = NULL;
	UINT cbVerInfo = 0;
	DWORD cbVerInfoBlock = 0;
	DWORD dwUnused = 0;

	cbVerInfoBlock = GetFileVersionInfoSize( m_pszFile, &dwUnused );
	if ( 0 == cbVerInfoBlock )
	{
		return false;
	}
	
	pVerInfoBlock = GlobalAlloc( GPTR, cbVerInfoBlock );
	if ( NULL == pVerInfoBlock )
	{
		return false;
	}
	
	if ( !GetFileVersionInfo( m_pszFile, NULL, cbVerInfoBlock, pVerInfoBlock ) )
	{
		GlobalFree( pVerInfoBlock );
		return false;
	}
	
	if( !VerQueryValue(pVerInfoBlock, L"\\", (void **)&pVerInfo, &cbVerInfo) )
	{
		GlobalFree( pVerInfoBlock );
		return false;
	}
	
	m_V1 = HIWORD(pVerInfo->dwProductVersionMS);
	m_V2 = LOWORD(pVerInfo->dwProductVersionMS);
	m_V3 = HIWORD(pVerInfo->dwProductVersionLS);
	m_V4 = LOWORD(pVerInfo->dwProductVersionLS);
	m_fRead = true;
	
	GlobalFree( pVerInfoBlock );
	return true;
}
