#define _CRT_SECURE_NO_WARNINGS

#include"CsrALG_C2.h"

void CsrHistogramEqualize(BYTE*pGryImg,int width,int height,int channel)
{
	int g;
	BYTE *pCur, *pEnd ;
	int histogram[256];
	int LUT[256];
	memset(histogram, 0, sizeof(int) * 256);
	memset(LUT, 0, sizeof(int) * 256);
	
	// step.1-------------统计直方图---------------------//
	pCur = pGryImg, pEnd = pCur + width * height*channel;
	for (; pCur < pEnd;)
	{
		histogram[*(pCur++)]++;
	}
	// step.2-------------求名次-------------------------//
	LUT[0] = 255 * histogram[0] / (width*height*channel);//千万别漏LUT[0]
	for (g = 1; g < 256; g++)//<256
	{
		histogram[g] = histogram[g] + histogram[g - 1];
		LUT[g] = 255 * histogram[g] / (width*height*channel);
	}
	// step.3-------------查表均衡化--------------------//
	for (pCur = pGryImg; pCur < pEnd;)
	{
		*(pCur++) = LUT[*pCur];
	}
	
}
void CsrMeanStdDev(BYTE*pGryImg, int width, int height,double bright, double contrast)
{
	int g;
	BYTE *pCur, *pEnd;
	long long num = width * height, sum;
	double mean = 0.0, std = 0.0;
	pCur = pGryImg;
	pEnd = pCur + num;
	//求直方图
	int histogram[256];
	memset(histogram, 0, sizeof(int) * 256);
	for (pCur = pGryImg; pCur < pEnd;) histogram[*(pCur++)]++;
	//求亮度
	for (g = 0, sum = 0; g < 256;)
	{
		sum += histogram[g++] * g;
	}
	mean = sum * 1.0 / num;
	//求对比度
	for (g = 0, std = 0.0; g < 256; g++)
	{
		std += histogram[g] * (g - mean)*(g - mean);
	}
	std = sqrt(std / num - 1);//样本方差
	//打表
	double LUT[256] = { 0 };
	for (int x = 0; x < 256; x++)
	{
		LUT[x] = max(0, min(255, (x - mean) / std))*contrast + bright;
	}
	//转换
	for (pCur = pGryImg; pCur < pEnd;)
	{
		*(pCur++) = LUT[*pCur];
	}
	printf("\n mean: %f std: %f \n", mean, std);
}int BGR_LUT[3][256] = { 0 };
void getBGR_LUT()
{
	//生成三个通道各灰度值对应的查找表
	for (int x = 0; x < 256; x++)
	{
		BGR_LUT[0][x] = x * 0.114;
		BGR_LUT[1][x] = x * 0.587;
		BGR_LUT[2][x] = x * 0.299;
	}
}
void CsrRgb2GrayLUT(BYTE*pRgbImg, int width, int height, BYTE *pResImg)
{
	BYTE* pCur, *pEnd;
	memset(pResImg, 0, sizeof(BYTE)*width * height);
	// step.1----------生成查找表---------------------------//
	getBGR_LUT();
	// step.2----------RGB转Gray---------------------------//
	pEnd = pRgbImg + width * height * 3;
	for (pCur = pRgbImg; pCur < pEnd;)
	{
		*(pResImg++) = BGR_LUT[0][*(pCur)] + BGR_LUT[1][*(pCur + 1)] + BGR_LUT[2][*(pCur + 2)];
		pCur += 3;
	}
	
}
void CsrGrayExtend(BYTE*pGryImg, int width, int height)
{
	BYTE *pCur, *pEnd;
	int  g, minGray, maxGray;
	int histogram[256];
	int LUT[256];
	memset(histogram, 0, sizeof(int) * 256);
	memset(LUT, 0, sizeof(int) * 256);
	
	long long num = width * height;
	//求直方图
	pCur = pGryImg, pEnd = pCur + width * height;
	for (; pCur < pEnd;)
	{
		histogram[*(pCur++)]++;
	}
	//找最小值。
	for (g = 0; g < 256; g++)
	{
		if (histogram[g]) break;
	}
	minGray = g;
	//找最大值
	for (g = 255; g >= 0; g--)
	{
		if (histogram[g]) break;
	}
	maxGray = g;
	//求增益和偏置
	int k = 255 / (maxGray - minGray);
	int b = 255 - k * maxGray;
	//k < 0,表示反相
	//k = -3; b = 255;

	//打表
	for (g = 0; g < 256; g++)
	{
		LUT[g] = max(0,min(k * g + b,255));
	}
	//查表实现拉伸
	for (pCur = pGryImg; pCur < pEnd;)
	{
		*(pCur++) = LUT[*pCur];
	}


}
int RmwRead14BitImgFile(short int * p14Img, int width, int height, const char * fileName)
{
	FILE *fp;
	fp = fopen(fileName, "rb");
	if (!fp) return 0;
	fread(p14Img, sizeof(short int), width*height, fp);
	fclose(fp);
	return 1;
}
void Csr14BitChangeTo8Bit(short int*pGryImg, int width, int height, BYTE *pResImg)
{
	short int *pCur, *pEnd;
	
	int channel = 1;
	const int grayLevel = 1 << 14;
	int one_channel_num = width * height;
	int histogram14Bit[grayLevel];
	int LUT14Bit[grayLevel];
	memset(histogram14Bit, 0, sizeof(int) * grayLevel * channel);//-----------------漏乘通道数
	memset(LUT14Bit, 0, sizeof(int) * grayLevel * channel);
	
	//计算直方图
	pEnd = pGryImg + one_channel_num;
	for (pCur = pGryImg; pCur < pEnd; pCur++)
	{
		histogram14Bit[*(pCur++)]++;
	}
	//计算累计排名
	int A = 0, g = 0;
	for (g = 0; g < grayLevel; g++)
	{
		A += histogram14Bit[g];
		LUT14Bit[g] = 255 * A / one_channel_num;
	}
	//查表均衡化
	for (pCur = pGryImg; pCur < pEnd; pCur++)
	{
		*(pResImg++) = LUT14Bit[*pCur];
	}
	

}