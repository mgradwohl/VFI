#ifndef TCODEPAGE_H
#pragma once

#ifdef _DEBUG
	#ifdef DEBUG_NEW
		#define new DEBUG_NEW
	#endif
#endif

#include <windows.h>
#include <mlang.h>
#include "strlib.h"

class TCodePage  
{
public:
	TCodePage()
	{
		m_wCodePage = 0;
		m_pszCodePage = NULL;
		m_pML = NULL;

		CoInitialize(NULL);
		if (S_OK != CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, IID_IMultiLanguage2, (void**)&m_pML))
//		if (S_OK != CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, IID_IMultiLanguage, (void**)&m_pML))
		{
			m_pML = NULL;
		}
	}

	TCodePage(WORD wCodePage)
	{
		m_wCodePage = wCodePage;
		m_pszCodePage = NULL;
		CoUninitialize();
	}

	bool GetCodePageName(WORD wLanguage, UINT CodePage, LPWSTR pszBuf)
	{
		if (NULL == pszBuf)
			return false;
		
		if (m_pML)
		{
			ZeroMemory(&m_mcp, sizeof(MIMECPINFO));
			if (S_OK  == m_pML->GetCodePageInfo(CodePage, wLanguage, &m_mcp))
			{
				#if defined(UNICODE)
					wsprintf( pszBuf, _T("%s (%lu)"), m_mcp.wszDescription, CodePage);
				#else
					CHAR szOut[MAX_MIMECP_NAME];
					ZeroMemory(szOut, MAX_MIMECP_NAME);
					//WideCharToMultiByte(CP_ACP, 0, m_mcp.wszDescription, -1, szOut, 256, NULL, NULL);
					lw2mb(m_mcp.wszDescription, szOut, MAX_MIMECP_NAME);
					
					wsprintf( pszBuf, _T("%s (%lu)"), szOut, CodePage);
				#endif
				return true;
			}
		}

		CPINFOEX cp;
		ZeroMemory(&cp, sizeof(CPINFOEX));
		if (GetCPInfoEx(CodePage, 0, &cp))
		{
			wsprintf( pszBuf, _T("%s"), cp.CodePageName);
			return true;
		}
		
		switch (CodePage)
		{
			case	37:		wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - U.S./Canada"));
			break;

			case	709:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("Arabic (ASMO 449+, BCON V4)"));
			break;

			case	710:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("Arabic (Transparent Arabic)"));
			break;

			case	437:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("OEM - United States"));
			break;

			case	500:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - International"));
			break;

			case	708:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("Arabic - ASMO"));
			break;

			case	720:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("Arabic - Transparent ASMO"));
			break;

			case	737:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("OEM - Greek 437G"));
			break;

			case	775:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("OEM - Baltic"));
			break;

			case	850:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("OEM - Multilingual Latin I"));
			break;

			case	852:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("OEM - Latin II"));
			break;

			case	855:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("OEM - Cyrillic"));
			break;

			case	857:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("OEM - Turkish"));
			break;

			case	860:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("OEM - Portuguese"));
			break;

			case	861:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("OEM - Icelandic"));
			break;

			case	862:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("OEM - Hebrew"));
			break;

			case	863:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("OEM - Canadian French"));
			break;

			case	864:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("OEM - Arabic"));
			break;

			case	865:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("OEM - Nordic"));
			break;

			case	866:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("OEM - Russian"));
			break;

			case	869:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("OEM - Modern Greek"));
			break;

			case	870:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Multilingual/ROECE (Latin-2)"));
			break;

			case	874:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ANSI/OEM - Thai"));
			break;

			case	875:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Modern Greek"));
			break;

			case	932:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ANSI/OEM - Japanese Shift-JIS"));
			break;

			case	936:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ANSI/OEM - Simplified Chinese GBK"));
			break;

			case	949:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ANSI/OEM - Korean"));
			break;

			case	950:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ANSI/OEM - Traditional Chinese Big5"));
			break;

			case	1200:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("Unicode (BMP of ISO 10646)"));
			break;

			case	1026:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Turkish (Latin-5)"));
			break;

			case	1250:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ANSI - Central Europe"));
			break;

			case	1251:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ANSI - Cyrillic"));
			break;

			case	1252:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ANSI - Latin I"));
			break;

			case	1253:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ANSI - Greek"));
			break;

			case	1254:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ANSI - Turkish"));
			break;

			case	1255:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ANSI - Hebrew"));
			break;

			case	1256:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ANSI - Arabic"));
			break;

			case	1257:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ANSI - Baltic"));
			break;

			case	1258:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ANSI/OEM - Viet Nam"));
			break;

			case	1361:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("Korean - Johab"));
			break;

			case	10000:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("MAC - Roman"));
			break;

			case	10001:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("MAC - Japanese"));
			break;

			case	10002:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("MAC - Traditional Chinese Big5"));
			break;

			case	10003:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("MAC - Korean"));
			break;

			case	10004:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("MAC - Arabic"));
			break;

			case	10005:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("MAC - Hebrew"));
			break;

			case	10006:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("MAC - Greek I"));
			break;

			case	10007:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("MAC - Cyrillic"));
			break;

			case	10008:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("MAC - Simplified Chinese GB 2312"));
			break;

			case	10010:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("MAC - Romania"));
			break;

			case	10017:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("MAC - Ukraine"));
			break;

			case	10029:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("MAC - Latin II"));
			break;

			case	10079:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("MAC - Icelandic"));
			break;

			case	10081:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("MAC - Turkish"));
			break;

			case	10082:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("MAC - Croatia"));
			break;

			case	20866:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("Russian - KOI8"));
			break;

			case	29001:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("Europa 3"));
			break;

			case	65000:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("UTF-7"));
			break;

			case	65001:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("UTF-8"));
			break;

			case	20000:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("CNS - Taiwan"));
			break;

			case	20001:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("TCA - Taiwan"));
			break;

			case	20002:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("Eten - Taiwan"));
			break;

			case	20003:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM5550 - Taiwan"));
			break;

			case	20004:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("TeleText - Taiwan"));
			break;

			case	20005:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("Wang - Taiwan"));
			break;

			case	20105:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IA5 IRV International Alphabet No. 5"));
			break;

			case	20106:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IA5 German"));
			break;

			case	20107:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IA5 Swedish"));
			break;

			case	20108:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IA5 Norwegian"));
			break;

			case	20127:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("US-ASCII"));
			break;

			case	20261:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("T.61"));
			break;

			case	20269:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ISO 6937 Non-Spacing Accent"));
			break;

			case	20273:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Germany"));
			break;

			case	20277:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Denmark/Norway"));
			break;

			case	20278:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Finland/Sweden"));
			break;

			case	20280:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Italy"));
			break;

			case	20284:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Latin America/Spain"));
			break;

			case	20285:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - United Kingdom"));
			break;

			case	20290:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Japanese Katakana Extended"));
			break;

			case	20297:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - France"));
			break;

			case	20420:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Arabic"));
			break;

			case	20423:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Greek"));
			break;

			case	20424:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Hebrew"));
			break;

			case	20833:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Korean Extended"));
			break;

			case	20838:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Thai"));
			break;

			case	20871:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Icelandic"));
			break;

			case	20880:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Cyrillic,(Russian)"));
			break;

			case	20905:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Turkish"));
			break;

			case	21025:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Cyrillic (Serbian, Bulgarian)"));
			break;

			case	21027:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("Ext Alpha Lowercase"));
			break;

			case	28591:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ISO 8859-1 Latin I"));
			break;

			case	28592:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ISO 8859-2 Central Europe"));
			break;

			case	28593:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ISO 8859-3 Turkish"));
			break;

			case	28594:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ISO 8859-4 Baltic"));
			break;

			case	28595:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ISO 8859-5 Cyrillic"));
			break;

			case	28596:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ISO 8859-6 Arabic"));
			break;

			case	28597:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ISO 8859-7 Greek"));
			break;

			case	28598:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ISO 8859-8 Hebrew"));
			break;

			case	28599:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ISO 8859-9 Latin 5"));
			break;

			case	50220:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ISO-2022 Japanese with no halfwidth Katakana"));
			break;

			case	50221:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ISO-2022 Japanese with halfwidth Katakana"));
			break;

			case	50222:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ISO-2022 Japanese JIS X 0201-1989"));
			break;

			case	50225:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ISO-2022 Korean"));
			break;

			case	50227:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ISO-2022 Simplified Chinese"));
			break;

			case	50229:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("ISO-2022 Traditional Chinese"));
			break;

			case	52936:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("HZ-GB2312 Simplified Chinese"));
			break;

			case	51932:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("EUC-Japanese"));
			break;

			case	51949:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("EUC-Korean"));
			break;

			case	51936:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("EUC-Simplified Chinese"));
			break;

			case	51950:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("EUC-Traditional Chinese"));
			break;

			case	50930:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Japanese (Katakana) Extended and Japanese"));
			break;

			case	50931:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - US/Canada and Japanese"));
			break;

			case	50933:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Korean Extended and Korean"));
			break;

			case	50935:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Simplified Chinese"));
			break;

			case	50937:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - US/Canada and Traditional Chinese"));
			break;

			case	50939:	wsprintf(pszBuf,_T("%lu %s"),CodePage,_T("IBM EBCDIC - Japanese (Latin) Extended and Japanese"));
			break;

			default:	wsprintf( pszBuf, _T("%lu Unknown"), CodePage);
			break;
		}

		#ifdef DEBUG
				lstrcat(pszBuf, _T(" Lookup"));
		#endif//DEBUG
		return true;
	}

	DWORD GetStringLength()
	{
		return lstrcch(m_pszCodePage);
	}

#if 0
	LPCTSTR GetString()
	{
		if (NULL == m_pszCodePage)
		{
			WCHAR szBuf[1024];
			GetCodePageName(m_wCodePage, szBuf);
			m_pszCodePage = new WCHAR[lstrcb(szBuf) + 1];
			lstrcpy(m_pszCodePage, szBuf);
		}
		return m_pszCodePage;
	}
#endif

	virtual ~TCodePage()
	{
		m_wCodePage = 0;
		delete [] m_pszCodePage;
	}

	operator LPCTSTR()
	{
		//return GetString();
		return NULL;
	}

	operator WORD() const
	{
		return m_wCodePage;
	}

	void Set(WORD wCodePage)
	{
		m_wCodePage = wCodePage;
	}

	WORD Get(void) const
	{
		return m_wCodePage;
	}

	const TCodePage& operator=(const TCodePage& cp)
	{
		m_wCodePage = (WORD)cp;
		return *this;
	}

	const TCodePage& operator=(const WORD w)
	{
		m_wCodePage = w;
		return *this;
	}

	bool operator==(const TCodePage& cp)
	{
		return m_wCodePage == (WORD)cp;
	}

	bool operator!=(const TCodePage& cp)
	{
		return m_wCodePage != (WORD)cp;
	}

	bool operator==(const WORD w)
	{
		return m_wCodePage == w;
	}

	bool operator!=(const WORD w)
	{
		return m_wCodePage != w;
	}

private:
	IMultiLanguage2* m_pML;
	//IMultiLanguage* m_pML;

	MIMECPINFO m_mcp;
	WORD m_wCodePage;
	LPWSTR m_pszCodePage;
};

#define TCODEPAGE_H
#endif//TCODEPAGE_H