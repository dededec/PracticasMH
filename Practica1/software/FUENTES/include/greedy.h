#ifndef __GREEDY_PPIO_H
#define __GREEDY_PPIO_H

#include <vector>

int infeasibilityLocal(int &dato, int &cluster, std::vector<std::vector<int>> &matrizRest, std::vector<int> distribCluster);
// void COPKM_Set(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, int &numClusters, std::vector<int> &distribCluster, std::vector<std::vector<int>> &clusters, std::vector<std::vector<float>> &centroides, const float &LAMBDA);
void COPKM(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, int &numClusters, std::vector<int> &distribCluster, std::vector<std::vector<int>> &clusters, std::vector<std::vector<float>> &centroides, const float &LAMBDA);
#endif