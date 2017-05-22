// DIP_SystemView.h : interface of the CDIP_SystemView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIP_SYSTEMVIEW_H)
#define AFX_DIP_SYSTEMVIEW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VIEW_MAX_WIDTH 4096  //能查看的最大尺寸
#define VIEW_MIN_WIDTH 16    //能查看的最小尺寸
#define CHECK_IMAGE_NOT_NULL { if (GetDocument()->DIBImage() == NULL) return; }	//检查图像非空
#define CHECK_IF_BINARY_MAP  { if (GetDocument()->DIBImage()->GetBPP() == 1) return; }	//检测图像不为二进制位图

class CZoomDlg;

class CDIP_SystemView : public CScrollView
{
public: // create from serialization only
	CDIP_SystemView();
	DECLARE_DYNCREATE(CDIP_SystemView)

// Attributes
public:
	CDIP_SystemDoc* GetDocument();

// Operations
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);

// Implementation
public:
	CRect						m_PaintRect;		// 绘图的矩形区域
	int							m_nInteract;		// 一个交互的参数
	BOOL						m_bShowImage;		// 显示CImage图像
	int							m_nViewType;		// 当前的视图类型
	CZoomDlg*					m_pZoomDlg;			// 放大图像对话框
	bool						m_bLeftButtonDown;	// 鼠标左键被按下
	CToolTipCtrl				m_ToolTip;			// 用于显示像素值
	COLORREF					m_Color_I;			// 上一个颜色值
	COLORREF					m_Color_II;			// 当前的颜色值
	bool						m_bGetCurPixel;		// 获取像素值
	BOOL						m_bRNegativeImage;	// 是否为R负片
	BOOL						m_bGNegativeImage;	// 是否为G负片
	BOOL						m_bBNegativeImage;	// 是否为B负片
	int							m_nCurSeed;			// 种子点个数
	COLORREF					m_clrSeed_I;		// 种子点1颜色
	COLORREF					m_clrSeed_II;		// 种子点2颜色
	HACCEL						m_hHotKey;			// [1]快捷键

	void DrawCircle(CPoint point);
	void DoEmboss(int nEmbossMode);
	void CurrentRGB(CPoint point);
	void SetPaintRect(CSize size);
	void NegativeImage(int channel);					// 查看负片
	void ConvertImage(int R = 2, int G = 1, int B = 0);	// 转换色彩
	void ResetView();									// 重置视图
	virtual ~CDIP_SystemView();

// Generated message map functions
protected:
	afx_msg void OnDoRealize(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnTogray();
	afx_msg void OnRgnGrow();
	afx_msg void OnPointInvert();
	afx_msg void OnPointEqua();
	afx_msg void OnViewYIntensity();
	afx_msg void OnViewRIntensity();
	afx_msg void OnViewGIntensity();
	afx_msg void OnViewBIntensity();
	afx_msg void OnPointStre();
	afx_msg void OnPointWind();
	afx_msg void OnPointDt();
	afx_msg void OnPointDz();
	afx_msg void OnSetIntensity();
	afx_msg void OnPointSml();
	afx_msg void OnPointGml();
	afx_msg void OnDynamicCompress();
	afx_msg void OnGeomZoomin();
	afx_msg void OnGeomZoomout();
	afx_msg void OnFreqFour();
	afx_msg void OnButterworthL();
	afx_msg void OnEmboss_1();
	afx_msg void OnEmboss_2();
	afx_msg void OnEmboss_3();
	afx_msg void OnEmboss_4();
	afx_msg void OnEmboss_5();
	afx_msg void OnEmboss_6();
	afx_msg void OnEmboss_7();
	afx_msg void OnEmboss_8();
	afx_msg void OnButterworthH();
	afx_msg void OnMutualFilter();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLineSmoothFilter();
	afx_msg void OnLineQuickFilter();
	afx_msg void OnMedianFilter();
	afx_msg void OnJpegEncoding();
	afx_msg void OnJpegDecoding();
	afx_msg void OnRetrorseFilter();
	afx_msg void OnWienerFilter();
	afx_msg void OnSobel();
	afx_msg void OnPrewitt();
	afx_msg void OnRoberts();
	afx_msg void OnLaplacian();
	afx_msg void OnGuassLaplacian();
	afx_msg void OnHough();
	afx_msg void OnKrisch();
	afx_msg void OnContourdib();
	afx_msg void OnTracedib();
	afx_msg void OnIterateThreshold();
	afx_msg void OnFreqDct();
	afx_msg void OnFreqWalh();
	afx_msg void OnHErosion();
	afx_msg void OnVErosion();
	afx_msg void OnHDilation();
	afx_msg void OnVDilation();
	afx_msg void OnOpenOperate();
	afx_msg void OnCloseOperate();
	afx_msg void OnVhErosion();
	afx_msg void OnVhDilation();
	afx_msg void OnThin();
	afx_msg void OnRandomNoise();
	afx_msg void OnSaltNoise();
	afx_msg void OnPCXEncoding();
	afx_msg void OnPCXDecoding();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLineSmoothFilter2();
	afx_msg void OnLineSmoothFilter3();
	afx_msg void OnOverrunFilter();
	afx_msg void OnCutWave();
	afx_msg void OnPSE_Filter();
	afx_msg void OnMinPower();
	afx_msg void OnRotate();
	afx_msg void OnRectinication();
	afx_msg void OnThick();
	afx_msg void OnMat();
	afx_msg void OnHuffman();
	afx_msg void OnShannonFannon();
	afx_msg void OnLzwEncoding();
	afx_msg void OnLzwDecoding();
	afx_msg void OnSetWatermarking();
	afx_msg void OnGetWatermarking();
	afx_msg void OnDwt();
	afx_msg void OnIdwt();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnCallZoomDlg();
	afx_msg void OnCallColorgradDlg();
	afx_msg void OnCallCurveDlg();
	afx_msg void OnCallHistogramDlg();
	afx_msg void OnCallSpacefilterDlg();
	afx_msg void OnTempFile();
	afx_msg void OnUpdateTempFile(CCmdUI *pCmdUI);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	afx_msg void OnHistogramIi();
	afx_msg void OnCallStretchDlg();
	afx_msg void OnCallThresholdDlg();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);
	afx_msg void OnViewZoomin();
	afx_msg void OnViewZoomout();
	afx_msg void OnViewFitWindow();
	afx_msg void OnViewScreenSize();
	afx_msg void OnViewDefault();
	afx_msg void OnCallBppDlg();
	afx_msg void OnCallRotateDlg();
	afx_msg void OnFlipVertical();
	afx_msg void OnFlipHorizontal();
	afx_msg void OnTranspose();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnGetCurrentPixel();
	afx_msg void OnUpdateGetCurrentPixel(CCmdUI *pCmdUI);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnViewRChannelImage();
	afx_msg void OnViewGChannelImage();
	afx_msg void OnViewBChannelImage();
	afx_msg void OnConvertRgb2Rbg();
	afx_msg void OnConvertRgb2Brg();
	afx_msg void OnConvertRgb2Bgr();
	afx_msg void OnConvertRgb2Grb();
	afx_msg void OnConvertRgb2Gbr();
	afx_msg void OnViewAllChannelImage();
	afx_msg void OnUpdateViewRChannelImage(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewGChannelImage(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewBChannelImage(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewAllChannelImage(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewZoomin(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewZoomout(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewScreenSize(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewFitWindow(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewDefault(CCmdUI *pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnHotkeyAltZ();
	afx_msg void OnHotkeyCtrl0();
	afx_msg void OnHotkeyCtrlC();
	afx_msg void OnHotkeyCtrlF();
	afx_msg void OnHotkeyCtrlH();
	afx_msg void OnHotkeyCtrlL();
	afx_msg void OnHotkeyCtrlM();
	afx_msg void OnHotkeyCtrlN();
	afx_msg void OnHotkeyCtrlO();
	afx_msg void OnHotkeyCtrlP();
	afx_msg void OnHotkeyCtrlQ();
	afx_msg void OnHotkeyCtrlR();
	afx_msg void OnHotkeyCtrlS();
	afx_msg void OnHotkeyCtrlV();
	afx_msg void OnHotkeyCtrlW();
	afx_msg void OnHotkeyCtrlZ();
	afx_msg void OnHotkeyZoomIn();
	afx_msg void OnHotkeyZoomOut();
};

#ifndef _DEBUG  // debug version in DIP_SystemView.cpp
inline CDIP_SystemDoc* CDIP_SystemView::GetDocument()
   { return (CDIP_SystemDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_DIP_SYSTEMVIEW_H)
