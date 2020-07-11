///////////////////////////////////////////////////////////////////////////////////
//
//
//画直方图
//
//
///////////////////////////////////////////////////////////////////////////////////
void RmwDrawHistogram2Img( int *histogram, int histSize, 
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
		y1 = (height-1)-histogram[i]*(height-1)/(maxV+1);
		y1 = max(0, y1);
		x1 = i*width/histSize;
		x2 = (i+1)*width/histSize;
		for (x = x1; x<x2; x++)
		{
			for (y = height-1; y>y1; y--) *(pGryImg+y*width+x) = 0;
		}
	}
	// step.4-------------结束------------------------------//
	return;
}
