// ************************************************************************
//  文件名：DynamicCompressParam.cpp
//
//  图像灰度动态压缩处理尺度比例常数设定面板类：
//
// ************************************************************************

#include "stdafx.h"
#include "DIP_System.h"
#include "DynamicCompressParam.h"
#include "GrayProc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDynamicCompressParam dialog

CDynamicCompressParam::CDynamicCompressParam(CWnd* pParent /*=NULL*/) : CDialog(CDynamicCompressParam::IDD, pParent)
{
	m_nCValue = 100;
	m_pDoc = NULL;
}

void CDynamicCompressParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN, m_ctrlSpin);
	DDX_Control(pDX, IDC_PARAMSET, m_ctrlSlider);
	DDX_Text(pDX, IDC_CVALUE, m_nCValue);
}

BEGIN_MESSAGE_MAP(CDynamicCompressParam, CDialog)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_PARAMSET, OnReleasedcaptureParamset)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDynamicCompressParam message handlers

BOOL CDynamicCompressParam::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// 滑动条初始居中
	m_ctrlSlider.SetRange(100, 200);
	m_ctrlSlider.SetPos(m_nCValue);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDynamicCompressParam::OnReleasedcaptureParamset(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// 获取当前参数
	m_nCValue = m_ctrlSlider.GetPos();
	
	// 更新参数
	UpdateData(FALSE);

	// 初始化脏标记为FALSE
 	m_pDoc->SetModifiedFlag(FALSE);
 
	// 重新装载文件
	m_pDoc->m_pDIB->ReLoad();

	// 调用WindowTrans()函数进行灰度窗口变换
	CGrayProc clsGrayProc(m_pDoc->m_pDIB);
	clsGrayProc.DynamicCompress(m_nCValue);
	
	// 设置脏标记
	m_pDoc->SetModifiedFlag(TRUE);
	
	// 更新视图
	m_pDoc->UpdateAllViews(NULL);

	*pResult = 0;
}

void CDynamicCompressParam::OnSet() 
{
	// 读取参数
	UpdateData(TRUE);

	// 检测越上界
	if (m_nCValue > m_ctrlSlider.GetRangeMax() || m_nCValue < m_ctrlSlider.GetRangeMin())
	{
		// 重新设置滑动范围
		m_ctrlSlider.SetRange(100 * (int)(m_nCValue / 100), 100 * (int)(m_nCValue / 100) + 100);
	}

	// 重新设置滑动条
	m_ctrlSlider.SetPos(m_nCValue);

	// 初始化脏标记为FALSE
 	m_pDoc->SetModifiedFlag(FALSE);
 
	// 重新装载文件
	m_pDoc->m_pDIB->ReLoad();

	// 调用WindowTrans()函数进行灰度窗口变换
	CGrayProc clsGrayProc(m_pDoc->m_pDIB);
	clsGrayProc.DynamicCompress(m_nCValue);
	
	// 设置脏标记
	m_pDoc->SetModifiedFlag(TRUE);
	
	// 更新视图
	m_pDoc->UpdateAllViews(NULL);
}

void CDynamicCompressParam::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	// 微调参数
	m_nCValue -= pNMUpDown->iDelta;

	// 更新参数显示
	UpdateData(FALSE);

	// 更新设定
	OnSet();

	*pResult = 0;
}

