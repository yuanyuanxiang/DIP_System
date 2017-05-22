// ************************************************************************
//  文件名：SpaceFilter.cpp
//
//  图像空域滤波处理函数库：
//
//  MakeEmboss()			- 浮雕化处理
//	PixelFilter()			- 对图象进行小区域模版滤波
//	StencilFilter()			- 对图象进行模版滤波
//  MedianFilter()			- 对图象进行中值滤波（非线形平滑滤波）
//	OverRun()				- 对图象进行超限模版滤波处理
//
// ************************************************************************

#include "stdafx.h"
#include "dip_system.h"
#include "SpaceFilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX(a,b) ((a)>(b))?(a):(b) 
#define MIN(a,b) ((a)<(b))?(a):(b)
#define BOUND(c,i,j)  (MAX((i),MIN((c),(j))))

/////////////////////////////////////////////////////////////////////////////
// CSpaceFilter

CSpaceFilter::CSpaceFilter()
{
}

CSpaceFilter::CSpaceFilter(CDIB* pDIB)
{
	m_pDIB = pDIB;
}

CSpaceFilter::~CSpaceFilter()
{
}

BEGIN_MESSAGE_MAP(CSpaceFilter, CWnd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpaceFilter message handlers

/*************************************************************************
 *
 * 函数名称：
 *   PixelFilter()
 *
 * 参数:
 *   BYTE* lpDst			- 目标图象数据
 *   BYTE* lpSrc			- 源图象数据
 *   LPKERNEL lpKernel		- 指向KERNEL结构对象的指针
 *   DWORD dwWidthBytes		- 一行图象所占字节数
 *   BOOL bEmboss			- 是否需要增强处理
 *
 * 返回值:
 *   无返回值
 *
 * 说明:
 *   该函数将对图象进行小区域模版滤波
 *
 ************************************************************************/
void CSpaceFilter::PixelFilter(BYTE* lpDst, BYTE* lpSrc, LPKERNEL lpKernel, DWORD dwWidthBytes, BOOL bEmboss)
{
	
	LONG   i, j;

	// 临时变量
	LONG temp, R, G, B;
	BYTE *lpTemp1, *lpTemp2;

	// 初始化
	R = G = B = 0;

	// lpTemp1 = lpSrc + dwWidthBytes - 3
	lpTemp1 = lpSrc + dwWidthBytes - lpKernel->Dimention; 
	
	// 对小区域进行模版空域滤波
	for(i = 0; i < lpKernel->Dimention; i++)
	{
		lpTemp2 = lpTemp1;
		for(j = 0; j < lpKernel->Dimention; j++)
		{
			temp = lpKernel->Element[i][j];
			B += (*lpTemp2++) * temp;
			G += (*lpTemp2++) * temp;
			R += (*lpTemp2++) * temp;
		}
		lpTemp1 += dwWidthBytes;
	}

	// 取模版除数
	temp = lpKernel->Divisor;

	if(temp != 1)
	{
		R /= temp;
		G /= temp;
		B /= temp;
	}

	// 如浮雕处理则需将背景色置为RGB(128,128,128)
	if(bEmboss)
	{
		R += 128;
		G += 128;
		B += 128;
	}

	// 防止超界
	R = BOUND(R,0,255);
	G = BOUND(G,0,255);
	B = BOUND(B,0,255);

	// 将处理后结果保存到DIB
	*lpDst++ = (BYTE)B;
	*lpDst++ = (BYTE)G;
	*lpDst   = (BYTE)R;
}

/*************************************************************************
 *
 * 函数名称：
 *   MakeEmboss()
 *
 * 参数:
 *   int  nEmbossMode   - 采取的浮雕化模式
 *
 * 返回值:
 *   HDIB				- 新的DIB
 *
 * 说明:
 *   该函数将彩色位图进行浮雕化处理
 *
 ************************************************************************/
void CSpaceFilter::MakeEmboss(int nEmbossMode)
{
	
	LONG i, j, k;

	// 获取指向KERNEL结构对象的指针
	LPKERNEL  lpKernel = &Emboss[nEmbossMode];
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();

	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLength = m_pDIB->Length();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = (BYTE*)m_pDIB->FindAddress();
	BYTE* lpNewDIB = new BYTE[lLength];

	// 临时变量
    BYTE *lpSrcTemp, *lpDstTemp;
	BYTE *lpSrcPixel, *lpDstPixel;

	// 图象第一行地址
	lpSrcTemp = lpDIBBits;
	lpDstTemp = lpNewDIB;

	// 对整幅图象实施小区域空域滤波处理
	for(i = 0; i< lHeight - 3; i++)
	{
		lpSrcPixel = lpSrcTemp;
		lpDstPixel = lpDstTemp;
		
		// 复制每行的第一个像素;
		for(k = 0; k < 3; k++)
		{
			*lpDstPixel = *lpSrcPixel;
			lpDstPixel++;
			lpSrcPixel++;
		}

		// 调用小区域滤波函数对各行数据进行处理
		for(j = 0; j < lWidth - 2; j++)
		{
			PixelFilter(lpDstPixel, lpSrcPixel, lpKernel, lLineBytes, TRUE);
			lpSrcPixel += 3;
			lpDstPixel += 3;
		}

		// 复制每行的最后一个像素;
		for(k = 0; k < 3; k++)
		{
			*lpDstPixel = *lpSrcPixel;
			lpDstPixel++;
			lpSrcPixel++;
		}

		// 修正指针
		lpSrcTemp += lLineBytes;
		lpDstTemp += lLineBytes;
	}

	// 复制图像的最后一行;
	memcpy(lpDstTemp, lpSrcTemp, lLineBytes);

	// 复制替换原图像
	memcpy(lpDIBBits, lpNewDIB, lLength);

	delete[] lpNewDIB;

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   StencilFilter()
 *
 * 参数:
 *	 LPKERNEL lpKernel	- 获取指向KERNEL结构对象的指针
 *
 * 返回值:
 *	 无返回值
 *
 * 说明:
 *   该函数将彩色位图进行模版滤波处理
 *
 ************************************************************************/
void CSpaceFilter::StencilFilter(LPKERNEL  lpKernel)
{
	
	LONG i;
	LONG j;
	LONG k;
	LONG l;

	// 临时变量
	LONG R, G, B;
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	LONG lLength = m_pDIB->Length();
	BYTE* lpDIBBits = m_pDIB->FindAddress();

	// 申请并分配中间缓存
	BYTE* temp = new BYTE[lLength];

	// 复制图象数据到中间缓存
	memcpy(temp, lpDIBBits, lLength);
	
	// 模版滤波
	for (i = 0; i < lWidth; i++)			//被处理像素在i列
	{
		for (j = 0; j < lHeight; j++)		//被处理像素在j行
		{
			// 计数清零
			R = G = B = 0;

			// 进行小区域模版滤波
			for (k = i - (int)(lpKernel->Dimention / 2); k < i + (int)(lpKernel->Dimention / 2) + 1; k++)
			{
				for(l = j - (int)(lpKernel->Dimention / 2); l < j + (int)(lpKernel->Dimention / 2) + 1; l++)
				{
					// 防止内存溢出
					if (k >= 0 && l >= 0 && k < lWidth && l < lHeight)
					{
						BYTE TR = *((BYTE *)lpDIBBits + l * lLineBytes + k * nChannel);
						R += lpKernel->Element[k - i + (int)(lpKernel->Dimention / 2)][l - j + (int)(lpKernel->Dimention / 2)] * TR;

						BYTE TG = *((BYTE *)lpDIBBits + l * lLineBytes + k * nChannel + 1);
						G += lpKernel->Element[k - i + (int)(lpKernel->Dimention / 2)][l - j + (int)(lpKernel->Dimention / 2)] * TG;

						BYTE TB = *((BYTE *)lpDIBBits + l * lLineBytes + k * nChannel + 2);
						B += lpKernel->Element[k - i + (int)(lpKernel->Dimention / 2)][l - j + (int)(lpKernel->Dimention / 2)] * TB;
					}
				}
			}

			// 进行模版平均
			R /= lpKernel->Divisor;
			G /= lpKernel->Divisor;
			B /= lpKernel->Divisor;

			// 存储计算结果到中间缓存
			*(temp + j * lLineBytes + i * nChannel)     = (BYTE)R;
			*(temp + j * lLineBytes + i * nChannel + 1) = (BYTE)G;
			*(temp + j * lLineBytes + i * nChannel + 2) = (BYTE)B;
		}
	}

	// 将转换后的中间缓存数据回存到DIB
	memcpy(lpDIBBits, temp, lLength);

	delete [] temp;
	
	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   MedianFilter()
 *
 * 参数:
 *
 * 返回值:
 *   无返回值
 *
 * 说明:
 *   该函数将彩色位图进行中值滤波处理
 *
 ************************************************************************/
void CSpaceFilter::MedianFilter()
{
	
	LONG i;
	LONG j;
	LONG k;
	LONG l;

	// 掩码模版
	BYTE mask_r[9];
	BYTE mask_g[9];
	BYTE mask_b[9];
	COLORREF mask[9];

	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	LONG lLength = m_pDIB->Length();
	BYTE* lpDIBBits = m_pDIB->FindAddress();

	// 申请并分配中间缓存
	BYTE* temp = new BYTE[lLength];

	// 复制图象数据到中间缓存
	memcpy(temp, lpDIBBits, lLength);

	// 模版滤波
	for (i = 0; i < lWidth; i++)			//被处理像素在i列
	{
		for (j = 0; j < lHeight; j++)		//被处理像素在j行
		{
			// 索引
			int	id = 0;

			// 进行小区域模版滤波
			for (k = i - 1; k < i + 2; k++)
			{
				for(l = j - 1; l < j + 2; l++)
				{
					// 防止内存溢出
					if (k >= 0 && l >= 0 && k < lWidth && l < lHeight)
					{
						mask_r[id] = *((BYTE *)lpDIBBits + l * lLineBytes + k * nChannel);
						mask_g[id] = *((BYTE *)lpDIBBits + l * lLineBytes + k * nChannel + 1);
						mask_b[id] = *((BYTE *)lpDIBBits + l * lLineBytes + k * nChannel + 2);
				
						mask[id] = RGB(mask_r[id], mask_g[id], mask_b[id]);
						id++;
					}
				}
			}
			
			// 中间变量
			BYTE T;
			
			// 冒泡排序法
			for (k = 0; k < 8; k++)		
			{
				for (l = 8; l > k; l--)					
				{
					if (mask_r[l] < mask_r[l - 1])
					{
						T = mask_r[l];
						mask_r[l] = mask_r[l - 1];
						mask_r[l - 1] = T;
					}
					if (mask_g[l] < mask_g[l - 1])
					{
						T = mask_g[l];
						mask_g[l] = mask_g[l - 1];
						mask_g[l - 1] = T;
					}
					if (mask_b[l] < mask_b[l - 1])
					{
						T = mask_b[l];
						mask_b[l] = mask_b[l - 1];
						mask_b[l - 1] = T;
					}
				}
			}
			temp[j * lLineBytes + i * nChannel] = mask_r[4];
			temp[j * lLineBytes + i * nChannel + 1] = mask_g[4];
			temp[j * lLineBytes + i * nChannel + 2] = mask_b[4];
		}
	}

	// 将转换后的中间缓存数据回存到DIB
	memcpy(lpDIBBits, temp, lLength);
	
	delete [] temp;

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   OverRun()
 *
 * 参数:
 *	 LPKERNEL lpKernel	- 获取指向KERNEL结构对象的指针
 *   int T				- 超限阀值
 *
 * 返回值:
 *	 无返回值
 *
 * 说明:
 *   该函数位图进行超限模版滤波处理
 *	
 ************************************************************************/
void CSpaceFilter::OverRun(LPKERNEL lpKernel, int T)
{
	
	LONG i;
	LONG j;
	LONG k;
	LONG l;

	// 临时变量
	LONG R, G, B;
	LONG MR, MG, MB;
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	LONG lLength = m_pDIB->Length();
	BYTE* lpDIBBits = m_pDIB->FindAddress();

	// 申请并分配中间缓存
	BYTE* temp = new BYTE[lLength];

	// 复制图象数据到中间缓存
	memcpy(temp, lpDIBBits, lLength);

	// 模版滤波
	for (i = 0; i < lWidth; i++)			//被处理像素在i列
	{
		for (j = 0; j < lHeight; j++)		//被处理像素在j行
		{
			// 计数清零
			R = G = B = 0;

			// 进行小区域模版滤波
			for (k = i - (int)(lpKernel->Dimention / 2); k < i + (int)(lpKernel->Dimention / 2) + 1; k++)
			{
				for(l = j - (int)(lpKernel->Dimention / 2); l < j + (int)(lpKernel->Dimention / 2) + 1; l++)
				{
					// 防止内存溢出
					if (k >= 0 && l >= 0 && k < lWidth && l < lHeight)
					{
						BYTE TR = *((BYTE *)lpDIBBits + l * lLineBytes + k * nChannel);
						BYTE TG = *((BYTE *)lpDIBBits + l * lLineBytes + k * nChannel + 1);
						BYTE TB = *((BYTE *)lpDIBBits + l * lLineBytes + k * nChannel + 2);
						
						if (k == i && l == j)
						{
							MR = TR;
							MG = TG;
							MB = TB;
						}
						
						R += lpKernel->Element[k - i + (int)(lpKernel->Dimention / 2)][l - j + (int)(lpKernel->Dimention / 2)] * TR;
						G += lpKernel->Element[k - i + (int)(lpKernel->Dimention / 2)][l - j + (int)(lpKernel->Dimention / 2)] * TG;
						B += lpKernel->Element[k - i + (int)(lpKernel->Dimention / 2)][l - j + (int)(lpKernel->Dimention / 2)] * TB;
					}
				}
			}
			
			// 进行模版平均
			R /= lpKernel->Divisor;
			G /= lpKernel->Divisor;
			B /= lpKernel->Divisor;
			
			// 进行超闲限判断
			if (abs(MR - R) > T)
				MR = R;
			if (abs(MG - G) > T)
				MG = G;
			if (abs(MB - B) > T)
				MB = B;

			// 存储计算结果到中间缓存
			*(temp + j * lLineBytes + i * nChannel)     = (BYTE)MR;
			*(temp + j * lLineBytes + i * nChannel + 1) = (BYTE)MG;
			*(temp + j * lLineBytes + i * nChannel + 2) = (BYTE)MB;
		}
	}

	// 将转换后的中间缓存数据回存到DIB
	memcpy(lpDIBBits, temp, lLength);

	delete [] temp;

	EndWaitCursor();
}