#ifndef GENETICO_H
#define GENETICO_H

#include "individual.h"
#include <vector>

const int POPULATION_SIZE_AG = 50;
const int POPULATION_SIZE_AM = 10;

std::vector<int> create_genome(int numDatos, int numClusters);

Individual selection(Individual &i, Individual &j);
int selection(int &i, int &j, std::vector<Individual> &population);

Individual AGG(Individual(Individual::*fp)(Individual), std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, int &numClusters, float &LAMBDA);
Individual AGE(Individual(Individual::*fp)(Individual), std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, int &numClusters, float &LAMBDA);

void BLS(Individual &S, int numFallos);
Individual AM_1(float porcentajeBLS, int intervaloBLS, Individual(Individual::*fp)(Individual), std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, int &numClusters, float &LAMBDA);
Individual AM_2(float porBLS, int interBLS, Individual(Individual::*fp)(Individual), std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, int &numClusters, float &LAMBDA);

#endif