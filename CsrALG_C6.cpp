#include"CsrALG_C6.h"

int CsrGetBoundary(BYTE*pGryImg, int width, int height,int *xPos, int *yPos,BYTE* pResImg)
{
	int deep = 30;
	int x, y,count = 0;
	BYTE *pCur, *pRes,*pEndGry,*pEndRes;
	pCur = pGryImg; 
	pRes = pResImg;
	pEndGry = pGryImg + width * (height - 1);//尾行首元素
	pEndRes = pResImg + width * (height - 1);
	//按行
	for (y = 0; y < height; y++)
	{
		//从左往右
		for (x = 0; x < width; x++)
		{
			if (*(pCur + x) < 1)
			{
				//if (x > deep) continue;

				*(pRes + x) = 0;
				xPos[count] = x;
				yPos[count++] = y;
				break;
			}
		}
		//从右往左
		for (x = width - 1; x >= 0; x--)
		{
			if (*(pCur + x) < 1)
			{
				//if (x < width - deep) continue;

				*(pRes + x) = 0;
				xPos[count] = x;
				yPos[count++] = y;
				break;
			}
		}
		//换行
		pCur += width;
		pRes += width;
	}
	//按列
	for (x = 0;x < width;x++)
	{
		//从上往下
		pCur = pGryImg + x; 
		pRes = pResImg + x;
		for (y = 0; y < height; y++,pCur += width, pRes += width )
		{
			if (*pCur < 1)
			{
				//if (y > deep) continue;//排除内部点的干扰，因为内部共线的点

				*pRes = 0;
				xPos[count] = x;
				yPos[count++] = y;
				break;
			}
		}
		//从下往上
		pCur = pEndGry + x;
		pRes = pEndRes + x;
		for (y = height - 1; y >= 0; y--, pCur -= width, pRes -= width)
		{
			if (*pCur < 1)
			{
				//if (y < height - deep) continue;

				*pRes = 0;
				xPos[count] = x;
				yPos[count++] = y;
				break;
			}
		}
	}
	//Mat pMat = Mat(height, width, CV_8UC1, pGryImg);
	//Mat RMat = Mat(height, width, CV_8UC1, pResImg);


	return count;
}
#define PI 3.1415926 

void CsrGetSinVCosV(int minAngle,int maxAngle,int*sinV,int*cosV)
{
	int i,offset;
	double rad = PI / 180;
	offset = abs(minAngle);
	if (minAngle > maxAngle)
	{
		i = maxAngle;
		maxAngle = minAngle;
		minAngle = i;
	}
	for (i = minAngle; i < maxAngle; i++)
	{
		sinV[i + offset] = (1 << 11) * sin(i * rad);
		cosV[i + offset] = (1 << 11) * cos(i * rad);
	}
	return;
}
void getDiagVal(int *x, int*y, int n,int *diagVal)
{
	int i;
	for (i = 0; i < n; i++)
	{
		diagVal[i] = sqrt(x[i] * x[i] + y[i] * y[i]);
	}
}

#if 0 // backUp
void CsrHoughTransform(int*x, int*y, int *sinV, int*cosV, int *diagVal, int n, int(*count)[ANGLE], int diag)
{
	int tmpx[300] = { 0 };
	int tmpy[300] = { 0 };
	int tmpCount = 0;
	int wSize = 3;
	int theta, thro, i, j;
	int step_theta = 1;
	for (theta = 0; theta < ANGLE; theta += step_theta)
	{
		for (i = 0; i < n; i++)
		{
			thro = (x[i] * cosV[theta] + y[i] * sinV[theta]) >> 11;
			if (thro == 126 && theta == 90)
			{
				tmpx[tmpCount] = x[i];
				tmpy[tmpCount++] = y[i];
			}
			if (thro < diag)
			{
				count[thro][theta]++;
			}
		}
	}

	const int pair = 4;
	int max, num = 0;
	int resThro[pair];
	int resTheta[pair];
	while (num < pair)
	{
		max = 0;
		for (i = 0; i < diag; i++)
		{
			for (j = 0; j < ANGLE; j++)
			{
				if (count[i][j] > max)
				{
					resThro[num] = i;
					resTheta[num] = j;
					max = count[i][j];
				}
			}

		}
		//把附近的参数去掉，避免重复，因为边界不是一条直线，会错开一两个像素。破碎图，略微有偏差。
		for (i = resThro[num] - wSize; i <= resThro[num] + wSize; i++)
		{
			for (j = resTheta[num] - wSize; j <= resTheta[num] + wSize; j++)
			{
				count[i][j] = 0;
			}
		}
		num++;

	}
	BYTE *pResImg = new BYTE[diag*ANGLE];
	BYTE *pRes = pResImg;
	for (i = 0; i < diag; i++)
	{
		for (j = 0; j < ANGLE; j++)
		{
			*(pRes++) = min(255, count[i][j] * 255 / 30);
		}
	}
	Rmw_Write8BitImg2BmpFile(pResImg, ANGLE, diag, "..\\0606BinBoundary.bmp");

}
#endif
void CsrHoughTransform(int*x,int*y,int *sinV,int*cosV,int *throMax, int n, int(*count)[ANGLE],int shiftRigthBits)
{
	int theta,thro,i;
	int step_theta = 1;
	//--------------step1.Hough------------------------//
	for (theta = 0; theta < ANGLE; theta += step_theta)
	{
		for (i = 0; i < n; i++)
		{
			thro = (x[i] * cosV[theta] + y[i] * sinV[theta]) >> shiftRigthBits;
			if (thro >= 0 && thro <= *throMax)//因为是屏幕坐标系，thro是非负且不超过图像对角线的距离
			{
				count[thro][theta] += 1;
			}
		}
	}
	//Mat pMat = Mat(throMax, ANGLE, CV_32SC1, count);
}


void CsrShowCountImg(BYTE*pCountImg, int width, int height, int(*count)[ANGLE],int n)
{
	int i, j;
	BYTE *pRes = pCountImg;
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < ANGLE; j++)
		{
			*(pRes++) = min((BYTE)255, (BYTE)(count[i][j] * 255 / 30));//放缩增加亮度
		}
	}
	CsrGrayExtend(pCountImg, ANGLE, width);
	Rmw_Write8BitImg2BmpFile(pCountImg, ANGLE, width, "..\\0601CountImg.bmp");
}

int CountColProject[ANGLE] = { 0 };
//按列投影两个两个找，效果不好。
void CsrFindParametersCountProjection(int(*count)[ANGLE],int maxThro,int*theta,int * thro)
{
	int i, j, pair = 4;
	int CountRowProject[2048] = { 0 };
	

	//Mat pMat = Mat(maxThro, ANGLE, CV_32SC1, count);
	//-----------------step1.按列投影----------------------------//
	for (i = 0; i < maxThro/2; i++)//整个按列投影没有波峰，前90°都是平的。所以只投一半
	{
		for (j = 0; j < ANGLE; j++)
		{
			CountColProject[j] += count[i][j];
		}
	}

	int sum = 0;
	for (i = 0; i < maxThro; i++)
	{
		sum += count[i][12];
	}
	//-----------------step2.按行投影----------------------------//
	for (i = 0; i < maxThro; i++)
	{
		for (j = 0; j < ANGLE; j++)
		{
			CountRowProject[i] += count[i][j];
		}
	}
	//-----------------step3.分别找到最大的两个thro和theta-------//
#if 1
	BYTE pGryImg[ANGLE*ANGLE];
	RmwDrawData2Img(CountColProject, ANGLE, 2, pGryImg, ANGLE, ANGLE);
	Rmw_Write8BitImg2BmpFile(pGryImg, ANGLE, ANGLE, "..\\0601res2.bmp");
#endif
#if 1 
	BYTE pResImg[ANGLE*ANGLE*4];
	RmwDrawData2Img(CountRowProject, maxThro, 2, pResImg, ANGLE*2, ANGLE*2);
	Rmw_Write8BitImg2BmpFile(pResImg, ANGLE*2, ANGLE*2, "..\\0601res3.bmp");
#endif
	CsrFind1DMaxValSequence(CountRowProject, maxThro, pair, 10, thro);

	CsrFind1DMaxValSequence(CountColProject, ANGLE, pair, 5, theta);

	printf("0601:");
	for (i = 0; i < pair; i++)
	{
		for (j = 0; j < pair; j++)
		{
			printf("\nthro = %d  ；theta = %d \n" , thro[i],theta[j]);
		}
	}
}
//找到前几个最大值
void CsrFind1DMaxValSequence(int * pOrigin, int width,const int pair,int wSize,int *pResult)
{
	int i,j,max = 0,num = 0,index;
	for (j = 0; j < pair; j++)
	{
		max = 0;
		for (i = 0; i < width; i++)
		{
			if (pOrigin[i] > max)
			{
				max = pOrigin[i];
				index = i;
			}
		}
		pResult[num++] = index;
		//pOrigin[index] = 0;
		memset(pOrigin + index - (wSize>>1), 0, sizeof(int)*wSize);
	}
}

void CsrFindParametersThroughCount(int (*count)[ANGLE],int n,int maxThro,int wSize,int pair ,int * resThro,int *resTheta,int stepThro)
{
	int max,i,j, num = 0;
	int maxCount[256];
	//--------------step1.根据次数筛选---------------------//
	while (num < pair)
	{
		max = 0;
		for (i = 0; i < maxThro; i++)
		{
			for (j = 0; j < ANGLE; j++)
			{
				if (count[i][j] > max)
				{
					resThro[num] = i;
					resTheta[num] = j;
					max = count[i][j];
					maxCount[num] = max;
				}
			}
		}
		//把附近的参数去掉，避免重复，因为边界不是一条直线，会错开一两个像素。破碎图，略微有偏差。
		for (i = resThro[num] - wSize; i <= resThro[num] + wSize; i++)
		{
			for (j = resTheta[num] - wSize; j <= resTheta[num] + wSize; j++)
			{
				count[i][j] = 0;
			}
		}
		num++;
	}
	//--------------step2.输出结果------------------------//
	printf("0606:");
	for (i = 0; i < num; i++)
	{
		resThro[i] = resThro[i] << stepThro;//精度低的时候，thro靠近原点，把thro还原，便于绘制
		printf("\nthro = %d ；theta = %d 次数 = %d \n", resThro[i], resTheta[i],maxCount[i]);
	}
}

void drawHoughLine(BYTE *pGryImg, int width, int height,int pair,int thro,int theta)
{
	int x, y;
	BYTE*pCur;
	pCur = pGryImg; 
	int sinV = sin(PI / 180 * theta) * 2048;
	int cosV = cos(PI / 180 * theta) * 2048;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			if (((x * cosV + y*sinV)>>11) == thro )//对直线上任意一点，x*cos(theta) + y*sin(theta) = thro
			{
				*pCur = 255;
			}
			*pCur++;
		}
	}
}
//debug
void CsrHoughTransform(BYTE* pGryImg,int width,int height,int*x, int*y, int *sinV, int*cosV, int *diagVal, int n, int(*count)[ANGLE], int diag)
{
	int tmpx[300] = { 0 };
	int tmpy[300] = { 0 };
	int tmpCount = 0;
	int theta, thro, i, j;
	double step_theta = 1;
	for (theta = 0; theta < ANGLE; theta += step_theta)
	{
		for (i = 0; i < n; i++)
		{
			thro = (x[i] * cosV[theta] + y[i] * sinV[theta]) >> 11;
			if (thro == 11)
			{
				tmpx[tmpCount] = x[i];
				tmpy[tmpCount++] = y[i];
			}
			if (thro < diag)
			{
				count[thro][theta]++;
			}
		}
	}
	//Mat RMat = Mat(height, width, CV_8UC1, pGryImg);
	const int pair = 6;
	int max, num = 0;
	int resThro[pair];
	int resTheta[pair];
	while (num < pair)
	{
		max = 0;
		for (i = 0; i < diag*2; i++)
		{
			for (j = 0; j < ANGLE; j++)
			{
				if (count[i][j] > max)
				{
					resThro[num] = i;
					resTheta[num] = j;
					max = count[i][j];
				}
			}

		}
		count[resThro[num]][resTheta[num]] = 0;
		num++;
	}
	BYTE *pResImg = new BYTE[diag*ANGLE];
	BYTE *pRes = pResImg;
	for (i = 0; i < diag*2; i++)
	{
		for (j = 0; j < ANGLE; j++)
		{
			*(pRes++) = min(255, count[i][j] * 255 / 30);
		}
	}
Rmw_Write8BitImg2BmpFile(pResImg, ANGLE, diag, "..\\0602Res.bmp");

//Mat rMat = Mat(diag, ANGLE, CV_32SC1, count);
}


int findMaxPair(int(*count)[ANGLE], int diag)
{

	const int pair = 4;
	int i, j, max, num = 0;
	int resThro[pair];
	int resTheta[pair];
	while (num < pair)
	{
		max = 0;
		for (i = 0; i < diag; i++)
		{
			for (j = 0; j < ANGLE; j++)
			{
				if (count[i][j] > max)
				{
					resThro[num] = i;
					resTheta[num] = j;
					max = count[i][j];
				}
			}

		}
		count[resThro[num]][resTheta[num]] = 0;
		num++;
	}
	return count[resThro[0]][resTheta[0]];
}


void CsrShowTwoDimensionData(int(*count)[ANGLE], int width, int height, int maxVal, BYTE*pResImg)
{


	//Rmw_Write8BitImg2BmpFile()


}

void CsrHoughTransformImg(BYTE*pGryImg, int height, int width, int *sinV, int*cosV, int diag, int(*count)[ANGLE])
{
	int theta, thro, x, y;
	double step_theta = 1;
	BYTE*pCur = pGryImg;

	for (y = 0; y < width; y++)
	{
		for (x = 0; x < height; x++)
		{
			if (*pCur < 1)
			{
				for (theta = 0; theta < ANGLE; theta += step_theta)
				{
					thro = (x * cosV[theta] + y * sinV[theta]) >> 11;
					if (thro < diag && thro > -diag)
					{
						count[thro][theta]++;
					}
				}
			}
			pCur++;
		}
	}
	//Mat pMat = Mat(ANGLE, diag, CV_32SC1, count);

	int a = 0;
}

int CsrGetEdgePoint(BYTE * pGryImg, int width, int height, int*xPos, int*yPos)
{
	int x, y, count = 0;
	BYTE*pCur = pGryImg;
	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++)
		{
			if (*pCur < 1)
			{
				xPos[count] = x;
				yPos[count++] = y;
			}
			pCur++;
		}
	return count;
}
void CsrCircleHough(int *x, int *y, int n, int a, int b, int*r, int err)
{
	int i, j, Val, max;
	int countR[512];
	for (j = *r - err; j < *r + err; j++)
	{
		for (i = 0; i < n; i++)
		{
			//完全等于半径的点太少，可在一定误差范围内即可
			if (abs((x[i] - a)*(x[i] - a) + (y[i] - a)*(y[i] - a) - j * j)< 10)
			{
				countR[j]++;
			}
		}
	}
	for (i = 0, max = 0; i < 512; i++)
	{
		if (countR[i] > max)
		{
			max = countR[i];
			*r = i;
		}
	}

}

void CsrCircleHough(int *x, int *y, int n, int a, int b, int*r, int err,int *countR,int dimen)
{
	int i, j, Val, max;
	for (j = *r - err; j < *r + err; j++)//在误差范围内寻找半径
	{
		for (i = 0; i < n; i++)
		{
			//完全等于半径的点太少，可在一定误差范围内即可
			if (abs((x[i] - a)*(x[i] - a) + (y[i] - a)*(y[i] - a) - j * j )< 10)
			{
				countR[j]++; 
			}
		}
	}
	for (i = 0,max = 0; i < dimen; i++)
	{
		if (countR[i] > max)
		{
			max = countR[i];
			*r = i;
		}
	}
	
}


int CsrGetCircleXpos(BYTE*pGryImg,int width,int height,BYTE *pResImg)
{
	int x, y,i, max = 0, a = 0,flagLR;
	int xLeft, xRight, xCount[2048] = {0};
	BYTE *pCur, *pRes;
	pCur = pGryImg;
	pRes = pResImg;
	//按行
	for (y = 0; y < height; y++)
	{
		//从左往右
		for (x = 0; x < width; x++)
		{
			if (*(pCur + x) < 1)
			{
				*(pRes + x) = 0;
				xLeft = x;
				flagLR++;
				break;
			}
		}
		//从右往左
		for (x = width - 1; x >= 0; x--)
		{
			if (*(pCur + x) < 1)
			{
				*(pRes + x) = 0;
				xRight = x;
				flagLR++;
				break;
			}
		}
		if(flagLR>1)
		xCount[(xLeft + xRight) >> 1]++;
		//换行
		pCur += width;
		pRes += width;
	}
	for (i = 0; i < 2048; i++)
	{
		if (xCount[i] > max)
		{
			a = i;
			max = xCount[i];
		}
	}
	return a;

}

int CsrGetCircleCenterYpos(BYTE*pGryImg, int width, int height, BYTE *pResImg)
{
	int x, y, i, max = 0, b = 0, flagLR;
	int yUp, yDown, yCount[2048] = { 0 };
	BYTE *pCur, *pRes, *pEndGry, *pEndRes;
	pCur = pGryImg;
	pRes = pResImg;
	pEndGry = pGryImg + width * (height - 1);//尾行首元素
	pEndRes = pResImg + width * (height - 1);


	//按列
	for (x = 0; x < width; x++)
	{
		//从上往下
		pCur = pGryImg + x;
		pRes = pResImg + x;
		for (y = 0; y < height; y++, pCur += width, pRes += width)
		{
			if (*pCur < 1)
			{
				*pRes = 0;
				yUp = y;
				flagLR++;
				break;
			}
		}
		//从下往上
		pCur = pEndGry + x;
		pRes = pEndRes + x;
		for (y = height - 1; y >= 0; y--, pCur -= width, pRes -= width)
		{
			if (*pCur < 1)
			{
				*pRes = 0;
				yDown = y;
				flagLR++;
				break;
			}
		}
		if (flagLR>1)
		yCount[(yUp + yDown) >> 1]++;
	}
	for (i = 0; i < 2048; i++)
	{
		if (yCount[i] > max)
		{
			b = i;
			max = yCount[i];
		}
	}
	return b;
}




void CsrGetCircleCenterPos(BYTE*pGryImg, int width, int height, int *a, int *b, BYTE *pResImg)
{
	int x, y, i, max = 0,flagLR = 0;
	int yUp, yDown, yCount[2048] = { 0 };//上下边界点
	int xLeft, xRight, xCount[2048] = { 0 };//左右边界点
	BYTE *pCur, *pRes,*pEndGry,*pEndRes;
	pCur = pGryImg; pRes = pResImg;
	pEndGry = pGryImg + width * (height - 1);//尾行首元素
	pEndRes = pResImg + width * (height - 1);
	
	//----------------step1.按行扫描------------//
	for (y = 0; y < height; y++)
	{
		//从左往右
		for (x = 0; x < width; x++)
		{
			if (*(pCur + x) < 1)
			{
				*(pRes + x) = 0;
				xLeft = x;
				flagLR++;
				break;
			}
		}
		//从右往左
		for (x = width - 1; x >= 0; x--)
		{
			if (*(pCur + x) < 1)
			{
				*(pRes + x) = 0;
				xRight = x;
				flagLR++;
				break;
			}
		}
		if (flagLR > 1)//左右都找到两个点，才计算中点
		{
			xCount[(xLeft + xRight) >> 1]++;
		}
		//换行
		pCur += width;
		pRes += width;
		flagLR = 0;
	}
	//----------------step2.找xPos--------------//
	for (i = 0; i < 2048; i++)
	{
		if (xCount[i] > max)
		{
			*a = i;
			max = xCount[i];
		}
	}
	RmwDrawData2Img(xCount, 512, 2, pResImg, width, height);
	Rmw_Write8BitImg2BmpFile(pResImg, width, height, "..\\0604bCount.bmp");
	//----------------step3.按列扫描------------//
	for (x = 0; x < width; x++)
	{
		//从上往下
		pCur = pGryImg + x;
		pRes = pResImg + x;
		flagLR = 0;
		for (y = 0; y < height; y++, pCur += width, pRes += width)
		{
			if (*pCur < 1)
			{
				*pRes = 0;
				yUp = y;
				flagLR++;
				break;
			}
		}
		//从下往上
		pCur = pEndGry + x;
		pRes = pEndRes + x;
		for (y = height - 1; y >= 0; y--, pCur -= width, pRes -= width)
		{
			if (*pCur < 1)
			{
				*pRes = 0;
				yDown = y;
				flagLR++;
				break;
			}
		}
		if (flagLR > 1)
		{
			yCount[(yUp + yDown) >> 1]++;
		}
		
	}
	//----------------step4.找yPos--------------//
	for (i = 0; i < 2048; i++)
	{
		if (yCount[i] > max)
		{
			*b = i;
			max = yCount[i];
		}
	}
	RmwDrawData2Img(yCount, 512, 2, pResImg, width, height);
	Rmw_Write8BitImg2BmpFile(pResImg, width, height, "..\\0604bCount.bmp");
}


//---------------第六章第三题----------------------------------//







void CsrTraceAndFilling()
{
	unsigned char *pResImg;
	unsigned char *pBinImg;
	unsigned char *pCodeImg;
	int width, height;
	// step.1-------------读图像文件------------------------//
	pBinImg = Rmw_Read8BitBmpFile2Img("..\\0605Bin.bmp", &width, &height);
	if (!pBinImg)
	{
		printf("*file open err!\n");
		getchar();
		return;
	}
	pResImg = new BYTE[width*height];
	pCodeImg = new BYTE[width*height];
	// step.2-------------处理图像--------------------------//
	BYTE *pRow, *pCur;
	int x, y, x1, x2, y1, y2;
	int nCode, w, h;
	int Area, Perimeter;
	int i;
	for (i = 0; i < height*width; i++) pBinImg[i] = (pBinImg[i] >= 255)*250;//预处理，边缘实现伪彩色
	RmwSetImageBoundary(pBinImg, width, height, 0); //为了防止跟踪超界,必须边框清0
	memcpy(pResImg, pBinImg, sizeof(BYTE)*height*width);
	Rmw_Write8BitImg2BmpFileMark(pResImg, width, height, "..\\res_0605_去除残破大米.bmp");

	for (y = 1, pRow = pBinImg + y * width; y < height - 1; y++, pRow += width)
	{
		for (x = 1, pCur = pRow + x; x < width - 1; x++, pCur++)
		{
			if ((*pCur == 250) &&
				(*(pCur - 1) < 1)
				)
			{
				nCode = RmwTraceContour(pBinImg, width, x, y, pCodeImg, width*height, true);
				RmwRealAreaAndPerimeter(pCodeImg, nCode, &Area, &Perimeter);
				//满足条件者填充
				if ((Area < 500) && (Area > 100))
				{
					RmwContourRect(x, y, pCodeImg, nCode, &x1, &x2, &y1, &y2);
					w = x2 - x1 + 1;
					h = y2 - y1 + 1;
					//外接矩形贴边，就不填充
					if (x1 > 1 && x2 < width - 2 && y1 > 1 && y2 < height - 2)
					{
						printf("S=%d,w=%d,h=%d\n", Area, w, h);
						RmwFillingContour(pResImg, width, x, y, pCodeImg, nCode, true, 251, 255);
					}
				}
				//面积太小或者太大，只画轮廓线
				//else RmwMarkChainByColor(pResImg, width, x, y, pCodeImg, nCode, 254);
			}
			else if ((*pCur == 0) &&
				(*(pCur - 1) > 249)
				)
			{
				nCode = RmwTraceContour(pBinImg, width, x - 1, y, pCodeImg, width*height, false);
			}
		}//end of x
	}//end of y  
	 // step.3-------------保存图像--------------------------//
	Rmw_Write8BitImg2BmpFileMark(pResImg, width, height, "..\\res_0605_traceAndFilling2.bmp");
	// step.4-------------结束------------------------------//
	delete pResImg; //释放自己申请的内存
	delete pBinImg; //释放自己申请的内存
	delete pCodeImg; //释放自己申请的内存
	printf("Press any key to exit!\n");
	getchar();
	return;
}
//---------------第六章第四题----------------------------------//

void CsrGetImageRowBlock(BYTE *pGryImg, int width, int height,int blockH,BYTE*pResImg)
{
	int x, y;
	BYTE *pCur, *pEnd,*pRes;
	
}

void CsrVerticalProjectdebug(BYTE *pGryImg, int width, int height,int blockH, int nFast, int *histogram, int nSize)
{
	int x, y, i;
	BYTE *pCur,*pEnd;
	int c = (1 << 11) / blockH;
	pCur = pGryImg;
	nFast = max(1, nFast);
	//---------step1.初始化-------------//
	memset(histogram, 0, sizeof(int)*nSize);
	//---------step2.投影---------------//
	
	return;
}
//Error
void CsrVerticalProject(int *pSumImg, int width, int blockH,int offsetY,int nFast,int *pProjectOrg,int nSize)
{
	int x, y, i;
	int *pSum;
	int c = (1 << 11) / blockH;
	pSum = pSumImg + offsetY * width;
	nFast = max(1, nFast);
	//---------step1.初始化-------------//
	memset(pProjectOrg, 0, sizeof(int)*nSize);
	//---------step2.投影---------------//
	for (i = 0; i < width; i += nFast)
	{
		if(!i)//归一化，灰度值不会超过255
			pProjectOrg[i] = ((pSum[i]) * c) >> 11;
		else
		{
			pProjectOrg[i] = ((pSum[i] - pSum[i-1]) * c) >> 11;
		}
	}
	return;
}
void CsrVerticalProject(BYTE*pGryImg, int width, int blockH, int nFast, int id ,int *pProjectOrg)
{
	int x, y,c;
	BYTE *pCur;
	c = (1 << 11) / blockH;
	memset(pProjectOrg, 0, sizeof(int)*width);
	pCur = pGryImg;
	for (y = 0; y < blockH; y++, pCur += width)//按行累加，cache
	{
		for (x = 0; x < width; x++)
		{
			pProjectOrg[x] += *(pCur+x);
			
		}
	}
	for (x = 0; x < width; x++)//放缩到255以内
	{
		pProjectOrg[x] = (pProjectOrg[x]*c)>>11;
	}
}

void Csr1DAvgFilter(int * pData, int width, int M, int *pResData)
{
	int j, sum, x, c;
	int halfx, wSize;
	M = M / 2 * 2 + 1;
	halfx = M / 2;
	c = (1 << 12) / M;
	sum = pData[0];
	wSize = 1;
	//[0,halfx):对称即可
	for (x = 0, j = 1; x < halfx; x++, j += 2)
	{
		pResData[x] = sum / wSize;
		sum += pData[j] + pData[j + 1];
		wSize += 2;

	}
	for (x = halfx; x < width - halfx-1; x++)
	{
		pResData[x] = (sum*c >> 12);
		sum -= pData[x - halfx];
		sum += pData[x + halfx + 1];
	}
	//[width-halfx-1,width)
	wSize = (2 * halfx + 1);
	for (x = width - halfx - 1, j = x - halfx; x < width - 1 ; x++, j += 2)
	{
		pResData[x] = sum / wSize;
		sum -= pData[j] + pData[j + 1];
		wSize -= 2;

	}
	pResData[width-1] = sum / wSize;
	return;
}


#define MAX_RL_NUM 32
void CsrSegment(BYTE*pGryImg,int width,int blockH,int *histogram,int*pProjectOrg,int*pProjectAvg,int id,int *xPos,int * yPos,int &count,BYTE*pResImg)
{
	BYTE*pCur;
	bool isBegin = true;
	int x, y, i, T, dif;
	int LineWidth = 100, nRL = 0, leftX[MAX_RL_NUM], rightX[MAX_RL_NUM];
	//---------------------step1.垂直投影-------------------------//
	CsrVerticalProject(pGryImg, width,blockH, 1,id, pProjectOrg);
	//---------------------step2.均值滤波-------------------------//
	Csr1DAvgFilter(pProjectOrg, width, LineWidth, pProjectAvg);
	RmwDrawData2Img(pProjectAvg, width, 1, pResImg, width, blockH);
	//---------------------step3.差值直方图，消除光照不均---------//
	memset(histogram, 0, sizeof(int) * 256);
	for (i = 0; i < width; i++)
	{
		dif = abs(pProjectOrg[i] - pProjectAvg[i]);
		histogram[dif]++;
	}
	//---------------------step4.求阈值--------------------------//
	T = CsrGetOtsuThreshold(histogram, 256);
	T = max(T, 4);

	//---------------------step5.分割--找RL----------------------//
	for (x = 0; x < width; x++)
	{
		dif = pProjectOrg[x] - pProjectAvg[x];
		if (dif < T)
		{
			if (!isBegin)
			{
				rightX[nRL++] = x-1;
				isBegin = true;
			}
		}
		else
		{
			if (isBegin)
			{
				if (nRL < MAX_RL_NUM)
				{
					leftX[nRL] = x;
					isBegin = false;
				}
			}
		}
	}
	//---------------------step6.绘制RL--------------------------//
	//if (nRL < 1 ||nRL > 2 )  return;//段数约束
	pCur = pGryImg + (blockH >>1 )*width;
	for (i = 0; i < nRL; i++)
	{
		if ((rightX[i] - leftX[i]) >= 10 && (rightX[i] - leftX[i]) <= 40)//长度约束
		{
			memset(pCur + leftX[i], 255, sizeof(BYTE)*(rightX[i] - leftX[i]));
			xPos[count] = (rightX[i] + leftX[i])>>1;//记录中点列坐标
			yPos[count++] = id + (blockH / 2);//记录行坐标
		}
	}
}

void CsrGetMedianPoint(int *x, int *y, int nRL, int *leftX, int *rightX, int id)
{
	int i;
	for (i = 0; i < nRL; i++)
	{
		x[i] = ((leftX[i] + rightX[i]) >> 1);
		y[i] = id;
	}
}











//无用
//---处理与边界邻接的大米---//因为这样填充肯定比先轮廓跟踪，再用老师的填充算法来的慢。而这样做，又比邻接矩形来的慢！
void CsrDealNearBoundary(BYTE *pBinImg, BYTE * pGry, int width, int height, int color)
{
	int  i;
	int offsetp[8];
	static int offsetx[8] = { 1, 1, 0,-1,-1,-1, 0, 1 };//dx
	static int offsety[8] = { 0,-1,-1,-1, 0, 1, 1, 1 };//dy
	static stack <BYTE*>pStack;//存放上一个像素点的位置
	BYTE *pCur, *pEnd, *pTmp;
	//---------------step1.初始化--------------------------------//
	pCur = pGry;
	pEnd = pBinImg + height * width;
	for (i = 0; i < 8; i++) offsetp[i] = offsety[i] * width + offsetx[i];//确定往哪个方向走，往上一行，下一行还是左右
																		 //---------------step2.找到连通区域并置零-------------------//
	*pCur = color;
	pStack.push(pCur);
	while (!pStack.empty())
	{
		for (i = 0; i < 8; i++)
		{
			pTmp = pCur + offsetp[i];
			if (pTmp <= pBinImg || pTmp >= pEnd - 1) continue;//越界检查
			if (*pTmp > color)
			{
				pCur = pTmp;//移动到需要处理的邻居像素
				*pCur = color;
				pStack.push(pCur);//保存当前像素的位置
				i = 0;//从当前点，重新检查八邻域
			}
		}
		pCur = pStack.top();//回到前一个像素，检查其他邻居
		pStack.pop();
	}
	return;
}

void CsrDeleteNearBoundary(BYTE *pBinImg, int width, int height, BYTE color)
{
	int x, y;
	BYTE *pCur, *pEnd;
	pCur = pBinImg; pEnd = pBinImg + height * width;
	//---------------step1.首行-----------------------//
	for (x = 0; x < width; x++)
	{
		if (*pCur > 0)
		{
			CsrDealNearBoundary(pBinImg, pCur, width, height, 0);
		}
		pCur++;
	}
	//---------------step2.尾行----------------------//
	pCur = pBinImg + (height - 1)*width;
	for (x = 0; x < width; x++)
	{
		if (*pCur > 0)
		{
			CsrDealNearBoundary(pBinImg, pCur, width, height, 0);
		}
		pCur++;
	}
	//---------------step3.首列----------------------//
	for (y = 0, pCur = pBinImg; y<height; y++, pCur += width)
	{
		if (*pCur > 0)
		{
			CsrDealNearBoundary(pBinImg, pCur, width, height, 0);
		}
	}
	//---------------step4.尾列----------------------//
	for (y = 0, pCur = pBinImg + width - 1; y < height; y++, pCur += width)
	{
		if (*pCur > 0)
		{
			CsrDealNearBoundary(pBinImg, pCur, width, height, 0);
		}
	}
	return;
}