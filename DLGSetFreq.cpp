// ************************************************************************
//  文件名：DlgSetFreq.cpp
//
//  频率设定面板类：
//
// ************************************************************************

#include "stdafx.h"
#include "dip_system.h"
#include "DLGSetFreq.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLGSetFreq dialog

CDLGSetFreq::CDLGSetFreq(CWnd* pParent /*=NULL*/)
	: CDialog(CDLGSetFreq::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLGSetFreq)
	m_fValue = 250.0f;
	//}}AFX_DATA_INIT
}

void CDLGSetFreq::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLGSetFreq)
	DDX_Text(pDX, IDC_VALUE, m_fValue);
	DDV_MinMaxFloat(pDX, m_fValue, 0.f, 10000.f);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDLGSetFreq, CDialog)
	//{{AFX_MSG_MAP(CDLGSetFreq)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLGSetFreq message handlers

