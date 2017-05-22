// CLAHEDLg.cpp : 实现文件
//

#include "stdafx.h"
#include "HistogramDlg.h"
#include "afxdialogex.h"

#include "HistogramWnd.h"

// CCLAHEDLg 对话框

IMPLEMENT_DYNAMIC(CHistogramDlg, CDialogEx)

CHistogramDlg::CHistogramDlg(CWnd* pParent /*=NULL*/) : CDialogEx(CHistogramDlg::IDD, pParent)
{
	m_pHistWnd = NULL;
}

CHistogramDlg::~CHistogramDlg()
{
	if (m_pHistWnd != NULL)
	{
		delete m_pHistWnd;
	}
}

void CHistogramDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHistogramDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CCLAHEDLg 消息处理程序

int CHistogramDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pHistWnd = new CHistogramWnd;
	m_pHistWnd->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 500, 400), this, IDD_WND_HISTOGRAM);
	return 0;
}

void CHistogramDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

void CHistogramDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(&rect);
	if (m_pHistWnd->GetSafeHwnd())
	{
		m_pHistWnd->MoveWindow(0, 0, cx, cy);
	}
}

BOOL CHistogramDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置对话框大小
	int nDlgWidth = 500;
	int nDlgHeight = 400;
	::SetWindowPos(this->m_hWnd, HWND_TOP, 200, 600, nDlgWidth, nDlgHeight, SWP_SHOWWINDOW);
	CenterWindow(this);
	SETDLGICON(IDI_HISTOGRAM);
	return TRUE;
}

