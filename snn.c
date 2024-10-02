#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "snn.h"
#include "iqif.h"

SNN* snn_init(const char* model, int numNeurons) {
    SNN *snn = (SNN*) malloc(sizeof(SNN));
    snn->network = iqnet_create("iq-neuron/inputs/neuronParameter_IQIF.txt", "iq-neuron/inputs/Connection_Table_IQIF.txt"); // Assuming this function returns a void*
    
    // Cast the network to IQIFNetwork* where required
    snn->numNeurons = network_num_neurons((IQIFNetwork*)snn->network);
    
    return snn;
}

void snn_stimulate(SNN* snn, int neuronID, float current) {
    // Cast the network to IQIFNetwork*
    network_set_biascurrent((IQIFNetwork*)snn->network, neuronID, current);
}

void snn_run(SNN* snn, int numSteps) {
    for (int i = 0; i < numSteps; ++i) {
        // Cast the network to IQIFNetwork*
        network_send_synapse((IQIFNetwork*)snn->network);
    }
}

int snn_getMostActiveNeuron(SNN* snn) {
    // Cast malloc to float*
    float *activity = (float*) malloc(snn->numNeurons * sizeof(float));
    int mostActiveNeuron = 0;
    float maxActivity = -1.0;

    for (int i = 0; i < snn->numNeurons; i++) {
        activity[i] = network_spike_count((IQIFNetwork*)snn->network, i);
        if (activity[i] > maxActivity) {
            maxActivity = activity[i];
            mostActiveNeuron = i;
        }
    }
    free(activity);  // Don't forget to free allocated memory
    return mostActiveNeuron;
}


float snn_getPotential(SNN* snn, int neuronID) {
    // Cast the network to IQIFNetwork*
    return network_potential((IQIFNetwork*)snn->network, neuronID);
}

int snn_getSpikeCount(SNN* snn, int neuronID) {
    // Cast the network to IQIFNetwork*
    return network_spike_count((IQIFNetwork*)snn->network, neuronID);
}

void snn_cleanup(SNN* snn) {
    // Cast the network to IQIFNetwork*
    network_cleanup((IQIFNetwork*)snn->network);
    free(snn);
}
