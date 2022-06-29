#ifndef NBA_H
#define NBA_H

#include "individual.h"
#include "busquedaLocal.h"


Individual NBADraft(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, 
        int &numClusters, float &LAMBDA, int numEquipos, int numTemporadas, int numPartidos);
        
Individual NBADraftBL(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, 
        int &numClusters, float &LAMBDA, int numEquipos, int numTemporadas, int numPartidos);

#endif