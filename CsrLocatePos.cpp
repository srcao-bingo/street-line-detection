////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CsrLocatePos.cpp : implementation file
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "CsrLocatePos.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 构造/析构函数
//
////////////////////////////////////////////////////////////////////////////////////////////////////
CsrLocatePos::CsrLocatePos()
{  	//<在构造函数中把所有的成员变量赋初值>
	//是否初始化成功
	m_isInitOK=false;
	//图像属性
    m_isRGB=false;
	m_width=0;
	m_height=0;
    //内存空间
	m_pRGBImg=NULL;
	m_pGryImg=NULL;
	m_pGrdImg=NULL;
	m_pTmpImg=NULL;
	m_pResImg=NULL;
	m_pSumImg=NULL;
	m_memSize=0; //已申请的内存字节数
	//内部计数器
	m_nFrameID=0;
}
	
CsrLocatePos::~CsrLocatePos()
{	//<在析构函数中释放所有的内存>
	Dump();
}

void CsrLocatePos::Dump()
{   //<写一个专门的用作释放内存的函数,因为内存会有多次的申请与释放>
	if (m_pRGBImg) { delete m_pRGBImg; m_pRGBImg=NULL; }
	if (m_pGryImg) { delete m_pGryImg; m_pGryImg=NULL; }
	if (m_pGrdImg) { delete m_pGrdImg; m_pGrdImg=NULL; }
	if (m_pTmpImg) { delete m_pTmpImg; m_pTmpImg=NULL; }
	if (m_pResImg) { delete m_pResImg; m_pResImg=NULL; }
	if (m_pSumImg) { delete m_pSumImg; m_pSumImg=NULL; }
	m_memSize=0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 初始化
//
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CsrLocatePos::Initialize(bool isRGB,int width,int height,int nFast0123)
{   //<在初始化函数中对所有的成员变量赋初值>
	//<仅在初始化函数中申请内存,因为初始化函数不会被频繁调用>
	//<初始化函数可以被多次调用>
	//<内存申请时,若已经申请的内存大于本次需要的内存,则不再次申请>

	//step.1------图像属性--------------------------//
	m_isRGB=isRGB;
	m_width=width;
	m_height=height;
	nFast = min(3, max(0, nFast0123));
	m_nShrink = (int)(pow(2, nFast) + 0.5);
	//缩小后的尺寸
	m_width = m_width / m_nShrink;
	m_height = m_height / m_nShrink;
	shrinkWidth = m_width;
	shrinkHeight = m_height;
	//step.2------内存申请--------------------------//
	if (m_width*m_height>m_memSize)
	{
		//先释放
		Dump();
		//后申请
		m_pRGBImg=new BYTE [m_width*m_height*3]; //彩色图像
		m_pGryImg=new BYTE [m_width*m_height];  //灰度图像
		m_pGrdImg=new BYTE [m_width*m_height]; //梯度图像
		m_pTmpImg=new BYTE [m_width*m_height]; //临时图像
		m_pResImg=new BYTE [m_width*m_height]; //结果图像
		m_pSumImg=new int[m_width*m_height];   //积分图
        //记录申请的大小
		m_memSize=m_width*m_height;
	}
	//step.3------初始化成功标志--------------------//
	m_isInitOK=(m_pRGBImg && m_pGryImg && m_pGrdImg && m_pTmpImg && m_pResImg && m_pSumImg);
	return m_isInitOK;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 执行
//
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CsrLocatePos::DoNext(BYTE *pImgData)
{ 	
	if (!m_isInitOK) return false;
    //...
	//...
	//...
	//某个特定帧的调试,比如10
	if (m_nFrameID==10)
	{
		//Debug();
	}
	//内部计数器累加
	m_nFrameID++;
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 结果
//
////////////////////////////////////////////////////////////////////////////////////////////////////
BYTE *CsrLocatePos::GetResImg()
{   //返回结果图像
	return m_pResImg;
}
int CsrLocatePos::GetResWidth()
{   //返回结果图像的宽度
	return m_width;
}
int CsrLocatePos::GetResHeight()
{   //返回结果图像的高度
	return m_height;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 调试
//
////////////////////////////////////////////////////////////////////////////////////////////////////
void CsrLocatePos::Debug(BYTE*pGryImg, int width, int height, const char * name)
{   //<本类的内部调试: 屏幕输出、文件输出等>
	//Rmw_Write8BitImg2BmpFileMark(pSbImg, width, height, "..\\0402Gry_RiceSobel.bmp");

	char str[256];
	sprintf_s(str, "%s", name);
	namedWindow(str, WINDOW_NORMAL);
	Mat pMat = Mat(height, width, CV_8UC1, pGryImg);
	imshow(str, pMat);
	waitKey(0);
	Rmw_Write8BitImg2BmpFile(pGryImg, width, height, "..\\DectectGry_Sobel.bmp");
	return;
}


void CsrLocatePos::CsrSobel(BYTE*pGryImg, int width, int height, BYTE *pResImg)
{
	int x, y;
	int dx, dy;
	BYTE *pCur = pGryImg + width;
	BYTE *pRes = pResImg + width;
	//首行不做
	memset(pResImg, 0, sizeof(BYTE)*width);
	for (y = 1; y < height - 1; y++)
	{
		//首列不做
		*(pRes++) = 0;
		pCur++;
		for (x = 1; x < width - 1; x++)
		{
			//1.求dx
			dx = *(pCur - 1 - width) + (*(pCur - 1) << 1) + *(pCur - 1 + width)
				- *(pCur + 1 - width) - (*(pCur + 1) << 1) - *(pCur + 1 + width);
			//2.求dy
			dy = *(pCur - 1 - width) + (*(pCur - width) << 1) + *(pCur + 1 - width)
				- *(pCur - 1 + width) - (*(pCur + width) << 1) - *(pCur + 1 + width);
			//3.求边缘强度
			*(pRes++) = min(255, abs(dx) + abs(dy));
			*pCur++;
		}
		pCur++;
		//尾列不做
		*(pRes++) = 0;
	}
	//尾行不做
	memset(pRes, 0, sizeof(BYTE)*width);
}
//抽样,nFast_0123,长宽缩小2^n倍
void CsrLocatePos::CsrReduceImgSize(BYTE*pGryImg, int width, int height, int nFast_0123, BYTE *pResImg)
{
	int x, y, scale, num;
	BYTE *pCur = pGryImg, *pRes = pResImg;
	nFast_0123 = max(0, min(3, nFast_0123));
	scale = (int)(pow(2, nFast_0123) + 0.5);
	//每个区域取第一个
	for (y = 0; y < height; y += scale)
	{
		for (x = 0; x < width; x += scale)
		{
			*(pRes++) = *(pCur + x);
		}

		pCur += scale * width;

	}

}

void CsrLocatePos::CsrSearchMaxRectA(int* pSumImg, int width, int height, int w, int h, int delLR, int*xL, int*xR, int*yL, int*yR)
{
	int x, y, x1, x2;
	int *pY1, *pCur, *pY2, delH;
	int  sumC, sumL, sumR, sumU, maxVal, diff;
	maxVal = 0, y = 1;
	delH = delLR * width;//sumU区域的高度
	pY1 = pSumImg + (y + delLR)*width;
	pY2 = pSumImg + (y + h + delLR)*width;
	for (y = 1; y < height - h - delLR; y++, pY1 += width, pY2 += width)
	{
		for (x = 1, x1 = x + delLR, x2 = x + w + delLR; x < width - w - delLR * 2; x++, x1++, x2++)
		{
			//-------step1.求各区域梯度强度--------//
			sumC = *(pY2 + x2) - *(pY1 + x2) - *(pY2 + x1) + *(pY1 + x1);//Center的梯度强度
			sumL = *(pY2 + x1) - *(pY2 + x1 - delLR) - *(pY1 + x1) + *(pY1 + x1 - delLR);
			sumR = *(pY2 + x2 + delLR) - *(pY2 + x2) - *(pY1 + x2 + delLR) + *(pY1 + x2);
			sumU = *(pY1 + x2) - *(pY1 + x1) - *(pY1 - delH + x2) + *(pY1 - delH + x1);
			diff = sumC - sumU - sumL - sumR;//求出差值
			//-------step2.更新坐标--------------//
			if (diff > maxVal)
			{
				maxVal = diff;
				*xL = x1;//只需记录左上角的坐标
				*yL = y + delLR;
			}
		}
	}
	//计算右上角坐标
	*xR = *xL + w;  *yR = *yL + h;
	return;
}


void CsrLocatePos::CsrSearchMaxRectLeftUp(int* pSumImg, int width, int height, int w, int h, int delLR, int*xL, int*xR, int*yL, int*yR)
{
	int x, y, x1, x2;
	int *pY1, *pCur, *pY2, delH;
	int  sumC, sumL, sumR, sumU, maxVal, diff;
	maxVal = 0, y = 1;
	delH = delLR * width;//sumU区域的高度
	pY1 = pSumImg + (y + delLR)*width;
	pY2 = pSumImg + (y + h + delLR)*width;
	for (y = 1; y < height - h - delLR; y++, pY1 += width, pY2 += width)
	{
		for (x = 1, x1 = x + delLR, x2 = x + w + delLR; x < width - w - delLR * 2; x++, x1++, x2++)
		{
			//-------step1.求各区域梯度强度--------//
			sumC = *(pY2 + x2) - *(pY1 + x2) - *(pY2 + x1) + *(pY1 + x1);//Center的梯度强度
			sumL = *(pY2 + x1) - *(pY2 + x1 - delLR) - *(pY1 + x1) + *(pY1 + x1 - delLR);
			sumR = *(pY2 + x2 + delLR) - *(pY2 + x2) - *(pY1 + x2 + delLR) + *(pY1 + x2);
			sumU = *(pY1 + x2) - *(pY1 + x1) - *(pY1 - delH + x2) + *(pY1 - delH + x1);
			diff = sumC - sumU - sumL - sumR;//求出差值
											 //-------step2.更新坐标--------------//
			if (diff > maxVal)
			{
				maxVal = diff;
				*xL = x1;//只需记录左上角的坐标
				*yL = y + delLR;
			}
		}
	}
	//计算右上角坐标
	*xR = *xL + w;  *yR = *yL + h;
	return;
}



void CsrLocatePos::CsrDrawRect(BYTE*pGryImg, int width, int height, int& xL, int& xR, int& yL, int& yR, BYTE*pResImg)
{	
	BYTE *pGry;
	int x, y,gryVal = 255;
	//上边界
	pGry = pResImg + yL * width;
	for (x = xL; x <= xR; x++) *(pGry + x) = gryVal;
	//下边界
	pGry += (yR-yL) * width;
	for (x = xL; x <= xR; x++) *(pGry + x) = gryVal;
	//左边界
	pGry = pResImg + yL * width + xL;
	for (y = yL; y <= yR; y++, pGry += width) *(pGry) = gryVal;
	//右边界
	pGry = pResImg + yL * width + xR;
	for (y = yL; y <= yR; y++, pGry += width) *(pGry) = gryVal;

}

void CsrLocatePos::CsrGetSumImg(BYTE *pGryImg, int width, int height, int *pResImg)
{
	int x, y;
	BYTE*pCur = pGryImg;
	int *pRes = pResImg;
	int sumCol[4096];
	//初始化
	memset(sumCol, 0, sizeof(int) * 4096);
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			sumCol[x] += *(pCur++);
			//积分图递推
			if (!x)
			{
				*(pRes++) = sumCol[x];
			}
			else
			{
				*(pRes++) = sumCol[x] + *(pRes - 1);
			}
		}

	}

}

bool CsrLocatePos::CsrSearchRectTarget(BYTE*pGryImg, int width,int height, int targetW,int targetH,int delLR,BYTE *pResImg)
{
	int xL = width, xR = width, yL = height, yR = height;//记录位置

	//----------------step1.降低分辨率-------------------------------//
	CsrReduceImgSize(pGryImg, width, height, nFast, m_pResImg);

	//----------------step2.sobel求边缘强度--------------------------//
	CsrSobel(m_pResImg, shrinkWidth, shrinkHeight, m_pGrdImg);
	Debug(m_pGrdImg, shrinkWidth, shrinkHeight, "m_pGrdImg");
	//----------------step3.边缘强度的积分图-------------------------//
	CsrGetSumImg(m_pGrdImg, shrinkWidth, shrinkHeight, m_pSumImg);
	
	//----------------step4.由积分图、位置特征进行区域定位-----------//
	CsrSearchMaxRectLeftUp(m_pSumImg, shrinkWidth, shrinkHeight, targetW,targetH,delLR, &xL, &xR, &yL, &yR);

	//----------------step5.在原图上绘制区域-------------------------//
	xL = xL << nFast; xR = xR << nFast; 
	yL = yL << nFast; yR = yR << nFast;//原位置
	if (xL < width&& xR < width && yL < height && yR < height)
	{
		CsrDrawRect(pGryImg, width, height, xL, xR, yL, yR, pResImg);
		return true;
	}
	else
		return false;

}
