#ifndef SNN_H
#define SNN_H

typedef struct {
    void *network;
    int numNeurons;
} SNN;

SNN* snn_init(const char *model_type, int thread);
void snn_stimulate(SNN *snn, int neuronID, float current);
void snn_run(SNN *snn, int steps);
int snn_getMostActiveNeuron(SNN *snn);
float snn_getPotential(SNN *snn, int neuronID);
int snn_getSpikeCount(SNN *snn, int neuronID);
void snn_cleanup(SNN *snn);

#endif