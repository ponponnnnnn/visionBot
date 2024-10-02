#include "fps.h"
#include <time.h>
#include <stdio.h>

// Start the FPS timer
void fps_start(FPS *fps) {
    fps->start = (double)clock() / CLOCKS_PER_SEC;
    fps->end = 0.0;
    fps->numFrames = 0;
}

// Stop the FPS timer
void fps_stop(FPS *fps) {
    fps->end = (double)clock() / CLOCKS_PER_SEC;
}

// Update the frame count
void fps_update(FPS *fps) {
    fps->numFrames++;
}

// Calculate the elapsed time
double fps_elapsed(FPS *fps) {
    return ((fps->end == 0.0) ? (double)clock() / CLOCKS_PER_SEC : fps->end) - fps->start;
}

// Calculate frames per second
double fps_get_fps(FPS *fps) {
    return fps->numFrames / fps_elapsed(fps);
}

// Reset the FPS timer and frame count
void fps_reset(FPS *fps) {
    fps->start = (double)clock() / CLOCKS_PER_SEC;
    fps->end = 0.0;
    fps->numFrames = 0;
}

// Check if a specific number of frames has passed
int fps_isPassed(FPS *fps, int numFrames) {
    return fps->numFrames >= numFrames;
}