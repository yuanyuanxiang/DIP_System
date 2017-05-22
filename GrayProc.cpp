// ************************************************************************
//  文件名：GrayProc.cpp
//
//  图像灰度处理函数库：
//
//  ToGray()				- 彩色位图转化为灰度位图
//	PointInvert()			- 对图像进行反色变换
//	GetIntensity()			- 对图像各颜色分量的灰度分布(数目、密度)进行统计
//	PointEqua()				- 对图像进行灰度分布均衡化处理
//  GrayStretch()			- 对图像进行灰度折线变换
//	WindowTrans()			- 对图像进行灰度窗口变换
//	PointDZ()				- 对图像进行灰度带阻变换
//	PointDT()				- 对图像进行灰度带通变换
//	PointSML()				- 对图像进行单映射规则直方图规定化变换
//	PointGML()				- 对图像进行组映射规则直方图规定化变换
//	DynamicCompress()		- 对图像进行灰度动态范围压缩处理 
//	CutWave()				- 对图像进行灰度削波处理 
//
// ************************************************************************

#include "stdafx.h"
#include "DIP_System.h"
#include "GrayProc.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGrayProc

// 默认构造函数
CGrayProc::CGrayProc()
{
}

// 将pDIB赋值给内部变量
CGrayProc::CGrayProc(CDIB* pDIB)
{
	m_pDIB = pDIB;
}

CGrayProc::~CGrayProc()
{
}

BEGIN_MESSAGE_MAP(CGrayProc, CWnd)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
//							功能模块开始								  //
////////////////////////////////////////////////////////////////////////////

// 该函数将彩色位图转化为灰度图像
void CGrayProc::ToGray()
{
	LONG i, j;
	
	CHECK_EDITABLE(m_pDIB->GetImage());

	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = m_pDIB->FindAddress();

	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			BYTE *pSrc = lpDIBBits + nChannel * j + i * lLineBytes;
			BYTE R = *(pSrc + 2);
			BYTE G = *(pSrc + 1);
			BYTE B = *pSrc;
			BYTE Y = (9798 * R + 19235 * G + 3735 * B) / 32768;
			*pSrc = *(pSrc + 1) = *(pSrc + 2) = Y;
		}
	}

	EndWaitCursor();
}

// 该函数对图像进行反色变换处理
void CGrayProc::PointInvert()
{
	LONG i, j;
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i ++)
	{
		for (j = 0; j < lWidth; j ++)
		{
			// 对像素各颜色分量进行反色处理
			BYTE *pSrc = lpDIBBits + nChannel * j + lLineBytes * i;
			*pSrc = 255 - *pSrc;
			*(pSrc+1) = 255 - *(pSrc+1);
			*(pSrc+2) = 255 - *(pSrc+2);
		}
	}

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   GetIntensity()
 *
 * 参数:
 *	 int  *nNs_R		- 红色分量的灰度分布统计
 *   int  *nNs_G		- 绿色分量的灰度分布统计
 *   int  *nNs_B		- 蓝色分量的灰度分布统计
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图像RGB分量进行灰度分布的统计
 *
 ************************************************************************/
void CGrayProc::GetIntensity(int *nNs_R, int *nNs_G, int *nNs_B)
{
	LONG i, j;

	//变量初始化
	memset(nNs_R,0,sizeof(int) * 256);
	memset(nNs_G,0,sizeof(int) * 256);
	memset(nNs_B,0,sizeof(int) * 256);

	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = m_pDIB->FindAddress();

	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			BYTE *pSrc = lpDIBBits + nChannel * j + lLineBytes * i;
			nNs_R[*(pSrc + 2)]++;
			nNs_G[*(pSrc + 1)]++;
			nNs_B[*pSrc]++;
		}
	}

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   GetIntensity()
 *
 * 参数:
 *	 float *fPs_R		- 红色分量的灰度分布密度统计
 *   float *fPs_G		- 绿色分量的灰度分布密度统计
 *   float *fPs_B		- 蓝色分量的灰度分布密度统计
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图像RGB分量进行灰度分布密度的统计
 *
 ************************************************************************/
void CGrayProc::GetIntensity(float *fPs_R, float *fPs_G, float *fPs_B)
{
	LONG i, j;

	// 灰度计数
	int nNs_R[256];
	int nNs_G[256];
	int nNs_B[256];

	// 变量初始化
	memset(nNs_R,0,sizeof(nNs_R));
	memset(nNs_G,0,sizeof(nNs_G));
	memset(nNs_B,0,sizeof(nNs_B));
	
	CHECK_EDITABLE(m_pDIB->GetImage());

	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = m_pDIB->FindAddress();

	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			BYTE *pSrc = lpDIBBits + nChannel * j + lLineBytes * i;
			nNs_R[*(pSrc + 2)]++;
			nNs_G[*(pSrc + 1)]++;
			nNs_B[*pSrc]++;
		}
	}

	// 计算灰度分布密度
	for(i=0;i<256;i++)
	{
		fPs_R[i] = nNs_R[i] / (lHeight * lWidth * 1.0f);
		fPs_G[i] = nNs_G[i] / (lHeight * lWidth * 1.0f);
		fPs_B[i] = nNs_B[i] / (lHeight * lWidth * 1.0f);
	}

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   GetIntensity()
 *
 * 参数:
 *	 float *fPs_Y		- Y 分量的灰度分布密度统计
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图像Y分量进行灰度分布密度的统计
 *
 ************************************************************************/
void CGrayProc::GetIntensity(float *fPs_Y)
{
	LONG i, j;

	// 灰度计数
	int nNs_Y[256];

	// 变量初始化
	memset(nNs_Y,0,sizeof(nNs_Y));

	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();

	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = m_pDIB->FindAddress();

	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i ++)
	{
		for (j = 0; j < lWidth; j ++)
		{
			BYTE *pSrc = lpDIBBits + nChannel * j + lLineBytes * i;
			BYTE R = *(pSrc + 2);
			BYTE G = *(pSrc + 1);
			BYTE B = *pSrc;
			BYTE Y = (9798 * R + 19235 * G + 3735 * B) / 32768;
			nNs_Y[Y]++;
		}
	}

	// 计算灰度分布密度
	for(i=0;i<256;i++)
		fPs_Y[i] = nNs_Y[i] / (lHeight * lWidth * 1.0f);

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   GetIntensity()
 *
 * 参数:
 *	 int *nNs_Y			- Y 分量的灰度分布统计
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图像Y分量进行灰度分布统计
 *
 ************************************************************************/
void CGrayProc::GetIntensity(int *nNs_Y)
{
	LONG i, j;

	// 变量初始化
	memset(nNs_Y, 0, sizeof(nNs_Y));
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = m_pDIB->FindAddress();

	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i ++)
	{
		for (j = 0; j < lWidth; j ++)
		{
			// 计算灰度值
			BYTE *pCur = lpDIBBits + j * nChannel + lLineBytes * i;
			BYTE Y = (9798 * *(pCur+2) + 19235 * *(pCur+1) + 3735 * *pCur) / 32768;
			// 灰度统计计数
			nNs_Y[Y]++;		
		}
	}

	EndWaitCursor();
}

// 该函数对图像进行灰度分布均衡化处理
void CGrayProc::PointEqua()
{
	
	LONG i;
	LONG j;

	// 灰度分布密度
	float fPs_R[256];
	float fPs_G[256];
	float fPs_B[256];

	// 中间变量
	float temp_r[256];
	float temp_g[256];
	float temp_b[256];

	int nNs_R[256];
	int nNs_G[256];
	int nNs_B[256];
	
	// 初始化
	memset(temp_r, 0, sizeof(temp_r));
	memset(temp_g, 0, sizeof(temp_g));
	memset(temp_b, 0, sizeof(temp_b));
	
	CHECK_EDITABLE(m_pDIB->GetImage());	
	
	BeginWaitCursor();
	
	// 获取图像的灰度分布密度
	GetIntensity(fPs_R, fPs_G, fPs_B);
	
	// 进行均衡化处理 
	for(i = 0; i < 256; i++)
	{
		if(i == 0)
		{
			temp_r[0] = fPs_R[0];
			temp_g[0] = fPs_G[0];
			temp_b[0] = fPs_B[0];
		}
		else
		{
			temp_r[i] = temp_r[i-1] + fPs_R[i];
			temp_g[i] = temp_g[i-1] + fPs_G[i];
			temp_b[i] = temp_b[i-1] + fPs_B[i];
		}
		nNs_R[i] = (int)(255.0f * temp_r[i] + 0.5f);
		nNs_G[i] = (int)(255.0f * temp_g[i] + 0.5f);
		nNs_B[i] = (int)(255.0f * temp_b[i] + 0.5f);
	}

	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = m_pDIB->FindAddress();

	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			BYTE *pSrc = lpDIBBits + nChannel * j + lLineBytes * i;
			*(pSrc + 2) = nNs_R[*(pSrc + 2)];
			*(pSrc + 1) = nNs_G[*(pSrc + 1)];
			*pSrc = nNs_B[*pSrc]++;
		}
	}
	
	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   GrayStretch()
 *
 * 参数:
 *	 BYTE bX1			- 折点一的原始灰度
 *	 BYTE bY1			- 折点一的变换灰度
 *	 BYTE bX2			- 折点二的原始灰度
 *   BYTE bY2			- 折点二的对应灰度
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图像进行灰度折线变换
 *
 ************************************************************************/
void CGrayProc::GrayStretch(BYTE bX1, BYTE bY1, BYTE bX2, BYTE bY2)
{
	LONG i;
	LONG j;
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();

	// 灰度映射表
	BYTE	bMap[256] = {0};

	// 计算灰度映射表
	for (i = 0; i <= bX1; i++)	//[0 —— X1]
	{
		// 判断bX1是否大于0（防止分母为0）
		if (bX1 > 0)
		{
			// 线性变换
			bMap[i] = (BYTE) (bY1 * i / bX1);
		}
		else
		{
			// 直接赋值为0
			bMap[i] = 0;
		}
	}
	for (; i <= bX2; i++)		//(X1 —— X2]
	{
		// 判断bX1是否等于bX2（防止分母为0）
		if (bX2 != bX1)
		{
			// 线性变换
			bMap[i] = bY1 + (BYTE) ((bY2 - bY1) * (i - bX1) / (bX2 - bX1));
		}
		else
		{
			// 直接赋值为bY1
			bMap[i] = bY1;
		}
	}
	for (; i < 256; i++)		//(X2 —— 256)
	{
		// 判断bX2是否等于255（防止分母为0）
		if (bX2 != 255)
		{
			// 线性变换
			bMap[i] = bY2 + (BYTE) ((255 - bY2) * (i - bX2) / (255 - bX2));
		}
		else
		{
			// 直接赋值为255
			bMap[i] = 255;
		}
	}
	
	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = m_pDIB->FindAddress();

	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i ++)
	{
		for (j = 0; j < lWidth; j ++)
		{
			// 对像素各颜色分量进行灰度映射处理
			BYTE *pSrc = lpDIBBits + nChannel * j + lLineBytes * i;
			*pSrc = bMap[*pSrc];
			*(pSrc + 1) = bMap[*(pSrc + 1)];
			*(pSrc + 2) = bMap[*(pSrc + 2)];
		}
	}
	
	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   WindowTrans()
 *
 * 参数:
 *	 BYTE bLow			- 窗口下界
 *	 BYTE bUp			- 窗口上界
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图像进行灰度窗口变换
 *
 ************************************************************************/
void CGrayProc::WindowTrans(BYTE bLow, BYTE bUp)
{
	
	LONG i;
	LONG j;

	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = m_pDIB->FindAddress();

	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i ++)
	{
		for (j = 0; j < lWidth; j ++)
		{
			BYTE *pSrc = lpDIBBits + nChannel * j + lLineBytes * i;
			if (*pSrc < bLow)           *pSrc = 0;
			else if (*pSrc > bUp)       *pSrc = 255;
			if (*(pSrc + 1) < bLow)     *(pSrc + 1) = 0;
			else if (*(pSrc + 1) > bUp) *(pSrc + 1) = 255;
			if (*(pSrc + 2) < bLow)     *(pSrc + 2) = 0;
			else if (*(pSrc + 2) > bUp) *(pSrc + 2) = 255;
		}
	}

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   PointDZ()
 *
 * 参数:
 *	 BYTE bLow			- 窗口下界
 *	 BYTE bUp			- 窗口上界
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图像进行灰度带阻变换
 *
 ************************************************************************/
void CGrayProc::PointDZ(BYTE bLow, BYTE bUp)
{
	
	LONG i;
	LONG j;
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = m_pDIB->FindAddress();

	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			BYTE *pSrc = lpDIBBits + nChannel * j + lLineBytes * i;
			if (*pSrc < bLow || *pSrc > bUp)           *pSrc = 255;
			if (*(pSrc + 1) < bLow || *pSrc > bUp)     *(pSrc + 1) = 255;
			if (*(pSrc + 2) < bLow || *pSrc > bUp)     *(pSrc + 2) = 255;
		}
	}
	
	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   PointDT()
 *
 * 参数:
 *	 BYTE bLow			- 窗口下界
 *	 BYTE bUp			- 窗口上界
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图像进行灰度带通变换
 *
 ************************************************************************/
void CGrayProc::PointDT(BYTE bLow, BYTE bUp)
{
	
	LONG i;
	LONG j;
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();

	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			BYTE *pSrc = lpDIBBits + nChannel * j + lLineBytes * i;
			if (*pSrc < bLow || *pSrc > bUp)           *pSrc = 0;
			if (*(pSrc + 1) < bLow || *pSrc > bUp)     *(pSrc + 1) = 0;
			if (*(pSrc + 2) < bLow || *pSrc > bUp)     *(pSrc + 2) = 0;
		}
	}
	
	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   PointSML()
 *
 * 参数:
 *	 BYTE bNum			- 规定直方图灰度级数
 *	 int* npNu			- 规定直方图灰度映射关系
 *	 float* fpPu		- 规定直方图各灰度的分布概率
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图像进行单映射规则直方图规定化变换
 *
 ************************************************************************/
void CGrayProc::PointSML(BYTE bNum, int *npNu, float *fpPu)
{
	
	LONG i;
	LONG j;

	// 灰度分布密度
	int ns_r[256];
	int ns_g[256];
	int ns_b[256];
	
	// 灰度分布概率
	float ps_r[256];
	float ps_g[256];
	float ps_b[256];

	// 中间临时变量
	float temp_r[256];
	float temp_g[256];
	float temp_b[256];
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	// 对灰度密度分布进行统计	
	GetIntensity(ns_r, ns_g, ns_b);

	// 对灰度分布概率进行统计
	GetIntensity(ps_r, ps_g, ps_b);
	
	// 计算原始累计直方图
	for (i = 0; i < 256; i++)
	{
		if (i == 0)
		{
			temp_r[0] = ps_r[0];
			temp_g[0] = ps_g[0];
			temp_b[0] = ps_b[0];
		}
		else
		{
			temp_r[i] = temp_r[i-1] + ps_r[i];
			temp_g[i] = temp_g[i-1] + ps_g[i];
			temp_b[i] = temp_b[i-1] + ps_b[i];
		}
		ps_r[i] = temp_r[i];
		ps_g[i] = temp_g[i];
		ps_b[i] = temp_b[i];
	}
	
	// 计算规定累计直方图
	for (i = 0; i < bNum; i++)
	{
		if (i == 0)
		{
			temp_r[0] = fpPu[0];
		}
		else
		{
			temp_r[i] = temp_r[i-1] + fpPu[i];
		}
		fpPu[i] = temp_r[i];
	}
	
	// 确定映射对应关系
	for (i = 0; i < 256; i++)
	{
		// 最接近的规定直方图灰度级
		int m_r = 0;
		int m_g = 0;
		int m_b = 0;
		
		// 最小差值
		float min_value_r = 1.0f;
		float min_value_g = 1.0f;
		float min_value_b = 1.0f;

		// 对规定直方图各灰度进行枚举
		for (j = 0; j < bNum; j++)
		{
			// 当前差值
			float now_value = 0.0f;
			
			// 对R分量计算差值
			if (ps_r[i] - fpPu[j] >= 0.0f)
				now_value = ps_r[i] - fpPu[j];
			else
				now_value = fpPu[j] - ps_r[i];

			// 寻找最接近的规定直方图灰度级
			if (now_value < min_value_r)
			{
				// 最接近的灰度级
				m_r = j;

				// 暂存最小差值
				min_value_r = now_value;
			}

			// 对G分量计算差值
			if (ps_g[i] - fpPu[j] >= 0.0f)
				now_value = ps_g[i] - fpPu[j];
			else
				now_value = fpPu[j] - ps_g[i];

			// 寻找最接近的规定直方图灰度级
			if (now_value < min_value_g)
			{
				// 最接近的灰度级
				m_g = j;
			
				// 暂存最小差值
				min_value_g = now_value;
			}
				
			// 对B分量计算差值
			if (ps_b[i] - fpPu[j] >= 0.0f)
				now_value = ps_b[i] - fpPu[j];
			else
				now_value = fpPu[j] - ps_b[i];
			
			// 寻找最接近的规定直方图灰度级
			if (now_value < min_value_b)
			{
				// 最接近的灰度级
				m_b = j;
			
				// 暂存最小差值
				min_value_b = now_value;
			}
		}

		// 建立灰度映射表
		ns_r[i] = npNu[m_r];
		ns_g[i] = npNu[m_g];
		ns_b[i] = npNu[m_b];
	}

	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			BYTE *pSrc = lpDIBBits + nChannel * j + lLineBytes * i;
			*(pSrc + 2) = ns_r[*(pSrc + 2)];
			*(pSrc + 1) = ns_g[*(pSrc + 1)];
			*pSrc = ns_b[*pSrc];
		}
	}
	
	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   PointGML()
 *
 * 参数:
 *	 BYTE bNum			- 规定直方图灰度级数
 *	 int* npNu			- 规定直方图灰度映射关系
 *	 float* fpPu		- 规定直方图各灰度的分布概率
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图像进行组映射规则直方图规定化变换
 *
 ************************************************************************/
void CGrayProc::PointGML(BYTE bNum, int *npNu, float *fpPu)
{
	
	LONG i;
	LONG j;
	LONG k;

	// 灰度分布密度
	int ns_r[256];
	int ns_g[256];
	int ns_b[256];
	
	// 灰度分布概率
	float ps_r[256];
	float ps_g[256];
	float ps_b[256];

	// 中间临时变量
	float temp_r[256];
	float temp_g[256];
	float temp_b[256];

	// 灰度映射区间的起始灰度
	int A1_r = 0;
	int A1_g = 0;
	int A1_b = 0;
	
	// 灰度映射区间的终止灰度
	int A2_r = 0;
	int A2_g = 0;
	int A2_b = 0;

	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = m_pDIB->FindAddress();

	// 对灰度密度分布进行统计	
	GetIntensity(ns_r, ns_g, ns_b);

	// 对灰度分布概率进行统计
	GetIntensity(ps_r, ps_g, ps_b);
	
	// 计算原始累计直方图
	for (i = 0; i < 256; i++)
	{
		if (i == 0)
		{
			temp_r[0] = ps_r[0];
			temp_g[0] = ps_g[0];
			temp_b[0] = ps_b[0];
		}
		else
		{
			temp_r[i] = temp_r[i-1] + ps_r[i];
			temp_g[i] = temp_g[i-1] + ps_g[i];
			temp_b[i] = temp_b[i-1] + ps_b[i];
		}
		ps_r[i] = temp_r[i];
		ps_g[i] = temp_g[i];
		ps_b[i] = temp_b[i];
	}
	
	// 计算规定累计直方图
	for(i = 0; i < bNum; i++)
	{
		if(i == 0)
		{
			temp_r[0] = fpPu[0];
		}
		else
		{
			temp_r[i] = temp_r[i-1] + fpPu[i];
		}
		fpPu[i] = temp_r[i];
	}

	// 确定映射对应关系
	for (i = 0; i < bNum; i++)
	{
		// 最小差值
		float min_value_r = 1.0f;
		float min_value_g = 1.0f;
		float min_value_b = 1.0f;

		// 对原始直方图分布进行枚举检索
		for (j = 0; j < 256; j++)
		{
			// 当前差值
			float now_value = 0.0f;
			
			// 对R分量计算差值
			if (ps_r[j] - fpPu[i] >= 0.0f)
				now_value = ps_r[j] - fpPu[i];
			else
				now_value = fpPu[i] - ps_r[j];
			
			// 寻找最接近的规定直方图灰度级
			if (now_value < min_value_r)
			{
				// 最接近的灰度级
				A2_r=j;

				// 暂存最小差值
				min_value_r = now_value;
			}

			// 对G分量计算差值
			if (ps_g[j] - fpPu[i] >= 0.0f)
				now_value = ps_g[j] - fpPu[i];
			else
				now_value = fpPu[i] - ps_g[j];
			
			// 寻找最接近的规定直方图灰度级
			if (now_value < min_value_g)
			{
				// 最接近的灰度级
				A2_g = j;

				// 暂存最小差值
				min_value_g = now_value;
			}
				
			// 对B分量计算差值
			if (ps_b[j] - fpPu[i] >= 0.0f)
				now_value = ps_b[j] - fpPu[i];
			else
				now_value = fpPu[i] - ps_b[j];
			
			// 寻找最接近的规定直方图灰度级
			if (now_value < min_value_b)
			{
				// 最接近的灰度级
				A2_b = j;
				
				// 暂存最小差值
				min_value_b = now_value;
			}
		}
			
		// 将[A1_r,A2_r]区间的原始灰度k通过建立映射表ns_r[]映射到灰度nu[i]
		for (k = A1_r; k <= A2_r; k++)
			ns_r[k] = npNu[i];
	
		// 移动区间其始位置
		A1_r = A2_r + 1;
			
		// 将[A1_g,A2_g]区间的原始灰度k通过建立映射表ns_g[]映射到灰度nu[i]
		for (k = A1_g; k <= A2_g; k++)
			ns_g[k] = npNu[i];
		
		// 移动区间其始位置
		A1_g = A2_g + 1;
		
		// 将[A1_b,A2_b]区间的原始灰度k通过建立映射表ns_b[]映射到灰度nu[i]
		for (k = A1_b; k <= A2_b; k++)
			ns_b[k] = npNu[i];
		
		// 移动区间其始位置
		A1_b = A2_b + 1;
	}

	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			// 对像素各颜色分量进行单映射规则的直方图规定化映射处理
			BYTE *pSrc = lpDIBBits + nChannel * j + lLineBytes * i;
			*(pSrc + 2) = ns_r[*(pSrc + 2)];
			*(pSrc + 1) = ns_g[*(pSrc + 1)];
			*pSrc = ns_b[*pSrc];
		}
	}
	
	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   DynamicCompress()
 *
 * 参数:
 *	 float fC			- 尺度比例常数
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图像进行灰度动态范围压缩处理
 *
 ************************************************************************/
void CGrayProc::DynamicCompress(int nC)
{
	
	LONG i;
	LONG j;

	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();

	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = m_pDIB->FindAddress();

	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			BYTE *pSrc = lpDIBBits + nChannel * j + lLineBytes * i;
			*(pSrc + 2) = (BYTE)(nC * log(1.0 + *(pSrc + 2)));
			*(pSrc + 1) = (BYTE)(nC * log(1.0 + *(pSrc + 1)));
			*pSrc = (BYTE)(nC * log(1.0 + *pSrc));
		}
	}
	
	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   CutWave()
 *
 * 参数:
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数将对图象进行灰度削波处理
 *
 ************************************************************************/
void CGrayProc::CutWave()
{
	
	LONG i;
	LONG j;

	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();

	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = m_pDIB->FindAddress();

	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			BYTE *pSrc = lpDIBBits + nChannel * j + lLineBytes * i;
			BYTE B = *pSrc;
			BYTE G = *(pSrc + 1);
			BYTE R = *(pSrc + 2);
			BYTE Y = (9798 * R + 19235 * G + 3735 * B) / 32768;

			if (Y < 30)
			{
				R = G = B = 0;
			}

			if (Y >= 30 && Y < 200)
			{
				R = R * 256 / 200;
				G = G * 256 / 200;
				B = B * 256 / 200;

				// 超限保护
				if (R > 255)	R = 255;
				if (R < 0)		R = 0;
				if (G > 255)	G = 255;
				if (G < 0)		G = 0;
				if (B > 255)	B = 255;
				if (B < 0)		B = 0;
			}

			if (Y >= 200)
			{
				R = G = B = 255;
			}

			// 回写灰度值
			*(pSrc) = B;
			*(pSrc + 1) = G;
			*(pSrc + 2) = R;
		}
	}
	
	EndWaitCursor();
}

