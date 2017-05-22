#pragma once

#include "UndoStack.h"

// dib.h

// DIB句柄
DECLARE_HANDLE(HDIB);

// DIB常量
#define PALVERSION   0x300

/* DIB宏 */

// 判断是否是Win 3.0的DIB
#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))

// 计算矩形区域的宽度
#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)

// 计算矩形区域的高度
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)

// 在计算图像大小时，采用公式：biSizeImage = biWidth' × biHeight。
// 是biWidth'，而不是biWidth，这里的biWidth'必须是4的整倍数，表示
// 大于或等于biWidth的，离4最近的整倍数。WIDTHBYTES就是用来计算
// biWidth'
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

class CDIB
{
public:
	CDIB();
	~CDIB();

	CImage*    m_pImage;										// 图像指针
	CUndoStack m_UndoStack;										// 堆栈:撤销
	CUndoStack m_RedoStack;										// 堆栈:恢复

	// 函数原型
	CImage*	GetImage();											// 获取图像指针
	void	SetImage(CImage*);									// 修改图像指针
	void	SetCircleColor(CRect rect, COLORREF col);			// 用指定的颜色填充指定的圆形区域
	void	SetRgnColor(CRect rect, COLORREF col);				// 用指定的颜色填充指定的区域
	BOOL	Paint(HDC hDC,LPRECT lpDCRect,LPRECT lpDIBRect);	// 在hDC中绘制图像
	BYTE*   FindAddress ();										// 获取图像首地址
	DWORD   Width();											// 获取图像宽度
	DWORD   Height();											// 获取图像高度
	DWORD	RowLen();											// 获取每行字节
	DWORD	Length();											// 获取总字节数
	WORD	Channel();											// 获取图像的通道
	WORD	BitCount();											// 获取图像颜色位数
	BOOL    Save(CString sPath);								// 保存到指定文件中
	BOOL    Read(CFile& file);									// 读取DIB对象
	BOOL	Read(CString path);									// 从路径中读取图像
	void	PushCopy();											// 存入"撤销"堆栈
	CImage* MakeCopy();											// 返回当前图像的副本
	void	Undo();												// 取出"撤销"堆栈
	void	Redo();												// 恢复之前的操作
	BOOL	RGB2Gray();											// 彩色转为灰度图像
	BOOL	ChangeBPP(UINT bpp);								// 改变图像通道数
	void	Flip(BOOL bVertical = TRUE);						// 翻转图像
	void	Clear();											// 清空图像
	void	SetBits(BYTE* pBits, int channel = 4, 
		int width = 0, int height = 0);							// 设置像素
	void	ReLoad();											// 从栈的下面几层重新加载数据
	BOOL	BinaryMap2Other(int bpp);							// 二进制图像转换为其他

private:
	void	Bpp8To24();											// 8位转24位
	void	Bpp8To32();											// 8位转32位
	void	Bpp24To8();											// 24位转8位
	void	Bpp24To32();										// 24位转32位
	void	Bpp32To8();											// 32位转8位
	void	Bpp32To24();										// 32位转24位
	void	Bpp1To8();											// 掩码转8位
	void	Bpp1To24();											// 掩码转24位
	void	Bpp1To32();											// 掩码转32位

public://重载
	BOOL	Paint(HDC hDC, LPRECT lpDCRect, HDIB hDIB, LPRECT lpDIBRect, CPalette* pPal);
	BOOL    CreatePalette(HDIB hDIB, CPalette* cPal);
	BYTE*   FindAddress(BYTE*  lpbi);
	DWORD   Width(BYTE*  lpDIB);
	DWORD   Height(BYTE*  lpDIB);
	WORD    PaletteSize(BYTE*  lpbi);
	WORD    NumColors(BYTE*  lpbi);
	WORD	BitCount(BYTE*  lpbi);
	HGLOBAL CopyHandle(HGLOBAL h);
	BOOL    Save(HDIB hDib, CFile& file);
	HDIB    Read(CFile& file, BOOL bReserved);
};
