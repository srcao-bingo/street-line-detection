#include"CsrALG_C3.h"
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;

void RmwAvrFilter2(BYTE *pGrylmg, //原始灰度图像
	int *pSumImg, //计算得到的积分图
	int width, int height,//图像的宽度和高度
	int M, int N, //滤波邻域M列N行
	BYTE *pResImg //结果图像
)
{
	int *pYl, *pY2;
	BYTE *pRes;
	int halfx, halfy;
	int x, y, x1, x2;
	int sum, c;
	// step.1------------初始化---------------------------//
	M = M/2 * 2 + 1; //奇数化
	N = N/2 * 2 + 1; //奇数化
	halfx = M/2; //滤波器的半径x
	halfy = N/2; //滤波器的半径y
	c = (1 << 23) / (M*N);//乘法因子
	// step.2------------滤波----------------------------//
	
	y = halfy + 1 ,pRes = pResImg + y * width, pYl = pSumImg, pY2 = pSumImg + N * width;
	for (;y < height - halfy;	y++, pYl += width, pY2 += width)
	{
		pRes += halfx + 1; //跳过左侧
		for (x = halfx + 1, x1 = 0, x2 = M; x < width - halfx; x++, x1++, x2++)
			//for(x1 = 0, x2 = M; x2<width; xl++, x2++) //上一行可以简化如此，但不太容易读
		{
			sum = *(pY2 + x2) - *(pY2 + x1) - *(pYl + x2) + *(pYl + x1); 
			
			*(pRes++) = (sum*c) >> 23; //用整数乘法和移位代替除法
		}
		pRes += halfx;//跳过右侧

	}
	return;
}
void CsrAvgRowFilter(BYTE*pGryImg, BYTE *pSumImg, int width, int height,int N,int M , BYTE *pResImg)
{
	int halfx, halfy,y,x,c,sum;
	int x1, x2;
	BYTE *pRes = pResImg;
	N = N / 2 * 2 + 1;
	M = M / 2 * 2 + 1;
	halfx = M / 2 ;
	c = (1 << 23) / (M*N);
	memcpy(pResImg, pGryImg, width*height);
	for (y = 0; y < height; y++, pSumImg += width)
	{
		pRes += halfx+1;//跳过左侧；
		for (x = halfx + 1,x1 = 0,x2 = M; x < width - halfx; x++,x1++,x2++)
		{
			sum = *(pSumImg + x2) - *(pSumImg + x1);

			*(pRes++) = ((sum*c) >> 23);
		}
		pRes += halfx;//跳过右侧

	}


}


//基于列和的一维均值滤波
void CsrSumColAvgFilter(BYTE*pGryImg, int width, int height, int M, BYTE *pResImg)
{
	int x, y, halfx;
	int sum;
	M = M / 2 * 2 + 1;
	halfx = M / 2;
	int C = (1 << 23) / M;
	BYTE *pCur = pGryImg, *pRes = pResImg;
	for (y = 0; y < height; y++)
	{
		//----------step1.初始化邻域总和----------//
		sum = 0;
		for (x = 0; x < M; x++)
		{
			sum += *(pCur + x);
		}
		//----------step2.求均值-----------------//
		pRes += halfx;//跳过左侧不完整像素
		for (x = halfx; x < width - halfx ; x++)
		{
			*(pRes++) = (sum*C) >> 23;
			sum -= *(pCur + x - halfx);//减去左边的列和
			sum += *(pCur + x + halfx + 1);//加上右边的列和
		}
		pRes += halfx;//跳过右侧不完整像素
		pCur += width;//换下一行
	}

}

//行积分图
void CsrGetRowSumImg(BYTE *pGryImg, int width, int height, int *pResImg)
{
	int x, y;
	BYTE*pCur = pGryImg;
	int *pRes = pResImg;//  int*
	int sumCol = 0;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			sumCol = *(pCur++);//更新列和
			if (!x)
			{
				*(pRes++) = sumCol;
			}
			else
			{
				*(pRes++) = sumCol + *(pRes - 1);//递推
			}
		}

	}

}
//行均值滤波
void CsrSumImgRowAvgFilter( int *pSumImg, int width, int height,int M, BYTE *pResImg)
{
	int x, y, halfx,sum,c;
	int x1, x2;
	BYTE *pRes = pResImg;
	int*pY = pSumImg;
	M = M / 2 * 2 + 1;
	halfx = M / 2;
	c = (1 << 23) / M;//乘法因子
	for (y = 0; y <height ; y++)
	{
		pRes += halfx+1;//跳过左侧不完整像素
		for (x = halfx+1,x1 = 0,x2 = M; x <width - halfx ; x++,x1++,x2++)
		{
			sum = *(pY + x2) - *(pY + x1);
			*(pRes++) = (sum*c) >> 23;

		}
		pRes += halfx;//跳过右侧不完整像素
		pY += width;//积分图指针换行
	}
}

//通用积分图
void CsrGetSumImg(BYTE *pGryImg, int width, int height, int *pResImg)
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


void CsrRgb2Gray_Factor(BYTE*pRgbImg, int width, int height, BYTE *pResImg)
{
	BYTE* pCur, *pEnd,pRes;
	memset(pResImg, 0, sizeof(BYTE)*width * height);
	// step.1----------生成乘法因子--------------------------//
	int cB = 0.114 * (1<<10);
	int cG = 0.587 * (1<<10);
	int cR = 0.299 * (1<<10);

	// step.2----------RGB转Gray---------------------------//
	pEnd = pRgbImg + width * height * 3;
	for (pCur = pRgbImg; pCur < pEnd;)
	{
		//移位优先级比加法还低，必须加括号！
		*(pResImg++) = (*(pCur) * cB + *(pCur+1) * cG + *(pCur+2) * cR)>>10;
		pCur += 3;
	}

}
void CsrRgb2Gray_Low(BYTE*pRgbImg, int width, int height, BYTE *pResImg)
{
	BYTE* pCur, *pEnd, pRes;
	memset(pResImg, 0, sizeof(BYTE)*width * height);
	// step.1----------RGB转Gray---------------------------//
	pEnd = pRgbImg + width * height * 3;
	for (pCur = pRgbImg; pCur < pEnd;)
	{
		*(pResImg++) = (*(pCur)* 0.114) + (*(pCur+1)* 0.587) + (*(pCur+2)* 0.299);
		pCur += 3;
	}

}

void CsrInvertPhaseSSE(BYTE*pGryImg, int width, int height)
{
	/*_declspec(align(16)) BYTE sumCol[16];//一般来说用_declspec(align(16)) 对变量进行十六字节对齐*/
	if ((unsigned int)pGryImg % 16 != 0)
	{
		printf("\n图片首地址必须是是16的倍数！\n");
		return;
	}
	if (width*height % 16 != 0)
	{
		printf("\n图片总像素数必须是16的倍数！\n");
		return;
	}
	__m128i subFactor,A;
	memset(&subFactor, -1, sizeof(__m128i));
	BYTE *pCur = pGryImg, *pEnd=pCur+width*height;
	//-------------一次16个像素反相-----------------//
	for (; pCur < pEnd; pCur += 16)
	{
					  A = _mm_loadu_si128((__m128i*)pCur);
		*(__m128i*)pCur = _mm_subs_epu8(subFactor, A);

	}
}

double CsrMedianFilter(BYTE*pGryImg, int width, int height, int N, int M, BYTE *pResImg)
{
	BYTE *pCur, *pRes;
	int i, j, g, x, y, y1, y2;
	int histogram[256];
	int wSize, left, right;
	int num = 0, med, v, halfx, halfy;
	int dbgCmpTimes = 0;

	//奇数化邻域
	N = N / 2 * 2 + 1;
	M = M / 2 * 2 + 1;
	halfx = M / 2, halfy = N / 2;
	//邻域内像素总个数
	wSize = M * N;
	//---------------step1.初始化直方图
	for (y = halfy, pRes = pResImg + y * width; y < height - halfy; y++)
	{
		y1 = y - halfy;
		y2 = y + halfy;
		memset(histogram, 0, sizeof(int) * 256);
		for (i = y1, pCur = pGryImg + i * width; i <= y2; i++, pCur += width)
			for (j = 0; j < M; j++)
				histogram[*(pCur + j)]++;

		//step2,初始化中值
		num = 0;
		for (g = 0; g < 256; g++)
		{
			num += histogram[g];
			if (2 * num > wSize)
			{
				med = g;
				break;
			}
		}
		//滤波
		pRes += halfx;
		for (x = halfx; x < width - halfx; x++)
		{
			//更新像素值
			*(pRes++) = med;
			//step3.直方图递推
			left = x - halfx;
			right = x + halfx + 1;
			for (i = y1, pCur = pGryImg + i * width; i <= y2; i++, pCur += width)
			{
				g = *(pCur + left);
				histogram[g]--;
				if (g <= med)
					num--;
				g = *(pCur + right);
				histogram[g]++;
				if (g <= med)
					num++;

			}
			if (2 * num < wSize)
			{
				//num小了，原来的med肯定不够,med得右移。
				//不管num减掉多少，num只是说从灰度值0，加到med的总像素个数.
				for (med = med + 1; med < 256; med++)
				{
					num += histogram[med];
					dbgCmpTimes += 2;
					if (2 * num > wSize)
					{
						break;
					}
				}
				dbgCmpTimes++;
			}
			else
			{
				while (2 * (num - histogram[med]) > wSize)
				{
					dbgCmpTimes++;
					num -= histogram[med];
					med--;
				}
				dbgCmpTimes += 2;
			}
		}

		pRes += halfx;

	}
	return dbgCmpTimes * 1.0 / ((width - halfx * 2)*(height - halfy * 2));

}

double CsrMinFilter(BYTE*pGryImg, int width, int height, int N, int M, BYTE *pResImg)
{
	BYTE *pCur, *pRes;
	int i, j, g, x, y, y1, y2;
	int histogram[256];
	int wSize, left, right;
	int num = 0, min, v, halfx, halfy;
	int dbgCmpTimes = 0;

	//奇数化邻域
	N = N / 2 * 2 + 1;
	M = M / 2 * 2 + 1;
	halfx = M / 2, halfy = N / 2;
	//邻域内像素总个数
	wSize = M * N;
	//---------------step1.初始化直方图
	for (y = halfy, pRes = pResImg + y * width; y < height - halfy; y++)
	{
		y1 = y - halfy;
		y2 = y + halfy;
		memset(histogram, 0, sizeof(int) * 256);
		for (i = y1, pCur = pGryImg + i * width; i <= y2; i++, pCur += width)
			for (j = 0; j < M; j++)
				histogram[*(pCur + j)]++;

		//step2,初始化最小值
		num = 0;
		for (g = 0; g < 256; g++)
		{
			num += histogram[g];
			if (num)
			{
				min = g;
				break;
			}
		}
		//滤波
		pRes += halfx;
		for (x = halfx; x < width - halfx; x++)
		{
			//更新像素值
			*(pRes++) = min;
			//step3.直方图递推
			left = x - halfx;
			right = x + halfx + 1;
			for (i = y1, pCur = pGryImg + i * width; i <= y2; i++, pCur += width)
			{
				g = *(pCur + left);
				histogram[g]--;
				if (g == min)//减去比min大的，min可能会变大
					num--;
				g = *(pCur + right);
				histogram[g]++;
				if (g <= min)//加上比min小的，向左移
					num++;

			}
			if (num <= 0 )//看num变大变小？
			{
				//num小了，原来的med肯定不够,med得右移。
				//不管num减掉多少，num只是说从灰度值0，加到med的总像素个数.
				for (min = min + 1; min < 256; min++)
				{
					num += histogram[min];
					dbgCmpTimes += 2;
					if ( num > 0)
					{
						break;
					}
				}
				dbgCmpTimes++;
			}
			else
			{
				while ( num - histogram[min] > 0)
				{
					dbgCmpTimes++;
					num -= histogram[min];
					min--;
				}
				dbgCmpTimes += 2;
			}
		}

		pRes += halfx;

	}
	return dbgCmpTimes * 1.0 / ((width - halfx * 2)*(height - halfy * 2));

}



double CsrMinFilter_Low(BYTE*pGryImg, int width, int height, int N, int M, BYTE *pResImg)
{
	BYTE *pCur, *pRes;
	int i, j, g, x, y, y1, y2;
	int histogram[256];
	int wSize, left, right;
	int num = 0, min, halfx, halfy;
	unsigned int count = 0;
	//参数初始化
	N = N / 2 * 2 + 1; M = M / 2 * 2 + 1;
	halfx = M / 2; halfy = N / 2;
	pRes = pResImg + halfy * width;

	for (y = halfy; y < height - halfy; y++)
	{
		//---------------step1.初始化直方图------------------//
		y1 = y - halfy;
		y2 = y + halfy;
		memset(histogram, 0, sizeof(int) * 256);
		for (i = y1, pCur = pGryImg + i * width; i <= y2; i++, pCur += width)
			for (j = 0; j < M; j++)
				histogram[*(pCur + j)]++;

		//---------------step2.初始化最小值------------------//
		for (g = 0; g < 256; g++)
		{
			if (histogram[g])
			{
				min = g;
				break;
			}
		}
		//---------------step3.最小值滤波-------------------//
		pRes += halfx;
		for (x = halfx; x < width - halfx; x++)
		{
			*(pRes++) = min;//更新像素值
			left = x - halfx;
			right = x + halfx + 1;
			for (i = y1, pCur = pGryImg + i * width; i <= y2; i++)
			{
				histogram[*(pCur + left)]--;//减去左边一列
				histogram[*(pCur + right)]++;//加上右边一列
				pCur += width;
			}
			for (g = 0; g < 256; g++)//重新找最小值
			{
				count++;//记录比较次数
				if (histogram[g])
				{
					min = g;
					break;
				}
			}
		}
		pRes += halfx;
	}
	return count*1.0/((width-M)*(height - N));
}

double CsrMinFilterALG(BYTE*pGryImg, int width, int height, int N, int M, BYTE *pResImg)
{
	BYTE *pCur, *pRes;
	int i, j, g, x, y, y1, y2;
	int histogram[256];
	int left, right;
	int min,halfx, halfy;
	unsigned int count = 0;//记录比较次数
	//奇数化邻域
	N = N / 2 * 2 + 1;	M = M / 2 * 2 + 1;
	halfx = M / 2;	halfy = N / 2;
	pRes = pResImg + halfy * width;
	for (y = halfy; y < height - halfy; y++)
	{
		//---------------step1.初始化直方图------------------//
		y1 = y - halfy;
		y2 = y + halfy;
		memset(histogram, 0, sizeof(int) * 256);
		for (i = y1, pCur = pGryImg + i * width; i <= y2; i++, pCur += width)
			for (j = 0; j < M; j++)
				histogram[*(pCur + j)]++;
		//---------------step2.初始化最小值------------------//
		for (g = 0; g < 256; g++)
		{
			if (histogram[g])
			{
				min = g;
				break;
			}
		}
		//---------------step3.最小值滤波-------------------//
		pRes += halfx;
		for (x = halfx; x < width - halfx; x++)
		{
			*(pRes++) = min;//更新像素值
			left = x - halfx;
			right = x + halfx + 1;
			//-----------直方图递推----------//
			for (i = y1, pCur = pGryImg + i * width; i <= y2; i++,pCur += width)
			{
				g = *(pCur + left);
				histogram[g]--;
				while (histogram[min] < 1)
				{
					count++;
					min++;
				}
				g = *(pCur + right);
				histogram[g]++;
				if (g < min) min = g;
				count += 2;
			}
		}
		pRes += halfx;
	}
	//---------------step4.返回比较次数-------------------//
	return count * 1.0 / ((width - M)*(height - N));
}




//Mat pSrc = imread("..\\0304Gry.bmp");

void Gaussian()
{
	int g, LUT1[256], LUT2[256],LUT3[256],LUT4[256];
	int T = (0.0111 * 2 + 0.1353 * 2 + 0.6065 * 2 + 1.0);
	int C1 = 0.0111 / T, C2 = 0.1353 / T, C3 = 0.6065 / T, C4 = 1.0 / T;
	for (g = 0 ;g < 256;g++)
	{
		LUT1[g] = C1 * g;
		LUT2[g] = C2 * g;
		LUT3[g] = C3 * g;
		LUT4[g] = C4 * g;
	}
	

}