#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "bmpFile.h"
void CsrEdgeSharpen(BYTE*pGryImg, int width, int height, double k, BYTE *pResImg);
void CsrSobel(BYTE*pGryImg, int width, int height, BYTE *pResImg);
void CsrSobelAndBinaryThreshold(BYTE*pGryImg, int width, int height,int T, BYTE *pResImg);
void Csr_Sobel_BinaryThreshold_CombineEdege(BYTE*pGryImg, BYTE*pSjImg, int width, int height, int T, BYTE *pResImg);
void CsrShenJunOperator(BYTE*pGryImg, BYTE*pTmpImg, int width, int height, double a0, BYTE *pResImg);
void CsrShenJunOperator2(BYTE*pGryImg, BYTE*pTmpImg, int width, int height, double a0, BYTE *pResImg);
void CsrBinaryThreshold(BYTE*pGryImg, int width, int height, int T, BYTE *pResImg);
void CsrImgAND(BYTE*pGryImg1, BYTE*pGryImg2, int width, int height, BYTE *pResImg);
void CsrImgCombine(BYTE*pGryImg, BYTE *pEdgeImg, int width, int height, BYTE *pResImg);
void CsrReduceImgSize(BYTE*pGryImg, int width, int height, int nFast_0123, BYTE *pResImg);
void CsrSearchMaxRectA(int* pSumImg, int width, int height, int w, int h, int delLR, int*xL, int*xR, int*yL, int*yR);
void CsrDrawRect(BYTE*pGryImg, int width, int height, int& xL, int& xR, int& yL, int& yR,BYTE*pResImg);
void CsrDealBoundary(BYTE*pGryImg, int width, int height, int N, int M, BYTE *pResImg);

























