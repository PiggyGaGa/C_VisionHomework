#include <vector>
#include <opencv2/core.hpp>

using std::vector;
class QR
{
public:
    void start();
   

private:
    //预处理
    void preProcess(const cv::Mat &img, cv::Mat &preImg);
    //检测边缘
    void edgeDetection(const cv::Mat &preImg, cv::Mat &edgeImg);
    //寻找三个detection pattern
    void findDetectionPattern(const cv::Mat &edgeImg, 
        const cv::Mat &preProcImg, 
        vector<vector<cv::Point>> &detPattern);
    //提取QRCode区域
    void extractQRImg(const vector<vector<cv::Point>> &detPattern, cv::Mat &frame);
    //解码
    void decode();

    void findMatchContours(
        const vector<vector<cv::Point>> &contours, 
        const vector<cv::Vec4i> &hierarchy, 
        vector<vector<cv::Point>> &hierarMthanFive);

    void correctThreePattern(
        const vector<vector<cv::Point>> &hierarMthanFive, 
        const cv::Mat &preProcImg, 
        vector<vector<cv::Point>> &correctDP);

    
    void drawDetectLines(cv::Mat &back, 
        const vector<cv::Vec4i> &lines, 
        cv::Scalar color);
};