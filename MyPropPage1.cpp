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

// MyPropPage1.cpp : implementation file
//
#include "stdafx.h"
#include "VFI.h"
#include "resource.h"
#include "MyPropPage1.h"
//#include <../cpputil/myfilebox.h>

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static CHAR THIS_FILE[] = __FILE__;
#endif

#include "filelib.h"

IMPLEMENT_DYNCREATE(CPageGeneral, CPropertyPage)
IMPLEMENT_DYNCREATE(CPageColumn,  CPropertyPage)

// CPageGeneral property page

CPageGeneral::CPageGeneral() : CPropertyPage(CPageGeneral::IDD)
{
	//{{AFX_DATA_INIT(CPageGeneral)
	m_fAudioCue = false;
	m_strWave = "";
	m_fUseISORoot = false;
	m_fIncludePath = false;
	m_fSavePrompt = FALSE;
	m_strSavePath = "";
	//}}AFX_DATA_INIT
}

CPageGeneral::~CPageGeneral()
{
}

void CPageGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageGeneral)
	DDX_Control(pDX, IDC_BROWSEWAVE, m_btnBrowse);
	DDX_Check(pDX, IDC_AUDIOCUE, m_fAudioCue);
	DDX_Text(pDX, IDC_EDITWAVE, m_strWave);
	DDV_MaxChars(pDX, m_strWave, 260);
	DDX_Check(pDX, IDC_ISOINCLUDEPATH, m_fIncludePath);
	DDX_Check(pDX, IDC_SAVEPROMPT, m_fSavePrompt);
	DDX_Text(pDX, IDC_SAVEPATH, m_strSavePath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CPageGeneral)
	ON_BN_CLICKED(IDC_AUDIOCUE, OnAudiocue)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BROWSEWAVE, OnBrowseWave)
	ON_EN_CHANGE(IDC_EDITWAVE, OnChangeWave)
	ON_EN_KILLFOCUS(IDC_EDITWAVE, OnKillFocusEditWave)
	ON_EN_CHANGE(IDC_SAVEPATH, OnChangeSavepath)
	ON_EN_KILLFOCUS(IDC_SAVEPATH, OnKillfocusSavepath)
	ON_BN_CLICKED(IDC_BROWSEPATH, OnBrowsepath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////

///
// CPageColumn property page
//
CPageColumn::CPageColumn() : CPropertyPage(CPageColumn::IDD)
{
	m_fInitialized=FALSE;
	m_pci = NULL;
	//{{AFX_DATA_INIT(CPageColumn)
	//}}AFX_DATA_INIT
}

CPageColumn::~CPageColumn()
{
	m_pci = NULL;
}

void CPageColumn::DoDataExchange(CDataExchange* pDX)
{
	if (m_fInitialized == true)
	{
		for( int i=0; i < LIST_NUMCOLUMNS; i++ )
		{
			m_pci[i].SetVisible(m_ctlColumns.GetCheck( i ) ? true : false);
			TRACE(L">>> CPageColumn::DoDataExchange %d, %d\r\n",i,m_ctlColumns.GetCheck( i )==1 ? TRUE : FALSE);
		}
	}
	else
	{
		m_fInitialized = true;
	}

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageColumn)
	DDX_Control(pDX, IDC_LISTCOLUMNS, m_ctlColumns);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageColumn, CPropertyPage)
	//{{AFX_MSG_MAP(CPageColumn)
	ON_BN_CLICKED(IDC_CLEARALL, OnClearall)
	ON_BN_CLICKED(IDC_CHECKALL, OnCheckall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPageGeneral::OnAudiocue() 
{
	CButton* pCheck = static_cast<CButton*> (GetDlgItem(IDC_AUDIOCUE));
	ASSERT (pCheck);

	if (pCheck != nullptr)
	{
		m_fAudioCue = pCheck->GetCheck();
	}

	CButton* pBrowse = static_cast<CButton*> (GetDlgItem(IDC_BROWSEWAVE));
	ASSERT (pBrowse);
	pBrowse->EnableWindow( m_fAudioCue );

	CWnd* pEdit = static_cast<CWnd*> (GetDlgItem(IDC_EDITWAVE));
	ASSERT (pEdit);
	pEdit->EnableWindow( m_fAudioCue );
}

BOOL CPageGeneral::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	OnAudiocue();

	// return TRUE unless you set the focus to a control
	return TRUE;
}

CCheckListBox* CPageColumn::GetCheckListBox()
{
	return &m_ctlColumns;
}

bool CPageColumn::InitCheckList(CColumnInfo* pci)
{
	m_pci = pci;
	return true;
}

BOOL CPageColumn::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Initialize the Check List Box
	CString strLabel;
	for( int i=0; i < LIST_NUMCOLUMNS; i++ )
	{
		strLabel.LoadString( m_pci[i].GetLabelID() );
		strLabel = " " + strLabel;
		m_ctlColumns.AddString( strLabel );
		m_ctlColumns.SetCheck( i, m_pci[i].IsVisible() ? 1 : 0  );
		TRACE(L">>> CPageColumn::OnInitDialog %s, %d\r\n",strLabel, m_pci[i].IsVisible() ? 1 : 0);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

CColumnInfo* CPageColumn::GetColumnInfo()
{
	return m_pci;
}

INT_PTR CPageColumn::DoModal() 
{
	for( int i=0; i < LIST_NUMCOLUMNS; i++ )
	{
		m_pci[i].SetVisible(m_ctlColumns.GetCheck( i ) ? true : false);
		TRACE(L">>> CPageColumn::DoModal %d, %d\r\n",i,m_ctlColumns.GetCheck( i )==1 ? TRUE : FALSE);
	}
	return CPropertyPage::DoModal();
}

void CPageColumn::OnClearall() 
{
	for( int i=0; i < LIST_NUMCOLUMNS; i++ )
	{
		m_ctlColumns.SetCheck( i, 0 );
	}
}

void CPageColumn::OnCheckall() 
{
	for( int i=0; i < LIST_NUMCOLUMNS; i++ )
	{
		m_ctlColumns.SetCheck( i, 1 );
	}
}

void CPageGeneral::OnDestroy() 
{
	CPropertyPage::OnDestroy();
}

void CPageGeneral::OnBrowseWave() 
{
	WCHAR szFile[MAX_PATH];
	WCHAR szFilter[1024];
	WCHAR szFolder[MAX_PATH];

	LoadString(AfxGetResourceHandle(), STR_WAVEFILTER, szFilter, 1024);
	pipe2null(szFilter);

	WCHAR szTitle[MAX_PATH];
	LoadString(AfxGetResourceHandle(), STR_BROWSEWAVE, szTitle, MAX_PATH);

	SHGetFolderPath(NULL, CSIDL_MYMUSIC|CSIDL_FLAG_CREATE, NULL, 0, szFolder);

	if (OpenBox(m_hWnd, szTitle, szFilter, szFile, szFolder, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY))
	{
		m_strWave = szFile;

		CButton* pCheck = static_cast<CButton*> (GetDlgItem(IDC_AUDIOCUE));
		ASSERT (pCheck);
		pCheck->SetCheck(TRUE);

		UpdateData(FALSE);
	}
}

void CPageGeneral::OnChangeWave() 
{
}

void CPageGeneral::OnKillFocusEditWave() 
{
	UpdateData(TRUE);
	if (!DoesFileExist(m_strWave) && !m_strWave.IsEmpty())
	{
		CString strError;
		CString strTitle;
		strTitle.LoadString(ERR_TITLE);
		strError.FormatMessage(ERR_REENTERWAVE, m_strWave);

		ErrorMessageBox(AfxGetMainWnd()->GetSafeHwnd(),
			GetLastError(),
			strTitle,
			strError);

		m_strWave.Empty();
		UpdateData(FALSE);
		GetDlgItem(IDC_EDITWAVE)->SetFocus();
	}
}

void CPageGeneral::OnChangeSavepath() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CPageGeneral::OnKillfocusSavepath() 
{
	UpdateData(TRUE);
	if (!DoesFolderExist(m_strSavePath) && !m_strSavePath.IsEmpty())
	{
		CString strTitle;
		strTitle.LoadString(ERR_TITLE);
		CString strError;
		strError.FormatMessage(ERR_REENTERPATH, m_strSavePath);
		ErrorMessageBox(AfxGetMainWnd()->GetSafeHwnd(), GetLastError(), strTitle, strError);

		m_strSavePath.Empty();
		UpdateData(FALSE);
		GetDlgItem(IDC_SAVEPATH)->SetFocus();
	}	
}

void CPageGeneral::OnBrowsepath() 
{
	WCHAR szFolder[MAX_PATH];
	if (BrowseForFolder(AfxGetMainWnd()->GetSafeHwnd(), L"Choose a folder as your default save path.", szFolder))
	{
		m_strSavePath = szFolder;
	}
	UpdateData(FALSE);
}
