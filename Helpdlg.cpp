// HelpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wndlib.h"
#include "VFI.h"
#include "HelpDlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static CHAR THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHelpDlg dialog

CHelpDlg::CHelpDlg(CWnd* pParent, UINT nHelpID, bool fShowCheck)
	: CDialog(CHelpDlg::IDD, pParent)
{
	m_fShowCheck = fShowCheck;
	m_nHelpID = nHelpID;
}

void CHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHelpDlg)
	DDX_Check(pDX, IDC_DISPLAYAGAIN, m_fShowAgain);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_RICHEDIT, m_RTF);
}


BEGIN_MESSAGE_MAP(CHelpDlg, CDialog)
	//{{AFX_MSG_MAP(CHelpDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelpDlg message handlers

BOOL CHelpDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//ChangeDialogFont(this, theApp.GetUIFont(), CDF_CENTER);

	CWnd* pWnd = GetDlgItem(IDC_DISPLAYAGAIN);
	if (m_fShowCheck)
	{
		pWnd->ShowWindow(SW_NORMAL);
	}
	else
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	UpdateData(FALSE);

	LoadRTF();
	
	return TRUE;
}

bool CHelpDlg::LoadRTF(UINT nID)
{
	m_nHelpID = nID;
	return LoadRTF();
}


DWORD CALLBACK CHelpDlg::EditStreamInCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG* pcb)
{
	CMemFile* pFile = reinterpret_cast<CMemFile*> (dwCookie);
	ASSERT(pFile != NULL);

	// read the number of bytes requested (no more than that!)
	*pcb = pFile->Read(pbBuff, cb);

	return (DWORD)(*pcb ? NOERROR : E_FAIL);
}

bool CHelpDlg::LoadRTF()
{
	if (m_nHelpID == 0)
	{
		// error
		ErrorMessageBox(AfxGetResourceHandle(),
			NULL,
			GetLastError(),
			ERR_TITLE,
			ERR_HELPTOPICNOTFOUND);

		return false;
	}

	HGLOBAL hG = NULL;
	BYTE* pV = NULL;
	LONG cbRes = 0;
	HRSRC hRes = NULL;

	// Get the Module Handle for Resource Operations
	HMODULE hMod=AfxGetResourceHandle();

	// Find our resource
	hRes = FindResource(hMod, MAKEINTRESOURCE(m_nHelpID), L"RTF");
	if (NULL == hRes)
	{
		ErrorMessageBox(AfxGetResourceHandle(),
			NULL,
			GetLastError(),
			ERR_TITLE,
			ERR_HELPTOPICNOTFOUND);

		return false;
	}

	//Load it
	hG = LoadResource(hMod, hRes);
	if (NULL == hG)
	{
		return false;
	}

	// How big is it
	cbRes = SizeofResource(hMod, hRes);

	//Lock it in memory
	pV = static_cast<BYTE*> (LockResource(hG));

	CMemFile resstm(pV, cbRes) ;
	/*	DWORD dwSize = */ resstm.GetLength();

	EDITSTREAM es;
	es.dwCookie = (ULONG) &resstm;  //dwCookie is made for this
	es.dwError = 0;
	es.pfnCallback = (EDITSTREAMCALLBACK) EditStreamInCallBack;

	m_RTF.StreamIn(SF_RTF, es);

	// detach from the memory stream
	resstm.Detach();

	return true;
}
