#ifndef CUBE_H
#define CUBE_H
<<<<<<< HEAD
=======

>>>>>>> 0a6cbbdb6b796a41fa0c54c18a42a268d63c1984

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <thread>
using std::vector;
class Cube
{
public:
    Cube();
    void DetectCubeColor(const cv::Mat &src);
    void ImageProducer();
    void ImageShow();
private:
    void drawDetectLines(cv::Mat &back, const vector<cv::Vec4i> &lines, cv::Scalar color);
    vector<cv::RotatedRect> findMatchRect(const vector<vector<cv::Point> > &contours);
    bool isMatch(cv::RotatedRect rect);
    void RectInvertToMat(const cv::RotatedRect &rect, const cv::Mat &source, cv::Size plateSize, cv::Mat &dst);
    void RealCubeFaceColor(const cv::Mat &cubeFace, cv::Mat &result);
    int findCubeRank(const cv::Mat &faceEdge);
    void findRectROI(int rank, cv::Size faceEdge, vector<cv::Rect> &rectROI);
    void detectROIColor(int rank, const cv::Mat &cubeFace, vector<cv::Rect> rectROI, vector<cv::Scalar> &BGRColor);
    void drawCubeFace(vector<cv::Rect> rectROI, vector<cv::Scalar> BGRColor, cv::Mat &result);

    cv::Mat srcImage;
    cv::Mat showImage;
};
#endif 