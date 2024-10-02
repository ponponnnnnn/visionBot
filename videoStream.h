#ifndef VIDEOSTREAM_H
#define VIDEOSTREAM_H
#include <opencv2/opencv.hpp>

cv::VideoCapture initializeVideoStream(const char* inputFile, cv::Size frameHW, int frameRate);

#endif
