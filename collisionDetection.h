// collisionDetection.h
#ifndef COLLISION_DETECTION_H
#define COLLISION_DETECTION_H

// Function to initialize collision detection with pillar locations from the file
void initialize_collision_detection(const char *pillar_file_path);

// Function to check if there is a collision based on the current pose
int check_collision(float pose[6]);

// Function to clean up resources used by collision detection
void cleanup_collision_detection();

#endif // COLLISION_DETECTION_H
