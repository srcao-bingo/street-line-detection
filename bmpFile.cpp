///////////////////////////////////////////////////////////////////////////////////
//
//
//  Date: 2013.10.07
//
//
///////////////////////////////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

///////////////////////////////////////////////////////////////////////////////////
//
//
//  8 Bits
//
//
///////////////////////////////////////////////////////////////////////////////////
BYTE *Rmw_Read8BitBmpFile2Img(const char * filename,int *width,int *height)
{   //BmpHeader记录的真实的宽度,
	//读出的是真实的宽度,去掉了4对齐的扩展.
	FILE *fp;
	BITMAPFILEHEADER FileHeader;
	BITMAPINFOHEADER BmpHeader;
	_declspec(align(16)) BYTE *pImg;//保证分配的时候，首地址一定是16的倍数
	unsigned int size;
	int Suc=1,w,h;
	int k,extend,tmp;

	// Open File
	*width=*height=0;
	if((fp=fopen(filename,"rb"))==NULL) return NULL;
	// Read Struct Info
	if (fread((void *)&FileHeader,1,sizeof(FileHeader),fp)!=sizeof(FileHeader)) Suc=-1;
	if (fread((void *)&BmpHeader,1,sizeof(BmpHeader),fp)!=sizeof(BmpHeader)) Suc=-1;
    if ( (Suc==-1) || 
		 (FileHeader.bfOffBits<sizeof(FileHeader)+sizeof(BmpHeader) )
	   ) 
	{ 
		fclose(fp); 
		return NULL; 
	}
	// Read Image Data
	*width=w=BmpHeader.biWidth;
	//*width=w=(BmpHeader.biWidth+3)/4*4;
	*height=h=BmpHeader.biHeight;
    extend=(w+3)/4*4-w;
	size=BmpHeader.biWidth*BmpHeader.biHeight;
	fseek(fp,FileHeader.bfOffBits,SEEK_SET);
	if ( (pImg=new BYTE[size])!=NULL)
	{
		for(int i=0;i<h;i++)  // 0,1,2,3,4(5): 400-499
		{
			if ((int)fread(pImg+(h-1-i)*w,sizeof(BYTE),w,fp)!=w)
			{ 
				fclose(fp);
				delete pImg;
				pImg=NULL;
				return NULL;
			}
			//读掉扩充的数据
			for(k=0;k<extend;k++) 
			{ 
				if (fread(&tmp,1,1,fp)!=1) 
				{
					fclose(fp);
					delete pImg;
					pImg=NULL;
					return NULL;
					
				}
			}
		}
	}
	fclose(fp);
	return pImg;
}

bool Rmw_Write8BitImg2BmpFile(BYTE *pImg,int width,int height,const char * filename)
{   //BmpHeader记录的真实的宽度
	//当宽度不是4的倍数时自动添加成4的倍数,进行了4对齐的扩展.
	FILE * fp;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER BmpHeader;
    int i,extend;
	bool Suc=true;
	BYTE p[4],*pCur;

    // Open File
    if((fp=fopen(filename,"w+b"))==NULL) {  return false; }
	// Fill the FileHeader
	FileHeader.bfType= ((WORD) ('M' << 8) | 'B');
	FileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+256*4L;
    FileHeader.bfSize=FileHeader.bfOffBits+width*height ;
    FileHeader.bfReserved1=0;
    FileHeader.bfReserved2=0;
	if (fwrite((void *)&FileHeader,1,sizeof(FileHeader),fp)!=sizeof(FileHeader)) Suc=false;
	// Fill the ImgHeader
	BmpHeader.biSize = 40;
    BmpHeader.biWidth = width;
	BmpHeader.biHeight = height;
	BmpHeader.biPlanes = 1 ;
	BmpHeader.biBitCount = 8 ;
	BmpHeader.biCompression = 0 ;
	BmpHeader.biSizeImage = 0 ;
	BmpHeader.biXPelsPerMeter = 0;
	BmpHeader.biYPelsPerMeter = 0;
	BmpHeader.biClrUsed = 0;
	BmpHeader.biClrImportant = 0;
	if (fwrite((void *)&BmpHeader,1,sizeof(BmpHeader),fp)!=sizeof(BmpHeader)) Suc=false;
    // write Pallete
    for (i=0,p[3]=0;i<256;i++) 
    {  
	   p[3]=0;
	   p[0]=p[1]=p[2]=i; // blue,green,red;
       if (fwrite((void *)p,1,4,fp)!=4) { Suc=false; break; }
	}
    // write image data
	extend=(width+3)/4*4-width;
	if (extend==0)
	{   
		for(pCur=pImg+(height-1)*width;pCur>=pImg;pCur-=width)
		{   
		    if (fwrite((void *)pCur,1,width,fp)!=(unsigned int)width) Suc=false; // 真实的数据
		}
	}
	else
	{   
		for(pCur=pImg+(height-1)*width;pCur>=pImg;pCur-=width)
		{   
		    if (fwrite((void *)pCur,1,width,fp)!=(unsigned int)width) Suc=false; // 真实的数据
			for(i=0;i<extend;i++) // 扩充的数据
		       if (fwrite((void *)(pCur+width-1),1,1,fp)!=1) Suc=false;
		}
	}
	// return;
	fclose(fp);
	return Suc;
}

bool Rmw_Write8BitImg2BmpFileMark(BYTE *pImg,int width,int height,const char * filename)
{   //BmpHeader记录的真实的宽度
	//当宽度不是4的倍数时自动添加成4的倍数,进行了4对齐的扩展.
	FILE * fp;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER BmpHeader;
    int i,extend;
	bool Suc=true;
	BYTE p[4],*pCur;

    // Open File
    if((fp=fopen(filename,"w+b"))==NULL) {  return false; }
	// Fill the FileHeader
	FileHeader.bfType= ((WORD) ('M' << 8) | 'B');
	FileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+256*4L;
    FileHeader.bfSize=FileHeader.bfOffBits+width*height ;
    FileHeader.bfReserved1=0;
    FileHeader.bfReserved2=0;
	if (fwrite((void *)&FileHeader,1,sizeof(FileHeader),fp)!=sizeof(FileHeader)) Suc=false;
	// Fill the ImgHeader
	BmpHeader.biSize = 40;
    BmpHeader.biWidth = width;
	BmpHeader.biHeight = height;
	BmpHeader.biPlanes = 1 ;
	BmpHeader.biBitCount = 8 ;
	BmpHeader.biCompression = 0 ;
	BmpHeader.biSizeImage = 0 ;
	BmpHeader.biXPelsPerMeter = 0;
	BmpHeader.biYPelsPerMeter = 0;
	BmpHeader.biClrUsed = 0;
	BmpHeader.biClrImportant = 0;
	if (fwrite((void *)&BmpHeader,1,sizeof(BmpHeader),fp)!=sizeof(BmpHeader)) Suc=false;
    // write Pallete
    for (i=0,p[3]=0;i<256;i++) 
    {  //B、G、R、A顺序,最后四个是yellow blue green red，其余是三通道相同的数值，表示灰色
		p[3]=0;
		if (i < 254)
		{
			p[0] = i;
			p[1] = i;
			p[2] = i;
		}
		else if (i == 254)
		{
			p[0] = 0;
			p[1] = 255;
			p[2] = 0;
		}
		else 
		{
			p[0] = 0;
			p[1] = 0;
			p[2] = 255;
		}
       if (fwrite((void *)p,1,4,fp)!=4) { Suc=false; break; }
	}
    // write image data
	extend=(width+3)/4*4-width;
	if (extend==0)
	{   
		for(pCur=pImg+(height-1)*width;pCur>=pImg;pCur-=width)
		{   
		    if (fwrite((void *)pCur,1,width,fp)!=(unsigned int)width) Suc=false; // 真实的数据
		}
	}
	else
	{   
		for(pCur=pImg+(height-1)*width;pCur>=pImg;pCur-=width)
		{   
		    if (fwrite((void *)pCur,1,width,fp)!=(unsigned int)width) Suc=false; // 真实的数据
			for(i=0;i<extend;i++) // 扩充的数据
		       if (fwrite((void *)(pCur+width-1),1,1,fp)!=1) Suc=false;
		}
	}
	// return;
	fclose(fp);
	return Suc;
}
///////////////////////////////////////////////////////////////////////////////////
//
//
//  24 Bits
//
//
///////////////////////////////////////////////////////////////////////////////////
BYTE *Rmw_Read24BitBmpFile2Img(const char * filename,int *width,int *height)
{   //BmpHeader记录的真实的宽度,
	//读出的是真实的宽度,去掉了4对齐的扩展.
	FILE *fp;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER BmpHeader;
	BYTE *pImg;
	unsigned int size;
    int Suc=1,w,h,k,extend,tmp;

	// Open File
	*width=*height=0;
	if((fp=fopen(filename,"rb"))==NULL) return NULL;
	// Read Struct Info
    if (fread((void *)&FileHeader,1,sizeof(FileHeader),fp)!=sizeof(FileHeader)) Suc=-1;
	if (fread((void *)&BmpHeader,1,sizeof(BmpHeader),fp)!=sizeof(BmpHeader)) Suc=-1;
	if ( (Suc==-1) || 
		 (FileHeader.bfOffBits<sizeof(FileHeader)+sizeof(BmpHeader) )
	   ) 
	{ 
		fclose(fp); 
		return NULL; 
	}
	// Read Image Data
	*width=w=BmpHeader.biWidth;// biwidth 是不是四的倍数？很复杂，自己琢磨！！！ 因为按四字节对齐
	*height=h=BmpHeader.biHeight;
	extend=((w*3+3)/4*4)-w*3;
	size=(*width)*(*height)*3;
	fseek(fp,FileHeader.bfOffBits,SEEK_SET);//bfOffByte!!!!
    if ( (pImg=new BYTE[size])!=NULL)
    {   
		for(int i=0;i<h;i++)//
		{
			if((int)fread(pImg+(h-1-i)*w*3,sizeof(BYTE),w*3,fp)!=w*3)
			{ 
				fclose(fp);
				delete pImg;
				pImg=NULL;
				return NULL;
			}
			for(k=0;k<extend;k++) // 扩充的数据
			{ 
			  if (fwrite((void *)&tmp,sizeof(BYTE),1,fp)!=1) Suc=false;
			}
		}
    }
    fclose(fp);
    return pImg;
}

bool Rmw_Write24BitImg2BmpFile(BYTE *pImg,int width,int height,const char * filename)
{   //BmpHeader记录的真实的宽度
	//当宽度不是4的倍数时自动添加成4的倍数,进行了4对齐的扩展.
	FILE *fp;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER BmpHeader;
    bool Suc=true;
    int i,extend;
	BYTE *pCur;

    // Open File
    if((fp=fopen(filename,"w+b"))==NULL) {  return false; }
	// Fill the FileHeader
	FileHeader.bfType= ((WORD) ('M' << 8) | 'B');
	FileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
    FileHeader.bfSize=FileHeader.bfOffBits+width*height*3L ;
    FileHeader.bfReserved1=0;
    FileHeader.bfReserved2=0;
	if (fwrite((void *)&FileHeader,1,sizeof(BITMAPFILEHEADER),fp)!=sizeof(BITMAPFILEHEADER)) Suc=false;
	// Fill the ImgHeader
	BmpHeader.biSize = 40;
    BmpHeader.biWidth = width;
	BmpHeader.biHeight = height;
	BmpHeader.biPlanes = 1 ;
	BmpHeader.biBitCount = 24 ;
	BmpHeader.biCompression = 0 ;
	BmpHeader.biSizeImage = 0 ;
	BmpHeader.biXPelsPerMeter = 0;
	BmpHeader.biYPelsPerMeter = 0;
	BmpHeader.biClrUsed = 0;
	BmpHeader.biClrImportant = 0;
	if (fwrite((void *)&BmpHeader,1,sizeof(BITMAPINFOHEADER),fp)!=sizeof(BITMAPINFOHEADER)) Suc=false;
	// write image data
	extend=((width*3+3)/4*4)-width*3;
	if (extend==0)
	{   
		for(pCur=pImg+(height-1)*3*width;pCur>=pImg;pCur-=3*width)
		{   
		   if (fwrite((void *)pCur,1,width*3,fp)!=(unsigned int)(3*width)) Suc=false; // 真实的数据
		}
	}
	else
	{   
		for(pCur=pImg+(height-1)*3*width;pCur>=pImg;pCur-=3*width)
		{   
		    if (fwrite((void *)pCur,1,width*3,fp)!=(unsigned int)(3*width)) Suc=false; // 真实的数据
			for(i=0;i<extend;i++) // 扩充的数据
			{ 
			  if (fwrite((void *)(pCur+3*(width-1)+0),1,1,fp)!=1) Suc=false;
			}
		}
	}
	// return;
	fclose(fp);
	return Suc;
}
 


///////////////////////////////////////////////////////////////////////////////////
//
//
//画直方图
//
//
///////////////////////////////////////////////////////////////////////////////////
void RmwDrawHistogram2Img(int *histogram, int histSize,
	double maxScale,
	BYTE *pGryImg, int width, int height
)
{   //直方图数据画为黑色
	int x1, x2, y1;
	int maxV, i;
	int x, y;

	// step.1-------------初始化----------------------------//
	memset(pGryImg, 225, width*height);
	// step.2-------------寻找最大值------------------------//
	maxV = 0;
	for (i = 0; i<histSize; i++)
	{
		maxV = max(maxV, histogram[i]);
	}
	maxV = (int)(maxV*maxScale);
	// step.3-------------像素填充--------------------------//
	for (i = 0; i<histSize; i++)
	{
		y1 = (height - 1) - histogram[i] * (height - 1) / (maxV + 1);
		y1 = max(0, y1);
		x1 = i * width / histSize;
		x2 = (i + 1)*width / histSize;
		for (x = x1; x<x2; x++)
		{
			for (y = height - 1; y>y1; y--) *(pGryImg + y * width + x) = 0;
		}
	}
	// step.4-------------结束------------------------------//
	return;
}
