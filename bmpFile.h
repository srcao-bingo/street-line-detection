#ifndef BMP_FILE_H
#define BMP_FILE_H
//读单通道八位图
BYTE *Rmw_Read8BitBmpFile2Img(const char * filename,int *width,int *height);
//写8位单通道图
bool Rmw_Write8BitImg2BmpFile(BYTE *pImg,int width,int height,const char * filename);
//带调色板
bool Rmw_Write8BitImg2BmpFileMark(BYTE *pImg,int width,int height,const char * filename);
//读24位三通道
BYTE *Rmw_Read24BitBmpFile2Img(const char * filename,int *width,int *height);
//写24位三通道图
bool Rmw_Write24BitImg2BmpFile(BYTE *pImg,int width,int height,const char * filename);
//绘制直方图
void RmwDrawHistogram2Img(int *histogram, int histSize,
	double maxScale,
	BYTE *pGryImg, int width, int height
);
#endif
