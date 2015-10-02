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

// ProgressBox.cpp : implementation file
//

#include "stdafx.h"
#include "wndlib.h"
#include "resource.h"
#include "VFI.h"
#include "ProgressBox.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static CHAR THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressBox dialog


CProgressBox::CProgressBox(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressBox)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_wAlign = MWX_APP | MWX_CENTER;
}


void CProgressBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressBox)
	DDX_Control(pDX, IDC_PROGRESS1, m_ctlProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressBox, CDialog)
	//{{AFX_MSG_MAP(CProgressBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressBox message handlers

BOOL CProgressBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (!MoveWindowEx(m_hWnd, m_wAlign))
		return FALSE;

	return TRUE;
}

BOOL CProgressBox::Create(CWnd* pParentWnd, WORD wAlign)
{
	pParentWnd;
	m_wAlign = wAlign;
	//return CDialog::Create(IDD_PROGRESS, pParentWnd);
	return CDialog::Create(IDD_PROGRESS, AfxGetMainWnd());
}
