// ************************************************************************
//  文件名：WaterMarking.cpp
//
//   水印添加函数库：
//
//  Set()				- 水印嵌入函数
//  Get()				- 水印检测函数
//
// ************************************************************************

#include "stdafx.h"
#include "dip_system.h"
#include "WaterMarking.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WIDTH	100
#define HEIGHT	100

/////////////////////////////////////////////////////////////////////////////
// CWaterMarking

CWaterMarking::CWaterMarking()
{
}

CWaterMarking::CWaterMarking(CDIB* pDIB)
{
	m_pDIB = pDIB;
}

CWaterMarking::~CWaterMarking()
{
}

BEGIN_MESSAGE_MAP(CWaterMarking, CWnd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaterMarking message handlers

// 该函数为DIB图象添加水印
void CWaterMarking::Set()
{
	CHECK_EDITABLE(m_pDIB->GetImage());

	BeginWaitCursor();
		
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	// 从水印文件获取相关信息
	CFileDialog hFileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_READONLY, TEXT("所有文件(*.*)|*.* | 水印文件(*.bmp)|*.bmp||"), NULL);
	hFileDlg.m_ofn.nFilterIndex = 2;
	hFileDlg.m_ofn.hwndOwner = m_hWnd;
	hFileDlg.m_ofn.lStructSize = sizeof(OPENFILENAME);
	hFileDlg.m_ofn.lpstrTitle = TEXT("打开水印文件...\0");
	hFileDlg.m_ofn.nMaxFile = MAX_PATH;

	if (hFileDlg.DoModal() != IDOK) return;
	CString strPath = hFileDlg.GetPathName();

	CFile file;
	file.Open(strPath, CFile::modeReadWrite);
	int nFileLen = file.GetLength();
	BYTE* lpBuf;
	lpBuf = new BYTE[nFileLen];
	nFileLen = file.Read(lpBuf, nFileLen);
	file.Close();

	if (lLineBytes * lHeight < 4 * nFileLen)
	{
		MessageBox(L"载体位图太小，不能容纳水印！", L"系统提示" , MB_ICONINFORMATION | MB_OK);
		delete lpBuf;
		return;
	}

	// 填冗余位时的移位序列
	int move1[13] = {6, 5, 2, 0, 7, 4, 2, 1, -2, 6, 4, 3, 0};
	
	// 待隐藏文件的字节掩码序列
    BYTE mask1[13]={192, 32, 28, 3, 128, 112, 12, 2, 1, 192, 48, 8, 7};
	
	// 位图文件的字节掩码序列
	BYTE mask2[13]={252, 254, 248, 252, 254, 248, 252, 254, 251, 252, 252, 254, 248};
	
	// 为1时pointer1指针加一
	int add1[13]={0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1};

	// 为1时pointer2指针加一
	int add2[13]={1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1};

	// 临时指针
	int pointer1 = 0;
	int pointer2 = 0;
	int pointer3 = 0;

	while(pointer1 <= nFileLen)
	{
		// 将水印比特流嵌入到载体位图
		if (move1[pointer3] > 0)
			*(lpDIBBits + pointer2) = (*(lpDIBBits + pointer2) & mask2[pointer3])|((lpBuf[pointer1] & mask1[pointer3]) >> move1[pointer3]);
		else
			*(lpDIBBits + pointer2) = (*(lpDIBBits + pointer2) & mask2[pointer3])|((lpBuf[pointer1] & mask1[pointer3]) << move1[pointer3] * (-1));

		//修正指针
		if (add1[pointer3] == 1)
			pointer1++;
		if (add2[pointer3] == 1)
			pointer2++;
		pointer3++;
		pointer3 %= 13;
	}
	delete lpBuf;//释放缓存

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   Get() 
 * 
 * 参数:
 *	 CFile& file		- 水印文件
 *
 * 返回值:
 *   void				- 无返回值
 *
 * 说明:
 *   该函数从DIB图象提取出水印
 *
 ************************************************************************/
void CWaterMarking::Get(CFile &file)
{
	// 嵌入水印的长度
	int nFileLen = 54 + 3 * WIDTH * HEIGHT;
	
	CHECK_EDITABLE(m_pDIB->GetImage());

	BeginWaitCursor();
		
	LONG lWidth = m_pDIB->Width();
	LONG lHeight = m_pDIB->Height();
	LONG lLineBytes = m_pDIB->RowLen();
	BYTE* lpDIBBits = m_pDIB->FindAddress();

	// 分配并初始化内存
	BYTE* lpBuf1;
	BYTE* lpBuf2;

	lpBuf1 = new BYTE [lLineBytes * lHeight];
	lpBuf2 = new BYTE [lLineBytes * lHeight];

	memset(lpBuf1, 0, lLineBytes * lHeight);
	memset(lpBuf2, 0, lLineBytes * lHeight);

	//拼合文件信息字节时的移位序列
	int move2[13] = {6, 5, 2, 0, 7, 4, 2, 1, -2, 6, 4, 3, 0};

	//位图文件字节的掩码序列
    BYTE mask2[13]={3, 1, 7, 3, 1, 7, 3, 1, 4, 3, 3, 1, 7};

	// 为1时pointer1指针加一
	int add1[13]={0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1};

	// 为1时pointer2指针加一
	int add2[13]={1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1};

	// 临时指针
	int pointer1 = 0;
	int pointer2 = 0;
	int pointer3 = 0;
	int pointer4 = 0;

	// 提取水印数据
	while(true)
	{
		// 从DIB字节流中提取出水印数据
		if(move2[pointer3] > 0)
			lpBuf2[pointer1] |= (*(lpDIBBits + pointer2) & mask2[pointer3]) << move2[pointer3];
		else
			lpBuf2[pointer1] |= (*(lpDIBBits + pointer2) & mask2[pointer3]) >> (move2[pointer3] * (-1));
		
		if(add1[pointer3] == 1)
		{
			lpBuf1[pointer4] = lpBuf2[pointer1];
			pointer4++;

			// 水印提取完毕，跳出循环
			if(pointer4 > nFileLen)
				break;

			//修正指针
			pointer1++;
		}
		
		//修正指针
		if(add2[pointer3] == 1)
			pointer2++;
		pointer3++;
		pointer3 %= 13;
	}
	file.Write(lpBuf1, nFileLen);

	//释放缓存
	delete lpBuf1;
	delete lpBuf2;
	
	EndWaitCursor();
}

