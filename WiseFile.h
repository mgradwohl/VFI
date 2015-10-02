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

#ifndef WISEFILE_H
#pragma once
#ifdef _DEBUG
	#ifdef DEBUG_NEW
		#define new DEBUG_NEW
	#endif
#endif

#include "wndlib.h"
#include "strlib.h"
#include "filelib.h"

// forwards
bool GetLanguageName(UINT Locale, LPWSTR pszBuf);

//class CCodePage;
#include "tcodepage.h"

#include "isocheck.h"

// file states
enum FileStates
{
	FWFS_VALID				=0x0000,	// constructed only
	FWFS_ATTACHED			=0x0001,	// attached to a file
	FWFS_VERSION			=0x0002,	// version information
	FWFS_CRC_PENDING		=0x0004,	// unused
	FWFS_CRC_WORKING		=0x0008,	// working on CRC
	FWFS_CRC_COMPLETE		=0x0010,	// CRC complete
	FWFS_CRC_ERROR			=0x0020,	// error generating CRC
	FWFS_DELETE				=0x0040,	// pending deletion
	FWFS_INVALID			=0x0080	// invalid state
};

#define	FWF_SUCCESS				0
#define	FWF_ERR_NONE			0
#define	FWF_ERR_INVALID			-1;
#define FWF_ERR_BADPARAM		-2;
#define FWF_ERR_FOLDER			-3;
#define FWF_ERR_WILDCARD		-4;
#define FWF_ERR_SPECIALPATH		-5;
#define FWF_ERR_NOVERSION		-6
#define FWF_ERR_LOWMEMORY		-7;
#define FWF_ERR_ISO				-8;
#define FWF_ERR_OTHER			-64;

class CWiseFile : public CObject
{
	DECLARE_DYNAMIC( CWiseFile )

public:
	int ReadVersionInfoEx();
	bool GetShortPath(LPWSTR pszBuf, int cchBuf);
	bool GetFieldString(LPWSTR pszBuf, int iField, bool fOptions);
	// construction, destruction
	CWiseFile();
	CWiseFile(const CWiseFile& rwf);
	CWiseFile(LPCWSTR pszFileSpec);
	~CWiseFile();

	// initialization, release
	int Attach();
	int Attach(LPCWSTR pszFileName);
	int Detach();

	const CWiseFile& Copy(const CWiseFile& rwf);
	const CWiseFile& operator=(const CWiseFile& rwf);
	bool operator==(const CWiseFile& rwf);

	// access members
	int GetFullPath( LPWSTR pszText )
		{ return ( (NULL == lstrcpy( pszText, m_szFullPath )) ? lstrcb(m_szFullPath) : FWF_SUCCESS ); }
	LPWSTR GetFullPath()
		{ return m_szFullPath; }

	int GetPath( LPWSTR pszText )
		{ return ( (NULL == lstrcpy( pszText, m_szPath )) ? lstrcb(m_szPath) : FWF_SUCCESS ); }
	LPWSTR GetPath()
		{ return m_szPath; }

 	int GetName( LPWSTR pszText )
		{ return ( (NULL == lstrcpy( pszText, m_szName )) ? lstrcb(m_szName) : FWF_SUCCESS ); }
	LPWSTR GetName()
		{ return m_szName; }

	int GetExt( LPWSTR pszText )
		{ return ( (NULL == lstrcpy( pszText, m_szExt )) ? lstrcb(m_szExt) : FWF_SUCCESS ); }
	LPWSTR GetExt()
		{ return m_szExt; }

	int GetShortName( LPWSTR pszText )
		{ return ( (NULL == lstrcpy( pszText, m_szShortName )) ? lstrcb(m_szShortName) : FWF_SUCCESS ); }
	LPWSTR GetShortName()
		{ return m_szShortName; }

	int GetSize( LPWSTR pszText, bool bHex=false );
	DWORD GetSize()
		{ return LODWORD( m_qwSize ); }

	int GetSize64( LPWSTR pszText, bool bHex=false );
	QWORD GetSize64()
		{ return m_qwSize; }

	int GetAttribs( LPWSTR pszText );
	DWORD GetAttribs()
		{ return m_dwAttribs; }

	int GetDateCreation( LPWSTR pszText, bool fLocal=true );
	LPSYSTEMTIME GetDateCreation( bool fLocal=true );
	int GetDateLastAccess( LPWSTR pszText, bool fLocal=true );
	LPSYSTEMTIME GetDateLastAccess( bool fLocal=true );
	int GetDateLastWrite( LPWSTR pszText, bool fLocal=true );
	LPSYSTEMTIME GetDateLastWrite( bool fLocal=true );

	int GetTimeCreation( LPWSTR pszText, bool fLocal=true );
	LPSYSTEMTIME GetTimeCreation( bool fLocal=true );
	int GetTimeLastAccess( LPWSTR pszText, bool fLocal=true );
	LPSYSTEMTIME GetTimeLastAccess( bool fLocal=true );
	int GetTimeLastWrite( LPWSTR pszText, bool fLocal=true );
	LPSYSTEMTIME GetTimeLastWrite( bool fLocal=true );

	int GetFileVersion( LPWSTR pszText );
	QWORD GetFileVersion()
		{ return m_qwFileVersion; }
	int GetFileVersion( LPDWORD pdwHigh, LPDWORD pdwLow );
	int GetFileVersion(LPWORD pwHighMS, LPWORD pwLowMS, LPWORD pwHighLS, LPWORD pwLowLS);

	int GetProductVersion( LPWSTR pszText );
	QWORD GetProductVersion()
		{ return m_qwProductVersion; }
	int GetProductVersion(LPDWORD pdwMS, LPDWORD pdwLS);
	int GetProductVersion(LPWORD pwHighMS, LPWORD pwLowMS, LPWORD pwHighLS, LPWORD pwLowLS);

	int GetLanguage( LPWSTR pszText, bool bNumeric=false );
	WORD GetLanguage()
		{ return m_wLanguage; }

	int GetCodePage( LPWSTR pszText, bool bNumeric=false );
	const TCodePage& GetCodePage()
		{ return m_CodePage; }

	int GetFullLanguage( LPWSTR pszText, bool bNumeric=false );

	int GetOSString( LPWSTR pszText );
	void GetOSString();
	DWORD GetOS()
		{ return m_dwOS; }

	int GetTypeString( LPWSTR pszText );
	void GetTypeString();
	DWORD GetType()
		{ return m_dwType; }

	int GetFlags( LPWSTR pszText );
	DWORD GetFlags()
		{ return m_dwFlags; }

	int GetCRC( LPWSTR pszText, bool bHex=true );
	DWORD GetCRC()
		{ return m_dwCRC; }

	LPWSTR GetISO(bool fIncludePath);
	int GetISO( LPWSTR pszText, bool fIncludePath );
	WORD GetISO()
		{ return m_wISOFlags; }

	void SetState( WORD wState )
		{ m_wFileState = wState; }

	void OrState( WORD wState )
		{ m_wFileState |= wState; }

	WORD GetState()
		{ return m_wFileState; }
	
	bool CheckState( WORD wCheck )
		{ return (0 != (m_wFileState & wCheck)); }

	int TouchFileTime( FILETIME* lpTime );

	void _SetCRC(DWORD dwCRC)
		{ m_dwCRC = dwCRC;}
	// execute helpers
	int ReadVersionInfo();
	int CheckISO();

#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	bool Init();
	// Member Variables
	// Set when a file is added
	WCHAR		m_szFullPath[MAX_PATH];
	WCHAR		m_szPath[MAX_PATH];
	WCHAR		m_szName[MAX_PATH];
	WCHAR		m_szExt[MAX_PATH];
	WCHAR		m_szShortName[14];

	// Set when information requested, if not set
	WCHAR		m_szAttribs[10];
	WCHAR		m_szFlags[256];
	WCHAR		m_szOS[64];
	WCHAR		m_szType[64];

	// from WIN32_FIND_DATA
	DWORD		m_dwAttribs;
	SYSTEMTIME	m_stUTCCreation;
	SYSTEMTIME	m_stUTCLastAccess;
	SYSTEMTIME	m_stUTCLastWrite;

	// set when requested, if not set
	SYSTEMTIME	m_stLocalCreation;
	SYSTEMTIME	m_stLocalLastAccess;
	SYSTEMTIME	m_stLocalLastWrite;

	QWORD		m_qwSize;
	QWORD		m_qwFileVersion;
	QWORD		m_qwProductVersion;
	WORD		m_wLanguage;
	TCodePage	m_CodePage;
	DWORD		m_dwOS;
	DWORD		m_dwType;
	DWORD		m_dwFlags;

	DWORD		m_dwCRC;
	WORD		m_wISOFlags;
	WORD		m_wFileState;
	
	// Flags
	bool m_fDebugStripped;
	bool m_fHasVersion;
	bool m_fszOS;
	bool m_fszType;
	bool m_fszFlags;
	bool m_fszAttribs;
};

#define WISEFILE_H
#endif//WISEFILE_H
