#ifndef GUID_H
#pragma once

#include <windows.h>
#include "strlib.h"
#include <objbase.h>

#ifdef _DEBUG
	#ifdef DEBUG_NEW
		#define new DEBUG_NEW
		#ifndef THIS_FILE
			static WCHAR THIS_FILE[] = __FILE__;
		#endif
	#endif
#endif

#ifndef TRACE
	#define TRACE(x) OutputDebugString(x)
#endif

class TGuid  
{
public:
	TGuid()
	{
		m_guid = GUID_NULL;
		::ZeroMemory(m_szGuid, 1024 * sizeof(WCHAR));
	}

	virtual ~TGuid()
	{
		::CoUninitialize();
	}

	bool Create()
	{
		if (::CoInitialize(NULL) != S_OK)
			return false;

		return (S_OK == ::CoCreateGuid(&m_guid));
	}

	void GetString(LPWSTR pszGuid)
	{
		if (m_szGuid[0] == NULL)
		{
			SetString();
		}

		lstrcpy(pszGuid, m_szGuid);
	}

	LPCWSTR GetString()
	{
		if (m_szGuid[0] == NULL)
		{
			SetString();
		}

		return m_szGuid;
	}

	GUID GetGuid()
	{
		return m_guid;
	}


private:
	GUID m_guid;
	WCHAR m_szGuid[1024 * sizeof(WCHAR)];

	void SetString()
	{
		WCHAR szFmt[104];
		lstrcpy(szFmt, (LPCWSTR) _T("{%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"));
		//static const WCHAR szFmt[] = "{%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}";
		wsprintf(m_szGuid, szFmt, 
			m_guid.Data1, m_guid.Data2, m_guid.Data3, 
			m_guid.Data4[0], m_guid.Data4[1], m_guid.Data4[2], m_guid.Data4[3],
			m_guid.Data4[4], m_guid.Data4[5], m_guid.Data4[6], m_guid.Data4[7]);
	}
};

#define GUID_H
#endif//GUID_H
