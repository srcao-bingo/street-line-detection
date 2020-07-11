#include <windows.h>
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
		pre = *(--pTmp);//先自减
		for (x = width - 2, pTmp--; x >= 0; x--)
		{
			*(pTmp--) = pre = pre + ALUT[*(pTmp)-pre];
		}
		pTmp += width + 1;
	}

	for (x = 0, pTmp = pTmpImg; x < width; x++, pTmp = pTmpImg + x)//
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
		//pTmp += width;
	}

#if 1
	//----------------step3.求出二阶导数--------------------//
	pEnd = pTmpImg + width * height;
	for (pCur = pTmpImg, pGry = pGryImg; pCur < pEnd;)
	{
		*(pCur++) -= *(pGry++);
	}
	//----------------step4.找出过零点-------------------------//
	memset(pResImg, 0, sizeof(BYTE)*width*height);
	pCur = pTmpImg + width; pRes = pResImg + width;
	for (y = 1; y < height - 1; y++)
	{
		pRes++; pCur++;
		for (x = 1; x < width - 1; x++, pCur++, pRes++)
		{
			if (*pCur > 0)
			{
				if ((*(pCur - width) <= 0) ||
					(*(pCur - 1) <= 0) ||
					(*(pCur + 1) <= 0) ||
					(*(pCur + width) <= 0)
					)
				{
					*pRes = 255;
				}
			}

		}
		pRes++; pCur++;

	}
#endif

#if 0	//----------------step3.找出过零点---------------------//
	memset(pResImg, 0, sizeof(BYTE)*width*height);
	pCur = pTmpImg + width; pRes = pResImg + width; pGry = pGryImg + width;
	for (y = 1; y < height - 1; y++)
	{
		pRes++; pGry++; pCur++;
		for (x = 1; x < width - 1; x++, pGry++, pCur++, pRes++)
		{
			if (*pCur - *pGry > 0)
			{
				if ((*(pCur - width) - *(pGry - width) <= 0) ||
					(*(pCur - 1) - *(pGry - 1) <= 0) ||
					(*(pCur + 1) - *(pGry + 1) <= 0) ||
					(*(pCur + width) - *(pGry + width) <= 0)
					)
				{
					*pRes = 255;
				}
			}

		}
		pRes++; pGry++; pCur++;
	}
#endif
	return;
}