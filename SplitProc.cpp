// ************************************************************************
//  文件名：SplitProc.cpp
//
//  图像分割处理函数库：
//
//  RgnGrow()				- 区域生长
//	ContourDIB()			- 轮廓提取
//	TraceDIB()				- 边界跟踪
//	Sobel()					- 索贝尔算子的边缘检测
//	Roberts()				- 罗伯特交叉算子的边缘检测
//	Prewitt()				- 蒲瑞维特算子的边缘检测
//	Kirsch()				- Kirsch算子的边缘检测
//	Laplacian()				- 拉普拉斯算子的边缘检测
//	GuassLaplacian()		- 高斯——拉普拉斯算子的边缘检测
//	Hough()					- 哈夫变换检测圆周(r = 40) 
//	Threshold()				- 阀值化区域分割
//
//
// ************************************************************************

#include "stdafx.h"
#include "DIP_System.h"
#include "SplitProc.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitProc

CSplitProc::CSplitProc()
{
}

CSplitProc::CSplitProc(CDIB* pDIB)
{
	m_pDIB = pDIB;
}

CSplitProc::~CSplitProc()
{
}

BEGIN_MESSAGE_MAP(CSplitProc, CWnd)
	//{{AFX_MSG_MAP(CSplitProc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
//							功能模块开始								  //
////////////////////////////////////////////////////////////////////////////

/*************************************************************************
 *
 * 函数名称：
 *   RgnGrow()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *   COLORREF clrSeed_I - 种子点一的RGB值
 *   COLORREF clrSeed_II- 种子点二的RGB值
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数将彩色位图转化为灰度图像。
 *
 ************************************************************************/
void CSplitProc::RgnGrow(COLORREF clrSeed_I, COLORREF clrSeed_II)
{
	
	LONG i;
	LONG j;

	// 计算种子点一的灰度值
	BYTE R_I = GetRValue(clrSeed_I);
	BYTE G_I = GetGValue(clrSeed_I);
	BYTE B_I = GetBValue(clrSeed_I);
	float fY_I = (9798.0f * R_I + 19235.0f * G_I + 3735.0f * B_I) / 32768.0f;
	
	// 计算种子点二的灰度值
	BYTE R_II = GetRValue(clrSeed_II);
	BYTE G_II = GetGValue(clrSeed_II);
	BYTE B_II = GetBValue(clrSeed_II);
	float fY_II = (9798.0f * R_II + 19235.0f * G_II + 3735.0f * B_II) / 32768.0f;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i ++)
	{
		for (j = 0; j < lLineBytes; j ++)
		{
			//获取各颜色分量
			BYTE R = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			j++;
			BYTE G = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			j++;
			BYTE B = *((BYTE *)lpDIBBits + lLineBytes * i + j);

			//计算当前点灰度值
			BYTE Y = (9798 * R + 19235 * G + 3735 * B) / 32768;
#if _MSC_VER>=1900
			if (fabs(Y - fY_I) < fabs(Y - fY_II))
#else
			if (abs(Y - fY_I) < abs(Y - fY_II))
#endif
			{
				//当前点同种子一灰度值比较接近
				
				//将种子一的颜色赋给当前像素
				*((BYTE *)lpDIBBits + lLineBytes * i + j - 2) = R_I;
				*((BYTE *)lpDIBBits + lLineBytes * i + j - 1) = G_I;
				*((BYTE *)lpDIBBits + lLineBytes * i + j ) = B_I;
			}
			else
			{
				//当前点同种子二灰度值比较接近
				
				//将种子二的颜色赋给当前像素
				*((BYTE *)lpDIBBits + lLineBytes * i + j - 2) = R_II;
				*((BYTE *)lpDIBBits + lLineBytes * i + j - 1) = G_II;
				*((BYTE *)lpDIBBits + lLineBytes * i + j ) = B_II;
			}
		}
	}

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   Sobel()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *	 对图像进行索贝尔算子的边缘检测
 *
 ************************************************************************/
void CSplitProc::Sobel()
{
	
	LONG i;
	LONG j;
	LONG k;
	LONG l;
	
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();

	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// 不能用char，也不能用::strcpy()
	BYTE* m_temp;
	m_temp=new BYTE [lLineBytes * lHeight];

	// 中间变量
	int v_r_v, v_g_v, v_b_v, g_v;
	int v_r_h, v_g_h, v_b_h, g_h;

	// 复制图象数据到中间缓存
	for (i = 0; i < lLineBytes * lHeight; i++)
		m_temp[i] = *(lpDIBBits + i);

	// 3X3 模版
	for (i = 0; i < lWidth; i++)			//被处理像素在i列
	{
		for (j = 0; j < lHeight; j++)	//被处理像素在j行
		{
			v_r_v = v_g_v = v_b_v = v_r_h = v_g_h = v_b_h = 0;
			
			for (k = i - 1; k < i + 2; k++)	//3*3模版
			{
				for (l = j - 1; l < j + 2; l++)
				{
					// 防止内存溢出
					if (k >= 0  && l >= 0 && k < lWidth && l < lHeight)
					{
						// 检测模版
						if (k == i - 1)
						{
							if (l == j)
								g_v = -2;
							else
								g_v = -1;
						}
						if (k == i + 1)
						{
							if (l == j)
								g_v = 2;
							else
								g_v = 1;
						}
						if (k == i)
							g_v = 0;
						if (l == j - 1)
						{	
							if (k == i)
								g_h = 2;
							else
								g_h = 1;
						}
						if (l == j + 1)
						{
							if (k == i)
								g_h = -2;
							else
								g_h = -1;
						}
						if (l == j)
							g_h = 0;
						
						v_r_v += *(lpDIBBits + l * lLineBytes + k * nChannel) * g_v;
						v_r_h += *(lpDIBBits + l * lLineBytes + k * nChannel) * g_h;
						v_g_v += *(lpDIBBits + l * lLineBytes + k * nChannel + 1) * g_v;
						v_g_h += *(lpDIBBits + l * lLineBytes + k * nChannel + 1) * g_h;
						v_b_v += *(lpDIBBits + l * lLineBytes + k * nChannel + 2) * g_v;
						v_b_h += *(lpDIBBits + l * lLineBytes + k * nChannel + 2) * g_h;
					}
				}
			}
			m_temp[j * lLineBytes + i * nChannel] = (int)sqrt(1.0*v_r_v * v_r_v + v_r_h * v_r_h);
			m_temp[j * lLineBytes + i * nChannel + 1] = (int)sqrt(1.0*v_g_v * v_g_v + v_g_h * v_g_h);
			m_temp[j * lLineBytes + i * nChannel + 2] = (int)sqrt(1.0*v_b_v * v_b_v + v_b_h * v_b_h);
		}
	}

	// 回存处理结果到DIB
	for(i = 0; i < lLineBytes * lHeight; i++)
		*(lpDIBBits + i) = m_temp[i];

	// 释放内存
	delete[] m_temp;
	
	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   Prewitt()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *	 对图像进行蒲瑞维特算子的边缘检测
 *
 ************************************************************************/
void CSplitProc::Prewitt()
{
	
	LONG i;
	LONG j;
	LONG k;
	LONG l;
	
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();

	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// 不能用char，也不能用::strcpy()
	BYTE* m_temp;
	m_temp = new BYTE [lLineBytes * lHeight];

	// 中间变量
	int v_r_v, v_g_v, v_b_v, g_v;
	int v_r_h, v_g_h, v_b_h, g_h;

	// 复制图象数据到中间缓存
	for (i = 0; i < lLineBytes * lHeight; i++)
		m_temp[i] = *(lpDIBBits + i);

	// 3X3 模版
	for (i = 0; i < lWidth; i++)			//被处理像素在i列
	{
		for (j = 0; j < lHeight; j++)	//被处理像素在j行
		{
			v_r_v = v_g_v = v_b_v = v_r_h = v_g_h = v_b_h = 0;
			
			for (k = i - 1; k < i + 2; k++)	//3*3模版
			{
				for (l = j - 1; l < j + 2; l++)
				{
					// 防止内存溢出
					if (k >= 0  && l >= 0 && k < lWidth && l < lHeight)
					{
						// 检测模版
						if (k == i - 1)
							g_v = -1;
						if (k == i + 1)
							g_v = 1;
						if (k == i)
							g_v = 0;
						if (l == j - 1)
							g_h = 1;
						if (l == j + 1)
							g_h = -1;
						if (l == j)
							g_h = 0;
						
						v_r_v += *(lpDIBBits + l * lLineBytes + k * nChannel) * g_v;
						v_r_h += *(lpDIBBits + l * lLineBytes + k * nChannel) * g_h;
						v_g_v += *(lpDIBBits + l * lLineBytes + k * nChannel + 1) * g_v;
						v_g_h += *(lpDIBBits + l * lLineBytes + k * nChannel + 1) * g_h;
						v_b_v += *(lpDIBBits + l * lLineBytes + k * nChannel + 2) * g_v;
						v_b_h += *(lpDIBBits + l * lLineBytes + k * nChannel + 2) * g_h;
					}
				}
			}
			m_temp[j * lLineBytes + i * nChannel] = (int)sqrt(1.0*v_r_v * v_r_v + v_r_h * v_r_h);
			m_temp[j * lLineBytes + i * nChannel + 1] = (int)sqrt(1.0*v_g_v * v_g_v + v_g_h * v_g_h);
			m_temp[j * lLineBytes + i * nChannel + 2] = (int)sqrt(1.0*v_b_v * v_b_v + v_b_h * v_b_h);
		}
	}

	// 回存处理结果到DIB
	for(i = 0; i < lLineBytes * lHeight; i++)
		*(lpDIBBits + i) = m_temp[i];

	// 释放内存
	delete[] m_temp;
	
	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   Roberts()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *	 对图像进行罗伯特交叉算子的边缘检测
 *
 ************************************************************************/
void CSplitProc::Roberts()
{
	
	LONG i;
	LONG j;
	LONG k;
	LONG l;
	
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();

	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// 不能用char，也不能用::strcpy()
	BYTE* m_temp;
	m_temp = new BYTE [lLineBytes * lHeight];

	// 中间变量
	int v_r_v, v_g_v, v_b_v, g_v;
	int v_r_h, v_g_h, v_b_h, g_h;

	// 复制图象数据到中间缓存
	for (i = 0; i < lLineBytes * lHeight; i++)
		m_temp[i] = *(lpDIBBits + i);

	// 2X2 模版
	for (i = 0; i < lWidth; i++)		//被处理像素在i列
	{
		for (j = 0; j < lHeight; j++)	//被处理像素在j行
		{
			v_r_v = v_g_v = v_b_v = v_r_h = v_g_h = v_b_h = 0;
			
			for (k = i - 1; k < i + 1; k++)	//2*2模版
			{
				for (l = j - 1; l < j + 1; l++)
				{
					// 防止内存溢出
					if (k >= 0  && l >= 0 && k < lWidth && l < lHeight)
					{
						// 检测模版
						if (k == i - 1 && l == j - 1)
							g_v = 1;
						else if (k == i && l == j)
							g_v = -1;
						else
							g_v = 0;

						if(k == i - 1 && l == j)
							g_h = -1;
						else if (k == i && l == j - 1)
							g_h = 1;
						else
							g_h = 0;
						
						v_r_v += *(lpDIBBits + l * lLineBytes + k * nChannel) * g_v;
						v_r_h += *(lpDIBBits + l * lLineBytes + k * nChannel) * g_h;
						v_g_v += *(lpDIBBits + l * lLineBytes + k * nChannel + 1) * g_v;
						v_g_h += *(lpDIBBits + l * lLineBytes + k * nChannel + 1) * g_h;
						v_b_v += *(lpDIBBits + l * lLineBytes + k * nChannel + 2) * g_v;
						v_b_h += *(lpDIBBits + l * lLineBytes + k * nChannel + 2) * g_h;
					}
				}
			}
			m_temp[j * lLineBytes + i * nChannel] = (int)sqrt(1.0*v_r_v * v_r_v + v_r_h * v_r_h);
			m_temp[j * lLineBytes + i * nChannel + 1] = (int)sqrt(1.0*v_g_v * v_g_v + v_g_h * v_g_h);
			m_temp[j * lLineBytes + i * nChannel + 2] = (int)sqrt(1.0*v_b_v * v_b_v + v_b_h * v_b_h);
		}
	}

	// 回存处理结果到DIB
	for(i = 0; i < lLineBytes * lHeight; i++)
		*(lpDIBBits + i) = m_temp[i];

	// 释放内存
	delete[] m_temp;

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   Laplacian()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *	 对图像进行拉普拉斯算子的边缘检测
 *
 ************************************************************************/
void CSplitProc::Laplacian()
{
	
	LONG i;
	LONG j;
	LONG k;
	LONG l;
	
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();

	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// 不能用char，也不能用::strcpy()
	BYTE* m_temp;
	m_temp=new BYTE [lLineBytes * lHeight];

	// 中间变量
	int v_r, v_g, v_b, p_g;

	// 检测模版
	int g[9]={-1, -1, -1, -1, 8, -1, -1, -1, -1};

	// 复制图象数据到中间缓存
	for (i = 0; i < lLineBytes * lHeight; i++)
		m_temp[i] = *(lpDIBBits + i);

	// 3X3 模版
	for (i = 0; i < lWidth; i++)		//被处理像素在i列
	{
		for (j = 0; j < lHeight; j++)	//被处理像素在j行
		{
			v_r = v_g = v_b = p_g = 0;
	
			for (k = i - 1; k < i + 2; k++)	//3*3模版
			{
				for (l = j - 1; l < j + 2; l++)
				{
					// 防止内存溢出
					if (k >= 0  && l >= 0 && k < lWidth && l < lHeight)
					{
							v_r += *(lpDIBBits + l * lLineBytes + k * nChannel) * g[p_g];
							v_g += *(lpDIBBits + l * lLineBytes + k * nChannel + 1) * g[p_g];
							v_b += *(lpDIBBits + l * lLineBytes + k * nChannel + 2) * g[p_g];
							p_g++;
					}
				}
			}
			
			if (v_r < 0)
				v_r = 0;
			if (v_g < 0)
				v_g = 0;
			if (v_b < 0)
				v_b = 0;

			m_temp[j * lLineBytes + i * nChannel] = v_r;
			m_temp[j * lLineBytes + i * nChannel + 1] = v_g;
			m_temp[j * lLineBytes + i * nChannel + 2] = v_b;
		}
	}

	// 回存处理结果到DIB
	for(i = 0; i < lLineBytes * lHeight; i++)
		*(lpDIBBits + i) = m_temp[i];

	// 释放内存
	delete[] m_temp;

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   GuassLaplacian()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *	 对图像进行高斯——拉普拉斯算子的边缘检测
 *
 ************************************************************************/
void CSplitProc::GuassLaplacian()
{
	
	LONG i;
	LONG j;
	LONG k;
	LONG l;
	
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();

	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// 不能用char，也不能用::strcpy()
	BYTE* m_temp;
	m_temp=new BYTE [lLineBytes * lHeight];

	// 中间变量
	int v_r, v_g, v_b, p_g;

	// 检测模版
	int g[25]={-2,-4,-4,-4,-2,
			   -4, 0, 8, 0,-4,
			   -4, 8,24, 8,-4,
			   -4, 0, 8, 0,-4,
			   -2,-4,-4,-4,-2};

	// 复制图象数据到中间缓存
	for (i = 0; i < lLineBytes * lHeight; i++)
		m_temp[i] = *(lpDIBBits + i);

	// 5X5 模版
	for (i = 0; i < lWidth; i++)		//被处理像素在i列
	{
		for (j = 0; j < lHeight; j++)	//被处理像素在j行
		{
			v_r = v_g = v_b = p_g = 0;
	
			for (k = i - 2; k < i + 3; k++)	//5*5模版
			{
				for (l = j - 2; l < j + 3; l++)
				{
					// 防止内存溢出
					if (k >= 0  && l >= 0 && k < lWidth && l < lHeight)
					{
							v_r += *(lpDIBBits + l * lLineBytes + k * nChannel) * g[p_g];
							v_g += *(lpDIBBits + l * lLineBytes + k * nChannel + 1) * g[p_g];
							v_b += *(lpDIBBits + l * lLineBytes + k * nChannel + 2) * g[p_g];
							p_g++;
					}
				}
			}
			
			if (v_r < 0)
				v_r = 0;
			if (v_g < 0)
				v_g = 0;
			if (v_b < 0)
				v_b = 0;

			m_temp[j * lLineBytes + i * nChannel] = v_r;
			m_temp[j * lLineBytes + i * nChannel + 1] = v_g;
			m_temp[j * lLineBytes + i * nChannel + 2] =v_b;
		}
	}

	// 回存处理结果到DIB
	for(i = 0; i < lLineBytes * lHeight; i++)
		*(lpDIBBits + i) = m_temp[i];

	// 释放内存
	delete[] m_temp;

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 * 1  Hough()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数将半径40个象素的圆周从原始图象中提取出来。
 *
 ************************************************************************/
void CSplitProc::Hough()
{
	
	LONG i;
	LONG j;
	LONG k;
	
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();

	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();
	
	// 不能用char，也不能用::strcpy()
	BYTE* m_temp;
	m_temp = new BYTE [lLineBytes * lHeight];

	// 中间变量
	int cx, cy1, cy2;

	memset(m_temp, 0, sizeof(m_temp));

	// 以离散点为圆心画圆
	for (i = 0; i < lWidth; i++)		//被处理像素在i列
	{
		for (j = 0; j < lHeight; j++)	//被处理像素在j行
		{
			if (*(lpDIBBits + j * lLineBytes + i * nChannel) == 0)
			{
				for (k = -40; k < 40; k++)
				{
					cx = k;
					cy1 = (int)sqrt(1.0*40 * 40 - cx * cx);
					cy2 = (int)sqrt(1.0*40 * 40 - cx * cx) * (-1);
					cx += i;
					cy1 += j;
					cy2 += j;
					if (cx >= 0 && cx < lWidth && cy1 >= 0 && cy1 < lHeight)
					{
						m_temp[cy1 * lLineBytes + cx * nChannel]++;
						m_temp[cy1 * lLineBytes + cx * nChannel + 1]++;
						m_temp[cy1 * lLineBytes + cx * nChannel + 2]++;
					}
					if (cx >= 0 && cx < lWidth && cy2 >= 0 && cy2 < lHeight)
					{
						m_temp[cy2 * lLineBytes + cx * nChannel]++;
						m_temp[cy2 * lLineBytes + cx * nChannel + 1]++;
						m_temp[cy2 * lLineBytes + cx * nChannel + 2]++;
					}
				}
			}
		}
	}

	// 显示中间统计结果
	for (i = 0; i <lLineBytes * lHeight; i++)
		*(lpDIBBits + i) = m_temp[i];

	// 通过求最大值计算出圆心位置
	int nMaxValue = 0;
	int nMaxX, nMaxY;
	
	for (i = 0; i < lWidth; i++)		//被处理像素在i列
	{
		for (j = 0; j < lHeight; j++)	//被处理像素在j行
		{
			if (nMaxValue < m_temp[j * lLineBytes + i * nChannel])
			{
				nMaxValue = m_temp[j * lLineBytes + i * nChannel];
				nMaxX = i;
				nMaxY = j;
			}
		}
	}

	// 显示检测结果
	for (i = 0; i <lLineBytes * lHeight; i++)
		m_temp[i] = 255;

	for (k = -40; k < 40; k++)
	{
		cx = k;
		cy1 = (int)sqrt(1.0*40 * 40 - cx * cx);
		cy2 = (int)sqrt(1.0*40 * 40 - cx * cx) * (-1);
		cx += nMaxX;
		cy1 += nMaxY;
		cy2 += nMaxY;

		if (cx >= 0 && cx < lWidth && cy1 >= 0 && cy1 < lHeight)
		{
			m_temp[cy1 * lLineBytes + cx * nChannel] = 0;
			m_temp[cy1 * lLineBytes + cx * nChannel + 1] = 0;
			m_temp[cy1 * lLineBytes + cx * nChannel + 2] = 0;
		}
		if (cx >= 0 && cx < lWidth && cy2 >= 0 && cy2 < lHeight)
		{
			m_temp[cy2 * lLineBytes + cx * nChannel] = 0;
			m_temp[cy2 * lLineBytes + cx * nChannel + 1] = 0;
			m_temp[cy2 * lLineBytes + cx * nChannel + 2] = 0;
		}
	}
	
	// 显示检测结果
	for (i = 0; i <lLineBytes * lHeight; i++)
		*(lpDIBBits + i) = m_temp[i];

	// 释放内存
	delete[] m_temp;

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   Kirsch()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数使用Kirsch算子对图象进行边缘检测
 *
 ************************************************************************/
void CSplitProc::Kirsch()
{
	
	LONG i;
	LONG j;
	LONG k;
	LONG l;
	
	BYTE* lpNewDIBBits1;
	BYTE* lpNewDIBBits2;
	BYTE* lpDst1;
	BYTE* lpDst2;

	HLOCAL hNewDIBBits1;
	HLOCAL hNewDIBBits2;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();

	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// 暂时分配内存，以保存新图象
	hNewDIBBits1 = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits1 == NULL)
	{
		// 分配内存失败
		return;
	}

	// 锁定内存
	lpNewDIBBits1 = (BYTE*)LocalLock(hNewDIBBits1);

	// 暂时分配内存，以保存新图象
	hNewDIBBits2 = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits1 == NULL)
	{
		// 分配内存失败
		return;
	}

	// 锁定内存
	lpNewDIBBits2 = (BYTE*)LocalLock(hNewDIBBits2);

	// 复制图象到缓存
	lpDst1 = lpNewDIBBits1;
	lpDst2 = lpNewDIBBits2;

	memcpy(lpNewDIBBits1, lpDIBBits, lLineBytes * lHeight);
	memcpy(lpNewDIBBits2, lpDIBBits, lLineBytes * lHeight);

	// 不能用char，也不能用::strcpy()
	BYTE* m_temp;
	m_temp = new BYTE [lLineBytes * lHeight];

	// Kirsch算子的八个方向上的掩模模版
	int g[8][9] = { { 5, 5, 5, -3, 0, -3, -3, -3, -3},
					{ -3, 5, 5, -3, 0, 5, -3, -3, -3},
					{ -3, -3, 5, -3, 0, 5, -3, -3, 5},
					{ -3, -3, -3, -3, 0, 5, -3, 5, 5},
					{ -3, -3, -3, -3, 0, -3, 5, 5, 5},
					{ -3, -3, -3, 5, 0, -3, 5, 5, -3},
					{ 5, -3, -3, 5, 0, -3, 5, -3, -3},
					{ 5, 5, -3, 5, 0, -3, -3, -3, -3}};
	
	// 中间变量
	int v_r, v_g, v_b, p_g;

    // 复制图象数据到中间缓存
	memcpy(m_temp, lpNewDIBBits1, lLineBytes * lHeight);

	// 3X3 模版
	for (i = 0; i < lWidth; i++)		//被处理像素在i列
	{
		for (j = 0; j < lHeight; j++)	//被处理像素在j行
		{
			v_r = v_g = v_b = p_g = 0;
			
			for (k = i - 1; k < i + 2; k++)	//3*3模版
			{
				for (l = j - 1; l < j + 2; l++)
				{
					// 防止内存溢出
					if (k >= 0  && l >= 0 && k < lWidth && l < lHeight)
					{
						v_r += *(lpNewDIBBits1 + l * lLineBytes + k * nChannel) * g[0][p_g];
						v_g += *(lpNewDIBBits1 + l * lLineBytes + k * nChannel + 1) * g[0][p_g];
						v_b += *(lpNewDIBBits1 + l * lLineBytes + k * nChannel + 2) * g[0][p_g];
						p_g++;
					}
				}
			}

			v_r /= 9;
			v_g /= 9;
			v_b /= 9;

			if (v_r < 0)
				v_r = 0;
			if (v_g < 0)
				v_g = 0;
			if (v_b < 0)
				v_b = 0;

			m_temp[j * lLineBytes + i * nChannel] = v_r;
			m_temp[j * lLineBytes + i * nChannel + 1] = v_g;
			m_temp[j * lLineBytes + i * nChannel + 2] = v_b;
		}
	}

	// 回存处理结果到DIB
	memcpy(lpNewDIBBits1, m_temp, lLineBytes * lHeight);

	// 第一图象缓存同第二~第八图象缓存数据依次比较，将
	// 大值保存在第一图象缓存。
	for (int loop = 1; loop < 8; loop++)
	{
		// 复制图象数据到中间缓存
		memcpy(m_temp, lpNewDIBBits2, lLineBytes * lHeight);
	
		// 3X3 模版
		for (i = 0; i < lWidth; i++)		//被处理像素在i列
		{
			for (j = 0; j < lHeight; j++)	//被处理像素在j行
			{
				v_r = v_g = v_b = p_g = 0;
			
				for (k = i - 1; k < i + 2; k++)	//3*3模版
				{
					for (l = j - 1; l < j + 2; l++)
					{
						// 防止内存溢出
						if (k >= 0  && l >= 0 && k < lWidth && l < lHeight)
						{
							v_r += *(lpNewDIBBits2 + l * lLineBytes + k * nChannel) * g[loop][p_g];
							v_g += *(lpNewDIBBits2 + l * lLineBytes + k * nChannel + 1) * g[loop][p_g];
							v_b += *(lpNewDIBBits2 + l * lLineBytes + k * nChannel + 2) * g[loop][p_g];
							p_g++;
						}
					}
				}

				v_r /= 9;
				v_g /= 9;
				v_b /= 9;

				if (v_r < 0)
					v_r = 0;
				if (v_g < 0)
					v_g = 0;
				if (v_b < 0)
					v_b = 0;

				m_temp[j * lLineBytes + i * nChannel] = v_r;
				m_temp[j * lLineBytes + i * nChannel + 1] = v_g;
				m_temp[j * lLineBytes + i * nChannel + 2] = v_b;
			}
		}

		// 回存处理结果到DIB
		memcpy(lpNewDIBBits2, m_temp, lLineBytes * lHeight);

		// 提取两缓存对应象素的最大值
		for (j = 0; j < lHeight; j++)
		{
			for (i = 0; i < lLineBytes; i++)
			{
				lpDst1 = (BYTE*)lpNewDIBBits1 + lLineBytes * j + i;
				lpDst2 = (BYTE*)lpNewDIBBits2 + lLineBytes * j + i;
				if (*lpDst2 > *lpDst1)
					*lpDst1 = * lpDst2;
			}
		}
	}

	// 将检测结果复制到原图象
	memcpy(lpDIBBits, lpNewDIBBits1, lLineBytes * lHeight); 

	// 释放内存、解除锁定
	LocalUnlock(hNewDIBBits1);
	LocalFree(hNewDIBBits1);
	LocalUnlock(hNewDIBBits2);
	LocalFree(hNewDIBBits2);
	
	delete[] m_temp;
	
	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   ContourDIB()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *   COLORREF clrSeed_I - 种子点一的RGB值
 *   COLORREF clrSeed_II- 种子点二的RGB值
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图象进行轮廓提取
 *
 ************************************************************************/
void CSplitProc::ContourDIB(COLORREF clrSeed_I,COLORREF clrSeed_II)
{
	
	LONG i;
	LONG j;
	LONG k;
	LONG s;

	// 计算种子点一的灰度值
	BYTE R_I = GetRValue(clrSeed_I);
	BYTE G_I = GetGValue(clrSeed_I);
	BYTE B_I = GetBValue(clrSeed_I);
	
	// 计算种子点二的灰度值
	BYTE R_II = GetRValue(clrSeed_II);
	BYTE G_II = GetGValue(clrSeed_II);
	BYTE B_II = GetBValue(clrSeed_II);

	BYTE R, G, B;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();

	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// 不能用char，也不能用::strcpy()
	BYTE* m_temp;
	m_temp = new BYTE [lLineBytes * lHeight];

	// 复制图象数据到中间缓存
	for (i = 0; i < lLineBytes * lHeight; i++)
		m_temp[i] = *(lpDIBBits + i);

	// 对各像素进行灰度转换
	for (i = 0; i < lWidth; i++)		//被处理像素在i列
	{
		for (j = 0; j < lHeight; j++)	//被处理像素在j行
		{
			int I, II;
			I = II = 0;
			for (k = i - 1; k < i + 2; k++)	//3*3模版
			{
				for (s = j - 1; s < j + 2; s++)
				{
					// 防止内存溢出
					if (k >= 0  && s >= 0 && k < lWidth && s < lHeight)
					{
						// 获取各颜色分量
						R = *((BYTE *)lpDIBBits + s * lLineBytes + k * nChannel);
						G = *((BYTE *)lpDIBBits + s * lLineBytes + k * nChannel + 1);
						B = *((BYTE *)lpDIBBits + s * lLineBytes + k * nChannel + 2);
						
						// 判断是否在期望灰度范围内
						if (abs(B - B_I) < 10 && abs(G - B_I) < 10 && abs(R - R_I) < 10)
							I++;
						if (abs(B - B_II) < 10 && abs(G - B_II) < 10 && abs(R - R_II) < 10)
							II++;			
					}
				}
			}
			// 如果周围所有象素灰度相同，当前点一定非边界，可以去掉（设为白色）
			if (I == 9 || II == 9)
			{
				m_temp[j * lLineBytes + i * nChannel] = 255;
				m_temp[j * lLineBytes + i * nChannel + 1] = 255;
				m_temp[j * lLineBytes + i * nChannel + 2] = 255;
			}
		}
	}

	// 回存处理结果到DIB
	for(i = 0; i < lLineBytes * lHeight; i++)
		*(lpDIBBits + i) = m_temp[i];

	EndWaitCursor();

	SAFE_DELETE(m_temp);
}

/*************************************************************************
 *
 * 函数名称：
 *   TraceDIB()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *   COLORREF clrSeed	- 种子的RGB值 
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图象进行边界跟踪
 *
 ************************************************************************/
void CSplitProc::TraceDIB(COLORREF clrSeed)
{
	
	LONG i;
	LONG j;

	// 指向源图像的指针
	BYTE* lpSrc;
	
	// 指向缓存图像的指针	
	BYTE* lpDst;

	//是否找到起始点及回到起始点
	bool bFindStartPoint;

	//是否扫描到一个边界点
	bool bFindPoint;

	//起始边界点与当前边界点
	CPoint StartPoint, CurrentPoint;

	//八个方向和起始扫描方向
	int Direction[8][2] = { {-1, 1}, {0, 1}, {1, 1}, {1, 0},
							{1, -1}, {0, -1}, {-1, -1}, {-1, 0}};
	int BeginDirect;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();

	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// 暂时分配内存，以保存新图像
	HLOCAL	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);

	if (hNewDIBBits == NULL)  
	{
		// 分配内存失败
		return;
	}
	
	// 锁定内存
	BYTE*	lpNewDIBBits = (BYTE*)LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为255
	lpDst = (BYTE*)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes * lHeight);
	
	//先找到最左上方的边界点
	bFindStartPoint = false;

	for (j = 0; j < lHeight && !bFindStartPoint; j++)
	{
		for (i = 0; i < lWidth && !bFindStartPoint; i++)
		{
			// 指向源图像倒数第j行，第i个象素的指针			
			lpSrc = (BYTE*)(lpDIBBits + lLineBytes * j + i * nChannel);
			if(abs(*lpSrc - GetRValue(clrSeed)) < 10 && abs(*(lpSrc + 1) - GetGValue(clrSeed)) < 10 && abs(*(lpSrc + 2) - GetBValue(clrSeed)) < 10)
			{
				bFindStartPoint = true;

				StartPoint.y = j;
				StartPoint.x = i;

				// 指向目标图像倒数第j行，第i个象素的指针			
				lpDst = (BYTE*)(lpNewDIBBits + lLineBytes * j + i * nChannel);	
				*lpDst = (BYTE) 0;
				*(lpDst + 1) = (BYTE) 0;
				*(lpDst + 2) = (BYTE) 0;
			}		
		}
	}

	//由于起始点是在左下方，故起始扫描沿左上方向
	BeginDirect = 0;
	
	//跟踪边界
	bFindStartPoint = false;
	
	//从初始点开始扫描 
	CurrentPoint.y = StartPoint.y;
	CurrentPoint.x = StartPoint.x;
	
	while (!bFindStartPoint)
	{
		bFindPoint = false;
		while (!bFindPoint)
		{
			if ((CurrentPoint.y + Direction[BeginDirect][1]) >= 0
				&& (CurrentPoint.x + Direction[BeginDirect][0]) >= 0
				&& (CurrentPoint.y + Direction[BeginDirect][1]) < lHeight
				&& (CurrentPoint.x + Direction[BeginDirect][0]) < lWidth)
			{

				//沿扫描方向查看一个像素
				lpSrc = (BYTE*)(lpDIBBits + lLineBytes * (CurrentPoint.y + Direction[BeginDirect][1])
					+ (CurrentPoint.x + Direction[BeginDirect][0]) * 3);
			
				if (abs(*lpSrc - GetRValue(clrSeed)) < 10 && abs(*(lpSrc + 1) - GetGValue(clrSeed)) < 10 && abs(*(lpSrc + 2) - GetBValue(clrSeed)) < 10)
				{
					bFindPoint = true;
					CurrentPoint.y = CurrentPoint.y + Direction[BeginDirect][1];
					CurrentPoint.x = CurrentPoint.x + Direction[BeginDirect][0];
					if (CurrentPoint.y == StartPoint.y && CurrentPoint.x == StartPoint.x)
						bFindStartPoint = true;
					lpDst = (BYTE*)lpNewDIBBits + lLineBytes * CurrentPoint.y + CurrentPoint.x * 3;
					*lpDst = (BYTE) 0;
					*(lpDst + 1) = (BYTE) 0;
					*(lpDst + 2) = (BYTE) 0;
		
					//扫描的方向逆时针旋转两格
					BeginDirect--;
					if(BeginDirect == -1)
						BeginDirect = 7;
					BeginDirect--;
					if(BeginDirect == -1)
					BeginDirect = 7;
				}
				else
				{
					//扫描方向顺时针旋转一格
					BeginDirect++;
					if(BeginDirect == 8)
						BeginDirect = 0;
				}
			}
			else
			{
				bFindPoint = true;
				bFindStartPoint = true;
			}
		}
	}

	// 复制腐蚀后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);

	// 释放内存
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   Threshold()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *   int T				- 阀值
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图象进行阀值化区域分割处理
 *
 ************************************************************************/
void CSplitProc::Threshold(int *nNY)
{
	
	LONG i;
	LONG j;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// 迭代阀值
	int T1, T2;
	T1 = 127;
	T2 = 0;

	// 临时变量
	int Temp0, Temp1, Temp2, Temp3;
	Temp0 = Temp1 = Temp2 = Temp3 = 0;

	while (true)
	{
		// 计算下一个迭代阀值
		for (i = 0; i < T1 + 1; i++)
		{
			Temp0 += nNY[i] * i;
			Temp1 += nNY[i];
		}
		for (i = T1 + 1; i < 256; i++)
		{
			Temp2 += nNY[i] * i;
			Temp3 += nNY[i];
		}
		T2 = (Temp0 / Temp1 + Temp2 / Temp3) / 2;

		// 看迭代结果是否已收敛
		if (T1 == T2)
			break;
		else
			T1 = T2;
	}

	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i ++)
	{
		for (j = 0; j < lLineBytes; j ++)
		{
			// 读取像素R分量
			BYTE R = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			
			// 判断R分量是否超出范围
			if (R < T1)
				R = 0;
			else
				R = 255;
			
			// 回写处理完的R分量
			*((BYTE *)lpDIBBits + lLineBytes * i + j) = R;
			j++;

			// 读取像素G分量
			BYTE G = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			
			// 判断G分量是否超出范围
			if (G < T1)
				G = 0;
			else 
				G = 255;
			
			// 回写处理完的G分量
			*((BYTE *)lpDIBBits + lLineBytes * i + j) = G;
			j++;

			// 读取像素B分量
			BYTE B = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			
			// 判断B分量是否超出范围
			if (B < T1)
				B = 0;
			else
				B = 255;
			
			// 回写处理完的B分量
			*((BYTE *)lpDIBBits + lLineBytes * i + j) = B;
		}
	}

	EndWaitCursor();
}


