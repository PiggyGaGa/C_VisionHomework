#include "QuickResponse.hpp"
#include <iostream>
#include <cstdlib>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using std::vector;
#define DEBUG
#define VIDEODEVICE 0
void QR::start()
{
    cv::VideoCapture video;
    video.open(VIDEODEVICE);
    if (!video.isOpened())
    {
        std::cout << "can not open video device " << std::endl;
        std::cout << "press any key to continue " << std::endl;
        std::getchar();
        std::exit(0);

    }
    cv::Mat frame;
    while(1)
    {
        cv::Mat preProcessImg, edgeDectImg;
        video.read(frame);
        //预处理
        preProcess(frame, preProcessImg);
#ifdef DEBUG
        cv::imshow("preProcessImg", preProcessImg);
        cv::waitKey(30);
#endif 
        //边缘检测
        edgeDetection(preProcessImg, edgeDectImg);
/* #ifdef DEBUG
        cv::imshow("image edge", edgeDectImg);
        cv::waitKey(30);
#endif */
        //寻找DetectionPattern
        vector<vector<cv::Point> > detectionPattern;
        findDetectionPattern(edgeDectImg, preProcessImg, detectionPattern);
        //提取QR图像
        extractQRImg(detectionPattern, frame);

    }
}

void QR::preProcess(const cv::Mat &img, cv::Mat &preImg)
{
    cv::Mat grayImg;
    cv::cvtColor(img, grayImg, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(grayImg, grayImg, cv::Size(5, 5), 0);
    preImg = grayImg;

}

void QR::edgeDetection(const cv::Mat &preImg, cv::Mat &edgeImg)
{
    int edgeThresh = 10;
    cv::Canny(preImg, edgeImg, edgeThresh, edgeThresh * 3);
#ifdef DEBUG
    cv::imshow("After Canny Image", edgeImg);
    cv::waitKey(30);
#endif
/*
    int element_shape = cv::MORPH_RECT;
    int an = 1;
    cv::Mat element = cv::getStructuringElement(element_shape, cv::Size(an*2+1, an*2+1), cv::Point(an, an));
    cv::dilate(edgeImg, edgeImg, element);
#ifdef DEBUG
    cv::imshow("After dilate image", edgeImg);
    cv::waitKey(30);
#endif

    //利用霍夫变换自己重新画直线
    vector<cv::Vec4i> lines;
    cv::Mat QRLine = cv::Mat::zeros(preImg.size(), CV_8UC1);
    cv::HoughLinesP(edgeImg, lines, 1, CV_PI / 180, 80, 50, 10);
    drawDetectLines(QRLine, lines, cv::Scalar(255));
#ifdef DEBUG
    cv::imshow("HougH draw lines", QRLine);
    cv::waitKey(30);
#endif */
}


void QR::findDetectionPattern(const cv::Mat &edgeImg, 
    const cv::Mat &preProcImg, 
    vector<vector<cv::Point> > &detPattern)
{
    vector<cv::RotatedRect> dpResult(3);
    vector<vector<cv::Point>> contours;
    vector<cv::Vec4i> hierarchy;
    vector<vector<cv::Point>> hierarMthanFive;  //hierarchy more than 5
    cv::findContours(edgeImg, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
    findMatchContours(contours, hierarchy, hierarMthanFive);
#ifdef DEBUG
    std::cout << "the numbeer of contours hierar more than five" << hierarMthanFive.size() << std::endl;
#endif
    if (hierarMthanFive.size() == 3)
    {
       detPattern = hierarMthanFive;
    }
    else if (hierarMthanFive.size() > 3)
    {
        vector<vector<cv::Point>> correctDP;  //correct result of detection pattern
        correctThreePattern(hierarMthanFive, preProcImg, correctDP);
        detPattern = correctDP;
        
    }
    else
    {
        return;
    }

}

void QR::extractQRImg(const vector<vector<cv::Point> > &detPattern, cv::Mat &frame)
{
    if (detPattern.size() != 3)
    {
        return;
    }
    vector<cv::Point> pointsContours;   //three  detection patertion contours points;
    for (int i = 0; i < int(detPattern.size()); i++)
    {
        pointsContours.insert(pointsContours.end(), detPattern[i].begin(), detPattern[i].end());
    }
    cv::RotatedRect rect = cv::minAreaRect(cv::Mat(pointsContours));
    cv::Point2f vertices[4];
    rect.points(vertices);
    for (int i = 0; i < 4; i++)
    {
        line(frame, vertices[i], vertices[(i+1)%4], cv::Scalar(0,255,0));
    } 
    cv::imshow("QRCode", frame);
    cv::waitKey(30);
}
void QR::decode()
{

}


 void QR::findMatchContours(const vector<vector<cv::Point>> &contours, 
    const vector<cv::Vec4i> &hierarchy, 
    vector<vector<cv::Point>> &hierarMthanFive)
 {
    int dimention = contours.size();
    for (int i = 0; i < dimention; i++)
    {
        int k = i;
        int count = 0;
        while(hierarchy[k][2] > 0)
        {
            count++;
            k = hierarchy[k][2];
        }
        if (count > 5)
        {
            cv::RotatedRect rect = cv::minAreaRect(cv::Mat(contours[i]));
            double ratio = double(rect.size.width) / rect.size.height;
            if (ratio > 1)
            {
                ratio = double(rect.size.height) / rect.size.width;
            }
            if (ratio > 0.85)
            {
                hierarMthanFive.push_back(contours[i]);
            }
           
        }
    }
 }

 void QR::correctThreePattern(
    const vector<vector<cv::Point>> &hierarMthanFive, 
    const cv::Mat &preProcImg, 
    vector<vector<cv::Point>> &correctDP)
{
    cv::Mat binImg;
    cv::threshold(preProcImg, binImg, 100, 255, cv::THRESH_BINARY_INV);
    vector<cv::RotatedRect> rectVec;
    for (int i = 0; i < int(hierarMthanFive.size()); i++)
    {
        rectVec[i] = cv::minAreaRect(cv::Mat(hierarMthanFive[i]));
    }

}

void QR::drawDetectLines(cv::Mat &back, const vector<cv::Vec4i> &lines, cv::Scalar color)
{
    vector<cv::Vec4i>::const_iterator iter1 = lines.begin();
    while(iter1 != lines.end())
    {
        cv::Point p1((*iter1)[0], (*iter1)[1]);
        cv::Point p2((*iter1)[2], (*iter1)[3]);
        line(back, p1, p2, color, 2);
        iter1++;
    }
    return;

}