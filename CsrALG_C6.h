#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include<stack>
#include "bmpFile.h"
#include"RmwALGTest.h"
#include"CsrALG_C5.h"
#include"CsrALG_C2.h"
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;

#define ANGLE 360
//1
int CsrGetBoundary(BYTE*pGryImg, int width, int height, int *xPos, int *yPos, BYTE* pResImg);
void CsrHoughTransform(int*x, int*y, int *sinV, int*cosV, int *throMax, int n, int(*count)[ANGLE], int shiftRigthBits);
void CsrFind1DMaxValSequence(int * pOrigin, int width, const int pair, int wSize,int *pResult);
void CsrFindParametersCountProjection(int(*count)[ANGLE], int maxThro, int*theta, int * thro);
void CsrShowCountImg(BYTE*pCountImg, int width, int height, int(*count)[ANGLE], int n);
void CsrFindParametersThroughCount(int(*count)[ANGLE], int n, int maxThro, int wSize, int pair, int * resThro, int *resTheta,int stepThro);
void drawHoughLine(BYTE *pGryImg, int width, int height, int pair, int thro, int theta);
void CsrGetSinVCosV(int minAngle, int maxAngle, int*sinV, int*cosV);
//2
void CsrGetCircleCenterPos(BYTE*pGryImg, int width, int height, int *a, int *b, BYTE *pResImg);
int CsrGetEdgePoint(BYTE * pGryImg, int width, int height, int*xPos, int*yPos);
void CsrCircleHough(int *x, int *y, int n, int a, int b, int*r, int err);
void CsrCircleHough(int *x, int *y, int n, int a, int b, int*r, int err, int *countR, int dimen);
//3
void CsrTraceAndFilling();
//4
void CsrVerticalProject(BYTE*pGryImg, int width, int blockH, int nFast,int id, int *pProjectOrg);
void CsrVerticalProject(int *pSumImg, int width, int blockH, int offsetY, int nFast, int *histogram, int nSize);
void Csr1DAvgFilter(int * pData, int width, int M, int *pResData);
void CsrSegment(BYTE*pGryImg, int width, int blockH, int *histogram, int*pProjectOrg, int*pProjectAvg, int id, int *xPos, int * yPos, int &count, BYTE*pResImg);



//Œﬁ”√
void CsrHoughTransform(BYTE* pResImg, int width, int height, int*x, int*y, int *sinV, int*cosV, int *diagVal, int n, int(*count)[ANGLE], int diag);
void getDiagVal(int *x, int*y, int n, int *diagVal);
void CsrHoughTransformImg(BYTE*pGryImg, int height, int width, int *sinV, int*cosV, int diag, int(*count)[ANGLE]);