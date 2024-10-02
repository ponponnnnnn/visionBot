// imageProcessing.h
#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <opencv2/opencv.hpp>

using namespace cv;

typedef struct {
    Ptr<DISOpticalFlow> dis;
} MyAlgorithm;

typedef struct SimpleFlow {
    float th;         // threshold
    int presize;      // presize
    int postsize;     // postsize
    Mat* K;       // K matrix (can be NULL)
    char* method;     // method ('simple', etc.)
    float noise_stddev;  // noise standard deviation
} SimpleFlow;


SimpleFlow* init_simple_flow(float threshold, int presize, int postsize, Mat* K, const char* method, float noise_stddev);
Mat calculate_optical_flow(SimpleFlow* flowObj, const Mat& previousFrame, const Mat& currentFrame, float previous_pose[6], float current_pose[6]);
MyAlgorithm* init_algorithm();
Mat calculate_optical_flow(MyAlgorithm* alg, const Mat& previousFrame, const Mat& currentFrame);
Mat contrast_enhance(const Mat& frame);
Mat polartoxy(const Mat& mag, const Mat& ang);
Mat angleonpixel(const Mat& frame, const float prev_pose[6], const float curr_pose[6], const Mat& K, double rotation_threshold);
std::pair<Mat, int> pdrun(const Mat& frame1, const Mat& frame2, const float pos1[6], const float pos2[6], float th, int presize, int postsize, const Mat& K, const char* method, double noise_stddev);

#endif // IMAGE_PROCESSING_H
