#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include "MagicCube.hpp"
#include <cstdio>
#include <cstdlib>

#define FRAMENUMBER 5
volatile unsigned int flag_process = 0;
cv::Size ImageSize(720, 480);

Cube::Cube()
{
    this->showImag = cv::Mat::zeros(ImageSize, CV_8UC3);
    this->srcImag = cv::Mat::zeros(ImageSize, CV_8UC3);
}

void Cube::ImageProducer()
{
    if (flag_process % 2 == 1)
    {
        cv::VideoCapture video0(0);
        if (!video0.isOpened())
        {
            std::cout << "Can not open video device " << std::endl;
            
            std::cout << "Please press any key to contine" << std::endl;
            std::getchar();
            std::exit(0);
        }
        cv::Mat frame;
        int count = 0;
        while(1)
        {
            video0.read(frame);
            //video0 >> frame;
            if (count % FRAMENUMBER == 0)
            {
                //将图像的大小改成需要的大小
                if (frame.size != ImageSize)
                {
                    frame.resize(ImageSize);
                } 
                this->srcImag = frame;
                this->showImage = DetectCubeColor(frame);
            }
            count++;
        }
        video0.release();
        flag_process++;
    }
    
}

void Cube::ImageDetect()
{
    while(1)
    {
        if (flag_process % 2 == 0)
        {
            cv::imshow("Source Image from video", this->srcImag);
            cv::waitKey(30);
            cv::imshow("Detect Color", this->showImag);
            cv::waitKey(30);
            flag_process++;
        }

    }
}

cv::Mat Cube::DetectCubeColor(const cv::Mat &src)
{
    cv::Mat result;
    cv::Mat grayImage, cubeEdge, binImage;
    //灰度化
    cv::cvtColor(src, grayImage, cv::COLOR_BGR2GRAY);
    //滤波
    cv::blur(grayImage, grayImage, cv::Size(3, 3));

    //寻找边缘
    int edgeThread = 1;

    cv::Canny(grayImage, cubeEdge, edgeThread, edgeThread * 3, 3);

    //膨胀腐蚀操作
    
}