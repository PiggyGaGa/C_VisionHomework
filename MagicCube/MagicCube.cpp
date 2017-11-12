
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "MagicCube.hpp"

#define FRAMENUMBER 5

Cube::Cube()
{
    this->showImag = cv::Mat::zeros(ImageSize, CV_8UC3);
    this->srcImag = cv::Mat::zeros(ImageSize, CV_8UC3);
}

void Cube::ImageProducer()
{
    cv::VideoCapture video0(0);
    cv::Mat frame;
    int count = 0;
    while(1)
    {
        video >> frame;
        if (count % FRAMENUMBER == 0)
        {
            //将图像的大小改成需要的大小
            if (frame.size != ImageSize)
            {
                frame.resize(ImageSize);
            } 
            if (flag_process % 2 == 1)
            {
                this->srcImag = frame;
                this->showImage = DetectCubeColor(frame);
                flag_process++;
            }
            
        }
        count++;
    }
    video0.release();
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




    return result;
}
