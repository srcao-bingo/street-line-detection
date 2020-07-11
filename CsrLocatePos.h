// CsrLocatePos.h : header file
//
#ifndef CSR_GET_POS_H
#define CSR_GET_POS_H
#include"bmpFile.h"
#include<opencv2/opencv.hpp>
using namespace cv;
class CsrLocatePos
{
public:
	//构造/析构
	CsrLocatePos();
	~CsrLocatePos();
    //初始化
    bool Initialize(bool isRGB,int width,int height,int nFast0123);
	//执行
	bool DoNext(BYTE *pImgData);
    //结果
    BYTE *GetResImg();
	int GetResWidth();
    int GetResHeight();
	void CsrSobel(BYTE*pGryImg, int width, int height, BYTE *pResImg);
	void CsrGetSumImg(BYTE *pGryImg, int width, int height, int *pResImg);
	void CsrReduceImgSize(BYTE*pGryImg, int width, int height, int nFast_0123, BYTE *pResImg);
	//上左右
	void CsrSearchMaxRectA(int* pSumImg, int width, int height, int w, int h, int delLR, int*xL, int*xR, int*yL, int*yR);
	//上左
	void CsrSearchMaxRectLeftUp(int* pSumImg, int width, int height, int w, int h, int delLR, int*xL, int*xR, int*yL, int*yR);
	void CsrDrawRect(BYTE*pGryImg, int width, int height, int& xL, int& xR, int& yL, int& yR, BYTE*pResImg);
	bool CsrSearchRectTarget(BYTE*pGryImg, int width, int height, int targetW, int targetH, int delLR, BYTE *pResImg);
private:
	// 内存释放
	void Dump();
	// 调试
	void Debug();
	void Debug(BYTE*pGryImg, int width, int height, const char * name);
private:
	//初始化成功
	bool m_isInitOK;
	//图像属性
	bool m_isRGB;
	int m_width;
	int m_height;
	int nFast;
	int m_nShrink;
	int shrinkWidth;
	int shrinkHeight;
	//内存
	BYTE *m_pRGBImg;
	BYTE *m_pGryImg;
	BYTE *m_pGrdImg;
	BYTE *m_pTmpImg;
	BYTE *m_pResImg;
	int * m_pSumImg;
	int m_memSize;
	//内部计数器
	int m_nFrameID;
};
#endif
