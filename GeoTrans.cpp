// ************************************************************************
//  文件名：GeoTrans.cpp
//
//  图像几何变换函数库：
//
//  ZoomDIB()				- 缩放位图
//	Rotate()				- 旋转位图
//	Rectinication()			- 空间较正图像
//
// ************************************************************************

#include "stdafx.h"
#include "DIP_System.h"
#include "GeoTrans.h"
#include "SpaceFilter.h"
#include "math.h"
#include "DIP_SystemDoc.h"
#include "DIP_SystemView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGeoTrans

CGeoTrans::CGeoTrans()
{
}

CGeoTrans::CGeoTrans(CDIB* pDIB)
{
	m_pDIB = pDIB;
}

CGeoTrans::~CGeoTrans()
{
}

BEGIN_MESSAGE_MAP(CGeoTrans, CWnd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeoTrans message handlers

BOOL CGeoTrans::Zoom(int widthnew, int heightnew)
{
	if (m_pDIB->GetImage() == NULL) return FALSE;

	CMDIFrameWnd* pFrame = (CMDIFrameWnd *)AfxGetApp()->GetMainWnd();
	CMDIChildWnd* pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();
	CDIP_SystemDoc* pDoc = (CDIP_SystemDoc *)pChild->GetActiveDocument();
	CDIP_SystemView* pView = (CDIP_SystemView *)pChild->GetActiveView();
	int m_nChannel = m_pDIB->Channel();
	LONG m_nImageWidth = m_pDIB->Width();
	LONG m_nImageHeight = m_pDIB->Height();
	LONG m_nRowLen = m_pDIB->RowLen();
	LONG m_nDatalen = m_pDIB->Length();
	BYTE* pBits = m_pDIB->FindAddress();

	//设置缩放后的图像尺寸数据
	long WidthCur = widthnew;
	long HeightCur = heightnew;
	long RowlenCur = (widthnew*m_nChannel * 8 + 31) / 32 * 4;
	long DataLenCur = HeightCur * RowlenCur;
	//计算缩放比例
	float ratewidth = 1.0f * WidthCur / m_nImageWidth;
	float rateheight = 1.0f * HeightCur / m_nImageHeight;
	//申请空间用于保存目标图像数据及中间结果
	BYTE* dataStep1 = new BYTE[m_nImageHeight * RowlenCur];
	BYTE* dataDes = new BYTE[DataLenCur];

	//双二次插值法
	int r = 0, c = 0, k = 0;
	//先行向插值
	for (r = 0; r < m_nImageHeight; r++)
	{
		BYTE* pDes = dataStep1 + r * RowlenCur;
		for (c = 0; c < WidthCur; c++)
		{
			int col = (int)(c / ratewidth);
			float fRateX = c / ratewidth - col;
			BYTE* pSrc = pBits + r * m_nRowLen + col * m_nChannel;
			if (col < m_nImageWidth - 1)
			{
				for (k = 0; k<m_nChannel; k++)
				{
					*(pDes + k) = (BYTE)(*(pSrc + k) * (1 - fRateX) + *(pSrc + m_nChannel + k) * fRateX);
				}
			}
			else
			{
				for (k = 0; k<m_nChannel; k++)
				{
					*(pDes + k) = (BYTE)*(pSrc + k);
				}
			}
			pDes += m_nChannel;
		}
	}
	//再纵向插值
	for (c = 0; c < WidthCur; c++)
	{
		BYTE* pDes = dataDes + c * m_nChannel;
		for (r = 0; r < HeightCur; r++)
		{
			int row = (int)(r / rateheight);
			float fRateY = r / rateheight - row;
			BYTE* pSrc = dataStep1 + row * RowlenCur + c * m_nChannel;
			if (row < m_nImageHeight - 1)
			{
				for (k = 0; k<m_nChannel; k++)
				{
					*(pDes + k) = (BYTE)(*(pSrc + k) * (1 - fRateY) + *(pSrc + RowlenCur + k) * fRateY);
				}
			}
			else
			{
				for (k = 0; k<m_nChannel; k++)
				{
					*(pDes + k) = (BYTE)*(pSrc + k);
				}
			}
			pDes += RowlenCur;
		}
	}
	CImage* pImage = new CImage;
	if (m_nChannel == 1)//灰度图像特殊处理
	{
		pImage->Create(widthnew, heightnew, 8);
		RGBQUAD ColorTab[256];
		for (int i = 0; i<256; i++)
		{
			ColorTab[i].rgbBlue = ColorTab[i].rgbGreen = ColorTab[i].rgbRed = i;
		}
		pImage->SetColorTable(0, 256, ColorTab);
	}
	else pImage->Create(widthnew, heightnew, m_nChannel * 8);
	pBits = (BYTE *)pImage->GetBits() + (pImage->GetPitch()*(heightnew - 1));
	memcpy(pBits, dataDes, DataLenCur);
	// 修改图像指针：先清空，然后重新赋值
	pDoc->m_pDIB->Clear();
	m_pDIB->m_UndoStack.SetTop(pImage);
	m_pDIB->SetImage(pImage);
	pDoc->SetDocSize();
	pView->Invalidate(TRUE);
	delete[] dataStep1;
	delete[] dataDes;
	return TRUE;
}

BOOL CGeoTrans::ZoomDIB(float fZoomRatio)
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd *)AfxGetApp()->GetMainWnd();
	CMDIChildWnd* pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();
	CDIP_SystemView* pView = (CDIP_SystemView *)pChild->GetActiveView();
	LONG m_nImageWidth = m_pDIB->Width();
	LONG m_nImageHeight = m_pDIB->Height();
	long WidthNew = m_nImageWidth * fZoomRatio;
	long HeightNew = m_nImageHeight * fZoomRatio;
	// 避免图像太小或者图像太大
	if (16 <= WidthNew && WidthNew <= 16384 && 16 <= HeightNew && HeightNew <= 16384 )
		Zoom(WidthNew, HeightNew);
	return TRUE;
}

/*************************************************************************
 *
 * 函数名称：
 *   Rotate()
 *
 * 参数:
 *   int	nAngle		- 缩放比率
 *
 * 返回值:
 *   void			    - 无返回值
 *
 * 说明:
 *   该函数用来旋转DIB图像
 *
 ************************************************************************/
void CGeoTrans::Rotate(float fAngle)
{
	LONG i0;
	LONG j0;
	LONG i1;
	LONG j1;
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = m_pDIB->FindAddress();

	// 旋转角度的弧度
	fAngle = (float)(fAngle * PI / 180);

	// 角度的正余弦
	float fSinAngle = (float)sin(fAngle);
	float fCosAngle = (float)cos(fAngle);

	// 不能用char，也不能用::strcpy()
	BYTE* temp;
	temp = new BYTE [lLineBytes * lHeight];

	// 复制空白数据到中间缓存
	for (i0 = 0; i0 < lLineBytes * lHeight; i0++)
		temp[i0] = 255;

	// 对各像素进行旋转处理
	for (i0 = 0; i0 < lHeight; i0 ++)
	{
		for (j0 = 0; j0 < lWidth; j0 ++)
		{
			// 计算旋转后的坐标位置
			j1 = (LONG)((j0 - lWidth / 2) * fCosAngle + (lHeight / 2 - i0) * fSinAngle + lWidth / 2 + 0.5);
			i1 = (LONG)((j0 - lWidth / 2) * fSinAngle - (lHeight / 2 - i0) * fCosAngle + lHeight / 2 + 0.5);

			// 将原始象素复制到目标位置
			if (i1 >= 0 && i1 < lHeight && j1 >= 0 && j1 < lWidth)
			{
				temp[lLineBytes * (lHeight - i1 - 1) + j1 * nChannel] = *((BYTE *)lpDIBBits + lLineBytes * (lHeight - i0 - 1) + j0 * nChannel);
				temp[lLineBytes * (lHeight - i1 - 1) + j1 * nChannel + 1] = *((BYTE *)lpDIBBits + lLineBytes * (lHeight - i0 - 1) + j0 * nChannel + 1);
				temp[lLineBytes * (lHeight - i1 - 1) + j1 * nChannel + 2] = *((BYTE *)lpDIBBits + lLineBytes * (lHeight - i0 - 1) + j0 * nChannel + 2);
			}
		}
	}
	
	// 回存处理结果到DIB
	for(i0 = 0; i0 < lLineBytes * lHeight; i0++)
		*(lpDIBBits + i0) = temp[i0];

	// 释放内存
	delete[] temp;
	
	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   Rectinication()
 *
 * 参数:
 *   int	nAngle		- 面倾斜角度
 *
 * 返回值:
 *   void			    - 无返回值
 *
 * 说明:
 *   该函数用来在空间矫正DIB图像
 *
 ************************************************************************/
void CGeoTrans::Rectinication(float fAngle)
{
	LONG i0;
	LONG j0;
	LONG i1;
	LONG j1;
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = m_pDIB->FindAddress();

	// 旋转角度的弧度
	fAngle = (float)(fAngle * PI / 180);

	// 角度的正余弦
	float fSinAngle = (float)sin(fAngle);
	float fCosAngle = (float)cos(fAngle);
	float fTgAngle = fSinAngle / fCosAngle;

	// 不能用char，也不能用::strcpy()
	BYTE* m_temp;
	m_temp = new BYTE [lLineBytes * lHeight];

	// 复制空白数据到中间缓存
	for (i0 = 0; i0 < lLineBytes * lHeight; i0++)
		m_temp[i0] = 255;

	// 先对X方向进行矫正处理
	for (i0 = 0; i0 < lHeight; i0 ++)
	{
		for (j0 = 0; j0 < lWidth; j0 ++)
		{
			// 计算校直后的坐标位置
			j1 = (LONG)(lWidth - (lWidth - j0) / fCosAngle + 0.5f);
			i1 = i0;			

			// 将原始象素复制到目标位置
			if (i1 >= 0 && i1 < lHeight && j1 >= 0 && j1 < lWidth)
			{
				m_temp[lLineBytes * (lHeight - i1 - 1) + j1 * nChannel] = *((BYTE *)lpDIBBits + lLineBytes * (lHeight - i0 - 1) + j0 * nChannel);
				m_temp[lLineBytes * (lHeight - i1 - 1) + j1 * nChannel + 1] = *((BYTE *)lpDIBBits + lLineBytes * (lHeight - i0 - 1) + j0 * nChannel + 1);
				m_temp[lLineBytes * (lHeight - i1 - 1) + j1 * nChannel + 2] = *((BYTE *)lpDIBBits + lLineBytes * (lHeight - i0 - 1) + j0 * nChannel + 2);
			}
		}
	}

	// 回存处理结果到DIB
	for(i0 = 0; i0 < lLineBytes * lHeight; i0++)
		*(lpDIBBits + i0) = m_temp[i0];

	// 调用中值滤波进行平滑处理
	CSpaceFilter clsSpaceFilter(m_pDIB);
	clsSpaceFilter.MedianFilter();

	// 再对y方向进行矫正处理
	for (i0 = 0; i0 < lHeight; i0 ++)
	{
		for (j0 = 0; j0 < lWidth; j0 ++)
		{
			// 计算校直后的坐标位置
			j1 = j0;
			if (i0 < lHeight / 2)
				i1 = (LONG)(i0 - (lHeight / 2 - i0) * fSinAngle * (lWidth - j1) / lWidth  + 0.5f);
			else
				i1 = (LONG)(i0 + (i0 - lHeight / 2) * fSinAngle * (lWidth - j1) / lWidth + 0.5f);

			// 将原始象素复制到目标位置
			if (i1 >= 0 && i1 < lHeight && j1 >= 0 && j1 < lWidth)
			{
				m_temp[lLineBytes * (lHeight - i1 - 1) + j1 * nChannel] = *((BYTE *)lpDIBBits + lLineBytes * (lHeight - i0 - 1) + j0 * nChannel);
				m_temp[lLineBytes * (lHeight - i1 - 1) + j1 * nChannel + 1] = *((BYTE *)lpDIBBits + lLineBytes * (lHeight - i0 - 1) + j0 * nChannel + 1);
				m_temp[lLineBytes * (lHeight - i1 - 1) + j1 * nChannel + 2] = *((BYTE *)lpDIBBits + lLineBytes * (lHeight - i0 - 1) + j0 * nChannel + 2);
			}
		}
	}

	// 回存处理结果到DIB
	for(i0 = 0; i0 < lLineBytes * lHeight; i0++)
		*(lpDIBBits + i0) = m_temp[i0];

	// 调用中值滤波进行平滑处理
	clsSpaceFilter.MedianFilter();

	// 释放内存
	delete[] m_temp;
	
	EndWaitCursor();
}


