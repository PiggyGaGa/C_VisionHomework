#ifdef CUBE_H

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <thread>
cv::Size ImagSize(720, 480);

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