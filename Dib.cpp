// ************************************************************************
//  文件名：dib.cpp
//

#include "stdafx.h"
#include "dib.h"
#include <io.h>
#include <errno.h>

#include <math.h>
#include <direct.h>

//	Dib文件头标志（字符串"BM"，写DIB时用到该常数）
#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')

// 默认构造函数
CDIB::CDIB()
{
	m_pImage = NULL;
}

// 默认析构函数
CDIB::~CDIB()
{
	// 已经在堆栈中将其销毁
	// SAFE_DELETE(m_pImage);
}

// 获取图像指针
CImage*	CDIB::GetImage()
{
	return m_pImage;
}

// 修改图像指针为pImage
void	CDIB::SetImage(CImage* pImage)
{
	m_pImage = pImage;
}

////////////////////////////////////////////////////////////////////////////
//							功能模块开始								  //
////////////////////////////////////////////////////////////////////////////

/*************************************************************************
 *
 * 函数名称：
 *   Paint()
 *
 * 参数:
 *   HDC hDC            - 设备环境句柄
 *   LPRECT lpDCRect    - 矩形区域
 *   LPRECT lpDIBRect   - 待输出DIB区域
 *
 * 返回值:
 *   BOOL               - 绘制成功返回TRUE，否则返回FALSE。
 *
 * 说明:
 *   绘制DIB对象
 *
 ************************************************************************/
BOOL CDIB::Paint(HDC hDC, LPRECT lpDCRect, LPRECT lpDIBRect)
{
	if (lpDCRect->right <= lpDCRect->left + 2 || lpDCRect->bottom <= lpDCRect->top + 2)
		return FALSE;
	return m_pImage->Draw(hDC, *lpDCRect, *lpDIBRect);
}

// 获取图像首地址
BYTE* CDIB::FindAddress()
{
	if (m_pImage == NULL) return NULL;
	return (BYTE*)m_pImage->GetBits() + (m_pImage->GetPitch() * (m_pImage->GetHeight() - 1));
}

// 获取图像宽度
DWORD CDIB::Width()
{
	return m_pImage->GetWidth();
}

// 获取图像高度
DWORD CDIB::Height()
{
	return m_pImage->GetHeight();
}

// 获取图像每行字节数
DWORD	CDIB::RowLen()
{
	return abs(m_pImage->GetPitch());
}

// 获取图像的通道(8位为1通道)
WORD CDIB::Channel()
{
	return BitCount() / 8;
}

// 获取图像总的字节数
DWORD CDIB::Length()
{
	return RowLen() * Height();
}

// 获取图像颜色位数
WORD CDIB::BitCount()
{
	return m_pImage->GetBPP();
}

/*************************************************************************
 *
 * 函数名称：
 *   Save()
 *
 * 参数:
 *   Cstring sPath        - 文件名称
 *
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE
 *
 * 说明:
 *   将DIB保存到指定文件中
 *
 *************************************************************************/
BOOL CDIB::Save(CString sPath)
{
	if (m_pImage == NULL) return FALSE;
	if (m_pImage->Save(sPath) != S_OK)
	{
		AfxMessageBox(L"保存图片失败！");
		return FALSE;
	}
	return TRUE;
}

/*************************************************************************
 *
 * 函数名称：
 *   Read()
 *
 * 参数:
 *   CFile& file        - 要读取得文件文件CFile
 *
 * 返回值:
 *   BOOL               - 成功TRUE
 *
 * 说明:
 *   从指定文件中读取DIB对象
 *
 *************************************************************************/
BOOL CDIB::Read(CFile& file)
{
	CString path = file.GetFilePath();
	m_pImage = new CImage;
	m_pImage->Load(path);
	if (m_pImage->IsNull())
	{
		SAFE_DELETE(m_pImage);
		return FALSE;
	}
	// 将原始图像指针压入栈底
	m_UndoStack.Push(m_pImage);
	return TRUE;
}

// 从指定路径中读取图像
BOOL CDIB::Read(CString path)
{
	m_pImage = new CImage;
	m_pImage->Load(path);
	if (m_pImage->IsNull())
	{
		SAFE_DELETE(m_pImage);
		return FALSE;
	}
	// 将原始图像指针压入栈底
	m_UndoStack.Push(m_pImage);
	// 检测图像是否为二进制位图
	if (BitCount() == 1)
	{
		AfxMessageBox(_T("二进制位图将被转换为8色图像！"), MB_ICONINFORMATION | MB_OK);
		BinaryMap2Other(8);
	}
	return TRUE;
}

// 将当前图像副本存入“撤销”堆栈.栈顶始终是当前图像
void CDIB::PushCopy()
{
	CImage* pImage = MakeCopy();
	m_UndoStack.Push(pImage);
	SetImage(pImage);
}

// 返回当前图像的副本图像，用于剪切板等操作
CImage* CDIB::MakeCopy()
{
	int channel = Channel();
	int width = Width();
	int height = Height();
	long length = Length();
	// 创建副本图像
	CImage* pImage = new CImage;
	if (channel == 1)//灰度图像特殊处理
	{
		pImage->Create(width, height, 8);
		RGBQUAD ColorTab[256];
		for(int i = 0; i<256; i++)
		{
			ColorTab[i].rgbBlue = ColorTab[i].rgbGreen = ColorTab[i].rgbRed = i;
		}
		pImage->SetColorTable(0, 256, ColorTab);
	}
	else pImage->Create(width, height, channel*8);
	BYTE* pDst = (BYTE *)pImage->GetBits()+(pImage->GetPitch()*(height-1));
	BYTE* pSrc = (BYTE *)m_pImage->GetBits()+(m_pImage->GetPitch()*(height-1));
	memcpy(pDst, pSrc, length);
	return pImage;
}

// 将当前图像取出“撤销”堆栈，放入“恢复”堆栈
void CDIB::Undo()
{
	if (m_UndoStack.m_nSizeOfStack == 1) return;
	m_RedoStack.Push(m_UndoStack.Top());
	SetImage(m_UndoStack.GetTop());
}

// 撤销之后，恢复并继续之前的操作
void	CDIB::Redo()
{
	if (m_RedoStack.m_nSizeOfStack == 0) return;
	m_UndoStack.Push(m_RedoStack.Top());
	SetImage(m_UndoStack.GetTop());
}

// RGB图像转为灰度图像
BOOL	CDIB::RGB2Gray()
{
	LONG i, j;

	int nChannel = Channel();
	if (nChannel<3) return FALSE;

	LONG lWidth = Width();
	LONG lHeight = Height();
	LONG lLineBytes = RowLen();
	BYTE* lpDIBBits = FindAddress();

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
	return TRUE;
}

// 改变图像通道数, bpp：目标图像的位色
BOOL	CDIB::ChangeBPP(UINT bpp)
{
	if(bpp != 8 && bpp != 24 && bpp != 32)
		return FALSE;

	int nBitCount = BitCount();
	if (nBitCount == bpp) return TRUE;
	switch (nBitCount)
	{
	case 8:
		if (bpp == 24)    Bpp8To24();
		else if(bpp == 32)Bpp8To32();
		break;
	case 16:
		break;
	case 24:
		if (bpp == 8)     Bpp24To8();
		else if(bpp == 32)Bpp24To32();
		break;
	case 32:
		if (bpp == 8)     Bpp32To8();
		else if(bpp == 24)Bpp32To24();
		break;
	default:
		break;
	}
	return TRUE;
}

// 改变二进制图像通道数, bpp：目标图像的位色
BOOL	CDIB::BinaryMap2Other(int bpp)
{
	int nBitCount = BitCount();
	if (nBitCount != 1) return TRUE;
	switch (bpp)
	{
	case 8:
		Bpp1To8();
		break;
	case 16:
		break;
	case 24:
		Bpp1To24();
		break;
	case 32:
		Bpp1To32();
		break;
	default:
		break;
	}
	return TRUE;
}

// 翻转图像，默认为垂直翻转
void	CDIB::Flip(BOOL bVertical)
{
	if(bVertical)
	{

	}
	else
	{

	}
}

// 清空图像
void	CDIB::Clear()
{
	SAFE_DELETE(m_pImage);
}

// 设置图像像素，默认值 ： int channel = 4, int width = 0, int height = 0
void	CDIB::SetBits(BYTE* pBits, int channel, int width, int height)
{
	// 默认参数
	int channelnew = Channel();
	int widthnew = Width();
	int heightnew = Height();

	// 不采用默认参数
	if (width * height != 0)
	{
		widthnew = width;
		heightnew = height;
	}
	if (channel != 4)
		channelnew = channel;

	// 清空图像
	Clear();
	CImage *pNew = new CImage;
	pNew->Create(widthnew, heightnew, channel * 8);
	BYTE* pDst = (BYTE *)pNew->GetBits()+(pNew->GetPitch()*(heightnew - 1));
	memcpy(pDst, pBits, widthnew * heightnew * channel);
	m_UndoStack.SetTop(pNew);
	SetImage(pNew);
}

// 8位色转24位色
void	CDIB::Bpp8To24()
{
	LONG lWidth = Width();
	LONG lHeight = Height();
	int nChannel = 1, nChannelNew = 3;
	LONG lRowLen = RowLen(), lRowLenNew = WIDTHBYTES(24 * lWidth);

	CImage *pNew = new CImage;
	pNew->Create(lWidth, lHeight, 24);

	BYTE *pSrc = FindAddress();
	BYTE *pDst = (BYTE*)pNew->GetBits() + (pNew->GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + i * lRowLen;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			*(pDstTemp + 2) = *(pDstTemp + 1) = *pDstTemp = *pSrcTemp;
		}
	}
	Clear();
	m_UndoStack.SetTop(pNew);
	SetImage(pNew);
}

// 8位色转32位色
void	CDIB::Bpp8To32()
{
	LONG lWidth = Width();
	LONG lHeight = Height();
	int nChannel = 1, nChannelNew = 4;
	LONG lRowLen = RowLen(), lRowLenNew = WIDTHBYTES(32 * lWidth);

	CImage *pNew = new CImage;
	pNew->Create(lWidth, lHeight, 32);

	BYTE *pSrc = FindAddress();
	BYTE *pDst = (BYTE*)pNew->GetBits() + (pNew->GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + i * lRowLen;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			*(pDstTemp + 2) = *(pDstTemp + 1) = *pDstTemp = *pSrcTemp;
		}
	}
	Clear();
	m_UndoStack.SetTop(pNew);
	SetImage(pNew);
}

// 24位色转8位色
void	CDIB::Bpp24To8()
{
	LONG lWidth = Width();
	LONG lHeight = Height();
	int nChannel = 3, nChannelNew = 1;
	LONG lRowLen = RowLen(), lRowLenNew = WIDTHBYTES(8 * lWidth);

	CImage *pNew = new CImage;
	pNew->Create(lWidth, lHeight, 8);
	RGBQUAD ColorTab[256];
	for(int i = 0; i<256; i++)
	{
		ColorTab[i].rgbBlue = ColorTab[i].rgbGreen = ColorTab[i].rgbRed = i;
	}
	pNew->SetColorTable(0, 256, ColorTab);

	BYTE *pSrc = FindAddress();
	BYTE *pDst = (BYTE*)pNew->GetBits() + (pNew->GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + i * lRowLen;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			BYTE R = *(pSrcTemp + 2);
			BYTE G = *(pSrcTemp + 1);
			BYTE B = *pSrcTemp;
			*pDstTemp = (9798 * R + 19235 * G + 3735 * B) / 32768;
		}
	}
	Clear();
	m_UndoStack.SetTop(pNew);
	SetImage(pNew);
}

// 24位色转32位色
void	CDIB::Bpp24To32()
{
	LONG lWidth = Width();
	LONG lHeight = Height();
	int nChannel = 3, nChannelNew = 4;
	LONG lRowLen = RowLen(), lRowLenNew = WIDTHBYTES(32 * lWidth);

	CImage *pNew = new CImage;
	pNew->Create(lWidth, lHeight, 32);

	BYTE *pSrc = FindAddress();
	BYTE *pDst = (BYTE*)pNew->GetBits() + (pNew->GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + i * lRowLen;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			*(pDstTemp + 2) = *(pSrcTemp + 2);
			*(pDstTemp + 1) = *(pSrcTemp + 1);
			*pDstTemp = *pSrcTemp;
		}
	}
	Clear();
	m_UndoStack.SetTop(pNew);
	SetImage(pNew);
}

// 32位色转8位色
void	CDIB::Bpp32To8()
{
	LONG lWidth = Width();
	LONG lHeight = Height();
	int nChannel = 4, nChannelNew = 1;
	LONG lRowLen = RowLen(), lRowLenNew = WIDTHBYTES(8 * lWidth);

	CImage *pNew = new CImage;
	pNew->Create(lWidth, lHeight, 8);
	RGBQUAD ColorTab[256];
	for(int i = 0; i<256; i++)
	{
		ColorTab[i].rgbBlue = ColorTab[i].rgbGreen = ColorTab[i].rgbRed = i;
	}
	pNew->SetColorTable(0, 256, ColorTab);

	BYTE *pSrc = FindAddress();
	BYTE *pDst = (BYTE*)pNew->GetBits() + (pNew->GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + i * lRowLen;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			BYTE R = *(pSrcTemp + 2);
			BYTE G = *(pSrcTemp + 1);
			BYTE B = *pSrcTemp;
			*pDstTemp = (9798 * R + 19235 * G + 3735 * B) / 32768;
		}
	}
	Clear();
	m_UndoStack.SetTop(pNew);
	SetImage(pNew);
}

// 32位色转24位色
void	CDIB::Bpp32To24()
{
	LONG lWidth = Width();
	LONG lHeight = Height();
	int nChannel = 4, nChannelNew = 3;
	LONG lRowLen = RowLen(), lRowLenNew = WIDTHBYTES(24 * lWidth);

	CImage *pNew = new CImage;
	pNew->Create(lWidth, lHeight, 24);

	BYTE *pSrc = FindAddress();
	BYTE *pDst = (BYTE*)pNew->GetBits() + (pNew->GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			BYTE *pSrcTemp = pSrc + nChannel * j + i * lRowLen;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			*(pDstTemp + 2) = *(pSrcTemp + 2);
			*(pDstTemp + 1) = *(pSrcTemp + 1);
			*pDstTemp = *pSrcTemp;
		}
	}
	Clear();
	m_UndoStack.SetTop(pNew);
	SetImage(pNew);
}

// 将二进制图像转换为8位色图像
void	CDIB::Bpp1To8()
{
	LONG lWidth = Width();
	LONG lHeight = Height();
	int nChannelNew = 1;
	LONG lRowLen = RowLen(), lRowLenNew = WIDTHBYTES(8 * lWidth);

	CImage *pNew = new CImage;
	pNew->Create(lWidth, lHeight, 8);
	RGBQUAD ColorTab[256];
	for(int i = 0; i<256; i++)
	{
		ColorTab[i].rgbBlue = ColorTab[i].rgbGreen = ColorTab[i].rgbRed = i;
	}
	pNew->SetColorTable(0, 256, ColorTab);

	BYTE *pSrc = FindAddress();
	BYTE *pDst = (BYTE*)pNew->GetBits() + (pNew->GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			int mod = j % 8;
			BYTE *pSrcTemp = pSrc + int(j / 8.f) + i * lRowLen;
			// *pSrcTemp的第mod二进制位
			BYTE temp = ( *pSrcTemp>>(7 - mod) ) & 1;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			*pDstTemp = 255 * temp;
		}
	}
	Clear();
	m_UndoStack.SetTop(pNew);
	SetImage(pNew);
}

// 将二进制图像转换为24位色图像
void	CDIB::Bpp1To24()
{
	LONG lWidth = Width();
	LONG lHeight = Height();
	int nChannelNew = 3;
	LONG lRowLen = RowLen(), lRowLenNew = WIDTHBYTES(24 * lWidth);

	CImage *pNew = new CImage;
	pNew->Create(lWidth, lHeight, 24);

	BYTE *pSrc = FindAddress();
	BYTE *pDst = (BYTE*)pNew->GetBits() + (pNew->GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			int mod = j % 8;
			BYTE *pSrcTemp = pSrc + int(j / 8.f) + i * lRowLen;
			// *pSrcTemp的第mod二进制位
			BYTE temp = ( *pSrcTemp>>(7 - mod) ) & 1;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			*pDstTemp = *(pDstTemp + 1) = *(pDstTemp + 2) = 255 * temp;
		}
	}
	Clear();
	m_UndoStack.SetTop(pNew);
	SetImage(pNew);
}

// 将二进制图像转换为32位色图像
void	CDIB::Bpp1To32()
{
	LONG lWidth = Width();
	LONG lHeight = Height();
	int nChannelNew = 4;
	LONG lRowLen = RowLen(), lRowLenNew = WIDTHBYTES(32 * lWidth);

	CImage *pNew = new CImage;
	pNew->Create(lWidth, lHeight, 32);

	BYTE *pSrc = FindAddress();
	BYTE *pDst = (BYTE*)pNew->GetBits() + (pNew->GetPitch()*(lHeight - 1));
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			int mod = j % 8;
			BYTE *pSrcTemp = pSrc + int(j / 8.f) + i * lRowLen;
			// *pSrcTemp的第mod二进制位
			BYTE temp = ( *pSrcTemp>>(7 - mod) ) & 1;
			BYTE *pDstTemp = pDst + nChannelNew * j + i * lRowLenNew;
			*pDstTemp = *(pDstTemp + 1) = *(pDstTemp + 2) = 255 * temp;
		}
	}
	Clear();
	m_UndoStack.SetTop(pNew);
	SetImage(pNew);
}

// 表示从栈顶下一层加载图像数据，往往是makecopy之后使用，避免以后的操作引起叠加
void	CDIB::ReLoad()
{
	int next = m_UndoStack.m_nTopOfStack - 1;
	if (next == -1)
		next = MAX_UNDO_NUMBER - 1;
	CImage* pTop = m_UndoStack.GetTop();
	CImage* pNext = m_UndoStack.m_Array[next];
	BYTE *pDst = (BYTE*)pTop->GetBits() + (pTop->GetPitch()*(pTop->GetHeight() - 1));
	BYTE *pSrc = (BYTE*)pNext->GetBits() + (pNext->GetPitch()*(pNext->GetHeight() - 1));
	long length = pTop->GetHeight() * abs(pTop->GetPitch());
	memcpy(pDst, pSrc, length);
}
/*************************************************************************
 *
 * 函数名称：
 *   SetRgnColor()
 *
 * 参数:	
 *	 CRect rect			- 设置的矩形区
 *	 COLORREF col		- 填充的颜色
 *
 * 返回值:
 *   无返回值
 *
 * 说明:
 *   该函数用指定的颜色填充指定的区域
 *
 *************************************************************************/
void CDIB::SetRgnColor(CRect rect, COLORREF col)
{
	LONG i, j;
	BYTE* lpDIBBits = FindAddress();
	LONG lWidth = Width();
	LONG lHeight = Height();
	LONG lLineBytes = RowLen();

	// 对区域进行颜色填充
	for (i = lHeight - rect.bottom; i < lHeight - rect.top; i++)
	{
		for (j = rect.left * 3; j < rect.right * 3; j++)
		{
			*(BYTE *)(lpDIBBits + lLineBytes * i + j) = GetBValue(col);
			j++;
			*(BYTE *)(lpDIBBits + lLineBytes * i + j) = GetGValue(col);
			j++;
			*(BYTE *)(lpDIBBits + lLineBytes * i + j) = GetRValue(col);
		}
	}
}

/*************************************************************************
 *
 * 函数名称：
 *   SetCircleColor()
 *
 * 参数:	
 *	 CRect rect			- 设置的矩形区
 *	 COLORREF col		- 填充的颜色
 *
 * 返回值:
 *   无返回值
 *
 * 说明:
 *   该函数用指定的颜色填充指定的圆形区域
 *
 *************************************************************************/
void CDIB::SetCircleColor(CRect rect, COLORREF col)
{
	LONG i, j;
	int cx, cy, dr;
	
	LONG lWidth = Width();
	LONG lHeight = Height();
	LONG lLineBytes = RowLen();
	int nChannel = Channel();
	BYTE* lpDIBBits = FindAddress();

	// 对圆形区域进行颜色填充
	for (i = lHeight - rect.bottom; i < lHeight - rect.top; i++)
	{
		for (j = rect.left; j < rect.right; j ++)
		{
			cx = abs(j - rect.left - 8);
			cy = abs(i - lHeight + rect.bottom - 8);
			dr = cx * cx + cy * cy;
			if (dr > 64 || i < 0 || i > lHeight || j < 0 || j > lWidth)
				continue;
			*(BYTE *)(lpDIBBits + lLineBytes * i + j * nChannel) = GetBValue(col);
			*(BYTE *)(lpDIBBits + lLineBytes * i + j * nChannel + 1) = GetGValue(col);
			*(BYTE *)(lpDIBBits + lLineBytes * i + j * nChannel + 2) = GetRValue(col);
		}
	}
}

/**********************************************************/
/***********************************************************
/***********************************************************
/***********************************************************
/***********************************************************
/***********************************************************
/***********************************************************
/***********************************************************
/***********************************************************
/***********************************************************
/**********************************************************/

////////////////////////////////////////////////////////////////////////////
//							功能模块开始								  //
////////////////////////////////////////////////////////////////////////////

/*************************************************************************
*
* 函数名称：
*   PaintDIB()
*
* 参数:
*   HDC hDC            - 设备环境句柄
*   LPRECT lpDCRect    - 矩形区域
*   HDIB hDIB          - 指向DIB对象的指针
*   LPRECT lpDIBRect   - 待输出DIB区域
*   CPalette* pPal     - 调色板指针
*
* 返回值:
*   BOOL               - 绘制成功返回TRUE，否则返回FALSE。
*
* 说明:
*   绘制DIB对象
*
************************************************************************/
BOOL CDIB::Paint(HDC hDC, LPRECT lpDCRect, HDIB hDIB, LPRECT lpDIBRect, CPalette* pPal)
{
	BYTE*    lpDIBINFOHEADER;             // BITMAPINFOHEADER指针
	BYTE*    lpDIBBits;				      // DIB象素指针
	BOOL	  bRet = FALSE;				  // 成功标志
	HPALETTE  hNewPal = NULL;			  // 当前的调色板
	HPALETTE  hOldPal = NULL;			  // 以前的调色板

	// 判断DIB对象是否为空
	if (hDIB == NULL)
	{
		return FALSE;
	}

	// 锁定DIB
	lpDIBINFOHEADER = (LPBYTE) ::GlobalLock((HGLOBAL)hDIB);

	lpDIBBits = FindAddress(lpDIBINFOHEADER);

	// 获取DIB调色板，并选中
	if (pPal != NULL)
	{
		hNewPal = (HPALETTE)pPal->m_hObject;

		// 选中调色板
		hOldPal = ::SelectPalette(hDC, hNewPal, TRUE);
	}

	// 设置显示模式
	::SetStretchBltMode(hDC, COLORONCOLOR);

	// 判断是调用StretchDIBits()还是SetDIBitsToDevice()来绘制DIB对象
	if ((RECTWIDTH(lpDCRect) == RECTWIDTH(lpDIBRect)) &&
		(RECTHEIGHT(lpDCRect) == RECTHEIGHT(lpDIBRect)))
	{
		// 原始大小，不用拉伸。
		bRet = ::SetDIBitsToDevice(hDC,	// hDC
			lpDCRect->left,				// DestX
			lpDCRect->top,			    // DestY
			RECTWIDTH(lpDCRect),		// nDestWidth
			RECTHEIGHT(lpDCRect),		// nDestHeight
			lpDIBRect->left,			// SrcX
			(int)Height(lpDIBINFOHEADER) -
			lpDIBRect->top -
			RECTHEIGHT(lpDIBRect),		// SrcY
			0,							// nStartScan
			(WORD)Height(lpDIBINFOHEADER),  // nNumScans
			lpDIBBits,					// lpBits
			(LPBITMAPINFO)lpDIBINFOHEADER,  // lpBitsInfo
			DIB_RGB_COLORS);			// wUsage
	}
	else
	{
		// 非原始大小，拉伸。
		bRet = ::StretchDIBits(hDC,			// hDC
			lpDCRect->left,					// DestX
			lpDCRect->top,					// DestY
			RECTWIDTH(lpDCRect),			// nDestWidth
			RECTHEIGHT(lpDCRect),			// nDestHeight
			lpDIBRect->left,				// SrcX
			lpDIBRect->top,					// SrcY
			RECTWIDTH(lpDIBRect),			// wSrcWidth
			RECTHEIGHT(lpDIBRect),			// wSrcHeight
			lpDIBBits,						// lpBits
			(LPBITMAPINFO)lpDIBINFOHEADER,  // lpBitsInfo
			DIB_RGB_COLORS,					// wUsage
			SRCCOPY);						// dwROP
	}

	// 解除锁定
	::GlobalUnlock((HGLOBAL)hDIB);

	// 恢复以前的调色板
	if (hOldPal != NULL)
	{
		::SelectPalette(hDC, hOldPal, TRUE);
	}

	return bRet;
}

/*************************************************************************
*
* 函数名称：
*   CreateDIBPalette()
*
* 参数:
*   HDIB hDIB          - 指向DIB对象的指针
*   CPalette* pPal     - 调色板指针
*
* 返回值:
*   BOOL               - 创建成功返回TRUE，否则返回FALSE。
*
* 说明:
*   创建DIB对象调色板
*
************************************************************************/
BOOL CDIB::CreatePalette(HDIB hDIB, CPalette* pPal)
{
	// 指向逻辑调色板的指针
	LPLOGPALETTE lpPal;

	// 逻辑调色板的句柄
	HANDLE hLogPal;

	// 调色板的句柄
	HPALETTE hPal = NULL;

	int i;

	// 颜色表中的颜色数目
	int nNumColors;

	// 指向DIB的指针
	BYTE* lpbi;

	// 指向BITMAPINFO结构的指针（Win3.0）
	LPBITMAPINFO lpbmi;

	// 指向BITMAPCOREINFO结构的指针
	LPBITMAPCOREINFO lpbmc;

	// 表明是否是Win3.0 DIB的标记
	BOOL bWinStyleDIB;

	// 创建结果
	BOOL bRet = FALSE;

	// 判断DIB是否为空
	if (hDIB == NULL)
	{
		return FALSE;
	}

	// 锁定DIB
	lpbi = (LPBYTE) ::GlobalLock((HGLOBAL)hDIB);

	// 获取指向BITMAPINFO结构的指针（Win3.0）
	lpbmi = (LPBITMAPINFO)lpbi;

	// 获取指向BITMAPCOREINFO结构的指针
	lpbmc = (LPBITMAPCOREINFO)lpbi;

	// 获取DIB中颜色表中的颜色数目
	nNumColors = NumColors(lpbi);

	if (nNumColors != 0)
	{
		// 分配为逻辑调色板内存
		hLogPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE)
			+sizeof(PALETTEENTRY)
			* nNumColors);

		// 如果内存不足，退出
		if (hLogPal == 0)
		{
			// 解除锁定
			::GlobalUnlock((HGLOBAL)hDIB);

			return FALSE;
		}

		lpPal = (LPLOGPALETTE) ::GlobalLock((HGLOBAL)hLogPal);

		// 设置版本号
		lpPal->palVersion = PALVERSION;

		// 设置颜色数目
		lpPal->palNumEntries = (WORD)nNumColors;

		// 判断是否是WIN3.0的DIB
		bWinStyleDIB = IS_WIN30_DIB(lpbi);

		// 读取调色板
		for (i = 0; i < nNumColors; i++)
		{
			if (bWinStyleDIB)
			{
				// 读取红色分量
				lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;

				// 读取绿色分量
				lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;

				// 读取蓝色分量
				lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;

				// 保留位
				lpPal->palPalEntry[i].peFlags = 0;
			}
			else
			{
				// 读取红色分量
				lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;

				// 读取绿色分量
				lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;

				// 读取红色分量
				lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;

				// 保留位
				lpPal->palPalEntry[i].peFlags = 0;
			}
		}

		// 按照逻辑调色板创建调色板，并返回指针
		bRet = pPal->CreatePalette(lpPal);

		// 解除锁定
		::GlobalUnlock((HGLOBAL)hLogPal);

		// 释放逻辑调色板
		::GlobalFree((HGLOBAL)hLogPal);
	}

	// 解除锁定
	::GlobalUnlock((HGLOBAL)hDIB);

	return bRet;
}

/*************************************************************************
*
* 函数名称：
*   FindDIBBits()
*
* 参数:
*   BYTE* lpbi        - 指向DIB对象的指针
*
* 返回值:
*   BYTE*             - 指向DIB图像象素起始位置
*
* 说明:
*   返回DIB图像象素起始位置
*
************************************************************************/
BYTE* CDIB::FindAddress(BYTE* lpbi)
{
	return (lpbi + *(LPDWORD)lpbi + PaletteSize(lpbi));
}

/*************************************************************************
*
* 函数名称：
*   DIBWidth()
*
* 参数:
*   BYTE* lpbi        - 指向DIB对象的指针
*
* 返回值:
*   DWORD              - DIB位像宽度
*
* 说明:
*   获取DIB位图的宽度
*
************************************************************************/
DWORD CDIB::Width(BYTE* lpDIB)
{
	// 指向BITMAPINFO结构的指针（Win3.0）
	LPBITMAPINFOHEADER lpbmi;

	// 指向BITMAPCOREINFO结构的指针
	LPBITMAPCOREHEADER lpbmc;

	// 获取指针
	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	if (IS_WIN30_DIB(lpDIB))
	{
		// 对于Windows 3.0 DIB，返回lpbmi->biWidth
		return lpbmi->biWidth;
	}
	else
	{
		// 对于其它格式的DIB，返回lpbmc->bcWidth
		return (DWORD)lpbmc->bcWidth;
	}
}

/*************************************************************************
*
* 函数名称：
*   DIBHeight()
*
* 参数:
*   BYTE* lpDIB       - 指向DIB对象的指针
*
* 返回值:
*   DWORD              - DIB位图高度
*
* 说明:
*   获取DIB位图高度
*
************************************************************************/
DWORD CDIB::Height(BYTE* lpDIB)
{
	// 指向BITMAPINFO结构的指针（Win3.0）
	LPBITMAPINFOHEADER lpbmi;

	// 指向BITMAPCOREINFO结构的指针
	LPBITMAPCOREHEADER lpbmc;

	// 获取指针
	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	if (IS_WIN30_DIB(lpDIB))
	{
		// 对于Windows 3.0 DIB，返回lpbmi->biHeight
		return lpbmi->biHeight;
	}
	else
	{
		// 对于其它格式的DIB，返回lpbmc->bcHeight
		return (DWORD)lpbmc->bcHeight;
	}
}

/*************************************************************************
*
* 函数名称：
*   PaletteSize()
*
* 参数:
*   BYTE* lpbi        - 指向DIB对象的指针
*
* 返回值:
*   WORD               - DIB中调色板的大小
*
* 说明:
*   返回DIB调色板大小
*
************************************************************************/
WORD CDIB::PaletteSize(BYTE* lpbi)
{
	// 计算DIB中调色板的大小
	if (IS_WIN30_DIB(lpbi))
	{
		//返回颜色数目×RGBQUAD的大小
		return (WORD)(NumColors(lpbi) * sizeof(RGBQUAD));
	}
	else
	{
		//返回颜色数目×RGBTRIPLE的大小
		return (WORD)(NumColors(lpbi) * sizeof(RGBTRIPLE));
	}
}

/*************************************************************************
*
* 函数名称：
*   DIBNumColors()
*
* 参数:
*   BYTE* lpbi        - 指向DIB对象的指针
*
* 返回值:
*   WORD               - 返回调色板中颜色的种数
*
* 说明:
*   计算DIB调色板颜色数目
*
************************************************************************/
WORD CDIB::NumColors(BYTE* lpbi)
{
	int nBitCount;

	// 对于Windows的DIB, 实际颜色的数目可以比象素的位数要少。
	// 对于这种情况，则返回一个近似的数值。

	// 判断是否是WIN3.0 DIB
	if (IS_WIN30_DIB(lpbi))
	{
		// 读取dwClrUsed值
		DWORD dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;

		if (dwClrUsed != 0)
		{
			// 如果dwClrUsed（实际用到的颜色数）不为0，直接返回该值
			return (WORD)dwClrUsed;
		}
	}

	// 读取象素的位数
	if (IS_WIN30_DIB(lpbi))
	{
		// 读取biBitCount值
		nBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	}
	else
	{
		// 读取biBitCount值
		nBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;
	}

	// 按照象素的位数计算颜色数目
	switch (nBitCount)
	{
	case 1:
		return 2;

	case 4:
		return 16;

	case 8:
		return 256;
	default:
		return 0;
	}
}

/*************************************************************************
*
* 函数名称：
*   DIBBitCount()
*
* 参数:
*   BYTE* lpbi        - 指向DIB对象的指针
*
* 返回值:
*   WORD               - 返回调色板中颜色的种数
*
* 说明:
*   该函数返回DIBBitCount。
*
************************************************************************/
WORD CDIB::BitCount(BYTE* lpbi)
{
	int nBitCount;

	// 读取象素的位数
	if (IS_WIN30_DIB(lpbi))
	{
		// 读取biBitCount值
		nBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	}
	else
	{
		// 读取biBitCount值
		nBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;
	}

	return nBitCount;
}

/*************************************************************************
*
* 函数名称：
*   CopyHandle()
*
* 参数:
*   HGLOBAL h          - 要复制的内存区域
*
* 返回值:
*   HGLOBAL            - 复制后的新内存区域
*
* 说明:
*   拷贝内存块
*
************************************************************************/
HGLOBAL CDIB::CopyHandle(HGLOBAL h)
{
	if (h == NULL)
		return NULL;

	// 获取指定内存区域大小
	DWORD dwLen = ::GlobalSize((HGLOBAL)h);

	// 分配新内存空间
	HGLOBAL hCopy = ::GlobalAlloc(GHND, dwLen);

	// 判断分配是否成功
	if (hCopy != NULL)
	{
		// 锁定
		void* lpCopy = ::GlobalLock((HGLOBAL)hCopy);
		void* lp = ::GlobalLock((HGLOBAL)h);

		// 复制
		memcpy(lpCopy, lp, dwLen);

		// 解除锁定
		::GlobalUnlock(hCopy);
		::GlobalUnlock(h);
	}

	return hCopy;
}

/*************************************************************************
*
* 函数名称：
*   SaveDIB()
*
* 参数:
*   HDIB hDib          - 要保存的DIB
*   CFile& file        - 保存文件CFile
*
* 返回值:
*   BOOL               - 成功返回TRUE，否则返回FALSE或者CFileException
*
* 说明:
*   将DIB保存到指定文件中
*
*************************************************************************/
BOOL CDIB::Save(HDIB hDib, CFile& file)
{
	// Bitmap文件头
	BITMAPFILEHEADER bmfHdr;

	// 指向BITMAPINFOHEADER的指针
	LPBITMAPINFOHEADER lpBI;

	if (hDib == NULL)
	{
		// 如果DIB为空，返回FALSE
		return FALSE;
	}

	// 读取BITMAPINFO结构，并锁定
	lpBI = (LPBITMAPINFOHEADER) ::GlobalLock((HGLOBAL)hDib);

	if (lpBI == NULL)
	{
		// 为空，返回FALSE
		return FALSE;
	}

	// 判断是否是WIN3.0 DIB
	if (!IS_WIN30_DIB(lpBI))
	{
		// 不支持其它类型的DIB保存

		// 解除锁定
		::GlobalUnlock((HGLOBAL)hDib);

		return FALSE;
	}

	// 填充文件头

	// 文件类型"BM"
	bmfHdr.bfType = DIB_HEADER_MARKER;

	// 计算DIB大小时，最简单的方法是调用GlobalSize()函数。但是全局内存大小并
	// 不是DIB真正的大小，它总是多几个字节。这样就需要计算一下DIB的真实大小。

	// 文件头大小＋颜色表大小
	// （BITMAPINFOHEADER和BITMAPCOREHEADER结构的第一个DWORD都是该结构的大小）
	DWORD dwDIBSize = *(LPDWORD)lpBI + PaletteSize((LPBYTE)lpBI);

	// 计算图像大小
	if ((lpBI->biCompression == BI_RLE8) || (lpBI->biCompression == BI_RLE4))
	{
		// 对于RLE位图，没法计算大小，只能信任biSizeImage内的值
		dwDIBSize += lpBI->biSizeImage;
	}
	else
	{
		// 大小为Width * Height
		DWORD dwBmBitsSize = WIDTHBYTES((lpBI->biWidth)*((DWORD)lpBI->biBitCount)) * lpBI->biHeight;

		// 计算出DIB真正的大小
		dwDIBSize += dwBmBitsSize;

		// 更新biSizeImage（很多BMP文件头中biSizeImage的值是错误的）
		lpBI->biSizeImage = dwBmBitsSize;
	}

	// 计算文件大小：DIB大小＋BITMAPFILEHEADER结构大小
	bmfHdr.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);

	// 两个保留字
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;

	// 计算偏移量bfOffBits，它的大小为Bitmap文件头大小＋DIB头大小＋颜色表大小
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+lpBI->biSize + PaletteSize((LPBYTE)lpBI);

	// 尝试写文件
	TRY
	{
		// 写文件头
		file.Write((LPBYTE)&bmfHdr, sizeof(BITMAPFILEHEADER));

		// 写DIB头和象素
		file.Write(lpBI, dwDIBSize);
	}
	CATCH(CFileException, e)
	{
		// 解除锁定
		::GlobalUnlock((HGLOBAL)hDib);

		// 抛出异常
		THROW_LAST();
	}
	END_CATCH

		// 解除锁定
		::GlobalUnlock((HGLOBAL)hDib);

	return TRUE;
}

/*************************************************************************
*
* 函数名称：
*   ReadDIBFile()
*
* 参数:
*   CFile& file        - 要读取得文件文件CFile
*
* 返回值:
*   HDIB               - 成功返回DIB的句柄，否则返回NULL。
*
* 说明:
*   重指定文件中读取DIB对象
*
*************************************************************************/
HDIB CDIB::Read(CFile& file, BOOL bReserved)
{
	BITMAPFILEHEADER bmfHeader;
	HDIB hDIB;
	BYTE* lpDIB;

	// 获取DIB（文件）长度（字节）
	DWORD dwBitsSize = (DWORD)file.GetLength();

	// 尝试读取DIB文件头
	if (file.Read((LPBYTE)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
	{
		// 大小不对，返回NULL。
		return NULL;
	}

	// 判断是否是DIB对象，检查头两个字节是否是"BM"
	if (bmfHeader.bfType != DIB_HEADER_MARKER)
	{
		// 非DIB对象，返回NULL。
		return NULL;
	}

	// 为DIB分配内存
	hDIB = (HDIB) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize);
	if (hDIB == 0)
	{
		// 内存分配失败，返回NULL。
		return NULL;
	}

	// 锁定
	lpDIB = (LPBYTE) ::GlobalLock((HGLOBAL)hDIB);

	// 读象素
	if (file.Read(lpDIB, dwBitsSize - sizeof(BITMAPFILEHEADER)) != dwBitsSize - sizeof(BITMAPFILEHEADER))
	{
		// 大小不对。

		// 解除锁定
		::GlobalUnlock((HGLOBAL)hDIB);

		// 释放内存
		::GlobalFree((HGLOBAL)hDIB);

		return NULL;
	}

	// 解除锁定
	::GlobalUnlock((HGLOBAL)hDIB);

	return hDIB;
}