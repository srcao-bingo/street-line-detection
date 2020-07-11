

#include"CsrALG_C5.h"

int CsrGetOtsuThreshold(int * histogram,int nSize)
{
	int threshold;
	int i, gmin, gmax;
	double maxVal, dist,N;
	int n1, n2, s1, s2;
	gmin = 0;
	gmax = nSize - 1;
	maxVal = 0;
	threshold = gmin;
	//-------------step1.找最小值---------------------//
	while (!histogram[gmin]) gmin++;
	//-------------step2.找最大值---------------------//
	while (!histogram[gmax]) gmax--;
	if (gmin == gmax) return -1; //全黑全白，返回
	//-------------step3.初始化类别---------------------//
	n1 = s1 = 0;
	n2 = s2 = 0;
	for (i = gmin; i <= gmax; i++)//所有像素都是目标,<=gmax
	{
		n2 += histogram[i];
		s2 += histogram[i] * i;
	}
	N = n2;
	//-------------step4.阈值搜索---------------------//
	for (i = gmin; i < gmax; i++)//因为找的时候是[gmin gmax)    而 gmin< threshold <= gmax,二值图为例
	{							 //类似[0 254] ,最后取值[1 255]。
		//加速，为零，均值不变！
		if (!histogram[i]) continue;
		//更新数量
		n1 += histogram[i];
		n2 -= histogram[i];
		//更新灰度总和
		s1 += histogram[i] * i;
		s2 -= histogram[i] * i;
		//计算距离
		dist = (s1 * 1.0 / n1) - (s2*1.0 / n2);
		dist = dist * dist*(n1*1.0 / N)*(n2*1.0 / N);
		if (dist > maxVal)
		{
			maxVal = dist;
			threshold = i;
		}

	}
	return threshold+1;
}





void CsrGetHistogram(BYTE*pGryImg, int width, int height, int channel,int *histogram,int nSize)
{
	BYTE *pCur, *pEnd;
	memset(histogram, 0, sizeof(int) * nSize);
	//-------------统计直方图---------------------//
	pCur = pGryImg, pEnd = pCur + width * height*channel;
	for (; pCur < pEnd;)
	{
		histogram[*(pCur++)]++;
	}
	return;
}

//最基本的减法
void CsrImgSub(BYTE*pGryImg, BYTE*pBackImg, int width, int height, BYTE*pResImg)
{
	BYTE *pCur, *pEnd, *pGry, *pRes;
	pGry = pGryImg;
	pCur = pBackImg; pEnd = pBackImg + width * height;
	pRes = pResImg;
	for (; pCur < pEnd; pCur++,pGry++)
	{
		*(pRes++) =  max(0,(*pGry - *pCur));
	}
	return;
}
void CsrImgSub(BYTE*pGryImg, BYTE*pBackImg,int width, int height,bool positive,BYTE*pResImg)
{
	BYTE *pCur, *pEnd,*pGry,*pRes;
	pGry = pGryImg;
	pCur = pBackImg; pEnd = pBackImg + width * height;
	pRes = pResImg;
	if (positive)
	{
		//白色目标:g(x,y)>u(x,y)
		for (; pCur < pEnd; pCur++,pGry++)
		{
			//*(pRes++) = (*pGry > *pCur) * (*pGry - *pCur);
			*(pRes++) = max(0, (*pGry - *pCur));
		}
	}
	else
	{
		//黑色目标:g(x,y)<u(x,y)
		for (; pCur < pEnd; pCur++,pGry++)
		{
			*(pRes++) = (*pGry < *pCur) * ( -(*pGry - *pCur) );
		}
	}
	return;
}

void CsrGetHistogramMaxMin(int *histogram, int nSize, int * minVal, int * maxVal)
{
	int i, gmin, gmax;
	gmin = 0; gmax = nSize - 1;
	//-------------step1.找最小值---------------------//
	while (!histogram[gmin]) gmin++;
	//-------------step2.找最大值---------------------//
	while (!histogram[gmax]) gmax--;

	*minVal = gmin;
	*maxVal = gmax;
	return;
}





void CsrImgKeans2Fast(int *histogram, int nSize, int gmin, int gmax, int *d1)
{
	int i, t1;
	int n1, n2, s1, s2;
	double u1, u2, v1, v2;
	t1 = *d1; 
	if (gmin == gmax)
	{
		printf("类别数为 1,不用进行K聚类!\n");
		return;
	}
	//-------------step1.初始化聚类中心---------------//
	v1 = u1 = (gmin + t1) / 2;
	v2 = u2 = (t1 + gmax) / 2;
	//-------------step2.聚类------------------------//
	while (1)
	{
		n1 = n2 = 0;
		s1 = s2 = 0;
		for (i = gmin; i < t1; i++)
		{
			s1 += histogram[i] * i;
			n1 += histogram[i];
		}
		
		for (i = t1; i <= gmax; i++)
		{
			s2 += histogram[i] * i;
			n2 += histogram[i];
		}
		//更新均值
		u1 = s1 / n1; u2 = s2 / n2; 
		if (fabs(u1 - v1) < 1e-6 && fabs(u2 - v2) < 1e-6)
		{
			*d1 = t1; 
			return;
		}
		//保存聚类中心
		v1 = u1; v2 = u2;
		//记录分割点
		t1 = (v1 + v2) / 2;
	}
}




//把直方图聚类转换成，有序区间分段的问题,输出*d1,*d2为分割点位置
void CsrImgKeans3Fast(int *histogram, int nSize, int gmin,int gmax,int *d1,int *d2)
{
	int i,t1,t2;
	int n1, n2, n3, s1, s2, s3;
	double u1, u2, u3, v1, v2, v3;
	if (*d1 > *d2)
	{
		i = *d1;
		*d1 = *d2;
		*d2 = i;
	}
	t1 = *d1; t2 = *d2;
	if (gmin == gmax)
	{
		printf("类别数为 1,不用进行K聚类!\n");
		return;
	}
	//-------------step1.初始化聚类中心---------------//
	v1 = u1 = (gmin + t1)/2;
	v2 = u2 = (t1 + t2)/2;
	v3 = u3 = (t2 + gmax)/2;
	//-------------step2.聚类------------------------//
	while (1)
	{
		n1 = n2 = n3 = 0;
		s1 = s2 = s3 = 0;
		for (i = gmin; i < t1; i++)
		{
			s1 += histogram[i] * i;
			n1 += histogram[i];
		}
		for (i = t1; i < t2; i++)
		{
			s2 += histogram[i] * i;
			n2 += histogram[i];
		}
		for (i = t2; i <= gmax; i++)
		{
			s3 += histogram[i] * i;
			n3 += histogram[i];
		}
		//更新均值
		u1 = s1 / n1; u2 = s2 / n2; u3 = s3 / n3;
		if (fabs(u1 - v1) < 1e-6 && fabs(u2 - v2) < 1e-6 && fabs(u3 - v3) < 1e-6)
		{
			*d1 = t1; *d2 = t2;
			return;
		}
		//保存聚类中心
		v1 = u1; v2 = u2; v3 = u3;
		//记录分割点
		t1 = (v1 + v2) / 2;
		t2 = (v2 + v3) / 2;
	}
}
//覆盖原始图像
void CsrKmeans3FastShow(BYTE*pGryImg,int width,int height,int d1,int d2)
{
	BYTE *pCur, *pEnd;
	pCur = pGryImg; pEnd = pGryImg + width * height;
	for (; pCur < pEnd;pCur++)
	{
		if ( *pCur < d1)
		{
			*pCur = 0;
		}
		else if (*pCur < d2 && *pCur >= d1)
		{
			*pCur = 128;
		}
		else
		{
			*pCur = 255;
		}
	}

	return;
}
void CsrKmeans2FastShow(BYTE*pGryImg, int width, int height, int d1)
{
	BYTE *pCur, *pEnd;
	pCur = pGryImg; pEnd = pGryImg + width * height;
	for (; pCur < pEnd; pCur++)
	{
		 *pCur = (*pCur >= d1) * 255;
	}

	return;
}
void CsrImgKmeans_Kind_And_Numbers(int *histogram,int nSize,int k,ClusterCenter *center)
{
	int threshold;
	int i,j, gmin, gmax;
	bool flag = true;//是否需要进行聚类
	int tmpDist, minDist,id,seed;
	gmin = 0;
	gmax = nSize - 1;
	int count = 0;
	//-------------step1.找最小值---------------------//
	while (!histogram[gmin]) gmin++;
	//-------------step2.找最大值---------------------//
	while (!histogram[gmax]) gmax--;
	if (gmin == gmax)
	{
		printf("类别数为 1,不用进行K聚类!\n");
		return;
	}
	//-------------step3.初始化聚类中心---------------//
	srand(1);
	for (i = 0; i < k; i++) 
	{
		center[i].mean = rand() % (gmax - gmin) + gmin;
	}
	//-------------step4.聚类------------------------//
	while (flag)
	{
		//标志位还原
		flag = false;
		for (i = gmin; i <= gmax; i++)
		{
			minDist = 0x3fffffff;
			//个数为零，不算入，图中没有这样的像素，直接记录每一类的最小值和最大值，在区间内的赋为均值
			if (!histogram[i]) continue;
			for (j = 0; j < k; j++)
			{
				//计算距离
				tmpDist = fabs(i - center[j].mean);
				//记录所属类别
				if (tmpDist < minDist)
				{
					id = j;
					minDist = tmpDist;
				}
			}
			//累计灰度值
			center[id].sum += i*histogram[i];
			center[id].count += histogram[i];
			center[id].grayAry[center[id].aryNum++] = i;
		}
		//更新均值
		for (j = 0; j < k; j++)
		{
			if (!center[j].count) { printf("类别数过多，第 %d 类,均值为：%d ,个数为0 !\n", j,center[j].mean); return; }
			//计算新的聚类中心
			center[j].mean = center[j].sum / center[j].count;
			//聚类中心变化，仍需迭代
			if (center[j].mean - center[j].oldMean > 1e-6)
			{
				flag = true;
			}
			
		}
		if(flag)
			for (j = 0; j < k; j++)
			{
				//还原
				center[j].oldMean = center[j].mean;
				center[j].count = 0;
				center[j].sum = 0;
				center[j].aryNum = 0;
			}
	}

	return;
}
void CsrImgKmeans_Kind(int *histogram, int nSize, int k, ClusterCenter *center)
{
	int threshold;
	int i, j, gmin, gmax;
	bool flag = true;//是否需要进行聚类
	int tmpDist, minDist, id, seed;
	gmin = 0;
	gmax = nSize - 1;
	int count = 0;
	//-------------step1.找最小值---------------------//
	while (!histogram[gmin]) gmin++;

	//-------------step2.找最大值---------------------//
	while (!histogram[gmax]) gmax--;
	if (gmin == gmax)
	{
		printf("类别数为 1,不用进行K聚类!\n");
		return;
	}
	//-------------step3.初始化聚类中心---------------//
	srand(1);
	for (i = 0; i < k; i++)
	{
		center[i].mean = rand() % (gmax - gmin) + gmin;
	}
	//-------------step4.聚类------------------------//
	while (flag)
	{
		//标志位还原
		flag = false;
		for (i = gmin; i <= gmax; i++)
		{
			minDist = 0x3fffffff;
			//个数为零，不算入，图中也没有这样的像素，直接记录每一类的最小值和最大值，在区间内的赋值为均值
			if (!histogram[i]) continue;
			for (j = 0; j < k; j++)
			{
				//计算距离
				tmpDist = (i - center[j].mean)*(i - center[j].mean);
				//记录所属类别
				if (tmpDist < minDist)
				{
					id = j;
					minDist = tmpDist;
				}
			}
			//累计灰度值,按种类进行聚类
			center[id].sum += i;
			center[id].grayAry[center[id].aryNum++] = i;
		}
		//更新均值
		for (j = 0; j < k; j++)
		{
			if (!center[j].aryNum) { printf("类别数过多，第 %d 类,均值为：%d ,个数为0 !\n", j, center[j].mean); return; }
			//计算新的聚类中心
			center[j].mean = center[j].sum / center[j].aryNum;
			//聚类中心变化，仍需迭代
			if (center[j].mean - center[j].oldMean > 1e-6)
			{
				flag = true;
			}

		}
		if (flag)
			for (j = 0; j < k; j++)
			{
				//还原
				center[j].oldMean = center[j].mean;
				center[j].sum = 0;
				center[j].aryNum = 0;
			}
	}

	return;
}
void CsrImgDivideBykMeans(BYTE*pGryImg, int width, int height, int k, ClusterCenter * center, BYTE*pResImg)
{
	int i;
	BYTE *pCur, *pEnd, *pRes;
	pRes = pResImg;
	pCur = pGryImg;
	pEnd = pGryImg + width * height;
	//对图像分类
	for (; pCur < pEnd;)
	{
		for (i = 0; i < k; i++)
		{
			//center[i].grayAry[0]<= 第i类的灰度值 <= center[i].grayAry[center[i].count - 1]
			if (center[i].grayAry[center[i].aryNum - 1] >= *pCur && *pCur >= center[i].grayAry[0])
			{
				*(pRes) = center[i].mean;
			}
		}
		pCur++; pRes++;
	}
	return;

}

void CsrImgDiv(BYTE*pGryImg, int width, int height, double div, BYTE*pResImg)
{
	BYTE *pCur, *pEnd, *pRes;
	pRes = pResImg;
	pCur = pGryImg; pEnd = pGryImg + width * height;
	int c;
	//除零检查
	if (div - 0.0 < 1e-6) return;
	c = (1 << 10) / div;
	for (; pCur < pEnd;*pCur++)
	{
		*(pRes++) = max(0,min(255,(*pCur * c) >> 10));
	}

}

void CsrHistogramAvgFilter(int * histogram,int nSize,int M)
{
	int halfx,sum,x,c;
	sum = 0;
	M = M / 2 * 2 + 1;
	halfx = M / 2;
	c = (1 << 23) / M;
	for (x = 0; x <= M; x++) sum += histogram[x];
	for (x = halfx; x < nSize - halfx; x++)
	{
		histogram[x] = (sum*c >> 23);
		sum -= histogram[x - halfx];
		sum += histogram[x + halfx + 1];
	}

}
void CsrHistogramAvgFilter(int * histogram, int nSize, int M,int *resHistogram)
{
	int halfx, sum, x, c;
	sum = 0;
	M = M / 2 * 2 + 1;
	halfx = M / 2;
	c = (1 << 23) / M;
	for (x = 0; x <= M; x++) sum += histogram[x];
	for (x = halfx; x < nSize - halfx - 1; x++)
	{
		resHistogram[x] = (sum*c >> 23);
		sum -= histogram[x - halfx];
		sum += histogram[x + halfx + 1];
	}

}

