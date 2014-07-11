// MyFileBox.cpp : implementation file
//

#include "stdafx.h"
#include "wndlib.h"
#include "MyFileBox.h"

#ifdef _DEBUG
	#ifdef DEBUG_NEW
		#define new DEBUG_NEW
	#endif
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyFileBox

IMPLEMENT_DYNAMIC(CMyFileBox, CFileDialog)

CMyFileBox::CMyFileBox(BOOL bOpenFileDialog, LPCWSTR lpszDefExt, LPCWSTR lpszFileName,
		DWORD dwFlags, LPCWSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	m_hFont = NULL;
}


BEGIN_MESSAGE_MAP(CMyFileBox, CFileDialog)
	//{{AFX_MSG_MAP(CMyFileBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

bool CMyFileBox::SetFont(HFONT hFont)
{
	if (NULL != hFont)
	{
		m_hFont = hFont;
		return true;
	}
	return false;
}

BOOL CMyFileBox::OnInitDialog() 
{
	CFileDialog::OnInitDialog();
	
	//if (NULL != m_hFont)
	//	ChangeDialogFont(GetParent()->m_hWnd, m_hFont, CDF_CENTER);	
	return TRUE;
}
