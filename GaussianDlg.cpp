// GaussianDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GaussianDlg.h"
#include "afxdialogex.h"

// CGaussianDlg 对话框

IMPLEMENT_DYNAMIC(CGaussianDlg, CDialogEx)

CGaussianDlg::CGaussianDlg(CWnd* pParent /*=NULL*/) : CDialogEx(CGaussianDlg::IDD, pParent)
{
	m_dGaussianMean = 0;
	m_dGaussianStd = 0;
	m_String1 = L"均值:";
	m_String2 = L"方差:";
}

CGaussianDlg::~CGaussianDlg()
{
}

void CGaussianDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GAUSSIAN_MEAN, m_EditGaussianMean);
	DDX_Control(pDX, IDC_GAUSSIAN_STD, m_EditGaussianStd);
	DDX_Text(pDX, IDC_GAUSSIAN_MEAN, m_dGaussianMean);
	DDX_Text(pDX, IDC_GAUSSIAN_STD, m_dGaussianStd);
	DDX_Control(pDX, IDC_ITEM1, m_Static1);
	DDX_Control(pDX, IDC_ITEM2, m_Static2);
	DDX_Text(pDX, IDC_ITEM1, m_String1);
	DDX_Text(pDX, IDC_ITEM2, m_String2);
}

BEGIN_MESSAGE_MAP(CGaussianDlg, CDialogEx)
END_MESSAGE_MAP()

// CGaussianDlg 消息处理程序

