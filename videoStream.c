#include "videoStream.h"
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace cv;

// 初始化影像流
VideoCapture initializeVideoStream(const char* inputFile, Size frameHW, int frameRate) {
    VideoCapture vs;
    if (inputFile != NULL) {
        vs.open(inputFile);
        if (!vs.isOpened()) {
            fprintf(stderr, "Error opening video file: %s\n", inputFile);
            exit(EXIT_FAILURE);
        }
        else{
            printf("successfully open video!\n");
        }
    } 
    else {
        vs.open(0);
        if (!vs.isOpened()) {
            fprintf(stderr, "Error opening default camera\n");
            exit(EXIT_FAILURE);
        }
    }
    vs.set(CAP_PROP_FRAME_WIDTH, frameHW.width);
    vs.set(CAP_PROP_FRAME_HEIGHT, frameHW.height);
    vs.set(CAP_PROP_FPS, frameRate);

    return vs;
}
