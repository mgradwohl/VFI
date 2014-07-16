// SystemInformation.cpp: implementation of the CSystemInformation class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <ShlObj.h>
#ifdef CSI_WINSOCK
	#ifndef _WINSOCK2API_	
		#include <winsock2.h>
		#pragma comment(lib, L"ws2_32")
	#endif
#endif
	#pragma comment(lib, "version")

#include "sysinfo.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[]=__FILE__;
	#ifdef DEBUG_NEW
		#define new DEBUG_NEW
	#endif
#endif

struct
{
	int iLevel;
	LPWSTR szName;
} CPU_PPC[]=
{
	{ 1, L"PowerPC 601"},
	{ 2, L"PowerPC 60x L2"},
	{ 3, L"PowerPC 603"},
	{ 4, L"PowerPC 604"},
	{ 5, L"PowerPC 605"},
	{ 6, L"PowerPC 603+"},
	{ 7, L"PowerPC 60x L7"},
	{ 8, L"PowerPC 60x L8"},
	{ 9, L"PowerPC 604+"},
	{10, L"PowerPC 60x L10"},
	{11, L"PowerPC 60x L11"},
	{12, L"PowerPC 60x L12"},
	{13, L"PowerPC 60x L13"},
	{14, L"PowerPC 60x L14"},
	{15, L"PowerPC 60x L15"},
	{16, L"PowerPC 60x L16"},
	{17, L"PowerPC 60x L17"},
	{18, L"PowerPC 60x L18"},
	{19, L"PowerPC 60x L19"},
	{20, L"PowerPC 620"},
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSystemInformation::CSystemInformation()
{
	m_fIERead = false;
	m_nIEMajor = 0;
	m_nIEMinor = 0;
	m_nIEBuild = 0;

	m_fComCtlRead = false;
	m_nComCtlMajor = 0;
	m_nComCtlMinor = 0;
	m_nComCtlBuild = 0;

	m_fExtendedInfo = false;
#ifdef CSI_WINSOCK
	m_fWinsockRead = false;
	m_wWinsock = MAKEWORD(0,0);
#endif

	::ZeroMemory(&m_osvi, sizeof(OSVERSIONINFOEX));
	m_osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	#pragma warning(disable : 4996)
	m_fExtendedInfo = ( 0 != ::GetVersionEx((OSVERSIONINFO*)&m_osvi));
	if (!m_fExtendedInfo)
	{
		m_osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		::GetVersionEx((OSVERSIONINFO*)&m_osvi);
	}
	#pragma warning(3 : 4996)
	::ZeroMemory(&m_si, sizeof(SYSTEM_INFO));
	GetSystemInfo(&m_si);
}

CSystemInformation::~CSystemInformation()
{

}

int CSystemInformation::GetIEVersionMajor()
{
	if (!m_fIERead)
	{
		ReadIEVersion();
	}

	return HIWORD(m_nIEMajor);
}

int CSystemInformation::GetIEVersionMinor()
{
	if (!m_fIERead)
	{
		ReadIEVersion();
	}

	return m_nIEMinor;
}

int CSystemInformation::GetIEVersionBuild()
{
	if (!m_fIERead)
	{
		ReadIEVersion();
	}

	return m_nIEBuild;
}

bool CSystemInformation::GetIEVersionString(LPWSTR pszBuf)
{
	if (NULL == pszBuf)
	{
		return false;
	}

	if (!m_fIERead)
	{
		if (!ReadIEVersion())
		{
			return false;
		}
	}

	LPCWSTR szVerFmt = L"%d%s%02d%s%04d";
	WCHAR szDec[2 * sizeof(WCHAR)];

	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDec, 2);
	if ( wsprintf(pszBuf, szVerFmt, m_nIEMajor, szDec, m_nIEMinor, szDec, m_nIEBuild) < lstrlen(szVerFmt) )
	{
		return false;
	}
	return true;
}

bool CSystemInformation::ReadIEVersion()
{
	LPCWSTR szIEPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE";

	HRESULT hr = 0;
	HKEY hKey = 0;
	LPVOID pVerInfoBlock = NULL;
	VS_FIXEDFILEINFO* pVerInfo = NULL;
	UINT cbVerInfo;
	DWORD cbVerInfoBlock = 0;
	DWORD dwUnused = 0;
	DWORD cbPathSize = 0;
	WCHAR szIELocalPath[MAX_PATH + 1] = L"";
	
	// get path to the IE executable
	hr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szIEPath,0, KEY_READ, &hKey);
	if (hr != ERROR_SUCCESS)
	{
		return false;
	}
	
	cbPathSize = sizeof (szIELocalPath);
	hr = RegQueryValueEx(hKey, NULL, NULL, NULL, (LPBYTE) szIELocalPath, &cbPathSize);
	RegCloseKey( hKey );
	if (hr != ERROR_SUCCESS)
	{
		return false;
	}
	
	// now go through the convoluted process of digging up the version info
	cbVerInfoBlock = GetFileVersionInfoSize( szIELocalPath, &dwUnused );
	if ( 0 == cbVerInfoBlock )
	{
		return false;
	}
	
	pVerInfoBlock = GlobalAlloc( GPTR, cbVerInfoBlock );
	if ( NULL == pVerInfoBlock )
	{
		return false;
	}
	
	if ( !GetFileVersionInfo( szIELocalPath, NULL, cbVerInfoBlock, pVerInfoBlock ) )
	{
		GlobalFree( pVerInfoBlock );
		return false;
	}
	
	if( !VerQueryValue(pVerInfoBlock, L"\\", (void **)&pVerInfo, &cbVerInfo) )
	{
		GlobalFree( pVerInfoBlock );
		return false;
	}
	
	m_nIEMajor = HIWORD(pVerInfo->dwProductVersionMS);
	m_nIEMinor = LOWORD(pVerInfo->dwProductVersionMS);
	m_nIEBuild = HIWORD(pVerInfo->dwProductVersionLS);
	m_fIERead = true;
	
	GlobalFree( pVerInfoBlock );
	
	return true;
}

int CSystemInformation::GetComCtlMajorVersion()
{
	if (!m_fComCtlRead)
	{
		ReadComCtlVersion();
	}

	return m_nComCtlMajor;
}

int CSystemInformation::GetComCtlMinorVersion()
{
	if (!m_fComCtlRead)
	{
		ReadComCtlVersion();
	}

	return m_nComCtlMinor;
}

int CSystemInformation::GetComCtlBuildVersion()
{
	if (!m_fComCtlRead)
	{
		ReadComCtlVersion();
	}

	return m_nComCtlBuild;
}

bool CSystemInformation::ReadComCtlVersion()
{
	LPVOID pVerInfoBlock = NULL;
	VS_FIXEDFILEINFO* pVerInfo = NULL;
	UINT cbVerInfo;
	DWORD cbVerInfoBlock = 0;
	DWORD dwUnused = 0;
	WCHAR szSystemPath[(MAX_PATH + 1) * sizeof(WCHAR)] = L"";
	
	if (0 == ::GetSystemDirectory(szSystemPath, MAX_PATH))
	{
		return false;
	}

	lstrcat(szSystemPath, L"\\comctl32.dll");
	
	// now go through the convoluted process of digging up the version info
	cbVerInfoBlock = GetFileVersionInfoSize( szSystemPath, &dwUnused );
	if ( 0 == cbVerInfoBlock )
	{
		return false;
	}
	
	pVerInfoBlock = GlobalAlloc( GPTR, cbVerInfoBlock );
	if ( NULL == pVerInfoBlock )
	{
		return false;
	}
	
	if ( !GetFileVersionInfo( szSystemPath, NULL, cbVerInfoBlock, pVerInfoBlock ) )
	{
		GlobalFree( pVerInfoBlock );
		return false;
	}
	
	if( !VerQueryValue(pVerInfoBlock, L"\\", (void **)&pVerInfo, &cbVerInfo) )
	{
		GlobalFree( pVerInfoBlock );
		return false;
	}
	
	m_nComCtlMajor = HIWORD(pVerInfo->dwProductVersionMS);
	m_nComCtlMinor = LOWORD(pVerInfo->dwProductVersionMS);
	m_nComCtlBuild = HIWORD(pVerInfo->dwProductVersionLS);
	m_fComCtlRead = true;
	
	GlobalFree( pVerInfoBlock );
	
	return true;
}

bool CSystemInformation::GetComCtlVersionString(LPWSTR pszBuf)
{
	if (NULL == pszBuf)
	{
		return false;
	}

	if (!m_fComCtlRead)
	{
		if (!ReadComCtlVersion())
		{
			return false;
		}
	}

	LPCWSTR szVerFmt = L"%d%s%02d%s%04d";
	WCHAR szDec[2 * sizeof(WCHAR)];

	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDec, 2);
	if ( wsprintf(pszBuf, szVerFmt, m_nComCtlMajor, szDec, m_nComCtlMinor, szDec, m_nComCtlBuild) < lstrlen(szVerFmt) )
	{
		return false;
	}
	return true;
}

bool CSystemInformation::GetOSVersionString(LPWSTR pszBuf)
{
	if (NULL == pszBuf)
	{
		return false;
	}

	LPCWSTR szVerFmt1 = L"%s %d%s%02d%s%04d %s";
	LPCWSTR szVerFmt2 = L"%s\r\nBuild %d%s%02d%s%04d %s";
	// LPCWSTR szVerFmt3 = L"%s\r\nBuild %d%s%02d%s%04d %s, Service Pack %d%s%d";
	WCHAR szDec[2];

	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDec, 2);

	WCHAR szOS[256];
	if (IsWinNT())
	{
		switch (m_osvi.dwMajorVersion)
		{
			case 1:
			case 2:
			case 3:
			case 4:
				if (IsServer())
				{
					wsprintf(szOS, L"Microsoft Windows NT Server");
				}
				else
				{
					wsprintf(szOS, L"Microsoft Windows NT Workstation");
				}
				break;

			case 5:
				if (m_osvi.dwMinorVersion == 0)
				{
					if (IsServer())
					{
						wsprintf(szOS, L"Microsoft Windows 2000 Server");
					}
					else
					{
						wsprintf(szOS, L"Microsoft Windows 2000 Professional");
					}
				}
				else
				{
					if (IsServer())
					{
						wsprintf(szOS, L"Microsoft Windows XP Server");
					}
					else
					{
						wsprintf(szOS, L"Microsoft Windows XP Professional");
					}
				}
				break;

			default:
				if (IsServer())
				{
					wsprintf(szOS, L"Microsoft Windows XP Server");
				}
				else
				{
					wsprintf(szOS, L"Microsoft Windows XP Professional");
				}
				break;
		}
	}

	else
	{
		if(GetOSVersionMinor()==0)
		{
			wsprintf(szOS, L"Micrsoft Windows 95");
		}
		else if(GetOSVersionMinor()==10)
		{
			wsprintf(szOS, L"Microsoft Windows 98");
		}
		else
		{
			wsprintf(szOS, L"Microsoft Windows 9x");
		}
	}

	// Win98 2nd Edition
	if ( !IsWinNT() &&
		 GetOSVersionMajor() == 4 &&
		 GetOSVersionMinor() == 10 &&
		 GetOSVersionBuild() == 2222 &&
		 0==lstrcmpi(m_osvi.szCSDVersion, L" A "))
	{
			lstrcpy(m_osvi.szCSDVersion, L"A");
			wsprintf(szOS, L"Microsoft Windows 98 Second Edition");
	}

	// WinME Edition
	if ( !IsWinNT() &&
		 GetOSVersionMajor() == 4 &&
		 GetOSVersionMinor() == 90 )
	{
			wsprintf(szOS, L"Microsoft Windows Millenium");
	}

	if (!m_fExtendedInfo)
	{
		wsprintf(pszBuf, szVerFmt1, szOS, GetOSVersionMajor(),
			szDec, GetOSVersionMinor(), szDec, GetOSVersionBuild(),
			m_osvi.szCSDVersion);
	}
	else
	{
			wsprintf(pszBuf, szVerFmt2, szOS, GetOSVersionMajor(),
				szDec, GetOSVersionMinor(), szDec, GetOSVersionBuild(),
				m_osvi.szCSDVersion);
#if 0
			if (GetServicePackMajor() == 0 && GetServicePackMinor() == 0)
		{
			wsprintf(pszBuf, szVerFmt2, szOS, GetOSVersionMajor(),
				szDec, GetOSVersionMinor(), szDec, GetOSVersionBuild(),
				m_osvi.szCSDVersion);
		}
		else
		{
			wsprintf(pszBuf, szVerFmt3, szOS, GetOSVersionMajor(),
				szDec, GetOSVersionMinor(), szDec, GetOSVersionBuild(),
				m_osvi.szCSDVersion, m_osvi.wServicePackMajor, szDec, m_osvi.wServicePackMinor);
		}
#endif
	}

	return true;
}

bool CSystemInformation::IsServer()
{
/*
   WINNT      Windows NT Workstation is running.
   SERVERNT   Windows NT Server (3.5 or later) is running.
   LANMANNT   Windows NT Advanced Server (3.1) is running.
*/
	LPCWSTR szKey = L"SYSTEM\\CurrentControlSet\\Control\\ProductOptions";

	HRESULT hr = 0;
	HKEY hKey = 0;
	DWORD cbValue = 0;
	WCHAR szValue[MAX_PATH + 1] = L"";
	
	// get path to the IE executable
	hr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKey,0, KEY_READ, &hKey);
	if (hr != ERROR_SUCCESS)
	{
		return false;
	}
	
	cbValue = sizeof (szValue);
	hr = RegQueryValueEx(hKey, NULL, NULL, NULL, (LPBYTE) szValue, &cbValue);
	RegCloseKey( hKey );
	if (hr != ERROR_SUCCESS)
	{
		return false;
	}
	return (0 != lstrcmpi(L"WINNT", szValue));
}

bool CSystemInformation::GetKeyboardLayout(LPWSTR pszBuf)
{
	if (NULL == pszBuf)
	{
		return false;
	}

	// Get the keyboard variant
	WCHAR szVariant[KL_NAMELENGTH * sizeof(WCHAR)];
	if (!::GetKeyboardLayoutName(szVariant))
		return false;

	LPWSTR pch = szVariant;
	int i;
	for (i=0; i < 4; i++)
	{
		pch = CharNext(pch);
	}
	*pch = '\0';

#if 0
	WCHAR szKey[256] = L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts\\\0";
	DWORD dwType = REG_SZ;
  
	lstrcat(szKey, szVariant);
	szVariant[4] = '\0';
  
	HKEY  hKey;
	LONG  lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKey, 0, KEY_READ, &hKey);
  
	if (lRet != ERROR_SUCCESS)
	{
		return false;
	}
  
	lRet = RegQueryValueEx(hKey, L"Layout Text", NULL, &dwType, (LPBYTE)szKey, &dw);
  
	if(lRet != ERROR_SUCCESS)
	{
		return false;
	}

#endif
	// Get the keyboard locale ID
	HKL hkl = ::GetKeyboardLayout(0);
	WORD wLang = LOWORD(hkl);
	LCID lcid = MAKELCID(wLang, SORT_DEFAULT);

	// get the keyboard language name
	LPWSTR pszLang = NULL;
	int cbLang  = ::GetLocaleInfo(lcid, LOCALE_SLANGUAGE, pszLang, 0);
	pszLang = new WCHAR[cbLang];
	if ( 0 == ::GetLocaleInfo(::GetThreadLocale(), LOCALE_SLANGUAGE, pszLang, cbLang))
	{
		delete [] pszLang;
		return false;
	}

//	wsprintf(pszBuf, L"%s (%s, variant %s)", pszLang, szVariant);
	wsprintf(pszBuf, L"%s, %04x variant %s", pszLang, wLang, szVariant);
	delete [] pszLang;
	return true;
}

HKL CSystemInformation::GetKeyboardLayout()
{
	return ::GetKeyboardLayout(0);
}

bool CSystemInformation::GetLanguage(LPWSTR pszBuf, int cchBuf)
{
	if (NULL == pszBuf || cchBuf < 1)
		return false;

	return (0 != ::GetLocaleInfo(::GetThreadLocale(), LOCALE_SLANGUAGE, pszBuf, cchBuf));
}


bool CSystemInformation::GetCountry(LPWSTR pszBuf, int cchBuf)
{
	if (NULL == pszBuf || cchBuf < 1)
		return false;

	return (0 != ::GetLocaleInfo(::GetThreadLocale(), LOCALE_SCOUNTRY, pszBuf, cchBuf));
}

LCID CSystemInformation::GetLocale()
{
	return ::GetThreadLocale();
}

DWORD CSystemInformation::GetOSLanguage()
{
	WCHAR szUser[MAX_PATH * sizeof(WCHAR)];
	if (0 == ::GetWindowsDirectory(szUser, MAX_PATH))
	{
		return 0;
	}

	lstrcat(szUser, L"\\user.exe");


	UINT cbBuf = ::GetFileVersionInfoSize(szUser, 0);
	if (cbBuf == 0)
	{
		return 0;
	}

	LPBYTE pBuf= new BYTE[cbBuf];
	if (NULL == pBuf)
	{
		return 0;
	}

	if (! ::GetFileVersionInfo(szUser, 0, cbBuf, pBuf))
	{
		delete [] pBuf;
		return 0;
	}

	LPVOID pVerData;
	UINT cbVerData;
	if (!VerQueryValue(pBuf, L"\\VarFileInfo\\Translation", &pVerData, &cbVerData))
	{
		delete [] pBuf;
		return 0;
	}

	DWORD dw = *(LPDWORD)pVerData;
	delete [] pBuf;
	return dw;
}

bool CSystemInformation::GetIEShortPath(LPWSTR pszBuf, DWORD cchBuf)
{
	LPCWSTR szIEPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE";

	HRESULT hr = 0;
	HKEY hKey = 0;
	//DWORD cbPathSize = 0;
	//WCHAR szIELocalPath[MAX_PATH + 1] = L"";
	
	// get path to the IE executable
	hr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szIEPath,0 , KEY_READ, &hKey);
	if (hr != ERROR_SUCCESS)
	{
		return false;
	}
	
	//cbPathSize = sizeof (szIELocalPath);
	hr = RegQueryValueEx(hKey, NULL, NULL, NULL, (LPBYTE) pszBuf, &cchBuf);
	RegCloseKey( hKey );
	if (hr != ERROR_SUCCESS)
	{
		return false;
	}

	return true;
}

bool CSystemInformation::GetIELongPath(LPWSTR pszBuf, DWORD cchBuf)
{
	WCHAR szBuf[MAX_PATH];
	GetIEShortPath(szBuf, MAX_PATH);

	if (0 == PathGetLongName(szBuf, pszBuf))
		return false;

	cchBuf;
	return true;
}

#ifdef CSI_WINSOCK
bool CSystemInformation::ReadWinsockVersion()
{
	WSADATA  wsaData;
	::ZeroMemory(&wsaData, sizeof(wsaData));

	// Make sure version 1.0 is installed
	m_wWinsock = MAKEWORD( 1, 0 );
	if (0 != WSAStartup( m_wWinsock, &wsaData ))
	{
		WSACleanup();
		ALERT(true, L"Winsock 1.0 not found";
		m_wWinsock = MAKEWORD( 0, 0);
		m_fWinsockRead = true;
		return false;
	}

	WSACleanup();
	m_wWinsock = wsaData.wHighVersion;
	m_fWinsockRead = true;
	return true;
}

bool CSystemInformation::GetWinsockVersionString(LPWSTR pszBuf, int cchBuf)
{
	if (!m_fWinsockRead)
	{
		ReadWinsockVersion();
	}

	cchBuf;
	LPCWSTR szVerFmt = L"Windows Sockets %d%s%02d";
	WCHAR szDec[2 * sizeof(WCHAR)];

	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDec, 2);
	if ( wsprintf(pszBuf, szVerFmt, LOBYTE(m_wWinsock), szDec, HIBYTE(m_wWinsock)) < lstrlen(szVerFmt) )
	{
		return false;
	}
	return true;
}

bool CSystemInformation::GetWinsockStatus(LPWSTR pszBuf, int cchBuf)
{
	cchBuf;
	WSADATA  wsaData;
	::ZeroMemory(&wsaData, sizeof(wsaData));

	// Make sure version 1.0 is installed
	m_wWinsock = MAKEWORD( 1, 0 );
	if (0 != WSAStartup( m_wWinsock, &wsaData ))
	{
		WSACleanup();
		return false;
	}

	WSACleanup();
	lstrcpy(pszBuf, wsaData.szSystemStatus);
	return true;
}

bool CSystemInformation::GetWinsockDescription(LPWSTR pszBuf, int cchBuf)
{
	cchBuf;
	WSADATA  wsaData;
	::ZeroMemory(&wsaData, sizeof(wsaData));

	// Make sure version 1.0 is installed
	m_wWinsock = MAKEWORD( 1, 0 );
	if (0 != WSAStartup( m_wWinsock, &wsaData ))
	{
		WSACleanup();
		return false;
	}

	WSACleanup();
	lstrcpy(pszBuf, wsaData.szDescription);
	return true;
}
#endif

bool CSystemInformation::IsAlpha()
{
	return (PROCESSOR_ARCHITECTURE_ALPHA == m_si.wProcessorArchitecture);
}

bool CSystemInformation::IsMIPS()
{
	return (PROCESSOR_ARCHITECTURE_MIPS == m_si.wProcessorArchitecture);
}

bool CSystemInformation::IsPPC()
{
	return (PROCESSOR_ARCHITECTURE_PPC == m_si.wProcessorArchitecture);
}

bool CSystemInformation::IsIntel()
{
	return (PROCESSOR_ARCHITECTURE_INTEL == m_si.wProcessorArchitecture);
}

bool CSystemInformation::GetCPUDescription(LPWSTR psz)
{
	if (IsAlpha())
	{
		wsprintf(psz, L"Alpha %lu Model %c Pass %lu", m_si.wProcessorLevel,
			'A' + HIBYTE(m_si.wProcessorRevision),
			LOBYTE(m_si.wProcessorRevision));
		return true;
	}

	if (IsIntel())
	{
		if (m_si.wProcessorLevel < 5)
		{
			if (0xFF == HIBYTE(m_si.wProcessorRevision))
			{
				wsprintf(psz, L"Intel 80%lu86 Model %lu, Stepping %lu",
					m_si.wProcessorLevel,
					HIBYTE(m_si.wProcessorRevision) - 0x0A,
					LOBYTE(m_si.wProcessorRevision));
			}
			else
			{
				wsprintf(psz, L"Intel 80%lu86 Stepping %c.%02x",
					m_si.wProcessorLevel,
					HIBYTE(m_si.wProcessorRevision) + 'A',
					LOBYTE(m_si.wProcessorRevision));
			}
		}
		else
		{
			wsprintf(psz, L"Intel Pentium Class %lu Model %lu, Stepping %lu",
				m_si.wProcessorLevel - 4,
				HIBYTE(m_si.wProcessorRevision),
				LOBYTE(m_si.wProcessorRevision));
		}
		return true;
	}
			

	if (IsMIPS())
	{
		wsprintf(psz, L"MIPS R4000 Revision %lu", m_si.wProcessorRevision);
		return true;
	}

	if (IsPPC())
	{
		wsprintf(psz, L"%s %lu.%lu", CPU_PPC[m_si.wProcessorLevel],
			HIBYTE(m_si.wProcessorRevision),
			LOBYTE(m_si.wProcessorRevision));
		return true;
	}

	//unknown
	wsprintf(psz, L"Unknown Processor");
	return true;
}

int CSystemInformation::GetCPUCount()
{
	return m_si.dwNumberOfProcessors;
}

bool CSystemInformation::SupportsMultiMon()
{
	return (::GetSystemMetrics(SM_CMONITORS) > 0);
}

int CSystemInformation::GetMonitorCount()
{
	return ::GetSystemMetrics(SM_CMONITORS);
}
