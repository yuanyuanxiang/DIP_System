#pragma once

#include "afxwin.h"
#include "resource.h"
#include "DIP_SystemDoc.h"
#include "DIP_SystemView.h"

// CZoomDlg 对话框

class CDIP_SystemDoc;

class CZoomDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CZoomDlg)

public:
	CZoomDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CZoomDlg();

	// 对话框数据
	enum { IDD = IDD_DLG_ZOOM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox					m_ZoomDlgCombo;
	CEdit						m_EditImageWidth;
	CEdit						m_EditImageHeight;
	CEdit						m_EditImageRatio;
	CComboBox					m_ComboMethod;
	int							m_nComboSelected;
	double						m_dWidthEdit;
	double						m_dHeightEdit;
	double						m_dImageRatio;
	int							m_nMethod;

	int							m_nWidthNew;
	int							m_nHeightNew;
	double						m_dWidthRatio;
	double						m_dHeightRatio;

	BYTE*						m_dataSrc;            //原始数据
	CDIP_SystemDoc*				m_pDoc;               //活动文档
	CDIP_SystemView*            m_pView;              //活动视图
	CImage*						m_pImage;             //当前图像
	int							m_nChannel;           //通道数
	long						m_nImageWidth;        //图像宽度
	long						m_nImageHeight;       //图像高度
	long						m_nRowLen;            //每行字节数
	long						m_nDatalen;           //总长度
	BOOL						m_bModified;          //图片被更新

	BOOL ZoomNormal(int widthnew, int heightnew);		//最临近插值
	BOOL ZoomInterpolation(int widthnew, int heightnew);//双线性插值
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedZoomDlgApply();
	afx_msg void OnCbnSelchangeZoomDlgCombo();
	afx_msg void OnSelchangeZoomDlgMethod();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

