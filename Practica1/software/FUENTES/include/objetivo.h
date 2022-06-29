#ifndef __OBJETIVO_PPIO_H
#define __OBJETIVO_PPIO_H

#include <vector>

float calcularLambda(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> matrizRest);
std::vector<float> calcularCentroide(std::vector<std::vector<float>> &datos, std::vector<int> &cluster);
void vectorCentroides(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &clusters, std::vector<std::vector<float>> &centroides);
float Ci(std::vector<std::vector<float>> &datos, std::vector<int> &cluster, std::vector<float> &centroide);
float C(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &solucion, std::vector<std::vector<float>> &centroides);
int violaRestriccion(int &i, int &j, std::vector<std::vector<int>> &matrizRest, std::vector<int> &distribCluster);
int infeasibility(std::vector<std::vector<int>> &matrizRest, std::vector<int> &distribCluster);
float funcionObjetivo(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, std::vector<std::vector<int>> &clusters, std::vector<int> &distribCluster, std::vector<std::vector<float>> &centroides, const float &LAMBDA);

float funcionObjetivo(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, std::vector<std::vector<int>> &clusters, std::vector<int> &distribCluster, const float &LAMBDA);

#endif