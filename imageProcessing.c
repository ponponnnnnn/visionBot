#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include "imageProcessing.h"

using namespace cv;

typedef struct {
    double x;
    double y;
} Flow;

// Initialize the algorithm (DISOpticalFlow)
MyAlgorithm* init_algorithm() {
    MyAlgorithm* alg = (MyAlgorithm*)malloc(sizeof(MyAlgorithm));
    if (!alg) {
        fprintf(stderr, "Error allocating memory for Algorithm\n");
        exit(EXIT_FAILURE);
    }
    alg->dis = DISOpticalFlow::create(0);
    alg->dis->setFinestScale(0);
    return alg;
}

// Calculate optical flow using DISOpticalFlow
Mat calculate_optical_flow(MyAlgorithm* alg, const Mat& previousFrame, const Mat& currentFrame) {
    Mat flow, prevGray, currGray;

    // Ensure frames are not empty
    if (previousFrame.empty() || currentFrame.empty()) {
        fprintf(stderr, "Error: One of the input frames is empty.\n");
        return flow;
    }

    // Convert frames to grayscale if they are not already
    if (previousFrame.channels() > 1) {
        cv::cvtColor(previousFrame, prevGray, COLOR_BGR2GRAY);
    } else {
        prevGray = previousFrame;
    }

    if (currentFrame.channels() > 1) {
        cvtColor(currentFrame, currGray, cv::COLOR_BGR2GRAY);
    } else {
        currGray = currentFrame;
    }

    // Ensure the frames are of the correct depth (8-bit unsigned integer)
    if (prevGray.depth() != CV_8U || currGray.depth() != CV_8U) {
        fprintf(stderr, "Error: Input frames must have 8-bit unsigned depth.\n");
        return flow;
    }

    // Calculate optical flow
    alg->dis->calc(prevGray, currGray, flow);

    return flow;
}


// Perform contrast enhancement on the frame
Mat contrast_enhance(const Mat& frame) {
    Mat frame_c, blur, unsharpFrame;
    frame.copyTo(frame_c);
    GaussianBlur(frame_c, blur, Size(7, 7), 5.0);
    addWeighted(frame_c, 5, blur, -4, 0, unsharpFrame);
    return unsharpFrame;
}

// Initialize SimpleFlow structure
SimpleFlow* init_simple_flow(float threshold, int presize, int postsize, Mat* K, const char* method, float noise_stddev) {
    SimpleFlow* flowObj = (SimpleFlow*)malloc(sizeof(SimpleFlow));
    if (flowObj == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for SimpleFlow object.\n");
        return NULL;
    }

    flowObj->th = threshold;
    flowObj->presize = presize;
    flowObj->postsize = postsize;
    flowObj->K = K;  // K can be NULL
    flowObj->method = strdup(method);  // Copy method string
    flowObj->noise_stddev = noise_stddev;

    return flowObj;
}

// Calculate optical flow using SimpleFlow
Mat calculate_optical_flow(SimpleFlow* flowObj, const Mat& previousFrame, const Mat& currentFrame, float previous_pose[6], float current_pose[6]) {
    // Perform optical flow calculation
    Mat flow, D;

    // Ensure frames are not empty
    if (previousFrame.empty() || currentFrame.empty()) {
        fprintf(stderr, "Error: One of the input frames is empty.\n");
        return flow;
    }
    pdrun(previousFrame, currentFrame, previous_pose, current_pose, flowObj->th, flowObj->presize, flowObj->postsize, flowObj->K, flowObj->method, flowObj->noise_stddev, flow, D);

    return flow;
}

Flow* polartoxy(double* mag, double* ang, int size) {
    Flow* flow = (Flow*)malloc(size * sizeof(Flow));
    if (flow == NULL) {
        return NULL; // Memory allocation failure
    }

    for (int i = 0; i < size; i++) {
        double angle_rad = ang[i] * M_PI / 180.0; // Convert angle to radians
        flow[i].x = mag[i] * cos(angle_rad);
        flow[i].y = mag[i] * sin(angle_rad);
    }

    return flow;
}

void matmul(double result[3], double matrix[3][3], double vector[3]) {
    for (int i = 0; i < 3; i++) {
        result[i] = 0.0;
        for (int j = 0; j < 3; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
    }
}

// Rotation matrix for Euler angles in X and Y
void rotation_matrix(double rx, double ry, double matrix[3][3]) {
    // Rotation matrix for x-axis
    double R_x[3][3] = {
        {1, 0, 0},
        {0, cos(rx), -sin(rx)},
        {0, sin(rx), cos(rx)}
    };

    // Rotation matrix for y-axis
    double R_y[3][3] = {
        {cos(ry), 0, sin(ry)},
        {0, 1, 0},
        {-sin(ry), 0, cos(ry)}
    };

    // Multiply the two matrices R_x and R_y
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrix[i][j] = 0;
            for (int k = 0; k < 3; k++) {
                matrix[i][j] += R_x[i][k] * R_y[k][j];
            }
        }
    }
}

// Function to calculate angle based on camera motion
double* angleonpixel(double* frame, int h, int w, double* prev_pose, double* curr_pose, double K[3][3], double rotation_threshold) {
    double rx = curr_pose[4];
    double ry = curr_pose[5];

    // Check for significant rotation
    double rx_diff = fabs(curr_pose[4] - prev_pose[4]);
    double ry_diff = fabs(curr_pose[5] - prev_pose[5]);
    if (rx_diff > rotation_threshold || ry_diff > rotation_threshold) {
        printf("Warning: Significant rotation detected between frames.\n");
    }

    // Intrinsics: fx = fy and are in K[0][0]
    double f = K[0][0];
    double p0x = K[0][2];
    double p0y = K[1][2];

    // Translation from world frame to camera frame
    double xd = -(curr_pose[1] - prev_pose[1]);
    double yd = -(curr_pose[2] - prev_pose[2]);
    double zd = curr_pose[3] - prev_pose[3];

    double translation[3] = {xd, yd, zd};
    double camera[3];
    
    // Get the rotation matrix
    double rot_matrix[3][3];
    rotation_matrix(rx, -ry, rot_matrix);
    matmul(camera, rot_matrix, translation);

    double d_x = camera[0];
    double d_y = camera[1];
    double d_z = camera[2];

    // Allocate memory for the angle array
    double* angle = (double*)malloc(h * w * sizeof(double));

    // Calculate the angle based on translation
    if (d_z != 0) {
        p0x += f * d_x / d_z;
        p0y += f * d_y / d_z;
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                int idx = i * w + j;
                angle[idx] = fmod(atan2(i - p0y, j - p0x) * 180 / M_PI + 360, 360);
            }
        }
    } else {
        double ang_val = fmod(atan2(d_y, d_x) * 180 / M_PI + 360, 360);
        for (int i = 0; i < h * w; i++) {
            angle[i] = ang_val;
        }
    }

    return angle;
}

// Function declaration for polartoxy and angleonpixel
Flow* polartoxy(double* mag, double* ang, int size);
double* angleonpixel(double* frame, int h, int w, double* prev_pose, double* curr_pose, double K[3][3], double rotation_threshold);

Flow* pdrun(Mat frame1, Mat frame2, double* pos1, double* pos2, double th, int presize, int postsize, double K[3][3], const char* method, double noise_stddev){
    // Image pre-processing (blur)
    Mat prev, curr;
    if (presize != 1) {
        blur(frame1, prev, Size(presize, presize));
        blur(frame2, curr, Size(presize, presize));
    } 
    else {
        prev = frame1;
        curr = frame2;
    }

    // Get the frame dimensions
    int h = prev.rows;
    int w = prev.cols;

    // Estimate angle using the pose
    double* pd_degr = angleonpixel((double*)prev.data, h, w, pos1, pos2, K, 5.0);

    Flow* flow = NULL;

    if (strcmp(method, "simple") == 0) {
        double* mag = (double*)malloc(h * w * sizeof(double)); // Placeholder for magnitude
        // Add logic for calculating optical flow magnitude, if needed
        flow = polartoxy(mag, pd_degr, h * w);
        free(mag);
    } 
    else {
        printf("Warning: invalid method %s\n", method);
    }

    free(pd_degr); // Free the angle array

    return flow;
}
