#ifndef __BUSQUEDALOCAL_PPIO_H
#define __BUSQUEDALOCAL_PPIO_H

#include <vector>

void generarEntornoVirtual(std::vector<std::pair<int,int>> &entornoVirtual, const int &numDatos, const int &numClusters);
void generarEntornoVirtual(std::vector<std::pair<int,int>> &entornoVirtual, std::vector<int> &distribCluster, std::vector<std::vector<int>> &clusters, const int &numDatos, const int &numClusters);

/*
    Genera el vecino asociado al argumento vecino, guardando
    el resultado en clustersVecino y distribClusterVecino.
*/
void generarVecino(const std::pair<int, int> &vecino, std::vector<std::vector<int>> &clustersVecino, std::vector<int> &distribClusterVecino);
void busquedaLocal(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, int numClusters, std::vector<int> &distribCluster, std::vector<std::vector<int>> &clusters, std::vector<std::vector<float>> &centroides, const float &LAMBDA);

#endif