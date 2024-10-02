#ifndef FPS_H
#define FPS_H

// Define the FPS structure
typedef struct {
    double start;    // Start time
    double end;      // End time
    int numFrames;   // Number of frames
} FPS;

// Function declarations for FPS management
void fps_start(FPS *fps);
void fps_stop(FPS *fps);
void fps_update(FPS *fps);
double fps_elapsed(FPS *fps);
double fps_get_fps(FPS *fps);
void fps_reset(FPS *fps);
int fps_isPassed(FPS *fps, int numFrames);

#endif // FPS_H