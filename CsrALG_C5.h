#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "bmpFile.h"
#include<opencv2/opencv.hpp>
using namespace cv;
int CsrGetOtsuThreshold(int * histogram, int nSize);

void CsrGetHistogram(BYTE*pGryImg, int width, int height, int channel, int *histogram,int nSize);
//基本减法，向下饱和
void CsrImgSub(BYTE*pGryImg, BYTE*pBackImg, int width, int height, BYTE*pResImg);
//positive == 1 ,白色目标减法，反之，黑色目标减法
void CsrImgSub(BYTE*pGryImg, BYTE*pBackImg, int width, int height, bool positive, BYTE*pResImg);
void CsrImgDiv(BYTE*pGryImg, int width, int height, double div, BYTE*pResImg);
void CsrHistogramAvgFilter(int * histogram, int nSize, int M);
void CsrHistogramAvgFilter(int * histogram, int nSize, int M, int* pResHistogram);
struct ClusterCenter
{
	int sum;
	int grayAry[256];
	int count,aryNum;
	int mean;
	int oldMean;
	ClusterCenter()
	{
		memset(grayAry, 0, sizeof(int)*256);
		sum = count = aryNum = 0;
		oldMean = mean = 0.0;
	}
};
void CsrImgKmeans_Kind(int *histogram, int nSize, int k, ClusterCenter *center);
void CsrImgKmeans_Kind_And_Numbers(int *histogram, int nSize, int k, ClusterCenter *center);
void CsrImgDivideBykMeans(BYTE*pGryImg, int width, int height, int k, ClusterCenter * center, BYTE*pResImg);

void CsrGetHistogramMaxMin(int *histogram, int nSize, int * minVal, int * maxVal);
void CsrImgKeans3Fast(int *histogram, int nSize, int gmin, int gmax, int *d1, int *d2);
void CsrKmeans3FastShow(BYTE*pGryImg, int width, int height, int d1, int d2);