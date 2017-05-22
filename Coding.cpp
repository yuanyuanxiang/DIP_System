// ************************************************************************
//  文件名：Coding.cpp
//
//  图像编码压缩、解压缩函数库：
//
//	LoadJPG()			- 装载JPEG图象（解压用）
//	SaveJPG()			- 保存JPEG图象（压缩用）
//	SaveBMP()			- 保存BMP图象到文件（解压用）
//	LoadBMP()			- 从文件装载BMP图象（压缩用）
//  DIBToPCX()			- 将DIB保存为PCX文件
//  PCXToDIB()			- 读取PCX文件
//	Huffman()			- 哈夫曼编码
//	Shannon_Fannon()	- 仙农-弗诺编码
//	DIBToGIF()			- 将DIB保存到GIF文件
//	LZW_Encode()		- 对图象进行LZW编码
//	OutputCode()		- 为GIF-LZW算法输出一个编码
//	GIFToDIB()			- 读取GIF到DIB
//	LZW_Decode()		- 对LZW编码进行解码
//	ReadGIF()			- 读取指定GIF文件中的图像编码
//
//*************************************************************************

#include "stdafx.h"
#include "dip_system.h"
#include "Coding.h"
#include "JPEGfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//	Dib文件头标志（字符串"BM"，写DIB时用到该常数）
#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')

/////////////////////////////////////////////////////////////////////////////
// CCoding

CCoding::CCoding()
{
}

CCoding::CCoding(CDIB* pDIB)
{
	m_pDIB = pDIB;
}

CCoding::~CCoding()
{
}

BEGIN_MESSAGE_MAP(CCoding, CWnd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCoding message handlers

/*************************************************************************
 *
 * 函数名称：
 *   SaveJPG()
 *
 * 参数:
 *   CDIP_SystemDoc* pDoc	- 文档指针
 *	 CString fileName		- 保存文件路径
 *
 * 返回值:
 *   void			    - 无返回值
 *
 * 说明:
 *   该函数对图象进行JPEG编码压缩
 *
 ************************************************************************/
void CCoding::SaveJPG(CDIP_SystemDoc* pDoc, CString fileName)
{
	// 临时缓存
	BYTE *buf;
	buf = NULL;

	// 获取文档尺寸
	CSize size = pDoc->GetDocSize();
	UINT width = size.cx;
	UINT height = size.cy;

	buf = LoadBMP(pDoc->m_strCurrentFile, &width, &height);

	// 压缩位图到JPEG文件，函数的最后参数标明压缩质量（1 - 100）
	JpegFile::RGBToJpegFile(fileName, buf, width, height, TRUE, 50);

	delete[] buf;
}

/*************************************************************************
 *
 * 函数名称：
 *   LoadJPG()
 *
 * 参数:
 *   CDIP_SystemDoc* pDoc	- 文档指针
 *	 CString fileName		- 装载文件路径
 *
 * 返回值:
 *   void			    - 无返回值
 *
 * 说明:
 *   该函数对图象进行JPEG编码解压缩
 *
 ************************************************************************/
void CCoding::LoadJPG(CDIP_SystemDoc* pDoc, CString filePath)
{
	// 缓存位图
	CString sNewPath = L"解码图像";

	// 临时缓存
	BYTE *buf;
	buf = NULL;

	// JPEG图象高度、宽度
	UINT width = 0;
	UINT height = 0;

	// 读到临时缓存
	buf = JpegFile::JpegFileToRGB(filePath, &width, &height);

	// 交换红绿象素
	JpegFile::BGRFromRGB(buf, width, height);

	// 垂直翻转显示
	JpegFile::VertFlipBuf(buf, width * 3, height);

	// 保存解压后的数据到临时位图文件
	SaveBMP(sNewPath, buf, width, height);

	// 删除指针
	delete[] buf;

	// 装载临时位图文件
	pDoc->SetTitle(sNewPath);
	pDoc->m_bIsDecoding = TRUE;
	pDoc->OnOpenDocument(sNewPath);

	// 删除临时文件
	::DeleteFile(sNewPath);

	// 初始化标记为FALSE
	pDoc->SetModifiedFlag(TRUE);
}

/*************************************************************************
 *
 * 函数名称：
 *   SaveBMP()
 *
 * 参数:
 *	 CString fileName		- 保存文件路径
 *	 BYTE* buf				- BGR缓存
 *	 UINT width				- 宽度
 *	 UINT height			- 高度
 *
 * 返回值:
 *   void			    - 无返回值
 *
 * 说明:
 *   该函数把BMP图象从内存保存到文件
 *
 ************************************************************************/
void CCoding::SaveBMP(CString fileName,	BYTE * buf,	UINT width,	UINT height)
{
	// 位图文件头
	char m1 = 'B';
	char m2 = 'M';
	short res1 = 0;
    short res2 = 0;
    long pixoff = 54;

	// 宽度
	DWORD widthDW = WIDTHBYTES(width * 24);

	// 文件长度
	long bmfsize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + widthDW * height;	
	
	// 文件指针
	long byteswritten = 0;

	// 位图信息头
	BITMAPINFOHEADER header;
  	header.biSize = 40; 		   		    // 头大小
	header.biWidth = width;
	header.biHeight = height;
	header.biPlanes = 1;
	header.biBitCount = 24;					// 24位
	header.biCompression = BI_RGB;			// 非压缩
	header.biSizeImage = 0;
	header.biXPelsPerMeter = 0;
	header.biYPelsPerMeter = 0;
	header.biClrUsed = 0;
	header.biClrImportant = 0;

	// 二进制写方式打开文件
	FILE *fp;	
	USES_CONVERSION;
	fp = fopen(W2A(fileName), "wb");

	// 写文件头
	fwrite((BYTE*)&(m1), 1, 1, fp); 
	byteswritten += 1;
	fwrite((BYTE*)&(m2), 1, 1, fp); 
	byteswritten += 1;
	fwrite((long*)&(bmfsize), 4, 1, fp);
	byteswritten += 4;
	fwrite((int*)&(res1), 2, 1, fp);
	byteswritten += 2;
	fwrite((int*)&(res2), 2, 1, fp);
	byteswritten += 2;
	fwrite((long*)&(pixoff), 4, 1, fp);
	byteswritten += 4;

	// 写位图信息头
	fwrite((BITMAPINFOHEADER *)&header, sizeof(BITMAPINFOHEADER), 1, fp);
	
	// 移动指针
	byteswritten += sizeof(BITMAPINFOHEADER);
	
	// 临时变量
	long row = 0;
	long rowidx;
	long row_size;
    long rc;

	// 行大小
	row_size = header.biWidth * 3;
	
	// 保存位图阵列
	for (row = 0; row < header.biHeight; row++)
	{
		rowidx = (long unsigned)row * row_size;						      

		// 写一行
		rc = fwrite((void *)(buf + rowidx), row_size, 1, fp);
		if (rc != 1) 
			break;
		byteswritten += row_size;	

		for (DWORD count = row_size; count < widthDW; count++) 
		{
			char dummy = 0;
			fwrite(&dummy, 1, 1, fp);
			byteswritten++;							  
		}
	}

	// 关闭文件
	fclose(fp);
}

/*************************************************************************
 *
 * 函数名称：
 *   LoadBMP()
 *
 * 参数:
 *	 CString fileName		- 装载路径
 *	 UINT width				- 宽度
 *	 UINT height			- 高度
 *
 * 返回值:
 *   BYTE*					- 读取图象缓存首地址
 *
 * 说明:
 *   该函数把BMP图象从文件装载到内存
 *
 ************************************************************************/
BYTE* CCoding::LoadBMP(CString fileName, UINT *width, UINT *height)
{
	BITMAP inBM;
	BYTE m1,m2;
    long filesize;
    short res1,res2;
    long pixoff;
    long bmisize;                    
    long compression;
    unsigned long sizeimage;
    long xscale, yscale;
    long colors;
    long impcol;
    
	// 初始化
	BYTE *outBuf = NULL;
	*width = 0;
	*height = 0;
	DWORD m_bytesRead = 0;

	// 以二进制读方式打开文件
	FILE *fp;
	USES_CONVERSION;
	fp = fopen(W2A(fileName), "rb");
	
	// 读取文件
	if (fp != NULL)
	{
		// 检验BMP标志
	    long rc;
		rc = fread((BYTE*)&(m1), 1, 1,fp);
		m_bytesRead += 1;
		if (rc == -1) 
		{
			fclose(fp); 
			return NULL;
		}

		rc = fread((BYTE*)&(m2), 1, 1,fp);
		m_bytesRead += 1;
		if ((m1!='B') || (m2!='M')) 
		{
			fclose(fp);
			return NULL;
        }
        
		// 读文件头信息
		rc = fread((long*)&(filesize), 4, 1, fp); 
		m_bytesRead += 4;

		if (rc != 1)
		{
			fclose(fp); 
			return NULL;
		}

		rc = fread((int*)&(res1), 2, 1, fp); 
		m_bytesRead += 2;

		if (rc != 1) 
		{
			fclose(fp); 
			return NULL;
		}

		rc = fread((int*)&(res2), 2, 1, fp); 
		m_bytesRead += 2;
		if (rc != 1)
		{
			fclose(fp); 
			return NULL;
		}

		rc = fread((long*)&(pixoff), 4, 1, fp);
		m_bytesRead += 4;
		if (rc != 1) 
		{ 
			fclose(fp); 
			return NULL;
		}

		rc = fread((long*)&(bmisize), 4, 1, fp);
		m_bytesRead += 4;
		if (rc != 1) 
		{
			fclose(fp); 
			return NULL;
		}

		rc = fread((long*)&(inBM.bmWidth), 4, 1, fp);
		m_bytesRead += 4;
		if (rc != 1) 
		{
			fclose(fp);
			return NULL;
		}

		rc = fread((long*)&(inBM.bmHeight), 4, 1, fp);
		m_bytesRead += 4;
		if (rc != 1)
		{
			fclose(fp); 
			return NULL;
		}

		rc = fread((int*)&(inBM.bmPlanes), 2, 1, fp); 
		m_bytesRead += 2;
		if (rc != 1) 
		{
			fclose(fp);
			return NULL;
		}

		rc = fread((int*)&(inBM.bmBitsPixel), 2, 1, fp);
		m_bytesRead += 2;
		if (rc != 1)
		{
			fclose(fp); 
			return NULL;
		}

		rc = fread((long*)&(compression), 4, 1, fp);
		m_bytesRead += 4;
		if (rc != 1) 
		{
			fclose(fp);
			return NULL;
		}

		rc = fread((long*)&(sizeimage), 4, 1, fp);
		m_bytesRead += 4;
		if (rc != 1) 
		{
			fclose(fp); 
			return NULL;
		}

		rc = fread((long*)&(xscale), 4, 1, fp);
		m_bytesRead += 4;

		if (rc != 1) 
		{
			fclose(fp);
			return NULL;
		}

		rc = fread((long*)&(yscale), 4, 1, fp); 
		m_bytesRead += 4;
		if (rc != 1)
		{
			fclose(fp);
			return NULL;
		}

		rc = fread((long*)&(colors), 4, 1, fp);
		m_bytesRead += 4;
		if (rc != 1)
		{
			fclose(fp); 
			return NULL;
		}

		rc = fread((long*)&(impcol), 4, 1, fp);
		m_bytesRead += 4;
		if (rc != 1)
		{
			fclose(fp);
			return NULL;
		}

		// 对RLE文件不做处理
		if (compression != BI_RGB) 
		{
	    	fclose(fp);
	    	return NULL;
	    }

		if (colors == 0) 
			colors = 1 << inBM.bmBitsPixel;
		
		// 读取颜色表
		RGBQUAD *colormap = NULL;

		if ((long)m_bytesRead > pixoff) 
		{
			fclose(fp);
			delete [] colormap;
			fclose(fp);
			return NULL;
		}

		while ((long)m_bytesRead < pixoff) 
		{
			char dummy;
			fread(&dummy,1,1,fp);
			m_bytesRead++;
		}

		int w = inBM.bmWidth;
		int h = inBM.bmHeight;

		// 设置输出参数
		*width = w;
		*height = h;

		long row_size = w * 3;

		long bufsize = (long)w * 3 * (long)h;

		// 分配内存
		outBuf = (BYTE*) new BYTE [bufsize];
		if (outBuf != NULL) 
		{
			long row = 0;
			long rowOffset = 0;

			// 读取点阵数据
			for (row = inBM.bmHeight - 1; row >= 0;row--) 
			{
				rowOffset = (long unsigned)row*row_size;						      

				if (inBM.bmBitsPixel == 24)
				{
					for (int col = 0; col < w; col++) 
					{
						long offset = col * 3;
						char pixel[3];

						if (fread((void  *)(pixel), 1, 3, fp) == 3)
						{
							//交换R、B信息
							*(outBuf + rowOffset + offset + 0) = pixel[2];		// r
							*(outBuf + rowOffset + offset + 1) = pixel[1];		// g
							*(outBuf + rowOffset + offset + 2) = pixel[0];		// b
						}

					}

					m_bytesRead += row_size;
					
					while ((m_bytesRead - pixoff) & 3)
					{
						char dummy;
						if (fread(&dummy, 1, 1, fp) != 1)
						{
							delete [] outBuf;
							fclose(fp);
							return NULL;
						}
						m_bytesRead++;
					}
				} 
			}
		}

		if (colormap) 
			delete [] colormap;

		fclose(fp);
    }
	return outBuf;
}

/*************************************************************************
 *
 * 函数名称：
 *   DIBToPCX()
 *
 * 参数:
 *   CString strPath	    - 要保存的文件路径
 *
 * 返回值:
 *   BOOL               - 成功返回True，否则返回False。
 *
 * 说明:
 *   该函数将DIB保存为PCX文件。
 *
 *************************************************************************/
BOOL CCoding::DIBToPCX(CString strPath)
{
	
	LONG	i;
	LONG	j;
	LONG	k;
	
	// 中间变量
	BYTE	bChar1;
	BYTE	bChar2;
	
	// 指向源图像象素的指针
	BYTE*	lpSrc;
	
	// 指向编码后图像数据的指针
	BYTE*	lpDst;
	
	// 重复像素计数
	int		iCount;
	
	// 调整顺序：B、G、R
	int	RGB[3] = {2, 1, 0};

	// 打开文件
	CFile file;
	file.Open(strPath, CFile::modeCreate | CFile::modeReadWrite);
	
	// 缓冲区已使用的字节数
	DWORD	dwBuffUsed;
	
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	if (m_pDIB->Channel() < 3)
	{
		MessageBox(L"请先将其转换为24位色(或32位色)，再进行处理！", L"系统提示" , MB_ICONINFORMATION | MB_OK);
		return FALSE;
	}
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();
	
	//*************************************************************************
	// PCX文件头
	PCXHEADER pcxHdr;
	
	// 给文件头赋值
	
	// PCX标识码
	pcxHdr.bManufacturer = 0x0A;
	
	// PCX版本号
	pcxHdr.bVersion = 5;
	
	// PCX编码方式（1表示RLE编码）
	pcxHdr.bEncoding = 1;
	
	// 像素位数（24位色为8位）
	pcxHdr.bBpp = 8;
	
	// 图像相对于屏幕的左上角X坐标（以像素为单位）
	pcxHdr.wLeft = 0;
	
	// 图像相对于屏幕的左上角Y坐标（以像素为单位）
	pcxHdr.wTop = 0;
	
	// 图像相对于屏幕的右下角X坐标（以像素为单位）
	pcxHdr.wRight = (WORD)(lWidth - 1);
	
	// 图像相对于屏幕的右下角Y坐标（以像素为单位）
	pcxHdr.wBottom = (WORD)(lHeight - 1);
		
	// 图像的水平分辨率
	pcxHdr.wXResolution = (WORD)lWidth;
	
	// 图像的垂直分辨率
	pcxHdr.wYResolution = (WORD)lHeight;

	// 保留域，设定为0。
	pcxHdr.bReserved = 0;
	
	// 图像色彩平面数目
	pcxHdr.bPlanes = 3;
	
	// 图像的宽度（字节为单位），必须为偶数。
	pcxHdr.wLineBytes = (WORD)lWidth;
	
	// 图像调色板的类型，1表示彩色或者单色图像，2表示图像是灰度图。
	pcxHdr.wPaletteType = 1;
	
	// 制作该图像的屏幕宽度（像素为单位）
	pcxHdr.wSrcWidth = 0;
	
	// 制作该图像的屏幕高度（像素为单位）
	pcxHdr.wSrcDepth = 0;
	
	// 保留域，取值设定为0。
	for (i = 0; i < 54; i ++)
	{
		pcxHdr.bFiller[i] = 0;
	}
	
	// 写入文件头
	file.Write((BYTE*)&pcxHdr, sizeof(PCXHEADER));
	
	//*******************************************************************************
	// 开始编码
	
	// 开辟一片缓冲区(2被原始图像大小)以保存编码结果
	lpDst = new BYTE[lHeight * lLineBytes * 2];
	
	// 指明当前已经用了多少缓冲区（字节数）
	dwBuffUsed = 0;
	
	// 每行
	for (i = 0; i < lHeight; i++)
	{
		for (k = 0; k < 3; k++)
		{
			// 指向DIB第i行，第0个象素的指针
			lpSrc = (BYTE*)(lpDIBBits + lLineBytes * (lHeight - 1 - i) + RGB[k]);
			
			// 给bChar1赋值
			bChar1 = *lpSrc;
			
			// 设置iCount为1
			iCount = 1;
		
			// 剩余列
			for (j = 3; j < lLineBytes; j += 3)
			{
				// 指向DIB第i行，第j个象素的指针
				bChar2 = *(lpSrc + j);

				// 判断是否和bChar1相同并且iCount < 63
				if ((bChar1 == bChar2) && (iCount < 63))
				{
					// 相同，计数加1
					iCount ++;
				}	
				else// 不同，或者iCount = 63
				{
					// 写入缓冲区
					if ((iCount > 1) || (bChar1 >= 0xC0))
					{
						// 保存码长信息
						lpDst[dwBuffUsed] = iCount | 0xC0;
					
						// 保存bChar1
						lpDst[dwBuffUsed + 1] = bChar1;
					
						// 更新dwBuffUsed
						dwBuffUsed += 2;
					}
					else
					{
						// 直接保存该值
						lpDst[dwBuffUsed] = bChar1;
					
						// 更新dwBuffUsed
						dwBuffUsed ++;
					}
				
					// 重新给bChar1赋值
					bChar1 = bChar2;
				
					// 设置iCount为1
					iCount = 1;
				}
			}
	
			// 保存每行最后一部分编码
			if ((iCount > 1) || (bChar1 >= 0xC0))
			{
				// 保存码长信息
				lpDst[dwBuffUsed] = iCount | 0xC0;
			
				// 保存bChar1
				lpDst[dwBuffUsed + 1] = bChar1;
				
				// 更新dwBuffUsed
				dwBuffUsed += 2;
			}
			else
			{
				// 直接保存该值
				lpDst[dwBuffUsed] = bChar1;
			
				// 更新dwBuffUsed
				dwBuffUsed ++;
			}
		}
	}

	// 写入编码结果
	file.Write((BYTE*)lpDst, dwBuffUsed);
	
	// 释放内存
	delete[] lpDst;
	
	// 关闭文件
	file.Close();

	EndWaitCursor();

	return TRUE;
}

/*************************************************************************
 *
 * 函数名称：
 *   PCXToDIB()
 *
 * 参数:
 *   CString strPath	    - 要装载的文件路径
 *
 * 返回值:
 *   HDIB		            - 成功返回DIB的句柄，否则返回NULL。
 *
 * 说明:
 *   该函数将读取指定的PCX文件。读取的结果保存在DIB对象中。
 *
 *************************************************************************/
HDIB CCoding::PCXToDIB(CString strPath)
{
	// PCX文件头
	PCXHEADER pcxHdr;
	
	// DIB大小（字节数）
	DWORD	dwDIBSize;
	
	// DIB句柄
	HDIB	hDIB;
	
	// DIB指针
	BYTE*	pDIB;
	
	LONG	i;
	LONG	j;
	LONG	k;
	LONG    l;

	// 重复像素计数
	int		iCount;
	
	// 调整RGB顺序
	int	RGB[3] = {2, 1, 0};
	
	// 中间变量
	BYTE	bChar;
	
	// 指向源图像象素的指针
	BYTE *	lpSrc;
	
	// 指向编码后图像数据的指针
	BYTE *	lpDst;
	
	// 临时指针
	BYTE *	lpTemp;

	// 打开文件
	CFile file;
	file.Open(strPath, CFile::modeReadWrite);
	
	// 尝试读取PCX文件头
	if (file.Read((BYTE*)&pcxHdr, sizeof(PCXHEADER)) != sizeof(PCXHEADER))
	{
		// 大小不对，返回NULL。
		return NULL;
	}
	
	// 判断是否是24位色PCX文件，检查第一个字节是否是0x0A，
	if ((pcxHdr.bManufacturer != 0x0A) || (pcxHdr.bBpp != 8) || (pcxHdr.bPlanes != 3))
	{
		// 非24位色PCX文件，返回NULL。
		return NULL;
	}
	
	// 获取图像高度
	LONG lHeight = pcxHdr.wBottom - pcxHdr.wTop + 1;
	
	// 获取图像宽度
	LONG lWidth  = pcxHdr.wRight - pcxHdr.wLeft + 1;
	
	LONG lLineBytes = m_pDIB->RowLen();
	
	// 计算DIB长度（字节）
	dwDIBSize = sizeof(BITMAPINFOHEADER) + 1024 + lHeight * lLineBytes;
	
	// 为DIB分配内存
	hDIB = (HDIB) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwDIBSize);
	if (hDIB == 0)
	{
		// 内存分配失败，返回NULL。
		return NULL;
	}
	
	// 锁定
	pDIB = (BYTE*) ::GlobalLock((HGLOBAL) hDIB);
	
	// 指向BITMAPINFOHEADER的指针
	LPBITMAPINFOHEADER lpBI;
	
	// 赋值
	lpBI = (LPBITMAPINFOHEADER) pDIB;
	
	// 给lpBI成员赋值
	lpBI->biSize = 40;
	lpBI->biWidth = lWidth;
	lpBI->biHeight = lHeight;
	lpBI->biPlanes = 1;
	lpBI->biBitCount = 24;
	lpBI->biCompression = BI_RGB;
	lpBI->biSizeImage = lHeight * lLineBytes;
	lpBI->biXPelsPerMeter = pcxHdr.wXResolution;
	lpBI->biYPelsPerMeter = pcxHdr.wYResolution;
	lpBI->biClrUsed = 0;
	lpBI->biClrImportant = 0;
	
	// 分配内存以读取编码后的象素
	lpSrc = new BYTE[file.GetLength() - sizeof(PCXHEADER) - 769];
	lpTemp = lpSrc;
	
	// 读取编码后的象素
	if (file.Read(lpSrc, file.GetLength() - sizeof(PCXHEADER) - 769) !=
		file.GetLength() - sizeof(PCXHEADER) - 769 )
	{
		// 大小不对。
		
		// 释放内存
		::GlobalFree((HGLOBAL) hDIB);
		
		return NULL;
	}
	
	// 计算DIB中像素位置
	m_pDIB->m_pImage->Create(lWidth, lHeight, 24);
	lpDst = (BYTE *) m_pDIB->FindAddress();
	
	// 一行一行解码
	for (j = 0; j < lHeight; j++)
	{
		for (k = 0; k < 3; k++)
		{
			i = 0;
			while (i < lWidth)
			{
				// 读取一个字节
				bChar = *lpTemp;
				lpTemp++;
				
				if ((bChar & 0xC0) == 0xC0)
				{
					// 行程
					iCount = bChar & 0x3F;
				
					// 读取下一个字节
					bChar = *lpTemp;
					lpTemp++;
				
					// bChar重复iCount次保存
					for (l = 0; l < iCount; l++)
						*(lpDst + (lHeight - j - 1) * lLineBytes + (i + l) * 3 + RGB[k]) = bChar;

					// 已经读取像素的个数加iCount
					i += iCount;
				}
				else
				{
					// 保存当前字节
					*(lpDst + (lHeight - j - 1) * lLineBytes + i * 3 + RGB[k]) = bChar;

					// 已经读取像素的个数加1
					i += 1;
				}
			}
		}
	}
	
	// 释放内存
	delete lpSrc;
 
	// 关闭文件
	file.Close();

	return hDIB;
}

// 功能待添加
BOOL CCoding::Huffman(CString strPath)
{
	return TRUE;
}

// 功能待添加
BOOL CCoding::Shannon_Fannon(CString strPath)
{
	return TRUE;
}

/*************************************************************************
 *
 * 函数名称：
 *   DIBToGIF()
 *
 * 参数:
 *   HDIB hDIB	        - DIB句柄
 *   CFile& file        - 要保存的文件
 *   BOOL bInterlace	- 保存方式
 *
 * 返回值:
 *   BOOL               - 成功返回True，否则返回False。
 *
 * 说明:
 *   该函数将256色DIB位图保存为GIF文件。
 *
 *************************************************************************/
BOOL CCoding::DIBToGIF(HDIB hDIB, CFile &file, BOOL bInterlace)
{
	
	LONG	i;
	LONG	j;

	// 锁定DIB
	LPBYTE lpDIB = (LPBYTE) ::GlobalLock((HGLOBAL)hDIB);

	// 判断是否是8-bpp位图
	if (m_pDIB->BitCount(lpDIB) != 8)
	{
		
		MessageBox(L"请先将其转换为8位色位图，再进行处理！", L"系统提示", MB_ICONINFORMATION | MB_OK);

		// 解除锁定
		::GlobalUnlock((HGLOBAL)hDIB);

		return FALSE;
	}

	// GIF文件头
	GIFHEADER          GIFH;

	// GIF逻辑屏幕描述块
	GIFSCRDESC         GIFS;

	// GIF图像描述块
	GIFIMAGE           GIFI;

	// GIF编码参数
	GIFC_VAR           GIFCVar;

	// 调色板
	BYTE               byGIF_Pal[768];

	// 字节变量
	BYTE               byChar;

	// 指向BITMAPINFO结构的指针（Win3.0）
	LPBITMAPINFO	   lpbmi;

	// 指向BITMAPCOREINFO结构的指针
	LPBITMAPCOREINFO   lpbmc;

	// 表明是否是Win3.0 DIB的标记
	BOOL			   bWinStyleDIB;

	BeginWaitCursor();

	// 获取DIB高度
	LONG lHeight = m_pDIB->Height(lpDIB);

	// 获取DIB宽度
	LONG lWidth = m_pDIB->Width(lpDIB);

	LPBYTE lpDIBBits = m_pDIB->FindAddress(lpDIB);

	// 计算每行字节数
	LONG lWidthBytes = DWORD_WBYTES(lWidth * 8);

	// 给GIFCVar结构赋值
	GIFCVar.wWidth = (WORD)lWidth;
	GIFCVar.wDepth = (WORD)lHeight;
	GIFCVar.wBits = m_pDIB->BitCount(lpDIB);
	GIFCVar.wLineBytes = (WORD)BYTE_WBYTES((DWORD)GIFCVar.wWidth * (DWORD)GIFCVar.wBits);

	// 计算颜色数目
	LONG lColors = 1 << GIFCVar.wBits;

	// 获取指向BITMAPINFO结构的指针（Win3.0）
	lpbmi = (LPBITMAPINFO)lpDIB;

	// 获取指向BITMAPCOREINFO结构的指针
	lpbmc = (LPBITMAPCOREINFO)lpDIB;

	// 判断是否是WIN3.0的DIB
	bWinStyleDIB = IS_WIN30_DIB(lpDIB);

	// 给调色板赋值
	if (bWinStyleDIB)
	{
		j = 0;
		for (i = 0; i < lColors; i++)
		{
			// 读取红色分量
			byGIF_Pal[j++] = lpbmi->bmiColors[i].rgbRed;

			// 读取绿色分量
			byGIF_Pal[j++] = lpbmi->bmiColors[i].rgbGreen;

			// 读取蓝色分量
			byGIF_Pal[j++] = lpbmi->bmiColors[i].rgbBlue;
		}
	}
	else
	{
		j = 0;
		for (i = 0; i < lColors; i++)
		{
			// 读取红色分量
			byGIF_Pal[j++] = lpbmc->bmciColors[i].rgbtRed;

			// 读取绿色分量
			byGIF_Pal[j++] = lpbmc->bmciColors[i].rgbtGreen;

			// 读取红色分量
			byGIF_Pal[j++] = lpbmc->bmciColors[i].rgbtBlue;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// 开始写GIF文件

	// 写GIF文件头
	GIFH.bySignature[0] = 'G';
	GIFH.bySignature[1] = 'I';
	GIFH.bySignature[2] = 'F';
	GIFH.byVersion[0] = '8';
	GIFH.byVersion[1] = '9';
	GIFH.byVersion[2] = 'a';
	file.Write((LPBYTE)&GIFH, 6);

	// 写GIF逻辑屏幕描述块
	GIFS.wWidth = GIFCVar.wWidth;
	GIFS.wDepth = GIFCVar.wDepth;
	GIFS.GlobalFlag.PalBits = GIFCVar.wBits - 1;
	GIFS.GlobalFlag.SortFlag = 0x00;
	GIFS.GlobalFlag.ColorRes = GIFCVar.wBits - 1;
	GIFS.GlobalFlag.GlobalPal = 0x01;
	GIFS.byBackground = 0x00;
	GIFS.byAspect = 0x00;
	file.Write(&GIFS, 7);

	// 写GIF全局调色板
	file.Write(byGIF_Pal, (lColors * 3));

	// 写GIF图像描述间隔符
	byChar = 0x2C;
	file.Write(&byChar, 1);

	// 写GIF图像描述块
	GIFI.wLeft = 0;
	GIFI.wTop = 0;
	GIFI.wWidth = GIFCVar.wWidth;
	GIFI.wDepth = GIFCVar.wDepth;
	GIFI.LocalFlag.PalBits = 0x00;
	GIFI.LocalFlag.Reserved = 0x00;
	GIFI.LocalFlag.SortFlag = 0x00;
	GIFI.LocalFlag.Interlace = bInterlace ? 0x01 : 0x00;
	GIFI.LocalFlag.LocalPal = 0x00;
	file.Write(&GIFI, 9);

	// 写GIF图像压缩数据
	HANDLE hSrcBuff = GlobalAlloc(GHND, (DWORD)MAX_BUFF_SIZE);
	GIFCVar.lpDataBuff = (LPBYTE)GlobalLock(hSrcBuff);
	GIFCVar.lpEndBuff = GIFCVar.lpDataBuff;
	GIFCVar.dwTempCode = 0UL;
	GIFCVar.wByteCnt = 0;
	GIFCVar.wBlockNdx = 1;
	GIFCVar.byLeftBits = 0x00;

	// 进行GIF_LZW编码
	LZW_Encode(lpDIBBits, file, &GIFCVar, (WORD)lWidthBytes, bInterlace);

	// 判断是否编码成功
	if (GIFCVar.wByteCnt)
	{
		// 写入文件
		file.Write(GIFCVar.lpDataBuff, GIFCVar.wByteCnt);
	}

	// 释放内存
	GlobalUnlock(hSrcBuff);
	GlobalFree(hSrcBuff);

	// 写GIF Block Terminator
	byChar = 0x00;
	file.Write(&byChar, 1);

	// 写GIF文件结尾块
	byChar = 0x3B;
	file.Write(&byChar, 1);

	// 解除锁定
	::GlobalUnlock((HGLOBAL)hDIB);

	EndWaitCursor();

	return TRUE;
}

/*************************************************************************
 *
 * 函数名称：
 *   LZW_Encode()
 *
 * 参数:
 *   BYTE* lpDIBBits		- 指向源DIB图像指针
 *   CFile& file			- 要保存的文件
 *   LPGIFC_VAR lpGIFCVar	- 指向GIFC_VAR结构的指针
 *	 WORD wWidthBytes		- 每行图像字节数
 *	 BOOL bInterlace		- 是否按照交错方式保存
 *
 * 返回值:
 *   无
 *
 * 说明:
 *   该函数对图像进行GIF_LZW编码。
 *
 *************************************************************************/
void CCoding::LZW_Encode(BYTE* lpDIBBits, CFile &file, LPGIFC_VAR lpGIFCVar, WORD wWidthBytes, BOOL bInterlace)
{
	// 内存分配句柄
	HANDLE hTableNdx;
	HANDLE hPrefix;
	HANDLE hSuffix;
	
	// 指向字串表指针
	LPWORD lpwTableNdx;
	
	// 用于字串表搜索的索引
	LPWORD lpwPrefix;
	BYTE* lpbySuffix;
	
	// 指向当前编码像素的指针
	BYTE*  lpImage;
	
	// 计算当前数据图像的偏移量
	DWORD  dwDataNdx;
	
	// LZW_CLEAR
	WORD   wLZW_CLEAR;
	
	// LZW_EOI
	WORD   wLZW_EOI;
	
	// LZW_MinCodeLen
	BYTE   byLZW_MinCodeLen;
	
	// 字串表索引
	WORD   wPreTableNdx;
	WORD   wNowTableNdx;
	WORD   wTopTableNdx;
	
	// 哈希表索引
	WORD   wHashNdx;
	WORD   wHashGap;
	WORD   wPrefix;
	WORD   wShiftBits;
	
	// 当前图像的行数
	WORD   wRowNum;
	
	WORD   wWidthCnt;
	
	WORD   wi;
	WORD   wj;
	
	// 交错方式存储时每次增加的行数
	WORD   wIncTable[5]  = {8, 8, 4, 2, 0}; 
	
	// 交错方式存储时起始行数
	WORD   wBgnTable[5]  = {0, 4, 2, 1, 0 }; 
	
	BOOL   bStart;
	BYTE   bySuffix;
	BYTE   bySubBlock[256];
	BYTE   byCurrentBits;
	BYTE   byMask;
	BYTE   byChar;
	BYTE   byPass;
	
	// 临时字节变量
	BYTE   byTemp;
	
	// 给字串表分配内存
	hTableNdx        = GlobalAlloc(GHND,(DWORD)(MAX_HASH_SIZE<<1));
	hPrefix          = GlobalAlloc(GHND,(DWORD)(MAX_HASH_SIZE<<1));
	hSuffix          = GlobalAlloc(GHND,(DWORD)MAX_HASH_SIZE);
	
	// 锁定内存
	lpwTableNdx      = (LPWORD)GlobalLock(hTableNdx);
	lpwPrefix        = (LPWORD)GlobalLock(hPrefix);
	lpbySuffix       = (BYTE*)GlobalLock(hSuffix);
	
	// 计算LZW_MinCodeLen
	byLZW_MinCodeLen = (lpGIFCVar->wBits>1) ? lpGIFCVar->wBits : 0x02;
	
	// 写GIF LZW最小代码大小
	file.Write(&byLZW_MinCodeLen, 1);
	
	wRowNum          = 0;
	bStart           = TRUE;
	byPass           = 0x00;
	
	// 计算LZW_CLEAR
	wLZW_CLEAR       = 1 << byLZW_MinCodeLen;
	
	// 计算LZW_EOI
	wLZW_EOI         = wLZW_CLEAR + 1;
	
	// 初始化字串表
	byCurrentBits    = byLZW_MinCodeLen + 0x01;
	wNowTableNdx     = wLZW_CLEAR + 2;
	wTopTableNdx     = 1 << byCurrentBits;
	for(wi = 0; wi < MAX_HASH_SIZE; wi++)
	{
		// 初始化为0xFFFF
		*(lpwTableNdx + wi) = 0xFFFF;
	}
	
	// 输出LZW_CLEAR
	OutputCode(file, wLZW_CLEAR, bySubBlock, &byCurrentBits, lpGIFCVar);
	
	// 逐行编码
	for(wi=0; wi < lpGIFCVar->wDepth; wi++)
	{
		// 计算当前偏移量
		dwDataNdx  = (DWORD)(lpGIFCVar->wDepth - 1 - wRowNum) * (DWORD)wWidthBytes;
		
		// 指向当前行图像的指针
		lpImage    = lpDIBBits + dwDataNdx;
		
		wWidthCnt  = 0;
		wShiftBits = 8 - lpGIFCVar->wBits;
		byMask     = (lpGIFCVar->wBits == 1) ? 0x80 : 0xF0;
		
		if (bStart)
		{
			// 判断是否是256色位图（一个像素一字节）
			if (lpGIFCVar->wBits == 8)
			{
				// 256色，直接赋值即可
				byTemp = *lpImage++;
			}
			else
			{
				// 非256色，需要移位获取像素值
				wShiftBits = 8 - lpGIFCVar->wBits;
				byMask = (lpGIFCVar->wBits == 1) ? 0x80 : 0xF0;
				byTemp = (*lpImage & byMask) >> wShiftBits;
				byMask >>= lpGIFCVar->wBits;
				wShiftBits -= lpGIFCVar->wBits;
			}
			wPrefix  = (WORD)byTemp;
			bStart = FALSE;
			wWidthCnt++;
		}
		
		// 每行编码
		while(wWidthCnt < lpGIFCVar->wWidth)
		{
			// 判断是否是256色位图（一个像素一字节）
			if (lpGIFCVar->wBits == 8)
			{
				// 256色，直接赋值即可
				byTemp = *lpImage++;
			}
			else
			{
				// 非256色，需要移位获取像素值
				byChar = *lpImage;
				byTemp = (byChar & byMask) >> wShiftBits;
				if (wShiftBits)
				{
					byMask >>= lpGIFCVar->wBits;
					wShiftBits -= lpGIFCVar->wBits;
				}
				else
				{
					wShiftBits = 8 - lpGIFCVar->wBits;
					byMask = (lpGIFCVar->wBits==1) ? 0x80 : 0xF0;
					lpImage++;
				}
			}
			bySuffix = byTemp;
			wWidthCnt ++;
			
			// 查找当前字符串是否存在于字串表中
			wHashNdx = wPrefix ^ ((WORD)bySuffix << 4);
			wHashGap = (wHashNdx ? (MAX_HASH_SIZE - wHashNdx) : 1);
			
			// 判断当前字符串是否在字串表中
			while(TRUE)
			{
				// 当前字符串不在字串表中
				if (*(lpwTableNdx + wHashNdx) == 0xFFFF)
				{
				   // 新字符串，退出循环
				   break;
				}
				
				// 判断是否找到该字符串
				if ((*(lpwPrefix+wHashNdx)  == wPrefix) &&
					(*(lpbySuffix+wHashNdx) == bySuffix))
				{
					// 找到，退出循环
					break;
				}
				
				// 第二哈希表
				if (wHashNdx < wHashGap)
				{
					wHashNdx += MAX_HASH_SIZE;
				}
				wHashNdx -= wHashGap;
			}
			
			// 判断是否是新字符串
			if (*(lpwTableNdx+wHashNdx) != 0xFFFF)
			{
				// 不是新字符串
				wPrefix = *(lpwTableNdx + wHashNdx);
			}
			else
			{
				// 新字符串
				
				// 输出该编码
				OutputCode(file, wPrefix, bySubBlock, &byCurrentBits, lpGIFCVar);
				
				// 将该新字符串添加到字串表中
				wPreTableNdx = wNowTableNdx;
				
				// 判断是否达到最大字串表大小
				if (wNowTableNdx < MAX_TABLE_SIZE)
				{
					*(lpwTableNdx+wHashNdx) = wNowTableNdx++;
					*(lpwPrefix+wHashNdx)   = wPrefix;
					*(lpbySuffix+wHashNdx)  = bySuffix;
				}
				
				if (wPreTableNdx == wTopTableNdx)
				{
					if (byCurrentBits<12)
					{
						byCurrentBits ++;
						wTopTableNdx <<= 1;
					}
					else
					{
						// 字串表到达最大长度
						
						// 输出LZW_CLEAR
						OutputCode(file, wLZW_CLEAR, bySubBlock, &byCurrentBits, lpGIFCVar);
						
						// 重新初始化字串表
						byCurrentBits    = byLZW_MinCodeLen + 0x01;
						wLZW_CLEAR       = 1 << byLZW_MinCodeLen;
						wLZW_EOI         = wLZW_CLEAR + 1;
						wNowTableNdx     = wLZW_CLEAR + 2;
						wTopTableNdx     = 1 << byCurrentBits;
						for(wj = 0; wj < MAX_HASH_SIZE; wj++)
						{
							// 初始化为0xFFFF
							*(lpwTableNdx+wj) = 0xFFFF;
						}
					}
				}
				wPrefix = (WORD)bySuffix;
			}
		}
		
		// 判断是否是交错方式
		if (bInterlace)
		{
			// 交错方式，计算下一行位置
			wRowNum += wIncTable[byPass];
			if (wRowNum >= lpGIFCVar->wDepth)
			{
				byPass++;
				wRowNum = wBgnTable[byPass];
			}
		}
		else
		{
			// 非交错方式，直接将行数加一即可
			wRowNum ++;
		}
	}
	
	// 输出当前编码
	OutputCode(file, wPrefix, bySubBlock, &byCurrentBits, lpGIFCVar);
	
	// 输出LZW_EOI
	OutputCode(file, wLZW_EOI, bySubBlock, &byCurrentBits, lpGIFCVar);
	
	if (lpGIFCVar->byLeftBits)
	{
		// 加入该字符
		bySubBlock[lpGIFCVar->wBlockNdx++] = (BYTE)lpGIFCVar->dwTempCode;
		
		// 判断是否超出MAX_SUBBLOCK_SIZE
		if (lpGIFCVar->wBlockNdx > MAX_SUBBLOCK_SIZE)
		{
			// 判断wByteCnt + 256是否超过MAX_BUFF_SIZE
			if ((lpGIFCVar->wByteCnt + 256) >= MAX_BUFF_SIZE)
			{
				// 输出
				file.Write(lpGIFCVar->lpDataBuff, lpGIFCVar->wByteCnt);
				lpGIFCVar->lpEndBuff = lpGIFCVar->lpDataBuff;
				lpGIFCVar->wByteCnt  = 0;
			}
			bySubBlock[0]           = (lpGIFCVar->wBlockNdx - 1);
			memcpy(lpGIFCVar->lpEndBuff, bySubBlock, lpGIFCVar->wBlockNdx);
			lpGIFCVar->lpEndBuff   += lpGIFCVar->wBlockNdx;
			lpGIFCVar->wByteCnt    += lpGIFCVar->wBlockNdx;
			lpGIFCVar->wBlockNdx    = 1;
		}
		lpGIFCVar->dwTempCode = 0UL;
		lpGIFCVar->byLeftBits = 0x00;
	}
	
	if (lpGIFCVar->wBlockNdx > 1)
	{
		// 判断wByteCnt + 256是否超过MAX_BUFF_SIZE
		if ((lpGIFCVar->wByteCnt + 256) >= MAX_BUFF_SIZE)
		{
			// 输出
			file.Write(lpGIFCVar->lpDataBuff, lpGIFCVar->wByteCnt);
			lpGIFCVar->lpEndBuff = lpGIFCVar->lpDataBuff;
			lpGIFCVar->wByteCnt  = 0;
		}
		bySubBlock[0]           = lpGIFCVar->wBlockNdx - 1;
		memcpy(lpGIFCVar->lpEndBuff, bySubBlock, lpGIFCVar->wBlockNdx);
		lpGIFCVar->lpEndBuff   += lpGIFCVar->wBlockNdx;
		lpGIFCVar->wByteCnt    += lpGIFCVar->wBlockNdx;
		lpGIFCVar->wBlockNdx    = 1;
	}
	
	GlobalUnlock(hTableNdx);
	GlobalUnlock(hPrefix);
	GlobalUnlock(hSuffix);
	
	// 释放内存
	GlobalFree(hTableNdx);
	GlobalFree(hPrefix);
	GlobalFree(hSuffix);
	
	// 退出
	return;
}

/*************************************************************************
 *
 * 函数名称：
 *   OutputCode()
 *
 * 参数:
 *   CFile& file			- 要保存的文件
 *	 WORD wCode				- 要添加的编码
 *   BYTE* lpbySubBlock	- 子块
 *	 BYTE* lpbyCurrentBits	- 当前位数
 *	 LPGIFC_VAR lpGIFCVar	- 指向GIFC_VAR结构的指针
 *
 * 返回值:
 *   无
 *
 * 说明:
 *   该函数用来输出一个编码。
 *
 *************************************************************************/
void CCoding::OutputCode(CFile &file, WORD wCode, BYTE* lpbySubBlock, BYTE* lpbyCurrentBits, LPGIFC_VAR lpGIFCVar)
{
	// 输出该编码
	lpGIFCVar->dwTempCode |= ((DWORD)wCode << lpGIFCVar->byLeftBits);
	lpGIFCVar->byLeftBits += (*lpbyCurrentBits);
	
	while(lpGIFCVar->byLeftBits >= 0x08)
	{
		lpbySubBlock[lpGIFCVar->wBlockNdx++] = (BYTE)lpGIFCVar->dwTempCode;
      
		// 判断是否超出MAX_SUBBLOCK_SIZE
		if (lpGIFCVar->wBlockNdx > MAX_SUBBLOCK_SIZE)
		{
			// 判断wByteCnt + 256是否超过MAX_BUFF_SIZE
			if ((lpGIFCVar->wByteCnt + 256) >= MAX_BUFF_SIZE)
			{
				// 输出
				file.Write(lpGIFCVar->lpDataBuff, lpGIFCVar->wByteCnt);
	            lpGIFCVar->lpEndBuff = lpGIFCVar->lpDataBuff;
		        lpGIFCVar->wByteCnt  = 0;
			}
			lpbySubBlock[0] = lpGIFCVar->wBlockNdx - 1;
			memcpy(lpGIFCVar->lpEndBuff, lpbySubBlock, lpGIFCVar->wBlockNdx);
			lpGIFCVar->lpEndBuff += lpGIFCVar->wBlockNdx;
			lpGIFCVar->wByteCnt += lpGIFCVar->wBlockNdx;
			lpGIFCVar->wBlockNdx = 1;
		}
		lpGIFCVar->dwTempCode >>= 8;
		lpGIFCVar->byLeftBits -= 0x08;
	}
	
	return;
}

/*************************************************************************
 *
 * 函数名称：
 *   GIFToDIB()
 *
 * 参数:
 *   CFile& file        - 要读取的文件
 *
 * 返回值:
 *   HDIB               - 成功返回DIB的句柄，否则返回NULL。
 *
 * 说明:
 *   该函数读取GIF文件到DIB
 *
 *************************************************************************/
HDIB CCoding::GIFToDIB(CFile &file)
{	
	// GIF文件头
	GIFHEADER          GIFH;
	
	// GIF逻辑屏幕描述块
	GIFSCRDESC         GIFS;
	
	// GIF图像描述块
	GIFIMAGE           GIFI;
	
	// GIF图像控制扩充块
	GIFCONTROL         GIFC;
	
	// GIF图像说明扩充块
	GIFPLAINTEXT       GIFP;
	
	// GIF应用程序扩充块
	GIFAPPLICATION     GIFA;
	
	// GIF编码参数
	GIFD_VAR           GIFDVar;
	
	// 颜色数目
	WORD               wColors;
	
	// 每行字节数
	WORD               wWidthBytes;
	
	// 调色板
	BYTE               byGIF_Pal[768];
	
	// 16色系统调色板
	BYTE               bySysPal16[48] = {	0,   0,   0,   0,   0, 128,
											0, 128,   0,   0, 128, 128,
										  128,   0,   0, 128,   0, 128,
										  128, 128,   0, 128, 128, 128,
										  192, 192, 192,   0,   0, 255,
											0, 255,   0,   0, 255, 255,
										  255,   0,   0, 255,   0, 255,
										  255, 255,   0, 255, 255, 255};
	
	// DIB大小(字节数)
	DWORD              dwDIB_Size;	
	
	// 调色板大小(字节数)
	WORD               wPalSize;
	
	// 字节变量
	BYTE               byTemp;
	
	// 内存句柄
	HANDLE			   hSrcBuff;
	HANDLE             hTemp;
	
	// 内存指针
	BYTE*              lpTemp;
	
	// 字变量
	WORD               wTemp;
	
	WORD               wi;
	
	// 标签
	BYTE               byLabel;
	
	// 块大小
	BYTE               byBlockSize;
	
	// 读取GIF文件头
	file.Read((BYTE*)&GIFH, sizeof(GIFH));
	
	// 判断是否是GIF文件
	if (memicmp((BYTE*)GIFH.bySignature,"GIF",3) != 0)
	{
		// 非GIF文件，返回NULL
		return NULL;
	}
	
	// 判断版本号是否正确
	if ((memicmp((BYTE*)GIFH.byVersion,"87a",3) != 0) &&
		(memicmp((BYTE*)GIFH.byVersion,"89a",3) != 0))
	{
		// 不支持该版本，返回NULL
		return NULL;
	}
	
	// 读取GIF逻辑屏幕描述块
	file.Read((BYTE*)&GIFS, 7);
	
	// 获取调色板的位数
	GIFDVar.wBits = (WORD)GIFS.GlobalFlag.PalBits + 1;
	
	// 判断是否有全局调色板
	if (GIFS.GlobalFlag.GlobalPal)
	{
		// 赋初值
		memset((BYTE*)byGIF_Pal,0,768);
		
		// 全局调色板大小
		wPalSize = 3 * (1 << GIFDVar.wBits);
		
		// 读取全局调色板
		file.Read((BYTE*)byGIF_Pal,wPalSize);
	}
	
	// 读取下一个字节
	file.Read((BYTE*)&byTemp,1);
	
	// 对每一个描述块循环
	while(TRUE)
	{
		// 判断是否是图像描述块
		if (byTemp == 0x2C)
		{
			// 是图像描述块，退出循环
			break;
		}
		
		// 判断是否是GIF扩展块
		if (byTemp==0x21)
		{
			// 是GIF扩展块
			
			// 分配内存
			hTemp  = GlobalAlloc(GHND, (DWORD)MAX_BUFF_SIZE);
			
			// 锁定内存
			lpTemp = (BYTE*) GlobalLock(hTemp);
			
			// 读取下一个字节
			file.Read((BYTE*)&byLabel, 1);
			
			// 针对各种扩充块，进行分别处理
			switch(byLabel)
			{
				case 0xF9:
				{
					// 图像控制扩充块
					file.Read((BYTE*)&GIFC, 6);
					
					// 跳出
					break;
				}
				case 0x01:
				{
					// 图像说明扩充块
					file.Read((BYTE*)&GIFP, sizeof(GIFP));
					
					// 读取扩充块大小
					file.Read((BYTE*)&byBlockSize, 1);
					
					// 当byBlockSize > 0时循环读取
					while(byBlockSize)
					{
						// 读取图像说明扩充块（这里没有进行任何处理）
						file.Read(lpTemp, byBlockSize);
						
						// 读取扩充块大小
						file.Read((BYTE*)&byBlockSize, 1);
					}
					
					// 跳出
					break;
				}
				case 0xFE:
				{
					// 注释说明扩充块
					
					// 读取扩充块大小
					file.Read((BYTE*)&byBlockSize, 1);
					
					// 当byBlockSize > 0时循环读取
					while(byBlockSize)
					{
						// 读取注释说明扩充块（这里没有进行任何处理）
						file.Read(lpTemp, byBlockSize);
					
						// 读取扩充块大小
						file.Read((BYTE*)&byBlockSize, 1);
					}
					
					// 跳出
					break;
				}
				case 0xFF:
				{
					// 应用程序扩充块
					file.Read((BYTE*)&GIFA, sizeof(GIFA));
					
					// 读取扩充块大小
					file.Read((BYTE*)&byBlockSize, 1);
					
					// 当byBlockSize > 0时循环读取
					while(byBlockSize)
					{
						// 读取应用程序扩充块（这里没有进行任何处理）
						file.Read(lpTemp, byBlockSize);
						
						// 读取扩充块大小
						file.Read((BYTE*)&byBlockSize, 1);
					}
					
					// 跳出
					break;
				}
				default:
				{
					// 忽略未知的扩充块
					
					// 读取扩充块大小
					file.Read((BYTE*)&byBlockSize, 1);
					
					// 当byBlockSize > 0时循环读取
					while(byBlockSize)
					{
						// 读取未知的扩充块（这里没有进行任何处理）
						file.Read(lpTemp, byBlockSize);
						
						// 读取扩充块大小
						file.Read((BYTE*)&byBlockSize, 1);
					}
					
					// 跳出
					break;
				}
			
				// 释放内存
				GlobalUnlock(hTemp);
				GlobalFree(hTemp);
			}
		}
		
		// 读取下一个字节
		file.Read((BYTE*)&byTemp, 1);
	}

	// 读取GIF图像描述块
	file.Read((BYTE*)&GIFI, 9);
	
	// 获取图像宽度
	GIFDVar.wWidth         = GIFI.wWidth;
	
	// 获取图像高度
	GIFDVar.wDepth         = GIFI.wDepth;
	
	// 判断是否有区域调色板
	if (GIFI.LocalFlag.LocalPal)
	{
		// 赋初值
		memset((BYTE*)byGIF_Pal, 0, 768);
		
		// 读取区域调色板位数
		GIFDVar.wBits = (WORD)GIFI.LocalFlag.PalBits + 1;
		
		// 区域调色板大小
		wPalSize      = 3 * (1 << GIFDVar.wBits);
		
		// 读取区域调色板
		file.Read((BYTE*)byGIF_Pal,wPalSize);
	}
	
	// 给GIFDVar成员赋值
	GIFDVar.wBits = ((GIFDVar.wBits==1) ? 1 : (GIFDVar.wBits<=4) ? 4 : 8);
	
	GIFDVar.wLineBytes = (WORD)BYTE_WBYTES((DWORD)GIFDVar.wWidth * (DWORD)GIFDVar.wBits);
	GIFDVar.bEOF = FALSE;
	
	// 交错方式
	GIFDVar.bInterlace = (GIFI.LocalFlag.Interlace ? TRUE : FALSE);
	
	// 每行字节数
	wWidthBytes	= (WORD)DWORD_WBYTES((DWORD)GIFDVar.wWidth * (DWORD)GIFDVar.wBits);
	
	// 颜色数目
	wColors = 1 << GIFDVar.wBits;
	
	// 调色板大小
	wPalSize = wColors * sizeof(RGBQUAD);
	
	// 计算DIB长度（字节）
	dwDIB_Size = sizeof(BITMAPINFOHEADER) + wPalSize + GIFDVar.wDepth * wWidthBytes;
	
	// 为DIB分配内存
	HDIB hDIB = (HDIB) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwDIB_Size);
	
	if (hDIB == 0)
	{
		// 内存分配失败，返回NULL。
		return NULL;
	}
	
	// 锁定
	BYTE* pDIB = (BYTE*) ::GlobalLock((HGLOBAL) hDIB);
	
	///////////////////////////////////////////////////////////////////////////
	// 设置BITMAPINFOHEADER

	// 指向BITMAPINFOHEADER的指针
	LPBITMAPINFOHEADER lpBIH = (LPBITMAPINFOHEADER) pDIB;
	
	// 指向BITMAPINFO的指针
	LPBITMAPINFO lpBI = (LPBITMAPINFO) pDIB;	

	// 给lpBIH成员赋值
	lpBIH->biSize = (DWORD)sizeof(BITMAPINFOHEADER);
	lpBIH->biWidth = (DWORD)GIFDVar.wWidth;
	lpBIH->biHeight = (DWORD)GIFDVar.wDepth;
	lpBIH->biPlanes = 1;
	lpBIH->biBitCount = GIFDVar.wBits;
	lpBIH->biCompression = BI_RGB;
	lpBIH->biSizeImage = (DWORD)wWidthBytes * GIFDVar.wDepth;
	lpBIH->biXPelsPerMeter = 0;
	lpBIH->biYPelsPerMeter = 0;
	lpBIH->biClrUsed = wColors;
	lpBIH->biClrImportant = 0;
	
	///////////////////////////////////////////////////////////////////////////
	// 设置调色板
	
	// 判断是否指定全局或区域调色板
	if (GIFS.GlobalFlag.GlobalPal || GIFI.LocalFlag.LocalPal)
	{
		wTemp = 0;
		for(wi=0; wi<wColors; wi++)
		{
			lpBI->bmiColors[wi].rgbRed      = byGIF_Pal[wTemp++];
			lpBI->bmiColors[wi].rgbGreen    = byGIF_Pal[wTemp++];
			lpBI->bmiColors[wi].rgbBlue     = byGIF_Pal[wTemp++];
			lpBI->bmiColors[wi].rgbReserved = 0x00;
		}
	}
	else
	{
		// 没有指定全局和区域调色板，采用系统调色板
		
		// 按照颜色数目来分别给调色板赋值
		switch(wColors)
		{
			case   2:
			{
				// 单色位图
				
				lpBI->bmiColors[0].rgbRed          = 0x00;
				lpBI->bmiColors[0].rgbGreen        = 0x00;
				lpBI->bmiColors[0].rgbBlue         = 0x00;
				lpBI->bmiColors[0].rgbReserved     = 0x00;
				lpBI->bmiColors[1].rgbRed          = 0xFF;
				lpBI->bmiColors[1].rgbGreen        = 0xFF;
				lpBI->bmiColors[1].rgbBlue         = 0xFF;
				lpBI->bmiColors[1].rgbReserved     = 0x00;
				
				// 跳出
				break;
			}
			case  16:
			{
				// 16色位图
				
				wTemp = 0;
				for(wi=0;wi<wColors;wi++)
				{
					lpBI->bmiColors[wi].rgbRed      = bySysPal16[wTemp++];
					lpBI->bmiColors[wi].rgbGreen    = bySysPal16[wTemp++];
					lpBI->bmiColors[wi].rgbBlue     = bySysPal16[wTemp++];
					lpBI->bmiColors[wi].rgbReserved = 0x00;
				}
				
				// 跳出
				break;
			}
			case 256:
			{
				// 256色位图
				for(wi=0; wi<wColors; wi++)
				{
					lpBI->bmiColors[wi].rgbRed      = (BYTE)wi;
					lpBI->bmiColors[wi].rgbGreen    = (BYTE)wi;
					lpBI->bmiColors[wi].rgbBlue     = (BYTE)wi;
					lpBI->bmiColors[wi].rgbReserved = 0x00;
				}
				
				// 跳出
				break;
			}
		}
	}
	
	///////////////////////////////////////////////////////////////////////////
	// 解码

	// 获取编码数据长度
	GIFDVar.dwDataLen  = (DWORD) (file.GetLength() - file.GetPosition());
	
	// 计算内存大小（最大不超过MAX_BUFF_SIZE）
	GIFDVar.wMemLen    = ((GIFDVar.dwDataLen > (DWORD)MAX_BUFF_SIZE) ?
						  (WORD)MAX_BUFF_SIZE : (WORD)GIFDVar.dwDataLen);
	
	// 分配内存
	hSrcBuff = GlobalAlloc(GHND, (DWORD)GIFDVar.wMemLen);
	
	// 锁定内存
	GIFDVar.lpDataBuff = (BYTE*)GlobalLock(hSrcBuff);
	
	// 读取编码数据
	ReadGIF(file, &GIFDVar.wMemLen, &GIFDVar.dwDataLen, GIFDVar.lpDataBuff, &GIFDVar.bEOF);
	
	// 缓冲区起始位置
	GIFDVar.lpBgnBuff  = GIFDVar.lpDataBuff;
	
	// 缓冲区中止位置
	GIFDVar.lpEndBuff  = GIFDVar.lpBgnBuff + GIFDVar.wMemLen;
	
	// 计算DIB中像素位置
	BYTE* lpDIBBits = m_pDIB->FindAddress(pDIB);
	
	// 解码
	LZW_Decode(file, lpDIBBits, &GIFDVar, wWidthBytes);

	// 释放内存
	GlobalUnlock(hSrcBuff);
	GlobalFree(hSrcBuff);
	
	return hDIB;
}

/*************************************************************************
 *
 * 函数名称：
 *   ReadGIF()
 *
 * 参数:
 *   CFile& file			- 源GIF文件
 *   LPWORD lpwMemLen		- 缓冲区长度（指针）
 *   LPDWORD lpdwDataLen	- 剩余数据长度（指针）
 *   BYTE* lpSrcBuff		- 缓冲区指针
 *	 LPBOOL lpbEOF			- 结束标志
 *
 * 返回值:
 *   无
 *
 * 说明:
 *   该函数用来读取指定GIF文件中的图像编码
 *
 *************************************************************************/
void CCoding::ReadGIF(CFile &file, LPWORD lpwMemLen, LPDWORD lpdwDataLen, BYTE* lpSrcBuf, LPBOOL lpbEOF)
{
	// 根据数据长度是否仍然大于内存大小来判断解码是否将要完成
	if ((*lpdwDataLen) > (DWORD)(*lpwMemLen))
		(*lpdwDataLen) -= (DWORD)(*lpwMemLen);
	else
	{
		// 内存大小就是剩余数据长度
		(*lpwMemLen)    = (WORD)(*lpdwDataLen);
		
		// EOF标志设置为TRUE，解码完成
		(*lpbEOF)       = TRUE;
	}
	
	// 读取编码数据
	file.Read(lpSrcBuf, (*lpwMemLen));
	
	return;
}

/*************************************************************************
 *
 * 函数名称：
 *   LZW_Decode()
 *
 * 参数:
 *   CFile& file			- 源GIF文件
 *   BYTE* lpDIBBits		- 指向要保存的DIB图像指针
 *   LPGIFD_VAR lpGIFDVar	- 指向GIFC_VAR结构的指针
 *	 WORD wWidthBytes		- 每行图像字节数
 *
 * 返回值:
 *   无
 *
 * 说明:
 *   该函数对LZW编码进行解码。
 *
 *************************************************************************/
void CCoding::LZW_Decode(CFile &file, BYTE* lpDIBBits, LPGIFD_VAR lpGIFDVar, WORD wWidthBytes)
{
	// 指向编码后图像数据的指针
	BYTE *	lpDst;
	
	// 内存分配句柄
	HANDLE hPrefix;
	HANDLE hSuffix;
	HANDLE hStack;
	HANDLE hImage;
	
	// 用于字串表搜索的索引
	LPWORD lpwPrefix;
	BYTE* lpbySuffix;
	BYTE* lpbyStack;
	BYTE* lpbyStackBgn;
	
	// 指向图像当前行解码结果的指针
	BYTE*  lpImageBgn;
	
	// 指向当前编码像素的指针
	BYTE*  lpImage;
	
	// 计算当前数据图像的偏移量
	DWORD  dwDataNdx;
	
	// LZW_CLEAR
	WORD   wLZW_CLEAR;
	
	// LZW_EOI
	WORD   wLZW_EOI;
	
	// LZW_MinCodeLen
	BYTE   byLZW_MinCodeLen;
	
	// 字串表索引
	WORD   wNowTableNdx;
	WORD   wTopTableNdx;
	
	// 当前图像的行数
	WORD   wRowNum;
	
	// 计数
	WORD   wWidthCnt;
	WORD   wBitCnt;
	WORD   wRowCnt;
	
	WORD   wi;
	
	// 交错方式存储时每次增加的行数
	WORD   wIncTable[5]  = { 8,8,4,2,0 }; 
	
	// 交错方式存储时起始行数
	WORD   wBgnTable[5]  = { 0,4,2,1,0 }; 
	
	// 块大小
	BYTE   byBlockSize;
	
	// 块索引
	BYTE   byBlockNdx;
	
	DWORD  dwData;
	
	// 当前编码
	WORD   wCode;
	
	// 上一个编码
	WORD   wOldCode;
	
	// 临时索引
	WORD   wTempNdx;
	
	WORD   wCodeMask[13] = {0x0000, 0x0001, 0x0003, 0x0007, 0x000F,
							0x001F, 0x003F, 0x007F, 0x00FF, 0x01FF,
							0x03FF, 0x07FF, 0x0FFF};
	
	BYTE   byLeftBits;
	BYTE   byFirstChar;
	BYTE   byCode;
	BYTE   byCurrentBits;
	BYTE   byPass;
	
	// 临时字节变量
	BYTE   byTempChar;
	
	// 给字串表分配内存
	hPrefix          = GlobalAlloc(GHND,(DWORD)(MAX_TABLE_SIZE<<1));
	hSuffix          = GlobalAlloc(GHND,(DWORD)MAX_TABLE_SIZE);
	hStack           = GlobalAlloc(GHND,(DWORD)MAX_TABLE_SIZE);
	hImage           = GlobalAlloc(GHND,(DWORD)wWidthBytes);
	
	// 锁定内存
	lpwPrefix        = (LPWORD)GlobalLock(hPrefix);
	lpbySuffix       = (BYTE*)GlobalLock(hSuffix);
	lpbyStack        = (BYTE*)GlobalLock(hStack);
	lpbyStackBgn     = lpbyStack;
	lpImage          = (BYTE*)GlobalLock(hImage);
	lpImageBgn       = lpImage;
	
	// 读取GIF LZW最小编码大小
	byLZW_MinCodeLen = *lpGIFDVar->lpBgnBuff++;
	
	byCurrentBits    = byLZW_MinCodeLen + (BYTE)0x01;
	
	// 计算LZW_CLEAR
	wLZW_CLEAR       = 1 << byLZW_MinCodeLen;
	
	// 计算LZW_EOI
	wLZW_EOI         = wLZW_CLEAR + 1;
	
	// 计算字串表索引
	wNowTableNdx     = wLZW_CLEAR + 2;
	wTopTableNdx     = 1 << byCurrentBits;
	
	// 赋初值
	dwData           = 0UL;
	wBitCnt          = lpGIFDVar->wBits;
	wRowNum          = 0;
	wRowCnt          = 1;
	wWidthCnt        = 0;
	wCode            = 0;
	wOldCode         = 0xFFFF;
	byBlockSize      = 0x01;
	byBlockNdx       = 0x00;
	byLeftBits       = 0x00;
	byTempChar       = 0x00;
	byPass           = 0x00;
	
	// 读取下一个编码
	while(byLeftBits < byCurrentBits)
	{
		// 读取下一个字符
		
		// 判断是否读完一个数据块
		if (++byBlockNdx == byBlockSize)
		{
			// 读取下一个数据块
			byBlockSize = *lpGIFDVar->lpBgnBuff++;
			byBlockNdx  = 0x00;
			
			// 判断是否读完
			if ((lpGIFDVar->lpBgnBuff == lpGIFDVar->lpEndBuff) &&
			    !lpGIFDVar->bEOF)
			{
				// 读取下一个数据块
				ReadGIF(file, &lpGIFDVar->wMemLen, &lpGIFDVar->dwDataLen, lpGIFDVar->lpDataBuff, &lpGIFDVar->bEOF);
				
				// 指针重新赋值
				lpGIFDVar->lpBgnBuff = lpGIFDVar->lpDataBuff;
				lpGIFDVar->lpEndBuff = lpGIFDVar->lpBgnBuff + lpGIFDVar->wMemLen;
			}
		}
		
		// 下一个字符
		byCode      = *lpGIFDVar->lpBgnBuff++;
		
		// 移位
		dwData     |= ((DWORD)byCode << byLeftBits);
		byLeftBits += 0x08;
		
		// 判断是否读完
		if ((lpGIFDVar->lpBgnBuff == lpGIFDVar->lpEndBuff) && !lpGIFDVar->bEOF)
		{
			// 读取下一个数据块
			ReadGIF(file, &lpGIFDVar->wMemLen, &lpGIFDVar->dwDataLen, lpGIFDVar->lpDataBuff, &lpGIFDVar->bEOF);
			
			// 指针重新赋值
			lpGIFDVar->lpBgnBuff = lpGIFDVar->lpDataBuff;
			lpGIFDVar->lpEndBuff = lpGIFDVar->lpBgnBuff + lpGIFDVar->wMemLen;
		}
	}
	
	wCode       = (WORD)dwData & wCodeMask[byCurrentBits];
	dwData    >>= byCurrentBits;
	byLeftBits -= byCurrentBits;
	
	// 解码
	while(wCode != wLZW_EOI)
	{
		// 当前编码不是LZW_EOI码
		
		// 判断是否是LZW_CLEAR码
		if (wCode == wLZW_CLEAR)
		{
			// 是LZW_CLEAR，清除字串表
			
			// 重新初始化字串表
			for(wi = 0; wi < wLZW_CLEAR; wi++)
			{
				*(lpwPrefix  + wi)  = 0xFFFF;
				*(lpbySuffix + wi) = (BYTE)wi;
			}
			
			for(wi = wNowTableNdx; wi < MAX_TABLE_SIZE; wi++)
			{
				*(lpwPrefix+wi)  = 0xFFFF;
				*(lpbySuffix+wi) = 0x00;
			}
			
			byCurrentBits = byLZW_MinCodeLen + (BYTE)0x01;
			wNowTableNdx  = wLZW_CLEAR + 2;
			wTopTableNdx  = 1 << byCurrentBits;
			wOldCode      = 0xFFFF;
			
			// 获取下一个编码
			while(byLeftBits < byCurrentBits)
			{
				// 读取下一个字符
				
				// 判断是否读完一个数据块
				if (++byBlockNdx == byBlockSize)
				{
					// 读取下一个数据块
					byBlockSize = *lpGIFDVar->lpBgnBuff++;
					byBlockNdx  = 0x00;
					
					// 判断是否读完
					if ((lpGIFDVar->lpBgnBuff == lpGIFDVar->lpEndBuff) && !lpGIFDVar->bEOF)
					{
						// 读取下一个数据块
						ReadGIF(file, &lpGIFDVar->wMemLen, &lpGIFDVar->dwDataLen, lpGIFDVar->lpDataBuff, &lpGIFDVar->bEOF);
						
						// 指针重新赋值
						lpGIFDVar->lpBgnBuff = lpGIFDVar->lpDataBuff;
						lpGIFDVar->lpEndBuff = lpGIFDVar->lpBgnBuff +
											   lpGIFDVar->wMemLen;
					}
				}
				byCode      = *lpGIFDVar->lpBgnBuff++;
				dwData     |= ((DWORD)byCode << byLeftBits);
				byLeftBits += 0x08;
				
				// 判断是否读完
				if ((lpGIFDVar->lpBgnBuff == lpGIFDVar->lpEndBuff) && !lpGIFDVar->bEOF)
				{
					// 读取下一个数据块
					ReadGIF(file, &lpGIFDVar->wMemLen, &lpGIFDVar->dwDataLen, lpGIFDVar->lpDataBuff, &lpGIFDVar->bEOF);
					
					// 指针重新赋值
					lpGIFDVar->lpBgnBuff = lpGIFDVar->lpDataBuff;
					lpGIFDVar->lpEndBuff = lpGIFDVar->lpBgnBuff + lpGIFDVar->wMemLen;
				}
			}
			wCode       = (WORD)dwData & wCodeMask[byCurrentBits];
			dwData    >>= byCurrentBits;
			byLeftBits -= byCurrentBits;
			
			// 判断编码是否为LZW_EOI
			if (wCode!=wLZW_EOI)
			{
				// 这里没有用到lpbyStack[0]
				lpbyStack  ++;
				
				// 将数据压入堆栈
				while((*(lpwPrefix+wCode)) != 0xFFFF)
				{
					*lpbyStack++ = *(lpbySuffix+wCode);
					wCode        = *(lpwPrefix+wCode);
				}
				*lpbyStack  = *(lpbySuffix+wCode);
				byFirstChar = *lpbyStack;
				
				// 输出数据
				while(lpbyStack>lpbyStackBgn)
				{
					byTempChar |= (*lpbyStack-- << (8-wBitCnt));
					
					if (wBitCnt==8)
					{
						*lpImage++ = byTempChar;
						byTempChar = 0x00;
						wBitCnt    = lpGIFDVar->wBits;
					}
					else
					{
						wBitCnt   += lpGIFDVar->wBits;
					}
					
					wWidthCnt  ++;
					
					if (wWidthCnt==lpGIFDVar->wWidth)
					{
						if (wBitCnt!=lpGIFDVar->wBits)
						{
							*lpImage   = byTempChar;
							byTempChar = 0x00;
							wBitCnt    = lpGIFDVar->wBits;
						}
						
						// 图像当前行偏移量
						dwDataNdx = (DWORD)(lpGIFDVar->wDepth - 1 - wRowNum) * (DWORD)wWidthBytes;
						
						// 图像当前行起始位置
						lpDst = (BYTE *)lpDIBBits + dwDataNdx;
						
						// 赋值
						memcpy(lpDst, lpImageBgn, wWidthBytes);
						
						lpImage   = lpImageBgn;
						
						// 判断是否按照交错方式保存
						if (lpGIFDVar->bInterlace)
						{
							// 交错方式
							
							// 计算下一行的行号
							wRowNum += wIncTable[byPass];
							if (wRowNum >= lpGIFDVar->wDepth)
							{
								byPass ++;
								wRowNum = wBgnTable[byPass];
							}
						}
						else
						{
							// 非交错方式，行号直接加1
							wRowNum ++;
						}
						wWidthCnt = 0;
					}
				}
			}
		}
		else
		{
			// 这里没有用到lpbyStack[0]
			lpbyStack  ++;
			
			// 判断字符串是否在字串表中
			if (wCode < wNowTableNdx)
			{
				// 不在字串表中
				wTempNdx   = wCode;
			}
			else
			{
				// 在字串表中
				wTempNdx     = wOldCode;
				*lpbyStack++ = byFirstChar;
			}
			
			// 将数据压入堆栈
			while((*(lpwPrefix+wTempNdx)) != 0xFFFF)
			{
				*lpbyStack++ = *(lpbySuffix+wTempNdx);
				wTempNdx     = *(lpwPrefix+wTempNdx);
			}
			*lpbyStack  = *(lpbySuffix+wTempNdx);
			byFirstChar = *lpbyStack;
			
			// 将字符串添加到字串表中
			*(lpwPrefix+wNowTableNdx)  = wOldCode;
			*(lpbySuffix+wNowTableNdx) = byFirstChar;
			if (++wNowTableNdx==wTopTableNdx && byCurrentBits<12)
			{
				byCurrentBits ++;
				wTopTableNdx = 1 << byCurrentBits;
			}
			
			// 输出数据
			while(lpbyStack>lpbyStackBgn)
			{
				byTempChar |= (*lpbyStack-- << (8 - wBitCnt));
				if (wBitCnt==8)
				{
					*lpImage++ = byTempChar;
					byTempChar = 0x00;
					wBitCnt = lpGIFDVar->wBits;
				}
				else
					wBitCnt += lpGIFDVar->wBits;
				
				wWidthCnt  ++;
				if (wWidthCnt==lpGIFDVar->wWidth)
				{
					if (wBitCnt!=lpGIFDVar->wBits)
					{
						*lpImage   = byTempChar;
						byTempChar = 0x00;
						wBitCnt    = lpGIFDVar->wBits;
					}
					
					// 图像当前行偏移量
					dwDataNdx = (DWORD)(lpGIFDVar->wDepth - 1 - wRowNum) * (DWORD)wWidthBytes;
					
					// 图像当前行起始位置
					lpDst = (BYTE *)lpDIBBits + dwDataNdx;
					
					// 赋值
					memcpy(lpDst, lpImageBgn, wWidthBytes);
					
					lpImage   = lpImageBgn;
					
					// 判断是否按照交错方式保存
					if (lpGIFDVar->bInterlace)
					{
						// 交错方式
						
						// 计算下一行的行号
						wRowNum += wIncTable[byPass];
						if (wRowNum >= lpGIFDVar->wDepth)
						{
							byPass ++;
							wRowNum = wBgnTable[byPass];
						}
					}
					else	// 非交错方式，行号直接加1
						wRowNum ++;
					wWidthCnt = 0;
				}
			}
		}
		wOldCode = wCode;
		
		// 读取下一个编码
		while(byLeftBits < byCurrentBits)
		{
			// 读取下一个字符
			
			// 判断是否读完一个数据块
			if (++byBlockNdx == byBlockSize)
			{
				// 读取下一个数据块
				byBlockSize = *lpGIFDVar->lpBgnBuff++;
				byBlockNdx  = 0x00;
				
				// 判断是否读完
				if ((lpGIFDVar->lpBgnBuff == lpGIFDVar->lpEndBuff) && !lpGIFDVar->bEOF)
				{
					// 读取下一个数据块
					ReadGIF(file, &lpGIFDVar->wMemLen, &lpGIFDVar->dwDataLen, lpGIFDVar->lpDataBuff,&lpGIFDVar->bEOF);					
					
					// 指针重新赋值
					lpGIFDVar->lpBgnBuff = lpGIFDVar->lpDataBuff;
					lpGIFDVar->lpEndBuff = lpGIFDVar->lpBgnBuff + lpGIFDVar->wMemLen;
				}
			}
			byCode      = *lpGIFDVar->lpBgnBuff++;
			dwData     |= ((DWORD)byCode << byLeftBits);
			byLeftBits += 0x08;
			
			// 判断是否读完
			if ((lpGIFDVar->lpBgnBuff == lpGIFDVar->lpEndBuff) && !lpGIFDVar->bEOF)
			{
				// 读取下一个数据块
				ReadGIF(file, &lpGIFDVar->wMemLen, &lpGIFDVar->dwDataLen, lpGIFDVar->lpDataBuff, &lpGIFDVar->bEOF);
				
				// 指针重新赋值
				lpGIFDVar->lpBgnBuff = lpGIFDVar->lpDataBuff;
				lpGIFDVar->lpEndBuff = lpGIFDVar->lpBgnBuff + lpGIFDVar->wMemLen;
			}
		}
		wCode       = (WORD)dwData & wCodeMask[byCurrentBits];
		dwData    >>= byCurrentBits;
		byLeftBits -= byCurrentBits;
	}
	
	// 释放内存
	GlobalUnlock(hPrefix);
	GlobalUnlock(hSuffix);
	GlobalUnlock(hStack);
	GlobalFree(hPrefix);
	GlobalFree(hSuffix);
	GlobalFree(hStack);
	
	return;
}

