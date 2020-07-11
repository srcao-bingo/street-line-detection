#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "bmpFile.h"
#include"CsrALG_C2.h"
#include"CsrALG_C3.h"
#include"CsrALG_C4.h"
#include"CsrALG_C5.h"
#include"CsrALG_C6.h"
#include"CsrLocatePos.h"

typedef unsigned char BYTE;
using namespace std;

void debug(BYTE*pGryImg, int width, int height, const char * name)
{
	char str[256];
	sprintf(str, "%s", name);
	namedWindow(str, WINDOW_NORMAL);
	Mat pMat = Mat(height, width, CV_8UC1, pGryImg);
	imshow(str, pMat);
	waitKey(0);
}
void CsrVerticalProjectTest(int nBlock)
{
	int i, y, blockH;
	bool isBegin = true;
	BYTE*pGryImg, *pResImg, *pCur;
	int width, height;
	int histogram[256], *pProjectAvg, *pProjectOrg;
	int xPos[256], yPos[256], count = 0;
	//----------------step1.读图像--------------------//
	pGryImg = Rmw_Read8BitBmpFile2Img("..\\0606Gry.bmp", &width, &height);
	if (!pGryImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	for (i = 0; i < height*width; i++) pGryImg[i] = (pGryImg[i] >= 251) ? 250 : pGryImg[i];//预处理，边缘实现伪彩色
	pProjectOrg = new int[width];
	pProjectAvg = new int[width];

	pResImg = new BYTE[width * height];
	memset(pResImg, 255, sizeof(BYTE)*width*height);
	//----------------step2.按块处理------------------//
	blockH = height / nBlock;
	for (y = 0; y < height - blockH; y += blockH)
	{
		CsrSegment(pGryImg + y * width, width, blockH, histogram, pProjectOrg, pProjectAvg, y, xPos, yPos, count, pResImg + y * width);
	}
	//----------------step3.HoughTransform-----------//
	CsrGetHoughTest(pGryImg, width, height, xPos, yPos, count);
	//----------------step4.保存图像--------------------//
	Rmw_Write8BitImg2BmpFile(pResImg, width, height, "..\\0606BinCount.bmp");
	Rmw_Write8BitImg2BmpFileMark(pGryImg, width, height, "..\\0606Res.bmp");
	delete pGryImg;
	delete pResImg;
	delete pProjectOrg;
	delete pProjectAvg;
}

int main()
{
	//分成40块
	CsrVerticalProjectTest(40);
	
	return 0;
}