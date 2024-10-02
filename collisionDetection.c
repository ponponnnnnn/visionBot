#include "collisionDetection.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SAFETY_MARGIN 0.2
#define PILLAR_RADIUS (1.0 / sqrt(2))  // Corrected to use floating-point division

// Static variables to store pillar locations
static float *pillars = NULL;
static int pillar_count = 0;

// Function to initialize collision detection with pillar file
void initialize_collision_detection(const char *pillar_file_path) {
    FILE *file = fopen(pillar_file_path, "r");
    if (!file) {
        perror("Error opening pillar file");
        return;
    }

    // Count the number of pillars in the file
    int count = 0;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        count++;
    }
    rewind(file);

    // Allocate memory for pillar coordinates
    pillars = (float *)malloc(count * 2 * sizeof(float));
    if (pillars == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return;
    }
    pillar_count = count;

    // Read pillar coordinates from the file
    int i = 0;
    while (fscanf(file, "%f %f", &pillars[i * 2], &pillars[i * 2 + 1]) == 2) {
        // Add offset to coordinates as in the Python code
        pillars[i * 2] += 0.5;
        pillars[i * 2 + 1] += 0.5;
        i++;
    }

    fclose(file);
}

int check_collision(float pose[6]) {
    if (pillars == NULL) {
        fprintf(stderr, "Pillar data not initialized\n");
        return 0;
    }

    float tx = pose[1]; // Current X position
    float tz = pose[3]; // Current Z position

    // Check collision with each pillar
    for (int i = 0; i < pillar_count; i++) {
        float pillar_tx = pillars[i * 2];
        float pillar_tz = pillars[i * 2 + 1];
        float distance = sqrt((pillar_tx - tx) * (pillar_tx - tx) + (pillar_tz - tz) * (pillar_tz - tz));

        // Print the current pose, pillar position, and the distance between them
        //printf("Pose: tx=%.2f, tz=%.2f, Pillar: tx=%.2f, tz=%.2f, Distance=%.2f\n",tx, tz, pillar_tx, pillar_tz, distance);

        if (distance < (SAFETY_MARGIN + PILLAR_RADIUS)) {
            return 1;  // Collision detected
        }
    }
    return 0;  // No collision detected
}

// Function to clean up resources
void cleanup_collision_detection() {
    free(pillars);
    pillars = NULL;
    pillar_count = 0;
}
