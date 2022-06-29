#ifndef ES_H
#define ES_H

#include "individual.h"
#include <vector>

bool metrop (float de, float t);
Individual enfriamientoSimulado(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, 
        int &numClusters, float &LAMBDA);
Individual BMB(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, 
        int &numClusters, float &LAMBDA);
Individual ILS(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, 
        int &numClusters, float &LAMBDA);
Individual ILS_ES(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, 
        int &numClusters, float &LAMBDA);

#endif