
#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "bmpFile.h"
#include<nmmintrin.h>

void CsrRgb2Gray_Factor(BYTE*pRgbImg, int width, int height, BYTE *pResImg);
void CsrRgb2Gray_Low(BYTE*pRgbImg, int width, int height, BYTE *pResImg);
void CsrSumImgRowAvgFilter(int *pSumImg, int width, int height, int M, BYTE *pResImg);
void CsrGetRowSumImg(BYTE *pGryImg, int width, int height, int *pResImg);
void CsrGetSumImg(BYTE *pGryImg, int width, int height, int *pResImg);

void CsrSumColAvgFilter(BYTE*pGryImg, int width, int height, int M, BYTE *pResImg);
void CsrInvertPhaseSSE(BYTE*pGryImg, int width, int height);
double CsrMedianFilter(BYTE*pGryImg, int width, int height, int N, int M, BYTE *pResImg);
double CsrMinFilter(BYTE*pGryImg, int width, int height, int N, int M, BYTE *pResImg);
double CsrMinFilterALG(BYTE*pGryImg, int width, int height, int N, int M, BYTE *pResImg);