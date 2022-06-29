#include "busquedaLocal.h"
#include "individual.h"
#include "operacionesVector.h"
#include "objetivo.h"
#include "random.h"

#include <vector>
#include <algorithm>
#include <chrono>

template<class T>
std::ostream& operator<<(std::ostream & os, const std::vector<T> &V)
{
	  
	  
	  typename std::vector<T>::const_iterator it;
	  
	  for (it=V.begin(); it!=V.end(); ++it) os << (*it) << " | ";

      os<< std::endl << "********************************************"<<std::endl;
	  
	  return os;
}

/*
    Genera una lista de pares que indican:
        -first: índice del elemento de los datos sobre los que se trabaja.
        -second: índice del cluster al que se cambia dicho elemento en ese vecino.
*/
void generarEntornoVirtual(std::vector<std::pair<int,int>> &entornoVirtual, std::vector<int> distribCluster, std::vector<std::vector<int>> clusters, const int &numDatos, const int &numClusters)
{
    
    for(int i=0; i<numClusters; ++i)
    {
        for(int j=0; j<numDatos; ++j)
        {
            if(clusters[distribCluster[j]].size()<2 || distribCluster[j] == i) continue;
            entornoVirtual.emplace_back(j,i);
        }
    }

    random_shuffle(entornoVirtual.begin(), entornoVirtual.end());

}


// void generarVecino(const std::pair<int, int> &vecino, std::vector<std::vector<int>> clusters, std::vector<int> distribCluster, Individual &ind)
// {
//     int indiceValor = vecino.first;
//     int indiceClusterNuevo = vecino.second;
//     int indiceClusterAntiguo = distribCluster[indiceValor];

//     distribCluster[indiceValor] = indiceClusterNuevo;

//     borrarPorValor(clusters[indiceClusterAntiguo], indiceValor);
//     clusters[indiceClusterNuevo].emplace_back(indiceValor);

//     std::pair<std::vector<int>,std::vector<std::vector<int>>> pareja(distribCluster, clusters);
//     ind = Individual(pareja);
// }

Individual busquedaLocal(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, int numClusters, float &LAMBDA)
{
    Individual solucionActual;
    solucionActual.inicializarAleatorio(numClusters, LAMBDA, datos, matrizRest);
   
    //Calculamos primer valor de f
    float valorActual = solucionActual.getFitness();


    /********************* BUSQUEDA LOCAL *********************/
    std::vector<std::pair<int,int>> entornoVirtual;
    std::pair<int,int> vecinoActual;

    bool hayOtroMejor = true;
    int i=0;
    while(i<10000 && hayOtroMejor)
    {
        if(entornoVirtual.empty())
        {
            generarEntornoVirtual(entornoVirtual, solucionActual.getSolucion().first, solucionActual.getSolucion().second, datos.size(), numClusters);
        }

        vecinoActual = entornoVirtual.back();
        entornoVirtual.pop_back();

        //Con generarEntornoVirtual ya aseguramos que el vecino es válido
        Individual vecino = solucionActual.generarVecino(vecinoActual);
        
        //Comparo con la solución actual
        if(vecino < solucionActual)
        {
            solucionActual = vecino;
            entornoVirtual.clear();
        }
        else if(entornoVirtual.empty())
        {
            hayOtroMejor = false;
        }
        
        ++i;
    }
    /*********************************************************/

    return solucionActual;
}

Individual busquedaLocal(Individual &ind)
{

    Individual solucionActual = ind;
   
    //Calculamos primer valor de f
    float valorActual = solucionActual.getFitness();


    /********************* BUSQUEDA LOCAL *********************/
    std::vector<std::pair<int,int>> entornoVirtual;
    std::pair<int,int> vecinoActual;

    bool hayOtroMejor = true;
    int i=0;
    while(i<10000 && hayOtroMejor)
    {
        if(entornoVirtual.empty())
        {
            generarEntornoVirtual(entornoVirtual, solucionActual.getSolucion().first, solucionActual.getSolucion().second, Individual::datos.size(), Individual::numClusters);
        }

        vecinoActual = entornoVirtual.back();
        entornoVirtual.pop_back();

        //Con generarEntornoVirtual ya aseguramos que el vecino es válido
        Individual vecino = solucionActual.generarVecino(vecinoActual);
        // generarVecino(vecinoActual, solucionActual.getSolucion().second, solucionActual.getSolucion().first, vecino);
        
        //Comparo con la solución actual
        if(vecino < solucionActual)
        {
            solucionActual = vecino;
            entornoVirtual.clear();
        }
        else if(entornoVirtual.empty())
        {
            hayOtroMejor = false;
        }
        
        ++i;
    }
    /*********************************************************/

    return solucionActual;
}