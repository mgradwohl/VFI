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
	
	//is dialog has no font usage, so don't fuck with it
	//ChangeDialogFont(this, theApp.GetUIFont(), CDF_CENTER);
	
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
