// SystemInformation.h: interface for the CSystemInformation class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#ifndef SYSINFO_H

#ifdef _DEBUG
	#ifdef DEBUG_NEW
		#define new DEBUG_NEW
	#endif
#endif

#include <windows.h>
#include <winuser.h>

//#include "wndlib.h"
#include "filelib.h"

#ifndef SM_CMONITORS
	#define SM_CMONITORS            80
#endif

class CSystemInformation  
{
public:
	int GetCPUCount();
	bool GetCPUDescription(LPWSTR psz);
	bool IsIntel();
	bool IsPPC();
	bool IsMIPS();
	bool IsAlpha();
	bool GetKeyboardVariant(WORD wVariant, LPWSTR pszBuf);
#ifdef CSI_WINSOCK
	bool GetWinsockDescription(LPWSTR pszBuf, int cchBuf);
	bool GetWinsockStatus(LPWSTR pszBuf, int cchBuf);
	bool GetWinsockVersionString(LPWSTR pszBuf, int cchBuf);
#endif
	bool GetIEShortPath(LPWSTR pszBuf, DWORD cchBuf);
	bool GetIELongPath(LPWSTR pszBuf, DWORD cchBuf);
	DWORD GetOSLanguage();
	LCID GetLocale();
	bool GetCountry(LPWSTR pszBuf, int cchBuf);
	bool GetLanguage(LPWSTR pszBuf, int cchBuf);
	HKL GetKeyboardLayout();
	bool GetKeyboardLayout(LPWSTR pszBuf);
	bool IsServer();
	CSystemInformation();
	virtual ~CSystemInformation();

	bool CSystemInformation::IsWinNT()
	{
		return (m_osvi.dwPlatformId == VER_PLATFORM_WIN32_NT);
	}

	bool CSystemInformation::IsWin9x()
	{
		return (m_osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS);
	}

	int GetComCtlBuildVersion();
	int GetComCtlMinorVersion();
	int GetComCtlMajorVersion();
	bool GetComCtlVersionString(LPWSTR pszBuf);

	int GetIEVersionBuild();
	int GetIEVersionMinor();
	int GetIEVersionMajor();
	bool GetIEVersionString(LPWSTR pszBuf);


	int CSystemInformation::GetOSVersionMajor()
	{
		return m_osvi.dwMajorVersion;
	}
	int CSystemInformation::GetOSVersionMinor()
	{
		return m_osvi.dwMinorVersion;
	}
	int CSystemInformation::GetServicePackMinor()
	{
		return m_osvi.wServicePackMinor;
	}
	int CSystemInformation::GetServicePackMajor()
	{
		return m_osvi.wServicePackMajor;
	}
	int CSystemInformation::GetOSVersionBuild()
	{
		if (IsWinNT())
		{
			return m_osvi.dwBuildNumber;
		}
		else
		{
			return LOWORD(m_osvi.dwBuildNumber);
		}
	}
	bool GetOSVersionString(LPWSTR pszBuf);

	bool SupportsMultiMon();
	int GetMonitorCount();

private:
	SYSTEM_INFO m_si;
#ifdef CSI_WINSOCK
	bool m_fWinsockRead;
	WORD m_wWinsock;
	bool ReadWinsockVersion();
#endif
	OSVERSIONINFOEXW m_osvi;
	int m_nIEMajor;
	int m_nIEMinor;
	int m_nIEBuild;
	int m_nComCtlMajor;
	int m_nComCtlMinor;
	int m_nComCtlBuild;
	bool m_fExtendedInfo;
	bool m_fIERead;
	bool m_fComCtlRead;
	bool ReadIEVersion();
	bool ReadComCtlVersion();

};

#define SYSINFO_H
#endif//SYSINFO_H
