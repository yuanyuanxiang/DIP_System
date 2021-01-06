// ************************************************************************
//  �ļ�����SplitProc.cpp
//
//  ͼ��ָ�����⣺
//
//  RgnGrow()				- ��������
//	ContourDIB()			- ������ȡ
//	TraceDIB()				- �߽����
//	Sobel()					- ���������ӵı�Ե���
//	Roberts()				- �޲��ؽ������ӵı�Ե���
//	Prewitt()				- ����ά�����ӵı�Ե���
//	Kirsch()				- Kirsch���ӵı�Ե���
//	Laplacian()				- ������˹���ӵı�Ե���
//	GuassLaplacian()		- ��˹����������˹���ӵı�Ե���
//	Hough()					- ����任���Բ��(r = 40) 
//	Threshold()				- ��ֵ������ָ�
//
//
// ************************************************************************

#include "stdafx.h"
#include "DIP_System.h"
#include "SplitProc.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitProc

CSplitProc::CSplitProc()
{
}

CSplitProc::CSplitProc(CDIB* pDIB)
{
	m_pDIB = pDIB;
}

CSplitProc::~CSplitProc()
{
}

BEGIN_MESSAGE_MAP(CSplitProc, CWnd)
	//{{AFX_MSG_MAP(CSplitProc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
//							����ģ�鿪ʼ								  //
////////////////////////////////////////////////////////////////////////////

/*************************************************************************
 *
 * �������ƣ�
 *   RgnGrow()
 *
 * ����:
 *   HDIB hDIB          - �������DIB
 *   COLORREF clrSeed_I - ���ӵ�һ��RGBֵ
 *   COLORREF clrSeed_II- ���ӵ����RGBֵ
 *
 * ����ֵ:
 *   void				- �޷���ֵ
 *
 * ˵��:
 *   �ú�������ɫλͼת��Ϊ�Ҷ�ͼ��
 *
 ************************************************************************/
void CSplitProc::RgnGrow(COLORREF clrSeed_I, COLORREF clrSeed_II)
{
	
	LONG i;
	LONG j;

	// �������ӵ�һ�ĻҶ�ֵ
	BYTE R_I = GetRValue(clrSeed_I);
	BYTE G_I = GetGValue(clrSeed_I);
	BYTE B_I = GetBValue(clrSeed_I);
	float fY_I = (9798.0f * R_I + 19235.0f * G_I + 3735.0f * B_I) / 32768.0f;
	
	// �������ӵ���ĻҶ�ֵ
	BYTE R_II = GetRValue(clrSeed_II);
	BYTE G_II = GetGValue(clrSeed_II);
	BYTE B_II = GetBValue(clrSeed_II);
	float fY_II = (9798.0f * R_II + 19235.0f * G_II + 3735.0f * B_II) / 32768.0f;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// �Ը����ؽ��лҶ�ת��
	for (i = 0; i < lHeight; i ++)
	{
		for (j = 0; j < lLineBytes; j ++)
		{
			//��ȡ����ɫ����
			BYTE R = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			j++;
			BYTE G = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			j++;
			BYTE B = *((BYTE *)lpDIBBits + lLineBytes * i + j);

			//���㵱ǰ��Ҷ�ֵ
			BYTE Y = (9798 * R + 19235 * G + 3735 * B) / 32768;
#if _MSC_VER>=1900
			if (fabs(Y - fY_I) < fabs(Y - fY_II))
#else
			if (abs(Y - fY_I) < abs(Y - fY_II))
#endif
			{
				//��ǰ��ͬ����һ�Ҷ�ֵ�ȽϽӽ�
				
				//������һ����ɫ������ǰ����
				*((BYTE *)lpDIBBits + lLineBytes * i + j - 2) = R_I;
				*((BYTE *)lpDIBBits + lLineBytes * i + j - 1) = G_I;
				*((BYTE *)lpDIBBits + lLineBytes * i + j ) = B_I;
			}
			else
			{
				//��ǰ��ͬ���Ӷ��Ҷ�ֵ�ȽϽӽ�
				
				//�����Ӷ�����ɫ������ǰ����
				*((BYTE *)lpDIBBits + lLineBytes * i + j - 2) = R_II;
				*((BYTE *)lpDIBBits + lLineBytes * i + j - 1) = G_II;
				*((BYTE *)lpDIBBits + lLineBytes * i + j ) = B_II;
			}
		}
	}

	EndWaitCursor();
}

/*************************************************************************
 *
 * �������ƣ�
 *   Sobel()
 *
 * ����:
 *   HDIB hDIB          - �������DIB
 *
 * ����ֵ:
 *   void				- �޷���ֵ
 *
 * ˵��:
 *	 ��ͼ��������������ӵı�Ե���
 *
 ************************************************************************/
void CSplitProc::Sobel()
{
	
	LONG i;
	LONG j;
	LONG k;
	LONG l;
	
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();

	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// ������char��Ҳ������::strcpy()
	BYTE* m_temp;
	m_temp=new BYTE [lLineBytes * lHeight];

	// �м����
	int v_r_v, v_g_v, v_b_v, g_v;
	int v_r_h, v_g_h, v_b_h, g_h;

	// ����ͼ�����ݵ��м仺��
	for (i = 0; i < lLineBytes * lHeight; i++)
		m_temp[i] = *(lpDIBBits + i);

	// 3X3 ģ��
	for (i = 0; i < lWidth; i++)			//������������i��
	{
		for (j = 0; j < lHeight; j++)	//������������j��
		{
			v_r_v = v_g_v = v_b_v = v_r_h = v_g_h = v_b_h = 0;
			
			for (k = i - 1; k < i + 2; k++)	//3*3ģ��
			{
				for (l = j - 1; l < j + 2; l++)
				{
					// ��ֹ�ڴ����
					if (k >= 0  && l >= 0 && k < lWidth && l < lHeight)
					{
						// ���ģ��
						if (k == i - 1)
						{
							if (l == j)
								g_v = -2;
							else
								g_v = -1;
						}
						if (k == i + 1)
						{
							if (l == j)
								g_v = 2;
							else
								g_v = 1;
						}
						if (k == i)
							g_v = 0;
						if (l == j - 1)
						{	
							if (k == i)
								g_h = 2;
							else
								g_h = 1;
						}
						if (l == j + 1)
						{
							if (k == i)
								g_h = -2;
							else
								g_h = -1;
						}
						if (l == j)
							g_h = 0;
						
						v_r_v += *(lpDIBBits + l * lLineBytes + k * nChannel) * g_v;
						v_r_h += *(lpDIBBits + l * lLineBytes + k * nChannel) * g_h;
						v_g_v += *(lpDIBBits + l * lLineBytes + k * nChannel + 1) * g_v;
						v_g_h += *(lpDIBBits + l * lLineBytes + k * nChannel + 1) * g_h;
						v_b_v += *(lpDIBBits + l * lLineBytes + k * nChannel + 2) * g_v;
						v_b_h += *(lpDIBBits + l * lLineBytes + k * nChannel + 2) * g_h;
					}
				}
			}
			m_temp[j * lLineBytes + i * nChannel] = (int)sqrt(1.0*v_r_v * v_r_v + v_r_h * v_r_h);
			m_temp[j * lLineBytes + i * nChannel + 1] = (int)sqrt(1.0*v_g_v * v_g_v + v_g_h * v_g_h);
			m_temp[j * lLineBytes + i * nChannel + 2] = (int)sqrt(1.0*v_b_v * v_b_v + v_b_h * v_b_h);
		}
	}

	// �ش洦������DIB
	for(i = 0; i < lLineBytes * lHeight; i++)
		*(lpDIBBits + i) = m_temp[i];

	// �ͷ��ڴ�
	delete[] m_temp;
	
	EndWaitCursor();
}

/*************************************************************************
 *
 * �������ƣ�
 *   Prewitt()
 *
 * ����:
 *   HDIB hDIB          - �������DIB
 *
 * ����ֵ:
 *   void				- �޷���ֵ
 *
 * ˵��:
 *	 ��ͼ���������ά�����ӵı�Ե���
 *
 ************************************************************************/
void CSplitProc::Prewitt()
{
	
	LONG i;
	LONG j;
	LONG k;
	LONG l;
	
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();

	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// ������char��Ҳ������::strcpy()
	BYTE* m_temp;
	m_temp = new BYTE [lLineBytes * lHeight];

	// �м����
	int v_r_v, v_g_v, v_b_v, g_v;
	int v_r_h, v_g_h, v_b_h, g_h;

	// ����ͼ�����ݵ��м仺��
	for (i = 0; i < lLineBytes * lHeight; i++)
		m_temp[i] = *(lpDIBBits + i);

	// 3X3 ģ��
	for (i = 0; i < lWidth; i++)			//������������i��
	{
		for (j = 0; j < lHeight; j++)	//������������j��
		{
			v_r_v = v_g_v = v_b_v = v_r_h = v_g_h = v_b_h = 0;
			
			for (k = i - 1; k < i + 2; k++)	//3*3ģ��
			{
				for (l = j - 1; l < j + 2; l++)
				{
					// ��ֹ�ڴ����
					if (k >= 0  && l >= 0 && k < lWidth && l < lHeight)
					{
						// ���ģ��
						if (k == i - 1)
							g_v = -1;
						if (k == i + 1)
							g_v = 1;
						if (k == i)
							g_v = 0;
						if (l == j - 1)
							g_h = 1;
						if (l == j + 1)
							g_h = -1;
						if (l == j)
							g_h = 0;
						
						v_r_v += *(lpDIBBits + l * lLineBytes + k * nChannel) * g_v;
						v_r_h += *(lpDIBBits + l * lLineBytes + k * nChannel) * g_h;
						v_g_v += *(lpDIBBits + l * lLineBytes + k * nChannel + 1) * g_v;
						v_g_h += *(lpDIBBits + l * lLineBytes + k * nChannel + 1) * g_h;
						v_b_v += *(lpDIBBits + l * lLineBytes + k * nChannel + 2) * g_v;
						v_b_h += *(lpDIBBits + l * lLineBytes + k * nChannel + 2) * g_h;
					}
				}
			}
			m_temp[j * lLineBytes + i * nChannel] = (int)sqrt(1.0*v_r_v * v_r_v + v_r_h * v_r_h);
			m_temp[j * lLineBytes + i * nChannel + 1] = (int)sqrt(1.0*v_g_v * v_g_v + v_g_h * v_g_h);
			m_temp[j * lLineBytes + i * nChannel + 2] = (int)sqrt(1.0*v_b_v * v_b_v + v_b_h * v_b_h);
		}
	}

	// �ش洦������DIB
	for(i = 0; i < lLineBytes * lHeight; i++)
		*(lpDIBBits + i) = m_temp[i];

	// �ͷ��ڴ�
	delete[] m_temp;
	
	EndWaitCursor();
}

/*************************************************************************
 *
 * �������ƣ�
 *   Roberts()
 *
 * ����:
 *   HDIB hDIB          - �������DIB
 *
 * ����ֵ:
 *   void				- �޷���ֵ
 *
 * ˵��:
 *	 ��ͼ������޲��ؽ������ӵı�Ե���
 *
 ************************************************************************/
void CSplitProc::Roberts()
{
	
	LONG i;
	LONG j;
	LONG k;
	LONG l;
	
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();

	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// ������char��Ҳ������::strcpy()
	BYTE* m_temp;
	m_temp = new BYTE [lLineBytes * lHeight];

	// �м����
	int v_r_v, v_g_v, v_b_v, g_v;
	int v_r_h, v_g_h, v_b_h, g_h;

	// ����ͼ�����ݵ��м仺��
	for (i = 0; i < lLineBytes * lHeight; i++)
		m_temp[i] = *(lpDIBBits + i);

	// 2X2 ģ��
	for (i = 0; i < lWidth; i++)		//������������i��
	{
		for (j = 0; j < lHeight; j++)	//������������j��
		{
			v_r_v = v_g_v = v_b_v = v_r_h = v_g_h = v_b_h = 0;
			
			for (k = i - 1; k < i + 1; k++)	//2*2ģ��
			{
				for (l = j - 1; l < j + 1; l++)
				{
					// ��ֹ�ڴ����
					if (k >= 0  && l >= 0 && k < lWidth && l < lHeight)
					{
						// ���ģ��
						if (k == i - 1 && l == j - 1)
							g_v = 1;
						else if (k == i && l == j)
							g_v = -1;
						else
							g_v = 0;

						if(k == i - 1 && l == j)
							g_h = -1;
						else if (k == i && l == j - 1)
							g_h = 1;
						else
							g_h = 0;
						
						v_r_v += *(lpDIBBits + l * lLineBytes + k * nChannel) * g_v;
						v_r_h += *(lpDIBBits + l * lLineBytes + k * nChannel) * g_h;
						v_g_v += *(lpDIBBits + l * lLineBytes + k * nChannel + 1) * g_v;
						v_g_h += *(lpDIBBits + l * lLineBytes + k * nChannel + 1) * g_h;
						v_b_v += *(lpDIBBits + l * lLineBytes + k * nChannel + 2) * g_v;
						v_b_h += *(lpDIBBits + l * lLineBytes + k * nChannel + 2) * g_h;
					}
				}
			}
			m_temp[j * lLineBytes + i * nChannel] = (int)sqrt(1.0*v_r_v * v_r_v + v_r_h * v_r_h);
			m_temp[j * lLineBytes + i * nChannel + 1] = (int)sqrt(1.0*v_g_v * v_g_v + v_g_h * v_g_h);
			m_temp[j * lLineBytes + i * nChannel + 2] = (int)sqrt(1.0*v_b_v * v_b_v + v_b_h * v_b_h);
		}
	}

	// �ش洦������DIB
	for(i = 0; i < lLineBytes * lHeight; i++)
		*(lpDIBBits + i) = m_temp[i];

	// �ͷ��ڴ�
	delete[] m_temp;

	EndWaitCursor();
}

/*************************************************************************
 *
 * �������ƣ�
 *   Laplacian()
 *
 * ����:
 *   HDIB hDIB          - �������DIB
 *
 * ����ֵ:
 *   void				- �޷���ֵ
 *
 * ˵��:
 *	 ��ͼ�����������˹���ӵı�Ե���
 *
 ************************************************************************/
void CSplitProc::Laplacian()
{
	
	LONG i;
	LONG j;
	LONG k;
	LONG l;
	
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();

	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// ������char��Ҳ������::strcpy()
	BYTE* m_temp;
	m_temp=new BYTE [lLineBytes * lHeight];

	// �м����
	int v_r, v_g, v_b, p_g;

	// ���ģ��
	int g[9]={-1, -1, -1, -1, 8, -1, -1, -1, -1};

	// ����ͼ�����ݵ��м仺��
	for (i = 0; i < lLineBytes * lHeight; i++)
		m_temp[i] = *(lpDIBBits + i);

	// 3X3 ģ��
	for (i = 0; i < lWidth; i++)		//������������i��
	{
		for (j = 0; j < lHeight; j++)	//������������j��
		{
			v_r = v_g = v_b = p_g = 0;
	
			for (k = i - 1; k < i + 2; k++)	//3*3ģ��
			{
				for (l = j - 1; l < j + 2; l++)
				{
					// ��ֹ�ڴ����
					if (k >= 0  && l >= 0 && k < lWidth && l < lHeight)
					{
							v_r += *(lpDIBBits + l * lLineBytes + k * nChannel) * g[p_g];
							v_g += *(lpDIBBits + l * lLineBytes + k * nChannel + 1) * g[p_g];
							v_b += *(lpDIBBits + l * lLineBytes + k * nChannel + 2) * g[p_g];
							p_g++;
					}
				}
			}
			
			if (v_r < 0)
				v_r = 0;
			if (v_g < 0)
				v_g = 0;
			if (v_b < 0)
				v_b = 0;

			m_temp[j * lLineBytes + i * nChannel] = v_r;
			m_temp[j * lLineBytes + i * nChannel + 1] = v_g;
			m_temp[j * lLineBytes + i * nChannel + 2] = v_b;
		}
	}

	// �ش洦������DIB
	for(i = 0; i < lLineBytes * lHeight; i++)
		*(lpDIBBits + i) = m_temp[i];

	// �ͷ��ڴ�
	delete[] m_temp;

	EndWaitCursor();
}

/*************************************************************************
 *
 * �������ƣ�
 *   GuassLaplacian()
 *
 * ����:
 *   HDIB hDIB          - �������DIB
 *
 * ����ֵ:
 *   void				- �޷���ֵ
 *
 * ˵��:
 *	 ��ͼ����и�˹����������˹���ӵı�Ե���
 *
 ************************************************************************/
void CSplitProc::GuassLaplacian()
{
	
	LONG i;
	LONG j;
	LONG k;
	LONG l;
	
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();

	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// ������char��Ҳ������::strcpy()
	BYTE* m_temp;
	m_temp=new BYTE [lLineBytes * lHeight];

	// �м����
	int v_r, v_g, v_b, p_g;

	// ���ģ��
	int g[25]={-2,-4,-4,-4,-2,
			   -4, 0, 8, 0,-4,
			   -4, 8,24, 8,-4,
			   -4, 0, 8, 0,-4,
			   -2,-4,-4,-4,-2};

	// ����ͼ�����ݵ��м仺��
	for (i = 0; i < lLineBytes * lHeight; i++)
		m_temp[i] = *(lpDIBBits + i);

	// 5X5 ģ��
	for (i = 0; i < lWidth; i++)		//������������i��
	{
		for (j = 0; j < lHeight; j++)	//������������j��
		{
			v_r = v_g = v_b = p_g = 0;
	
			for (k = i - 2; k < i + 3; k++)	//5*5ģ��
			{
				for (l = j - 2; l < j + 3; l++)
				{
					// ��ֹ�ڴ����
					if (k >= 0  && l >= 0 && k < lWidth && l < lHeight)
					{
							v_r += *(lpDIBBits + l * lLineBytes + k * nChannel) * g[p_g];
							v_g += *(lpDIBBits + l * lLineBytes + k * nChannel + 1) * g[p_g];
							v_b += *(lpDIBBits + l * lLineBytes + k * nChannel + 2) * g[p_g];
							p_g++;
					}
				}
			}
			
			if (v_r < 0)
				v_r = 0;
			if (v_g < 0)
				v_g = 0;
			if (v_b < 0)
				v_b = 0;

			m_temp[j * lLineBytes + i * nChannel] = v_r;
			m_temp[j * lLineBytes + i * nChannel + 1] = v_g;
			m_temp[j * lLineBytes + i * nChannel + 2] =v_b;
		}
	}

	// �ش洦������DIB
	for(i = 0; i < lLineBytes * lHeight; i++)
		*(lpDIBBits + i) = m_temp[i];

	// �ͷ��ڴ�
	delete[] m_temp;

	EndWaitCursor();
}

/*************************************************************************
 *
 * �������ƣ�
 * 1  Hough()
 *
 * ����:
 *   HDIB hDIB          - �������DIB
 *
 * ����ֵ:
 *   void				- �޷���ֵ
 *
 * ˵��:
 *   �ú������뾶40�����ص�Բ�ܴ�ԭʼͼ������ȡ������
 *
 ************************************************************************/
void CSplitProc::Hough()
{
	
	LONG i;
	LONG j;
	LONG k;
	
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();

	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();
	
	// ������char��Ҳ������::strcpy()
	BYTE* m_temp;
	m_temp = new BYTE [lLineBytes * lHeight];

	// �м����
	int cx, cy1, cy2;

	memset(m_temp, 0, sizeof(m_temp));

	// ����ɢ��ΪԲ�Ļ�Բ
	for (i = 0; i < lWidth; i++)		//������������i��
	{
		for (j = 0; j < lHeight; j++)	//������������j��
		{
			if (*(lpDIBBits + j * lLineBytes + i * nChannel) == 0)
			{
				for (k = -40; k < 40; k++)
				{
					cx = k;
					cy1 = (int)sqrt(1.0*40 * 40 - cx * cx);
					cy2 = (int)sqrt(1.0*40 * 40 - cx * cx) * (-1);
					cx += i;
					cy1 += j;
					cy2 += j;
					if (cx >= 0 && cx < lWidth && cy1 >= 0 && cy1 < lHeight)
					{
						m_temp[cy1 * lLineBytes + cx * nChannel]++;
						m_temp[cy1 * lLineBytes + cx * nChannel + 1]++;
						m_temp[cy1 * lLineBytes + cx * nChannel + 2]++;
					}
					if (cx >= 0 && cx < lWidth && cy2 >= 0 && cy2 < lHeight)
					{
						m_temp[cy2 * lLineBytes + cx * nChannel]++;
						m_temp[cy2 * lLineBytes + cx * nChannel + 1]++;
						m_temp[cy2 * lLineBytes + cx * nChannel + 2]++;
					}
				}
			}
		}
	}

	// ��ʾ�м�ͳ�ƽ��
	for (i = 0; i <lLineBytes * lHeight; i++)
		*(lpDIBBits + i) = m_temp[i];

	// ͨ�������ֵ�����Բ��λ��
	int nMaxValue = 0;
	int nMaxX, nMaxY;
	
	for (i = 0; i < lWidth; i++)		//������������i��
	{
		for (j = 0; j < lHeight; j++)	//������������j��
		{
			if (nMaxValue < m_temp[j * lLineBytes + i * nChannel])
			{
				nMaxValue = m_temp[j * lLineBytes + i * nChannel];
				nMaxX = i;
				nMaxY = j;
			}
		}
	}

	// ��ʾ�����
	for (i = 0; i <lLineBytes * lHeight; i++)
		m_temp[i] = 255;

	for (k = -40; k < 40; k++)
	{
		cx = k;
		cy1 = (int)sqrt(1.0*40 * 40 - cx * cx);
		cy2 = (int)sqrt(1.0*40 * 40 - cx * cx) * (-1);
		cx += nMaxX;
		cy1 += nMaxY;
		cy2 += nMaxY;

		if (cx >= 0 && cx < lWidth && cy1 >= 0 && cy1 < lHeight)
		{
			m_temp[cy1 * lLineBytes + cx * nChannel] = 0;
			m_temp[cy1 * lLineBytes + cx * nChannel + 1] = 0;
			m_temp[cy1 * lLineBytes + cx * nChannel + 2] = 0;
		}
		if (cx >= 0 && cx < lWidth && cy2 >= 0 && cy2 < lHeight)
		{
			m_temp[cy2 * lLineBytes + cx * nChannel] = 0;
			m_temp[cy2 * lLineBytes + cx * nChannel + 1] = 0;
			m_temp[cy2 * lLineBytes + cx * nChannel + 2] = 0;
		}
	}
	
	// ��ʾ�����
	for (i = 0; i <lLineBytes * lHeight; i++)
		*(lpDIBBits + i) = m_temp[i];

	// �ͷ��ڴ�
	delete[] m_temp;

	EndWaitCursor();
}

/*************************************************************************
 *
 * �������ƣ�
 *   Kirsch()
 *
 * ����:
 *   HDIB hDIB          - �������DIB
 *
 * ����ֵ:
 *   void				- �޷���ֵ
 *
 * ˵��:
 *   �ú���ʹ��Kirsch���Ӷ�ͼ����б�Ե���
 *
 ************************************************************************/
void CSplitProc::Kirsch()
{
	
	LONG i;
	LONG j;
	LONG k;
	LONG l;
	
	BYTE* lpNewDIBBits1;
	BYTE* lpNewDIBBits2;
	BYTE* lpDst1;
	BYTE* lpDst2;

	HLOCAL hNewDIBBits1;
	HLOCAL hNewDIBBits2;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();

	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits1 = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits1 == NULL)
	{
		// �����ڴ�ʧ��
		return;
	}

	// �����ڴ�
	lpNewDIBBits1 = (BYTE*)LocalLock(hNewDIBBits1);

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits2 = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits1 == NULL)
	{
		// �����ڴ�ʧ��
		return;
	}

	// �����ڴ�
	lpNewDIBBits2 = (BYTE*)LocalLock(hNewDIBBits2);

	// ����ͼ�󵽻���
	lpDst1 = lpNewDIBBits1;
	lpDst2 = lpNewDIBBits2;

	memcpy(lpNewDIBBits1, lpDIBBits, lLineBytes * lHeight);
	memcpy(lpNewDIBBits2, lpDIBBits, lLineBytes * lHeight);

	// ������char��Ҳ������::strcpy()
	BYTE* m_temp;
	m_temp = new BYTE [lLineBytes * lHeight];

	// Kirsch���ӵİ˸������ϵ���ģģ��
	int g[8][9] = { { 5, 5, 5, -3, 0, -3, -3, -3, -3},
					{ -3, 5, 5, -3, 0, 5, -3, -3, -3},
					{ -3, -3, 5, -3, 0, 5, -3, -3, 5},
					{ -3, -3, -3, -3, 0, 5, -3, 5, 5},
					{ -3, -3, -3, -3, 0, -3, 5, 5, 5},
					{ -3, -3, -3, 5, 0, -3, 5, 5, -3},
					{ 5, -3, -3, 5, 0, -3, 5, -3, -3},
					{ 5, 5, -3, 5, 0, -3, -3, -3, -3}};
	
	// �м����
	int v_r, v_g, v_b, p_g;

    // ����ͼ�����ݵ��м仺��
	memcpy(m_temp, lpNewDIBBits1, lLineBytes * lHeight);

	// 3X3 ģ��
	for (i = 0; i < lWidth; i++)		//������������i��
	{
		for (j = 0; j < lHeight; j++)	//������������j��
		{
			v_r = v_g = v_b = p_g = 0;
			
			for (k = i - 1; k < i + 2; k++)	//3*3ģ��
			{
				for (l = j - 1; l < j + 2; l++)
				{
					// ��ֹ�ڴ����
					if (k >= 0  && l >= 0 && k < lWidth && l < lHeight)
					{
						v_r += *(lpNewDIBBits1 + l * lLineBytes + k * nChannel) * g[0][p_g];
						v_g += *(lpNewDIBBits1 + l * lLineBytes + k * nChannel + 1) * g[0][p_g];
						v_b += *(lpNewDIBBits1 + l * lLineBytes + k * nChannel + 2) * g[0][p_g];
						p_g++;
					}
				}
			}

			v_r /= 9;
			v_g /= 9;
			v_b /= 9;

			if (v_r < 0)
				v_r = 0;
			if (v_g < 0)
				v_g = 0;
			if (v_b < 0)
				v_b = 0;

			m_temp[j * lLineBytes + i * nChannel] = v_r;
			m_temp[j * lLineBytes + i * nChannel + 1] = v_g;
			m_temp[j * lLineBytes + i * nChannel + 2] = v_b;
		}
	}

	// �ش洦������DIB
	memcpy(lpNewDIBBits1, m_temp, lLineBytes * lHeight);

	// ��һͼ�󻺴�ͬ�ڶ�~�ڰ�ͼ�󻺴��������αȽϣ���
	// ��ֵ�����ڵ�һͼ�󻺴档
	for (int loop = 1; loop < 8; loop++)
	{
		// ����ͼ�����ݵ��м仺��
		memcpy(m_temp, lpNewDIBBits2, lLineBytes * lHeight);
	
		// 3X3 ģ��
		for (i = 0; i < lWidth; i++)		//������������i��
		{
			for (j = 0; j < lHeight; j++)	//������������j��
			{
				v_r = v_g = v_b = p_g = 0;
			
				for (k = i - 1; k < i + 2; k++)	//3*3ģ��
				{
					for (l = j - 1; l < j + 2; l++)
					{
						// ��ֹ�ڴ����
						if (k >= 0  && l >= 0 && k < lWidth && l < lHeight)
						{
							v_r += *(lpNewDIBBits2 + l * lLineBytes + k * nChannel) * g[loop][p_g];
							v_g += *(lpNewDIBBits2 + l * lLineBytes + k * nChannel + 1) * g[loop][p_g];
							v_b += *(lpNewDIBBits2 + l * lLineBytes + k * nChannel + 2) * g[loop][p_g];
							p_g++;
						}
					}
				}

				v_r /= 9;
				v_g /= 9;
				v_b /= 9;

				if (v_r < 0)
					v_r = 0;
				if (v_g < 0)
					v_g = 0;
				if (v_b < 0)
					v_b = 0;

				m_temp[j * lLineBytes + i * nChannel] = v_r;
				m_temp[j * lLineBytes + i * nChannel + 1] = v_g;
				m_temp[j * lLineBytes + i * nChannel + 2] = v_b;
			}
		}

		// �ش洦������DIB
		memcpy(lpNewDIBBits2, m_temp, lLineBytes * lHeight);

		// ��ȡ�������Ӧ���ص����ֵ
		for (j = 0; j < lHeight; j++)
		{
			for (i = 0; i < lLineBytes; i++)
			{
				lpDst1 = (BYTE*)lpNewDIBBits1 + lLineBytes * j + i;
				lpDst2 = (BYTE*)lpNewDIBBits2 + lLineBytes * j + i;
				if (*lpDst2 > *lpDst1)
					*lpDst1 = * lpDst2;
			}
		}
	}

	// ����������Ƶ�ԭͼ��
	memcpy(lpDIBBits, lpNewDIBBits1, lLineBytes * lHeight); 

	// �ͷ��ڴ桢�������
	LocalUnlock(hNewDIBBits1);
	LocalFree(hNewDIBBits1);
	LocalUnlock(hNewDIBBits2);
	LocalFree(hNewDIBBits2);
	
	delete[] m_temp;
	
	EndWaitCursor();
}

/*************************************************************************
 *
 * �������ƣ�
 *   ContourDIB()
 *
 * ����:
 *   HDIB hDIB          - �������DIB
 *   COLORREF clrSeed_I - ���ӵ�һ��RGBֵ
 *   COLORREF clrSeed_II- ���ӵ����RGBֵ
 *
 * ����ֵ:
 *   void				- �޷���ֵ
 *
 * ˵��:
 *   �ú�����ͼ�����������ȡ
 *
 ************************************************************************/
void CSplitProc::ContourDIB(COLORREF clrSeed_I,COLORREF clrSeed_II)
{
	
	LONG i;
	LONG j;
	LONG k;
	LONG s;

	// �������ӵ�һ�ĻҶ�ֵ
	BYTE R_I = GetRValue(clrSeed_I);
	BYTE G_I = GetGValue(clrSeed_I);
	BYTE B_I = GetBValue(clrSeed_I);
	
	// �������ӵ���ĻҶ�ֵ
	BYTE R_II = GetRValue(clrSeed_II);
	BYTE G_II = GetGValue(clrSeed_II);
	BYTE B_II = GetBValue(clrSeed_II);

	BYTE R, G, B;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();

	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// ������char��Ҳ������::strcpy()
	BYTE* m_temp;
	m_temp = new BYTE [lLineBytes * lHeight];

	// ����ͼ�����ݵ��м仺��
	for (i = 0; i < lLineBytes * lHeight; i++)
		m_temp[i] = *(lpDIBBits + i);

	// �Ը����ؽ��лҶ�ת��
	for (i = 0; i < lWidth; i++)		//������������i��
	{
		for (j = 0; j < lHeight; j++)	//������������j��
		{
			int I, II;
			I = II = 0;
			for (k = i - 1; k < i + 2; k++)	//3*3ģ��
			{
				for (s = j - 1; s < j + 2; s++)
				{
					// ��ֹ�ڴ����
					if (k >= 0  && s >= 0 && k < lWidth && s < lHeight)
					{
						// ��ȡ����ɫ����
						R = *((BYTE *)lpDIBBits + s * lLineBytes + k * nChannel);
						G = *((BYTE *)lpDIBBits + s * lLineBytes + k * nChannel + 1);
						B = *((BYTE *)lpDIBBits + s * lLineBytes + k * nChannel + 2);
						
						// �ж��Ƿ��������Ҷȷ�Χ��
						if (abs(B - B_I) < 10 && abs(G - B_I) < 10 && abs(R - R_I) < 10)
							I++;
						if (abs(B - B_II) < 10 && abs(G - B_II) < 10 && abs(R - R_II) < 10)
							II++;			
					}
				}
			}
			// �����Χ�������ػҶ���ͬ����ǰ��һ���Ǳ߽磬����ȥ������Ϊ��ɫ��
			if (I == 9 || II == 9)
			{
				m_temp[j * lLineBytes + i * nChannel] = 255;
				m_temp[j * lLineBytes + i * nChannel + 1] = 255;
				m_temp[j * lLineBytes + i * nChannel + 2] = 255;
			}
		}
	}

	// �ش洦������DIB
	for(i = 0; i < lLineBytes * lHeight; i++)
		*(lpDIBBits + i) = m_temp[i];

	EndWaitCursor();

	SAFE_DELETE(m_temp);
}

/*************************************************************************
 *
 * �������ƣ�
 *   TraceDIB()
 *
 * ����:
 *   HDIB hDIB          - �������DIB
 *   COLORREF clrSeed	- ���ӵ�RGBֵ 
 *
 * ����ֵ:
 *   void				- �޷���ֵ
 *
 * ˵��:
 *   �ú�����ͼ����б߽����
 *
 ************************************************************************/
void CSplitProc::TraceDIB(COLORREF clrSeed)
{
	
	LONG i;
	LONG j;

	// ָ��Դͼ���ָ��
	BYTE* lpSrc;
	
	// ָ�򻺴�ͼ���ָ��	
	BYTE* lpDst;

	//�Ƿ��ҵ���ʼ�㼰�ص���ʼ��
	bool bFindStartPoint;

	//�Ƿ�ɨ�赽һ���߽��
	bool bFindPoint;

	//��ʼ�߽���뵱ǰ�߽��
	CPoint StartPoint, CurrentPoint;

	//�˸��������ʼɨ�跽��
	int Direction[8][2] = { {-1, 1}, {0, 1}, {1, 1}, {1, 0},
							{1, -1}, {0, -1}, {-1, -1}, {-1, 0}};
	int BeginDirect;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	int nChannel = m_pDIB->Channel();

	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	HLOCAL	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);

	if (hNewDIBBits == NULL)  
	{
		// �����ڴ�ʧ��
		return;
	}
	
	// �����ڴ�
	BYTE*	lpNewDIBBits = (BYTE*)LocalLock(hNewDIBBits);

	// ��ʼ���·�����ڴ棬�趨��ʼֵΪ255
	lpDst = (BYTE*)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes * lHeight);
	
	//���ҵ������Ϸ��ı߽��
	bFindStartPoint = false;

	for (j = 0; j < lHeight && !bFindStartPoint; j++)
	{
		for (i = 0; i < lWidth && !bFindStartPoint; i++)
		{
			// ָ��Դͼ������j�У���i�����ص�ָ��			
			lpSrc = (BYTE*)(lpDIBBits + lLineBytes * j + i * nChannel);
			if(abs(*lpSrc - GetRValue(clrSeed)) < 10 && abs(*(lpSrc + 1) - GetGValue(clrSeed)) < 10 && abs(*(lpSrc + 2) - GetBValue(clrSeed)) < 10)
			{
				bFindStartPoint = true;

				StartPoint.y = j;
				StartPoint.x = i;

				// ָ��Ŀ��ͼ������j�У���i�����ص�ָ��			
				lpDst = (BYTE*)(lpNewDIBBits + lLineBytes * j + i * nChannel);	
				*lpDst = (BYTE) 0;
				*(lpDst + 1) = (BYTE) 0;
				*(lpDst + 2) = (BYTE) 0;
			}		
		}
	}

	//������ʼ���������·�������ʼɨ�������Ϸ���
	BeginDirect = 0;
	
	//���ٱ߽�
	bFindStartPoint = false;
	
	//�ӳ�ʼ�㿪ʼɨ�� 
	CurrentPoint.y = StartPoint.y;
	CurrentPoint.x = StartPoint.x;
	
	while (!bFindStartPoint)
	{
		bFindPoint = false;
		while (!bFindPoint)
		{
			if ((CurrentPoint.y + Direction[BeginDirect][1]) >= 0
				&& (CurrentPoint.x + Direction[BeginDirect][0]) >= 0
				&& (CurrentPoint.y + Direction[BeginDirect][1]) < lHeight
				&& (CurrentPoint.x + Direction[BeginDirect][0]) < lWidth)
			{

				//��ɨ�跽��鿴һ������
				lpSrc = (BYTE*)(lpDIBBits + lLineBytes * (CurrentPoint.y + Direction[BeginDirect][1])
					+ (CurrentPoint.x + Direction[BeginDirect][0]) * 3);
			
				if (abs(*lpSrc - GetRValue(clrSeed)) < 10 && abs(*(lpSrc + 1) - GetGValue(clrSeed)) < 10 && abs(*(lpSrc + 2) - GetBValue(clrSeed)) < 10)
				{
					bFindPoint = true;
					CurrentPoint.y = CurrentPoint.y + Direction[BeginDirect][1];
					CurrentPoint.x = CurrentPoint.x + Direction[BeginDirect][0];
					if (CurrentPoint.y == StartPoint.y && CurrentPoint.x == StartPoint.x)
						bFindStartPoint = true;
					lpDst = (BYTE*)lpNewDIBBits + lLineBytes * CurrentPoint.y + CurrentPoint.x * 3;
					*lpDst = (BYTE) 0;
					*(lpDst + 1) = (BYTE) 0;
					*(lpDst + 2) = (BYTE) 0;
		
					//ɨ��ķ�����ʱ����ת����
					BeginDirect--;
					if(BeginDirect == -1)
						BeginDirect = 7;
					BeginDirect--;
					if(BeginDirect == -1)
					BeginDirect = 7;
				}
				else
				{
					//ɨ�跽��˳ʱ����תһ��
					BeginDirect++;
					if(BeginDirect == 8)
						BeginDirect = 0;
				}
			}
			else
			{
				bFindPoint = true;
				bFindStartPoint = true;
			}
		}
	}

	// ���Ƹ�ʴ���ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);

	// �ͷ��ڴ�
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	EndWaitCursor();
}

/*************************************************************************
 *
 * �������ƣ�
 *   Threshold()
 *
 * ����:
 *   HDIB hDIB          - �������DIB
 *   int T				- ��ֵ
 *
 * ����ֵ:
 *   void				- �޷���ֵ
 *
 * ˵��:
 *   �ú�����ͼ����з�ֵ������ָ��
 *
 ************************************************************************/
void CSplitProc::Threshold(int *nNY)
{
	
	LONG i;
	LONG j;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();

	// ������ֵ
	int T1, T2;
	T1 = 127;
	T2 = 0;

	// ��ʱ����
	int Temp0, Temp1, Temp2, Temp3;
	Temp0 = Temp1 = Temp2 = Temp3 = 0;

	while (true)
	{
		// ������һ��������ֵ
		for (i = 0; i < T1 + 1; i++)
		{
			Temp0 += nNY[i] * i;
			Temp1 += nNY[i];
		}
		for (i = T1 + 1; i < 256; i++)
		{
			Temp2 += nNY[i] * i;
			Temp3 += nNY[i];
		}
		T2 = (Temp0 / Temp1 + Temp2 / Temp3) / 2;

		// ����������Ƿ�������
		if (T1 == T2)
			break;
		else
			T1 = T2;
	}

	// �Ը����ؽ��лҶ�ת��
	for (i = 0; i < lHeight; i ++)
	{
		for (j = 0; j < lLineBytes; j ++)
		{
			// ��ȡ����R����
			BYTE R = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			
			// �ж�R�����Ƿ񳬳���Χ
			if (R < T1)
				R = 0;
			else
				R = 255;
			
			// ��д�������R����
			*((BYTE *)lpDIBBits + lLineBytes * i + j) = R;
			j++;

			// ��ȡ����G����
			BYTE G = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			
			// �ж�G�����Ƿ񳬳���Χ
			if (G < T1)
				G = 0;
			else 
				G = 255;
			
			// ��д�������G����
			*((BYTE *)lpDIBBits + lLineBytes * i + j) = G;
			j++;

			// ��ȡ����B����
			BYTE B = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			
			// �ж�B�����Ƿ񳬳���Χ
			if (B < T1)
				B = 0;
			else
				B = 255;
			
			// ��д�������B����
			*((BYTE *)lpDIBBits + lLineBytes * i + j) = B;
		}
	}

	EndWaitCursor();
}


