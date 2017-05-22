// SpaceFilterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SpaceFilterDlg.h"
#include "afxdialogex.h"
#include "SpaceFilterWnd.h"

// CSpaceFilterDlg 对话框

IMPLEMENT_DYNAMIC(CSpaceFilterDlg, CDialogEx)

CSpaceFilterDlg::CSpaceFilterDlg(CWnd* pParent /*=NULL*/) : CDialogEx(CSpaceFilterDlg::IDD, pParent)
{
	m_pSpaceFilterWnd = NULL;
}

CSpaceFilterDlg::~CSpaceFilterDlg()
{
	if (m_pSpaceFilterWnd != NULL)
	{
		delete m_pSpaceFilterWnd;
	}
}

void CSpaceFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSpaceFilterDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CSpaceFilterDlg 消息处理程序

void CSpaceFilterDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_pSpaceFilterWnd->GetSafeHwnd())
	{
		m_pSpaceFilterWnd->MoveWindow(0, 0, cx, cy);
	}
}

int CSpaceFilterDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetClientRect(&rect);

	//成员变量初始化
	m_pSpaceFilterWnd = new CSpaceFilterWnd;
	m_pSpaceFilterWnd->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect, this, IDD_WND_SPACEFILTER);

	return 0;
}

BOOL CSpaceFilterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 设置对话框大小
	int nDlgWidth = 600;
	int nDlgHeight = 400;
	::SetWindowPos(this->m_hWnd, HWND_TOP, 0, 0, nDlgWidth, nDlgHeight, SWP_SHOWWINDOW);
	CenterWindow(this);
	SETDLGICON(IDI_FILTER);
	return TRUE;  // return TRUE unless you set the focus to a control
}

