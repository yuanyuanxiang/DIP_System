
#pragma once

//
// Coding.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCoding window
#include "dib.h"
#include "DIP_SystemDoc.h"

// 宏运算
#define DWORD_WBYTES(x)         ( (((x) + 31UL) >> 5) << 2 )
#define WORD_WBYTES(x)          ( (((x) + 15UL) >> 4) << 1 )
#define BYTE_WBYTES(x)          (  ((x) +  7UL) >> 3       )

// 常量
#define MAX_BUFF_SIZE           32768 
#define MAX_HASH_SIZE            5051
#define MAX_TABLE_SIZE           4096
#define MAX_SUBBLOCK_SIZE         255

class CCoding : public CWnd
{
// Construction
public:
	CCoding();
	CCoding(CDIB* pDIB);

// Attributes
public:

	// PCX文件头结构
	typedef struct{
		 BYTE bManufacturer;
		 BYTE bVersion;
		 BYTE bEncoding;
		 BYTE bBpp;
		 WORD wLeft;
		 WORD wTop;
		 WORD wRight;
		 WORD wBottom;
		 WORD wXResolution;
		 WORD wYResolution;
		 BYTE bPalette[48];
		 BYTE bReserved;
		 BYTE bPlanes;
		 WORD wLineBytes;
		 WORD wPaletteType;
		 WORD wSrcWidth;
		 WORD wSrcDepth;
		 BYTE bFiller[54];
	} PCXHEADER;

	// 文件头结构
	typedef struct tagGIF_HEADER{
			BYTE bySignature[3];		// GIF署名
			BYTE byVersion[3];			// 版本号
	} GIFHEADER;	

	// 逻辑屏幕标识符结构
	typedef struct tagGIF_SCRDESC{
			WORD wWidth;				// 逻辑屏幕宽度
			WORD wDepth;				// 逻辑屏幕高度
			struct tagGLOBAL_FLAG{		
					BYTE PalBits   : 3;	// 指定全局调色板的位数
					BYTE SortFlag  : 1;	// 指明全局调色板中RGB颜色值是否经过排序
					BYTE ColorRes  : 3;	// 指定图象色彩分辨率
					BYTE GlobalPal : 1;	// 指定是否具有全局调色板
			} GlobalFlag;
			BYTE byBackground;			// 背景色
			BYTE byAspect;				// 象素宽高比
	} GIFSCRDESC;

	// 图象标识符结构 
	typedef struct tagGIF_IMAGE{
			WORD wLeft;					// X方向偏移量
			WORD wTop;					// Y方向偏移量
			WORD wWidth;				// 图象宽度
			WORD wDepth;				// 图象高度
			struct tagLOCALFLAG{		
		            BYTE PalBits   : 3;	// 局部颜色列表大小
					BYTE Reserved  : 2;	// 保留，必须初始化为0
					BYTE SortFlag  : 1;	// 分类标志
					BYTE Interlace : 1;	// 交织标志
					BYTE LocalPal  : 1;	// 局部颜色列表标志
			} LocalFlag;
	} GIFIMAGE;

	// 图形控制扩展结构 
	typedef struct tagGIF_CONTROL{		 
			BYTE byBlockSize;					// 指定图形控制扩展结构的长度，固定为6
			struct tagFLAG{					
					BYTE Transparency   : 1;	// 透明颜色标志
					BYTE UserInput      : 1;	// 用户输入标志
					BYTE DisposalMethod : 3;	// 处置方法
					BYTE Reserved       : 3;	// 保留
			} Flag;
			WORD wDelayTime;					// 指定进行下一步操作前延迟的时间
			BYTE byTransparencyIndex;			// 指定图象中透明色的颜色索引
			BYTE byTerminator;					// 块终结器，固定为0
	} GIFCONTROL;

	// 图形文本扩展结构
	typedef struct tagGIF_PLAINTEXT{
			BYTE byBlockSize;		// 本扩充块的长度
			WORD wTextGridLeft;		// 文本框离逻辑屏幕的左边界距离
			WORD wTextGridTop;		// 文本框离逻辑屏幕的上边界距离
			WORD wTextGridWidth;	// 文本框宽度
			WORD wTextGridDepth;	// 文本框高度
			BYTE byCharCellWidth;	// 字符单元格宽度
			BYTE byCharCellDepth;	// 字符单元格高度
			BYTE byForeColorIndex;	// 文本前景色索引
			BYTE byBackColorIndex;	// 文本背景色索引
	} GIFPLAINTEXT;

	// 应用程序扩展结构
	typedef struct tagGIF_APPLICATION{
			BYTE byBlockSize;			// 块大小
			BYTE byIdentifier[8];		// 应用程序标识符
			BYTE byAuthentication[3];	// 应用程序鉴别码
	} GIFAPPLICATION;

	typedef struct tagGIF_D_VAR{
			BYTE* lpDataBuff;
			BYTE* lpBgnBuff;
			BYTE* lpEndBuff;
			DWORD dwDataLen;
			WORD  wMemLen;
			WORD  wWidth;
			WORD  wDepth;
			WORD  wLineBytes;
			WORD  wBits;
			BOOL  bEOF;
			BOOL  bInterlace;	
	} GIFD_VAR, FAR* LPGIFD_VAR;

	typedef struct tagGIF_C_VAR{
			BYTE* lpDataBuff;
			BYTE* lpEndBuff;
			DWORD dwTempCode;
			WORD  wWidth;
			WORD  wDepth;
			WORD  wLineBytes;
			WORD  wBits;
			WORD  wByteCnt;
			WORD  wBlockNdx;
			BYTE  byLeftBits;
	} GIFC_VAR, FAR *LPGIFC_VAR;

// Implementation
public:
	void OutputCode(CFile &file, WORD wCode, BYTE* lpbySubBlock, BYTE* lpbyCurrentBits, LPGIFC_VAR lpGIFCVar);
	void LZW_Encode(BYTE* lpDIBBits, CFile& file, LPGIFC_VAR lpGIFCVar, WORD wWidthBytes, BOOL bInterlace);
	void LZW_Decode(CFile& file, BYTE* lpDIBBits, LPGIFD_VAR lpGIFDVar, WORD wWidthBytes);
	BOOL DIBToGIF(HDIB hDIB, CFile& file, BOOL bInterlace);
	HDIB GIFToDIB(CFile& file);
	void ReadGIF(CFile& file, LPWORD lpwMemLen, LPDWORD lpdwDataLen, BYTE* lpSrcBuff, LPBOOL lpbEOF);
	BOOL Huffman(CString strPath);
	BOOL Shannon_Fannon(CString strPath);
	BOOL DIBToPCX(CString strPath);
	HDIB PCXToDIB(CString strPath);
	void SaveBMP(CString fileName, BYTE * buf, UINT width, UINT height);
	BYTE* LoadBMP(CString fileName, UINT *width, UINT *height);
	void LoadJPG(CDIP_SystemDoc* pDoc, CString filePath);
	void SaveJPG(CDIP_SystemDoc* pDoc, CString filePath);
	virtual ~CCoding();

	// Generated message map functions
protected:
	CDIB* m_pDIB;
	DECLARE_MESSAGE_MAP()
};
