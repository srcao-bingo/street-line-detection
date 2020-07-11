#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "bmpFile.h"
//对于彩色图像，只要三个通道相同灰度值被分在一起，就保证了再分类的时候还是在一起。相当于三个灰度直方图合并成一个，只是高度变高了，内在联系还是一样的。
void CsrHistogramEqualize(BYTE*pGryImg, int width, int height, int channel = 1);
void CsrMeanStdDev(BYTE*pGryImg, int width, int height, double bright, double contrast);
void CsrRgb2GrayLUT(BYTE*pGryImg, int width, int height,BYTE *pResImg);
void CsrGrayExtend(BYTE*pGryImg, int width, int height);
int RmwRead14BitImgFile(short int * p14Img, int width, int height, const char * fileName);
void Csr14BitChangeTo8Bit(short int*pGryImg, int width, int height, BYTE *pResImg);
