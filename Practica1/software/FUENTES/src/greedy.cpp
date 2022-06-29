#include "operacionesVector.h"
#include "objetivo.h"
#include "random.h"

#include <set>
#include <algorithm> //random_shuffle
#include <limits.h> //INT_MAX
#include <chrono>

template<class T>
std::ostream& operator<<(std::ostream & os, const std::vector<T> &V)
{
	  
	  
	  typename std::vector<T>::const_iterator it;
	  
	  for (it=V.begin(); it!=V.end(); ++it) os << (*it) << " | ";

      os<< std::endl << "********************************************"<<std::endl;
	  
	  return os;
}


int infeasibilityLocal(int &dato, int &cluster, std::vector<std::vector<int>> &matrizRest, std::vector<int> distribCluster)
{
    int result = 0;

    for(int i=0; i<matrizRest[dato].size();++i)
    {
        if(matrizRest[dato][i] == 1 && cluster != distribCluster[i] && i != dato) ++result;
        else if(matrizRest[dato][i] == -1 && cluster == distribCluster[i]) ++result;
    }

    return result;
}

/******************************************************************************************************************        
    Greedy Transparencias.
*/
void COPKM(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, int &numClusters, std::vector<int> &distribCluster, std::vector<std::vector<int>> &clusters, std::vector<std::vector<float>> &centroides, const float &LAMBDA)
{
    std::chrono::high_resolution_clock::time_point inicial = std::chrono::high_resolution_clock::now();
    std::vector<int> rsi;
    std::vector<int> distribClusterAnterior;

    distribCluster = std::vector<int>(datos.size(), -1);

    clusters.clear();
    for(int i=0; i<numClusters; ++i) clusters.emplace_back();

    std::vector<float> maximos, minimos;
    for(int i = 0; i < datos[0].size(); ++i)
    {
        std::vector<std::vector<float>>::iterator it;
        maximos.push_back(datos[0][i]);
        minimos.push_back(datos[0][i]);
        for(it = datos.begin()+1; it != datos.end(); ++it)
        {
            if((*it)[i] > maximos[i]) maximos[i] = (*it)[i];
            else if((*it)[i] < minimos[i]) minimos[i] = (*it)[i];
        }
    }
       

    centroides = std::vector<std::vector<float>>(numClusters);
    for(int i=0; i<numClusters; ++i)
    {
        //Generamos centroides aleatorios, guardamos en centroides
        for(int j=0; j<datos[0].size();++j) 
            centroides[i].push_back(Randfloat(minimos[j],maximos[j]));
     
    }

    for(int i=0; i<datos.size(); ++i) rsi.emplace_back(i);
    random_shuffle(rsi.begin(), rsi.end()); //Barajamos los indices de las instancias

    do
    {
        distribClusterAnterior = distribCluster;
        for(int i=0; i<numClusters; ++i) clusters[i].clear();

        for(int i=0; i<datos.size(); ++i)
        {
            std::vector<float> dato = datos[rsi[i]];
            std::vector<float> resta;
            int clusterAsignado;
            int minimoRestricciones = INT_MAX;

            for(int j = 0; j<numClusters; ++j)
            {
                int restriccionesVioladas = infeasibilityLocal(rsi[i], j, matrizRest, distribCluster);

                if(restriccionesVioladas < minimoRestricciones)
                {
                    minimoRestricciones = restriccionesVioladas;
                    clusterAsignado = j;
                }
                else if(restriccionesVioladas == minimoRestricciones)
                {
                    std::vector<float> resta1,resta2;
                    restaVector(dato, centroides[clusterAsignado], resta1);
                    restaVector(dato, centroides[j], resta2);

                    if(moduloVector(resta1) > moduloVector(resta2))
                    {
                        clusterAsignado = j;
                    }
                }
            }

            distribCluster[rsi[i]] = clusterAsignado;
            clusters[clusterAsignado].push_back(rsi[i]);
        }

        vectorCentroides(datos, clusters, centroides);

    } while (distribClusterAnterior != distribCluster);
    


    std::chrono::high_resolution_clock::time_point final = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duracion = std::chrono::duration_cast<std::chrono::duration<double>>(final-inicial);
    std::cout << "***************** FINAL GREEDY *****************" << std::endl;
    std::cout << "Tasa_C: " << C(datos, clusters, centroides) << std::endl;
    std::cout << "Tasa_inf: " << infeasibility(matrizRest, distribCluster) << std::endl;
    std::cout << "f: " << funcionObjetivo(datos, matrizRest, clusters, distribCluster, centroides, LAMBDA) << std::endl;
    std::cout << "Duracion: " << duracion.count() << " segundos" << std::endl;
    std::cout << "************************************************" << std::endl;
    std::cout << "************************************************\n" << std::endl;
}