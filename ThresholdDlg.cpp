// ThresholdDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DIP_System.h"
#include "ThresholdDlg.h"
#include "GrayOperator.h"
#include "Mainfrm.h"

// CThresholdDlg 对话框

IMPLEMENT_DYNAMIC(CThresholdDlg, CDialog)

CThresholdDlg::CThresholdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CThresholdDlg::IDD, pParent)
	, m_nNum(120)
	, m_nPos(0)
{
	m_bModify = false;
	m_pBuf = NULL;
}

CThresholdDlg::~CThresholdDlg()
{
	SAFE_DELETE(m_pBuf);
	SAFE_DELETE (dlg);
}

void CThresholdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NUM, m_nNum);
	DDX_Control(pDX, IDC_SLIDER, m_SliderNum);
	DDX_Slider(pDX, IDC_SLIDER, m_nPos);
}

BEGIN_MESSAGE_MAP(CThresholdDlg, CDialog)
	ON_EN_CHANGE(IDC_NUM, &CThresholdDlg::OnEnChangeNum)
	ON_BN_CLICKED(IDC_SAVE, &CThresholdDlg::OnBnClickedSave)
	ON_BN_CLICKED(IDOK, &CThresholdDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CThresholdDlg::OnBnClickedCancel)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

CDIP_SystemDoc* CThresholdDlg::GetDocument()
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd *)AfxGetApp()->GetMainWnd();
	CMDIChildWnd* pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();
	return (CDIP_SystemDoc *)pChild->GetActiveDocument();
}

CDIP_SystemView* CThresholdDlg::GetActiveView()
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd *)AfxGetApp()->GetMainWnd();
	CMDIChildWnd* pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();
	return (CDIP_SystemView *)pChild->GetActiveView();
}

/******************************************************************************
*	作用:		对话框初始化时显示预览对话框
******************************************************************************/
BOOL CThresholdDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CDIP_SystemDoc* pDoc = GetDocument();

	m_nWidth = pDoc->m_pDIB->Width();
	m_nHeight = pDoc->m_pDIB->Height();
	m_nChannel = pDoc->m_pDIB->Channel();
	m_nRowLen = pDoc->m_pDIB->RowLen();
	m_nLength = pDoc->m_pDIB->Length();
	m_pSrc = pDoc->m_pDIB->FindAddress();
	m_pBuf = new BYTE[m_nLength];
	memcpy(m_pBuf, m_pSrc, m_nLength);

	dlg = new CPreviewDlg(this);

	dlg->m_nType = 1;
	dlg->m_bEnableClose = false;
	dlg->Create(IDD_DLG_PREVIEW);
	dlg->ShowWindow(SW_SHOW);

	m_SliderNum.SetRange(0, 255);
	m_nPos = 120;
	UpdateData(FALSE);

	Refresh();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

/******************************************************************************
*	作用:		滑块响应函数
******************************************************************************/
void CThresholdDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateData(TRUE);

	m_nNum = m_nPos;

	UpdateData(FALSE);
	Refresh();
	m_bModify = true;

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

/******************************************************************************
*	作用:		刷新预览窗口
******************************************************************************/
void CThresholdDlg::Refresh()
{
	UpdateData(TRUE);

	CDIP_SystemDoc* pDoc = GetDocument();

	ThresholdTrans(m_pBuf, m_pSrc, m_nWidth, m_nHeight, m_nRowLen, m_nChannel, m_nNum);

	dlg->Refresh();
}

/******************************************************************************
*	作用:		阈值被修改时的响应函数
******************************************************************************/
void CThresholdDlg::OnEnChangeNum()
{
	UpdateData(TRUE);

	if (m_nNum < 0)
		m_nNum = 0;
	else if (m_nNum > 255)
		m_nNum = 255;

	m_nPos = m_nNum;
	UpdateData(FALSE);
	Refresh();
	m_bModify = true;
}

void CThresholdDlg::OnBnClickedSave()
{
	if (m_bModify)
		GetActiveView()->Invalidate();
}

void CThresholdDlg::OnBnClickedOk()
{
	OnBnClickedSave();
	CDialog::OnOK();
}

void CThresholdDlg::OnBnClickedCancel()
{
	memcpy(m_pSrc, m_pBuf, m_nLength);
	OnBnClickedSave();
	CDialog::OnCancel();
}

