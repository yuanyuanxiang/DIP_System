// ColorGradWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "ColorGradWnd.h"

// CColorGradWnd

IMPLEMENT_DYNAMIC(CColorGradWnd, CWnd)

CColorGradWnd::CColorGradWnd()
{
	m_pWndFont = NULL;
	m_pClientRect = NULL;
	m_pPaintRect = NULL;
	m_dataSrc = NULL;
	m_dataCur = NULL;
	for (int i = 0; i<4; i++)
	{
			m_pfHistogram[i] = NULL;
			m_V4Transform[i] = NULL;
	}
	//字体
	m_FontName = L"Arial";
	m_nFontSize = 90;

	m_nCurDrag = -1;//未拽动的标志
	m_nLastDrag = 0;//上次拽动点初值
	m_dMedian = 1;//中间调比率
	m_dOldRate = 1;
}

CColorGradWnd::~CColorGradWnd()
{
	if (m_pWndFont != NULL)        delete m_pWndFont;
	if (m_pClientRect != NULL)     delete m_pClientRect;
	if (m_pPaintRect != NULL)      delete m_pPaintRect;
	if (m_dataSrc != NULL)         delete [] m_dataSrc;
	if (m_dataCur != NULL)         delete [] m_dataCur;
	for (int i = 0; i<m_WndVector.size(); i++)
	{
		if (m_WndVector[i] != NULL) delete m_WndVector[i];
	}
	for (int i = 0; i<4; i++)
	{
		if (m_pfHistogram[i] != NULL) delete [] m_pfHistogram[i];
		if (m_V4Transform[i] != NULL) delete [] m_V4Transform[i];
	}
}

BEGIN_MESSAGE_MAP(CColorGradWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CREATE()
	//鼠标
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//按钮
	ON_CBN_SELCHANGE(ID_CG_WND_COMBO, &CColorGradWnd::OnChannelSelected)
	ON_BN_CLICKED(IDOK, &CColorGradWnd::OnClickOK)
	ON_BN_CLICKED(IDCANCEL, &CColorGradWnd::OnClickCancel)
	//编辑控件失去输入焦点
	ON_EN_KILLFOCUS(ID_CG_WND_EDIT_LEFTTOP, &CColorGradWnd::OnEditLeftTop)
	ON_EN_KILLFOCUS(ID_CG_WND_EDIT_TOP, &CColorGradWnd::OnEditTop)
	ON_EN_KILLFOCUS(ID_CG_WND_EDIT_RIGHTTOP, &CColorGradWnd::OnEditRightTop)
	ON_EN_KILLFOCUS(ID_CG_WND_EDIT_LEFTBOTTOM, &CColorGradWnd::OnEditLeftBottom)
	ON_EN_KILLFOCUS(ID_CG_WND_EDIT_RIGHTBOTTOM, &CColorGradWnd::OnEditRightBottom)
END_MESSAGE_MAP()

// CColorGradWnd 消息处理程序

void CColorGradWnd::OnPaint()
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

	//显示CEdit的值
	ShowEditValue();

	//水平方向颜色条
	for (int i = m_pPaintRect->left, k = 0; i<m_pPaintRect->right; i++, k++)
	{
		LineColor = ColorSetting(m_nChannelSelected, int(k/m_fWidthRatio), 1);
		newPen.CreatePen(PS_SOLID, 1, LineColor);
		pOldPen = MemDC.SelectObject(&newPen);
		MemDC.MoveTo(i, m_pClientRect->bottom-70);
		MemDC.LineTo(i, m_pClientRect->bottom-50);
		MemDC.SelectObject(pOldPen);
		newPen.DeleteObject();
	}

	//控制点1
	newPen.CreatePen(PS_SOLID, 1, RGB(200, 0, 0));
	pOldPen = MemDC.SelectObject(&newPen);
	DrawPoint(m_ptDark, 5, MemDC);
	MemDC.SelectObject(pOldPen);
	newPen.DeleteObject();
	//控制点2
	newPen.CreatePen(PS_SOLID, 1, RGB(0, 200, 0));
	pOldPen = MemDC.SelectObject(&newPen);
	DrawPoint(m_ptMedian, 5, MemDC);
	MemDC.SelectObject(pOldPen);
	newPen.DeleteObject();
	//控制点3
	newPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 200));
	pOldPen = MemDC.SelectObject(&newPen);
	DrawPoint(m_ptBright, 5, MemDC);
	MemDC.SelectObject(pOldPen);
	newPen.DeleteObject();
	//控制点4
	newPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	pOldPen = MemDC.SelectObject(&newPen);
	DrawPoint(m_ptMin, 5, MemDC);
	MemDC.SelectObject(pOldPen);
	newPen.DeleteObject();
	//控制点5
	newPen.CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
	pOldPen = MemDC.SelectObject(&newPen);
	DrawPoint(m_ptMax, 5, MemDC);
	MemDC.SelectObject(pOldPen);
	newPen.DeleteObject();

	if(m_pImage != NULL)//已经载入图像
	{
		//直方图
		LineColor = ColorSetting(m_nChannelSelected, 128);
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
	dc.BitBlt(m_pPaintRect->left-10, m_pPaintRect->bottom, m_nRectWidth+20, 10, &MemDC, m_pPaintRect->left-10, m_pPaintRect->bottom, SRCCOPY);//3控制点
	dc.BitBlt(m_pPaintRect->left-10, m_pClientRect->bottom-70, m_nRectWidth+20, 30, &MemDC, m_pPaintRect->left-10, m_pClientRect->bottom-70, SRCCOPY);//2控制点和颜色条
	MemDC.SelectObject(pOldBmp);
	pOldBmp->DeleteObject();
	MemBitmap.DeleteObject();
}

void CColorGradWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	GetClientRect(m_pClientRect);

	m_pPaintRect->right = m_pClientRect->right-70;
	m_pPaintRect->bottom = m_pClientRect->bottom-150;

	m_nRectWidth = m_pPaintRect->Width();
	m_nRectHeight = m_pPaintRect->Height();
	m_fWidthRatio = (double)m_nRectWidth/255.0;
	m_fHeightRatio = (double)m_nRectHeight/255.0;

	//确定
	if (m_pBnOK->GetSafeHwnd()) m_pBnOK->MoveWindow(m_pClientRect->right-50, 20, 40, 20);

	//取消
	if (m_pBnCancel->GetSafeHwnd()) m_pBnCancel->MoveWindow(m_pClientRect->right-50, 60, 40, 20);

	//GroupBox
	if (m_pBnGroupBox->GetSafeHwnd()) m_pBnGroupBox->MoveWindow(10, 20, m_nRectWidth+20, m_pClientRect->Height()-20);

	//输入色阶
	if (m_pStaticInput->GetSafeHwnd()) m_pStaticInput->MoveWindow(20, 40, 60, 20);

	//RGB组合框, 组合框下拉列表高度在此设置
	if (m_pComboBoxRGB->GetSafeHwnd()) m_pComboBoxRGB->MoveWindow(50, 15, 90, 100);

	//左上角的编辑框
	if (m_pEditLeftTop->GetSafeHwnd()) m_pEditLeftTop->MoveWindow(m_pPaintRect->left, m_pPaintRect->bottom+20, 40, 20);

	//正上方的编辑框
	if (m_pEditTop->GetSafeHwnd()) m_pEditTop->MoveWindow(int(m_pPaintRect->left+m_pPaintRect->Width()/2.0-20), m_pPaintRect->bottom+20, 40, 20);

	//右上角的编辑框
	if (m_pEditRightTop->GetSafeHwnd()) m_pEditRightTop->MoveWindow(m_pPaintRect->right-40, m_pPaintRect->bottom+20, 40, 20);

	//左下角的编辑框
	if (m_pEditLeftBottom->GetSafeHwnd()) m_pEditLeftBottom->MoveWindow(m_pPaintRect->left, m_pClientRect->bottom-40, 40, 20);

	//右下角的编辑框
	if (m_pEditRightBottom->GetSafeHwnd()) m_pEditRightBottom->MoveWindow(m_pPaintRect->right-40, m_pClientRect->bottom-40, 40, 20);

	//输出色阶
	if (m_pStaticOutput->GetSafeHwnd()) m_pStaticOutput->MoveWindow(m_pPaintRect->left+10, m_pPaintRect->bottom+60, 60, 20);

	//控制点
	UpdateControls(-1);

	UpdateWindow();
}

void CColorGradWnd::UpdateControls(int number)
{
	switch (number)
	{
	case -1:
		m_ptDark = CPoint(m_pPaintRect->left+int(m_nRectWidth*m_dRatio[0]), m_pPaintRect->bottom);
		m_ptMedian = CPoint(m_pPaintRect->left+int(m_nRectWidth*m_dRatio[1]), m_pPaintRect->bottom);
		m_ptBright = CPoint(m_pPaintRect->left+int(m_nRectWidth*m_dRatio[2]), m_pPaintRect->bottom);
		m_ptMin = CPoint(m_pPaintRect->left+int(m_nRectWidth*m_dRatio[3]), m_pClientRect->bottom-50);
		m_ptMax = CPoint(m_pPaintRect->left+int(m_nRectWidth*m_dRatio[4]), m_pClientRect->bottom-50);
		break;
	case 0:
		*m_ptArray[0] = CPoint(m_pPaintRect->left+int(m_nRectWidth*m_dRatio[0]), m_pPaintRect->bottom);
		break;
	case 1:
		*m_ptArray[1] = CPoint(m_pPaintRect->left+int(m_nRectWidth*m_dRatio[1]), m_pPaintRect->bottom);
		break;
	case 2:
		*m_ptArray[2] = CPoint(m_pPaintRect->left+int(m_nRectWidth*m_dRatio[2]), m_pPaintRect->bottom);
		break;
	case 3:
		*m_ptArray[3] = CPoint(m_pPaintRect->left+int(m_nRectWidth*m_dRatio[3]), m_pClientRect->bottom-50);
		break;
	case 4:
		*m_ptArray[4] = CPoint(m_pPaintRect->left+int(m_nRectWidth*m_dRatio[4]), m_pClientRect->bottom-50);
		break;
	default:
		break;
	}
}

int CColorGradWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//字体
	m_pWndFont = new CFont;
	m_pWndFont->CreatePointFont(m_nFontSize, m_FontName);

	m_pClientRect = new CRect;
	GetClientRect(m_pClientRect);

	//绘图区
	m_pPaintRect = new CRect(20, 60, m_pClientRect->right-70, m_pClientRect->bottom-150);

	//宽高相对于255缩放比
	m_nChannelSelected = 0;
	m_nRectWidth = m_pPaintRect->Width();
	m_nRectHeight = m_pPaintRect->Height();
	m_fWidthRatio = m_nRectWidth/255.0;
	m_fHeightRatio = m_nRectHeight/255.0;

	//确定
	m_pBnOK = new CMFCButton;
	m_pBnOK->Create(L"确定", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(m_pClientRect->right-50, 20, m_pClientRect->right-10, 40), this, IDOK);
	m_pBnOK->SetFont(m_pWndFont, TRUE);

	//取消
	m_pBnCancel = new CMFCButton;
	m_pBnCancel->Create(L"取消", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(m_pClientRect->right-50, 60, m_pClientRect->right-10, 80), this, IDCANCEL);
	m_pBnCancel->SetFont(m_pWndFont, TRUE);

	//上面的GroupBox
	m_pBnGroupBox = new CButton;
	m_pBnGroupBox->Create(L"通道:", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, CRect(10, 20, m_pClientRect->right - 60, m_pClientRect->bottom - 10), this, ID_CG_WND_GROUP_UP);
	m_pBnGroupBox->SetFont(m_pWndFont, TRUE);

	//输入色阶
	m_pStaticInput = new CStatic;
	m_pStaticInput->Create(L"输入色阶:", WS_CHILD | WS_VISIBLE, CRect(20, 40, 80, 60), this, ID_CG_WND_INPUT);
	m_pStaticInput->SetFont(m_pWndFont, TRUE);

	//RGB组合框
	m_pComboBoxRGB = new CComboBox;
	m_pComboBoxRGB->Create(WS_CHILD | WS_VISIBLE | CBS_AUTOHSCROLL | CBS_DROPDOWNLIST | WS_EX_TOPMOST, CRect(50, 15, 90, 35), this, ID_CG_WND_COMBO);
	m_pComboBoxRGB->InsertString(0, L"RGB");
	m_pComboBoxRGB->InsertString(1, L"红");
	m_pComboBoxRGB->InsertString(2, L"绿");
	m_pComboBoxRGB->InsertString(3, L"蓝");
	m_pComboBoxRGB->SetCurSel(0);
	m_pComboBoxRGB->SetFont(m_pWndFont, TRUE);

	//左上角的编辑框
	m_pEditLeftTop = new CEdit;
	m_pEditLeftTop->Create(WS_CHILD | WS_VISIBLE | ES_NUMBER, CRect(m_pPaintRect->left, m_pPaintRect->bottom + 20, m_pPaintRect->left + 40, m_pPaintRect->bottom + 40), this, ID_CG_WND_EDIT_LEFTTOP);
	m_pEditLeftTop->SetFont(m_pWndFont, TRUE);

	//正上方的编辑框
	m_pEditTop = new CEdit;
	m_pEditTop->Create(WS_CHILD | WS_VISIBLE, CRect(int(m_pPaintRect->left + m_pPaintRect->Width() / 2.0 - 20), m_pPaintRect->bottom + 20, int(m_pPaintRect->left + m_pPaintRect->Width() / 2.0 + 20), m_pPaintRect->bottom + 40), this, ID_CG_WND_EDIT_TOP);
	m_pEditTop->SetFont(m_pWndFont, TRUE);

	//右上角的编辑框
	m_pEditRightTop = new CEdit;
	m_pEditRightTop->Create(WS_CHILD | WS_VISIBLE | ES_NUMBER, CRect(m_pPaintRect->right - 40, m_pPaintRect->bottom + 20, m_pPaintRect->right, m_pPaintRect->bottom + 40), this, ID_CG_WND_EDIT_RIGHTTOP);
	m_pEditRightTop->SetFont(m_pWndFont, TRUE);

	//左下角的编辑框
	m_pEditLeftBottom = new CEdit;
	m_pEditLeftBottom->Create(WS_CHILD | WS_VISIBLE | ES_NUMBER, CRect(m_pPaintRect->left, m_pClientRect->bottom - 40, m_pPaintRect->left + 40, m_pClientRect->bottom - 20), this, ID_CG_WND_EDIT_LEFTBOTTOM);
	m_pEditLeftBottom->SetFont(m_pWndFont, TRUE);

	//右下角的编辑框
	m_pEditRightBottom = new CEdit;
	m_pEditRightBottom->Create(WS_CHILD | WS_VISIBLE | ES_NUMBER, CRect(m_pPaintRect->right - 40, m_pClientRect->bottom - 40, m_pPaintRect->right, m_pClientRect->bottom - 20), this, ID_CG_WND_EDIT_RIGHTBOTTOM);
	m_pEditRightBottom->SetFont(m_pWndFont, TRUE);

	//输出色阶
	m_pStaticOutput = new CStatic;
	m_pStaticOutput->Create(L"输出色阶:", WS_CHILD | WS_VISIBLE, CRect(m_pPaintRect->left + 10, m_pPaintRect->bottom + 60, m_pPaintRect->left + 70, m_pPaintRect->bottom + 80), this, ID_CG_WND_OUTPUT);
	m_pStaticOutput->SetFont(m_pWndFont, TRUE);

	//控制点
	m_ptDark = CPoint(m_pPaintRect->left, m_pPaintRect->bottom);
	m_ptMedian = CPoint(int(m_pPaintRect->left+m_nRectWidth/2.0-20), m_pPaintRect->bottom);
	m_ptBright = CPoint(m_pPaintRect->right, m_pPaintRect->bottom);
	m_ptMin = CPoint(m_pPaintRect->left, m_pClientRect->bottom-50);
	m_ptMax = CPoint(m_pPaintRect->right, m_pClientRect->bottom-50);
	m_ptArray[0] = &m_ptDark;
	m_ptArray[1] = &m_ptMedian;
	m_ptArray[2] = &m_ptBright;
	m_ptArray[3] = &m_ptMin;
	m_ptArray[4] = &m_ptMax;
	//比率
	m_dRatio[0] = 0.0;
	m_dRatio[1] = 0.5;
	m_dRatio[2] = 1.0;
	m_dRatio[3] = 0.0;
	m_dRatio[4] = 1.0;
	//Value
	m_dValue[0] = 0;
	m_dValue[1] = 127;
	m_dValue[2] = 255;
	m_dValue[3] = 0;
	m_dValue[4] = 255;

	m_WndVector.push_back(m_pBnOK);
	m_WndVector.push_back(m_pBnCancel);
	m_WndVector.push_back(m_pBnGroupBox);
	m_WndVector.push_back(m_pStaticInput);
	m_WndVector.push_back(m_pComboBoxRGB);
	m_WndVector.push_back(m_pEditLeftTop);
	m_WndVector.push_back(m_pEditTop);
	m_WndVector.push_back(m_pEditRightTop);
	m_WndVector.push_back(m_pEditLeftBottom);
	m_WndVector.push_back(m_pEditRightBottom);
	m_WndVector.push_back(m_pStaticOutput);

	//获取主程序位图指针
	m_pImage = GetDocument()->DIBImage();

	//初始化变换函数
	for (int i = 0; i<4; i++)
	{
		m_V4Transform[i] = new int[256];
		for (int j = 0; j<256; j++)
		{
			m_V4Transform[i][j] = j;
		}
	}

	//初始化直方图
	for (int i = 0; i<4; i++)
	{
		m_pfHistogram[i] = new double[256];
		for (int j = 0; j<256; j++)
		{
			m_pfHistogram[i][j] = 0;
		}
	}

	//获取直方图
	GetHistogram();

	return 0;
}

//获取活动文档，通过指针能访问其公有成员
CDIP_SystemDoc* CColorGradWnd::GetDocument()
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd *)AfxGetApp()->GetMainWnd();
	CMDIChildWnd* pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();
	return (CDIP_SystemDoc *)pChild->GetActiveDocument();
}

//获取活动视图，通过指针能访问其公有成员
CDIP_SystemView* CColorGradWnd::GetActiveView()
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd *)AfxGetApp()->GetMainWnd();
	CMDIChildWnd* pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();
	return (CDIP_SystemView *)pChild->GetActiveView();
}

//获得位图的直方图数据，以便于绘制
BOOL CColorGradWnd::GetHistogram()
{
	if (m_pImage == NULL) return FALSE;

	//获取图像的基本信息
	m_nImageWidth = m_pImage->GetWidth();
	m_nImageHeight = m_pImage->GetHeight();
	m_nChannel = m_pImage->GetBPP()/8;
	m_nRowlen = (m_nImageWidth*8*m_nChannel+31)/32*4;
	m_nlenData = m_nRowlen*m_nImageHeight;

	//将图像源数据存储起来
	if (m_dataSrc == NULL) m_dataSrc = new BYTE[m_nlenData];
	else return FALSE;
	if (m_dataCur == NULL) m_dataCur = new BYTE[m_nlenData];
	else return FALSE;

	//获得图像数据首地址，并拷贝数据
	BYTE* pBits =(BYTE *)m_pImage->GetBits()+(m_pImage->GetPitch()*(m_nImageHeight-1));
	memcpy(m_dataSrc, pBits, m_nlenData);
	memcpy(m_dataCur, pBits, m_nlenData);

	//计算直方图
	BYTE *pixel;
	if (m_nChannel>=3)
	{//3、4通道彩色图像
		for (int i = 0; i<m_nImageHeight; i++)
		{
			for (int j = 0; j<m_nImageWidth; j++)
			{//RGB24各分量的排列顺序为：BGR, RGB32各分量的排列顺序为：BGRA
				pixel = m_dataSrc + i * m_nRowlen + j * m_nChannel;
				m_pfHistogram[3][int(*pixel)]++;                                              //Blue
				m_pfHistogram[2][int(*(pixel+1))]++;                                          //Green
				m_pfHistogram[1][int(*(pixel+2))]++;                                          //Red
				if (*pixel == *(pixel+1) && *(pixel+1) == *(pixel+2))
				{//解决某些图片的RGB通道灰度值缺失
					m_pfHistogram[0][int(*pixel)]++;
				}
				else m_pfHistogram[0][int(*pixel*0.299 + *(pixel+1)*0.587 + *(pixel+2)*0.114)]++;  //RGB
			}
		}
	}
	else if(m_nChannel == 1)
	{//单通道灰度图像
		for (int i = 0; i<m_nImageHeight; i++)
		{
			for (int j = 0; j<m_nImageWidth; j++)
			{
				pixel = m_dataSrc + i * m_nRowlen + j * m_nChannel;
				for (int k = 0; k<4; k++) m_pfHistogram[k][int(*pixel)]++;
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
		}
	}

	return TRUE;
}

//改变了控制点，进行灰度变换，例如改变黑暗程度
BOOL CColorGradWnd::GrayTransform()
{
	if (m_pImage == NULL) return FALSE;

	memcpy(m_dataCur, m_dataSrc, m_nlenData);

	int i, j, k;
	BYTE* pixel;
	if (m_nChannelSelected == 0)
	{//RGB
		for (j = 0; j<m_nImageHeight; j++)
		{
			for (i = 0; i<m_nImageWidth; i++)
			{
				pixel = m_dataCur + j * m_nRowlen + i * m_nChannel;
				for (k = 0; k<m_nChannel; k++)
				{
					*(pixel+k) = m_V4Transform[m_nChannelSelected][*(pixel+k)];
				}
			}
		}
	}
	else
	{
		for (j = 0; j<m_nImageHeight; j++)
		{
			for (i = 0; i<m_nImageWidth; i++)
			{
				pixel = m_dataCur + j * m_nRowlen + i * m_nChannel;
				*(pixel+3-m_nChannelSelected) = m_V4Transform[m_nChannelSelected][*(pixel+3-m_nChannelSelected)];
			}
		}
	}
	//获得图像数据首地址，并拷贝数据
	BYTE* pBits =(BYTE *)m_pImage->GetBits()+(m_pImage->GetPitch()*(m_nImageHeight-1));
	memcpy(pBits, m_dataCur, m_nlenData);
	return TRUE;
}

//样条函数:插值出[0,255]的灰度变换（输入/输出）
void CColorGradWnd::SplineFunc()
{
	//色阶的本质还是曲线，但是它是借助了直方图的直观，它相当于
	//只有三个控制点的曲线，分别表示暗影、中间调、高光，一般的
	//图片处理做法是，先用色阶粗调，再用曲线细调
	//以下是为了计算中间的控制点
	//0            b
	//1         (a+b)/2
	//10           a
	if (m_pImage == NULL) return;
	double &a = m_dValue[0], &b = m_dValue[2], Med = (a+b)/2.0;
	double In, Out;
	In  = m_dMedian;
	if (In<1) Out = (In-1)*(a-b)/2+Med;
	else      Out = (In-1)*(a-b)/18+Med;
	//Out = -2*(a-b)/45*In*In + 49*(a-b)/90*In + b;

	double Node[3], Value[3], x0[256], y0[256];
	double medX = Out;
	double medY = (m_dValue[3]+m_dValue[4])/2.0;
	//头
	Node[0] = 0;    Value[0] = 0;
	//中间
	Node[1] = medX; Value[1] = medY;
	//尾
	Node[2] = 255;  Value[2] = 255;

	if (Out<0)       Out = 0;
	else if(Out>255) Out = 255;

	for (int i = 0; i<256; i++) x0[i] = double(i);
	Spline(Node, Value, x0, y0, 3, 256, 0, 0);
	for (int i = 0; i<256; i++)
	{
		m_V4Transform[m_nChannelSelected][i] = int(y0[i]);
		if(m_V4Transform[m_nChannelSelected][i]<0)
		{
			m_V4Transform[m_nChannelSelected][i] = 0;
			continue;
		}
		if(m_V4Transform[m_nChannelSelected][i]>255)
		{
			m_V4Transform[m_nChannelSelected][i] = 255;
			continue;
		}
	}
	return;
}

//显示CEdit控件的内容
void CColorGradWnd::ShowEditValue()
{
	CString text;
	for (int i = 5; i<10; i++)//CEdit在CWnd中的排序
	{
		if(i != 6) text.Format(L"%3.0f", m_dValue[i-5]);
		else       text.Format(L"%3.2f", m_dMedian);
		m_WndVector[i]->SetWindowText(text);
	}
}

// *** 消息响应 ***

void CColorGradWnd::OnChannelSelected()
{
	m_nChannelSelected = m_pComboBoxRGB->GetCurSel();
	InvalidateRect(m_pPaintRect);//直方图
	InvalidateRect(CRect(m_pPaintRect->left, m_pClientRect->bottom-70, m_pPaintRect->right, m_pClientRect->bottom-50));//颜色条
}

void CColorGradWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();//获得输入焦点，使得CEdit失去输入焦点
	for (int i = m_nLastDrag, j = 0, k; j<5; i++, j++)
	{
		k = i>4 ? i%5 : i;
		if (fabs(1.0*point.x-m_ptArray[k]->x)<5 && fabs(1.0*point.y-m_ptArray[k]->y)<5)
		{//看看用户想拽动哪个控制点
			m_nCurDrag = k;
			m_nLastPos = m_nCurPos = point.x;
			SetTimer(1, 1000, NULL);//设定计时器1
			break;
		}
	}
	CWnd::OnLButtonDown(nFlags, point);
}

void CColorGradWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_nCurDrag != -1)
	{
		SplineFunc();
		GrayTransform();
		GetActiveView()->Invalidate(TRUE);
		m_nLastDrag = m_nCurDrag;
		m_nCurDrag = -1;
		KillTimer(1);
	}
	CWnd::OnLButtonUp(nFlags, point);
}

void CColorGradWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_nCurDrag != -1)
	{
		int err = point.x-m_pPaintRect->left;
		if (err<0 || point.x>m_pPaintRect->right) return;
		double tmp = 1.0*err/m_fWidthRatio;
		switch (m_nCurDrag)
		{//控制点受到限制
		case 0:
			if (tmp>=m_dValue[1]) return;
			break;
		case 1:
			if (tmp<=m_dValue[0]) return;
			if (tmp>=m_dValue[2]) return;
			break;
		case 2:
			if (tmp<=m_dValue[1]) return;
			break;
		default:
			break;
		}
		m_nCurPos = point.x;
		m_dValue[m_nCurDrag] = tmp;                    //改变CEdit显示值
		m_dRatio[m_nCurDrag] = 1.0*err/m_nRectWidth;   //改变相对绘图区宽的比率
		m_ptArray[m_nCurDrag]->x = m_nCurPos;//y值不改变，仅改变x,用于显示
		double &a = m_dValue[0], &b = m_dValue[2], &x = m_dValue[1], Med = (a+b)/2.0;
		if (m_nCurDrag == 1)
		{//最上面一排中间的CEdit控件
			if (x<Med) m_dMedian = pow(10.0, 2/(a-b)*(x-a)+1);//映射成[10,1]
			else       m_dMedian = pow(10.0, 4/(a-b)*(x-b)-2);//映射成[1,0.01]
		}
		else if(m_nCurDrag == 0 || m_nCurDrag == 2)
		{//拖动上面两个端点，中间点跟着变
			x = (b+a*m_dOldRate)/(1+m_dOldRate);
			m_dRatio[1] = x*m_fWidthRatio/m_nRectWidth;
			m_ptArray[1]->x = m_pPaintRect->left+x*m_fWidthRatio;
			UpdateControls(1);
		}
		m_dOldRate = (b-x)/(x-a);
		InvalidateRect(CRect(m_pPaintRect->left-10, m_pPaintRect->bottom, m_pPaintRect->right+10, m_pPaintRect->bottom+10));
		InvalidateRect(CRect(m_pPaintRect->left-10, m_pClientRect->bottom-50, m_pPaintRect->right+10, m_pClientRect->bottom-40));
	}
	CWnd::OnMouseMove(nFlags, point);
}

//定时进行灰度变换和刷新图像
void CColorGradWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		if (fabs(1.0*m_nCurPos-m_nLastPos)>5)
		{//在SetTimer设定的间隔内若鼠标拖动的像素少于5就不执行下述语句
			SplineFunc();
			GrayTransform();
			GetActiveView()->Invalidate(TRUE);
		}
		m_nLastPos = m_nCurPos;
	}
	CWnd::OnTimer(nIDEvent);
}

void CColorGradWnd::OnClickOK()
{
	//直接关闭
	::SendMessage(GetParent()->GetSafeHwnd(), WM_CLOSE, 0, 0);
}

void CColorGradWnd::OnClickCancel()
{
	//恢复数据后关闭
	if (m_pImage != NULL)
	{
		BYTE* pBits =(BYTE *)m_pImage->GetBits()+(m_pImage->GetPitch()*(m_nImageHeight-1));
		memcpy(pBits, m_dataSrc, m_nlenData);
		GetActiveView()->Invalidate(TRUE);
	}
	::SendMessage(GetParent()->GetSafeHwnd(), WM_CLOSE, 0, 0);
}

void CColorGradWnd::OnEditLeftTop()
{
	UpdateData(TRUE);
	UpdateControls(0);
	double &a = m_dValue[0], &b = m_dValue[2], &x = m_dValue[1], Med = (a+b)/2.0;
	x = (b+a*m_dOldRate)/(1+m_dOldRate);
	m_dRatio[1] = x*m_fWidthRatio/m_nRectWidth;
	m_ptArray[1]->x = m_pPaintRect->left+x*m_fWidthRatio;
	InvalidateRect(CRect(m_pPaintRect->left-10, m_pPaintRect->bottom, m_pPaintRect->right+10, m_pPaintRect->bottom+10));
}

void CColorGradWnd::OnEditTop()
{
	UpdateData(TRUE);
	UpdateControls(1);
	InvalidateRect(CRect(m_pPaintRect->left-10, m_pPaintRect->bottom, m_pPaintRect->right+10, m_pPaintRect->bottom+10));
}

void CColorGradWnd::OnEditRightTop()
{
	UpdateData(TRUE);
	UpdateControls(2);
	double &a = m_dValue[0], &b = m_dValue[2], &x = m_dValue[1], Med = (a+b)/2.0;
	x = (b+a*m_dOldRate)/(1+m_dOldRate);
	m_dRatio[1] = x*m_fWidthRatio/m_nRectWidth;
	m_ptArray[1]->x = m_pPaintRect->left+x*m_fWidthRatio;
	InvalidateRect(CRect(m_pPaintRect->left-10, m_pPaintRect->bottom, m_pPaintRect->right+10, m_pPaintRect->bottom+10));
}

void CColorGradWnd::OnEditLeftBottom()
{
	UpdateData(TRUE);
	UpdateControls(3);
	InvalidateRect(CRect(m_pPaintRect->left-10, m_pClientRect->bottom-50, m_pPaintRect->right+10, m_pClientRect->bottom-40));
}

void CColorGradWnd::OnEditRightBottom()
{
	UpdateData(TRUE);
	UpdateControls(4);
	InvalidateRect(CRect(m_pPaintRect->left-10, m_pClientRect->bottom-50, m_pPaintRect->right+10, m_pClientRect->bottom-40));
}

//编辑框的数据交换
void CColorGradWnd::DoDataExchange(CDataExchange* pDX)
{
	double Temp[5];
	for (int i = 0; i<5; i++)
	{//储存源数据
		if (i != 1) Temp[i] = m_dValue[i];
		else        Temp[1] = m_dMedian;
	}
	DDX_Text(pDX, ID_CG_WND_EDIT_LEFTTOP, m_dValue[0]);
	DDX_Text(pDX, ID_CG_WND_EDIT_TOP, m_dMedian);
	DDX_Text(pDX, ID_CG_WND_EDIT_RIGHTTOP, m_dValue[2]);
	DDX_Text(pDX, ID_CG_WND_EDIT_LEFTBOTTOM, m_dValue[3]);
	DDX_Text(pDX, ID_CG_WND_EDIT_RIGHTBOTTOM, m_dValue[4]);
	//看数据是否合法
	int flag = 1;
	if (!(0<=m_dValue[0] && m_dValue[0]<=m_dValue[2]))   flag = 0;
	if (!(0.01<=m_dMedian && m_dMedian<=10))             flag = 0;
	if (!(m_dValue[0]<=m_dValue[2] && m_dValue[2]<=255)) flag = 0;
	if (!(0<=m_dValue[3] && m_dValue[3]<=255))           flag = 0;
	if (!(0<=m_dValue[4] && m_dValue[4]<=255))           flag = 0;

	if (flag == 0)//数据不合法,恢复数据
	{
		for (int i = 0; i<5; i++)
		{
			if (i != 1) m_dValue[i] = Temp[i];
			else        m_dMedian   = Temp[1];
		}
		return;
	}

	double &a = m_dValue[0], &b = m_dValue[2], &x = m_dValue[1];
	for (int i = 0; i<5; i++)
	{
		if (i != 1)
		{
			m_dRatio[i] = m_dValue[i]*m_fWidthRatio/m_nRectWidth;
		}
		else
		{//中间点
			if (m_dMedian<1) x = (log10(m_dMedian)+2)*(a-b)/4+b;
			else             x = (log10(m_dMedian)-1)*(a-b)/2+a;
			m_dRatio[1] = x*m_fWidthRatio/m_nRectWidth;
			m_ptArray[1]->x = m_pPaintRect->left+x*m_fWidthRatio;
		}
	}
	CWnd::DoDataExchange(pDX);
}

//
// *** 其他 ***
//
//本来是设置通道颜色,为了使颜色条也能使用这个函数,WhereUse此参数仅仅为了节省代码量
COLORREF CColorGradWnd::ColorSetting(int Channel, int Strength, int WhereUse)
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

void CColorGradWnd::DrawPoint(CPoint& pt, int size, CDC& MemDC)
{
	for (int i = 0; i<size; i++)
	{
		MemDC.MoveTo(pt.x-i, pt.y+i);
		MemDC.LineTo(pt.x+i, pt.y+i);
	}
}

//
// *** 三次样条函数 ***
//

void CColorGradWnd::Spline(double* Node, double* Value, double* x0, double* s, int n, int m, double c1, double c2, int CASE)
{
	//Node:结点
	//Value:结点的值
	//x0:插值节点
	//s:插出来的值
	//n:结点个数
	//m:插值节点数
	//CASE：边界条件的情形
	//c1：边界条件1
	//c2：边界条件2
	double *u  = new double[n],
		*Lamda = new double[n],
		*d     = new double[n],
		*Mj    = new double[n],
		*h     = new double[n],
		*f1    = new double[n];
	double temp, f2;
	int i, j, sta = 0;
	for(j = 1; j<n; j++)
	{
		h[j-1] = Node[j]-Node[j-1];
		f1[j-1] = (Value[j]-Value[j-1])/h[j-1];
	}
	u[0] = 0;
	Lamda[0] = 0;
	d[0] = 0;
	for(j = 1; j<n-1; j++)
	{
		u[j] = h[j-1]/(h[j-1]+h[j]);
		Lamda[j] = h[j]/(h[j-1]+h[j]);
		f2 = (f1[j]-f1[j-1])/(Node[j+1]-Node[j-1]);
		d[j] = 6*f2;
	}

	//边界条件
	switch(CASE)
	{
	case 1:
		u[j] = 1;
		Lamda[0] = 1;
		d[0] = 6*(f1[0]-c1)/h[0];
		d[j] = 6*(c2-f1[n-2])/h[n-2];
		break;
	case 2:
		u[j] = 0;
		Lamda[0] = 0;
		d[0] = 2*c1;
		d[j] = 2*c2;
		break;
	default:
		break;
	}

	double* b = new double[n];
	for(i = 0; i<n; i++)
		b[i] = 2.0;
	u[0] = 0;
	Lamda[j] = 0;
	//追赶法求解
	Chase(u, b, Lamda, d, Mj, n);
	int k = 0;
	for (i = 0; i<m; i++)
	{
		if(x0[i]<Node[0])
		{
			j = 0;
			temp = Mj[j]*pow(Node[j+1]-x0[i],3)/(6*h[j])+Mj[j+1]*pow(x0[i]-Node[j],3)/(6*h[j])+(Value[j]-Mj[j]*pow(h[j],2)/6)*(Node[j+1]-x0[i])/h[j]+(Value[j+1]-Mj[j+1]*pow(h[j],2)/6)*(x0[i]-Node[j])/h[j];
			s[k++] = temp;
			continue;
		}
		if(x0[i]>Node[n-1])
		{
			j = n-2;
			temp = Mj[j]*pow(Node[j+1]-x0[i],3)/(6*h[j])+Mj[j+1]*pow(x0[i]-Node[j],3)/(6*h[j])+(Value[j]-Mj[j]*pow(h[j],2)/6)*(Node[j+1]-x0[i])/h[j]+(Value[j+1]-Mj[j+1]*pow(h[j],2)/6)*(x0[i]-Node[j])/h[j];
			s[k++] = temp;
			continue;
		}
		for(j = 0; j<n-1; j++)
		{
			if(Node[j]<=x0[i] && x0[i]<=Node[j+1])
			{
				temp= Mj[j]*pow(Node[j+1]-x0[i],3)/(6*h[j])+Mj[j+1]*pow(x0[i]-Node[j],3)/(6*h[j])+(Value[j]-Mj[j]*pow(h[j],2)/6)*(Node[j+1]-x0[i])/h[j]+(Value[j+1]-Mj[j+1]*pow(h[j],2)/6)*(x0[i]-Node[j])/h[j];
				s[k++] = temp;
				break;
			}
		}
	}

	delete [] u;
	delete [] Lamda;
	delete [] d;
	delete [] Mj;
	delete [] h;
	delete [] f1;
	delete [] b;
}

//追赶法
void CColorGradWnd::Chase(double* a, double* b, double* c, double* f, double* x, int n)
{
	//a,b,c：三对角线元素
	//x：方程的解
	//n：矩阵的行
	int i;
	double* belta = new double[n];
	double tmp;
	//追的过程
	belta[0] = c[0]/b[0];
	x[0] = f[0]/b[0];
	for(i = 1; i<n-1; i++)
	{
		tmp = b[i]-a[i]*belta[i-1];
		belta[i] = c[i]/tmp;
		x[i] = (f[i]-a[i]*x[i-1])/tmp;
	}
	tmp = b[i]-a[i]*belta[i-1];
	x[i] = (f[i]-a[i]*x[i-1])/tmp;
	//赶的过程
	for(i = n-2; i>=0; i--)
	{
		x[i] = x[i]-belta[i]*x[i+1];
	}
	delete [] belta;
}

