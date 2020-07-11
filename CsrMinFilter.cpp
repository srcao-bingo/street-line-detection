#include"CsrALG_C3.h"
double CsrMinFilter(BYTE*pGryImg, int width, int height, int N, int M, BYTE *pResImg)
{
	BYTE *pCur, *pRes;
	int i, j, g, x, y, y1, y2;
	int histogram[256];
	int wSize, left, right;
	int num = 0, min, max, v, halfx, halfy;
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
				histogram[(*pCur + j)]++;

		//step2,初始化最小值
		for (g = 0; g < 256; g++)
		{
			if (histogram[g])
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
				while (histogram[min] < 1)
					min++;
				g = *(pCur + right);
				histogram[g]++;
				if (g < min)
					min = g;
			}
		}

		pRes += halfx;

	}
	return dbgCmpTimes * 1.0 / ((width - halfx * 2)*(height - halfy * 2));

}
