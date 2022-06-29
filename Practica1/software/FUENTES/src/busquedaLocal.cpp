#include "busquedaLocal.h"
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
void generarEntornoVirtual(std::vector<std::pair<int,int>> &entornoVirtual, const int &numDatos, const int &numClusters)
{
    
    for(int i=0; i<numClusters; ++i)
    {
        for(int j=0; j<numDatos; ++j)
        {
            entornoVirtual.emplace_back(j,i);
        }
    }

    random_shuffle(entornoVirtual.begin(), entornoVirtual.end());

}

void generarEntornoVirtual(std::vector<std::pair<int,int>> &entornoVirtual, std::vector<int> &distribCluster, std::vector<std::vector<int>> &clusters, const int &numDatos, const int &numClusters)
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

/*
    Genera el vecino asociado al argumento vecino, guardando
    el resultado en clustersVecino y distribClusterVecino.
*/
void generarVecino(const std::pair<int, int> &vecino, std::vector<std::vector<int>> &clustersVecino, std::vector<int> &distribClusterVecino)
{
    int indiceValor = vecino.first;
    int indiceClusterNuevo = vecino.second;
    int indiceClusterAntiguo = distribClusterVecino[indiceValor];

    // if(clustersVecino[indiceClusterAntiguo].size() > 1)
    // {
        distribClusterVecino[indiceValor] = indiceClusterNuevo;

        borrarPorValor(clustersVecino[indiceClusterAntiguo], indiceValor);
        clustersVecino[indiceClusterNuevo].emplace_back(indiceValor);

    //     return true;
    // }
    // else return false;

}

void busquedaLocal(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, int numClusters, std::vector<int> &distribCluster, std::vector<std::vector<int>> &clusters, std::vector<std::vector<float>> &centroides, const float &LAMBDA)
{
    
    std::chrono::high_resolution_clock::time_point inicial = std::chrono::high_resolution_clock::now();

    clusters.clear();
    distribCluster = std::vector<int>(datos.size());

    //Generamos solución aleatoria
    for(int i =0; i<numClusters; ++i) clusters.emplace_back();

    int numAleatorio, limite = numClusters-1;
    int contadores[numClusters] = { 0 };
    bool repetir;
    
    do
    {
        
        repetir = false;
        for(int i=0; i<numClusters; ++i) clusters[i].clear();

        

        for(int i=0;i<datos.size();++i) 
        {
            numAleatorio = Randint(0,limite);
            distribCluster[i] = numAleatorio;
            clusters[numAleatorio].emplace_back(i);
            
            ++contadores[numAleatorio];
        }
        
        for(int i=0; i<numClusters; ++i)
        {
            if(contadores[i] == 0)
            {
                repetir = true;
                break;
            }
        }


    }while(repetir);
   


    //Calculamos los centroides asociados a la solución inicial
    vectorCentroides(datos, clusters, centroides);
   
    
    //Calculamos primer valor de f
    float valorActual = funcionObjetivo(datos,matrizRest,clusters,distribCluster, centroides, LAMBDA);
    
    
    // std::cout << "***************** INICIAL BL *****************" << std::endl;
    // // std::cout << "DistribCluster: " << distribCluster << std::endl;
    // // std::cout << "Clusters: " << clusters << std::endl;
    // std::cout << "f: " << valorActual << std::endl;
    // std::cout << "RestriccionesVioladas: " << infeasibility(matrizRest, distribCluster) << std::endl;
    // std::cout << "C: " << C(datos, clusters, centroides) << std::endl;
    // // std::cout << "LAMBDA: " << LAMBDA << std::endl;
    // std::cout << "********************************************\n" << std::endl;


    /********************* BUSQUEDA LOCAL *********************/

    std::vector<std::vector<int>> clustersVecino;
    std::vector<int> distribClusterVecino;
    
    std::vector<std::pair<int,int>> entornoVirtual;
    std::pair<int,int> vecinoActual;
    
    std::vector<std::vector<float>> centroidesVecino;

    bool hayOtroMejor = true;
    
    
    int i=0;
    while(i<100000 && hayOtroMejor)
    {
        
        
        if(entornoVirtual.empty())
        {
            generarEntornoVirtual(entornoVirtual, distribCluster, clusters, datos.size(), numClusters);
        }

        vecinoActual = entornoVirtual.back();
        entornoVirtual.pop_back();

        clustersVecino = clusters;
        distribClusterVecino = distribCluster;

        //Con generarEntornoVirtual ya aseguramos que el vecino es válido
        generarVecino(vecinoActual, clustersVecino, distribClusterVecino);
        
        //Generar nuevos centroides
        vectorCentroides(datos, clustersVecino, centroidesVecino);

        //Evaluo
        float fAux = funcionObjetivo(datos, matrizRest, clustersVecino, distribClusterVecino, centroidesVecino, LAMBDA);
        
        //Comparo con la solución actual
        if(fAux < valorActual)
        {
            valorActual = fAux;
            distribCluster = distribClusterVecino;
            clusters = clustersVecino;
            centroides = centroidesVecino;
            entornoVirtual.clear();
        }
        else if(entornoVirtual.empty())
        {
            hayOtroMejor = false;
        }
        
        ++i;
    }
    /*********************************************************/

    std::chrono::high_resolution_clock::time_point final = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duracion = std::chrono::duration_cast<std::chrono::duration<double>>(final-inicial);

    std::cout << "***************** FINAL BL *****************" << std::endl;
    // std::cout << "DistribCluster: " << distribCluster << std::endl;
    // std::cout << "Clusters: " << clusters << std::endl;
    std::cout << "NumIteraciones: " << i << std::endl;
    std::cout << "Tasa_C: " << C(datos, clusters, centroides) << std::endl;
    std::cout << "Tasa_inf: " << infeasibility(matrizRest, distribCluster) << std::endl;
    std::cout << "f: " << funcionObjetivo(datos, matrizRest, clusters, distribCluster, centroides, LAMBDA) << std::endl;
    
    // std::cout << "LAMBDA: " << LAMBDA << std::endl;
    std::cout << "Duracion: " << duracion.count() << " segundos" << std::endl;
    std::cout << "********************************************" << std::endl;
    std::cout << "********************************************\n" << std::endl;

}