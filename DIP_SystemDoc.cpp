// DIP_SystemDoc.cpp : implementation of the CDIP_SystemDoc class
//

#include "stdafx.h"
#include "DIP_System.h"

#include "DIP_SystemDoc.h"
#include "DIP_SystemView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDIP_SystemDoc

IMPLEMENT_DYNCREATE(CDIP_SystemDoc, CDocument)

BEGIN_MESSAGE_MAP(CDIP_SystemDoc, CDocument)
	ON_COMMAND(ID_FILE_REOPEN, OnFileReopen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDIP_SystemDoc construction/destruction

CDIP_SystemDoc::CDIP_SystemDoc()
{
	// 默认背景色，灰色
	m_refColorBKG = 0x00808080;	
	
	// 初始化变量
	m_hDIB = NULL;
	m_palDIB = NULL;
	m_sizeDoc = CSize(1, 1);
	m_ptPoint_I = CPoint(0, 0);
	m_ptPoint_II = CPoint(0, 0);
	m_ptLastPos = CPoint(0, 0);
	m_fZoomRatio = 2.0f;
	m_nRgnNum = 0;
	m_bIsFrequency = FALSE;
	m_strCurrentFile = "";

	m_Tracker.m_rect.SetRect (0, 0, 10, 10);
	m_Tracker.m_nStyle = CRectTracker::resizeInside | CRectTracker::dottedLine;

	m_pDIB = new CDIB;
	m_sFileName = "";
	m_sFilePath = "";
	m_bIsDecoding = FALSE;
	m_bJumpSecLine = FALSE;
}

CDIP_SystemDoc::~CDIP_SystemDoc()
{
	// 判断DIB对象是否存在
	if (m_hDIB != NULL)
	{
		// 清除DIB对象
		::GlobalFree((HGLOBAL)m_hDIB);
	}
	// 清除调色板
	if (m_palDIB != NULL)
		delete m_palDIB;
	if (m_pDIB != NULL)
		delete m_pDIB;
}

BOOL CDIP_SystemDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_strCurrentFile = "";

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDIP_SystemDoc serialization

void CDIP_SystemDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDIP_SystemDoc diagnostics

#ifdef _DEBUG
void CDIP_SystemDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDIP_SystemDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDIP_SystemDoc commands

///////////////////////////////////////////
//	重新打开文档						 //
///////////////////////////////////////////

void CDIP_SystemDoc::OnFileReopen() 
{
 	// 重新打开图像，放弃所有修改
 
	// 判断当前图像是否已经被改动
	if (IsModified())
	{
		// 该操作将丢失所有当前的修改
		if (MessageBox(NULL, L"重新打开图像将丢失所有改动！是否继续？", L"系统提示", MB_ICONQUESTION | MB_YESNO) == IDNO)
		{
			// 用户取消操作，直接返回
			return;
		}
	}
	
	CFile file;
 	CFileException fe;
 
 	CString strPathName;
  	
	// 获取当前文件路径
 	strPathName = GetPathName();
 
 	// 重新打开文件
 	if (!file.Open(strPathName, CFile::modeRead | CFile::shareDenyWrite, &fe))
 	{
 		// 失败
 		ReportSaveLoadException(strPathName, &fe, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		
 		return;
 	}
 	
 	
 	BeginWaitCursor();
 	
 	// 尝试调用ReadDIBFile()读取图像
	BOOL Status = FALSE;
 	TRY
 	{
		Status = m_pDIB->Read(file);
 	}
 	CATCH (CFileException, eLoad)
 	{
 		// 读取失败
 		file.Abort();
 		
 		EndWaitCursor();
 		
 		// 报告失败
 		ReportSaveLoadException(strPathName, eLoad,	FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
 		
 		
 		return;
 	}
 	END_CATCH
 	
 	// 初始化DIB
 	InitDIBData();
 	
 	// 判断读取文件是否成功
	if (!Status)
 	{
 		// 失败，可能非BMP格式
 		CString strMsg;
 		strMsg = "读取图像时出错！可能是非BMP格式的图像文件！";
 		
 		// 提示出错
 		MessageBox(NULL, strMsg, L"系统提示", MB_ICONINFORMATION | MB_OK);

 		EndWaitCursor();
 		
 		return;
 	}
 	
 	// 初始化脏标记为FALSE
 	SetModifiedFlag(FALSE);
 	
 	// 刷新
 	UpdateAllViews(NULL);
  	
 	EndWaitCursor();
 	
 	return;
}

///////////////////////////////////////////
//	打开文档							 //
///////////////////////////////////////////

BOOL CDIP_SystemDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	m_strCurrentFile = CString(lpszPathName);
	if (   m_strCurrentFile.Right(4) == ".pgm" || m_strCurrentFile.Right(4) == ".PGM"
		|| m_strCurrentFile.Right(4) == ".pbm" || m_strCurrentFile.Right(4) == ".PBM"
		|| m_strCurrentFile.Right(4) == ".ppm" || m_strCurrentFile.Right(4) == ".PPM")
	{
		return OpenPXMFile(lpszPathName);
	}

	// 清除文档类所有的数据
	DeleteContents();
	BeginWaitCursor();

	// 尝试调用Read()读取图像
	BOOL Status = TRUE;
	Status = m_pDIB->Read(m_strCurrentFile);
	// 判断读取文件是否成功
	if (!Status)
	{
		CString strMsg;
		strMsg = "读取图像时出错！";
		MessageBox(NULL, strMsg, L"系统提示", MB_ICONINFORMATION | MB_OK);
		return FALSE;
	}
	// 设置文档大小
	SetDocSize();
	EndWaitCursor();
	// 将打开文件路径添加到最近使用（MRU）的文件列表
	if (!m_bIsDecoding)
	{
		SetPathName(lpszPathName);
	}
	// 初始化胀标记为FALSE
	SetModifiedFlag(FALSE);
	m_sFileName = m_strTitle;
	m_sFilePath = m_strPathName;

	CDIP_SystemView* pView = (CDIP_SystemView*)GetView(RUNTIME_CLASS(CDIP_SystemView));
	ASSERT(pView != NULL);
	pView->m_PaintRect = CRect(0, 0, m_sizeDoc.cx, m_sizeDoc.cy);

	return TRUE;
}

///////////////////////////////////////////
//	保存文档							 //
///////////////////////////////////////////
BOOL CDIP_SystemDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	m_strCurrentFile = CString(lpszPathName);

	// 尝试调用SaveDIB保存图像
	BOOL bSuccess = TRUE;
	
	BeginWaitCursor();

	// 尝试保存图像
	bSuccess = m_pDIB->Save(m_strCurrentFile);

	EndWaitCursor();
	
	// 重置脏标记为FALSE
	SetModifiedFlag(FALSE);

	if (!bSuccess)
	{
		// 保存失败，可能是其它格式的DIB，可以读取但是不能保存
		// 或者是SaveDIB函数有误

		CString strMsg;
		strMsg = "无法保存BMP图像！";
		
		// 提示出错
		MessageBox(NULL, strMsg, L"系统提示", MB_ICONINFORMATION | MB_OK);
	}
	
	return bSuccess;
}

// HDIB转换为CImage图像
BOOL CDIP_SystemDoc::HDIB2CImage(HDIB hDIB)
{
	BYTE*  lpDIB = (BYTE*) ::GlobalLock((HGLOBAL)m_hDIB);
	BYTE* pSrc = m_pDIB->FindAddress(lpDIB);
	LONG Width = m_pDIB->Width(lpDIB);
	LONG Height = m_pDIB->Height(lpDIB);
	WORD nNumColors = m_pDIB->NumColors(lpDIB);
	WORD nBitCount = m_pDIB->BitCount(lpDIB);
	int Channel = nBitCount / 8;
	LONG Length = WIDTHBYTES(Width * nBitCount * Height);

	CImage* pImage = new CImage;
	if (Channel == 1)//灰度图像特殊处理
	{
		pImage->Create(Width, Height, 8);
		RGBQUAD ColorTab[256];
		for (int i = 0; i<256; i++)
		{
			ColorTab[i].rgbBlue = ColorTab[i].rgbGreen = ColorTab[i].rgbRed = i;
		}
		pImage->SetColorTable(0, 256, ColorTab);
	}
	else pImage->Create(Width, Height, 24);
	BYTE* pDst = (BYTE*)pImage->GetBits() + (pImage->GetPitch()*(pImage->GetHeight() - 1));
	memcpy(pDst, pSrc, Length);
	m_pDIB->Clear();
	m_pDIB->m_UndoStack.SetTop(pImage);
	m_pDIB->SetImage(pImage);

	// 判断图像是否过大（INT_MAX:2,147,483,647）
	if (m_pDIB->Width(lpDIB) > INT_MAX || m_pDIB->Height(lpDIB) > INT_MAX)
	{
		// 解锁DIB对象
		::GlobalUnlock((HGLOBAL)m_hDIB);

		// 释放DIB对象
		::GlobalFree((HGLOBAL)m_hDIB);

		// 设置DIB为空
		m_hDIB = NULL;

		CString strMsg;
		strMsg = "BMP图像太大！";
		
		MessageBox(NULL, strMsg, L"系统提示", MB_ICONINFORMATION | MB_OK);
		
		return FALSE;
	}

	// 设置文档大小
	m_sizeDoc = CSize((int)Width, (int)Height);

	// 解锁DIB对象
	::GlobalUnlock((HGLOBAL)m_hDIB);
	return TRUE;
}

// 创建缓存BMP位图
BOOL CDIP_SystemDoc::CreateTempFile(CString strFileName)
{
	::DeleteFile(strFileName);
	if (m_pDIB->m_pImage->Save(strFileName) == S_OK)
		return OnOpenDocument(strFileName, 0);
	return FALSE;
}

// 打开PGM格式图像
BOOL CDIP_SystemDoc::OpenPXMFile(LPCTSTR lpszPathName)
{
	char *fStream;
	CFile file;
	ULONGLONG filelength;

	if (!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite))
		return FALSE;

	//读取文件类型
	filelength = file.GetLength();
	fStream = new char[filelength];
	file.Read(fStream, filelength);

	int i = 0, j = 0, k = 0, start = 0, speLine = -1, curLine = 0, tempnum = 0, arg[4] = {0, 0, 0, 0};

	// 看看是否有注释行
	bool note = false;
	for (i = 0; curLine<3 && i<filelength; i++)
	{
		if(fStream[i] == '\n')
		{
			curLine++;
			//是否为注释行
			if (i+1 < filelength && fStream[i+1] == '#')
			{
				speLine = curLine + 1;
				note = true;
				break;
			}
		}
	}

	bool IsPBM = false;
	if (!note)
	{
		for(i = 0; i < filelength; i++)
		{
			//临时保存参数
			if(fStream[i] >= '0' && fStream[i] <= '9')
				tempnum = tempnum * 10 + fStream[i] - '0';
			if(fStream[i] == '\n' || fStream[i] == '\t' || fStream[i] == ' ')
			{
				arg[j++] = tempnum;
				tempnum = 0;
			}
			if (arg[0] == 1 || arg[0] == 4)
				IsPBM = true;
			if(j == 4 - IsPBM)
				break;
		}
	}
	else
	{
		curLine = 0;
		for(i = 0; i < filelength; i++)
		{
			//临时保存参数
			if(fStream[i] >= '0' && fStream[i] <= '9')
				tempnum = tempnum * 10 + fStream[i] - '0';
			if(curLine != speLine)
			{
				if(fStream[i] == '\n')
				{
					curLine++;
					//是否为注释行
					if(curLine != speLine)
					{
						arg[j++] = tempnum;
						tempnum = 0;
					}
					else tempnum = 0;
				}
			}
			else if(fStream[i] == '\n' || fStream[i] == '\t' || fStream[i] == ' ')
			{
				arg[j++] = tempnum;
				tempnum = 0;
			}
			if (arg[0] == 1 || arg[0] == 4)
				IsPBM = true;
			if(j == 4 - IsPBM)
				break;
		}
	}
	start = i + 1;

	int nType = arg[0];
	int nImageWidth = arg[1];
	int nImageHeight = arg[2];
	int nBitCount;
	if (nImageWidth * nImageHeight == 0)
	{
		file.Close();
		delete fStream;
		fStream = NULL;
		return FALSE;
	}

	switch (nType)
	{
	case 1: nBitCount = 8; break;
	case 4: nBitCount = 1; break;
	case 2: case 5: nBitCount = 8; break;
	case 3: case 6: nBitCount = 24;break;
	default:
		AfxMessageBox(L"图像是否为标准的PGM格式？", MB_OK | MB_ICONQUESTION);
		file.Close();
		delete fStream;
		fStream = NULL;
		return FALSE;
	}

	long Rowlen;
	if (nBitCount == 1)
		Rowlen = WIDTHBYTES(nImageWidth * 8);
	else
		Rowlen = WIDTHBYTES(nImageWidth * nBitCount);

	// 建立CImage对象
	CImage *pImage = new CImage;
	pImage->Create(nImageWidth, nImageHeight, nBitCount);
	BYTE* pBits = (BYTE*)pImage->GetBits() + pImage->GetPitch() * (nImageHeight - 1);

	int x = 0, y = 0;
	i = j = tempnum = 0;
	switch(nType)
	{
	case 4:
		{
			memcpy(pBits, fStream + start, filelength - start);
			break;
		}
	case 5:
	case 6:
		{
			BYTE *pCur = pBits + nImageHeight * Rowlen;
			for(i = start; i < filelength; ++i)
			{
				pCur--;
				*pCur = (BYTE)fStream[i];
			}
			break;
		}
	case 1:
		{
			for(i = start; i < filelength; ++i)
			{
				if(fStream[i] >= '0' && fStream[i] <= '9')
					tempnum = tempnum * 10 + fStream[i] - '0';
				if(fStream[i] == '\t' || fStream[i] == ' ')
				{
					*(pBits + x + y * Rowlen) = 255 * tempnum;
					x++;
					if (tempnum == 1)
					{
						tempnum = 0;
					}
					if (x == nImageWidth)
					{
						x = 0;
						y++;
					}
				}
			}
			break;
		}
	case 2:
	case 3:
		{
			BYTE *pCur = pBits + nImageHeight * Rowlen;
			for(i = start; i < filelength; ++i)
			{
				if(fStream[i] >= '0' && fStream[i] <= '9')
					tempnum = tempnum * 10 + fStream[i] - '0';
				if(fStream[i] == '\t' || fStream[i] == ' ')
				{
					pCur--;
					*pCur = (BYTE)tempnum;
					tempnum = 0;
				}
			}
			break;
		}
	}

	// 设置颜色表
	switch (nBitCount)
	{
	case 1:
		{
			RGBQUAD ColorTab[2];
			ColorTab[0].rgbRed = ColorTab[0].rgbGreen = ColorTab[0].rgbBlue = 0;
			ColorTab[0].rgbReserved = 0;
			ColorTab[1].rgbRed = ColorTab[1].rgbGreen = ColorTab[1].rgbBlue = 255;
			ColorTab[1].rgbReserved = 0;
			pImage->SetColorTable(0, 2, ColorTab);
			break;
		}
	case 8:
		{
			RGBQUAD ColorTab[256];
			for (i = 0; i < 256; i++)
			{
				ColorTab[i].rgbRed = ColorTab[i].rgbGreen = ColorTab[i].rgbBlue = i;
				ColorTab[i].rgbReserved = 0;
			}
			pImage->SetColorTable(0, 256, ColorTab);
			break;
		}
	default:
		break;
	}

	file.Close();
	delete fStream;
	fStream = NULL;

	// 将指针压入堆栈
	m_pDIB->m_UndoStack.Push(pImage);
	m_pDIB->SetImage(pImage);
	// 二进制位图转换为8位色图像
	if (nBitCount == 1)
		m_pDIB->BinaryMap2Other(8);
	SetDocSize();
	if (!m_bIsDecoding)
	{
		SetPathName(lpszPathName);
	}
	SetModifiedFlag(FALSE);
	m_sFileName = m_strTitle;
	m_sFilePath = m_strPathName;

	CDIP_SystemView* pView = (CDIP_SystemView*)GetView(RUNTIME_CLASS(CDIP_SystemView));
	ASSERT(pView != NULL);
	pView->m_PaintRect = CRect(0, 0, m_sizeDoc.cx, m_sizeDoc.cy);
	return TRUE;
}

///////////////////////////////////////////
//	初始化DIB对象						 //
///////////////////////////////////////////

// 设置文档大小,默认为图像大小
void CDIP_SystemDoc::SetDocSize()
{
	m_sizeDoc = CSize((int) m_pDIB->Width(), (int) m_pDIB->Height());
	GetView(RUNTIME_CLASS(CDIP_SystemView))->SetPaintRect(m_sizeDoc);
	GetView(RUNTIME_CLASS(CDIP_SystemView))->SetScrollSizes(MM_TEXT, m_sizeDoc);
}

// 重载：设置文档大小，根据cx和cy
void CDIP_SystemDoc::SetDocSize(int cx, int cy)
{
	m_sizeDoc.cx = cx;
	m_sizeDoc.cy = cy;
	GetView(RUNTIME_CLASS(CDIP_SystemView))->SetPaintRect(m_sizeDoc);
	GetView(RUNTIME_CLASS(CDIP_SystemView))->SetScrollSizes(MM_TEXT, m_sizeDoc);
}

//
//
//
//
//

///////////////////////////////////////////
//	初始化DIB对象						 //
///////////////////////////////////////////
void CDIP_SystemDoc::InitDIBData()
{
	// 判断调色板是否为空
	if (m_palDIB != NULL)
	{
		// 删除调色板对象
		delete m_palDIB;

		// 重置调色板为空
		m_palDIB = NULL;
	}

	// 如果DIB对象为空，直接返回
	if (m_hDIB == NULL)
	{
		
		return;
	}

	BYTE*  lpDIB = (BYTE* ) ::GlobalLock((HGLOBAL)m_hDIB);

	// 判断图像是否过大（INT_MAX:2,147,483,647）
	if (m_pDIB->Width(lpDIB) > INT_MAX || m_pDIB->Height(lpDIB) > INT_MAX)
	{
		// 解锁DIB对象
		::GlobalUnlock((HGLOBAL)m_hDIB);

		// 释放DIB对象
		::GlobalFree((HGLOBAL)m_hDIB);

		// 设置DIB为空
		m_hDIB = NULL;

		
		return;
	}

	// 解锁DIB对象
	::GlobalUnlock((HGLOBAL)m_hDIB);

	// 创建新调色板
	m_palDIB = new CPalette;

	// 判断是否创建成功
	if (m_palDIB == NULL)
	{
		// 失败，可能是内存不足
		::GlobalFree((HGLOBAL)m_hDIB);

		// 设置DIB对象为空
		m_hDIB = NULL;

		
		return;
	}

	// 调用CreateDIBPalette来创建调色板
	if (m_pDIB->CreatePalette(m_hDIB, m_palDIB) == NULL)
	{
		// 空，可能该DIB对象没有调色板

		// 删除
		delete m_palDIB;

		// 设置为空
		m_palDIB = NULL;

		
		return;
	}
}

///////////////////////////////////////////
//	替换DIB，在功能粘贴中用到该函数		 //
///////////////////////////////////////////
void CDIP_SystemDoc::ReplaceHDIB(HDIB hDIB)
{
	// 判断DIB是否为空
	if (m_hDIB != NULL)
	{
		// 非空，则清除
		::GlobalFree((HGLOBAL)m_hDIB);
	}

	// 替换成新的DIB对象
	m_hDIB = hDIB;
}

///////////////////////////////////////////
//	打开文档							 //
///////////////////////////////////////////
BOOL CDIP_SystemDoc::OnOpenDocument(LPCTSTR lpszPathName, BOOL bReserved)
{
	CFile file;
	CFileException fe;
	m_strCurrentFile = CString(lpszPathName);

	// 打开文件
	if (!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite, &fe))
	{
		ReportSaveLoadException(lpszPathName, &fe, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}

	// 清除文档类所有的数据
	DeleteContents();
	BeginWaitCursor();

	// 尝试调用ReadDIBFile()读取图像，并捕获异常
	TRY
	{
		m_hDIB = m_pDIB->Read(file, 0);
	}
	CATCH(CFileException, eLoad)
	{
		// 读取失败
		file.Abort();
		EndWaitCursor();
		ReportSaveLoadException(lpszPathName, eLoad, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		m_hDIB = NULL;
		return FALSE;
	}
	END_CATCH

	InitDIBData();
	EndWaitCursor();
	if (m_hDIB == NULL)
		return FALSE;
	SetTitle(L"缓存位图");
	return TRUE;
}

///////////////////////////////////////////
//	保存文档							 //
///////////////////////////////////////////
BOOL CDIP_SystemDoc::OnSaveDocument(LPCTSTR lpszPathName, BOOL bReserved)
{
	CFile file;
	CFileException fe;

	m_strCurrentFile = CString(lpszPathName);

	// 打开文件
	if (!file.Open(lpszPathName, CFile::modeCreate |
		CFile::modeReadWrite | CFile::shareExclusive, &fe))
	{
		// 失败
		ReportSaveLoadException(lpszPathName, &fe, TRUE, AFX_IDP_INVALID_FILENAME);

		return FALSE;
	}

	// 尝试调用SaveDIB保存图像
	BOOL bSuccess = FALSE;
	TRY
	{
		
		BeginWaitCursor();

		// 尝试保存图像
		bSuccess = m_pDIB->Save(m_hDIB, file);

		// 关闭文件
		file.Close();
	}
	CATCH(CException, eSave)
	{
		// 失败
		file.Abort();

		EndWaitCursor();
		ReportSaveLoadException(lpszPathName, eSave, TRUE, AFX_IDP_FAILED_TO_SAVE_DOC);

		return FALSE;
	}
	END_CATCH

	EndWaitCursor();

	// 重置脏标记为FALSE
	SetModifiedFlag(FALSE);

	if (!bSuccess)
	{
		// 保存失败，可能是其它格式的DIB，可以读取但是不能保存
		// 或者是SaveDIB函数有误

		CString strMsg;
		strMsg = "无法保存BMP图像！";

		// 提示出错
		MessageBox(NULL, strMsg, L"系统提示", MB_ICONINFORMATION | MB_OK);
	}

	return bSuccess;
}

// 从文档获取视图指针
// 使用如下： CYourView* pView=(CYourView*)GetView(RUNTIME_CLASS(CYourView));
CDIP_SystemView* CDIP_SystemDoc::GetView(CRuntimeClass* pClass)
{

	CView *pView = NULL; 
	POSITION pos = GetFirstViewPosition(); 
	while(pos != NULL) 
	{ 
		pView = GetNextView(pos); 
		if(!pView->IsKindOf(pClass)) 
			break; 
	} 
	if(!pView->IsKindOf(pClass)) 
	{ 
		AfxMessageBox(L"Can not locate the View."); 
		return NULL; 
	} 
	return (CDIP_SystemView*)pView; 
}

void CDIP_SystemDoc::OnCloseDocument()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDocument::OnCloseDocument();
}