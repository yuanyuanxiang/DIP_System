// ChangeBppDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DIP_System.h"
#include "ChangeBppDlg.h"
#include "afxdialogex.h"
#include "DIP_SystemDoc.h"
#include "DIP_SystemView.h"

// CChangeBppDlg 对话框

IMPLEMENT_DYNAMIC(CChangeBppDlg, CDialogEx)

CChangeBppDlg::CChangeBppDlg(CWnd* pParent /*=NULL*/) : CDialogEx(CChangeBppDlg::IDD, pParent)
{
	m_nTargetBpp = 24;
}

CChangeBppDlg::~CChangeBppDlg()
{
}

void CChangeBppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_BPP, m_StaticBpp);
	DDX_Control(pDX, IDC_COMBO_BPP, m_ComboBpp);
}

BEGIN_MESSAGE_MAP(CChangeBppDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_BPP, &CChangeBppDlg::OnSelchangeComboBpp)
	ON_BN_CLICKED(IDOK, &CChangeBppDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CChangeBppDlg 消息处理程序

void CChangeBppDlg::OnSelchangeComboBpp()
{
	int nSel = m_ComboBpp.GetCurSel();
	switch (nSel)
	{
	case 0 : m_nTargetBpp = 8; break;
	case 1 : m_nTargetBpp = 24;break;
	case 2 : m_nTargetBpp = 32;break;
	default:                   break;
	}
}

void CChangeBppDlg::OnBnClickedOk()
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd *)AfxGetApp()->GetMainWnd();
	CMDIChildWnd* pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();
	CDIP_SystemDoc* pDoc =  (CDIP_SystemDoc *)pChild->GetActiveDocument();
	CDIP_SystemView* pView = (CDIP_SystemView *)pChild->GetActiveView();

	pDoc->m_pDIB->PushCopy();
	pDoc->m_pDIB->ChangeBPP(m_nTargetBpp);
	pView->Invalidate(TRUE);

	::SendMessage(GetSafeHwnd(), WM_CLOSE, 0, 0);

	CDialogEx::OnOK();
}

BOOL CChangeBppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CMDIFrameWnd* pFrame = (CMDIFrameWnd *)AfxGetApp()->GetMainWnd();
	CMDIChildWnd* pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();
	CDIP_SystemDoc* pDoc =  (CDIP_SystemDoc *)pChild->GetActiveDocument();

	CString sCurBpp;
	sCurBpp.Format(L"%d", pDoc->m_pDIB->BitCount());
	m_StaticBpp.SetWindowText(sCurBpp);

	m_ComboBpp.InsertString(0, L"8位色");
	m_ComboBpp.InsertString(1, L"24位色");
	m_ComboBpp.InsertString(2, L"32位色");
	m_ComboBpp.SetCurSel(1);

	return TRUE;  // return TRUE unless you set the focus to a control
}

