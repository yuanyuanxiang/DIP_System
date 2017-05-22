// ************************************************************************
//  文件名：Morph.cpp
//
//   数学形态学函数库：
//
//  VErosion()				- 对图象进行垂直腐蚀
//  HErosion()				- 对图象进行水平腐蚀
//	VDilation()				- 对图象进行垂直膨胀
//	HDilation()				- 对图象进行水平膨胀
//	VHErosion()				- 对图象进行全方向腐蚀
//	VHDilation()			- 对图象进行全方向膨胀
//	OpenOperate()			- 对图象进行开运算处理
//	CloseOperate()			- 对图象进行闭运算处理
//	Thin()					- 对图象进行细化处理
//	Thick()					- 对图象进行粗化处理
//	MAT()					- 对图象进行中轴变换
//
// ************************************************************************

#include "stdafx.h"
#include "dip_system.h"
#include "Morph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMorph

CMorph::CMorph()
{
}

CMorph::CMorph(CDIB* pDIB)
{
	m_pDIB = pDIB;
}

CMorph::~CMorph()
{
}

BEGIN_MESSAGE_MAP(CMorph, CWnd)
	//{{AFX_MSG_MAP(CMorph)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMorph message handlers

/*************************************************************************
 *
 * 函数名称：
 *   VErosion()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图象进行垂直方向上的腐蚀
 *
 ************************************************************************/

void CMorph::VErosion()
{
	//循环变量
	LONG i;
	LONG j;
	LONG n;

	// 指向源图像的指针
	BYTE*	lpSrc;
	
	// 指向缓存图像的指针
	BYTE*	lpDst;

	// 指向缓存DIB图像的指针
	BYTE*	lpNewDIBBits;
	HLOCAL	hNewDIBBits;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);

	if (hNewDIBBits == NULL)
	{
		// 分配内存失败
		return;
	}
	
	// 锁定内存
	lpNewDIBBits = (BYTE*)LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为255
	lpDst = (BYTE*)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes * lHeight);

	//使用垂直方向的结构元素进行腐蚀
	for(i = 1; i < lHeight - 1; i++)
	{
		for(j = 0; j < lLineBytes; j += 3)
		{
			//由于使用3×1的结构元素，为防止越界，所以不处理最上边和最下边的两列像素
			// 指向源图像倒数第j行，第i个象素的指针			
			lpSrc = (BYTE *)(lpDIBBits + lLineBytes * i + j);

			// 指向目标图像倒数第j行，第i个象素的指针			
			lpDst = (BYTE *)(lpNewDIBBits + lLineBytes * i + j);

			//目标图像中的当前点先赋成黑色
			*lpDst = 0;
			*(lpDst + 1) = 0;
			*(lpDst + 2) = 0;

			// 如果源图像中(0,-1)、(0,0)、(0,1)三个点之一有白点，
			// 则将目标图像中的(0,0)点赋成白色
			for (n = 0; n < 3; n++)
			{
				if (*(lpSrc + (n - 1) * lLineBytes) > 128)
				{
					*lpDst = 255;
					*(lpDst + 1) = 255;
					*(lpDst + 2) = 255;
					break;
				}
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
 *   HErosion()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图象进行水平方向上的腐蚀
 *
 ************************************************************************/

void CMorph::HErosion()
{
	//循环变量
	LONG i;
	LONG j;
	LONG n;

	// 指向源图像的指针
	BYTE*	lpSrc;
	
	// 指向缓存图像的指针
	BYTE*	lpDst;

	// 指向缓存DIB图像的指针
	BYTE*	lpNewDIBBits;
	HLOCAL	hNewDIBBits;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);

	if (hNewDIBBits == NULL)
	{
		// 分配内存失败
		return;
	}
	
	// 锁定内存
	lpNewDIBBits = (BYTE*)LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为255
	lpDst = (BYTE*)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes * lHeight);

	// 使用水平方向的结构元素进行腐蚀
	for (i = 0; i < lHeight; i++)
	{
		for (j = 3; j < lLineBytes - 3; j += 3)
		{
			// 由于使用1×3的结构元素，为防止越界，所以不处理最左边和最右边的两列像素
			// 指向源图像倒数第j行，第i个象素的指针			
			lpSrc = (BYTE *)(lpDIBBits + lLineBytes * i + j);

			// 指向目标图像倒数第j行，第i个象素的指针			
			lpDst = (BYTE *)(lpNewDIBBits + lLineBytes * i + j);

			// 目标图像中的当前点先赋成黑色
			*lpDst = 0;
			*(lpDst + 1) = 0;
			*(lpDst + 2) = 0;
 
			// 如果源图像中(-1,0)、(0,0)、(1,0)三个点之一有白点，
			// 则将目标图像中的(0,0)点赋成白色
			for (n = 0; n < 3; n++)
			{
				if (*(lpSrc + (n - 1) * 3) > 128)
				{
					*lpDst = 255;
					*(lpDst + 1) = 255;
					*(lpDst + 2) = 255;
					break;
				}
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
 *   VDilation()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图象进行垂直方向上的膨胀
 *
 ************************************************************************/

void CMorph::VDilation()
{
	//循环变量
	LONG i;
	LONG j;
	LONG n;

	// 指向源图像的指针
	BYTE*	lpSrc;
	
	// 指向缓存图像的指针
	BYTE*	lpDst;

	// 指向缓存DIB图像的指针
	BYTE*	lpNewDIBBits;
	HLOCAL	hNewDIBBits;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);

	if (hNewDIBBits == NULL)
	{
		// 分配内存失败
		return;
	}
	
	// 锁定内存
	lpNewDIBBits = (BYTE*)LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为255
	lpDst = (BYTE*)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes * lHeight);

	//使用垂直方向的结构元素进行膨胀
	for(i = 1; i < lHeight - 1; i++)
	{
		for(j = 0; j < lLineBytes; j += 3)
		{
			//由于使用3×1的结构元素，为防止越界，所以不处理最上边和最下边的两列像素
			// 指向源图像倒数第j行，第i个象素的指针			
			lpSrc = (BYTE *)(lpDIBBits + lLineBytes * i + j);

			// 指向目标图像倒数第j行，第i个象素的指针			
			lpDst = (BYTE *)(lpNewDIBBits + lLineBytes * i + j);

			//目标图像中的当前点先赋成白色
			*lpDst = 255;
			*(lpDst + 1) = 255;
			*(lpDst + 2) = 255;

			// 如果源图像中(0,-1)、(0,0)、(0,1)三个点之一有黑点，
			// 则将目标图像中的(0,0)点赋成黑色
			for (n = 0; n < 3; n++)
			{
				if (*(lpSrc + (n - 1) * lLineBytes) < 128)
				{
					*lpDst = 0;
					*(lpDst + 1) = 0;
					*(lpDst + 2) = 0;
					break;
				}
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
 *   HDilation()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图象进行水平方向上的膨胀
 *
 ************************************************************************/

void CMorph::HDilation()
{
	//循环变量
	LONG i;
	LONG j;
	LONG n;
	
	// 指向源图像的指针
	BYTE*	lpSrc;
	
	// 指向缓存图像的指针
	BYTE*	lpDst;

	// 指向缓存DIB图像的指针
	BYTE*	lpNewDIBBits;
	HLOCAL	hNewDIBBits;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);

	if (hNewDIBBits == NULL)
	{
		// 分配内存失败
		return;
	}
	
	// 锁定内存
	lpNewDIBBits = (BYTE*)LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为255
	lpDst = (BYTE*)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes * lHeight);

	// 使用水平方向的结构元素进行膨胀
	for (i = 0; i < lHeight; i++)
	{
		for (j = 3; j < lLineBytes - 3; j += 3)
		{
			// 由于使用1×3的结构元素，为防止越界，所以不处理最左边和最右边的两列像素
			// 指向源图像倒数第j行，第i个象素的指针			
			lpSrc = (BYTE *)(lpDIBBits + lLineBytes * i + j);

			// 指向目标图像倒数第j行，第i个象素的指针			
			lpDst = (BYTE *)(lpNewDIBBits + lLineBytes * i + j);

			// 目标图像中的当前点先赋成白色
			*lpDst = 255;
			*(lpDst + 1) = 255;
			*(lpDst + 2) = 255;
 
			// 如果源图像中(-1,0)、(0,0)、(1,0)三个点之一有黑点，
			// 则将目标图像中的(0,0)点赋成黑色
			for (n = 0; n < 3; n++)
			{
				if (*(lpSrc + (n - 1) * 3) < 128)
				{
					*lpDst = 0;
					*(lpDst + 1) = 0;
					*(lpDst + 2) = 0;
					break;
				}
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
 *   VHDilation()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图象进行全方向膨胀
 *
 ************************************************************************/

void CMorph::VHDilation()
{
	//循环变量
	LONG i;
	LONG j;
	LONG m;
	LONG n;

	// 指向源图像的指针
	BYTE*	lpSrc;
	
	// 指向缓存图像的指针
	BYTE*	lpDst;

	// 指向缓存DIB图像的指针
	BYTE*	lpNewDIBBits;
	HLOCAL	hNewDIBBits;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);

	if (hNewDIBBits == NULL)
	{
		// 分配内存失败
		return;
	}
	
	// 锁定内存
	lpNewDIBBits = (BYTE*)LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为255
	lpDst = (BYTE*)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes * lHeight);

	// 3×3的结构元素
	int T[9] = {0, 1, 0,
				1, 1, 1,
				0, 1, 0};

	// 使用水平方向的结构元素进行腐蚀
	for (i = 1; i < lHeight - 1; i++)
	{
		for (j = 3; j < lLineBytes - 3; j += 3)
		{
			// 由于使用3×3的结构元素，为防止越界，所以不处理最左、右、上、下四边的像素
			
			// 指向源图像倒数第j行，第i个象素的指针			
			lpSrc = (BYTE *)(lpDIBBits + lLineBytes * i + j);

			// 指向目标图像倒数第j行，第i个象素的指针			
			lpDst = (BYTE *)(lpNewDIBBits + lLineBytes * i + j);

			// 目标图像中的当前点先赋成白色
			*lpDst = 255;
			*(lpDst + 1) = 255;
			*(lpDst + 2) = 255;
 
			// 如果源图像中3×3结构元素对应位置有黑点	
			// 则将目标图像中的(0,0)点赋成黑色
			for (m = 0; m < 3; m++)
			{
				for (n = 0; n < 3; n++)
				{
					if (T[m * 3 + n] == 0)
						continue;

					if (*(lpSrc + (1 - m) * lLineBytes +(n - 1) * 3) < 128)
					{
						*lpDst = 0;
						*(lpDst + 1) = 0;
						*(lpDst + 2) = 0;
						break;
					}
				}
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
 *   VHErosion()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图象进行全方向腐蚀
 *
 ************************************************************************/
void CMorph::VHErosion()
{
	//循环变量
	LONG i;
	LONG j;
	LONG m;
	LONG n;

	// 指向源图像的指针
	BYTE*	lpSrc;
	
	// 指向缓存图像的指针
	BYTE*	lpDst;

	// 指向缓存DIB图像的指针
	BYTE*	lpNewDIBBits;
	HLOCAL	hNewDIBBits;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);

	if (hNewDIBBits == NULL)
	{
		// 分配内存失败
		return;
	}
	
	// 锁定内存
	lpNewDIBBits = (BYTE*)LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为255
	lpDst = (BYTE*)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes * lHeight);

	// 3×3的结构元素
	int T[9] = {0, 1, 0,
				1, 1, 1,
				0, 1, 0};

	// 使用水平方向的结构元素进行腐蚀
	for (i = 1; i < lHeight - 1; i++)
	{
		for (j = 3; j < lLineBytes - 3; j += 3)
		{
			// 由于使用3×3的结构元素，为防止越界，所以不处理最左、右、上、下四边的像素
			
			// 指向源图像倒数第j行，第i个象素的指针			
			lpSrc = (BYTE *)(lpDIBBits + lLineBytes * i + j);

			// 指向目标图像倒数第j行，第i个象素的指针			
			lpDst = (BYTE *)(lpNewDIBBits + lLineBytes * i + j);

			// 目标图像中的当前点先赋成黑色
			*lpDst = 0;
			*(lpDst + 1) = 0;
			*(lpDst + 2) = 0;
 
			// 如果源图像中3×3结构元素对应位置有白点	
			// 则将目标图像中的(0,0)点赋成白色
			for (m = 0; m < 3; m++)
			{
				for (n = 0; n < 3; n++)
				{
					if (T[m * 3 + n] == 0)
						continue;

					if (*(lpSrc + (1 - m) * lLineBytes +(n - 1) * 3) > 128)
					{
						*lpDst = 255;
						*(lpDst + 1) = 255;
						*(lpDst + 2) = 255;
						break;
					}
				}
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
 *   OpenOperate()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图象进行开运算处理
 *
 ************************************************************************/

void CMorph::OpenOperate()
{
	VHErosion();
	VHDilation();
}

/*************************************************************************
 *
 * 函数名称：
 *   CloseOperate()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图象进行闭运算处理
 *
 ************************************************************************/

void CMorph::CloseOperate()
{
	VHDilation();
	VHErosion();
}

/*************************************************************************
 *
 * 函数名称：
 *   Thin()
 *
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图象进行细化处理
 *
 ************************************************************************/

void CMorph::Thin()
{
	
	LONG i;
	LONG j;
	LONG m;
	LONG n;

	// 5×5相邻区域像素值
	BYTE S[5][5];

	// 计数器
	BYTE nCount;

	// 循环跳出标志
	BOOL bJump = TRUE;

	// 指向源图像的指针
	BYTE*	lpSrc;
	
	// 指向缓存图像的指针
	BYTE*	lpDst;

	// 指向缓存DIB图像的指针
	BYTE*	lpNewDIBBits;
	HLOCAL	hNewDIBBits;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);

	if (hNewDIBBits == NULL)
	{
		// 分配内存失败
		return;
	}
	
	// 锁定内存
	lpNewDIBBits = (BYTE*)LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为255
	lpDst = (BYTE*)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes * lHeight);
	
	while (bJump)
	{
		bJump = FALSE;
		
		// 初始化新分配的内存，设定初始值为255
		lpDst = (BYTE*)lpNewDIBBits;
		memset(lpDst, (BYTE)255, lLineBytes * lHeight);

		// 由于使用5×5的结构元素，为防越界，不处理外围的2行、2列像素
		for (i = 2; i < lHeight - 2; i++)
		{
			for (j = 2 * 3; j < lLineBytes - 2 * 3; j += 3)
			{
				// 指向源图像倒数第j行，第i个象素的指针			
				lpSrc = (BYTE*)(lpDIBBits + lLineBytes * i + j);

				// 指向目标图像倒数第j行，第i个象素的指针			
				lpDst = (BYTE*)(lpNewDIBBits + lLineBytes * i + j);

				// 如果源图像中当前点为白色，则跳过
				if (*lpSrc > 127)
					continue;

				// 获得当前点相邻的5×5区域内像素值，白色用0代表，黑色用1代表
				for (m = 0; m < 5; m++)
				{
					for (n = 0; n < 5; n++)
					{
						if (*(lpSrc + (2 - m) * lLineBytes + (n - 2) * 3) > 127)
							S[m][n] = 0;
						else
							S[m][n] = 1;
					}
				}

				// 判断条件一是否成立：
				nCount =  S[1][1] + S[1][2] + S[1][3] + S[2][1] 
						+ S[2][3] + S[3][1]	+ S[3][2] + S[3][3];

				if (nCount < 2 || nCount >6)
				{
					*lpDst = 0;
					*(lpDst + 1) = 0;
					*(lpDst + 2) = 0;
					continue;
				}

				// 判断条件二是否成立：
				nCount = 0;
				
				if (S[1][2] == 0 && S[1][1] == 1) 
					nCount++;
				if (S[1][1] == 0 && S[2][1] == 1)
					nCount++;
				if (S[2][1] == 0 && S[3][1] == 1)
					nCount++;
				if (S[3][1] == 0 && S[3][2] == 1)
					nCount++;
				if (S[3][2] == 0 && S[3][3] == 1)
					nCount++;
				if (S[3][3] == 0 && S[2][3] == 1)
					nCount++;
				if (S[2][3] == 0 && S[1][3] == 1)
					nCount++;
				if (S[1][3] == 0 && S[1][2] == 1)
					nCount++;

				if (nCount != 1)
				{
					*lpDst = 0;
					*(lpDst + 1) = 0;
					*(lpDst + 2) = 0;
					continue;
				}

				// 判断条件三是否成立；
				if (S[1][2] * S[2][1] * S[2][3] != 0)
				{
					nCount = 0;
					if (S[0][2] == 0 && S[0][1] == 1)
						nCount++;
					if (S[0][1] == 0 && S[1][1] == 1)
						nCount++;
					if (S[1][1] == 0 && S[2][1] == 1)
						nCount++;
					if (S[2][1] == 0 && S[2][2] == 1)
						nCount++;
					if (S[2][2] == 0 && S[2][3] == 1)
						nCount++;
					if (S[2][3] == 0 && S[1][3] == 1)
						nCount++;
					if (S[1][3] == 0 && S[0][3] == 1)
						nCount++;
					if (S[0][3] == 0 && S[0][2] == 1)
						nCount++;

					if (nCount == 1)
					{
						*lpDst = 0;
						*(lpDst + 1) = 0;
						*(lpDst + 2) = 0;
						continue;
					}
				}

				// 判断条件四是否成立：
				if (S[1][2] * S[2][1] * S[3][2] != 0)
				{
					nCount = 0;
					if (S[1][1] == 0 && S[1][0] == 1)
						nCount++;
					if (S[1][0] == 0 && S[2][0] == 1)
						nCount++;
					if (S[2][0] == 0 && S[3][0] == 1)
						nCount++;
					if (S[3][0] == 0 && S[3][1] == 1)
						nCount++;
					if (S[3][1] == 0 && S[3][2] == 1)
						nCount++;
					if (S[3][2] == 0 && S[2][2] == 1)
						nCount++;
					if (S[2][2] == 0 && S[1][2] == 1)
						nCount++;
					if (S[1][2] == 0 && S[1][1] == 1)
						nCount++;
					if (nCount == 1)
					{
						*lpDst = 0;
						*(lpDst + 1) = 0;
						*(lpDst + 2) = 0;
						continue;
					}
				}

				// 如果条件均满足则删除该点
				*lpDst = 255;
				*(lpDst + 1) = 255;
				*(lpDst + 2) = 255;
				bJump = TRUE;
			}
		}

		// 复制腐蚀后的图像
		memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
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
 *   Thick() 
 * 
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图象进行粗化处理
 *
 ************************************************************************/

void CMorph::Thick()
{
	
	LONG i;
	LONG j;
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	int nChannel = m_pDIB->Channel();
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	// 对各像素进行灰度转换
	for (i = 0; i < lHeight; i ++)
	{
		for (j = 0; j < lWidth; j ++)
		{
			BYTE *pSrc = lpDIBBits + nChannel * j + i * lLineBytes;
			// 对像素各颜色分量进行二值化求补处理
			if (*(pSrc + 2) > 127) *(pSrc + 2) = 0;
			else                   *(pSrc + 2) = 255;
			if (*(pSrc + 1) > 127) *(pSrc + 1) = 0;
			else                   *(pSrc + 1) = 255;
			if (*pSrc > 127)       *pSrc = 0;
			else                   *pSrc = 255;
		}
	}

	// 在求补后再对图象进行细化
	Thin();

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   MAT() 
 * 
 * 参数:
 *   HDIB hDIB          - 待处理的DIB
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数对图象进行中轴变换
 *
 ************************************************************************/

void CMorph::MAT()
{
	 
	LONG i;
	LONG j;
	LONG m;
	LONG n;

	// 5×5相邻区域像素值
	BYTE S[5][5];

	// 计数器
	BYTE nCount;

	// 循环跳出标志 
	BOOL bJump = TRUE;

	// 指向源图像的指针
	BYTE*	lpSrc;
	
	// 指向缓存图像的指针
	BYTE*	lpDst;

	// 指向缓存DIB图像的指针
	BYTE*	lpNewDIBBits;
	HLOCAL	hNewDIBBits;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);

	if (hNewDIBBits == NULL)
	{
		// 分配内存失败
		return;
	}
	
	// 锁定内存	
	lpNewDIBBits = (BYTE*)LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为255
	lpDst = (BYTE*)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes * lHeight);
	
	while (bJump)
	{
		bJump = FALSE;
		
		// 初始化新分配的内存，设定初始值为255
		lpDst = (BYTE*)lpNewDIBBits;
		memset(lpDst, (BYTE)255, lLineBytes * lHeight);

		// 由于使用5×5的结构元素，为防越界，不处理外围的2行、2列像素
		for (i = 2; i < lHeight - 2; i++)
		{
			for (j = 2 * 3; j < lLineBytes - 2 * 3; j += 3)
			{
				// 指向源图像倒数第j行，第i个象素的指针			
				lpSrc = (BYTE*)(lpDIBBits + lLineBytes * i + j);

				// 指向目标图像倒数第j行，第i个象素的指针			
				lpDst = (BYTE*)(lpNewDIBBits + lLineBytes * i + j);

				// 如果源图像中当前点为白色，则跳过
				if (*lpSrc > 127)
					continue;

				// 获得当前点相邻的5×5区域内像素值，白色用0代表，黑色用1代表
				for (m = 0; m < 5; m++)
				{
					for (n = 0; n < 5; n++)
					{
						if (*(lpSrc + (2 - m) * lLineBytes + (n - 2) * 3) > 127)
							S[m][n] = 0;
						else
							S[m][n] = 1;
					}
				}

				// 判断条件1-1是否成立：
				nCount =  S[1][1] + S[1][2] + S[1][3] + S[2][1] 
						+ S[2][3] + S[3][1]	+ S[3][2] + S[3][3];

				if (nCount < 2 || nCount >6)
				{
					*lpDst = 0;
					*(lpDst + 1) = 0;
					*(lpDst + 2) = 0;
					continue;
				}

				// 判断条件1-2是否成立：
				nCount = 0;
				
				if (S[1][2] == 0 && S[1][1] == 1) 
					nCount++;
				if (S[1][1] == 0 && S[2][1] == 1)
					nCount++;
				if (S[2][1] == 0 && S[3][1] == 1)
					nCount++;
				if (S[3][1] == 0 && S[3][2] == 1)
					nCount++;
				if (S[3][2] == 0 && S[3][3] == 1)
					nCount++;
				if (S[3][3] == 0 && S[2][3] == 1)
					nCount++;
				if (S[2][3] == 0 && S[1][3] == 1)
					nCount++;
				if (S[1][3] == 0 && S[1][2] == 1)
					nCount++;

				if (nCount != 1)
				{
					*lpDst = 0;
					*(lpDst + 1) = 0;
					*(lpDst + 2) = 0;
					continue;
				}

				// 判断条件1-3是否成立；
				if (S[1][2] * S[2][1] * S[3][2] != 0)
				{
					*lpDst = 0;
					*(lpDst + 1) = 0;
					*(lpDst + 2) = 0;
					continue;
				}

				// 判断条件1-4是否成立：
				if (S[2][1] * S[3][2] * S[2][3] != 0)
				{
					*lpDst = 0;
					*(lpDst + 1) = 0;
					*(lpDst + 2) = 0;
					continue;
				}

				// 如果条件均满足则删除该点
				*lpDst = 255;
				*(lpDst + 1) = 255;
				*(lpDst + 2) = 255;
			}
		}

		// 由于使用5×5的结构元素，为防越界，不处理外围的2行、2列像素
		for (i = 2; i < lHeight - 2; i++)
		{
			for (j = 2 * 3; j < lLineBytes - 2 * 3; j += 3)
			{
				// 指向源图像倒数第j行，第i个象素的指针			
				lpSrc = (BYTE*)(lpDIBBits + lLineBytes * i + j);

				// 指向目标图像倒数第j行，第i个象素的指针			
				lpDst = (BYTE*)(lpNewDIBBits + lLineBytes * i + j);

				// 如果源图像中当前点为白色，则跳过
				if (*lpSrc > 127)
					continue;

				// 获得当前点相邻的5×5区域内像素值，白色用0代表，黑色用1代表
				for (m = 0; m < 5; m++)
				{
					for (n = 0; n < 5; n++)
					{
						if (*(lpSrc + (2 - m) * lLineBytes + (n - 2) * 3) > 127)
							S[m][n] = 0;
						else
							S[m][n] = 1;
					}
				}

				// 判断条件2-1是否成立：
				nCount =  S[1][1] + S[1][2] + S[1][3] + S[2][1] 
						+ S[2][3] + S[3][1]	+ S[3][2] + S[3][3];

				if (nCount < 2 || nCount >6)
				{
					*lpDst = 0;
					*(lpDst + 1) = 0;
					*(lpDst + 2) = 0;
					continue;
				}

				// 判断条件2-2是否成立：
				nCount = 0;
				
				if (S[1][2] == 0 && S[1][1] == 1) 
					nCount++;
				if (S[1][1] == 0 && S[2][1] == 1)
					nCount++;
				if (S[2][1] == 0 && S[3][1] == 1)
					nCount++;
				if (S[3][1] == 0 && S[3][2] == 1)
					nCount++;
				if (S[3][2] == 0 && S[3][3] == 1)
					nCount++;
				if (S[3][3] == 0 && S[2][3] == 1)
					nCount++;
				if (S[2][3] == 0 && S[1][3] == 1)
					nCount++;
				if (S[1][3] == 0 && S[1][2] == 1)
					nCount++;

				if (nCount != 1)
				{
					*lpDst = 0;
					*(lpDst + 1) = 0;
					*(lpDst + 2) = 0;
					continue;
				}

				// 判断条件2-3是否成立；
				if (S[1][2] * S[2][1] * S[2][3] != 0)
				{
					*lpDst = 0;
					*(lpDst + 1) = 0;
					*(lpDst + 2) = 0;
					continue;
				}

				// 判断条件2-4是否成立：
				if (S[1][2] * S[3][2] * S[2][3] != 0)
				{
					*lpDst = 0;
					*(lpDst + 1) = 0;
					*(lpDst + 2) = 0;
					continue;
				}

				// 如果条件均满足则删除该点
				*lpDst = 255;
				*(lpDst + 1) = 255;
				*(lpDst + 2) = 255;
				bJump = TRUE;
			}
		}

		// 复制腐蚀后的图像
		memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	}
	
	// 复制腐蚀后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);

	// 释放内存
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	EndWaitCursor();
}


