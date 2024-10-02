#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include "collisionDetection.h"
#include "fps.h"
#include "snn.h"
#include "imageProcessing.h"

#define FILE_AVOID_COMMAND "/home/lingyi/.local/share/PrismLauncher/instances/1.12.2_pipe_input_lua/.minecraft/mods/advancedMacros/macros/output_avoid_command_here"
#define FILE_POSE "/home/lingyi/.local/share/PrismLauncher/instances/1.12.2_pipe_input_lua/.minecraft/mods/advancedMacros/macros/output_pose_here"

using namespace cv;

// Function to keep writing the "forward" command to the file
void keep_going() {
    FILE *file;
    while (1) {
        file = fopen(FILE_AVOID_COMMAND, "w");
        if (file == NULL) {
            perror("Error opening avoid command file");
            exit(EXIT_FAILURE);
        }

        fprintf(file, "forward\n");
        fflush(file);  // Ensure the command is written immediately
        fclose(file);

        // Increase the delay to reduce command frequency
        usleep(40000);  // 40ms delay
    }
}

// Function to read and handle the avoid command
void write_avoid_command() {
    FILE *file = fopen(FILE_AVOID_COMMAND, "r");
    if (!file) {
        perror("Error opening avoid command file");
        return;
    }

    char command[256];
    while (fgets(command, sizeof(command), file)) {
        command[strcspn(command, "\n")] = 0;  // Remove newline character

        // Handle the command
        if (strcmp(command, "forward") == 0) {
            printf("Moving forward\n");
        }
        else if (strcmp(command, "back") == 0) {
            printf("Moving back\n");
        }
        else if (strcmp(command, "left") == 0) {
            printf("Turning left\n");
        }
        else if (strcmp(command, "right") == 0) {
            printf("Turning right\n");
        }
        else {
            printf("Unknown command: %s\n", command);
        }
    }
    fclose(file);
}

// Function to read pose from the file
void read_pose(float curr_pose[6]) {
    FILE *file = fopen(FILE_POSE, "r");
    if (!file) {
        perror("Error opening pose file");
        return;
    }

    char line[256];
    if (fgets(line, sizeof(line), file)) {
        float t, tx, ty, tz, rx, ry;
        if (sscanf(line, "%f %f %f %f %f %f", &t, &tx, &ty, &tz, &rx, &ry) == 6) {
            curr_pose[0] = t;
            curr_pose[1] = tx;
            curr_pose[2] = ty;
            curr_pose[3] = tz;
            curr_pose[4] = rx;
            curr_pose[5] = ry;
            //printf("Pose: t=%f, tx=%f, ty=%f, tz=%f, rx=%f, ry=%f\n", t, tx, ty, tz, rx, ry);
        }
        else {
            printf("Failed to parse pose data.\n");
        }
    }
    else {
        printf("Failed to read from pose file.\n");
    }
    fclose(file);
}

int main() {

    FPS fps;
    SNN *snn = snn_init("IQIF", 1);

    // Initialize SimpleFlow object
    SimpleFlow* flowObj = init_simple_flow(0.8, 3, 5, NULL, "simple", 0);

    // Initialize the algorithm for image processing
    MyAlgorithm* algorithm = init_algorithm();

    // Initialize and start the FPS timer
    fps_start(&fps);

    // Initialize the collision detection system
    initialize_collision_detection("/home/lingyi/Desktop/cclo_lab/VisionBot-master/eval/pillar_location.txt");

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        keep_going();
        exit(0);
    }
    else if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    for(int i=0; i<1000000; i++) {

        fps_update(&fps);  // Update frame count

        Mat prevFrame, currFrame;

        prevFrame = imread("circle2.jpg");
        currFrame = imread("circle3.jpg");

        if(prevFrame.empty() || currFrame.empty()){
            fprintf(stderr, "Error. Failed to load frames.\n");
            continue;
        }

        float prev_pose[6] = {1.0f};
        float curr_pose[6] = {0.0f};  // Array to store pose data
       
        read_pose(curr_pose);

        if (check_collision(curr_pose)) {
            printf("Collision detected!\n");
            snn_run(snn, 10);
            int active_neuron = snn_getMostActiveNeuron(snn);

            if(active_neuron == 0){
                printf("Moving left to avoid obstacle.\n");
            }
            else if(active_neuron == 1){
                printf("Moving right to avoid obstacle.\n");
            }
            else{
                printf("No action required.\n");
            }
        }
        else {
            printf("No collision detected.\n");
        }

        Mat flow = calculate_optical_flow(flowObj, prevFrame, currFrame, prev_pose, curr_pose);
        Mat enhancedFrame = contrast_enhance(currFrame);

        // Check if 60 frames have passed
        if (fps_isPassed(&fps, 1)) {
            //printf("1 frames have passed.\n");
            fps_reset(&fps);  // Reset the FPS counter after 60 frames
        }

        // Sleep for a short while before reading again
        usleep(100000);  // Poll every second
    }

    cleanup_collision_detection();
    free(algorithm);
    snn_cleanup(snn);

    fps_stop(&fps);
    printf("Elapsed time: %.2f seconds\n", fps_elapsed(&fps));
    printf("Frames per second: %.2f\n", fps_get_fps(&fps));

    return 0;
}