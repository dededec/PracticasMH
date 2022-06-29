#include "individual.h"
#include "objetivo.h"
#include "random.h"
#include "operacionesVector.h"
#include <vector>
#include <iostream>


template<class T>
std::ostream& operator<<(std::ostream & os, const std::vector<T> &V)
{
	  
	  
	  typename std::vector<T>::const_iterator it;
	  
	  for (it=V.begin(); it!=V.end(); ++it) os << (*it) << " | ";

      os<< std::endl << "********************************************"<<std::endl;
	  
	  return os;
}


template<class T, class T2>
std::ostream& operator<<(std::ostream & os, const std::pair<T, T2> &P)
{
	  os << P.first << std::endl << P.second << std::endl;
	  return os;
}


//Inicializo valores estáticos
int Individual::numClusters = 0;
float Individual::LAMBDA = 0.0;
std::vector<std::vector<int>> Individual::matrizRest = std::vector<std::vector<int>>();
std::vector<std::vector<float>> Individual::datos = std::vector<std::vector<float>>();

static void setVariablesClase(int &numClusters, float &LAMBDA, std::vector<std::vector<float>> &datos,  std::vector<std::vector<int>> &matrizRest)
{
    Individual::numClusters = numClusters;
    Individual::LAMBDA = LAMBDA;
    Individual::datos = datos;
    Individual::matrizRest = matrizRest;
}


Individual::Individual()
{
    this->solucion.first = std::vector<int>();
    this->solucion.second = std::vector<std::vector<int>>();
    this->centroides = std::vector<std::vector<float>>();
}

Individual::Individual(std::vector<int> &solucion, std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, int &numClusters, float &LAMBDA) 
{ 
    //Establecemos variables de clase:
    Individual::numClusters = numClusters;
    Individual::datos = datos;
    Individual::matrizRest = matrizRest;
    Individual::LAMBDA = LAMBDA;

    this->solucion.first = solucion;
    std::vector<std::vector<int>> clusters(Individual::numClusters);
    for(int i = 0; i < solucion.size();++i)
    {
        clusters[solucion[i]].emplace_back(i);
    } 
    this->solucion.second = clusters;

    vectorCentroides(Individual::datos, this->solucion.second, this->centroides);

    this->fitness = funcionObjetivo(datos, matrizRest, clusters, solucion, this->centroides, LAMBDA);  
}

Individual::Individual(std::vector<int> &solucion)
{
    this->solucion.first = solucion;
    std::vector<std::vector<int>> clusters(Individual::numClusters);
    for(int i = 0; i < solucion.size();++i)
    {
        clusters[solucion[i]].emplace_back(i);
    } 
    this->solucion.second = clusters;
    vectorCentroides(Individual::datos, this->solucion.second, this->centroides);
    this->fitness = funcionObjetivo(Individual::datos, Individual::matrizRest, clusters, solucion, this->centroides, Individual::LAMBDA);
}

Individual::Individual(std::pair<std::vector<int>,std::vector<std::vector<int>>> &solucion)
{
    this->solucion = solucion;
    vectorCentroides(Individual::datos, this->solucion.second, this->centroides);
    this->fitness = funcionObjetivo(Individual::datos, Individual::matrizRest, solucion.second, solucion.first, this->centroides, Individual::LAMBDA);
}


void Individual::inicializarAleatorio(int &numClusters, float &LAMBDA, std::vector<std::vector<float>> &datos,  std::vector<std::vector<int>> &matrizRest)
{
    Individual::numClusters = numClusters;
    Individual::datos = datos;
    Individual::matrizRest = matrizRest;
    Individual::LAMBDA = LAMBDA;
    
    int numAleatorio, numDatos = Individual::datos.size(), limite = Individual::numClusters-1;
    
    bool repetir;

    do
    {
        int contadores[numClusters] = { 0 };
        if(!this->solucion.first.empty())
        {
            this->solucion.first.clear();
            this->solucion.second.clear();
        }

        repetir = false;
        for(int i=0;i<numDatos;++i) 
        {
            int numAleatorio = Randint(0, limite);
            this->solucion.first.emplace_back(numAleatorio);
            
            ++contadores[numAleatorio];
        }
        
        for(int i=0; i< Individual::numClusters; ++i)
        {
            if(contadores[i] == 0)
            {
                repetir = true;
                break;
            }
        }

    }while(repetir);

    this->solucion.second = std::vector<std::vector<int>>(Individual::numClusters);
    for(int i = 0; i < numDatos;++i)
    {
        this->solucion.second[this->solucion.first[i]].emplace_back(i);
    }

    vectorCentroides(Individual::datos, this->solucion.second, this->centroides);
    this->fitness = funcionObjetivo(Individual::datos, Individual::matrizRest, solucion.second, solucion.first, this->centroides, Individual::LAMBDA);
}

float Individual::getFitness() const
{
    return this->fitness;
}

std::pair<std::vector<int>,std::vector<std::vector<int>>> Individual::getSolucion()
{
    return this->solucion;
}

std::vector<std::vector<float>> Individual::getCentroides()
{
    return this->centroides;
}


  
// Cruce uniforme
Individual Individual::mateUN(Individual par2) 
{ 
    int len = this->solucion.first.size(); 
    std::vector<int> child_solucion(len,-1);

    for(int i = 0;i<len/2;i++) 
    { 
        int random = Randint(0, len-1);
        child_solucion[random] = this->solucion.first[random];
    }

    for(int i=0; i<len; ++i)
    {
        if(child_solucion[i] == -1)
        {
            child_solucion[i] = par2.solucion.first[i];
        }
    } 

    return Individual(child_solucion); 
}

// Cruce por segmento fijo
Individual Individual::mateSF(Individual par2)
{
    int len = this->solucion.first.size();

    int inicio = Randint(0, len-1);
    int longitud = Randint(0, len-1);
    std::vector<int> child_solucion(len,-1);

    int i=inicio, iter=0;
    while(iter < longitud)
    {
        child_solucion[i] = this->solucion.first[i];
        i = (i+1) % len;
        ++iter;
    }
    for(int i=0; i<len; ++i)
    {
        if(child_solucion[i] == -1)
        {
            child_solucion[i] = par2.solucion.first[i];
        }
    } 
    return Individual(child_solucion);
}


bool Individual::mutate(int &numGen)
{
    bool terminado = false;
    int indiceClusterAntiguo = this->solucion.first[numGen];

    if(this->solucion.second[indiceClusterAntiguo].size() <= 1) //No puede mutar en ese gen
    {
        return false;
    }

    do
    {
        int indiceClusterNuevo = Randint(0, Individual::numClusters -1);
        if(indiceClusterNuevo != indiceClusterAntiguo)
        {
            this->solucion.first[numGen] = indiceClusterNuevo;
            borrarPorValor(this->solucion.second[indiceClusterAntiguo], numGen);
            this->solucion.second[indiceClusterNuevo].emplace_back(numGen);
            terminado = true;
        }

    }while(!terminado);

    this->fitness = funcionObjetivo(Individual::datos, Individual::matrizRest, this->solucion.second, this->solucion.first, Individual::LAMBDA);
    
    return true;
}

//Inserta un dato aleatorio donde haya un cluster vacío
bool Individual::repair()
{
    auto solucionOld = this->solucion;
   for(int i=0; i < this->solucion.second.size(); ++i)
   {
       if(this->solucion.second[i].empty())
       {
            int datoRandom;
            int clusterAntiguo;
            do
            {
                datoRandom = Randint(0, datos.size()-1);
                clusterAntiguo = this->solucion.first[datoRandom];

            } while (this->solucion.second[clusterAntiguo].size() < 2);

            this->solucion.first[datoRandom] = i;
            borrarPorValor(this->solucion.second[clusterAntiguo], datoRandom);
            this->solucion.second[i].push_back(datoRandom);
       }
   }
   if(this->solucion.first != solucionOld.first)
   {
       this->fitness = funcionObjetivo(Individual::datos, Individual::matrizRest, this->solucion.second, this->solucion.first, Individual::LAMBDA);
       return true;
   }
   else
   {
       return false;
   }
}

// Genera un vecino aleatorio
Individual Individual::generarVecino()
{
    Individual aux;
    auto solucion = this->solucion.first;
    auto clusters = this->solucion.second;
    int indiceValor, indiceClusterNuevo, indiceClusterAntiguo;

    do{
        indiceValor = Randint(0, datos.size()-1);
        indiceClusterNuevo = Randint(0, numClusters-1);
        indiceClusterAntiguo = solucion[indiceValor];
    }while(clusters[indiceClusterAntiguo].size() < 2 || indiceClusterAntiguo == indiceClusterNuevo); // Mientras no sea válido (El cluster al que pertenezca solo tiene un elemento)

    solucion[indiceValor] = indiceClusterNuevo;

    borrarPorValor(clusters[indiceClusterAntiguo], indiceValor);
    clusters[indiceClusterNuevo].emplace_back(indiceValor);
    
    aux.solucion.first = solucion;
    aux.solucion.second = clusters;
    
    //---------------------------- FACTORIZACION EN CENTROIDES -------------------------------
    std::vector<std::vector<float>> centroidesAux = this->centroides;
    auto centroideClusterAntiguo = centroidesAux[indiceClusterAntiguo];
    auto centroideClusterNuevo = centroidesAux[indiceClusterNuevo];


    std::vector<float> resultado;
    
    auto vectorAux = multiplicarVectorPorEntero(centroideClusterAntiguo, this->solucion.second[indiceClusterAntiguo].size());
    restaVector(vectorAux, datos[indiceValor], resultado);
    auto NEWcentroideClusterAntiguo = multiplicarVectorPorEntero(resultado, 1.0/(this->solucion.second[indiceClusterAntiguo].size() -1.0));
    centroidesAux[indiceClusterAntiguo] = NEWcentroideClusterAntiguo;

    vectorAux = multiplicarVectorPorEntero(centroideClusterNuevo, this->solucion.second[indiceClusterNuevo].size());
    sumaVector(vectorAux, datos[indiceValor], resultado);
    auto NEWcentroideClusterNuevo = multiplicarVectorPorEntero(resultado, 1.0/(this->solucion.second[indiceClusterNuevo].size() +1.0));
    centroidesAux[indiceClusterNuevo] = NEWcentroideClusterNuevo;
    
    aux.centroides = centroidesAux;

    // ----------------------------- SIN FACTORIZAR ----------------------------
    // vectorCentroides(Individual::datos, aux.solucion.second, aux.centroides);

    aux.fitness = funcionObjetivo(Individual::datos, Individual::matrizRest, aux.solucion.second, aux.solucion.first, aux.centroides, Individual::LAMBDA);
    
    return aux;
}

// Se supone que el par no incumple restricciones
Individual Individual::generarVecino(std::pair<int,int> &vecino)
{
    Individual aux;
    auto solucion = this->solucion.first;
    auto clusters = this->solucion.second;

    int indiceValor = vecino.first;
    int indiceClusterNuevo = vecino.second;
    int indiceClusterAntiguo = solucion[indiceValor];

    solucion[indiceValor] = indiceClusterNuevo;

    borrarPorValor(clusters[indiceClusterAntiguo], indiceValor);
    clusters[indiceClusterNuevo].emplace_back(indiceValor);
    
    aux.solucion.first = solucion;
    aux.solucion.second = clusters;
    
    //---------------------------- FACTORIZACION EN CENTROIDES -------------------------------
    std::vector<std::vector<float>> centroidesAux = this->centroides;
    auto centroideClusterAntiguo = centroidesAux[indiceClusterAntiguo];
    auto centroideClusterNuevo = centroidesAux[indiceClusterNuevo];


    std::vector<float> resultado;
    
    auto vectorAux = multiplicarVectorPorEntero(centroideClusterAntiguo, this->solucion.second[indiceClusterAntiguo].size());
    restaVector(vectorAux, datos[indiceValor], resultado);
    auto NEWcentroideClusterAntiguo = multiplicarVectorPorEntero(resultado, 1.0/(this->solucion.second[indiceClusterAntiguo].size() -1.0));
    centroidesAux[indiceClusterAntiguo] = NEWcentroideClusterAntiguo;

    vectorAux = multiplicarVectorPorEntero(centroideClusterNuevo, this->solucion.second[indiceClusterNuevo].size());
    sumaVector(vectorAux, datos[indiceValor], resultado);
    auto NEWcentroideClusterNuevo = multiplicarVectorPorEntero(resultado, 1.0/(this->solucion.second[indiceClusterNuevo].size() +1.0));
    centroidesAux[indiceClusterNuevo] = NEWcentroideClusterNuevo;
    
    aux.centroides = centroidesAux;


    // ----------------------------- SIN FACTORIZAR ----------------------------
    // vectorCentroides(Individual::datos, aux.solucion.second, aux.centroides);


    aux.fitness = funcionObjetivo(Individual::datos, Individual::matrizRest, aux.solucion.second, aux.solucion.first, aux.centroides, Individual::LAMBDA);
    
    return aux;
}

void Individual::operator=(const Individual &ind)
{
    this->solucion = ind.solucion;
    this->centroides = ind.centroides;
    this->fitness = ind.getFitness();
}

bool Individual::operator<(const Individual &ind) 
{ 
    return this->getFitness() < ind.getFitness(); 
}

bool Individual::operator>=(const Individual &ind) 
{ 
    return !(*this < ind); 
}

bool Individual::operator==(const Individual &ind)
{
    if(this == &ind)
    {
        return true;
    }
    else
    {
        if(this->solucion.first == ind.solucion.first)
        {
            return true;
        }
        else
        {
            return false;
        }
        
    }
    
}

bool Individual::operator!=(const Individual &ind)
{
    return !(*this == ind);
}

bool Individual::operator>(const Individual &ind)
{
    return (*this >= ind) && (*this != ind);
}
