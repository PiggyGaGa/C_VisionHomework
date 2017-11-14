#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include "MagicCube.hpp"
#include <iostream>
#include <cstdlib>

//#define DEBUG
//#define DEBUGSHOW
#define FRAMENUMBER 5
#define VIDEODEVICE "/dev/video0"
using std::cout; using std::vector;

volatile unsigned int flag_process = 0;
cv::Size ImageSize(720, 480);
Cube::Cube()
{
    this->showImage = cv::Mat(ImageSize, CV_8UC3, cv::Scalar(0, 0, 0));
    this->srcImage = cv::Mat(ImageSize, CV_8UC3, cv::Scalar(0)); 
}
void Cube::ImageProducer()
{
    cv::VideoCapture video0;//("./cube2.mp4");
    video0.open(VIDEODEVICE);
    if (!video0.isOpened())
    {
        cout << "Can not open video device " << std::endl;
        
        cout << "Please press any key to contine" << std::endl;
        std::getchar();
        std::exit(0);
    }
    cv::Mat frame;
    int count = 0;
    while(1)
    {
        if (flag_process % 2 == 0)
        {
            video0.read(frame);
            //video0 >> frame;

            if (count % FRAMENUMBER == 0)
            {
                //将图像的大小改成需要的大小
                if (!(frame.size() == ImageSize))
                {
                    cv::resize(frame, frame, ImageSize);
                } 
#ifdef DEBUGSHOW
            cv::imshow("frame", frame);
            cv::waitKey(30);
#endif
                frame.copyTo(this->srcImage);
                DetectCubeColor(frame);
            }
            flag_process++;
            count++;           
        }
        
    }
    video0.release();
}

void Cube::ImageShow()
{
    while(1)
    {
        if (flag_process % 2 == 1)
        {
            cv::imshow("Source Image from video", this->srcImage);
            cv::waitKey(30);
            flag_process++;
        }

    }
}

void Cube::DetectCubeColor(const cv::Mat &src)
{
    cv::Mat result;
    cv::Mat grayImage, cubeEdge;
    //灰度化
    cv::cvtColor(src, grayImage, cv::COLOR_BGR2GRAY);
    //滤波
#ifdef DEBUG
    cv::imshow("gray", grayImage);
    cv::waitKey(30);
#endif  
    cv::blur(grayImage, grayImage, cv::Size(3, 3));

    //寻找边缘
    int edgeThread = 20;

    cv::Canny(grayImage, cubeEdge, edgeThread, edgeThread * 3, 3);
    int element_shapec = cv::MORPH_RECT;
    int anc = 1;
    cv::Mat elementc = cv::getStructuringElement(element_shapec, cv::Size(2 * anc + 1, 2 * anc + 1));    
    cv::dilate(cubeEdge, cubeEdge, elementc);
    //cv::Mat elementcErode = cv::getStructuringElement(element_shapec, cv::Size(1 * anc + 1, 1 * anc + 1));
    cv::erode(cubeEdge, cubeEdge, elementc);

#ifdef DEBUG
    cv::imshow("canny edge", cubeEdge);
    cv::waitKey(30);
#endif
    //霍夫变换
    vector<cv::Vec4i> lines;
    cv::Mat cubeLine = cv::Mat::zeros(ImageSize, CV_8UC1);
    cv::HoughLinesP(cubeEdge, lines, 1, CV_PI / 180, 80, 50, 10);
    drawDetectLines(cubeLine, lines, cv::Scalar(255));

#ifdef DEBUG
    cv::imshow("hough translation lines", cubeLine);
    cv::waitKey(30);
#endif

    //膨胀腐蚀操作
    cv::Mat erodeDst, dilateDst;
    int element_shape = cv::MORPH_RECT;
    int an = 7;
    cv::Mat element = cv::getStructuringElement(element_shape, cv::Size(2 * an + 1, 2 * an + 1), cv::Point(an, an));    
    cv::dilate(cubeLine, dilateDst, element);
#ifdef DEBUG
    cv::imshow("after dilate", dilateDst);
    cv::waitKey(30);
#endif
    cv::erode(dilateDst, erodeDst, element);
    
#ifdef DEBUG
    cv::imshow("after erode ", erodeDst);
    cv::waitKey(30);
#endif
    //erodeDst.release();
    grayImage.release();
    cubeEdge.release();
    cv::Mat findCont;
    erodeDst.copyTo(findCont);
    vector<vector<cv::Point> > contours;
    vector<cv::Vec4i> hierarchy;
    cv::findContours(findCont, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    vector<cv::RotatedRect> matchRect;
    matchRect = findMatchRect(contours);

    if (matchRect.size() < 1)
    {
        return;
    }
    for (int i = 0; i < int(matchRect.size()); i++)
    {
#ifdef DEBUG
        cv::Point2f vertices[4];
        matchRect[i].points(vertices);
        for (int i = 0; i < 4; i++)
            cv::line(src, vertices[i], vertices[(i+1)%4], cv::Scalar(0,255,0));
        cv::imshow("with contours", src);
#endif
        cv::Mat cubeFace;
        RectInvertToMat(matchRect[i], src, cv::Size(100, 100), cubeFace);
        cv::imshow("detect face of cube", cubeFace);
        cv::waitKey(30);
        cv::Mat colorFace;
        RealCubeFaceColor(cubeFace, colorFace);
    }
    return;
}


void Cube::drawDetectLines(cv::Mat &back, const vector<cv::Vec4i> &lines, cv::Scalar color)
{
    vector<cv::Vec4i>::const_iterator iter1 = lines.begin();
    while(iter1 != lines.end())
    {
        cv::Point p1((*iter1)[0], (*iter1)[1]);
        cv::Point p2((*iter1)[2], (*iter1)[3]);
        line(back, p1, p2, color, 3);
        iter1++;
    }
    return;

}

vector<cv::RotatedRect> Cube::findMatchRect(const vector<vector<cv::Point> > &contours)
{
    vector<cv::RotatedRect> result;
    for (int i = 0; i < int(contours.size()); i++)
    {
        cv::RotatedRect rect = cv::minAreaRect(cv::Mat(contours[i]));
        if (isMatch(rect))
        {
            result.push_back(rect);
        }
    }
    return result;
}

bool Cube::isMatch(cv::RotatedRect rect)
{
    double ratio = rect.size.height / rect.size.width;
    if (ratio > 1)
    {
        ratio = rect.size.width / rect.size.height;
    }
    if (ratio < 0.5)
    {
        return false;
    }
    double area = rect.size.height * rect.size.width;
    if (area < 500 || area > 250000)
    {
        return false;
    }
    return true;
}

void Cube::RectInvertToMat(const cv::RotatedRect &rect, const cv::Mat &source, cv::Size plateSize, cv::Mat &dst)
{

	cv::RotatedRect minRect = rect;
	double r = (double)minRect.size.width / (double)minRect.size.height;
	double angle = minRect.angle;
	if (r < 1)
		angle = 90 + angle;   //��������ת

	cv::Mat rotmat = getRotationMatrix2D(minRect.center, angle, 1);
	//Create and rotate image  
	cv::Mat img_rotated;
	cv::warpAffine(source, img_rotated, rotmat, source.size(), CV_INTER_CUBIC);
	//Crop image  
	cv::Size rect_size = minRect.size;
	if (r < 1)
		cv::swap(rect_size.width, rect_size.height);
	cv::Mat img_crop;   //img_crop�����೵�ƾ�������
	cv::getRectSubPix(img_rotated, rect_size, minRect.center, img_crop);

	cv::Mat resultResized;
	resultResized.create(plateSize, CV_8UC3);
	resize(img_crop, resultResized, plateSize, 0, 0, cv::INTER_CUBIC);
	//Equalize croped image  
	//Mat grayResult;
	//if (resultResized.)
	//cvtColor(resultResized, grayResult, CV_BGR2GRAY);
	//blur(grayResult, grayResult, Size(3, 3));
	//grayResult = Histogram_Equalization(grayResult);
	dst = resultResized;
}

void Cube::RealCubeFaceColor(const cv::Mat &cubeFace, cv::Mat &result)
{
    result = cv::Mat(cubeFace.size(), CV_8UC3, cv::Scalar(0, 0, 0));
    cv::Mat grayFace, faceEdge;
    cv::cvtColor(cubeFace, grayFace, cv::COLOR_BGR2GRAY);
    cv::blur(grayFace, grayFace, cv::Size(3, 3));
    
    //寻找边缘
    int edgeThread = 20;

    cv::Canny(grayFace, faceEdge, edgeThread, edgeThread * 3, 3);
    int element_shapec = cv::MORPH_RECT;
    int anc = 7;
    cv::Mat elementc = cv::getStructuringElement(element_shapec, cv::Size(2 * anc + 1, 2 * anc + 1));    
    cv::dilate(faceEdge, faceEdge, elementc);
    //cv::Mat elementcErode = cv::getStructuringElement(element_shapec, cv::Size(1 * anc + 1, 1 * anc + 1));
    cv::erode(faceEdge, faceEdge, elementc);
    cv::imshow("face bin", faceEdge);
    cv::waitKey(30);
    // detect the rank of the cube 2 3 4 or 5
    int rank = findCubeRank(faceEdge);
    vector<cv::Rect> rectROI(rank * rank);
    vector<cv::Scalar> BGRColor(rank * rank);
    findRectROI(rank, faceEdge.size(), rectROI);
    detectROIColor(rank, cubeFace, rectROI, BGRColor);

    drawCubeFace(rectROI, BGRColor, result);
    cv::imshow("final answer", result);
    cv::waitKey(30);
}

int Cube::findCubeRank(const cv::Mat &faceEdge)
{
    vector<vector<cv::Point>> contours;
    vector<cv::Vec4i> hierarchy;
    cv::findContours(faceEdge, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    if (contours.size() < 7)
    {
        return 2;
    }
    else if (contours.size() < 14)
    {
        return 3;
    }
    else if (contours.size() < 20)
    {
        return 4;
    }
    else
    {
        return 5;
    }
}
void Cube::findRectROI(int rank, cv::Size faceSize, vector<cv::Rect> &rectROI)
{
    int width = faceSize.width / rank;
    int height = faceSize.height / rank;
    int count = 0;
    cv::Size size(width, height);
    for (int i = 0; i < rank; i++)
    {
        for (int j = 0; j < rank; j++)
        {
            cv::Point pt;
            pt.x = j * width;
            pt.y = i * height;
            rectROI[count] = cv::Rect(pt, size);
            count++;
        }
    }
}
void Cube::detectROIColor(int rank, const cv::Mat &cubeFace, vector<cv::Rect> rectROI, vector<cv::Scalar> &BGRColor)
{
    for (int i = 0;  i < int(rectROI.size()); i++)
    {
        //every color block get the center small part
        cv::Rect colorRect;
        colorRect.x = rectROI[i].x + int(0.25 * rectROI[i].width);
        colorRect.y = rectROI[i].y + int(0.25 * rectROI[i].height);
        colorRect.width = rectROI[i].width / 4;
        colorRect.height = rectROI[i].height / 4;
        
        cv::Mat roi = cubeFace(colorRect);
        cv::Mat hsv;
        cv::cvtColor(roi, hsv, cv::COLOR_BGR2HSV);
        BGRColor[i] = cv::mean(roi);

    }
}
void Cube::drawCubeFace(vector<cv::Rect> rectROI, vector<cv::Scalar> BGRColor, cv::Mat &result)
{
    for (int i = 0; i < int(rectROI.size()); i++)
    {
        cv::Mat roiImage = result(rectROI[i]);
        cv::Mat pic = cv::Mat(rectROI[i].size(), CV_8UC3, BGRColor[i]);    
        cv::Mat mask = cv::Mat(rectROI[i].size(), CV_8UC1, cv::Scalar(255));
        pic.copyTo(roiImage, mask);
    }
    int rank = std::sqrt(rectROI.size());
    for (int i = 1; i < rank; i++)
    {
        cv::Point p1(rectROI[rank * i].x, rectROI[rank * i].y);
        cv::Point p2(rectROI[rank * i].x + rectROI[i].width * rank, rectROI[rank * i].y);
        cv::Point p3(rectROI[i].x, rectROI[i].y);
        cv::Point p4(rectROI[i].x, rectROI[i].y + rectROI[i].height * rank);
        cv::line(result, p1, p2, cv::Scalar(255, 255, 255), 2);
        cv::line(result, p3, p4, cv::Scalar(255, 255, 255), 2);
    }
}