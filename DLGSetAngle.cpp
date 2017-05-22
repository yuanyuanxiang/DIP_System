// ************************************************************************
//  文件名：DlgSetAngle.cpp
//
//	旋转角度设定面板类：
//
// ************************************************************************

#include "stdafx.h"
#include "dip_system.h"
#include "DLGSetAngle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLGSetAngle dialog

CDLGSetAngle::CDLGSetAngle(CWnd* pParent /*=NULL*/)
	: CDialog(CDLGSetAngle::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLGSetAngle)
	m_fAngle = 0.0f;
	//}}AFX_DATA_INIT
}

void CDLGSetAngle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLGSetAngle)
	DDX_Text(pDX, IDC_PARAM, m_fAngle);
	DDV_MinMaxFloat(pDX, m_fAngle, -360.f, 360.f);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDLGSetAngle, CDialog)
	//{{AFX_MSG_MAP(CDLGSetAngle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLGSetAngle message handlers

