#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <string>
#include "objetivo.h"

// Clase que representa una solución
class Individual 
{ 
private:
    std::pair<std::vector<int>,std::vector<std::vector<int>>> solucion;
    std::vector<std::vector<float>> centroides; 
    float fitness;
public:
    
    // VARIABLES DE CLASE
    static int numClusters;
    static float LAMBDA;
    static std::vector<std::vector<int>> matrizRest;
    static std::vector<std::vector<float>> datos;
    static void setVariablesClase(int &numClusters, float &LAMBDA, std::vector<std::vector<float>> &datos,  std::vector<std::vector<int>> &matrizRest);
    
    // CONSTRUCTORES
    Individual();
    Individual(std::vector<int> &solucion, std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, int &numClusters, float &LAMBDA);
    Individual(std::vector<int> &solucion); 
    Individual(std::pair<std::vector<int>,std::vector<std::vector<int>>> &solucion); 

    void inicializarAleatorio(int &numClusters, float &LAMBDA, std::vector<std::vector<float>> &datos,  std::vector<std::vector<int>> &matrizRest);
    void inicializarAleatorio();


    // ACCESOS A MIEMBROS
    float getFitness() const;
    std::pair<std::vector<int>,std::vector<std::vector<int>>> getSolucion();
    std::vector<std::vector<float>> getCentroides();
    

    
    // GENETICO
    Individual mateUN(Individual parent2);
    Individual mateSF(Individual parent2);
    bool mutate(int &numGen);
    bool repair();

    Individual generarVecino();
    Individual generarVecino(std::pair<int,int> &pareja);

    // OPERACIONES BOOLEANAS
    void operator=(const Individual &ind);
    bool operator==(const Individual &ind);
    bool operator!=(const Individual &ind);
    bool operator<(const Individual &ind);
    bool operator>=(const Individual &ind);
    bool operator>(const Individual &ind);
};


 
#endif