// ColorGradDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ColorGradDlg.h"

// CColorGradDlg 对话框

IMPLEMENT_DYNAMIC(CColorGradDlg, CDialogEx)

CColorGradDlg::CColorGradDlg(CWnd* pParent /*=NULL*/) : CDialogEx(CColorGradDlg::IDD, pParent)
{
	m_pColorGradWnd = NULL;
}

CColorGradDlg::~CColorGradDlg()
{
	if (m_pColorGradWnd != NULL)
	{
		delete m_pColorGradWnd;
	}
}

void CColorGradDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CColorGradDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CColorGradDlg 消息处理程序

void CColorGradDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (m_pColorGradWnd != NULL)
		m_pColorGradWnd->MoveWindow(0, 0, cx, cy);
}

int CColorGradDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetClientRect(&rect);
	m_pColorGradWnd = new CColorGradWnd;
	m_pColorGradWnd->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect, this, IDD_WND_COLORGRAD);

	return 0;
}

BOOL CColorGradDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置对话框大小
	int nDlgWidth = 420;
	int nDlgHeight = 380;
	::SetWindowPos(this->m_hWnd, HWND_TOP, 200, 200, nDlgWidth, nDlgHeight, SWP_SHOWWINDOW);

	//设置图标
	SETDLGICON(IDI_DIALOG)

	return TRUE;  // return TRUE unless you set the focus to a control
}

