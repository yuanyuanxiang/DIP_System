// ************************************************************************
//  文件名：Noise.cpp
//
//  噪声模型函数库：
//
//  RandomNoise()			- 随机噪声模型
//	SaltNoise()				- 盐性噪声模型
//
// ************************************************************************

#include "stdafx.h"
#include "dip_system.h"
#include "Noise.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNoise

CNoise::CNoise()
{
}

CNoise::CNoise(CDIB* pDIB)
{
	m_pDIB = pDIB;
}

CNoise::~CNoise()
{
}

BEGIN_MESSAGE_MAP(CNoise, CWnd)
	//{{AFX_MSG_MAP(CNoise)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNoise message handlers

/*************************************************************************
 *
 * 函数名称：
 *   RandomNoise()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   HDIB				- 新的DIB
 *
 * 说明:
 *   该函数对图象引入随机噪声模型
 *
 ************************************************************************/

void CNoise::RandomNoise()
{
	
	LONG i;
	LONG j;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();

	LONG lLineBytes = m_pDIB->RowLen();
	
	// 生成伪随机种子
	srand((unsigned)time(NULL));

	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i ++)
	{
		for (j = 0; j < lLineBytes; j ++)
		{
			// 取随机值
			LONG Temp = rand();

			// 对像素各颜色分量进行随机加噪处理
			BYTE R = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			*((BYTE *)lpDIBBits + lLineBytes * i + j) = (BYTE)(R * 224 / 256 + Temp / 1024);
			j++;
			BYTE G = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			*((BYTE *)lpDIBBits + lLineBytes * i + j) = (BYTE)(G * 224 / 256 + Temp / 1024);
			j++;
			BYTE B = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			*((BYTE *)lpDIBBits + lLineBytes * i + j) = (BYTE)(B * 224 / 256 + Temp / 1024);
		}
	}

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   SaltNoise()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   HDIB				- 新的DIB
 *
 * 说明:
 *   该函数对图象引入盐性噪声模型
 *
 ************************************************************************/

void CNoise::SaltNoise()
{
	
	LONG i;
	LONG j;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();

	LONG lLineBytes = m_pDIB->RowLen();
	
	// 生成伪随机种子
	srand((unsigned)time(NULL));

	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i ++)
	{
		for (j = 0; j < lLineBytes; j ++)
		{
			// 取随机值
			if (rand() > 32500)
			{
				// 对像素各颜色分量进行随机加噪处理
				*((BYTE *)lpDIBBits + lLineBytes * i + j) = 0;
				j++;
				*((BYTE *)lpDIBBits + lLineBytes * i + j) = 0;
				j++;
				*((BYTE *)lpDIBBits + lLineBytes * i + j) = 0;
			}
		}
	}

	EndWaitCursor();
}


