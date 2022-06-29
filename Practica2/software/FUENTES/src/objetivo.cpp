#include "random.h"
#include "objetivo.h"
#include "operacionesVector.h"

#include <numeric>
#include <cmath>
#include <algorithm>
#include <functional>

/*
    Calcula Lambda ajustado a la mayor distancia entre dos datos y el nº de restricciones.
*/
float calcularLambda(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> matrizRest)
{
    float maxDistancia;
    float aux;
    std::vector<float> resta;
    float numRest = 0;

    for(int i=0; i< datos.size(); ++i)
    {

        for(int j=i+1; j< datos.size(); ++j)
        {
            if(abs(matrizRest[i][j]) == 1 && i!=j) ++numRest;

            restaVector(datos[i], datos[j], resta);
            aux = moduloVector(resta);
            if(aux > maxDistancia)
            {
                maxDistancia = aux;
            }
        }
    }
    
    return maxDistancia/numRest;
}

/*
    Esto te coge un todos los componentes de un cluster y 
    te calcula el valor medio (el centroide).
*/
std::vector<float> calcularCentroide(std::vector<std::vector<float>> &datos, std::vector<int> &cluster)
{

    std::vector<float> centroide;
    int numElem = cluster.size(); //Numero de datos en el cluster
    int numCaract = datos[0].size(); //Todos los datos tienen el mismo nº de caracteristicas.
    float aux;
    
    for(int i=0; i<numCaract; ++i)
    {
        aux=0;
        for(int j=0; j<numElem;++j) aux += datos[cluster[j]][i]; 
        centroide.emplace_back(aux/numElem);
    }
    
    
    return centroide;
}

/*
    Calcula los centroides de cada cluster en clusters y los guarda en el vector de vector centroides.
*/
void vectorCentroides(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &clusters, std::vector<std::vector<float>> &centroides)
{
    centroides.clear();

    std::vector<std::vector<int>>::iterator it;
    for(it = clusters.begin(); it != clusters.end(); ++it)
    {
        centroides.emplace_back(calcularCentroide(datos, (*it)));
    }

    //std::cout << "TAMANO CENTROIDES: " << centroides.size() << std::endl; //DEBUG
}


/*
    Esto calcula la distancia media intra-cluster de un cluster.
    Es decir, le das los componentes de un cluster y un centroide
    y te calcula la media de las diferencias entre cada componente
    y el centroide.
*/
float Ci(std::vector<std::vector<float>> &datos, std::vector<int> &cluster, std::vector<float> &centroide)
{
    

    int numElementosCluster = cluster.size();
    float distanciaTotal = 0;
    
    std::vector<float> resta;
    
    for(int i=0; i<numElementosCluster; ++i)
    {
        std::vector<float> fila = datos[cluster[i]];
        restaVector(fila,centroide, resta);
        
        distanciaTotal += moduloVector(resta);
    }

    return distanciaTotal/numElementosCluster;
    
}

/*
    Esto calcula la media de los Ci's (Desviación general de la solución).
*/
float C(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &solucion, std::vector<std::vector<float>> &centroides)
{
    int numClusters = solucion.size();
    float total = 0;

    for(int i=0; i<numClusters; ++i)
    {
        float aux = Ci(datos, solucion[i], centroides[i]);
        total+=aux;
    }
    
    return total/numClusters;
}

float C(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &solucion)
{
    std::vector<std::vector<float>> centroides;
    vectorCentroides(datos, solucion, centroides);
    return C(datos, solucion, centroides);
}

/*
    Esta función mira si dos datos violan una restricción, es decir,
    si tienen ML (Must-Link) y no están en el mismo cluster, o tienen
    CL (Cannot-Link) y están en el mismo cluster.
    Si hay restricción, devuelve 1, y si no, devuelve 0.
*/
int violaRestriccion(int &i, int &j, std::vector<std::vector<int>> &matrizRest, std::vector<int> &distribCluster)
{
    int num = 0;
    
    if ( (matrizRest[i][j] == -1 && distribCluster[i] == distribCluster[j]) || (matrizRest[i][j] == 1 && distribCluster[i] != distribCluster[j] && i!=j) ) 
        num = 1;
    
    return num;
}

/*
    Esta función mira si todas las restricciones que se incumplen para
    la solución C.
*/
int infeasibility(std::vector<std::vector<int>> &matrizRest, std::vector<int> &distribCluster)
{
    
    int resultado = 0;

    for(int i=0; i<matrizRest.size();++i){
        for(int j=i+1; j<matrizRest[0].size();++j){
            resultado += violaRestriccion(i,j, matrizRest, distribCluster);
        }
    }
    
    return resultado;
}

template<class T>
std::ostream& operator<<(std::ostream & os, const std::vector<T> &V)
{
	  
	  
	  typename std::vector<T>::const_iterator it;
	  
	  for (it=V.begin(); it!=V.end(); ++it) os << (*it) << " | ";

      os<< std::endl << "********************************************"<<std::endl;
	  
	  return os;
}

/*
    Calcula la bondad de la solución 
*/
float funcionObjetivo(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, std::vector<std::vector<int>> &clusters, std::vector<int> &distribCluster, std::vector<std::vector<float>> &centroides, float &LAMBDA)
{
    return ( C(datos,clusters, centroides) + (infeasibility(matrizRest, distribCluster)*LAMBDA) );
}

float funcionObjetivo(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, std::vector<std::vector<int>> &clusters, std::vector<int> &distribCluster, float &LAMBDA)
{   
    // std::cout << "Datos: " << datos << "\n";
    // std::cout << "MatrizRest: " << matrizRest << "\n";
    // std::cout << "Clusters: " << clusters << "\n";
    // std::cout << "DistribCluster: " << distribCluster << "\n";
    // std::cout << "LAMBDA: " << LAMBDA << "\n";

    

    std::vector<std::vector<float>> centroides;
    vectorCentroides(datos, clusters, centroides);

    // std::cout << "C: " << C(datos,clusters, centroides) << "\n";
    // std::cout << "INFEASIBILITY: " << infeasibility(matrizRest, distribCluster) << "\n";
    // std::cout << "LAMBDA: " << LAMBDA << "\n";
    // std::cout << "INFEASIBILITY * LAMBDA: " << infeasibility(matrizRest, distribCluster) * LAMBDA << "\n";

    return ( C(datos,clusters, centroides) + (infeasibility(matrizRest, distribCluster)*LAMBDA) );
}

