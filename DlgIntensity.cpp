// ************************************************************************
//  文件名：DlgIntensity.cpp
//
//  图像灰度分布直方图显示面板类：
//
// ************************************************************************

#include "stdafx.h"
#include "DIP_System.h"
#include "DlgIntensity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgIntensity dialog

CDlgIntensity::CDlgIntensity(CWnd* pParent /*=NULL*/) : CDialog(CDlgIntensity::IDD, pParent)
{
}

void CDlgIntensity::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgIntensity, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgIntensity message handlers

void CDlgIntensity::OnPaint() 
{
	// 字符串
	CString str;
	
	LONG i;
	
	// 最大计数
	float fMaxIntensity = 0;
	
	// 设备上下文
	CPaintDC dc(this);
	
	// 获取绘制坐标的静态框
	CWnd* pWnd = GetDlgItem(IDC_COORD);
	
	// 指针
	CDC* pDC = pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow();
	
	pDC->Rectangle(0,0,330,300);
	
	// 创建画笔对象
	CPen* pPenRed = new CPen;
	
	// 红色画笔
	pPenRed->CreatePen(PS_SOLID,1,RGB(255,0,0));
	
	// 创建画笔对象
	CPen* pPenBlue = new CPen;
	
	// 蓝色画笔
	pPenBlue->CreatePen(PS_SOLID,1,RGB(0,0, 255));
	
	// 选中当前红色画笔，并保存以前的画笔
	CGdiObject* pOldPen = pDC->SelectObject(pPenRed);
	
	// 绘制坐标轴
	pDC->MoveTo(10,10);
	
	// 垂直轴
	pDC->LineTo(10,280);
	
	// 水平轴
	pDC->LineTo(320,280);
	
	// 写X轴刻度值
	str.Format(L"0");
	pDC->TextOut(10, 283, str);
	str.Format(L"50");
	pDC->TextOut(60, 283, str);
	str.Format(L"100");
	pDC->TextOut(110, 283, str);
	str.Format(L"150");
	pDC->TextOut(160, 283, str);
	str.Format(L"200");
	pDC->TextOut(210, 283, str);
	str.Format(L"255");
	pDC->TextOut(265, 283, str);
	
	// 绘制X轴刻度
	for (i = 0; i < 256; i += 5)
	{
		if ((i & 1) == 0)
		{
			// 10的倍数
			pDC->MoveTo(i + 10, 280);
			pDC->LineTo(i + 10, 284);
		}
		else
		{
			// 10的倍数
			pDC->MoveTo(i + 10, 280);
			pDC->LineTo(i + 10, 282);
		}
	}
	
	// 绘制X轴箭头
	pDC->MoveTo(315,275);
	pDC->LineTo(320,280);
	pDC->LineTo(315,285);
	
	// 绘制X轴箭头
	pDC->MoveTo(10,10);
	pDC->LineTo(5,15);
	pDC->MoveTo(10,10);
	pDC->LineTo(15,15);
	
	// 计算最大计数值
	for (i = 0; i < 256; i ++)
	{
		// 判断是否大于当前最大值
		if (m_fIntensity[i] > fMaxIntensity)
		{
			// 更新最大值
			fMaxIntensity = m_fIntensity[i];
		}
	}
	
	// 输出最大计数值
	pDC->MoveTo(10, 25);
	pDC->LineTo(14, 25);
	str.Format(L"%f", fMaxIntensity);
	pDC->TextOut(11, 26, str);
	
	// 更改成蓝色画笔
	pDC->SelectObject(pPenBlue);	
	
	// 判断是否有计数
	if (fMaxIntensity > 0)
	{
		// 绘制直方图
		for (i = 0; i < 256; i ++)
		{
			pDC->MoveTo(i + 10, 280);
			pDC->LineTo(i + 10, 281 - (int) (m_fIntensity[i] * 256 / fMaxIntensity));
		}
	}
	
	// 恢复以前的画笔
	pDC->SelectObject(pOldPen);	
	
	// 删除新的画笔
	delete pPenRed;
	delete pPenBlue;
}


