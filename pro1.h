#pragma once
#ifndef PRO1
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>   
#include "opencv2/imgproc/imgproc.hpp"
#include "cvaux.h"
#include <iostream>
#include <cv.h>  
#include <cmath>

int IMG_SIZE_X;					//图像的大小
int IMG_SIZE_Y;
int CENTER_X;					//图像中心的位置
int CENTER_Y;

using namespace std;
using namespace cv;

static Point realPoi;				//目标的几何中心
static int sizeLocation = 150;		//目标标定框的大小
static int IS_LOCATED = 0;			//追踪到目标的标识
static int sumOfPixel = 0;			//目标的像素点总和
static int X0, X1, Y0, Y1;
static Point lastPoint1 = (-1, -1), lastPoint2 = (-1, -1), lastPoint3 = (-1, -1);
int CH1 = -2, CH2, CH3;
int landFlag = 0;					//是否降落（达到图像中心）的标识
int landSize = 10;					//降落范围的大小

Mat backImg, image, tem, reImg;		//backImg是背景
double learningRate = 0.1;	// 控制背景累积学习的速率
int nThreshold = 100;	// 前景提取的阈值


void init();			//初始化窗口
void  backinit();		//初始化背景
void frame();			//逐帧处理
int  wait_key();
//在图像上画一个正方形方框
void drawrReferenceLine(Mat image, Point Poi, int size);
//在整个图像中搜索目标的位置
Point getLocationFromImage(Mat image);
//背景差并更新背景
Mat reBuiltImage(Mat image, Mat  backImage);
//对传入的背景图进行灰度化处理，并进行高斯滤波
Mat reBuiltBackgroundImage(Mat backImage);
//在图像上画一个实心圆点
void drawFilledCircle(Mat img, Point center);
//在图像上画一条线
void drawLine(Mat img, Point start, Point end);
//在追踪区域中搜索目标的位置 
Point getLocationFromArea(Mat image, Point lastLocation);
//对传入的坐标进行滤波处理，减少坐标点抖动
Point reviseLocation(Point baseLocation);
//计算目标的位置、将目标标定出来，并返回目标的坐标
Point trackROI(Mat image, Mat imageOutput, Point lastLocation);
//根据追踪到的目标大小，确定下一次遍历区域
void adjustARR();
void drawLandLine(Mat img, Point start, Point end, int landFlag);
//在图像中心画一个正方形方框.以传入点为圆心，size为边长
void drawrLandARR(Mat image, Point Poi, int landFlag);

Point location, lastLocation;		//目标位置、目标最新位置
Mat pFrame;			//存取每一帧

#endif // !PRO1

