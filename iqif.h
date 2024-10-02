#ifndef IQIF_H
#define IQIF_H

typedef struct {
    int numNeurons;
    float *potentials;  
    int *spikeCounts;  
} IQIFNetwork;

IQIFNetwork* iqnet_create(const char *neuronParamFile, const char *connectionTableFile);
void network_set_biascurrent(IQIFNetwork *network, int neuronID, float current);
void network_send_synapse(IQIFNetwork *network);
float network_potential(IQIFNetwork *network, int neuronID);
int network_spike_count(IQIFNetwork *network, int neuronID);
void network_cleanup(IQIFNetwork *network);
int network_num_neurons(IQIFNetwork *network);

#endif