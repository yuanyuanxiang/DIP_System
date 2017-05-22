// DIP_SystemView.cpp : implementation of the CDIP_SystemView class
//

#include "stdafx.h"
#include "DIP_System.h"

#include "DIP_SystemDoc.h"
#include "DIP_SystemView.h"
#include "MainFrm.h"
#include "SplitProc.h"
#include "GrayProc.h"
#include "DlgIntensity.h"
#include "DlgPointStre.h"
#include "DlgPointWin.h"
#include "DlgPointDT.h"
#include "DlgPointDZ.h"
#include "DLGSetAngle.h"
#include "DynamicCompressParam.h"
#include "GeoTrans.h"
#include "FreqCalculate.h"
#include "SpaceFilter.h"
#include "FreqFilter.h"
#include "DLGSetFreq.h"
#include "Coding.h"
#include "Morph.h"
#include "Noise.h"
#include "WaterMarking.h"

// 2014年7月新增
#include "ZoomDlg.h"
#include "ColorGradDlg.h"
#include "CurveDlg.h"
#include "HistogramDlg.h"
#include "SpaceFilterDlg.h"
#include "EnhancedHistDlg.h"
#include "StretchDlg.h"
#include "ThresholdDlg.h"
#include "ChangeBppDlg.h"
#include "RotateDlg.h"
#include "GeoOperator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 浮雕化模式
static enum EmbossType
{
	EMBOSS_1 = 0,
	EMBOSS_2,
	EMBOSS_3,
	EMBOSS_4,
	EMBOSS_5,
	EMBOSS_6,
	EMBOSS_7,
	EMBOSS_8
};

/////////////////////////////////////////////////////////////////////////////
// CDIP_SystemView

IMPLEMENT_DYNCREATE(CDIP_SystemView, CScrollView)

BEGIN_MESSAGE_MAP(CDIP_SystemView, CScrollView)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_TOGRAY, OnTogray)
	ON_COMMAND(ID_RGN_GROW, OnRgnGrow)
	ON_COMMAND(ID_POINT_INVERT, OnPointInvert)
	ON_COMMAND(ID_POINT_EQUA, OnPointEqua)
	ON_COMMAND(ID_VIEW_INTENSITY, OnViewYIntensity)
	ON_COMMAND(ID_VIEW_R_INTENSITY, OnViewRIntensity)
	ON_COMMAND(ID_VIEW_G_INTENSITY, OnViewGIntensity)
	ON_COMMAND(ID_VIEW_B_INTENSITY, OnViewBIntensity)
	ON_COMMAND(ID_POINT_STRE, OnPointStre)
	ON_COMMAND(ID_POINT_WIND, OnPointWind)
	ON_COMMAND(ID_POINT_DT, OnPointDt)
	ON_COMMAND(ID_POINT_DZ, OnPointDz)
	ON_COMMAND(ID_SET_INTENSITY, OnSetIntensity)
	ON_COMMAND(ID_POINT_SML, OnPointSml)
	ON_COMMAND(ID_POINT_GML, OnPointGml)
	ON_COMMAND(ID_DYNAMIC_COMPRESS, OnDynamicCompress)
	ON_COMMAND(ID_GEOM_ZOOMIN, OnGeomZoomin)
	ON_COMMAND(ID_GEOM_ZOOMOUT, OnGeomZoomout)
	ON_COMMAND(ID_FREQ_FOUR, OnFreqFour)
	ON_COMMAND(ID_BUTTERWORTH_L, OnButterworthL)
	ON_COMMAND(IDM_EMBOSS_1, OnEmboss_1)
	ON_COMMAND(IDM_EMBOSS_2, OnEmboss_2)
	ON_COMMAND(IDM_EMBOSS_3, OnEmboss_3)
	ON_COMMAND(IDM_EMBOSS_4, OnEmboss_4)
	ON_COMMAND(IDM_EMBOSS_5, OnEmboss_5)
	ON_COMMAND(IDM_EMBOSS_6, OnEmboss_6)
	ON_COMMAND(IDM_EMBOSS_7, OnEmboss_7)
	ON_COMMAND(IDM_EMBOSS_8, OnEmboss_8)
	ON_COMMAND(ID_BUTTERWORTH_H, OnButterworthH)
	ON_COMMAND(ID_MUTUAL_FILTER, OnMutualFilter)
	ON_COMMAND(ID_LINE_SMOOTH_FILTER, OnLineSmoothFilter)
	ON_COMMAND(ID_LINE_QUICK_FILTER, OnLineQuickFilter)
	ON_COMMAND(ID_MEDIAN_FILTER, OnMedianFilter)
	ON_COMMAND(ID_JPEG_ENCODING, OnJpegEncoding)
	ON_COMMAND(ID_JPEG_DECODING, OnJpegDecoding)
	ON_COMMAND(ID_RETRORSE_FILTER, OnRetrorseFilter)
	ON_COMMAND(ID_WIENER_FILTER, OnWienerFilter)
	ON_COMMAND(ID_SOBEL, OnSobel)
	ON_COMMAND(ID_PREWITT, OnPrewitt)
	ON_COMMAND(ID_ROBERTS, OnRoberts)
	ON_COMMAND(ID_LAPLACIAN, OnLaplacian)
	ON_COMMAND(ID_GUASS__LAPLACIAN, OnGuassLaplacian)
	ON_COMMAND(ID_HOUGH, OnHough)
	ON_COMMAND(ID_KRISCH, OnKrisch)
	ON_COMMAND(ID_CONTOURDIB, OnContourdib)
	ON_COMMAND(ID_TRACEDIB, OnTracedib)
	ON_COMMAND(ID_ITERATE_THRESHOLD, OnIterateThreshold)
	ON_COMMAND(ID_FREQ_DCT, OnFreqDct)
	ON_COMMAND(ID_FREQ_WALH, OnFreqWalh)
	ON_COMMAND(ID_H_EROSION, OnHErosion)
	ON_COMMAND(ID_V_EROSION, OnVErosion)
	ON_COMMAND(ID_H_DILATION, OnHDilation)
	ON_COMMAND(ID_V_DILATION, OnVDilation)
	ON_COMMAND(ID_OPEN_OPERATE, OnOpenOperate)
	ON_COMMAND(ID_CLOSE_OPERATE, OnCloseOperate)
	ON_COMMAND(ID_VH_EROSION, OnVhErosion)
	ON_COMMAND(ID_VH_DILATION, OnVhDilation)
	ON_COMMAND(ID_THIN, OnThin)
	ON_COMMAND(ID_RANDOM_NOISE, OnRandomNoise)
	ON_COMMAND(ID_SALT_NOISE, OnSaltNoise)
	ON_COMMAND(ID_PCX_ENCODING, OnPCXEncoding)
	ON_COMMAND(ID_PCX_DECODING, OnPCXDecoding)
	ON_COMMAND(ID_LINE_SMOOTH_FILTER2, OnLineSmoothFilter2)
	ON_COMMAND(ID_LINE_SMOOTH_FILTER3, OnLineSmoothFilter3)
	ON_COMMAND(ID_OVERRUN_FILTER, OnOverrunFilter)
	ON_COMMAND(ID_CUTWAVE, OnCutWave)
	ON_COMMAND(ID_PSE_FILTER, OnPSE_Filter)
	ON_COMMAND(ID_MIN_POWER, OnMinPower)
	ON_COMMAND(ID_ROTATE, OnRotate)
	ON_COMMAND(ID_RECTINICATION, OnRectinication)
	ON_COMMAND(ID_THICK, OnThick)
	ON_COMMAND(ID_MAT, OnMat)
	ON_COMMAND(ID_HUFFMAN, OnHuffman)
	ON_COMMAND(ID_SHANNON_FANNON, OnShannonFannon)
	ON_COMMAND(ID_LZW_ENCODING, OnLzwEncoding)
	ON_COMMAND(ID_LZW_DECODING, OnLzwDecoding)
	ON_COMMAND(ID_SET_WATERMARKING, OnSetWatermarking)
	ON_COMMAND(ID_GET_WATERMARKING, OnGetWatermarking)
	ON_COMMAND(ID_DWT, OnDwt)
	ON_COMMAND(ID_IDWT, OnIdwt)
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	ON_COMMAND(ID_CALL_ZOOM_DLG, &CDIP_SystemView::OnCallZoomDlg)
	ON_COMMAND(ID_CALL_COLORGRAD_DLG, &CDIP_SystemView::OnCallColorgradDlg)
	ON_COMMAND(ID_CALL_CURVE_DLG, &CDIP_SystemView::OnCallCurveDlg)
	ON_COMMAND(ID_CALL_HISTOGRAM_DLG, &CDIP_SystemView::OnCallHistogramDlg)
	ON_COMMAND(ID_CALL_SPACEFILTER_DLG, &CDIP_SystemView::OnCallSpacefilterDlg)
	ON_COMMAND(ID_TEMP_FILE, &CDIP_SystemView::OnTempFile)
	ON_UPDATE_COMMAND_UI(ID_TEMP_FILE, &CDIP_SystemView::OnUpdateTempFile)
	ON_COMMAND(ID_HISTOGRAM_II, &CDIP_SystemView::OnHistogramIi)
	ON_COMMAND(ID_CALL_STRETCH_DLG, &CDIP_SystemView::OnCallStretchDlg)
	ON_COMMAND(ID_CALL_THRESHOLD_DLG, &CDIP_SystemView::OnCallThresholdDlg)
	ON_COMMAND(ID_EDIT_UNDO, &CDIP_SystemView::OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, &CDIP_SystemView::OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CDIP_SystemView::OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CDIP_SystemView::OnUpdateEditRedo)
	ON_COMMAND(ID_VIEW_ZOOMIN, &CDIP_SystemView::OnViewZoomin)
	ON_COMMAND(ID_VIEW_ZOOMOUT, &CDIP_SystemView::OnViewZoomout)
	ON_COMMAND(ID_VIEW_FIT_WINDOW, &CDIP_SystemView::OnViewFitWindow)
	ON_COMMAND(ID_VIEW_SCREEN_SIZE, &CDIP_SystemView::OnViewScreenSize)
	ON_COMMAND(ID_VIEW_DEFAULT, &CDIP_SystemView::OnViewDefault)
	ON_COMMAND(ID_CALL_BPP_DLG, &CDIP_SystemView::OnCallBppDlg)
	ON_COMMAND(ID_CALL_ROTATE_DLG, &CDIP_SystemView::OnCallRotateDlg)
	ON_COMMAND(ID_FLIP_VERTICAL, &CDIP_SystemView::OnFlipVertical)
	ON_COMMAND(ID_FLIP_HORIZONTAL, &CDIP_SystemView::OnFlipHorizontal)
	ON_COMMAND(ID_TRANSPOSE, &CDIP_SystemView::OnTranspose)
	ON_COMMAND(ID_GET_CURRENT_PIXEL, &CDIP_SystemView::OnGetCurrentPixel)
	ON_UPDATE_COMMAND_UI(ID_GET_CURRENT_PIXEL, &CDIP_SystemView::OnUpdateGetCurrentPixel)
	ON_COMMAND(ID_VIEW_R_CHANNEL_IMAGE, &CDIP_SystemView::OnViewRChannelImage)
	ON_COMMAND(ID_VIEW_G_CHANNEL_IMAGE, &CDIP_SystemView::OnViewGChannelImage)
	ON_COMMAND(ID_VIEW_B_CHANNEL_IMAGE, &CDIP_SystemView::OnViewBChannelImage)
	ON_COMMAND(ID_CONVERT_RGB_2_RBG, &CDIP_SystemView::OnConvertRgb2Rbg)
	ON_COMMAND(ID_CONVERT_RGB_2_BRG, &CDIP_SystemView::OnConvertRgb2Brg)
	ON_COMMAND(ID_CONVERT_RGB_2_BGR, &CDIP_SystemView::OnConvertRgb2Bgr)
	ON_COMMAND(ID_CONVERT_RGB_2_GRB, &CDIP_SystemView::OnConvertRgb2Grb)
	ON_COMMAND(ID_CONVERT_RGB_2_GBR, &CDIP_SystemView::OnConvertRgb2Gbr)
	ON_COMMAND(ID_VIEW_ALL_CHANNEL_IMAGE, &CDIP_SystemView::OnViewAllChannelImage)
	ON_UPDATE_COMMAND_UI(ID_VIEW_R_CHANNEL_IMAGE, &CDIP_SystemView::OnUpdateViewRChannelImage)
	ON_UPDATE_COMMAND_UI(ID_VIEW_G_CHANNEL_IMAGE, &CDIP_SystemView::OnUpdateViewGChannelImage)
	ON_UPDATE_COMMAND_UI(ID_VIEW_B_CHANNEL_IMAGE, &CDIP_SystemView::OnUpdateViewBChannelImage)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ALL_CHANNEL_IMAGE, &CDIP_SystemView::OnUpdateViewAllChannelImage)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMIN, &CDIP_SystemView::OnUpdateViewZoomin)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMOUT, &CDIP_SystemView::OnUpdateViewZoomout)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCREEN_SIZE, &CDIP_SystemView::OnUpdateViewScreenSize)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FIT_WINDOW, &CDIP_SystemView::OnUpdateViewFitWindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DEFAULT, &CDIP_SystemView::OnUpdateViewDefault)
	ON_COMMAND(ID_HOTKEY_ALT_Z, &CDIP_SystemView::OnHotkeyAltZ)
	ON_COMMAND(ID_HOTKEY_CTRL_0, &CDIP_SystemView::OnHotkeyCtrl0)
	ON_COMMAND(ID_HOTKEY_CTRL_C, &CDIP_SystemView::OnHotkeyCtrlC)
	ON_COMMAND(ID_HOTKEY_CTRL_F, &CDIP_SystemView::OnHotkeyCtrlF)
	ON_COMMAND(ID_HOTKEY_CTRL_H, &CDIP_SystemView::OnHotkeyCtrlH)
	ON_COMMAND(ID_HOTKEY_CTRL_L, &CDIP_SystemView::OnHotkeyCtrlL)
	ON_COMMAND(ID_HOTKEY_CTRL_M, &CDIP_SystemView::OnHotkeyCtrlM)
	ON_COMMAND(ID_HOTKEY_CTRL_N, &CDIP_SystemView::OnHotkeyCtrlN)
	ON_COMMAND(ID_HOTKEY_CTRL_O, &CDIP_SystemView::OnHotkeyCtrlO)
	ON_COMMAND(ID_HOTKEY_CTRL_P, &CDIP_SystemView::OnHotkeyCtrlP)
	ON_COMMAND(ID_HOTKEY_CTRL_Q, &CDIP_SystemView::OnHotkeyCtrlQ)
	ON_COMMAND(ID_HOTKEY_CTRL_R, &CDIP_SystemView::OnHotkeyCtrlR)
	ON_COMMAND(ID_HOTKEY_CTRL_S, &CDIP_SystemView::OnHotkeyCtrlS)
	ON_COMMAND(ID_HOTKEY_CTRL_V, &CDIP_SystemView::OnHotkeyCtrlV)
	ON_COMMAND(ID_HOTKEY_CTRL_W, &CDIP_SystemView::OnHotkeyCtrlW)
	ON_COMMAND(ID_HOTKEY_CTRL_Z, &CDIP_SystemView::OnHotkeyCtrlZ)
	ON_COMMAND(ID_HOTKEY_ZOOM_IN, &CDIP_SystemView::OnHotkeyZoomIn)
	ON_COMMAND(ID_HOTKEY_ZOOM_OUT, &CDIP_SystemView::OnHotkeyZoomOut)
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDIP_SystemView construction/destruction
CDIP_SystemView::CDIP_SystemView()
{
	m_PaintRect = CRect();
	m_nInteract = 0;
	m_bShowImage = TRUE;
	m_nViewType = ID_VIEW_DEFAULT;

	m_pZoomDlg = new CZoomDlg;
	m_pZoomDlg->Create(IDD_DLG_ZOOM, this);
	m_pZoomDlg->SETDLGICON(IDI_ZOOM_DLG);

	m_Color_I = m_Color_II = RGB(0, 0, 0);

	m_bGetCurPixel = false;
	m_bRNegativeImage = 0;
	m_bGNegativeImage = 0;
	m_bBNegativeImage = 0;
	m_nCurSeed = 1;
}

CDIP_SystemView::~CDIP_SystemView()
{
	DELETE_WND(m_pZoomDlg);
}

BOOL CDIP_SystemView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////[辅助函数]/////////////////////////////////////////////////

///////////////////////////////////////////
//	获取并报告当前点的颜色值			 //	
///////////////////////////////////////////
void CDIP_SystemView::CurrentRGB(CPoint point)
{
	CDC* pDC = GetDC();
	m_Color_I = m_Color_II;
	m_Color_II = pDC->GetPixel(point);
	if (m_Color_I == m_Color_II)
		return;
	// 各颜色分量
	int R = GetRValue(m_Color_II);
	int G = GetGValue(m_Color_II);
	int B = GetBValue(m_Color_II);
	// 报告当前点颜色值
	CString  strRGB;
	strRGB.Format(_T("R = %d\nG = %d\nB = %d"), R, G, B);
	m_ToolTip.UpdateTipText(strRGB, this);
}

// 设置绘图区大小
void CDIP_SystemView::SetPaintRect(CSize size)
{
	m_PaintRect.right = size.cx;
	m_PaintRect.bottom = size.cy;
}

void CDIP_SystemView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	//按右键退出拾取像素
	m_bGetCurPixel = false;
	m_nInteract = 0;
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	ReleaseCapture();
	CScrollView::OnRButtonDown(nFlags, point);
}


void CDIP_SystemView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CScrollView::OnRButtonUp(nFlags, point);
}



void CDIP_SystemView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bGetCurPixel)
	{
		CDIP_SystemDoc* pDoc = GetDocument();
		CPoint curPos = pDoc->m_ptLastPos;
		pDoc->m_ptLastPos = point;
		// 鼠标移动了才更新tip
		if (curPos != pDoc->m_ptLastPos)
			CurrentRGB(point);
	}

	CScrollView::OnMouseMove(nFlags, point);
}


void CDIP_SystemView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CDIP_SystemDoc* pDoc = GetDocument();
	if (m_bLeftButtonDown)
	{
		m_bLeftButtonDown = false;
		CDC* pDC = GetDC();
		if (m_nCurSeed == 1)
		{
			pDoc->m_ptPoint_I = point;
			// 获取种子一的RGB值
			m_clrSeed_I = pDC->GetPixel(pDoc->m_ptPoint_I);
			m_nCurSeed = 2;
		}
		else
		{
			pDoc->m_ptPoint_II = point;
			// 获取种子二的RGB值
			m_clrSeed_II = pDC->GetPixel(pDoc->m_ptPoint_II);
			m_nCurSeed = 1;
		}
		ReleaseDC(pDC);
		Invalidate(TRUE);
	}

	CScrollView::OnLButtonUp(nFlags, point);
}

void CDIP_SystemView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bLeftButtonDown = true;

	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	// 在交互状态下对当前点画圆
	DrawCircle(point);

	// 重画视图
	Invalidate();

	// 获取设备环境句柄
	CDC* pDC = GetDC();
	
	// 判断是否选中已圈矩形
	BOOL bResult = pDoc->m_Tracker.HitTest(point) != CRectTracker::hitNothing;
	
	// 如果选中矩形
	if (bResult)
	{
		if(pDoc->m_bIsFrequency)
		{
			// 获取当前滚动位置
			CPoint pt = GetScrollPosition();

			// 修正选中矩形在全图中的坐标
			pDoc->m_rect[pDoc->m_nRgnNum] = pDoc->m_Tracker.m_rect;
			pDoc->m_rect[pDoc->m_nRgnNum].top += pt.y;
			pDoc->m_rect[pDoc->m_nRgnNum].bottom += pt.y;
			pDoc->m_rect[pDoc->m_nRgnNum].left += pt.x;
			pDoc->m_rect[pDoc->m_nRgnNum].right += pt.x;
		
			// 获取当前图象尺寸
			CSize size = pDoc->GetDocSize();

			// 越界保护
			if (pDoc->m_rect[pDoc->m_nRgnNum].top < 0)
				pDoc->m_rect[pDoc->m_nRgnNum].top = 0;
			if (pDoc->m_rect[pDoc->m_nRgnNum].bottom > size.cy)
				pDoc->m_rect[pDoc->m_nRgnNum].bottom = size.cy;
			if (pDoc->m_rect[pDoc->m_nRgnNum].left < 0)
				pDoc->m_rect[pDoc->m_nRgnNum].left = 0;
			if (pDoc->m_rect[pDoc->m_nRgnNum].right > size.cx)
				pDoc->m_rect[pDoc->m_nRgnNum].right = size.cx;

			// 设置选中区域的颜色
			pDoc->m_pDIB->SetRgnColor(pDoc->m_rect[pDoc->m_nRgnNum], RGB(0, 0, 0));
		
			// 重画视图
			InvalidateRect(pDoc->m_rect[pDoc->m_nRgnNum]);

			// 选中区域个数计数
			pDoc->m_nRgnNum++;
	
			// 设置脏标记
			pDoc->SetModifiedFlag(TRUE);
		}
	}
	// 如果不是选中区域则拖放
	else
		pDoc->m_Tracker.TrackRubberBand(this, point, TRUE);

	// 画选中区域边框
	pDoc->m_Tracker.Draw(pDC);

	// 释放资源
	ReleaseDC(pDC);

	CScrollView::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////
//	在当前点画圆						 //
///////////////////////////////////////////
void CDIP_SystemView::DrawCircle(CPoint point)
{
	if (m_nInteract > 0)
	{
		// 获取文档
		CDIP_SystemDoc* pDoc = GetDocument();

		// 获取当前图象尺寸
		CSize size = pDoc->GetDocSize();

		// 获取当前滚动位置
		CPoint pt = GetScrollPosition();
		
		// 标定的区域
		CRect rect(point.x + pt.x + 8, point.y + pt.y + 8, point.x + pt.x + 24, point.y + pt.y + 24);

		// 越界保护
		if (rect.top < 0 || rect.bottom > size.cy + 8 || rect.left < -8 || rect.right > size.cx + 8)
			return;		
			
		// 获取设备环境句柄
		CDC* pDC = GetDC();
		
		// 设置选中区域的颜色
		COLORREF col;
		if (m_nInteract == 1)
			col = RGB(255, 0, 0);
		if (m_nInteract == 2)
			col = RGB(0, 0, 0);
		if (m_nInteract == 3)
			col = RGB(255, 255, 255);
		pDoc->m_pDIB->SetCircleColor(rect, col);
		
		// 重画视图
		InvalidateRect(rect);

		// 释放资源
		ReleaseDC(pDC);
	}
}

///////////////////////////////////////////
//	重画视图							 //
///////////////////////////////////////////
void CDIP_SystemView::OnDraw(CDC* pDC)
{
	CDIP_SystemDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	// 获取指针
	HDIB hDIB = pDoc->GetHDIB();
	CImage* pImage = pDoc->DIBImage();
	if (hDIB == NULL) m_bShowImage = TRUE;

	// 使视图适应窗口
	if (m_nViewType == ID_VIEW_FIT_WINDOW)
	{
		CRect rect;
		GetClientRect(&rect);
		m_PaintRect = rect;
		pDoc->SetDocSize(m_PaintRect.right, m_PaintRect.bottom);
	}

	// 显示当前图像
	if (m_bShowImage)
	{
		if (pImage != NULL)
		{
			int cxDIB = (int)pDoc->m_pDIB->Width();
			int cyDIB = (int)pDoc->m_pDIB->Height();
			CRect rcDIB(0, 0, cxDIB, cyDIB);
			CRect rcDest = m_PaintRect;
			// 判断是否是打印
			if (pDC->IsPrinting())
			{
				// 是打印，计算输出图像的位置和大小，以便符合页面

				// 获取打印页面的水平宽度(象素)
				int cxPage = pDC->GetDeviceCaps(HORZRES);

				// 获取打印页面的垂直高度(象素)
				int cyPage = pDC->GetDeviceCaps(VERTRES);

				// 获取打印机每英寸象素数
				int cxInch = pDC->GetDeviceCaps(LOGPIXELSX);
				int cyInch = pDC->GetDeviceCaps(LOGPIXELSY);

				// 计算打印图像大小（缩放，根据页面宽度调整图像大小）
				rcDest.top = rcDest.left = 0;
				rcDest.bottom = (int)(((double)cyDIB * cxPage * cyInch) / ((double)cxDIB * cxInch));
				rcDest.right = cxPage;

				// 计算打印图像位置（垂直居中）
				int temp = cyPage - (rcDest.bottom - rcDest.top);
				rcDest.bottom += temp / 2;
				rcDest.top += temp / 2;
			}
			pDoc->m_pDIB->Paint(pDC->m_hDC, &rcDest, &rcDIB);
		}
	}
	//显示缓存图像
	else
	{
		if (hDIB != NULL)
		{
			LPBYTE lpDIB = (LPBYTE) ::GlobalLock((HGLOBAL)hDIB);
			int cxDIB = (int)pDoc->m_pDIB->Width(lpDIB);
			int cyDIB = (int)pDoc->m_pDIB->Height(lpDIB);
			::GlobalUnlock((HGLOBAL)hDIB);
			CRect rcDIB(0, 0, cxDIB, cyDIB);
			CRect rcDest = m_PaintRect;
			// 判断是否是打印
			if (pDC->IsPrinting())
			{
				// 是打印，计算输出图像的位置和大小，以便符合页面

				// 获取打印页面的水平宽度(象素)
				int cxPage = pDC->GetDeviceCaps(HORZRES);

				// 获取打印页面的垂直高度(象素)
				int cyPage = pDC->GetDeviceCaps(VERTRES);

				// 获取打印机每英寸象素数
				int cxInch = pDC->GetDeviceCaps(LOGPIXELSX);
				int cyInch = pDC->GetDeviceCaps(LOGPIXELSY);

				// 计算打印图像大小（缩放，根据页面宽度调整图像大小）
				rcDest.top = rcDest.left = 0;
				rcDest.bottom = (int)(((double)cyDIB * cxPage * cyInch) / ((double)cxDIB * cxInch));
				rcDest.right = cxPage;

				// 计算打印图像位置（垂直居中）
				int temp = cyPage - (rcDest.bottom - rcDest.top);
				rcDest.bottom += temp / 2;
				rcDest.top += temp / 2;
			}
			pDoc->m_pDIB->Paint(pDC->m_hDC, &rcDest, pDoc->GetHDIB(), &rcDIB, pDoc->GetDocPalette());
		}
	}

	// 画种子点1
	if (pDoc->m_ptPoint_I != CPoint(0, 0))
	{
		pDC->Ellipse(pDoc->m_ptPoint_I.x - 3, pDoc->m_ptPoint_I.y - 3, 
			pDoc->m_ptPoint_I.x + 3, pDoc->m_ptPoint_I.y + 3);
	}

	// 画种子点2
	if (pDoc->m_ptPoint_II != CPoint(0, 0))
	{
		pDC->Ellipse(pDoc->m_ptPoint_II.x - 3, pDoc->m_ptPoint_II.y - 3, 
			pDoc->m_ptPoint_II.x + 3, pDoc->m_ptPoint_II.y + 3);
	}
}

void CDIP_SystemView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	// ref：http://bbs.csdn.net/topics/230047928
	m_bLeftButtonDown = false;
	m_ToolTip.Create(this, TTS_ALWAYSTIP);       
	m_ToolTip.AddTool(this, _T(""));   
	m_ToolTip.SetTipBkColor(RGB(0,222,0));
	m_ToolTip.SetMaxTipWidth(50);
	m_ToolTip.SetDelayTime(500);
	SetScrollSizes(MM_TEXT, GetDocument()->GetDocSize());
}

/////////////////////////////////////////////////////////////////////////////
//									 图像打印							   //
/////////////////////////////////////////////////////////////////////////////
BOOL CDIP_SystemView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 设置总页数为一。
	pInfo->SetMaxPage(1);

	return DoPreparePrinting(pInfo);
}

void CDIP_SystemView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDIP_SystemView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CDIP_SystemView diagnostics

#ifdef _DEBUG
void CDIP_SystemView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CDIP_SystemView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CDIP_SystemDoc* CDIP_SystemView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDIP_SystemDoc)));
	return (CDIP_SystemDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDIP_SystemView message handlers
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//																		   //
//								基本位图操作							   //
//																		   //
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////
//	设置子窗体默认背景色为m_refColorBKG  //
///////////////////////////////////////////
BOOL CDIP_SystemView::OnEraseBkgnd(CDC* pDC) 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	// 创建一个Brush
	CBrush brush(pDoc->m_refColorBKG);                                              
                                                                                  
	// 保存以前的Brush
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	
	// 获取重绘区域
	CRect rectClip;
	pDC->GetClipBox(&rectClip);
	
	if (m_nViewType != ID_VIEW_FIT_WINDOW)
	{
		// 重绘，除开绘图区域
		pDC->PatBlt(m_PaintRect.right, 0, rectClip.right - m_PaintRect.right, rectClip.bottom, PATCOPY);
		pDC->PatBlt(0, m_PaintRect.bottom, rectClip.right, rectClip.bottom - m_PaintRect.bottom, PATCOPY);
	}

	// 恢复以前的Brush
	pDC->SelectObject(pOldBrush);                                                 
	
	return TRUE;
}

///////////////////////////////////////////
//	实现新的调色板						 //
///////////////////////////////////////////
void CDIP_SystemView::OnDoRealize(WPARAM wParam, LPARAM lParam)
{
	ASSERT(wParam != NULL);

	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();
	
	// 判断DIB是否为空
	if (pDoc->DIBImage() == NULL)
	{
		// 直接返回
		return;
	}

	// 获取MainFrame
	CMainFrame* pAppFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	ASSERT_KINDOF(CMainFrame, pAppFrame);

	CClientDC appDC(pAppFrame);
}

///////////////////////////////////////////
//	计算窗口视图大小，以计算滚动条设置   //	
///////////////////////////////////////////
void CDIP_SystemView::CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType) 
{
	CScrollView::CalcWindowRect(lpClientRect, nAdjustType);
	ASSERT(GetDocument() != NULL);
	SetScrollSizes(MM_TEXT, GetDocument()->GetDocSize());
}

void CDIP_SystemView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if (bActivate)
	{
		ASSERT(pActivateView == this);
		OnDoRealize((WPARAM)m_hWnd, 0);   // same as SendMessage(WM_DOREALIZE);
	}
}

///////////////////////////////////////////
//	复制当前DIB到剪贴版					 //	
///////////////////////////////////////////
void CDIP_SystemView::OnEditCopy()
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (OpenClipboard())
	{
		if (pDoc->DIBImage() == NULL)
		{
			CloseClipboard();
			return;
		}
		CImage* pSrc = pDoc->m_pDIB->MakeCopy();
		CBitmap* pDst = new CBitmap;
		HBITMAP bi = pSrc->Detach();
		pDst->Attach(bi);
		int width = pDoc->m_pDIB->Width();
		int height = pDoc->m_pDIB->Height();

		//获得当前DC
		CDC* pDC = GetDC();

		//创建关联位图
		CBitmap   cBmp;   
		cBmp.CreateCompatibleBitmap(pDC, width, height);   

		//创建与当前DC适配的内存DC
		CDC   cMemDC;   
		cMemDC.CreateCompatibleDC(pDC); 

		//为内存DC关联位图
		CBitmap* pOldBitmap = cMemDC.SelectObject(&cBmp);

		//在内存DC绘制文档内容
		OnDraw(&cMemDC);

		AfxGetApp()->m_pMainWnd->OpenClipboard() ;
		EmptyClipboard();
		SetClipboardData(CF_BITMAP, cBmp.GetSafeHandle() ) ;
		CloseClipboard();
		cMemDC.SelectObject(pOldBitmap);
		cMemDC.DeleteDC();
		ReleaseDC(pDC);
		BITMAP bmp = {0};
		cBmp.GetBitmap(&bmp);
		cBmp.Detach();

		delete pSrc;
		delete pDst;
	}
}

void CDIP_SystemView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	// 如果当前DIB对象不空，复制菜单项有效
	pCmdUI->Enable(GetDocument()->DIBImage() != NULL);
}

///////////////////////////////////////////
//	从剪贴版粘贴DIB到当前视图			 //	
///////////////////////////////////////////
void CDIP_SystemView::OnEditPaste() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();
	if (pDoc->DIBImage() != NULL)
		pDoc = pDoc->CreateNewDocument();
	if (OpenClipboard())
	{
		HBITMAP handle = (HBITMAP)GetClipboardData(CF_BITMAP);
		CBitmap* pBitmap = CBitmap::FromHandle(handle);
		CImage* pImage;
		// 取得源数据
		BITMAP bmp = {0};
		pBitmap->GetBitmap(&bmp);
		long Length = bmp.bmHeight * bmp.bmWidthBytes;
		BYTE* pSrc = new BYTE[Length];
		pBitmap->GetBitmapBits(Length, pSrc);
		// 给目标地址拷贝数据
		pImage = new CImage;
		pImage->Create(bmp.bmWidth, bmp.bmHeight, bmp.bmBitsPixel);
		BYTE* pDst = (BYTE*)pImage->GetBits() + (pImage->GetPitch()*(pImage->GetHeight() - 1));
		memcpy(pDst, pSrc, Length);
		pDoc->m_pDIB->m_UndoStack.Push(pImage);
		pDoc->m_pDIB->SetImage(pImage);
		pDoc->SetDocSize();
		pDoc->GetView(RUNTIME_CLASS(CDIP_SystemView))->m_PaintRect = CRect(0, 0, bmp.bmWidth, bmp.bmHeight);
		Invalidate();
		delete [] pSrc;
		CloseClipboard();
	}
}

void CDIP_SystemView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	// 如果当前剪贴板中有DIB对象，粘贴菜单项有效
	pCmdUI->Enable(::IsClipboardFormatAvailable(CF_DIB));
}

///////////////////////////////////////////
//	放大当前视图						 //	
///////////////////////////////////////////
void CDIP_SystemView::OnGeomZoomin() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	// 初始化脏标记为FALSE
	pDoc->SetModifiedFlag(FALSE);
 
	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 调用ZoomDIB()函数获取放大后的DIB
	CGeoTrans clsGeoTrans(pDoc->m_pDIB);
	clsGeoTrans.ZoomDIB(pDoc->m_fZoomRatio);

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);

	// 重新设置滚动视图大小
	SetScrollSizes(MM_TEXT, pDoc->GetDocSize());

	// 更新视图
	pDoc->UpdateAllViews(NULL);
}

///////////////////////////////////////////
//	缩小当前视图						 //	
///////////////////////////////////////////
void CDIP_SystemView::OnGeomZoomout() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	// 初始化脏标记为FALSE
	pDoc->SetModifiedFlag(FALSE);
 
	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 调用ZoomDIB()函数获取缩小后的DIB
	CGeoTrans clsGeoTrans(pDoc->m_pDIB);
	clsGeoTrans.ZoomDIB(1.0f / pDoc->m_fZoomRatio);

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);

	// 重新设置滚动视图大小
	SetScrollSizes(MM_TEXT, pDoc->GetDocSize());

	// 更新视图
	pDoc->UpdateAllViews(NULL);
}

/////////////////////////////////////////////////////////////////////////////
//																		   //
//								图像处理与分析							   //
//																		   //
/////////////////////////////////////////////////////////////////////////////

///////////////////////////[灰度处理部分]////////////////////////////////////////////

///////////////////////////////////////////
//	将彩色位图转化为灰度图像			 //	
///////////////////////////////////////////
void CDIP_SystemView::OnTogray() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	// 保存前一个状态
	if (pDoc->DIBImage() == NULL) return;
	if (pDoc->m_pDIB->Channel() == 1) return;
	pDoc->m_pDIB->PushCopy();

	// 进行灰度转化
	CGrayProc clsGrayProc(pDoc->m_pDIB);
	clsGrayProc.ToGray();
	
	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像实施反色变换处理				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnPointInvert() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 进行反色处理
	CGrayProc clsGrayProc(pDoc->m_pDIB);
	clsGrayProc.PointInvert();
	
	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像进行灰度分布均衡化处理		 //	
///////////////////////////////////////////
void CDIP_SystemView::OnPointEqua() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 进行灰度分布均衡化处理
	CGrayProc clsGrayProc(pDoc->m_pDIB);
	clsGrayProc.PointEqua();
	
	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	查看图像的灰度直方图				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnViewYIntensity() 
{
	
	int i;

	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();
	if (pDoc->DIBImage() == NULL) return;

	// 各颜色分量的灰度分布密度
	float fPs_Y[256];

	// 计算灰度分布密度
	CGrayProc clsGrayProc(pDoc->m_pDIB);
	clsGrayProc.GetIntensity(fPs_Y);
	
	CDlgIntensity dlg;

	// 传递灰度分布密度数据给面板类
	for (i = 0; i <256; i++)
		dlg.m_fIntensity[i] = fPs_Y[i];

	// 显示面板
	dlg.DoModal();
}

void CDIP_SystemView::OnViewRIntensity() 
{
	
	int i;

	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();
	if (pDoc->DIBImage() == NULL) return;

	// 各颜色分量的灰度分布密度
	float fPs_R[256];
	float fPs_G[256];
	float fPs_B[256];

	// 计算灰度分布密度
	CGrayProc clsGrayProc(pDoc->m_pDIB);
	clsGrayProc.GetIntensity(fPs_R, fPs_G, fPs_B);
	
	CDlgIntensity dlg;

	// 传递灰度分布密度数据给面板类
	for (i = 0; i <256; i++)
		dlg.m_fIntensity[i] = fPs_R[i];

	// 显示面板
	dlg.DoModal();
}

void CDIP_SystemView::OnViewGIntensity() 
{
	
	int i;

	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();
	if (pDoc->DIBImage() == NULL) return;

	// 各颜色分量的灰度分布密度
	float fPs_R[256];
	float fPs_G[256];
	float fPs_B[256];

	// 计算灰度分布密度
	CGrayProc clsGrayProc(pDoc->m_pDIB);
	clsGrayProc.GetIntensity(fPs_R, fPs_G, fPs_B);
	
	CDlgIntensity dlg;

	// 传递灰度分布密度数据给面板类
	for (i = 0; i <256; i++)
		dlg.m_fIntensity[i] = fPs_G[i];

	// 显示面板
	dlg.DoModal();
}

void CDIP_SystemView::OnViewBIntensity() 
{
	
	int i;

	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();
	if (pDoc->DIBImage() == NULL) return;

	// 各颜色分量的灰度分布密度
	float fPs_R[256];
	float fPs_G[256];
	float fPs_B[256];

	// 计算灰度分布密度
	CGrayProc clsGrayProc(pDoc->m_pDIB);
	clsGrayProc.GetIntensity(fPs_R, fPs_G, fPs_B);
	
	CDlgIntensity dlg;

	// 传递灰度分布密度数据给面板类
	for (i = 0; i <256; i++)
		dlg.m_fIntensity[i] = fPs_B[i];

	// 显示面板
	dlg.DoModal();
}

///////////////////////////////////////////
//	灰度折线变换						 //	
///////////////////////////////////////////
void CDIP_SystemView::OnPointStre() 
{
	
	int i;

	// 各颜色分量的灰度分布密度
	float fPs_Y[256];

	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 计算灰度分布密度
	CGrayProc clsGrayProc(pDoc->m_pDIB);
	clsGrayProc.GetIntensity(fPs_Y);
	
	// 创建对话框
	CDlgPointStre dlg;
	dlg.m_pDoc = pDoc;

	// 传递灰度分布密度数据给面板类
	for (i = 0; i <256; i++)
		dlg.m_fIntensity[i] = fPs_Y[i];

	// 显示对话框，由用户进行灰度折线变换
	dlg.DoModal();
}

///////////////////////////////////////////
//	灰度窗口变换						 //	
///////////////////////////////////////////
void CDIP_SystemView::OnPointWind() 
{
	
	int i;

	// 各颜色分量的灰度分布密度
	float fPs_Y[256];

	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 计算灰度分布密度
	CGrayProc clsGrayProc(pDoc->m_pDIB);
	clsGrayProc.GetIntensity(fPs_Y);
	
	// 创建对话框
	CDlgPointWin dlg;
	dlg.m_pDoc = pDoc;

	// 传递灰度分布密度数据给面板类
	for (i = 0; i <256; i++)
		dlg.m_fIntensity[i] = fPs_Y[i];

	// 显示对话框，由用户进行灰度折线变换
	dlg.DoModal();
}

///////////////////////////////////////////
//	灰度带通变换						 //	
///////////////////////////////////////////
void CDIP_SystemView::OnPointDt() 
{
	int i;

	// 各颜色分量的灰度分布密度
	float fPs_Y[256];

	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 计算灰度分布密度
	CGrayProc clsGrayProc(pDoc->m_pDIB);
	clsGrayProc.GetIntensity(fPs_Y);
	
	// 创建对话框
	CDlgPointDT dlg;
	dlg.m_pDoc = pDoc;

	// 传递灰度分布密度数据给面板类
	for (i = 0; i <256; i++)
		dlg.m_fIntensity[i] = fPs_Y[i];

	// 显示对话框，由用户进行灰度折线变换
	dlg.DoModal();
}

///////////////////////////////////////////
//	灰度带阻变换						 //	
///////////////////////////////////////////
void CDIP_SystemView::OnPointDz() 
{
	int i;

	// 各颜色分量的灰度分布密度
	float fPs_Y[256];

	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 计算灰度分布密度
	CGrayProc clsGrayProc(pDoc->m_pDIB);
	clsGrayProc.GetIntensity(fPs_Y);
	
	// 创建对话框
	CDlgPointDZ dlg;
	dlg.m_pDoc = pDoc;

	// 传递灰度分布密度数据给面板类
	for (i = 0; i <256; i++)
		dlg.m_fIntensity[i] = fPs_Y[i];

	// 显示对话框，由用户进行灰度折线变换
	dlg.DoModal();
}

///////////////////////////////////////////
//	规定灰度直方图分布					 //	
///////////////////////////////////////////
void CDIP_SystemView::OnSetIntensity() 
{
	AfxMessageBox(L"功能待添加！");
}

///////////////////////////////////////////
//	单映射规则进行直方图规定化			 //	
///////////////////////////////////////////
void CDIP_SystemView::OnPointSml() 
{

//--------调试代码 开始------------------------------------------
	int nu[64];
	float pu[64];

	float a=1.0f/(32.0f*63.0f);
	for(int i=0;i<64;i++)
	{
		nu[i]=i*4;
		pu[i]=a*i;
	}
//--------调试代码 结束------------------------------------------

	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 计算灰度分布密度
	CGrayProc clsGrayProc(pDoc->m_pDIB);
	clsGrayProc.PointSML(64,nu, pu);

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	组映射规则进行直方图规定化			 //	
///////////////////////////////////////////
void CDIP_SystemView::OnPointGml() 
{

//--------调试代码 开始------------------------------------------
	int nu[64];
	float pu[64];

	float a=1.0f/(32.0f*63.0f);
	for(int i=0;i<64;i++)
	{
		nu[i]=i*4;
		pu[i]=a*i;
	}
//--------调试代码 结束------------------------------------------

	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 计算灰度分布密度
	CGrayProc clsGrayProc(pDoc->m_pDIB);
	clsGrayProc.PointGML(64,nu, pu);

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对灰度进行动态范围压缩				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnDynamicCompress() 
{
	// 创建对话框
	CDynamicCompressParam dlg;
	
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	dlg.m_pDoc = pDoc;

	// 显示对话框，由用户进行尺度比例常数设定
	dlg.DoModal();
}

///////////////////////////////////////////
//	对灰度进行削波处理					 //	
///////////////////////////////////////////
void CDIP_SystemView::OnCutWave() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 对图象进行巴特沃斯低通滤波
	CGrayProc clsGrayProc(pDoc->m_pDIB);
	clsGrayProc.CutWave();

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////[图像分割部分]////////////////////////////////////////////

///////////////////////////////////////////
//	对图像实施区域生长处理				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnRgnGrow() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 操作导航
	if (pDoc->m_ptPoint_I == CPoint(0, 0))
	{
		MessageBox(L"请先双击图像以设置种子点一！", L"系统提示" , MB_ICONINFORMATION | MB_OK);
		return;
	}

	if (pDoc->m_ptPoint_II == CPoint(0, 0))
	{
		MessageBox(L"请先双击图像以设置种子点二！", L"系统提示" , MB_ICONINFORMATION | MB_OK);
		return;
	}

	// 调用区域生长函数对图象进行分割
	CSplitProc clsSplitProc(pDoc->m_pDIB);
	clsSplitProc.RgnGrow(m_clrSeed_I, m_clrSeed_II);
	
	// 清除种子点
	pDoc->m_ptPoint_I = CPoint(0, 0);
	pDoc->m_ptPoint_II = CPoint(0, 0);

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像进行轮廓提取					 //		
///////////////////////////////////////////
void CDIP_SystemView::OnContourdib() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 操作导航
	if (pDoc->m_ptPoint_I == CPoint(0, 0))
	{
		MessageBox(L"请先双击图像以设置种子点一！", L"系统提示" , MB_ICONINFORMATION | MB_OK);
		return;
	}

	if (pDoc->m_ptPoint_II == CPoint(0, 0))
	{
		MessageBox(L"请先双击图像以设置种子点二！", L"系统提示" , MB_ICONINFORMATION | MB_OK);
		return;
	}
	
	// 进行轮廓提取 
	CSplitProc clsSplitProc(pDoc->m_pDIB);
	clsSplitProc.ContourDIB(m_clrSeed_I, m_clrSeed_II);
	
	// 清除种子点
	pDoc->m_ptPoint_I = CPoint(0, 0);
	pDoc->m_ptPoint_II = CPoint(0, 0);

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像进行边界跟踪					 //		
///////////////////////////////////////////
void CDIP_SystemView::OnTracedib() 
{
	// 区域生长种子RGB值
	COLORREF clrSeed;

	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 操作导航
	if (pDoc->m_ptPoint_II == CPoint(0, 0))
	{
		MessageBox(L"请先双击目标以设置种子点！", L"系统提示" , MB_ICONINFORMATION | MB_OK);
		return;
	}

	// 获取目标的颜色值
	if (pDoc->m_ptPoint_I == CPoint(0, 0))
		clrSeed = m_clrSeed_II;
	else
		clrSeed = m_clrSeed_I;

	// 进行轮廓提取 
	CSplitProc clsSplitProc(pDoc->m_pDIB);
	clsSplitProc.TraceDIB(clrSeed);

	// 清除种子点
	pDoc->m_ptPoint_I = CPoint(0, 0);
	pDoc->m_ptPoint_II = CPoint(0, 0);

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像进行索贝尔算子的梯度滤波		 //	
///////////////////////////////////////////
void CDIP_SystemView::OnSobel() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 进行索贝尔算子梯度滤波
	CSplitProc clsSplitProc(pDoc->m_pDIB);
	clsSplitProc.Sobel();
	
	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像进行蒲瑞维特算子的梯度滤波	 //	
///////////////////////////////////////////
void CDIP_SystemView::OnPrewitt() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 进行索贝尔算子梯度滤波
	CSplitProc clsSplitProc(pDoc->m_pDIB);
	clsSplitProc.Prewitt();
	
	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像进行罗伯特交叉算子的梯度滤波	 //	
///////////////////////////////////////////
void CDIP_SystemView::OnRoberts() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 进行索贝尔算子梯度滤波
	CSplitProc clsSplitProc(pDoc->m_pDIB);
	clsSplitProc.Roberts();
	
	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像进行Kirsch算子的梯度滤波		 //	
///////////////////////////////////////////
void CDIP_SystemView::OnKrisch() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 进行索贝尔算子梯度滤波
	CSplitProc clsSplitProc(pDoc->m_pDIB);
	clsSplitProc.Kirsch();
	
	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像进行拉普拉斯算子的滤波		 //	
///////////////////////////////////////////
void CDIP_SystemView::OnLaplacian() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 进行索贝尔算子梯度滤波
	CSplitProc clsSplitProc(pDoc->m_pDIB);
	clsSplitProc.Laplacian();
	
	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像进行高斯——拉普拉斯算子的滤波 //	
///////////////////////////////////////////
void CDIP_SystemView::OnGuassLaplacian() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 进行索贝尔算子梯度滤波
	CSplitProc clsSplitProc(pDoc->m_pDIB);
	clsSplitProc.GuassLaplacian();
	
	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像进行哈夫变换以检测圆周		 //	
///////////////////////////////////////////
void CDIP_SystemView::OnHough() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 进行索贝尔算子梯度滤波
	CSplitProc clsSplitProc(pDoc->m_pDIB);
	clsSplitProc.Hough();
	
	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像进行阀值迭代的阀值化分割处理	 //	
///////////////////////////////////////////
void CDIP_SystemView::OnIterateThreshold() 
{
	// 各颜色分量的灰度分布密度
	int nNY[256];
	memset(nNY, 0, sizeof(nNY));

	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 计算灰度分布密度
	CGrayProc clsGrayProc(pDoc->m_pDIB);
	clsGrayProc.GetIntensity(nNY);

	// 根据阀值对图象进行阀值化分割处理
	CSplitProc clsSplitProc(pDoc->m_pDIB);
	clsSplitProc.Threshold(nNY);

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////[图像空域滤波部分]////////////////////////////////////////////

///////////////////////////////////////////
//	对图像实施浮雕化处理（共八种）		 //	
///////////////////////////////////////////
void CDIP_SystemView::OnEmboss_1() 
{
	CDIP_SystemDoc* pDoc = GetDocument();
	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();
	DoEmboss(EMBOSS_1);
}

void CDIP_SystemView::OnEmboss_2() 
{
	CDIP_SystemDoc* pDoc = GetDocument();
	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();
	DoEmboss(EMBOSS_2);
}

void CDIP_SystemView::OnEmboss_3() 
{
	CDIP_SystemDoc* pDoc = GetDocument();
	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();
	DoEmboss(EMBOSS_3);
}

void CDIP_SystemView::OnEmboss_4() 
{
	CDIP_SystemDoc* pDoc = GetDocument();
	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();
	DoEmboss(EMBOSS_4);
}

void CDIP_SystemView::OnEmboss_5() 
{
	CDIP_SystemDoc* pDoc = GetDocument();
	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();
	DoEmboss(EMBOSS_5);
}

void CDIP_SystemView::OnEmboss_6() 
{
	CDIP_SystemDoc* pDoc = GetDocument();
	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();
	DoEmboss(EMBOSS_6);
}

void CDIP_SystemView::OnEmboss_7() 
{
	CDIP_SystemDoc* pDoc = GetDocument();
	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();
	DoEmboss(EMBOSS_7);
}

void CDIP_SystemView::OnEmboss_8() 
{
	CDIP_SystemDoc* pDoc = GetDocument();
	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();
	DoEmboss(EMBOSS_8);
}

void CDIP_SystemView::DoEmboss(int nEmbossMode)
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 进行浮雕化处理
	CSpaceFilter clsSpaceFilter(pDoc->m_pDIB);
	clsSpaceFilter.MakeEmboss(nEmbossMode);

	// 更新DIB大小和调色板
	pDoc->InitDIBData();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);

	// 重新设置滚动视图大小
	SetScrollSizes(MM_TEXT, pDoc->GetDocSize());

	// 更新视图
	pDoc->UpdateAllViews(NULL);
}

///////////////////////////////////////////
//	对图像实施线形平滑滤波（G1）		 //	
///////////////////////////////////////////
void CDIP_SystemView::OnLineSmoothFilter() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 进行线形平滑滤波处理
	CSpaceFilter clsSpaceFilter(pDoc->m_pDIB);
	clsSpaceFilter.StencilFilter(&Line_Filter[0]);

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);

	// 更新视图
	Invalidate();
}

void CDIP_SystemView::OnLineSmoothFilter2() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 进行线形平滑滤波处理
	CSpaceFilter clsSpaceFilter(pDoc->m_pDIB);
	clsSpaceFilter.StencilFilter(&Line_Filter[1]);

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);

	// 更新视图
	Invalidate();
}

void CDIP_SystemView::OnLineSmoothFilter3() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 进行线形平滑滤波处理
	CSpaceFilter clsSpaceFilter(pDoc->m_pDIB);
	clsSpaceFilter.StencilFilter(&Line_Filter[2]);

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);

	// 更新视图
	Invalidate();
}

///////////////////////////////////////////
//	对图像实施非线形平滑滤波（G2）		 //	
///////////////////////////////////////////
void CDIP_SystemView::OnMedianFilter() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 进行线形平滑滤波处理
	CSpaceFilter clsSpaceFilter(pDoc->m_pDIB);
	clsSpaceFilter.MedianFilter();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);

	// 更新视图
	Invalidate();
}

///////////////////////////////////////////
//	对图像实施线形锐化滤波（G3）		 //	
///////////////////////////////////////////
void CDIP_SystemView::OnLineQuickFilter() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 进行线形平滑滤波处理
	CSpaceFilter clsSpaceFilter(pDoc->m_pDIB);
	clsSpaceFilter.StencilFilter(&Line_Filter[3]);

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);

	// 更新视图
	Invalidate();
}

///////////////////////////////////////////
//	对图像实施超限滤波					 //	
///////////////////////////////////////////
void CDIP_SystemView::OnOverrunFilter() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 进行线形平滑滤波处理
	CSpaceFilter clsSpaceFilter(pDoc->m_pDIB);
	clsSpaceFilter.OverRun(&OverRun_Filter, 10);

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);

	// 更新视图
	Invalidate();
}

///////////////////////////[正交变换处理]////////////////////////////////////////////

///////////////////////////////////////////
//	对图像实施傅立叶变换处理			 //	
///////////////////////////////////////////
void CDIP_SystemView::OnFreqFour() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();
	
	CFreqCalculate clsFreqCalculate(pDoc->m_pDIB);

	// 调用DIBFourier()函数进行付立叶变换
	if (clsFreqCalculate.DIBFourier())
	{
		// 标识为频谱图
		pDoc->m_bIsFrequency = TRUE;

		// 设置脏标记
		pDoc->SetModifiedFlag(TRUE);
		
		// 更新视图
		pDoc->UpdateAllViews(NULL);
	}
	else
	{
		
		MessageBox(L"分配内存失败！", L"系统提示" , MB_ICONINFORMATION | MB_OK);
	}
}

///////////////////////////////////////////
//	对图像实施离散余弦变换处理			 //	
///////////////////////////////////////////
void CDIP_SystemView::OnFreqDct() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();
	
	CFreqCalculate clsFreqCalculate(pDoc->m_pDIB);

	// 调用DIBFourier()函数进行付立叶变换
	if (clsFreqCalculate.DIBDCT())
	{
		// 设置脏标记
		pDoc->SetModifiedFlag(TRUE);
		
		// 更新视图
		pDoc->UpdateAllViews(NULL);	
	}
	else
	{
		
		MessageBox(L"分配内存失败！", L"系统提示" , MB_ICONINFORMATION | MB_OK);
	}
}

///////////////////////////////////////////
//	对图像实施沃尔什——哈达玛变换处理	 //	
///////////////////////////////////////////
void CDIP_SystemView::OnFreqWalh() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();
	
	CFreqCalculate clsFreqCalculate(pDoc->m_pDIB);

	// 调用DIBFourier()函数进行付立叶变换
	if (clsFreqCalculate.DIBWalsh())
	{
		// 设置脏标记
		pDoc->SetModifiedFlag(TRUE);
		
		// 更新视图
		pDoc->UpdateAllViews(NULL);
	}
	else
	{
		
		MessageBox(L"分配内存失败！", L"系统提示" , MB_ICONINFORMATION | MB_OK);
	}
}

///////////////////////////[图像频域滤波部分]////////////////////////////////////////////

///////////////////////////////////////////
//	对图像实施巴特沃斯低通滤波			 //	
///////////////////////////////////////////
void CDIP_SystemView::OnButterworthL() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 由对话框输入截止频率
	CDLGSetFreq dlg;
	dlg.m_fValue = 250.0f;
	if(dlg.DoModal() == IDOK)
	{
		// 更新数据
		UpdateData(TRUE);

		// 对图象进行巴特沃斯低通滤波
		CFreqFilter clsFreqFilter(pDoc->m_pDIB);
		clsFreqFilter.ButterworthL(dlg.m_fValue);

		// 重画视图
		Invalidate();

		// 设置脏标记
		pDoc->SetModifiedFlag(TRUE);
	}
}

///////////////////////////////////////////
//	对图像实施巴特沃斯高通滤波			 //	
///////////////////////////////////////////
void CDIP_SystemView::OnButterworthH() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 由对话框输入截止频率
	CDLGSetFreq dlg;
	dlg.m_fValue = 5.0f;
	if(dlg.DoModal() == IDOK)
	{
		// 更新数据
		UpdateData(TRUE);

		// 对图象进行巴特沃斯高通滤波
		CFreqFilter clsFreqFilter(pDoc->m_pDIB);
		clsFreqFilter.ButterworthH(dlg.m_fValue);

		// 重画视图
		Invalidate();

		// 设置脏标记
		pDoc->SetModifiedFlag(TRUE);
	}
}

///////////////////////////////////////////
//	对图像实施交互式带阻滤波			 //	
///////////////////////////////////////////
void CDIP_SystemView::OnMutualFilter() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

    if (pDoc->m_bIsFrequency == FALSE)
	{
		
		MessageBox(L"请先在频谱图指定滤波区域!", L"系统提示" , MB_ICONINFORMATION | MB_OK);
		return;
	}
	
	pDoc->SetModifiedFlag(FALSE);

	// 对图象进行交互式带阻滤波
	CFreqFilter clsFreqFilter(pDoc->m_pDIB);
	clsFreqFilter.MutualFilter(pDoc->m_rect, pDoc->m_nRgnNum);

	pDoc->m_nRgnNum = 0;

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像实施巴特沃斯低通滤波的逆滤波   //	
///////////////////////////////////////////
void CDIP_SystemView::OnRetrorseFilter() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 由对话框输入截止频率
	CDLGSetFreq dlg;
	dlg.m_fValue = 250.0f;
	if(dlg.DoModal() == IDOK)
	{
		// 更新数据
		UpdateData(TRUE);

		// 对图象进行巴特沃斯低通滤波
		CFreqFilter clsFreqFilter(pDoc->m_pDIB);
		clsFreqFilter.RetrorseFilter(dlg.m_fValue);

		// 重画视图
		Invalidate();

		// 设置脏标记
		pDoc->SetModifiedFlag(TRUE);
	}
}

///////////////////////////////////////////
//	对图像实施维纳滤波					 //	
///////////////////////////////////////////
void CDIP_SystemView::OnWienerFilter() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 由对话框输入截止频率
	CDLGSetFreq dlg;
	dlg.m_fValue = 250.0f;
	if(dlg.DoModal() == IDOK)
	{
		// 更新数据
		UpdateData(TRUE);

		// 对图象进行巴特沃斯低通滤波
		CFreqFilter clsFreqFilter(pDoc->m_pDIB);
		clsFreqFilter.WienerFilter(dlg.m_fValue);

		// 重画视图
		Invalidate();

		// 设置脏标记
		pDoc->SetModifiedFlag(TRUE);
	}
}

///////////////////////////////////////////
//	对图像实施功率谱均衡滤波			 //	
///////////////////////////////////////////
void CDIP_SystemView::OnPSE_Filter() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 由对话框输入截止频率
	CDLGSetFreq dlg;
	dlg.m_fValue = 250.0f;
	if(dlg.DoModal() == IDOK)
	{
		// 更新数据
		UpdateData(TRUE);

		// 对图象进行巴特沃斯低通滤波
		CFreqFilter clsFreqFilter(pDoc->m_pDIB);
		clsFreqFilter.PSE_Filter(dlg.m_fValue);

		// 重画视图
		Invalidate();

		// 设置脏标记
		pDoc->SetModifiedFlag(TRUE);
	}
}

///////////////////////////////////////////
//	对图像实施最小均方滤波				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnMinPower() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 由对话框输入截止频率
	CDLGSetFreq dlg;
	dlg.m_fValue = 250.0f;
	if(dlg.DoModal() == IDOK)
	{
		// 更新数据
		UpdateData(TRUE);

		// 对图象进行巴特沃斯低通滤波
		CFreqFilter clsFreqFilter(pDoc->m_pDIB);
		clsFreqFilter.MinPower(dlg.m_fValue);

		// 重画视图
		Invalidate();

		// 设置脏标记
		pDoc->SetModifiedFlag(TRUE);
	}
}

///////////////////////////////////////////
//	对空间失真图像进行矫正（从角度）	 //	
///////////////////////////////////////////
void CDIP_SystemView::OnRotate() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 由对话框输入旋转角度
	CDLGSetAngle dlg;
	dlg.m_fAngle = 0.0f;
	if(dlg.DoModal() == IDOK)
	{
		// 更新数据
		UpdateData(TRUE);

		// 对图象进行角度矫正
		CGeoTrans clsGeoTrans(pDoc->m_pDIB);
		clsGeoTrans.Rotate(dlg.m_fAngle);

		// 重画视图
		Invalidate();

		// 设置脏标记
		pDoc->SetModifiedFlag(TRUE);
	}
}

///////////////////////////////////////////
//	对空间失真图像进行校直（从空间角度） //	
///////////////////////////////////////////
void CDIP_SystemView::OnRectinication() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 由对话框输入旋转角度
	CDLGSetAngle dlg;
	dlg.m_fAngle = 0.0f;
	if(dlg.DoModal() == IDOK)
	{
		// 更新数据
		UpdateData(TRUE);

		// 对图象进行角度矫正
		CGeoTrans clsGeoTrans(pDoc->m_pDIB);
		clsGeoTrans.Rectinication(dlg.m_fAngle);

		// 重画视图
		Invalidate();

		// 设置脏标记
		pDoc->SetModifiedFlag(TRUE);
	}
}

///////////////////////////////////////////
//	向图像添加随机噪声					 //	
///////////////////////////////////////////
void CDIP_SystemView::OnRandomNoise() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 对图象进行巴特沃斯低通滤波
	CNoise clsNoise(pDoc->m_pDIB);
	clsNoise.RandomNoise();

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	向图象添加盐性噪声					 //	
///////////////////////////////////////////
void CDIP_SystemView::OnSaltNoise() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 对图象进行巴特沃斯低通滤波
	CNoise clsNoise(pDoc->m_pDIB);
	clsNoise.SaltNoise();

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////[数学形态学部分]//////////////////////////////////////////

///////////////////////////////////////////
//	对图像实施水平腐蚀处理				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnHErosion() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 对图象进行巴特沃斯低通滤波
	CMorph clsMorph(pDoc->m_pDIB);
	clsMorph.HErosion();

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像实施垂直腐蚀处理				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnVErosion() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 对图象进行巴特沃斯低通滤波
	CMorph clsMorph(pDoc->m_pDIB);
	clsMorph.VErosion();

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像实施垂直膨胀处理				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnHDilation() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 对图象进行巴特沃斯低通滤波
	CMorph clsMorph(pDoc->m_pDIB);
	clsMorph.HDilation();

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像实施垂直膨胀处理				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnVDilation() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 对图象进行巴特沃斯低通滤波
	CMorph clsMorph(pDoc->m_pDIB);
	clsMorph.VDilation();

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像实施全方向腐蚀				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnVhErosion() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 对图象进行巴特沃斯低通滤波
	CMorph clsMorph(pDoc->m_pDIB);
	clsMorph.VHErosion();

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像实施全方向膨胀				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnVhDilation() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 对图象进行巴特沃斯低通滤波
	CMorph clsMorph(pDoc->m_pDIB);
	clsMorph.VHDilation();

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像实施开运算处理				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnOpenOperate() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 对图象进行巴特沃斯低通滤波
	CMorph clsMorph(pDoc->m_pDIB);
	clsMorph.OpenOperate();

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像实施闭运算处理				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnCloseOperate() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 对图象进行巴特沃斯低通滤波
	CMorph clsMorph(pDoc->m_pDIB);
	clsMorph.CloseOperate();

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像实施细化处理					 //	
///////////////////////////////////////////
void CDIP_SystemView::OnThin() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 对图象进行巴特沃斯低通滤波
	CMorph clsMorph(pDoc->m_pDIB);
	clsMorph.Thin();

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像实施粗化处理					 //	
///////////////////////////////////////////
void CDIP_SystemView::OnThick() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 对图象进行巴特沃斯低通滤波
	CMorph clsMorph(pDoc->m_pDIB);
	clsMorph.Thick();

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	对图像实施中轴变换					 //	
///////////////////////////////////////////
void CDIP_SystemView::OnMat() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 对图象进行巴特沃斯低通滤波
	CMorph clsMorph(pDoc->m_pDIB);
	clsMorph.MAT();

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////[图像编码部分]////////////////////////////////////////////

///////////////////////////////////////////
//	对图像实施JPEG编码压缩				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnJpegEncoding() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();
	
	if (pDoc->m_strCurrentFile == "")
	{
		AfxMessageBox(L"请先打开BMP位图文件再行压缩!");
		return;
	}
	if (pDoc->m_strCurrentFile.Right(4) != ".bmp" || pDoc->m_strCurrentFile.Right(4) != ".BMP")
	{
		AfxMessageBox(L"请确认当前图像的格式为BMP位图!");
		return;
	}
	if (pDoc->IsModified())
	{
		AfxMessageBox(L"请先保存原始BMP位图文件再行压缩!");
		return;
	}

	// 选择保存文件
    CFileDialog fileDlg(FALSE, L"*.JPG", L"*.JPG", NULL, L"JPEG 文件(*.JPG)|*.JPG||", this);
	fileDlg.m_ofn.Flags |= OFN_FILEMUSTEXIST;
	fileDlg.m_ofn.lpstrTitle = L"文件编码为...";

	// 保存压缩图象
	if (fileDlg.DoModal() == IDOK)
	{
		CCoding clsCoding(pDoc->m_pDIB);
		clsCoding.SaveJPG(pDoc, fileDlg.GetPathName());
	}            
}

///////////////////////////////////////////
//	对图像实施JPEG编码解压缩			 //	
///////////////////////////////////////////
void CDIP_SystemView::OnJpegDecoding() 
{
	// 选择打开文件
	CFileDialog fileDlg(TRUE, L"*.JPG", L"*.JPG", NULL, L"JPEG 文件(*.JPG)|*.JPG||", this);
	fileDlg.m_ofn.Flags|=OFN_FILEMUSTEXIST;
	fileDlg.m_ofn.lpstrTitle = L"请选择JPG文件...";

	if (fileDlg.DoModal() == IDOK) 
	{
		CDIP_SystemDoc* pDoc = GetDocument();
		if(pDoc->DIBImage() != NULL) // 新建空白文档
			pDoc = pDoc->CreateNewDocument();

		// 解压图象
		CCoding clsCoding;
		clsCoding.LoadJPG(pDoc, fileDlg.GetPathName());

		// 重画视图
		Invalidate();
	}
}

///////////////////////////////////////////
//	对图像实施PCX编码压缩				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnPCXEncoding() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();
	
	// 选择保存文件
    CFileDialog fileDlg(FALSE, L"*.PCX", L"*.PCX", NULL, L"PCX 文件(*.PCX)|*.PCX||", this);
	fileDlg.m_ofn.Flags |= OFN_FILEMUSTEXIST;
	fileDlg.m_ofn.lpstrTitle = L"文件编码为...";

	// 保存压缩图象
	if (fileDlg.DoModal() == IDOK)
	{
		CCoding clsCoding(pDoc->m_pDIB);
		clsCoding.DIBToPCX(fileDlg.GetPathName());
	}            
}

///////////////////////////////////////////
//	对图像实施PCX编码解压缩				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnPCXDecoding() 
{
	// 创建新DIB
	HDIB hNewDIB = NULL;
	
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	// 选择打开文件
	CFileDialog fileDlg(TRUE, L"*.PCX", L"*.PCX", NULL, L"PCX 文件(*.PCX)|*.PCX||", this);
	fileDlg.m_ofn.Flags|=OFN_FILEMUSTEXIST;
	fileDlg.m_ofn.lpstrTitle = L"请选择PCX文件...";

	if (fileDlg.DoModal() == IDOK) 
	{
		// 获取文档
		CDIP_SystemDoc* pDoc = GetDocument();
		if (pDoc->DIBImage() != NULL)
			pDoc = pDoc->CreateNewDocument();
		// 初始化指针
		pDoc->m_pDIB->m_pImage = new CImage;
		pDoc->m_pDIB->m_UndoStack.Push(pDoc->m_pDIB->m_pImage);

		// 解压图象
		CCoding clsCoding(pDoc->m_pDIB);
		hNewDIB = clsCoding.PCXToDIB(fileDlg.GetPathName());

		// 判断处理是否成功
		if (hNewDIB != NULL)
		{
			// 更新DIB大小和调色板
			pDoc->InitDIBData();
		
			// 设置脏标记
			pDoc->SetModifiedFlag(TRUE);
		
			// 重新设置滚动视图大小
			SetScrollSizes(MM_TEXT, pDoc->GetDocSize());

			// 更新视图
			pDoc->UpdateAllViews(NULL);
		}
		else
		{
			
			MessageBox(L"分配内存失败！", L"系统提示" , MB_ICONINFORMATION | MB_OK);
		}
	}
}

///////////////////////////////////////////
//	对图像实施哈夫曼编码				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnHuffman() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	AfxMessageBox(L"功能待添加！"); return;
	
	// 选择保存文件
    CFileDialog fileDlg(FALSE, L"*.HUF", L"*.HUF", NULL, L"HUFFMAN 文件(*.HUF)|*.HUF||", this);
	fileDlg.m_ofn.Flags |= OFN_FILEMUSTEXIST;
	fileDlg.m_ofn.lpstrTitle = L"文件编码为...";

	// 保存压缩图象
	if (fileDlg.DoModal() == IDOK)
	{
	}            
}

///////////////////////////////////////////
//	对图像实施仙农-弗诺编码				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnShannonFannon() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();
	
	AfxMessageBox(L"功能待添加！"); return;

	// 选择保存文件
    CFileDialog fileDlg(FALSE, L"*.FAN", L"*.FAN", NULL, L"Shannon-Fannon 文件(*.FAN)|*.FAN||", this);
	fileDlg.m_ofn.Flags |= OFN_FILEMUSTEXIST;
	fileDlg.m_ofn.lpstrTitle = L"文件编码为...";

	// 保存压缩图象
	if (fileDlg.DoModal() == IDOK)
	{
	}            
}

///////////////////////////////////////////
//	对图像实施GIF编码压缩				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnLzwEncoding() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();
	
	// 选择保存文件
    CFileDialog fileDlg(FALSE, L"*.GIF", L"*.GIF", NULL, L"GIF 文件(*.GIF)|*.GIF||", this);
	fileDlg.m_ofn.Flags |= OFN_FILEMUSTEXIST;
	fileDlg.m_ofn.lpstrTitle = L"文件编码为...";

	// 保存压缩图象
	if (fileDlg.DoModal() == IDOK)
	{
		if (!pDoc->CreateTempFile())
		{
			AfxMessageBox(L"创建缓存文件出错！");
			return;
		}
		CFile file;
		file.Open(fileDlg.GetPathName(), CFile::modeCreate|CFile::modeReadWrite);
		CCoding clsCoding;
		clsCoding.DIBToGIF(pDoc->GetHDIB(), file, TRUE);
		file.Close();
	}            
}

///////////////////////////////////////////
//	对图像实施GIF编码解压缩				 //	
///////////////////////////////////////////
void CDIP_SystemView::OnLzwDecoding() 
{
	// 创建新DIB
	HDIB hNewDIB = NULL;
	
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	// 选择保存文件
    CFileDialog fileDlg(TRUE, L"*.GIF", L"*.GIF", NULL, L"GIF 文件(*.GIF)|*.GIF||", this);
	fileDlg.m_ofn.Flags |= OFN_FILEMUSTEXIST;
	fileDlg.m_ofn.lpstrTitle = L"请选择GIF文件...";

	// 保存压缩图象
	if (fileDlg.DoModal() == IDOK)
	{
		// 调用ZoomDIB()函数获取缩小后的DIB
		CCoding clsCoding(pDoc->m_pDIB);
		CFile file;
		file.Open(fileDlg.GetPathName(),CFile::modeReadWrite);
		hNewDIB = clsCoding.GIFToDIB(file);
		file.Close();

		// 判断缩放是否成功
		if (hNewDIB != NULL)
		{		
			// 替换DIB，同时释放旧DIB对象
			pDoc->ReplaceHDIB(hNewDIB);
	
			// 更新DIB大小和调色板
			pDoc->InitDIBData();
		
			pDoc->HDIB2CImage(hNewDIB);

			// 重新设置滚动视图大小
			SetScrollSizes(MM_TEXT, pDoc->GetDocSize());

			pDoc->SetModifiedFlag(TRUE);

			// 更新视图
			pDoc->UpdateAllViews(NULL);
		}
		else
		{
			
			MessageBox(L"分配内存失败！", L"系统提示" , MB_ICONINFORMATION | MB_OK);
		}
	}
}

///////////////////////////////////////////
//	嵌入水印							 //	
///////////////////////////////////////////
void CDIP_SystemView::OnSetWatermarking() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 保存压缩图象
	CWaterMarking clsWaterMarking(pDoc->m_pDIB);
	clsWaterMarking.Set();
}

///////////////////////////////////////////
//	检测水印							 //	
///////////////////////////////////////////
void CDIP_SystemView::OnGetWatermarking() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();
	
	// 选择保存文件
    CFileDialog fileDlg(FALSE, L"*.BMP", L"*.BMP", NULL, L"水印文件(*.BMP)|*.BMP||", this);
	fileDlg.m_ofn.Flags |= OFN_FILEMUSTEXIST;
	fileDlg.m_ofn.lpstrTitle = L"文件保存为...";

	// 保存压缩图象
	if (fileDlg.DoModal() == IDOK)
	{
		CFile file;
		file.Open(fileDlg.GetPathName(), CFile::modeCreate | CFile::modeReadWrite);
		CWaterMarking clsWaterMarking(pDoc->m_pDIB);
		clsWaterMarking.Get(file);
		file.Close();
	}            
}

///////////////////////////////////////////
//	小波分解							 //	
///////////////////////////////////////////
void CDIP_SystemView::OnDwt() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 对图象进行巴特沃斯低通滤波
	CFreqCalculate clsFreqCalculate(pDoc->m_pDIB);
	clsFreqCalculate.DIBDWT();

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	小波重构							 //	
///////////////////////////////////////////
void CDIP_SystemView::OnIdwt() 
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	// 对图象进行巴特沃斯低通滤波
	CFreqCalculate clsFreqCalculate(pDoc->m_pDIB);
	clsFreqCalculate.DIBIDWT();

	// 重画视图
	Invalidate();

	// 设置脏标记
	pDoc->SetModifiedFlag(TRUE);
}

///////////////////////////////////////////
//	呼出高级图像处理对话框				 //	
///////////////////////////////////////////

// 呼出"伸缩变换"对话框
void CDIP_SystemView::OnCallZoomDlg()
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();
	CImage* pImage = pDoc->DIBImage();
	if (pImage == NULL) return;

	pDoc->m_pDIB->PushCopy();

	m_pZoomDlg->m_nWidthNew = m_pZoomDlg->m_dWidthEdit = pImage->GetWidth();
	m_pZoomDlg->m_nHeightNew = m_pZoomDlg->m_dHeightEdit = pImage->GetHeight();
	m_pZoomDlg->ShowWindow(SW_SHOW);
	pDoc->m_sizeDoc.cx = pImage->GetWidth();
	pDoc->m_sizeDoc.cy = pImage->GetHeight();
	// 设置脏标记
	if (m_pZoomDlg->m_bModified)
		pDoc->SetModifiedFlag(TRUE);
}

// 呼出"色阶"对话框
void CDIP_SystemView::OnCallColorgradDlg()
{
	CColorGradDlg Dlg(this);
	Dlg.DoModal();
}

// 呼出"曲线"对话框
void CDIP_SystemView::OnCallCurveDlg()
{
	CCurveDlg Dlg;
	Dlg.DoModal();
}

// 呼出"直方图"对话框
void CDIP_SystemView::OnCallHistogramDlg()
{
	CHECK_IMAGE_NOT_NULL;
	CHistogramDlg Dlg;
	Dlg.DoModal();
}

// 呼出"空间滤波"对话框
void CDIP_SystemView::OnCallSpacefilterDlg()
{
	CHECK_IMAGE_NOT_NULL;
	CSpaceFilterDlg Dlg;
	Dlg.DoModal();
}

// 显示缓存文件
void CDIP_SystemView::OnTempFile()
{
	m_bShowImage = !m_bShowImage;
	Invalidate();
}

void CDIP_SystemView::OnUpdateTempFile(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->GetHDIB() != NULL);
	pCmdUI->SetCheck(!m_bShowImage);
}

// 查看加强版直方图
void CDIP_SystemView::OnHistogramIi()
{
	CHECK_IMAGE_NOT_NULL;
	CEnhancedHistDlg dlg;
	dlg.DoModal();
}

// 呼出"灰度拉伸"对话框
void CDIP_SystemView::OnCallStretchDlg()
{
	CHECK_IMAGE_NOT_NULL;
	CStretchDlg dlg;
	dlg.DoModal();
}

// 呼出"阈值变换"对话框
void CDIP_SystemView::OnCallThresholdDlg()
{
	CHECK_IMAGE_NOT_NULL;
	CThresholdDlg dlg;
	dlg.DoModal();
}

void CDIP_SystemView::ResetView()
{
	CDIP_SystemDoc* pDoc = GetDocument();
	m_PaintRect.right = pDoc->m_pDIB->Width();
	m_PaintRect.bottom = pDoc->m_pDIB->Height();
	m_bShowImage = ID_VIEW_DEFAULT;
	SetScrollSizes(MM_TEXT, CSize(m_PaintRect.right, m_PaintRect.bottom));
}

// 撤销
void CDIP_SystemView::OnEditUndo()
{
	CDIP_SystemDoc* pDoc = GetDocument();
	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->Undo();
	ResetView();
	Invalidate();
}

// 恢复
void CDIP_SystemView::OnEditRedo()
{
	CDIP_SystemDoc* pDoc = GetDocument();
	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->Redo();
	ResetView();
	Invalidate();
}

void CDIP_SystemView::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->m_pDIB->m_UndoStack.m_nSizeOfStack > 1);
}

void CDIP_SystemView::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->m_pDIB->m_RedoStack.IsEmpty());
}

// 放大视图
void CDIP_SystemView::OnViewZoomin()
{
	m_nViewType = ID_VIEW_ZOOMIN;
	m_PaintRect.right = m_PaintRect.right * 2.0;
	m_PaintRect.bottom = m_PaintRect.bottom * 2.0;

	CDIP_SystemDoc *pDoc = GetDocument();
	if (pDoc->DIBImage() == NULL) return;
	if (m_PaintRect.right > VIEW_MAX_WIDTH)
	{
		m_PaintRect.right = VIEW_MAX_WIDTH;
		float rate = 1.0f * VIEW_MAX_WIDTH / pDoc->m_pDIB->Width();
		m_PaintRect.bottom = int(pDoc->m_pDIB->Height() * rate);
	}
	pDoc->SetDocSize(m_PaintRect.right, m_PaintRect.bottom);

	// 滚到原点
	ScrollToPosition(POINT());
	Invalidate(TRUE);
}

// 缩小视图
void CDIP_SystemView::OnViewZoomout()
{
	m_nViewType = ID_VIEW_ZOOMOUT;
	m_PaintRect.right = m_PaintRect.right / 2.0;
	m_PaintRect.bottom = m_PaintRect.bottom / 2.0;

	CDIP_SystemDoc *pDoc = GetDocument();
	if (pDoc->DIBImage() == NULL) return;
	if (m_PaintRect.right < VIEW_MIN_WIDTH)
	{
		m_PaintRect.right = VIEW_MIN_WIDTH;
		float rate = 1.0f * VIEW_MIN_WIDTH / pDoc->m_pDIB->Width();
		m_PaintRect.bottom = int(pDoc->m_pDIB->Height() * rate);
	}
	pDoc->SetDocSize(m_PaintRect.right, m_PaintRect.bottom);

	// 滚到原点
	ScrollToPosition(POINT());
	Invalidate(TRUE);
}

// 适应窗口
void CDIP_SystemView::OnViewFitWindow()
{
	m_nViewType = ID_VIEW_FIT_WINDOW;
	Invalidate(TRUE);
}

// 调整到屏幕大小
void CDIP_SystemView::OnViewScreenSize()
{
	m_nViewType = ID_VIEW_SCREEN_SIZE;
	m_PaintRect.right = GetSystemMetrics(SM_CXSCREEN); //屏幕宽度
	m_PaintRect.bottom = GetSystemMetrics(SM_CYSCREEN); //屏幕高度
	GetDocument()->SetDocSize(m_PaintRect.right, m_PaintRect.bottom);
	Invalidate(TRUE);
}

// 采用默认的视图
void CDIP_SystemView::OnViewDefault()
{
	m_nViewType = ID_VIEW_DEFAULT;
	CDIP_SystemDoc* pDoc = GetDocument();
	if (pDoc->DIBImage() == NULL) return;
	m_PaintRect.right = pDoc->m_pDIB->Width();
	m_PaintRect.bottom = pDoc->m_pDIB->Height();
	pDoc->SetDocSize(m_PaintRect.right, m_PaintRect.bottom);
	Invalidate(TRUE);
}

void CDIP_SystemView::OnUpdateViewZoomin(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nViewType == ID_VIEW_ZOOMIN);
}

void CDIP_SystemView::OnUpdateViewZoomout(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nViewType == ID_VIEW_ZOOMOUT);
}


void CDIP_SystemView::OnUpdateViewScreenSize(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nViewType == ID_VIEW_SCREEN_SIZE);
}


void CDIP_SystemView::OnUpdateViewFitWindow(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nViewType == ID_VIEW_FIT_WINDOW);
}


void CDIP_SystemView::OnUpdateViewDefault(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nViewType == ID_VIEW_DEFAULT);
}


// 呼出"位深度"对话框
void CDIP_SystemView::OnCallBppDlg()
{
	CDIP_SystemDoc* pDoc = GetDocument();
	if (pDoc->DIBImage() == NULL) return;
	CChangeBppDlg dlg;
	dlg.DoModal();
}

// 呼出"旋转"对话框
void CDIP_SystemView::OnCallRotateDlg()
{
	CHECK_IMAGE_NOT_NULL;
	CRotateDlg dlg;
	dlg.DoModal();
}

// 垂直翻转
void CDIP_SystemView::OnFlipVertical()
{
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	int bpp = pDoc->m_pDIB->BitCount(), Sel = IDYES;
	if (bpp != 32)
	{
		Sel = MessageBox(L"当前图像将被转为32位色，你是否希望这样？", L"警告", MB_ICONWARNING | MB_YESNO);
		pDoc->m_pDIB->ChangeBPP(32);
	}

	UINT nWidth = pDoc->m_pDIB->Width();
	UINT nHeight = pDoc->m_pDIB->Height();
	BYTE* pSrc = pDoc->m_pDIB->FindAddress();
	BYTE* temp = NULL;
	
	VMirrorTrans(pSrc, temp, nWidth, nHeight);
	pDoc->m_pDIB->SetBits(temp, 4, nWidth, nHeight);

	if (Sel != IDYES)
	{
		pDoc->m_pDIB->ChangeBPP(bpp);
	}

	SAFE_DELETE(temp);

	pDoc->SetDocSize();
	Invalidate();
}

// 水平翻转
void CDIP_SystemView::OnFlipHorizontal()
{
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	int bpp = pDoc->m_pDIB->BitCount(), Sel = IDYES;
	if (bpp != 32)
	{
		Sel = MessageBox(L"当前图像将被转为32位色，你是否希望这样？", L"警告", MB_ICONWARNING | MB_YESNO);
		pDoc->m_pDIB->ChangeBPP(32);
	}

	UINT nWidth = pDoc->m_pDIB->Width();
	UINT nHeight = pDoc->m_pDIB->Height();
	BYTE* pSrc = pDoc->m_pDIB->FindAddress();
	BYTE* temp = NULL;

	HMirrorTrans(pSrc, temp, nWidth, nHeight);
	pDoc->m_pDIB->SetBits(temp, 4, nWidth, nHeight);

	if (Sel != IDYES)
	{
		pDoc->m_pDIB->ChangeBPP(bpp);
	}

	SAFE_DELETE(temp);

	pDoc->SetDocSize();
	Invalidate();
}

// 转置图像
void CDIP_SystemView::OnTranspose()
{
	CDIP_SystemDoc* pDoc = GetDocument();

	if (pDoc->DIBImage() == NULL) return;
	pDoc->m_pDIB->PushCopy();

	int bpp = pDoc->m_pDIB->BitCount(), Sel = IDYES;
	if (bpp != 32)
	{
		Sel = MessageBox(L"当前图像将被转为32位色，你是否希望这样？", L"警告", MB_ICONWARNING | MB_YESNO);
		pDoc->m_pDIB->ChangeBPP(32);
	}

	UINT nWidth = pDoc->m_pDIB->Width();
	UINT nHeight = pDoc->m_pDIB->Height();
	UINT nTempWidth = nWidth;
	UINT nTempHeight = nHeight;

	BYTE* pSrc = pDoc->m_pDIB->FindAddress();
	BYTE* temp = NULL;

	Transpose(pSrc, temp, nWidth, nHeight, nTempWidth, nTempHeight);

	pDoc->m_pDIB->SetBits(temp, 4, nTempWidth, nTempHeight);

	if (Sel != IDYES)
	{
		pDoc->m_pDIB->ChangeBPP(bpp);
	}

	SAFE_DELETE(temp);

	pDoc->SetDocSize();
	Invalidate();
}


LRESULT CDIP_SystemView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_MOUSEMOVE && m_bGetCurPixel)
	{
		MSG msg;
		msg.hwnd = m_hWnd;
		msg.message = message;
		msg.wParam = wParam;
		msg.lParam = lParam;

		m_ToolTip.RelayEvent(&msg);
	}
	return CScrollView::WindowProc(message, wParam, lParam);
}


void CDIP_SystemView::OnGetCurrentPixel()
{
	m_bGetCurPixel = !m_bGetCurPixel;
}


void CDIP_SystemView::OnUpdateGetCurrentPixel(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bGetCurPixel);
}

void CDIP_SystemView::NegativeImage(int channel)
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();
	CDIB* pDIB = pDoc->m_pDIB;
	int nChannel = pDIB->Channel();
	// 非彩色图像不处理
	if (nChannel < 3)
		return;
	long nWidth = pDIB->Width();
	long nHeight = pDIB->Height();
	long nRowLen = pDIB->RowLen();
	CImage* pImage = new CImage;
	pImage->Create(nWidth, nHeight, nChannel * 8, 0);
	BYTE* pDst = (BYTE*)pImage->GetBits() + (pImage->GetPitch()*(nHeight - 1));
	BYTE* pSrc = pDIB->FindAddress();
	memcpy(pDst, pSrc, nHeight * nRowLen);
	for (int i = 0; i < nHeight; ++i)
	{
		for (int j = 0; j < nWidth; ++j)
		{
			*(pDst + j * nChannel + 2 - channel + i * nRowLen) = 255 - *(pSrc + j * nChannel + 2 - channel + i * nRowLen);
		}
	}
	pDIB->Clear();
	pDIB->m_UndoStack.SetTop(pImage);
	pDIB->SetImage(pImage);
	Invalidate();
	return;
}

// 查看红色通道负片图像
void CDIP_SystemView::OnViewRChannelImage()
{
	CHECK_IMAGE_NOT_NULL;
	m_bRNegativeImage = !m_bRNegativeImage;
	NegativeImage(0);
}

// 查看绿色通道负片图像
void CDIP_SystemView::OnViewGChannelImage()
{
	CHECK_IMAGE_NOT_NULL;
	m_bGNegativeImage = !m_bGNegativeImage;
	NegativeImage(1);
}

// 查看蓝色通道负片图像
void CDIP_SystemView::OnViewBChannelImage()
{
	CHECK_IMAGE_NOT_NULL;
	m_bBNegativeImage = !m_bBNegativeImage;
	NegativeImage(2);
}

// 查看所有通道负片
void CDIP_SystemView::OnViewAllChannelImage()
{
	CHECK_IMAGE_NOT_NULL;
	if (!m_bRNegativeImage)
	{
		m_bRNegativeImage = true;
		NegativeImage(0);
	}
	if (!m_bGNegativeImage)
	{
		m_bGNegativeImage = true;
		NegativeImage(1);
	}
	if (!m_bBNegativeImage)
	{
		m_bBNegativeImage = true;
		NegativeImage(2);
	}
}

void CDIP_SystemView::OnUpdateViewRChannelImage(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bRNegativeImage);
}


void CDIP_SystemView::OnUpdateViewGChannelImage(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bGNegativeImage);
}


void CDIP_SystemView::OnUpdateViewBChannelImage(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bBNegativeImage);
}


void CDIP_SystemView::OnUpdateViewAllChannelImage(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bRNegativeImage && m_bGNegativeImage && m_bBNegativeImage);
}

// R-2, G-1, B-0.转换后RGB对应的通道
void CDIP_SystemView::ConvertImage(int R, int G, int B)
{
	// 获取文档
	CDIP_SystemDoc* pDoc = GetDocument();
	if (pDoc->DIBImage() == NULL) return;
	CDIB* pDIB = pDoc->m_pDIB;
	int nChannel = pDIB->Channel();
	// 非彩色图像不处理
	if (nChannel < 3)
		return;
	long nWidth = pDIB->Width();
	long nHeight = pDIB->Height();
	long nRowLen = pDIB->RowLen();
	CImage* pImage = new CImage;
	pImage->Create(nWidth, nHeight, nChannel * 8, 0);
	BYTE* pDst = (BYTE*)pImage->GetBits() + (pImage->GetPitch()*(nHeight - 1));
	BYTE* pSrc = pDIB->FindAddress();
	memcpy(pDst, pSrc, nHeight * nRowLen);
	for (int i = 0; i < nHeight; ++i)
	{
		for (int j = 0; j < nWidth; ++j)
		{
			*(pDst + j * nChannel + 2 + i * nRowLen) = *(pSrc + j * nChannel + R + i * nRowLen);
			*(pDst + j * nChannel + 1 + i * nRowLen) = *(pSrc + j * nChannel + G + i * nRowLen);
			*(pDst + j * nChannel     + i * nRowLen) = *(pSrc + j * nChannel + B + i * nRowLen);
		}
	}
	pDIB->Clear();
	pDIB->m_UndoStack.SetTop(pImage);
	pDIB->SetImage(pImage);
	Invalidate();
	return;
}

void CDIP_SystemView::OnConvertRgb2Rbg()
{
	ConvertImage(2, 0, 1);
}


void CDIP_SystemView::OnConvertRgb2Brg()
{
	ConvertImage(0, 2, 1);
}


void CDIP_SystemView::OnConvertRgb2Bgr()
{
	ConvertImage(0, 1, 2);
}


void CDIP_SystemView::OnConvertRgb2Grb()
{
	ConvertImage(1, 2, 0);
}


void CDIP_SystemView::OnConvertRgb2Gbr()
{
	ConvertImage(1, 0, 2);
}

// *** 以下是关于快捷键 ***

int CDIP_SystemView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	// [2] 加载快捷键资源
	m_hHotKey = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR));
	return 0;
}


BOOL CDIP_SystemView::PreTranslateMessage(MSG* pMsg)
{
	// [3] 翻译快捷键消息
	if(::TranslateAccelerator(GetSafeHwnd(), m_hHotKey, pMsg))   
		return true;

	return CScrollView::PreTranslateMessage(pMsg);
}

// 伸缩变换
void CDIP_SystemView::OnHotkeyAltZ()
{
	OnCallZoomDlg();
}

// 默认视图
void CDIP_SystemView::OnHotkeyCtrl0()
{
	OnViewDefault();
}

// 复制
void CDIP_SystemView::OnHotkeyCtrlC()
{
	OnEditCopy();
}

// 空间滤波
void CDIP_SystemView::OnHotkeyCtrlF()
{
	OnCallSpacefilterDlg();
}

// 直方图
void CDIP_SystemView::OnHotkeyCtrlH()
{
	OnCallHistogramDlg();
}

// 色阶
void CDIP_SystemView::OnHotkeyCtrlL()
{
	OnCallColorgradDlg();
}

// 曲线
void CDIP_SystemView::OnHotkeyCtrlM()
{
	OnCallCurveDlg();
}

// 新建
void CDIP_SystemView::OnHotkeyCtrlN()
{
	CDIP_SystemApp* pMyApp = (CDIP_SystemApp*)AfxGetApp();
	pMyApp->OnFileMynew();
}

// 打开
void CDIP_SystemView::OnHotkeyCtrlO()
{
	CDIP_SystemApp* pMyApp = (CDIP_SystemApp*)AfxGetApp();
	pMyApp->OnFileMyopen();
}

// 打印
void CDIP_SystemView::OnHotkeyCtrlP()
{
	OnFilePrint();
}

// 退出
void CDIP_SystemView::OnHotkeyCtrlQ()
{
	AfxGetMainWnd()->SendMessage(WM_CLOSE);
}

// 重做
void CDIP_SystemView::OnHotkeyCtrlR()
{
	OnEditRedo();
}

// 保存
void CDIP_SystemView::OnHotkeyCtrlS()
{
	CDIP_SystemDoc *pDoc = GetDocument();
	pDoc->OnSaveDocument(pDoc->m_strCurrentFile);
}

// 黏贴
void CDIP_SystemView::OnHotkeyCtrlV()
{
	OnEditPaste();
}

// 关闭
void CDIP_SystemView::OnHotkeyCtrlW()
{
	CDIP_SystemDoc *pDoc = GetDocument();
	pDoc->OnCloseDocument();
}

// 撤销
void CDIP_SystemView::OnHotkeyCtrlZ()
{
	OnEditUndo();
}

// 放大视图
void CDIP_SystemView::OnHotkeyZoomIn()
{
	OnViewZoomin();
}

// 缩小视图
void CDIP_SystemView::OnHotkeyZoomOut()
{
	OnViewZoomout();
}