#ifdef CUBE_H
#include <opencv2/core.hpp>
volatile unsigned int flag_process = 0;
cv::Size ImageSize(720, 480);
class Cube
{
public:
    
    Cube();
    
    cv::Mat DetectCubeColor(const cv::Mat &src);
    void ImageProducer();
    void ImageShow();
private:
    cv::Mat srcImag;
    cv::Mat showImag;
    
};


#endif 