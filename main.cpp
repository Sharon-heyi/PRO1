#include"pro1.h"

VideoCapture capture(0);
int main()
{
	int flag = 0;
	init();							//��ʼ������
	backinit();						//��ʼ������
	cout << IMG_SIZE_X << endl;		//��ʾͼ�񳤺Ϳ�
	cout << IMG_SIZE_Y << endl;
	while (1)
	{
		frame();					//����ÿ֡ͼ��
		flag = wait_key();
		if (!flag) break;
	}
	return 0;

}


void init()//��ʼ������
{
	namedWindow("imageOutput", CV_WINDOW_NORMAL);
	namedWindow("imageORI", CV_WINDOW_NORMAL);
	namedWindow("BACK", CV_WINDOW_NORMAL);
}




void  backinit()//��ʼ������
{
	capture >> pFrame;
	IMG_SIZE_X = pFrame.cols;
	IMG_SIZE_Y = pFrame.rows;
	CENTER_X = IMG_SIZE_X / 2;
	CENTER_Y = IMG_SIZE_Y / 2;
	tem = pFrame;
	tem.copyTo(backImg);
	backImg = reBuiltBackgroundImage(backImg);			//�Ա������лҶȻ�����˹�˲�
	imshow("BACK", backImg);

}

void frame()//��֡����
{
	capture >> pFrame;
	image = pFrame;
	reImg = reBuiltImage(image, backImg);				//�������
														//��ͼ��������
	int g_nStructElementSize = 3;
	Mat element = getStructuringElement(MORPH_RECT, Size(2 * g_nStructElementSize + 1, 2 * g_nStructElementSize + 1), Point(g_nStructElementSize, g_nStructElementSize));
	dilate(reImg, reImg, element);

	location = trackROI(image, reImg, lastLocation);		//����Ŀ���λ�á��궨Ŀ��
	lastLocation = location;
}

int  wait_key()
{
	if (waitKey(1) >= 0) return 0;
}



/**************************************************************/
//�������ƣ�adjustARR()
//�������ܣ�����׷�ٵ���Ŀ���С��ȷ����һ�α�������
//�����������
//���ز�������
//����޸�ʱ�䣺2016��10��19��14��43                            
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
//�������ƣ�trackROI
//�������ܣ�����Ŀ���λ�á���Ŀ��궨������������Ŀ�������
//���������image,imageOutput, lastLocation
//���ز�����Poi ���������Ŀ�꼸�����ģ�
//����޸�ʱ�䣺2017��8��10��13��50                            
/**************************************************************/

Point trackROI(Mat image, Mat imageOutput, Point lastLocation)
{
	Point Poi;
	if (IS_LOCATED)
	{
		//���׷�ٵ�Ŀ�꣬�������ʵ�ļ�������
		adjustARR();												//��С��������
		Poi = getLocationFromArea(imageOutput, lastLocation);		//��׷������������Ŀ���λ��
		realPoi = Poi;
	}
	if (!IS_LOCATED)
	{
		Poi = getLocationFromImage(imageOutput);					//������ͼ��������Ŀ���λ�� 
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
//�������ƣ�reBuiltImage
//�������ܣ���������±���
//���������image,backImage
//���ز�����imageOutput ��������ͼ��
//����޸�ʱ�䣺2016��10��19��14��43                            
/**************************************************************/
Mat reBuiltImage(Mat image, Mat  backImage)
{
	Mat gray_image = Mat::zeros(image.size(), CV_32F);
	Mat imageOutput = Mat::zeros(image.size(), image.type());
	gray_image.convertTo(backImg, CV_32F);
	gray_image.convertTo(backImage, image.type());
	cvtColor(image, gray_image, CV_BGR2GRAY);
	medianBlur(gray_image, gray_image, 3);//��ֵ�˲�
	absdiff(gray_image, backImage, imageOutput);
	threshold(imageOutput, imageOutput, nThreshold, 255, THRESH_BINARY_INV);
	accumulateWeighted(gray_image, backImg, learningRate);
	return imageOutput;
}

/**************************************************************/
//�������ƣ�reBuiltBackgroundImage
//�������ܣ��Դ���ı���ͼ���лҶȻ����������и�˹�˲�
//���������backImage������ͼ��
//���ز�����backImage2 �������ı���ͼ��
//����޸�ʱ�䣺2017��8��10��11��38                            
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
//�������ƣ�reviseLocation
//�������ܣ��Դ������������˲�������������㶶��
//���������baseLocation��δ���˲������꣩
//���ز�����Point��xx,yy�� ���˲��������ֵ��
//����޸�ʱ�䣺2016��10��24��11��24                            
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
//�������ƣ�getLocationFromImage
//�������ܣ�������ͼ��������Ŀ���λ�� 
//���������image����ֵ��ȥ������ͼ��
//���ز�����Poi �����ҵ������꣩
//����޸�ʱ�䣺2016��10��24��11��24                            
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
//�������ƣ�getLocationFromImage
//�������ܣ���׷������������Ŀ���λ�� 
//���������image ����ֵ��ȥ������ͼ�� x0,x1,y0,y1(׷����������)
//���ز�����Poi �����ҵ������꣩
//����޸�ʱ�䣺2016��10��24��11��24                            
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
//�������ƣ�drawrReferenceLine
//�������ܣ���ͼ���ϻ�һ�������η���.�Դ����ΪԲ�ģ�sizeΪ�߳�
//���������image  Poi
//����޸�ʱ�䣺2016��10��24��11��24                            
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
//�������ƣ�drawrLandARR
//�������ܣ���ͼ�����Ļ�һ�������η���.�Դ����ΪԲ�ģ�sizeΪ�߳�
//���������image  Poi
//����޸�ʱ�䣺2016��10��24��11��24                            
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
//�������ƣ�drawrReferenceLine
//�������ܣ���ͼ���ϻ�һ��ʵ��Բ��
//���������image  Poi
//����޸�ʱ�䣺2016��10��24��11��24                            
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
//�������ƣ�drawLandLine
//�������ܣ���ͼ���ϻ�һ����
//���������image  Poi
//����޸�ʱ�䣺2016��10��24��11��24                            
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
//�������ƣ�drawrReferenceLine
//�������ܣ���ͼ���ϻ�һ����
//���������image  Poi
//����޸�ʱ�䣺2016��10��24��11��24                            
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
