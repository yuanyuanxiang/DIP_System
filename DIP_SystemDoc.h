// DIP_SystemDoc.h : interface of the CDIP_SystemDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIP_SYSTEMDOC_H)
#define AFX_DIP_SYSTEMDOC_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dib.h"

class CDIP_SystemView;

class CDIP_SystemDoc : public CDocument
{
protected: // create from serialization only
	CDIP_SystemDoc();
	virtual ~CDIP_SystemDoc();
	DECLARE_DYNCREATE(CDIP_SystemDoc)

// Attributes
public:
	CSize				m_sizeDoc;			//
	CDIB*				m_pDIB;				//
	CRectTracker		m_Tracker;			//
	int					m_nColorIndex;		//
	COLORREF			m_refColorBKG;		// 背景色
	CPoint				m_ptPoint_I;		// 种子点1
	CPoint				m_ptPoint_II;		// 种子点2
	CPoint				m_ptLastPos;		// 鼠标移动上一个位置
	CString				m_strCurrentFile;	//
	BOOL				m_bIsFrequency;		//
	int					m_nRgnNum;			//
	CRect				m_rect[1000];		//
	float				m_fZoomRatio;		// 快速放大或缩小的倍率

	// 原有
	CPalette*			m_palDIB;
	HDIB				m_hDIB;

	// 新增
	CString				m_sFileName;		//文件名称
	CString				m_sFilePath;		//文件全名
	BOOL				m_bIsDecoding;		//是否为解码图像
	BOOL				m_bJumpSecLine;		//是否跳过第二行

public:
	CImage* DIBImage() const
		{
			return m_pDIB->GetImage();
	}
	CSize GetDocSize() const
		{ return m_sizeDoc; }
	void SetDocSize();
	void SetDocSize(int cx, int cy);
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	//新增
	BOOL HDIB2CImage(HDIB hDIB);
	BOOL CreateTempFile(CString strFileName = L"TempFile.bmp");
	CDIP_SystemDoc* CreateNewDocument()// 新建空白文档
		{
			return (CDIP_SystemDoc *)GetDocTemplate()->OpenDocumentFile(NULL);
		}
	BOOL OpenPXMFile(LPCTSTR lpszPathName);// 打开PGM格式
	//原有
	CPalette* GetDocPalette() const
		{
			return m_palDIB;
		}
	HDIB GetHDIB() const
		{
			return m_hDIB;
		}
	void InitDIBData();
	void ReplaceHDIB(HDIB hDIB);
	BOOL OnOpenDocument(LPCTSTR lpszPathName, BOOL bReserved);
	BOOL OnSaveDocument(LPCTSTR lpszPathName, BOOL bReserved);
	CDIP_SystemView* GetView(CRuntimeClass* pClass);

// Generated message map functions
public:
	afx_msg void OnFileReopen();
	DECLARE_MESSAGE_MAP()

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_DIP_SYSTEMDOC_H)
