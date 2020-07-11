
#include"CsrALG_C4.h"
//#include<opencv2/opencv.hpp>
//using namespace cv;
#if 0
void CsrEdgeSharpen(BYTE*pGryImg, int width, int height, int k, BYTE *pResImg)
{
	if (k <= 0)
	{
		printf("锐化倍数必须大于零！\n");
		return;
	}
	int i, j, g;
	int x, y;
	int c, modifyVal;
	unsigned int row;
	Mat pSrc = imread("..\\0302Gry.bmp");
	BYTE *pCur = pGryImg+width+1, *pRes = pResImg+width+1;
	c = (k << 10);//乘法因子
	//1.程序访问越界  2.图像结果不对
	for (y = 1; y < height - 1; y++)
	{
		for (x = 1; x < width - 1; x++)
		{
			//modifyVal = *(pCur) - ((*(pCur - 1) + *(pCur + 1) + *(pCur - width) + *(pCur + width) 
				//+ *(pCur - 1 - width) + *(pCur + 1 - width) + *(pCur - 1 + width) + *(pCur + 1 + width))>>3);
			/*int tmp1 = *(pCur);
			int tmp2 = *(pCur - 1);
			int tmp3 = *(pCur + 1);
			int tmp4 = *(pCur - width);*/

			row = (unsigned int)(pCur + x - pGryImg) / width;
			
			//int tmp5 = *(pCur + width);
			
			modifyVal = *(pCur)-((*(pCur + x - 1) + *(pCur + x + 1) + *(pCur + x - width) + *(pCur + x + width)) >> 2);
			
			int temp = ((modifyVal*c) >> 10);
			//int temp2 = *(pCur++) + ((modifyVal*c) >> 10);
			//int tmp = max(0, min(255, *(pCur+x) + modifyVal));
			*(pRes+x) = max(0,min(255,*(pCur+x) + ((modifyVal*c)>>10)));
			//*(pRes+x) = *(pCur+x)+((modifyVal * c) >> 10);
		}
		pCur += width;
		pRes += width;
	}

	return;
}
#endif

//1.图像结果不对，因为没有进行饱和运算。
//2.因为是八邻域，用列和其实效果不明显。卷积尺寸会改变的时候，考虑列和，大尺寸算的快。
void CsrEdgeSharpen(BYTE*pGryImg, int width, int height, double k, BYTE *pResImg)
{
	if (k <= 0)
	{
		printf("锐化倍数必须大于零！\n");
		return;
	}
	int x, y;
	int modifyVal, factor;//K可以是浮点数，但放大因子定义为int，只有整数才能移位
	BYTE *pCur = pGryImg + width;
	BYTE *pRes = pResImg + width;
	factor = ((1<<10) * k ) / 8;//乘法因子
	for (y = 1; y < height - 1; y++)
	{
		pCur++; pRes++;
		for (x = 1; x < width - 1; x++)
		{
			//----------------step1.计算修正量--------------------//
			modifyVal = (*(pCur)<<3) - ( *(pCur - 1) + *(pCur + 1) + *(pCur - width) + *(pCur + width) 
						                  + *(pCur - 1 - width) + *(pCur + 1 - width) + *(pCur - 1 + width) + *(pCur + 1 + width) );

			//----------------step2.锐化-------------------------//
			*pRes = max(0, min(255, *pCur + ((modifyVal * factor) >> 10)));
			pCur++; pRes++;//不要在 max , min 里面使用自增自减，边界检查时会被优化掉！！！！！！！！！！！！！！！
		}
		pCur++; pRes++;
	}
			//----------------step3.边界处理--------------------//
	CsrDealBoundary(pGryImg, width, height, 3, 3, pResImg);
	return;
}
//N行M列
void CsrDealBoundary(BYTE*pGryImg, int width, int height,int N,int M, BYTE *pResImg)
{
	BYTE *pCur,*pRes;
	int x,y,halfx, halfy;//半径与偏移量
	halfx = M / 2;
	halfy = N / 2;
	//首行
	memcpy(pResImg, pGryImg, sizeof(BYTE)*width*halfy);
	//尾行
	memcpy(pResImg + (height - halfy)*width, pGryImg + (height - halfy)*width, sizeof(BYTE)*width*halfy);
	//首列,尾列
	for (y = 0, pCur = pGryImg, pRes = pResImg; y < height; y++, pCur += width, pRes += width)
	{
		for (x = 0; x < halfx; x++)
			*(pCur + x) = *(pRes + x);
		*(pCur + width - 1 - x) = *(pRes + width - 1 - x);
	}
	return;
}

void CsrSobel(BYTE*pGryImg, int width, int height, BYTE *pResImg)
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
			dx =  *(pCur - 1 - width) + (*(pCur - 1) << 1) + *(pCur - 1 + width)
			    - *(pCur + 1 - width) - (*(pCur + 1) << 1) - *(pCur + 1 + width);
			//2.求dy
			dy =  *(pCur - 1 - width) + (*(pCur - width) << 1) + *(pCur + 1 - width)
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


void CsrSobelAndBinaryThreshold(BYTE*pGryImg, int width, int height, int T ,BYTE *pResImg)
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
			*(pRes++) = ((abs(dx) + abs(dy)) >= T) * 255;//这里饱和运算是多余的，因为是根据比较的结果赋值
			*pCur++;
		}
		pCur++;
		//尾列不做
		*(pRes++) = 0;
	}
	//尾行不做
	memset(pRes, 0, sizeof(BYTE)*width);
}



void Csr_Sobel_BinaryThreshold_CombineEdege(BYTE*pGryImg, BYTE*pSjImg,int width, int height, int T, BYTE *pResImg)
{
	int x, y,dx, dy;
	BYTE *pCur = pGryImg + width;
	BYTE *pRes = pResImg + width;
	BYTE *pSj = pSjImg + width;
	//首行不做
	memset(pResImg, 0, sizeof(BYTE)*width);
	for (y = 1; y < height - 1; y++)
	{
		//首列不做
		*(pRes++) = 0;
		pCur++;
		pSj++;
		for (x = 1; x < width - 1; x++)
		{
			//1.求dx
			dx = *(pCur - 1 - width) + (*(pCur - 1) << 1) + *(pCur - 1 + width)
				- *(pCur + 1 - width) - (*(pCur + 1) << 1) - *(pCur + 1 + width);
			//2.求dy
			dy = *(pCur - 1 - width) + (*(pCur - width) << 1) + *(pCur + 1 - width)
				- *(pCur - 1 + width) - (*(pCur + width) << 1) - *(pCur + 1 + width);
			//3.求边缘强度并二值化
			*pRes = ((abs(dx) + abs(dy)) >= T) * 255;//这里饱和运算是多余的，因为是根据比较的结果赋值
			//4.结合边缘图，去除沈俊中的噪声
			*pRes = *pRes & *pSj;
			pCur++; pRes++; pSj++;
		}
		pSj++;
		pCur++;
		//尾列不做
		*(pRes++) = 0;
	}
	//尾行不做
	memset(pRes, 0, sizeof(BYTE)*width);
}

/*if (*(pTmp - 1 - width) <= 0 || *(pTmp - width) <= 0 ||
*(pTmp + 1 - width) <= 0 || *(pTmp - 1) <= 0 ||
*(pTmp + 1) <= 0 || *(pTmp - 1 + width) <= 0 ||
*(pTmp + width) <= 0 || *(pTmp + 1 + width) <= 0
)
*/
void CsrShenJunOperator(BYTE*pGryImg, BYTE*pTmpImg, int width, int height, double a0, BYTE *pResImg)
{
	int x, y, diff;
	int pre, LUT[512];
	int * ALUT = LUT + 256;//a0查找表
	BYTE *pCur = pGryImg, *pRes = pResImg, *pTmp = pTmpImg, *pGry, *pEnd;
	//----------------step1.生成a0查找表--------------------//
	for (diff = 0; diff < 256; diff++)
	{
		ALUT[diff] = (int)(diff * a0 + 0.5);//四舍五入！!!!!!!!!!!!!!!!
		ALUT[-diff] = (int)(-diff * a0 - 0.5);
	}
	//----------------step2.生成滤波图像--------------------//
	for (y = 0; y < height; y++)
	{
		//从左往右
		*(pTmp++) = pre = *(pCur++);
		for (x = 1; x < width; x++)
		{
			*(pTmp++) = pre = pre + ALUT[*(pCur++) - pre];
		}
		//从右往左
		pTmp -= 2;//先自减到倒数第二个
		for (x = width - 2; x >= 0; x--)
		{
			*(pTmp--) = pre = pre + ALUT[*(pTmp)-pre];
		}
		pTmp += (1 + width);//换下一行
	}

	for (x = 0, pTmp = pTmpImg; x < width; x++, pTmp = pTmpImg + x)
	{
		//从上往下
		pre = *(pTmp);
		for (y = 1, pTmp += width; y < height; y++, pTmp += width)
		{
			*(pTmp) = pre = pre + ALUT[*(pTmp)-pre];
		}
		//从下往上
		pre = *(pTmp - width);
		for (y = height - 2, pTmp -= 2 * width; y >= 0; y--, pTmp -= width)//-2*width
		{
			*(pTmp) = pre = pre + ALUT[*(pTmp)-pre];
		}

	}

	//----------------step3.求出二阶导数--------------------//
	pEnd = pTmpImg + width * height;
	for (pCur = pTmpImg, pGry = pGryImg; pCur < pEnd; pCur++, pGry++)
	{
		*pCur = max(0, *pCur - *pGry);//BYTE是无符号整数，不可能小于零，会溢出！！！！！！！！！！！！！！！
	}
	//----------------step4.找出过零点---------------------//
	memset(pResImg, 0, sizeof(BYTE)*width*height);
	pCur = pTmpImg + width; pRes = pResImg + width;
	for (y = 1; y < height - 1; y++)
	{
		pRes++; pCur++;
		for (x = 1; x < width - 1; x++, pCur++, pRes++)
		{
			if (*pCur)//只要不是0，就检查其邻域是否有二阶导数为0的像素。
			{		  //因为饱和运算把小于零的赋为0
				if ((!*(pCur - width)) ||
					(!*(pCur - 1)) ||
					(!*(pCur + 1)) ||
					(!*(pCur + width))
					)
				{
					*pRes = 255;
				}
			}

		}
		pRes++; pCur++;
	}

	return;
}

void CsrBinaryThreshold(BYTE*pGryImg, int width, int height,int T, BYTE *pResImg)
{
	BYTE *pCur = pGryImg, *pRes = pResImg, *pEnd = pGryImg + width * height;
	for (; pCur < pEnd;pCur++,pRes++)
	{
		*pRes = (*pCur >= T) * 255;//最简洁！
	}
	return;

}

void CsrImgAND(BYTE*pSjImg, BYTE*pSbImg, int width, int height, BYTE *pResImg)
{
	BYTE *pCur = pSjImg, *pRes = pResImg, *pEnd = pSjImg + width * height;
	BYTE *pGry = pSbImg;
	for (; pCur < pEnd; pCur++, pRes++,pGry++)
	{
		*pRes = *pCur & *pGry;

	}
	return;

}
void CsrImgCombine(BYTE*pGryImg, BYTE *pEdgeImg,int width, int height, BYTE *pResImg)
{
	int x, y;
	BYTE *pGry = pGryImg,*pCur = pEdgeImg, *pRes = pResImg, *pEnd = pEdgeImg + width * height;
	for (; pCur < pEnd; pCur++, pRes++, pGry++)
	{
		if (*pGry >= 255) *pGry = 250;//修改原图255的像素

		*pRes = *pCur < 255 ? *pGry : *pCur;//边缘图的像素如果是边缘，则把原图对应位置改为255
	}

	return;
}

//随机抽样
void CsrReduceImgSize(BYTE*pGryImg, int width, int height,int nFast_0123, BYTE *pResImg)
{
	int x, y,scale,num;
	BYTE *pCur = pGryImg, *pRes = pResImg, *pEnd = pGryImg + width * height;
	nFast_0123 = max(0, min(3, nFast_0123));
	scale = (int)(pow(2,nFast_0123) + 0.5);
	for (y = 0; y < height; y += scale,pCur += scale*width)
	{
		for (x = 0; x < width; x += scale)
		{
			*(pRes++) = *(pCur + x);
		}
	}

}
//得到目标区域在压缩图像上的坐标
void CsrSearchMaxRectA(int* pSumImg,int width,int height,int w,int h,int delLR,int*xL, int*xR, int*yL,int*yR)
{
	int x, y, x1, x2;
	int *pY1 , *pCur, *pY2, delH;
	int  sumC, sumL, sumR, sumU, maxVal,diff;
	maxVal = 0, y = 1;
	delH = delLR * width;//sumU区域的高度
	pY1 = pSumImg + (y + delLR)*width;
	pY2 = pSumImg + (y + h + delLR)*width;
	for (y = 1; y < height - h - delLR; y++,pY1 += width,pY2 += width)
	{
		for (x = 1,x1 = x + delLR, x2 = x + w + delLR; x < width - w - delLR*2; x++,x1++,x2++)
		{
			//-------step1.求各区域梯度强度--------//
			sumC = *(pY2 + x2) - *(pY1 + x2) - *(pY2 + x1) + *(pY1 + x1);//Center的梯度强度

			sumL = *(pY2 + x1) - *(pY2 + x1 - delLR) - *(pY1 + x1) + *(pY1 + x1 - delLR);

			sumR = *(pY2 + x2 + delLR) - *(pY2 + x2) - *(pY1 + x2 + delLR) + *(pY1 + x2);

			sumU = *(pY1 + x2) - *(pY1 + x1) - *(pY1 - delH + x2 ) + *(pY1 - delH + x1);

			diff = sumC - sumU - sumL - sumR;
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
	*xR = *xL + w;
	*yR = *yL + h;
	return;
}

void CsrDrawRect(BYTE*pGryImg, int width, int height, int& xL, int& xR, int& yL, int& yR,BYTE*pResImg)
{
	int x, y;
	BYTE *pGry;
	//上边界
	pGry = pResImg + yL * width;
	for (x = xL; x <= xR; x++) *(pGry+x) = 255;
	//下边界
	pGry = pResImg + yR * width;
	for (x = xL; x <= xR; x++)
	{
		*(pGry + x) = 255;
	}
	//左边界
	pGry = pResImg + yL * width + xL;
	for (y = yL; y <= yR; y++, pGry += width)
	{
		*(pGry) = 255;

	}//右边界
	pGry = pResImg + yL * width + xR;
	for (y = yL; y <= yR; y++, pGry += width)
	{
		*(pGry) = 255;
	}
}

void CsrShenJunOperator2(BYTE*pGryImg, BYTE*pTmpImg, int width, int height, double a0, BYTE *pResImg)
{
	int x, y, diff;
	int pre, LUT[512];
	int * ALUT = LUT + 256;//a0查找表
	BYTE *pCur, *pRes , *pGry;
	//----------------step1.生成a0查找表--------------------//
	for (ALUT[0] = 0,diff = 1; diff < 256; diff++)
	{
		ALUT[diff] = (int)(diff * a0 + 0.5);//四舍五入！!!!!!!!!!!!!!!!
		ALUT[-diff] = (int)(-diff * a0 - 0.5);
	}
	//----------------step2.生成滤波图像--------------------//
	for (y = 0,pGry = pGryImg,pCur = pTmpImg; y < height; y++)
	{
		//从左往右
		*(pCur++) = pre = *(pGry++);
		for (x = 1; x < width; x++,pGry++)
		{
			*(pCur++) = pre = pre + ALUT[*pGry - pre];
		}
		//从右往左
		pre = *(--pGry);//----------------------------------------------------------
		for (x = width - 2, pCur -= 1; x >= 0; x--)
		{
			*(pCur--) = pre = pre + ALUT[*pCur - pre];
		}
		pCur += (width + 1);//----------------------------------------------------
	}

	for (x = 0, pCur = pTmpImg; x < width; x++, pCur = pTmpImg + x)//
	{
		//从上往下
		pre = *pCur;
		for (y = 1, pCur += width; y < height; y++, pCur += width)
		{
			*(pCur) = pre = pre + ALUT[*pCur - pre];
		}
		//从下往上
		pre = *(pCur - width);
		for (y = height - 2, pCur -= 2 * width; y >= 0; y--, pCur -= width)//-2*width
		{
			*pCur = pre = pre + ALUT[*pCur-pre];
		}
		//pTmp += width;
	}
	
	//----------------step3.找出过零点---------------------//
	memset(pResImg, 0, sizeof(BYTE)*width*height);
	pCur = pTmpImg + width; pRes = pResImg + width; pGry = pGryImg + width;
	for (y = 1; y < height - 1; y++)
	{
		pRes++; pGry++; pCur++;
		for (x = 1; x < width - 1; x++,pGry++, pCur++, pRes++)
		{
			if (*pCur - *pGry > 0)
			{
				/*if (*(pTmp - 1 - width) <= 0 || *(pTmp - width) <= 0 ||
				*(pTmp + 1 - width) <= 0 || *(pTmp - 1) <= 0 ||
				*(pTmp + 1) <= 0 || *(pTmp - 1 + width) <= 0 ||
				*(pTmp + width) <= 0 || *(pTmp + 1 + width) <= 0
				)
				*/
				if ((*(pCur - width) - *(pGry -width) <= 0 )||
					(*(pCur - 1) - *(pGry - 1) <= 0 )||
					(*(pCur + 1) - *(pGry + 1) <= 0 )||
					(*(pCur + width) - *(pGry + width) <= 0) 
					)
				{
					*pRes = 255;
				}
			}

		}
		pRes++; pGry++; pCur++;
	}
	return;
}


//Mat pSrc = imread("..\\0302Gry.bmp");













































