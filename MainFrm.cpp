// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "DIP_System.h"

#include "MainFrm.h"
#include "DIP_SystemDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_HOTKEY_CTRL_N, &CMainFrame::OnHotkeyCtrlN)
	ON_COMMAND(ID_HOTKEY_CTRL_O, &CMainFrame::OnHotkeyCtrlO)
	ON_COMMAND(ID_HOTKEY_CTRL_Q, &CMainFrame::OnHotkeyCtrlQ)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar.\n");
		return -1;      // fail to create
	}

	if (!m_MyToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
	{
		TRACE0("Failed to create my toolbar.\n");
		return -1;	    // fail to create
	}
	// 在ImageList中加上自己想要的图标
#define ITEMS 23  // 图标数目

	m_ImageList.Create(16, 16, ILC_COLOR24|ILC_MASK, ITEMS, ITEMS);
	m_ImageList.SetBkColor(::GetSysColor(COLOR_BTNFACE));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_UNDO));				//撤销
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_REDO));				//恢复

	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ZOOMIN));				//放大
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ZOOMOUT));			//缩小
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ZOOM_DEFAULT));		//1:1
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ZOOM_FIT_VIEW));		//适应窗口

	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_HISTOGRAM));			//直方图
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_HIST_RED));			//R直方图
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_HIST_GREEN));			//G直方图
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_HIST_BLUE));			//B直方图

	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_FLIP_V));				//垂直翻转
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_FLIP_H));				//水平翻转
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_TRANSPOSE));			//转置图像
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ROTATE));				//旋转图像
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_TO_GRAY));			//转为灰度图像

	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_CURVE));				//曲线
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_COLORGRAD));			//色阶
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_THRESHOLD));			//阈值
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ZOOM_DLG));			//缩放
	m_MyToolBar.GetToolBarCtrl().SetImageList(&m_ImageList); 

	m_MyToolBar.SetButtons(NULL, ITEMS);
	m_MyToolBar.SetSizes(CSize(23, 22), CSize(16, 16));
	m_MyToolBar.SetButtonInfo(0, ID_EDIT_UNDO, TBBS_BUTTON, 0);
	m_MyToolBar.SetButtonInfo(1, ID_EDIT_REDO, TBBS_BUTTON, 1);

	m_MyToolBar.SetButtonStyle(2, TBBS_SEPARATOR);
	m_MyToolBar.SetButtonInfo(3, ID_VIEW_ZOOMIN, TBBS_BUTTON, 2);
	m_MyToolBar.SetButtonInfo(4, ID_VIEW_ZOOMOUT, TBBS_BUTTON, 3);
	m_MyToolBar.SetButtonInfo(5, ID_VIEW_DEFAULT, TBBS_BUTTON, 4);
	m_MyToolBar.SetButtonInfo(6, ID_VIEW_FIT_WINDOW, TBBS_BUTTON, 5);

	m_MyToolBar.SetButtonStyle(7, TBBS_SEPARATOR);
	m_MyToolBar.SetButtonInfo(8, ID_HISTOGRAM_II, TBBS_BUTTON, 6);
	m_MyToolBar.SetButtonInfo(9, ID_VIEW_R_INTENSITY, TBBS_BUTTON, 7);
	m_MyToolBar.SetButtonInfo(10, ID_VIEW_G_INTENSITY, TBBS_BUTTON, 8);
	m_MyToolBar.SetButtonInfo(11, ID_VIEW_B_INTENSITY, TBBS_BUTTON, 9);

	m_MyToolBar.SetButtonStyle(12, TBBS_SEPARATOR);
	m_MyToolBar.SetButtonInfo(13, ID_FLIP_VERTICAL, TBBS_BUTTON, 10);
	m_MyToolBar.SetButtonInfo(14, ID_FLIP_HORIZONTAL, TBBS_BUTTON, 11);
	m_MyToolBar.SetButtonInfo(15, ID_TRANSPOSE, TBBS_BUTTON, 12);
	m_MyToolBar.SetButtonInfo(16, ID_CALL_ROTATE_DLG, TBBS_BUTTON, 13);
	m_MyToolBar.SetButtonInfo(17, ID_TOGRAY, TBBS_BUTTON, 14);

	m_MyToolBar.SetButtonStyle(18, TBBS_SEPARATOR);
	m_MyToolBar.SetButtonInfo(19, ID_CALL_CURVE_DLG, TBBS_BUTTON, 15);
	m_MyToolBar.SetButtonInfo(20, ID_CALL_COLORGRAD_DLG, TBBS_BUTTON, 16);
	m_MyToolBar.SetButtonInfo(21, ID_CALL_THRESHOLD_DLG, TBBS_BUTTON, 17);
	m_MyToolBar.SetButtonInfo(22, ID_CALL_ZOOM_DLG, TBBS_BUTTON, 18);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_MyToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_MyToolBar);

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	m_hHotKey = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR));

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style &= ~FWS_ADDTOTITLE;
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers



BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(::TranslateAccelerator(GetSafeHwnd(), m_hHotKey, pMsg))   
		return true;

	return CMDIFrameWnd::PreTranslateMessage(pMsg);
}


void CMainFrame::OnHotkeyCtrlN()
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd *)AfxGetApp()->GetMainWnd();
	CMDIChildWnd* pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();
	CDIP_SystemDoc* pDoc = (CDIP_SystemDoc *)pChild->GetActiveDocument();
	CDIP_SystemApp* pMyApp = (CDIP_SystemApp*)AfxGetApp();
	pMyApp->OnFileMynew();
}

void CMainFrame::OnHotkeyCtrlO()
{
	CDIP_SystemApp* pMyApp = (CDIP_SystemApp*)AfxGetApp();
	pMyApp->OnFileMyopen();
}


void CMainFrame::OnHotkeyCtrlQ()
{
	AfxGetMainWnd()->SendMessage(WM_CLOSE);
}
