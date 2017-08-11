#include"pro1.h"

VideoCapture capture(0);
int main()
{
	int flag = 0;
	init();							//初始化窗口
	backinit();						//初始化背景
	cout << IMG_SIZE_X << endl;		//显示图像长和宽
	cout << IMG_SIZE_Y << endl;
	while (1)
	{
		frame();					//处理每帧图像
		flag = wait_key();
		if (!flag) break;
	}
	return 0;

}


void init()//初始化窗口
{
	namedWindow("imageOutput", CV_WINDOW_NORMAL);
	namedWindow("imageORI", CV_WINDOW_NORMAL);
	namedWindow("BACK", CV_WINDOW_NORMAL);
}




void  backinit()//初始化背景
{
	capture >> pFrame;
	IMG_SIZE_X = pFrame.cols;
	IMG_SIZE_Y = pFrame.rows;
	CENTER_X = IMG_SIZE_X / 2;
	CENTER_Y = IMG_SIZE_Y / 2;
	tem = pFrame;
	tem.copyTo(backImg);
	backImg = reBuiltBackgroundImage(backImg);			//对背景进行灰度化、高斯滤波
	imshow("BACK", backImg);

}

void frame()//逐帧处理
{
	capture >> pFrame;
	image = pFrame;
	reImg = reBuiltImage(image, backImg);				//背景差分
														//对图像做膨胀
	int g_nStructElementSize = 3;
	Mat element = getStructuringElement(MORPH_RECT, Size(2 * g_nStructElementSize + 1, 2 * g_nStructElementSize + 1), Point(g_nStructElementSize, g_nStructElementSize));
	dilate(reImg, reImg, element);

	location = trackROI(image, reImg, lastLocation);		//计算目标的位置、标定目标
	lastLocation = location;
}

int  wait_key()
{
	if (waitKey(1) >= 0) return 0;
}



/**************************************************************/
//函数名称：adjustARR()
//函数功能：根据追踪到的目标大小，确定下一次遍历区域
//传入参数：无
//返回参数：无
//最后修改时间：2016年10月19日14：43                            
/**************************************************************/
void adjustARR()
{
	double x, y;
	int size = 5;
	x = sumOfPixel;
	y = sizeLocation * sizeLocation * 4;
	while ((int(y / x) != size) && (sumOfPixel != 0))
	{
		if (y / x > size)
		{
			sizeLocation--;
			y = sizeLocation*sizeLocation * 4;
			if (sizeLocation < 10)
				break;
		}
		else if (y / x < size)
		{
			{
				sizeLocation++;
				y = sizeLocation*sizeLocation * 4;
			}
		}

	}

}
/**************************************************************/
//函数名称：trackROI
//函数功能：计算目标的位置、将目标标定出来，并返回目标的坐标
//传入参数：image,imageOutput, lastLocation
//返回参数：Poi （计算出的目标几何中心）
//最后修改时间：2017年8月10日13：50                            
/**************************************************************/

Point trackROI(Mat image, Mat imageOutput, Point lastLocation)
{
	Point Poi;
	if (IS_LOCATED)
	{
		//如果追踪到目标，计算出真实的几何中心
		adjustARR();												//缩小遍历区域
		Poi = getLocationFromArea(imageOutput, lastLocation);		//在追踪区域中搜索目标的位置
		realPoi = Poi;
	}
	if (!IS_LOCATED)
	{
		Poi = getLocationFromImage(imageOutput);					//在整个图像中搜索目标的位置 
		realPoi = Poi;

	}

	drawrReferenceLine(image, Poi, sizeLocation);
	drawrLandARR(image, Point(IMG_SIZE_X / 2, IMG_SIZE_Y / 2), landFlag);
	drawFilledCircle(image, Point(IMG_SIZE_X / 2, IMG_SIZE_Y / 2));
	drawLine(image, Poi, Point(Poi.x + CH1 * 20, Poi.y));
	drawLine(image, Poi, Point(Poi.x, Poi.y + CH2 * 20));
	drawLine(image, Poi, Point(IMG_SIZE_X / 2, IMG_SIZE_Y / 2));

	/*
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(imageOutput, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	//Mat result(imageOutput.size(), CV_8U, Scalar(255));
	cv::drawContours(image, contours,
	-1, // draw all contours
	Scalar(0), // in black
	2); // with a thickness of 2
	*/

	imshow("imageOutput", imageOutput);
	imshow("imageORI", image);
	return(Poi);

}



/**************************************************************/
//函数名称：reBuiltImage
//函数功能：背景差并更新背景
//传入参数：image,backImage
//返回参数：imageOutput （处理后的图）
//最后修改时间：2016年10月19日14：43                            
/**************************************************************/
Mat reBuiltImage(Mat image, Mat  backImage)
{
	Mat gray_image = Mat::zeros(image.size(), CV_32F);
	Mat imageOutput = Mat::zeros(image.size(), image.type());
	gray_image.convertTo(backImg, CV_32F);
	gray_image.convertTo(backImage, image.type());
	cvtColor(image, gray_image, CV_BGR2GRAY);
	medianBlur(gray_image, gray_image, 3);//均值滤波
	absdiff(gray_image, backImage, imageOutput);
	threshold(imageOutput, imageOutput, nThreshold, 255, THRESH_BINARY_INV);
	accumulateWeighted(gray_image, backImg, learningRate);
	return imageOutput;
}

/**************************************************************/
//函数名称：reBuiltBackgroundImage
//函数功能：对传入的背景图进行灰度化处理，并进行高斯滤波
//传入参数：backImage（背景图）
//返回参数：backImage2 （处理后的背景图）
//最后修改时间：2017年8月10日11：38                            
/**************************************************************/
Mat reBuiltBackgroundImage(Mat backImage)
{
	Mat backImageGRY = Mat::zeros(backImage.size(), backImage.type());
	Mat backImage2 = Mat::zeros(backImage.size(), backImage.type());
	cvtColor(backImage, backImageGRY, CV_BGRA2GRAY);
	//threshold(gray_image, imageOutput, 100, 255, THRESH_BINARY);
	//threshold(backImageGRY, backImage2, 80, 255, THRESH_BINARY);
	//return backImage2;
	GaussianBlur(backImageGRY, backImage2, Size(21, 21), 0, 0);
	return backImage2;
}



/**************************************************************/
//函数名称：reviseLocation
//函数功能：对传入的坐标进行滤波处理，减少坐标点抖动
//传入参数：baseLocation（未经滤波的坐标）
//返回参数：Point（xx,yy） （滤波后的坐标值）
//最后修改时间：2016年10月24日11：24                            
/**************************************************************/
Point reviseLocation(Point baseLocation)
{

	if (lastPoint1.x == -1)
	{
		lastPoint1.x = baseLocation.x;
		lastPoint1.y = baseLocation.y;
		return  lastPoint1;
	}
	else if ((lastPoint1.x != -1) && (lastPoint2.x == -1))
	{
		lastPoint2.x = baseLocation.x;
		lastPoint2.y = baseLocation.y;
		int xx, yy;
		xx = (lastPoint1.x + lastPoint2.x) / 2;
		yy = (lastPoint1.y + lastPoint2.y) / 2;
		return (Point(xx, yy));
	}

	else if ((lastPoint1.x != -1) && (lastPoint2.x != -1) && (lastPoint3.x == -1))
	{
		lastPoint3.x = baseLocation.x;
		lastPoint3.y = baseLocation.y;
		int xx, yy;
		xx = (lastPoint1.x + lastPoint2.x + lastPoint3.x) / 3;
		yy = (lastPoint1.y + lastPoint2.y + lastPoint3.y) / 3;
		return (Point(xx, yy));
	}
	else
	{
		int xx, yy;
		if (((abs(baseLocation.x - lastPoint3.x)) <= 100) && ((abs(baseLocation.y - lastPoint3.y)) <= 100))
		{

			lastPoint1.x = lastPoint2.x;
			lastPoint2.x = lastPoint3.x;
			lastPoint3.x = baseLocation.x;
			lastPoint1.y = lastPoint2.y;
			lastPoint2.y = lastPoint3.y;
			lastPoint3.y = baseLocation.y;
			xx = (lastPoint1.x + lastPoint2.x + lastPoint3.x) / 3;
			yy = (lastPoint1.y + lastPoint2.y + lastPoint3.y) / 3;
			return (Point(xx, yy));

		}
	}

}



/**************************************************************/
//函数名称：getLocationFromImage
//函数功能：在整个图像中搜索目标的位置 
//传入参数：image（二值化去背景的图）
//返回参数：Poi （查找到的坐标）
//最后修改时间：2016年10月24日11：24                            
/**************************************************************/
Point getLocationFromImage(Mat image)
{
	Point Poi = (0, 0), conpoi = (0, 0);
	int i, j, x = 0, y = 0;
	int maxi = -1, maxj = -1, mini = 9999, minj = 9999;
	sumOfPixel = 0;
	for (i = 100; (i< IMG_SIZE_Y - 100) && (i + 3 <= IMG_SIZE_Y - 100); i = i + 3)
		for (j = 100; (j < IMG_SIZE_X - 100) && (j + 3 <= IMG_SIZE_X - 100); j = j + 3)
		{
			if (image.at<uchar>(i, j) == 255)
			{
				y = y + i;
				x = x + j;
				sumOfPixel++;
			}
		}
	if (sumOfPixel != 0)
	{
		Poi.x = x / sumOfPixel;
		Poi.y = y / sumOfPixel;
		IS_LOCATED = 1;

	}
	else
	{
		IS_LOCATED = 0;
	}
	return Poi;
}

/**************************************************************/
//函数名称：getLocationFromImage
//函数功能：在追踪区域中搜索目标的位置 
//传入参数：image （二值化去背景的图） x0,x1,y0,y1(追踪区域坐标)
//返回参数：Poi （查找到的坐标）
//最后修改时间：2016年10月24日11：24                            
/**************************************************************/
Point getLocationFromArea(Mat image, Point lastLocation)
{
	Point Poi = (0, 0), conpoi = (0, 0);
	int i, j, x = 0, y = 0;
	sumOfPixel = 0;
	int x0, x1, y0, y1;
	if ((lastLocation.x - sizeLocation) >= 0) x0 = lastLocation.x - sizeLocation;
	else x0 = 0;
	if ((lastLocation.x + sizeLocation) <= IMG_SIZE_X) x1 = lastLocation.x + sizeLocation;
	else x1 = IMG_SIZE_X;
	if ((lastLocation.y - sizeLocation) >= 0) y0 = lastLocation.y - sizeLocation;
	else y0 = 0;
	if ((lastLocation.y + sizeLocation) <= IMG_SIZE_Y) y1 = lastLocation.y + sizeLocation;
	else y1 = IMG_SIZE_Y;


	for (i = y0; i< y1; i++)
		for (j = x0; j < x1; j++)
		{
			if (image.at<uchar>(i, j) == 255)
			{
				y = y + i;
				x = x + j;
				sumOfPixel++;
			}
		}
	if (sumOfPixel > 20)
	{
		Poi.x = x / sumOfPixel;
		Poi.y = y / sumOfPixel;
		IS_LOCATED = 1;

	}
	else
	{
		IS_LOCATED = 0;
	}
	return Poi;
}

/**************************************************************/
//函数名称：drawrReferenceLine
//函数功能：在图像上画一个正方形方框.以传入点为圆心，size为边长
//传入参数：image  Poi
//最后修改时间：2016年10月24日11：24                            
/**************************************************************/
void drawrReferenceLine(Mat image, Point Poi, int size)
{
	drawFilledCircle(image, Poi);
	int SIZE;
	if (IS_LOCATED)
		SIZE = size;
	else
		SIZE = 0;

	X0 = Poi.x - SIZE;
	X1 = Poi.x + SIZE;
	Y0 = Poi.y - SIZE;
	Y1 = Poi.y + SIZE;
	drawLine(image, Point(X0, Y0), Point(X1, Y0));
	drawLine(image, Point(X0, Y0), Point(X0, Y1));
	drawLine(image, Point(X0, Y1), Point(X1, Y1));
	drawLine(image, Point(X1, Y0), Point(X1, Y1));
}

/**************************************************************/
//函数名称：drawrLandARR
//函数功能：在图像中心画一个正方形方框.以传入点为圆心，size为边长
//传入参数：image  Poi
//最后修改时间：2016年10月24日11：24                            
/**************************************************************/
void drawrLandARR(Mat image, Point Poi, int landFlag)
{
	drawFilledCircle(image, Poi);
	int SIZE = landSize;


	X0 = Poi.x - SIZE;
	X1 = Poi.x + SIZE;
	Y0 = Poi.y - SIZE;
	Y1 = Poi.y + SIZE;
	drawLandLine(image, Point(X0, Y0), Point(X1, Y0), landFlag);
	drawLandLine(image, Point(X0, Y0), Point(X0, Y1), landFlag);
	drawLandLine(image, Point(X0, Y1), Point(X1, Y1), landFlag);
	drawLandLine(image, Point(X1, Y0), Point(X1, Y1), landFlag);
}

/**************************************************************/
//函数名称：drawrReferenceLine
//函数功能：在图像上画一个实心圆点
//传入参数：image  Poi
//最后修改时间：2016年10月24日11：24                            
/**************************************************************/
void drawFilledCircle(Mat img, Point center)
{
	int thickness = -1;
	int lineType = 8;

	circle(img,
		center,
		5,
		Scalar(0, 0, 255),
		thickness,
		lineType);
}

/**************************************************************/
//函数名称：drawLandLine
//函数功能：在图像上画一条线
//传入参数：image  Poi
//最后修改时间：2016年10月24日11：24                            
/**************************************************************/
void drawLandLine(Mat img, Point start, Point end, int landFlag)
{
	int thickness = 2;
	int lineType = 8;
	int colorR = 0, colorG = 0, colorB = 0;
	if (landFlag)
	{
		colorG = 255;
		colorR = 0;
	}
	if (!landFlag)
	{
		colorG = 0;
		colorR = 255;
	}
	line(img,
		start,
		end,
		Scalar(colorB, colorG, colorR),
		thickness,
		lineType);
}

/**************************************************************/
//函数名称：drawrReferenceLine
//函数功能：在图像上画一条线
//传入参数：image  Poi
//最后修改时间：2016年10月24日11：24                            
/**************************************************************/
void drawLine(Mat img, Point start, Point end)
{
	int thickness = 2;
	int lineType = 8;
	int colorR = 0, colorG = 0, colorB = 0;
	if (IS_LOCATED)
	{
		colorG = 255;
		colorR = 0;
	}
	if (!IS_LOCATED)
	{
		colorG = 0;
		colorR = 255;
	}
	line(img,
		start,
		end,
		Scalar(colorB, colorG, colorR),
		thickness,
		lineType);
}
