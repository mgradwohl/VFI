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
