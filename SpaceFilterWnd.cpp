// SpaceFilterWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "SpaceFilterWnd.h"
#include "Filter.h"
#include <cmath>
#include "GaussianDlg.h"

// CSpaceFilterWnd

IMPLEMENT_DYNAMIC(CSpaceFilterWnd, CWnd)

CSpaceFilterWnd::CSpaceFilterWnd()
{
	m_pPaintRect = NULL;         //绘图区
	m_pAppFont = NULL;           //字体
	m_pSmallFont = NULL;         //更小的字体
	m_pImage = NULL;             //原图像指针
	m_dataSrc = NULL;            //图像数据
	for (int i = 0; i<MAX_FILTER_SIZE*MAX_FILTER_SIZE; i++)
	{
		m_pElements[i] = NULL;
	}

	m_pBnOK = NULL;              //确定
	m_pBnCancel = NULL;          //取消
	m_pBnReset = NULL;           //重置
	m_pBnMedianFilter = NULL;    //中值滤波
	m_pBnAdaptiveFilter = NULL;  //自适应中值滤波
	m_pBnAddPepperSaltNoise = NULL;
	m_pBnAddGaussianNoise = NULL;
	m_pBnApply = NULL;
	m_pComboOperation = NULL;
	m_pComboFilterSize = NULL;

	m_pPixelBlock = NULL;
	m_pMyFilter = NULL;
	m_nFilterSize = 3;
	m_nMaxFilterSize = 7;
	m_nOperation = 0;
	m_Mean = 8;
	m_Std = 24;
	m_Pepper = 0.5;
	m_Pollution = 0.02;
}

CSpaceFilterWnd::~CSpaceFilterWnd()
{
	if (m_pPaintRect != NULL)           delete m_pPaintRect;
	if (m_pAppFont != NULL)             delete m_pAppFont;
	if (m_pSmallFont != NULL)           delete m_pSmallFont;
	if (m_dataSrc != NULL)              delete m_dataSrc;
	for (int i = 0; i<MAX_FILTER_SIZE*MAX_FILTER_SIZE; i++)
	{
		if (m_pElements[i] != NULL)     delete m_pElements[i];
	}

	if (m_pBnOK != NULL)                delete m_pBnOK;
	if (m_pBnCancel != NULL)            delete m_pBnCancel;
	if (m_pBnReset != NULL)             delete m_pBnReset;
	if (m_pBnMedianFilter != NULL)      delete m_pBnMedianFilter;
	if (m_pBnAdaptiveFilter != NULL)    delete m_pBnAdaptiveFilter;
	if (m_pBnAddPepperSaltNoise != NULL)delete m_pBnAddPepperSaltNoise;
	if (m_pBnAddGaussianNoise != NULL)  delete m_pBnAddGaussianNoise;
	if (m_pBnApply != NULL)             delete m_pBnApply;
	if (m_pComboOperation != NULL)      delete m_pComboOperation;
	if (m_pComboFilterSize != NULL)     delete m_pComboFilterSize;

	if (m_pPixelBlock != NULL)          delete m_pPixelBlock;
	if (m_pMyFilter != NULL)            delete m_pMyFilter; 
}

BEGIN_MESSAGE_MAP(CSpaceFilterWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &CSpaceFilterWnd::OnCmdOK)
	ON_BN_CLICKED(IDCANCEL, &CSpaceFilterWnd::OnCmdCancel)
	ON_BN_CLICKED(ID_SF_WND_RESET, &CSpaceFilterWnd::OnCmdReset)
	ON_BN_CLICKED(ID_SF_WND_MEDIAN_FILTER, &CSpaceFilterWnd::OnCmdMedianFilter)
	ON_BN_CLICKED(ID_SF_WND_SAMEDIAN_FILTER, &CSpaceFilterWnd::OnCmdAdaptiveFilter)
	ON_BN_CLICKED(ID_SF_WND_PEPPERSALT_NOISE, &CSpaceFilterWnd::OnCmdAddPepperSaltNoise)
	ON_BN_CLICKED(ID_SF_WND_GAUSSIAN_NOISE, &CSpaceFilterWnd::OnCmdAddGaussianNoise)
	ON_BN_CLICKED(ID_SF_WND_APPLY, &CSpaceFilterWnd::OnCmdApply)
	ON_CBN_SELCHANGE(ID_SF_WND_COMBO, &CSpaceFilterWnd::OnCmdChooseOperation)
	ON_CBN_SELCHANGE(ID_SF_WND_FILTER_SIZE, &CSpaceFilterWnd::OnCmdChangeFilterSize)
END_MESSAGE_MAP()

// CSpaceFilterWnd 消息处理程序

void CSpaceFilterWnd::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文
}

int CSpaceFilterWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetClientRect(&rect);

	//绘图区
	m_pPaintRect = new CRect(150, 10, rect.right-20, rect.bottom-20);

	//字体
	m_pAppFont = new CFont;
	m_pAppFont->CreatePointFont(90, L"Times New Roman");
	m_pSmallFont = new CFont;
	m_pSmallFont->CreatePointFont(75, L"Times New Roman");

	//确定
	m_pBnOK = new CMFCButton;
	m_pBnOK->Create(L"确  定", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(), this, IDOK);
	m_pBnOK->SetFont(m_pAppFont);

	//取消
	m_pBnCancel = new CMFCButton;
	m_pBnCancel->Create(L"取  消", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(), this, IDCANCEL);
	m_pBnCancel->SetFont(m_pAppFont);

	//重置
	m_pBnReset = new CMFCButton;
	m_pBnReset->Create(L"重  置", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(), this, ID_SF_WND_RESET);
	m_pBnReset->SetFont(m_pAppFont);

	//中值滤波
	m_pBnMedianFilter = new CMFCButton;
	m_pBnMedianFilter->Create(L"中值滤波", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(), this, ID_SF_WND_MEDIAN_FILTER);
	m_pBnMedianFilter->SetFont(m_pAppFont);

	//自适应中值滤波
	m_pBnAdaptiveFilter = new CMFCButton;
	m_pBnAdaptiveFilter->Create(L"自适应", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(), this, ID_SF_WND_SAMEDIAN_FILTER);
	m_pBnAdaptiveFilter->SetFont(m_pAppFont);

	//添加椒盐噪声
	m_pBnAddPepperSaltNoise= new CMFCButton;
	m_pBnAddPepperSaltNoise->Create(L"椒盐噪声", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(), this, ID_SF_WND_PEPPERSALT_NOISE);
	m_pBnAddPepperSaltNoise->SetFont(m_pAppFont);

	//添加高斯噪声
	m_pBnAddGaussianNoise = new CMFCButton;
	m_pBnAddGaussianNoise->Create(L"高斯噪声", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(), this, ID_SF_WND_GAUSSIAN_NOISE);
	m_pBnAddGaussianNoise->SetFont(m_pAppFont);

	//应用操作
	m_pBnApply = new CMFCButton;
	m_pBnApply->Create(L"应  用", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(), this, ID_SF_WND_APPLY);
	m_pBnApply->SetFont(m_pAppFont);

	//选择操作类型
	m_pComboOperation = new CComboBox;
	m_pComboOperation->Create(WS_CHILD | WS_VISIBLE | CBS_AUTOHSCROLL | CBS_DROPDOWNLIST | WS_EX_TOPMOST, CRect(), this, ID_SF_WND_COMBO);
	m_pComboOperation->InsertString(0, L"算子");
	m_pComboOperation->InsertString(1, L"Smooth");
	m_pComboOperation->InsertString(2, L"Sharpen");
	m_pComboOperation->InsertString(3, L"Sobel");
	m_pComboOperation->InsertString(4, L"Prewitt");
	m_pComboOperation->SetCurSel(0);
	m_pComboOperation->SetFont(m_pSmallFont);

	//改变滤波器大小
	m_pComboFilterSize = new CComboBox;
	m_pComboFilterSize->Create(WS_CHILD | WS_VISIBLE | CBS_AUTOHSCROLL | CBS_DROPDOWNLIST | WS_EX_TOPMOST, CRect(), this, ID_SF_WND_FILTER_SIZE);
	m_pComboFilterSize->InsertString(0, L"3  阶");
	m_pComboFilterSize->InsertString(1, L"5  阶");
	m_pComboFilterSize->InsertString(2, L"7  阶");
	m_pComboFilterSize->SetCurSel(0);
	m_pComboFilterSize->SetFont(m_pAppFont);

	// Edit 编辑框
	double SpaceX = m_pPaintRect->Width()/(MAX_FILTER_SIZE+1);
	double SpaceY = m_pPaintRect->Height()/(MAX_FILTER_SIZE+1);
	for (int i = 0; i<MAX_FILTER_SIZE; i++)
	{
		for (int j = 0; j<MAX_FILTER_SIZE; j++)
		{
			m_pElements[j + i*MAX_FILTER_SIZE] = new CEdit;
			m_pElements[j + i*MAX_FILTER_SIZE]->Create(WS_CHILD | WS_VISIBLE, CRect(150+(j+1)*SpaceX, (i+1)*SpaceY, 150+(j+1)*SpaceX+40, (i+1)*SpaceY+10), this, 10000+i);
			m_pElements[j + i*MAX_FILTER_SIZE]->ShowWindow(SW_HIDE);
			m_pElements[j + i*MAX_FILTER_SIZE]->SetFont(m_pAppFont);
		}
	}

	//构造滤波器
	m_pPixelBlock = new CFilter(m_nFilterSize);
	m_pMyFilter = new CFilter(3);

	// 设置显示
	int R = MAX_FILTER_SIZE/2;
	int r = m_pMyFilter->m_nFilterWidth/2;
	for (int i = R-r, s = 0; i<=R+r; i++, s++)
	{
		for (int j = R-r, t = 0; j<=R+r; j++, t++)
		{
			m_pElements[j + i*MAX_FILTER_SIZE]->SetWindowText(L"0");
			m_pElements[j + i*MAX_FILTER_SIZE]->ShowWindow(SW_SHOW);
		}
	}

	//获取活动文档
	CMDIFrameWnd* pFrame = (CMDIFrameWnd *)AfxGetApp()->GetMainWnd();
	CMDIChildWnd* pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();
	m_pDoc = (CDIP_SystemDoc *)pChild->GetActiveDocument();
	m_pView = (CDIP_SystemView *)pChild->GetActiveView();
	m_pImage = m_pDoc->DIBImage();

	//获取位图信息
	if (m_pImage == NULL) return 0;
	/*if (m_pImage->GetBPP()>8)
	{
		MessageBox(L"彩色图像将转换为灰度图像！", L"警告", MB_OK | MB_ICONWARNING);
	}*/
	m_nImageWidth = m_pImage->GetWidth();    //宽
	m_nImageHeight = m_pImage->GetHeight();  //高
	m_nChannel = m_pImage->GetBPP()/8;       //通道
	m_nRowlen = abs(m_pImage->GetPitch());   //行宽
	m_nlenData = m_nRowlen*m_nImageHeight;   //长度

	//获取原始数据
	m_dataSrc = new BYTE[m_nlenData];
	BYTE* pBits = (BYTE *)m_pImage->GetBits()+(m_pImage->GetPitch()*(m_nImageHeight-1));
	memcpy(m_dataSrc, pBits, m_nlenData);

	return 0;
}

void CSpaceFilterWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(&rect);
	m_pPaintRect->right = rect.right-10;
	m_pPaintRect->bottom = rect.bottom-10;

	double Space = rect.Height()/6.0;
	Reshape(m_pBnOK, Space, 1);
	Reshape(m_pBnCancel, Space, 2);
	Reshape(m_pBnReset, Space, 3);
	Reshape(m_pBnAddPepperSaltNoise, Space, 4);
	Reshape(m_pBnAddGaussianNoise, Space, 5);
	Reshape(m_pBnMedianFilter, Space, 1, 2);
	Reshape(m_pBnAdaptiveFilter, Space, 2, 2);
	Reshape(m_pBnApply, Space, 3, 2);
	if (m_pComboOperation->GetSafeHwnd())
	{
		m_pComboOperation->MoveWindow(10 + 70, 4*Space - 10, 60, 200);
	}
	if (m_pComboFilterSize->GetSafeHwnd())
	{
		m_pComboFilterSize->MoveWindow(10 + 70, 5*Space - 10, 60, 100);
	}

	double SpaceX = m_pPaintRect->Width()/(MAX_FILTER_SIZE+1);
	double SpaceY = m_pPaintRect->Height()/(MAX_FILTER_SIZE+1);
	for (int i = 0; i<MAX_FILTER_SIZE; i++)
	{
		for (int j = 0; j<MAX_FILTER_SIZE; j++)
		{
			if (m_pElements[j + i*MAX_FILTER_SIZE]->GetSafeHwnd())
			{
				m_pElements[j + i*MAX_FILTER_SIZE]->MoveWindow(150+(j+1)*SpaceX-20, (i+1)*SpaceY, 40, 20);
			}
		}
	}
}

//*** 函数 ***

// 重置布局
void CSpaceFilterWnd::Reshape(CWnd* pWnd, double Space, int nRow, int nCol)
{
	if (pWnd->GetSafeHwnd())
	{
		pWnd->MoveWindow(10+(nCol-1)*70, nRow*Space-10, 60, 20);
	}
}

//添加椒盐噪声
BOOL CSpaceFilterWnd::AddPepperSaltNoise()
{
	if (m_pImage == NULL) return FALSE;
	BYTE* pCur = (BYTE *)m_pImage->GetBits()+(m_pImage->GetPitch()*(m_nImageHeight-1));

	BeginWaitCursor();
	time_t t;
	srand((unsigned)time(&t));
	//椒盐噪声
	for (int i = 0, Temp; i<m_nImageHeight; i++)
	{
		for (int j = 0; j<m_nImageWidth; j++)
		{
			BYTE* pixel = pCur + j*m_nChannel + i*m_nRowlen;
			if ((double)rand()/RAND_MAX < m_Pollution)
			{//当前像素是否被污染
				Temp = ProducePepperSaltNoise(m_Pepper);//被哪种噪声污染
				for (int k = 0; k<m_nChannel; k++)
				{
					*(pixel+k) = Temp;
				}
			}
		}
	}
	EndWaitCursor();
	return TRUE;
}

//添加高斯噪声
BOOL CSpaceFilterWnd::AddGaussianNoise()
{
	if (m_pImage == NULL) return FALSE;
	BYTE* pCur = (BYTE *)m_pImage->GetBits()+(m_pImage->GetPitch()*(m_nImageHeight-1));

	BeginWaitCursor();
	time_t t;
	srand((unsigned)time(&t));
	for (int i = 0, Temp; i<m_nImageHeight; i++)
	{
		for (int j = 0; j<m_nImageWidth; j++)
		{
			BYTE* pixel = pCur + j*m_nChannel + i*m_nRowlen;
			for (int k = 0; k<m_nChannel; k++)
			{
				Temp = ProduceGaussianNoise(m_Mean, m_Std);
				*(pixel+k) = *(pixel+k)+Temp;
			}
		}
	}
	EndWaitCursor();
	return TRUE;
}

//中值滤波
BOOL CSpaceFilterWnd::MedianFilter()
{
	if (m_pImage == NULL) return FALSE;

	BeginWaitCursor();
	BYTE* pCur = (BYTE *)m_pImage->GetBits()+(m_pImage->GetPitch()*(m_nImageHeight-1));
	int* Temp = new int[m_nChannel];
	for(int y = 0; y<m_nImageHeight; y++)
	{
		for(int x = 0; x<m_nImageWidth; x++)
		{
			for (int k = 0; k<m_nChannel; k++)
			{
				m_pPixelBlock->Assign(pCur, m_nImageWidth, m_nImageHeight, m_nChannel, m_nRowlen, k, y, x);
				Temp[k] = m_pPixelBlock->FindMed();
			}
			BYTE* pixel = pCur + x*m_nChannel + y*m_nRowlen;
			for (int k = 0; k<m_nChannel; k++)
			{
				*(pixel+k) = (BYTE)Temp[k];
			}
		}
	}
	delete [] Temp;
	EndWaitCursor();
	return TRUE;
}

//自适应中值滤波
BOOL CSpaceFilterWnd::AdaptMedianFilter()
{
	if (m_pImage == NULL) return FALSE;

	BeginWaitCursor();
	BYTE* pCur = (BYTE *)m_pImage->GetBits()+(m_pImage->GetPitch()*(m_nImageHeight-1));
	int* Temp = new int[m_nChannel];
	for(int y = 0; y<m_nImageHeight; y++)
	{
		for(int x = 0; x<m_nImageWidth; x++)
		{
			for (int k = 0; k<m_nChannel; k++)
			{
				Temp[k] = FilterProcessA(pCur, k, y, x, 3);
			}
			BYTE* pixel = pCur + x*m_nChannel + y*m_nRowlen;
			for (int k = 0; k<m_nChannel; k++)
			{
				*(pixel+k) = (BYTE)Temp[k];
			}
		}
	}
	delete [] Temp;
	EndWaitCursor();
	return TRUE;
}

//进程A
int CSpaceFilterWnd::FilterProcessA(BYTE* dataSrc, int curChannel, int y, int x, int FilterSize)
{
	m_pPixelBlock->Create(dataSrc, m_nImageWidth, m_nImageHeight, m_nChannel, m_nRowlen, curChannel, y, x, FilterSize);

	int Zmin = m_pPixelBlock->FindMin();
	int Zmax = m_pPixelBlock->FindMax();
	int Zmed = m_pPixelBlock->FindMed();
	int Zxy = *(dataSrc + x*m_nChannel+curChannel + y*m_nRowlen);
	if (Zmed-Zmin>0 && Zmed-Zmax<0)
	{//转进程B
		return FilterProcessB(Zmin, Zmax, Zmed, Zxy);
	}
	else
	{//增大窗口尺寸
		if (FilterSize<m_nMaxFilterSize) return FilterProcessA(dataSrc, curChannel, y, x, FilterSize+1);
		else                             return Zmed;
	}
}

//进程B
int CSpaceFilterWnd::FilterProcessB(int Zmin, int Zmax, int Zmed, int Zxy)
{
	if (Zxy-Zmin>0 && Zxy-Zmax<0) return Zxy;
	else                          return Zmed;
}

// 卷积
void CSpaceFilterWnd::Convolute(CFilter* Filter)
{
	BeginWaitCursor();
	BYTE* pCur = (BYTE *)m_pImage->GetBits()+(m_pImage->GetPitch()*(m_nImageHeight-1));
	BYTE* TempData = new BYTE[m_nlenData];
	memcpy(TempData, pCur, m_nlenData);
	m_pPixelBlock->Create(Filter);
	for(int y = 0; y<m_nImageHeight; y++)
	{
		for(int x = 0; x<m_nImageWidth; x++)
		{
			m_pPixelBlock->Assign(TempData, m_nImageWidth, m_nImageHeight, m_nChannel, m_nRowlen, 0, y, x);
			double Temp1[3][3], Temp2[3][3];
			for (int m = 0; m<3; m++)
			{
				for (int n = 0; n<3; n++)
				{
					Temp1[m][n] = m_pPixelBlock->Visit(n, m);
					Temp2[m][n] = Filter->Visit(n, m);
				}
			}
			BYTE* pixel = pCur + x*m_nChannel + y*m_nRowlen;
			double conv = m_pPixelBlock->Multiply(Filter);
			if (conv<0)       conv = 0;
			else if(conv>255) conv = 255;
			*pixel = (BYTE)conv;
		}
	}
	delete [] TempData;
	EndWaitCursor();
}

afx_msg void CSpaceFilterWnd::OnCmdOK()
{
	//直接关闭
	::SendMessage(GetParent()->GetSafeHwnd(), WM_CLOSE, 0, 0);
}

afx_msg void CSpaceFilterWnd::OnCmdCancel()
{
	OnCmdReset();
	::SendMessage(GetParent()->GetSafeHwnd(), WM_CLOSE, 0, 0);
}

afx_msg void CSpaceFilterWnd::OnCmdReset()
{
	//恢复数据
	if (m_pImage != NULL)
	{
		BYTE* pCur = (BYTE *)m_pImage->GetBits()+(m_pImage->GetPitch()*(m_nImageHeight-1));
		memcpy(pCur, m_dataSrc, m_nlenData);
		m_pView->Invalidate(TRUE);
	}
}

afx_msg void CSpaceFilterWnd::OnCmdMedianFilter()
{
	if (!MedianFilter()) return;
	m_pView->Invalidate(TRUE);
}

afx_msg void CSpaceFilterWnd::OnCmdAdaptiveFilter()
{
	if (!AdaptMedianFilter()) return;
	m_pView->Invalidate(TRUE);
}

afx_msg void CSpaceFilterWnd::OnCmdAddPepperSaltNoise()
{
	CGaussianDlg dlg;
	dlg.m_dGaussianMean = m_Pollution;
	dlg.m_dGaussianStd = m_Pepper;
	dlg.m_String1 = "程度:";
	dlg.m_String2 = "胡椒:";
	if (dlg.DoModal() == IDOK)
	{
		m_Pollution = dlg.m_dGaussianMean;
		m_Pepper = dlg.m_dGaussianStd;
		if(!AddPepperSaltNoise()) return;
		m_pView->Invalidate(TRUE);
	}
}

afx_msg void CSpaceFilterWnd::OnCmdAddGaussianNoise()
{
	CGaussianDlg dlg;
	dlg.m_dGaussianMean = m_Mean;
	dlg.m_dGaussianStd = m_Std;
	if (dlg.DoModal() == IDOK)
	{
		m_Mean = dlg.m_dGaussianMean;
		m_Std = dlg.m_dGaussianStd;
		if(!AddGaussianNoise()) return;
		m_pView->Invalidate(TRUE);
	}
}

afx_msg void CSpaceFilterWnd::OnCmdApply()
{
	if (m_pImage == NULL) return;
	if (m_nOperation == 0) return;
	// 获取滤波器元素值
	int R = MAX_FILTER_SIZE/2;
	int r = m_pMyFilter->m_nFilterWidth/2;
	CString text = L"";
	for (int i = R-r, s = 0; i<=R+r; i++, s++)
	{
		for (int j = R-r, t = 0; j<=R+r; j++, t++)
		{
			m_pElements[j + i*MAX_FILTER_SIZE]->GetWindowText(text);
			m_pMyFilter->Visit(t, s) = _ttoi(text.GetBuffer());
		}
	}
	Convolute(m_pMyFilter);
	m_pView->Invalidate(TRUE);
}

afx_msg void CSpaceFilterWnd::OnCmdChooseOperation()
{
	m_nOperation = m_pComboOperation->GetCurSel();
	if (m_nOperation == 0) return;
	double Smooth[9] = {1/16.0, 2/16.0, 1/16.0, 2/16.0, 4/16.0, 2/16.0, 1/16.0, 2/16.0, 1/16.0};//平滑
	double Laplacian[9] = {-1, -1, -1, -1, 8, -1, -1, -1, -1};                                  //锐化
	double Sobel[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};                                           //梯度
	double Prewitt[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
	double* CurOpr = NULL;
	switch (m_nOperation)
	{
	case 0:                       break;
	case 1:    CurOpr = Smooth;   break;
	case 2:    CurOpr = Laplacian;break;
	case 3:    CurOpr = Sobel;    break;
	case 4:    CurOpr = Prewitt;  break;
	default:                      break;
	}
	// 设置显示
	int R = MAX_FILTER_SIZE/2;
	int r = m_pMyFilter->m_nFilterWidth/2;
	CString text = L"";
	for (int i = R-r, s = 0, k = 0; i<=R+r; i++, s++)
	{
		for (int j = R-r, t = 0; j<=R+r; j++, t++)
		{
			if (2<=i && i<=4 && 2<=j && j<=4)
				m_pMyFilter->Visit(t, s) = CurOpr[k++];
			else
				m_pMyFilter->Visit(t, s) = 0;
			text.Format(L"%.3f", m_pMyFilter->Visit(t, s));
			m_pElements[j + i*MAX_FILTER_SIZE]->SetWindowText(text);
			m_pElements[j + i*MAX_FILTER_SIZE]->ShowWindow(SW_SHOW);
		}
	}
}

afx_msg void CSpaceFilterWnd::OnCmdChangeFilterSize()
{
	m_nFilterSize = 2*m_pComboFilterSize->GetCurSel()+3;
	m_pMyFilter->Create(m_nFilterSize);//修改滤波器
	for (int i = 0; i<MAX_FILTER_SIZE*MAX_FILTER_SIZE; i++)
	{
		m_pElements[i]->ShowWindow(SW_HIDE);
		m_pElements[i]->Invalidate(TRUE);
	}
	// 设置显示
	int R = MAX_FILTER_SIZE/2;
	int r = m_nFilterSize/2;
	for (int i = R-r, s = 0; i<=R+r; i++, s++)
	{
		for (int j = R-r, t = 0; j<=R+r; j++, t++)
		{
			if (i<2 || i>4 || j<2 || j>4)
				m_pElements[j + i*MAX_FILTER_SIZE]->SetWindowText(L"0");
			m_pElements[j + i*MAX_FILTER_SIZE]->ShowWindow(SW_SHOW);
		}
	}
	// 通知父窗口重绘，2014年12月7日找到这个问题。
	GetParent()->Invalidate(TRUE);
}

//
// *** 其他函数 ***
//

/************************************************************************/
/*  对于正态分布，利用反函数的方法来获取正态分布序列显然是很麻烦的，牵涉到
    很复杂的积分微分运算，同时为了方便，我们取，即标准正态分布。因此这里介
	绍了两种算法：
	第一种：
	    Box和Muller在1958年给出了由均匀分布的随机变量生成正态分布的随机变量
	的算法。设U1, U2是区间 (0, 1)上均匀分布的随机变量，且相互独立。令  
	X1=sqrt(-2*log(U1)) * cos(2*PI*U2); 
	X2=sqrt(-2*log(U1)) * sin(2*PI*U2);  
	那么X1, X2服从N(0,1)分布，且相互独立。
	p = rand()%100;//产生从0-32767的任意一个值
	b[j] = double(p)/double(100);
	a[j] = sqrt(-2*log(a[j]))*cos(2*3.1415926*b[j]);
	第二种：
	    近似生成标准正态分布，独立同分布的多个随机变量和的分布趋近于正态
	分布,取k个均匀分布的(0,1)随机变量，则它们的和近似服从正态分布。实践中,
	取k=12,(因为D(xi)=1/12),则新的随机变量y=x1+x2+...+x12-0.5*12，可以求出
	数学期望E(y)=0,方差D(y)=12*1/12=1，因此可以近似描述标准正态分布。 */
/************************************************************************/

//nMean-高斯分布的均值, nStd-高斯分布的标准差,使用中心极限定理
double ProduceGaussianNoise(double nMean, double nStd)
{
	double sum = 0; 
	for(int i = 0; i<12; i++) sum += (double)rand()/RAND_MAX;
	//计算机中rand()函数为0~32767（2^15-1）
	//故sum＋为0～12之间的均匀随机变量
	return nMean+nStd*(sum-6);
}

//nMean-高斯分布的均值, nStd-高斯分布的标准差,使用Box-Muller方法
double ProduceGaussianNoise2(double nMean, double nStd)
{
	return nMean+nStd*(sqrt(-2*log((double)rand()/RAND_MAX))*cos(2*3.1415926*(double)rand()/RAND_MAX));
}

//产生椒盐噪声:Probability是胡椒噪声所占比例
int ProducePepperSaltNoise(double Probability)
{
	if ((double)rand()/RAND_MAX <= Probability) return 0;
	else                                        return 255;
}

