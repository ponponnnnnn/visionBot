#include "iqif.h"
#include <stdio.h>
#include <stdlib.h>

IQIFNetwork* iqnet_create(const char *neuronParamFile, const char *connectionTableFile) { 
    IQIFNetwork *network = (IQIFNetwork *)malloc(sizeof(IQIFNetwork)); 
    network->numNeurons = 100; // 假設有 100 個神經元 
    network->potentials = (float *)malloc(sizeof(float) * network->numNeurons); 
    network->spikeCounts = (int *)malloc(sizeof(int) * network->numNeurons); 
    return network;
}  

void network_set_biascurrent(IQIFNetwork *network, int neuronID, float current) {
    if (neuronID >= 0 && neuronID < network->numNeurons) {
        // Placeholder: Adjust neuron potential by current
        network->potentials[neuronID] += current;
        printf("Set bias current for neuron %d to %f\n", neuronID, current);
    }
}

void network_send_synapse(IQIFNetwork *network) {
    for (int i = 0; i < network->numNeurons; ++i) {
        // Placeholder: Increment potential slightly for every step
        network->potentials[i] += 0.01f;
        if (network->potentials[i] > 1.0f) {
            network->spikeCounts[i] += 1;
            network->potentials[i] = 0.0f;  // Reset potential after spike
        }
    }
}

float network_potential(IQIFNetwork *network, int neuronID) {
    if (neuronID >= 0 && neuronID < network->numNeurons) {
        return network->potentials[neuronID];
    }
    return -1.0f;  // Return invalid potential if out of bounds
}

int network_spike_count(IQIFNetwork *network, int neuronID) {
    if (neuronID >= 0 && neuronID < network->numNeurons) {
        return network->spikeCounts[neuronID];
    }
    return -1;  // Return invalid count if out of bounds
}

void network_cleanup(IQIFNetwork *network) {
    free(network->potentials);
    free(network->spikeCounts);
    free(network);
}

int network_num_neurons(IQIFNetwork *network) { 
    return network->numNeurons; 
}  