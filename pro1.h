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

int IMG_SIZE_X;					//ͼ��Ĵ�С
int IMG_SIZE_Y;
int CENTER_X;					//ͼ�����ĵ�λ��
int CENTER_Y;

using namespace std;
using namespace cv;

static Point realPoi;				//Ŀ��ļ�������
static int sizeLocation = 150;		//Ŀ��궨��Ĵ�С
static int IS_LOCATED = 0;			//׷�ٵ�Ŀ��ı�ʶ
static int sumOfPixel = 0;			//Ŀ������ص��ܺ�
static int X0, X1, Y0, Y1;
static Point lastPoint1 = (-1, -1), lastPoint2 = (-1, -1), lastPoint3 = (-1, -1);
int CH1 = -2, CH2, CH3;
int landFlag = 0;					//�Ƿ��䣨�ﵽͼ�����ģ��ı�ʶ
int landSize = 10;					//���䷶Χ�Ĵ�С

Mat backImg, image, tem, reImg;		//backImg�Ǳ���
double learningRate = 0.1;	// ���Ʊ����ۻ�ѧϰ������
int nThreshold = 100;	// ǰ����ȡ����ֵ


void init();			//��ʼ������
void  backinit();		//��ʼ������
void frame();			//��֡����
int  wait_key();
//��ͼ���ϻ�һ�������η���
void drawrReferenceLine(Mat image, Point Poi, int size);
//������ͼ��������Ŀ���λ��
Point getLocationFromImage(Mat image);
//��������±���
Mat reBuiltImage(Mat image, Mat  backImage);
//�Դ���ı���ͼ���лҶȻ����������и�˹�˲�
Mat reBuiltBackgroundImage(Mat backImage);
//��ͼ���ϻ�һ��ʵ��Բ��
void drawFilledCircle(Mat img, Point center);
//��ͼ���ϻ�һ����
void drawLine(Mat img, Point start, Point end);
//��׷������������Ŀ���λ�� 
Point getLocationFromArea(Mat image, Point lastLocation);
//�Դ������������˲�������������㶶��
Point reviseLocation(Point baseLocation);
//����Ŀ���λ�á���Ŀ��궨������������Ŀ�������
Point trackROI(Mat image, Mat imageOutput, Point lastLocation);
//����׷�ٵ���Ŀ���С��ȷ����һ�α�������
void adjustARR();
void drawLandLine(Mat img, Point start, Point end, int landFlag);
//��ͼ�����Ļ�һ�������η���.�Դ����ΪԲ�ģ�sizeΪ�߳�
void drawrLandARR(Mat image, Point Poi, int landFlag);

Point location, lastLocation;		//Ŀ��λ�á�Ŀ������λ��
Mat pFrame;			//��ȡÿһ֡

#endif // !PRO1

