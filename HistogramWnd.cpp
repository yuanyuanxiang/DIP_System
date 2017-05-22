// CLAHEWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "HistogramWnd.h"

// CCLAHEWnd

IMPLEMENT_DYNAMIC(CHistogramWnd, CWnd)

	CHistogramWnd::CHistogramWnd()
{
	m_pPaintRect = NULL;
	m_pClientRect = NULL;
	m_pBnOK = NULL;
	m_pBnCancel = NULL;
	m_pBnHE = NULL;
	m_pBnLHE = NULL;
	m_pBnAHE = NULL;
	m_pBnCLAHE = NULL;
	m_pBnReset = NULL;
	m_dataSrc = NULL;
	m_curData = NULL;
	for (int i = 0; i<4; i++)
	{
		m_pfHistogram[i] = NULL;
		m_pfTotalHist[i] = NULL;
		m_V4Transform[i] = NULL;
	}
	m_nChannelSelected = 0;
	m_nRegionSize = 3;
	m_nRegionLength = m_nRegionSize*m_nRegionSize;
}

CHistogramWnd::~CHistogramWnd()
{
	if (m_pPaintRect != NULL)     delete m_pPaintRect;
	if (m_pClientRect != NULL)    delete m_pClientRect;
	if (m_pBnOK != NULL)          delete m_pBnOK;
	if (m_pBnCancel != NULL)      delete m_pBnCancel;
	if (m_pBnHE != NULL)          delete m_pBnHE;
	if (m_pBnLHE != NULL)         delete m_pBnLHE;
	if (m_pBnAHE != NULL)         delete m_pBnAHE;
	if (m_pBnCLAHE != NULL)       delete m_pBnCLAHE;
	if (m_pBnReset != NULL)       delete m_pBnReset;
	if (m_dataSrc != NULL)        delete [] m_dataSrc;
	if (m_curData != NULL)        delete [] m_curData;
	for (int i = 0; i<4; i++)
	{
		if (m_pfHistogram[i] != NULL) delete [] m_pfHistogram[i];
		if (m_pfTotalHist[i] != NULL) delete [] m_pfTotalHist[i];
		if (m_V4Transform[i] != NULL) delete [] m_V4Transform[i];
	}
}

BEGIN_MESSAGE_MAP(CHistogramWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &CHistogramWnd::OnCmdOK)
	ON_BN_CLICKED(IDCANCEL, &CHistogramWnd::OnCmdCancel)
	ON_BN_CLICKED(ID_HIST_WND_HE, &CHistogramWnd::OnCmdHe)
	ON_BN_CLICKED(ID_HIST_WND_LHE, &CHistogramWnd::OnCmdLhe)
	ON_BN_CLICKED(ID_HIST_WND_AHE, &CHistogramWnd::OnCmdAhe)
	ON_BN_CLICKED(ID_HIST_WND_CLAHE, &CHistogramWnd::OnCmdClahe)
	ON_BN_CLICKED(ID_HIST_WND_RESET, &CHistogramWnd::OnCmdReset)
END_MESSAGE_MAP()

//获取活动文档
CDIP_SystemDoc* CHistogramWnd::GetDocument()
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd *)AfxGetApp()->GetMainWnd();
	CMDIChildWnd* pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();
	return (CDIP_SystemDoc *)pChild->GetActiveDocument();
}

//获取活动视图
CDIP_SystemView* CHistogramWnd::GetActiveView()
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd *)AfxGetApp()->GetMainWnd();
	CMDIChildWnd* pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();
	return (CDIP_SystemView *)pChild->GetActiveView();
}

//获取图像的基本信息
BOOL CHistogramWnd::GetImageInfo()
{
	if (m_pImage == NULL) return FALSE;
	
	m_nImageWidth = m_pImage->GetWidth();
	m_nImageHeight = m_pImage->GetHeight();
	m_nChannel = m_pImage->GetBPP()/8;
	m_nRowlen = (m_nImageWidth*8*m_nChannel+31)/32*4;
	m_nlenData = m_nRowlen*m_nImageHeight;

	//将图像源数据存储起来
	if (m_dataSrc != NULL)
	{
		delete [] m_dataSrc;
		delete [] m_curData;
		m_dataSrc = NULL;
		m_curData = NULL;
	}
	m_dataSrc = new BYTE[m_nlenData];
	m_curData = new BYTE[m_nlenData];
	BYTE* pBits = (BYTE *)m_pImage->GetBits()+(m_pImage->GetPitch()*(m_nImageHeight-1));//获得图像数据首地址
	memcpy(m_dataSrc, pBits, m_nlenData);
	memcpy(m_curData, pBits, m_nlenData);
	return TRUE;
}

//获得位图的直方图数据，以便于绘制
BOOL CHistogramWnd::GetHistogram()
{
	if (m_pImage == NULL) return FALSE;

	//计算直方图
	BYTE *pixel = m_curData;
	if (m_nChannel>=3)
	{//3、4通道彩色图像
		for (int i = 0; i<m_nImageHeight; i++)
		{
			for (int j = 0; j<m_nImageWidth; j++)
			{//RGB24各分量的排列顺序为：BGR, RGB32各分量的排列顺序为：BGRA
				m_pfHistogram[3][int(*pixel)]++;                                              //Blue
				m_pfHistogram[2][int(*(pixel+1))]++;                                          //Green
				m_pfHistogram[1][int(*(pixel+2))]++;                                          //Red
				if (*pixel == *(pixel+1) && *(pixel+1) == *(pixel+2))
				{//解决某些图片的RGB通道灰度值缺失
					m_pfHistogram[0][int(*pixel)]++;
				}
				else m_pfHistogram[0][int(*pixel*0.299 + *(pixel+1)*0.587 + *(pixel+2)*0.114)]++;  //RGB
				pixel += m_nChannel;
			}
		}
	}
	else if(m_nChannel == 1)
	{//单通道灰度图像
		for (int i = 0; i<m_nImageHeight; i++)
		{
			for (int j = 0; j<m_nImageWidth; j++)
			{
				for (int k = 0; k<4; k++) m_pfHistogram[k][int(*pixel)]++;
				pixel += m_nChannel;
			}
		}
	}
	else AfxMessageBox(L"不受支持的图像格式。");

	//归一化
	for (int i = 0, j; i<4; i++)
	{
		for (j = 0; j<256; j++)
		{//计算每个灰度级的概率
			m_pfHistogram[i][j] /= (m_nImageWidth*m_nImageHeight);
			if (j != 0)
			{
				m_pfTotalHist[i][j] = m_pfTotalHist[i][j-1]+m_pfHistogram[i][j];
			}
			else
			{
				m_pfTotalHist[i][j] = m_pfHistogram[i][j];
			}
			//计算HE映射表
			m_V4Transform[i][j] = 255*m_pfTotalHist[i][j]+0.5;
		}
	}

	return TRUE;
}

void CHistogramWnd::HistogramEq(BYTE** Region, double Map[256])
{
	int i;
	for (i = 0; i<256; i++) Map[i] = 0;

	//获取概率分布
	BYTE** pixel = Region;
	for (i = 0; i<m_nRegionLength; i++) Map[*pixel[i]] += 1.0/m_nRegionLength;

	//累积概率分布
	for (i = 1; i<256; i++) Map[i] += Map[i-1];
	//计算映射表
	for (i = 0; i<256; i++) Map[i] = int(255*Map[i]+0.5);
	//作用到中心像素
	*pixel[m_nRegionLength/2] = BYTE(Map[*pixel[m_nRegionLength/2]]);
}

void CHistogramWnd::MappingLHE()
{
	int R = m_nRegionSize/2, C = min(3, m_nChannel), i, j, k, s, t, m;
	BYTE** Region = new BYTE*[m_nRegionLength];
	for (i = 0; i<m_nRegionLength; i++) Region[i] = NULL;
	double Map[256];
	BYTE *pixel = m_curData;
	pixel += R*m_nChannel + R*m_nRowlen;
	for (i = R; i<m_nImageHeight-R; i++)
	{
		for (j = R; j<m_nImageWidth-R; j++)
		{
			//计算局部直方图
			for (k = 0; k<C; k++)
			{
				m = 0;
				for (s = -R, t; s<=R; s++)
				{
					for (t = -R; t<=R; t++)
					{
						Region[m] = pixel + k + t*m_nChannel + s*m_nRowlen;
						m++;
					}
				}
				HistogramEq(Region, Map);
			}
			pixel += m_nChannel;
		}
	}
	delete [] Region;
	BYTE* pBits = (BYTE *)m_pImage->GetBits()+(m_pImage->GetPitch()*(m_nImageHeight-1));
	memcpy(pBits, m_curData, m_nlenData);
}

void CHistogramWnd::MappingHE()
{
	BYTE *pixel = m_curData;
	for (int i = 0; i<m_nImageHeight; i++)
	{
		for (int j = 0; j<m_nImageWidth; j++)
		{
			for (int k = 0; k < m_nChannel; k++)
			{
				*(pixel+k) = m_V4Transform[m_nChannelSelected][*(pixel+k)];
			}
			pixel += m_nChannel;
		}
	}
	BYTE* pBits = (BYTE *)m_pImage->GetBits()+(m_pImage->GetPitch()*(m_nImageHeight-1));
	memcpy(pBits, m_curData, m_nlenData);
}

// CCLAHEWnd 消息处理程序

int CHistogramWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pClientRect = new CRect;
	GetClientRect(m_pClientRect);

	//绘图区
	m_pPaintRect = new CRect(100, 20, m_pClientRect->right-20, m_pClientRect->bottom-80);

	double Space = m_pClientRect->Height()/8.0;

	//确定
	m_pBnOK = new CMFCButton;
	m_pBnOK->Create(L"确定", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, Space-10, 80, Space+10), this, IDOK);

	//取消
	m_pBnCancel = new CMFCButton;
	m_pBnCancel->Create(L"取消", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 2*Space-10, 80, 2*Space+10), this, IDCANCEL);

	//HE
	m_pBnHE = new CMFCButton;
	m_pBnHE->Create(L"HE", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 3 * Space - 10, 80, 3 * Space + 10), this, ID_HIST_WND_HE);

	//LHE
	m_pBnLHE = new CMFCButton;
	m_pBnLHE->Create(L"LHE", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 4 * Space - 10, 80, 4 * Space + 10), this, ID_HIST_WND_LHE);

	//AHE
	m_pBnAHE = new CMFCButton;
	m_pBnAHE->Create(L"AHE", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 5 * Space - 10, 80, 5 * Space + 10), this, ID_HIST_WND_AHE);

	//CLAHE
	m_pBnCLAHE = new CMFCButton;
	m_pBnCLAHE->Create(L"CLAHE", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 6 * Space - 10, 80, 6 * Space + 10), this, ID_HIST_WND_CLAHE);

	//CLAHE
	m_pBnReset = new CMFCButton;
	m_pBnReset->Create(L"重置", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 7 * Space - 10, 80, 7 * Space + 10), this, ID_HIST_WND_RESET);

	//其他
	m_pImage = GetDocument()->DIBImage();

	//初始化直方图、累计直方图和映射表
	for (int i = 0; i<4; i++)
	{
		m_pfHistogram[i] = new double[256];
		m_pfTotalHist[i] = new double[256];
		m_V4Transform[i] = new int[256];
		for (int j = 0; j<256; j++)
		{
			m_pfHistogram[i][j] = 0;
			m_pfTotalHist[i][j] = 0;
			m_V4Transform[i][j] = j;
		}
	}

	//获取图像信息
	GetImageInfo();

	//获取直方图
	GetHistogram();

	return 0;
}

void CHistogramWnd::OnPaint()
{
	CPaintDC dc(this);
	CBitmap MemBitmap;
	CDC MemDC;
	MemDC.CreateCompatibleDC(&dc);
	MemBitmap.CreateCompatibleBitmap(&dc, m_pClientRect->Width(), m_pClientRect->Height());
	CBitmap* pOldBmp = MemDC.SelectObject(&MemBitmap);
	MemDC.FillSolidRect(m_pClientRect, RGB(240, 240, 240));//使得填充的颜色比较好看
	CPen *pOldPen, newPen;
	COLORREF LineColor;

	//水平方向颜色条
	for (int i = m_pPaintRect->left, k = 0; i<m_pPaintRect->right; i++, k++)
	{
		LineColor = ColorSetting(0, int(k/m_fWidthRatio), 1);
		newPen.CreatePen(PS_SOLID, 1, LineColor);
		pOldPen = MemDC.SelectObject(&newPen);
		MemDC.MoveTo(i, m_pClientRect->bottom-70);
		MemDC.LineTo(i, m_pClientRect->bottom-50);
		MemDC.SelectObject(pOldPen);
		newPen.DeleteObject();
	}

	if(m_pImage != NULL)//已经载入图像
	{
		//直方图
		LineColor = ColorSetting(0, 128);
		newPen.CreatePen(PS_SOLID, 1, LineColor);
		pOldPen = MemDC.SelectObject(&newPen);
		double rate = 0.20*m_nRectWidth*m_nRectHeight/m_fWidthRatio;//使直方图好看
		for (int i = 0, nTemp; i<m_nRectWidth; i++)
		{
			nTemp = m_pPaintRect->left+i;
			MemDC.MoveTo(nTemp, m_pPaintRect->bottom);
			MemDC.LineTo(nTemp, int(m_pPaintRect->bottom - m_pfHistogram[m_nChannelSelected][int(i/m_fWidthRatio)]*rate));
		}
		newPen.DeleteObject();
	}

	//拷贝出来
	dc.BitBlt(m_pPaintRect->left, m_pPaintRect->top, m_nRectWidth, m_nRectHeight, &MemDC, m_pPaintRect->left, m_pPaintRect->top, SRCCOPY);//直方图
	dc.BitBlt(m_pPaintRect->left-10, m_pClientRect->bottom-70, m_nRectWidth+20, 30, &MemDC, m_pPaintRect->left-10, m_pClientRect->bottom-70, SRCCOPY);//2控制点和颜色条
	MemDC.SelectObject(pOldBmp);
	pOldBmp->DeleteObject();
	MemBitmap.DeleteObject();
}

void CHistogramWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	GetClientRect(m_pClientRect);
	m_pPaintRect->right = m_pClientRect->right-20;
	m_pPaintRect->bottom = m_pClientRect->bottom-80;

	m_nRectWidth = m_pPaintRect->Width();
	m_nRectHeight = m_pPaintRect->Height();
	m_fWidthRatio = (double)m_nRectWidth/255.0;
	m_fHeightRatio = (double)m_nRectHeight/255.0;

	double Space = m_pClientRect->Height()/8.0;
	Reshape(m_pBnOK, Space, 1);
	Reshape(m_pBnCancel, Space, 2);
	Reshape(m_pBnHE, Space, 3);
	Reshape(m_pBnLHE, Space, 4);
	Reshape(m_pBnAHE, Space, 5);
	Reshape(m_pBnCLAHE, Space, 6);
	Reshape(m_pBnReset, Space, 7);
}

void CHistogramWnd::Reshape(CWnd* pWnd, double space, int Number)
{
	if (pWnd->GetSafeHwnd())
	{
		pWnd->MoveWindow(20, space*Number-10, 60, 20);
	}
}

void CHistogramWnd::GetBits(int channel, BYTE* pBits)
{
	BYTE *pixel;
	for (int i = 0; i<m_nImageHeight; i++)
	{
		for (int j = 0; j<m_nImageWidth; j++)
		{
			pixel = m_curData + j * m_nChannel + i * m_nlenData;
			*pBits = *(pixel+channel);
			pBits += 1;
		}
	}
}

void CHistogramWnd::SetBits(int channel, BYTE* pBits)
{
	BYTE *pixel;
	for (int i = 0; i<m_nImageHeight; i++)
	{
		for (int j = 0; j<m_nImageWidth; j++)
		{
			pixel = m_curData + j * m_nChannel + i * m_nlenData;
			*(pixel+channel) = *pBits;
			pBits += 1;
		}
	}
}

void CHistogramWnd::OnCmdOK()
{
	//直接关闭
	::SendMessage(GetParent()->GetSafeHwnd(), WM_CLOSE, 0, 0);
}

void CHistogramWnd::OnCmdCancel()
{
	//恢复数据后关闭
	OnCmdReset();
	::SendMessage(GetParent()->GetSafeHwnd(), WM_CLOSE, 0, 0);
}

void CHistogramWnd::OnCmdReset()
{
	//重置
	if (m_pImage != NULL)
	{
		BYTE *pBits = (BYTE *)m_pImage->GetBits()+(m_pImage->GetPitch()*(m_nImageHeight-1));
		memcpy(pBits, m_dataSrc, m_nlenData);
		memcpy(m_curData, m_dataSrc, m_nlenData);
		GetActiveView()->Invalidate();
		//刷新直方图
		GetHistogram();
		InvalidateRect(m_pPaintRect);
	}
}

void CHistogramWnd::OnCmdHe()
{
	if (m_pImage == NULL) return;
	AfxMessageBox(L"该功能未添加。"); return;
	MappingHE();
	GetActiveView()->Invalidate();
	//刷新直方图
	GetHistogram();
	InvalidateRect(m_pPaintRect);
}

void CHistogramWnd::OnCmdLhe()
{
	if (m_pImage == NULL) return;
	AfxMessageBox(L"该功能未添加。"); return;
	BeginWaitCursor();
	MappingLHE();
	EndWaitCursor();
	GetActiveView()->Invalidate();
	//刷新直方图
	GetHistogram();
	InvalidateRect(m_pPaintRect);
}

void CHistogramWnd::OnCmdAhe()
{
	if (m_pImage == NULL) return;
	AfxMessageBox(L"该功能未添加。"); return;
	GetActiveView()->Invalidate();
}

void CHistogramWnd::OnCmdClahe()
{
	if (m_pImage == NULL) return;
	AfxMessageBox(L"该功能未添加。"); return;
	BYTE* pTemp = new BYTE[m_nImageWidth*m_nImageHeight];
	int channel = min(m_nChannel, 3);
	for (int i = 0; i<channel; i++)
	{//对每个通道都进行直方图均衡化操作
		GetBits(i, pTemp);
		CLAHE(pTemp, m_nImageWidth, m_nImageHeight, 16, 240, 8, 8, 256, 1.5);
		SetBits(i, pTemp);
	}
	BYTE *pBits = (BYTE *)m_pImage->GetBits()+(m_pImage->GetPitch()*(m_nImageHeight-1));
	memcpy(pBits, m_curData, m_nlenData);
	GetActiveView()->Invalidate();
	delete [] pTemp;

	//刷新直方图
	GetHistogram();
	InvalidateRect(m_pPaintRect);
}

//
// *** 其他 ***
//

template<typename TYPE>
void Write2File(CString FilePath, TYPE* dataSrc, int Width, int Height)//写入文件
{
	//打开文件用于写，若文件不存在就创建它
	ofstream Output(FilePath);
	if (!Output) return;
	for (int j = 0; j<Height; j++)
	{
		for (int i = 0; i<Width; i++) Output<<(double)*(dataSrc + i + j*Width)<<", L";
		Output<<"\n";
	}
	Output.close();
}

COLORREF ColorSetting(int Channel, int Strength, int WhereUse)
{
	//Channel:通道
	//Strength:颜色强度
	COLORREF LineColor;
	switch(Channel)
	{
	case 0 :
		if (WhereUse) Strength = 255 - Strength;
		LineColor = RGB(255-Strength, 255-Strength, 255-Strength);
		break;
	case 1 : LineColor = RGB(Strength, 0, 0); break;
	case 2 : LineColor = RGB(0, Strength, 0); break;
	case 3 : LineColor = RGB(0, 0, Strength); break;
	default: break;
	}
	return LineColor;
}

//
/*********************** CLAHE ************************/
//

const unsigned int uiMAX_REG_X = 16;      /* max. # contextual regions in x-direction */
const unsigned int uiMAX_REG_Y = 16;      /* max. # contextual regions in y-direction */

/************************** main function CLAHE ******************/

int CLAHE (BYTE* pImage, unsigned int uiXRes, unsigned int uiYRes, BYTE Min, BYTE Max, unsigned int uiNrX, unsigned int uiNrY, unsigned int uiNrBins, float fCliplimit)
	/*   pImage - Pointer to the input/output image
	*   uiXRes - Image resolution in the X direction
	*   uiYRes - Image resolution in the Y direction
	*   Min - Minimum greyvalue of input image (also becomes minimum of output image)
	*   Max - Maximum greyvalue of input image (also becomes maximum of output image)
	*   uiNrX - Number of contextial regions in the X direction (min 2, max uiMAX_REG_X)
	*   uiNrY - Number of contextial regions in the Y direction (min 2, max uiMAX_REG_Y)
	*   uiNrBins - Number of greybins for histogram (L"dynamic range")
	*   float fCliplimit - Normalized cliplimit (higher values give more contrast)
	* The number of "effective" greylevels in the output image is set by uiNrBins; selecting
	* a small value (eg. 128) speeds up processing and still produce an output image of
	* good quality. The output image will have the same minimum and maximum value as the input
	* image. A clip limit smaller than 1 results in standard (non-contrast limited) AHE.
	*/
{
	unsigned int uiX, uiY;          /* counters */
	unsigned int uiXSize, uiYSize, uiSubX, uiSubY; /* size of context. reg. and subimages */
	unsigned int uiXL, uiXR, uiYU, uiYB;  /* auxiliary variables interpolation routine */
	unsigned long ulClipLimit, ulNrPixels;/* clip limit and region pixel count */
	BYTE* pImPointer;           /* pointer to image */
	BYTE aLUT[uiNR_OF_GREY];        /* lookup table used for scaling of input image */
	unsigned long* pulHist, *pulMapArray; /* pointer to histogram and mappings*/
	unsigned long* pulLU, *pulLB, *pulRU, *pulRB; /* auxiliary pointers interpolation */

	if (uiNrX > uiMAX_REG_X) return -1;       /* # of regions x-direction too large */
	if (uiNrY > uiMAX_REG_Y) return -2;       /* # of regions y-direction too large */
	if (uiXRes % uiNrX) return -3;      /* x-resolution no multiple of uiNrX */
	if (uiYRes % uiNrY) return -4;      /* y-resolution no multiple of uiNrY */
	if (Max >= uiNR_OF_GREY) return -5;       /* maximum too large */
	if (Min >= Max) return -6;          /* minimum equal or larger than maximum */
	if (uiNrX < 2 || uiNrY < 2) return -7;/* at least 4 contextual regions required */
	if (fCliplimit <= 1.0) return 0;      /* is OK, immediately returns original image. */
	if (uiNrBins == 0) uiNrBins = 128;      /* default value when not specified */

	pulMapArray = new unsigned long[uiNrX*uiNrY*uiNrBins];
	if (pulMapArray == 0) return -8;      /* Not enough memory! (try reducing uiNrBins) */

	uiXSize = uiXRes/uiNrX; uiYSize = uiYRes/uiNrY;  /* Actual size of contextual regions */
	ulNrPixels = (unsigned long)uiXSize * (unsigned long)uiYSize;

	if(fCliplimit > 0.0) 
	{          /* Calculate actual cliplimit     */
		ulClipLimit = (unsigned long) (fCliplimit * (uiXSize * uiYSize) / uiNrBins);
		ulClipLimit = (ulClipLimit < 1UL) ? 1UL : ulClipLimit;
	}
	else ulClipLimit = 1UL<<14;          /* Large value, do not clip (AHE) */
	MakeLut(aLUT, Min, Max, uiNrBins);      /* Make lookup table for mapping of greyvalues */

	/* Calculate greylevel mappings for each contextual region */
	for (uiY = 0, pImPointer = pImage; uiY < uiNrY; uiY++) 
	{
		for (uiX = 0; uiX < uiNrX; uiX++, pImPointer += uiXSize) 
		{
			pulHist = &pulMapArray[uiNrBins * (uiY * uiNrX + uiX)];
			MakeHistogram(pImPointer,uiXRes,uiXSize,uiYSize,pulHist,uiNrBins,aLUT);
			ClipHistogram(pulHist, uiNrBins, ulClipLimit);
			MapHistogram(pulHist, Min, Max, uiNrBins, ulNrPixels);
		}
		pImPointer += (uiYSize - 1) * uiXRes;          /* skip lines, set pointer */
	}

	/* Interpolate greylevel mappings to get CLAHE image */
	for (pImPointer = pImage, uiY = 0; uiY <= uiNrY; uiY++) 
	{
		if (uiY == 0) 
		{                      /* special case: top row */
			uiSubY = uiYSize >> 1;  uiYU = 0; uiYB = 0;
		}
		else 
		{
			if (uiY == uiNrY) 
			{                  /* special case: bottom row */
				uiSubY = uiYSize >> 1;    uiYU = uiNrY-1;     uiYB = uiYU;
			}
			else 
			{                      /* default values */
				uiSubY = uiYSize; uiYU = uiY - 1; uiYB = uiYU + 1;
			}
		}
		for (uiX = 0; uiX <= uiNrX; uiX++) 
		{
			if (uiX == 0) 
			{                  /* special case: left column */
				uiSubX = uiXSize >> 1; uiXL = 0; uiXR = 0;
			}
			else 
			{
				if (uiX == uiNrX) 
				{              /* special case: right column */
					uiSubX = uiXSize >> 1;  uiXL = uiNrX - 1; uiXR = uiXL;
				}
				else 
				{                      /* default values */
					uiSubX = uiXSize; uiXL = uiX - 1; uiXR = uiXL + 1;
				}
			}
			pulLU = &pulMapArray[uiNrBins * (uiYU * uiNrX + uiXL)];
			pulRU = &pulMapArray[uiNrBins * (uiYU * uiNrX + uiXR)];
			pulLB = &pulMapArray[uiNrBins * (uiYB * uiNrX + uiXL)];
			pulRB = &pulMapArray[uiNrBins * (uiYB * uiNrX + uiXR)];
			Interpolate(pImPointer,uiXRes,pulLU,pulRU,pulLB,pulRB,uiSubX,uiSubY,aLUT);
			pImPointer += uiSubX;              /* set pointer on next matrix */
		}
		pImPointer += (uiSubY - 1) * uiXRes;
	}
	delete [] pulMapArray;                      /* free space for histograms */
	return 0;                          /* return status OK */
}

void ClipHistogram (unsigned long* pulHistogram, unsigned int uiNrGreylevels, unsigned long ulClipLimit)
	/* This function performs clipping of the histogram and redistribution of bins.
	* The histogram is clipped and the number of excess pixels is counted. Afterwards
	* the excess pixels are equally redistributed across the whole histogram (providing
	* the bin count is smaller than the cliplimit).
	*/
{
	unsigned long* pulBinPointer, *pulEndPointer, *pulHisto;
	unsigned long ulNrExcess, ulUpper, ulBinIncr, ulStepSize, i;
	long lBinExcess;

	ulNrExcess = 0;  pulBinPointer = pulHistogram;
	for (i = 0; i < uiNrGreylevels; i++) 
	{ /* calculate total number of excess pixels */
		lBinExcess = (long) pulBinPointer[i] - (long) ulClipLimit;
		if (lBinExcess > 0) ulNrExcess += lBinExcess;      /* excess in current bin */
	};

	/* Second part: clip histogram and redistribute excess pixels in each bin */
	ulBinIncr = ulNrExcess / uiNrGreylevels;          /* average binincrement */
	ulUpper =  ulClipLimit - ulBinIncr;     /* Bins larger than ulUpper set to cliplimit */

	for (i = 0; i < uiNrGreylevels; i++) 
	{
		if (pulHistogram[i] > ulClipLimit) pulHistogram[i] = ulClipLimit; /* clip bin */
		else 
		{
			if (pulHistogram[i] > ulUpper) 
			{        /* high bin count */
				ulNrExcess -= pulHistogram[i] - ulUpper; pulHistogram[i] = ulClipLimit;
				//ulNrExcess -= (ulClipLimit - pulHistogram[i]); pulHistogram[i] = ulClipLimit;
			}
			else 
			{                    /* low bin count */
				ulNrExcess -= ulBinIncr; pulHistogram[i] += ulBinIncr;
			}
		}
	}

	while (ulNrExcess) 
	{   /* Redistribute remaining excess  */
		pulEndPointer = &pulHistogram[uiNrGreylevels]; pulHisto = pulHistogram;

		while (ulNrExcess && pulHisto < pulEndPointer) 
		{
			ulStepSize = uiNrGreylevels / ulNrExcess;
			if (ulStepSize < 1) ulStepSize = 1;          /* stepsize at least 1 */
			for (pulBinPointer=pulHisto; pulBinPointer < pulEndPointer && ulNrExcess; pulBinPointer += ulStepSize) 
			{
				if (*pulBinPointer < ulClipLimit) 
				{
					(*pulBinPointer)++;     ulNrExcess--;      /* reduce excess */
				}
			}
			pulHisto++;          /* restart redistributing on other bin location */
		}
	}
}

void MakeHistogram (BYTE* pImage, unsigned int uiXRes, unsigned int uiSizeX, unsigned int uiSizeY, unsigned long* pulHistogram, unsigned int uiNrGreylevels, BYTE* pLookupTable)
	/* This function classifies the greylevels present in the array image into
	* a greylevel histogram. The pLookupTable specifies the relationship
	* between the greyvalue of the pixel (typically between 0 and 4095) and
	* the corresponding bin in the histogram (usually containing only 128 bins).
	*/
{
	BYTE* pImagePointer;
	unsigned int i;

	for (i = 0; i < uiNrGreylevels; i++) pulHistogram[i] = 0L; /* clear histogram */

	for (i = 0; i < uiSizeY; i++) 
	{
		pImagePointer = &pImage[uiSizeX];
		while (pImage < pImagePointer) pulHistogram[pLookupTable[*pImage++]]++;
		pImagePointer += uiXRes;
		pImage = &pImagePointer[-int(uiSizeX)];
	}
}

void MapHistogram (unsigned long* pulHistogram, BYTE Min, BYTE Max, unsigned int uiNrGreylevels, unsigned long ulNrOfPixels)
	/* This function calculates the equalized lookup table (mapping) by
	* cumulating the input histogram. Note: lookup table is rescaled in range [Min..Max].
	*/
{
	unsigned int i;  unsigned long ulSum = 0;
	const float fScale = ((float)(Max - Min)) / ulNrOfPixels;
	const unsigned long ulMin = (unsigned long) Min;

	for (i = 0; i < uiNrGreylevels; i++) 
	{
		ulSum += pulHistogram[i]; pulHistogram[i]=(unsigned long)(ulMin+ulSum*fScale);
		if (pulHistogram[i] > Max) pulHistogram[i] = Max;
	}
}

void MakeLut (BYTE * pLUT, BYTE Min, BYTE Max, unsigned int uiNrBins)
	/* To speed up histogram clipping, the input image [Min,Max] is scaled down to
	* [0,uiNrBins-1]. This function calculates the LUT.
	*/
{
	int i;
	const BYTE BinSize = (BYTE) (1 + (Max - Min) / uiNrBins);

	for (i = Min; i <= Max; i++)  pLUT[i] = (i - Min) / BinSize;
}

void Interpolate (BYTE * pImage, int uiXRes, unsigned long * pulMapLU, unsigned long * pulMapRU, unsigned long * pulMapLB,  unsigned long * pulMapRB, unsigned int uiXSize, unsigned int uiYSize, BYTE * pLUT)
	/* pImage      - pointer to input/output image
	* uiXRes      - resolution of image in x-direction
	* pulMap*     - mappings of greylevels from histograms
	* uiXSize     - uiXSize of image submatrix
	* uiYSize     - uiYSize of image submatrix
	* pLUT           - lookup table containing mapping greyvalues to bins
	* This function calculates the new greylevel assignments of pixels within a submatrix
	* of the image with size uiXSize and uiYSize. This is done by a bilinear interpolation
	* between four different mappings in order to eliminate boundary artifacts.
	* It uses a division; since division is often an expensive operation, I added code to
	* perform a logical shift instead when feasible.
	*/
{
	const unsigned int uiIncr = uiXRes-uiXSize; /* Pointer increment after processing row */
	BYTE GreyValue; unsigned int uiNum = uiXSize*uiYSize; /* Normalization factor */

	unsigned int uiXCoef, uiYCoef, uiXInvCoef, uiYInvCoef, uiShift = 0;

	if (uiNum & (uiNum - 1))   /* If uiNum is not a power of two, use division */
		for (uiYCoef = 0, uiYInvCoef = uiYSize; uiYCoef < uiYSize; uiYCoef++, uiYInvCoef--,pImage += uiIncr) 
		{
			for (uiXCoef = 0, uiXInvCoef = uiXSize; uiXCoef < uiXSize; uiXCoef++, uiXInvCoef--) 
			{
				GreyValue = pLUT[*pImage];           /* get histogram bin value */
				*pImage++ = (BYTE ) ((uiYInvCoef * (uiXInvCoef*pulMapLU[GreyValue] + uiXCoef * pulMapRU[GreyValue]) + uiYCoef * (uiXInvCoef * pulMapLB[GreyValue] + uiXCoef * pulMapRB[GreyValue])) / uiNum);
			}
		}
	else 
	{               /* avoid the division and use a right shift instead */
		while (uiNum >>= 1) uiShift++;           /* Calculate 2log of uiNum */
		for (uiYCoef = 0, uiYInvCoef = uiYSize; uiYCoef < uiYSize; uiYCoef++, uiYInvCoef--,pImage += uiIncr) 
		{
			for (uiXCoef = 0, uiXInvCoef = uiXSize; uiXCoef < uiXSize; uiXCoef++, uiXInvCoef--) 
			{
				GreyValue = pLUT[*pImage];      /* get histogram bin value */
				*pImage++ = (BYTE)((uiYInvCoef* (uiXInvCoef * pulMapLU[GreyValue] + uiXCoef * pulMapRU[GreyValue]) + uiYCoef * (uiXInvCoef * pulMapLB[GreyValue] + uiXCoef * pulMapRB[GreyValue])) >> uiShift);
			}
		}
	}
}

