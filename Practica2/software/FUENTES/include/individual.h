#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <string>
#include "objetivo.h"

// Class representing individual in population 
class Individual 
{ 
private:
    std::pair<std::vector<int>,std::vector<std::vector<int>>> chromosome; 
    float fitness;
public:
    

    //Guardamos indices de cluster por valor, y clusters por indices de valores
    static int numClusters;
    static float LAMBDA;
    static std::vector<std::vector<int>> matrizRest;
    static std::vector<std::vector<float>> datos;
    
    Individual();
    Individual(std::vector<int> &chromosome, std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, int &numClusters, int populationSize, float &LAMBDA);
    Individual(std::vector<int> &chromosome); 
    Individual(std::pair<std::vector<int>,std::vector<std::vector<int>>> &chromosome); 

    float getFitness() const;
    std::pair<std::vector<int>,std::vector<std::vector<int>>> getChromosome();

    
    Individual mateUN(Individual parent2);
    Individual mateSF(Individual parent2);
    bool mutate(int &numGen);
    bool repair();

    void operator=(const Individual &ind);
    bool operator==(const Individual &ind);
    bool operator!=(const Individual &ind);
    bool operator<(const Individual &ind);
    bool operator>=(const Individual &ind);
    bool operator>(const Individual &ind);
};


 
#endif