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


float Individual::getFitness() const
{
    return this->fitness;
}

std::pair<std::vector<int>,std::vector<std::vector<int>>> Individual::getChromosome()
{
    return this->chromosome;
}

Individual::Individual()
{
    this->chromosome.first = std::vector<int>();
    this->chromosome.second = std::vector<std::vector<int>>();
}

Individual::Individual(std::vector<int> &chromosome, std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, int &numClusters, int populationSize, float &LAMBDA) 
{ 
    //Establecemos variables de clase:
    Individual::numClusters = numClusters;
    Individual::datos = datos;
    Individual::matrizRest = matrizRest;
    Individual::LAMBDA = LAMBDA;

    this->chromosome.first = chromosome;
    std::vector<std::vector<int>> clusters(Individual::numClusters);
    for(int i = 0; i < chromosome.size();++i)
    {
        clusters[chromosome[i]].emplace_back(i);
    } 
    this->chromosome.second = clusters;

    this->fitness = funcionObjetivo(datos, matrizRest, clusters, chromosome, LAMBDA);  
}

Individual::Individual(std::vector<int> &chromosome)
{
    this->chromosome.first = chromosome;
    std::vector<std::vector<int>> clusters(Individual::numClusters);
    for(int i = 0; i < chromosome.size();++i)
    {
        clusters[chromosome[i]].emplace_back(i);
    } 
    this->chromosome.second = clusters;
    this->fitness = funcionObjetivo(Individual::datos, Individual::matrizRest, clusters, chromosome, Individual::LAMBDA);
}

Individual::Individual(std::pair<std::vector<int>,std::vector<std::vector<int>>> &chromosome)
{
    this->chromosome = chromosome;
    this->fitness = this->fitness = funcionObjetivo(Individual::datos, Individual::matrizRest, chromosome.second, chromosome.first, Individual::LAMBDA);
}
  
  /*
  
  Por ejemplo, con una población de 60 cromosomas de 100 genes cada
uno tenemos 6000 genes de los cuales mutarían unos 60 (Nº esperado
mutaciones = P_m · nº genes población, esperanza matemática)

✔ Generar 6000 números aleatorios en cada generación para hacer sólo
60 mutaciones (en media) es un gasto inútil. Para evitarlo, haremos
siempre exactamente (Nº esperado mutaciones) en cada generación

✔ Aparte de hacer un número fijo de mutaciones, hay que decidir cuáles
son los genes que mutan

✔ Normalmente, eso se hace también generando números aleatorios, en
concreto dos, un entero en {1, ..., M} para escoger el cromosoma y
otro en {1, ..., n} para el gen
  */

// Perform mating and produce new offspring 
Individual Individual::mateUN(Individual par2) 
{ 
    int len = this->chromosome.first.size(); 
    std::vector<int> child_chromosome(len,-1);

    for(int i = 0;i<len/2;i++) 
    { 
        int random = Randint(0, len-1);
        child_chromosome[random] = this->chromosome.first[random];
    }

    for(int i=0; i<len; ++i)
    {
        if(child_chromosome[i] == -1)
        {
            child_chromosome[i] = par2.chromosome.first[i];
        }
    } 

    return Individual(child_chromosome); 
}

Individual Individual::mateSF(Individual par2)
{
    int len = this->chromosome.first.size();

    int inicio = Randint(0, len-1);
    int longitud = Randint(0, len-1);
    std::vector<int> child_chromosome(len,-1);

    int i=inicio, iter=0;
    while(iter < longitud)
    {
        child_chromosome[i] = this->chromosome.first[i];
        i = (i+1) % len;
        ++iter;
    }
    for(int i=0; i<len; ++i)
    {
        if(child_chromosome[i] == -1)
        {
            child_chromosome[i] = par2.chromosome.first[i];
        }
    } 
    return Individual(child_chromosome); //<- FALLO AQUÍ
}


bool Individual::mutate(int &numGen)
{
    bool terminado = false;
    int indiceClusterAntiguo = this->chromosome.first[numGen];

    if(this->chromosome.second[indiceClusterAntiguo].size() <= 1) //No puede mutar en ese gen
    {
        return false;
    }

    do
    {
        int indiceClusterNuevo = Randint(0, Individual::numClusters -1);
        if(indiceClusterNuevo != indiceClusterAntiguo)
        {
            this->chromosome.first[numGen] = indiceClusterNuevo;
            borrarPorValor(this->chromosome.second[indiceClusterAntiguo], numGen);
            this->chromosome.second[indiceClusterNuevo].emplace_back(numGen);
            terminado = true;
        }

    }while(!terminado);

    this->fitness = funcionObjetivo(Individual::datos, Individual::matrizRest, this->chromosome.second, this->chromosome.first, Individual::LAMBDA);
    
    return true;
}

//Inserta un dato aleatorio donde haya un cluster vacío
bool Individual::repair()
{
    auto chromosomeOld = this->chromosome;
   for(int i=0; i < this->chromosome.second.size(); ++i)
   {
       if(this->chromosome.second[i].empty())
       {
            int datoRandom;
            int clusterAntiguo;
            do
            {
                datoRandom = Randint(0, datos.size()-1);
                clusterAntiguo = this->chromosome.first[datoRandom];

            } while (this->chromosome.second[clusterAntiguo].size() < 2);

            this->chromosome.first[datoRandom] = i;
            borrarPorValor(this->chromosome.second[clusterAntiguo], datoRandom);
            this->chromosome.second[i].push_back(datoRandom);
       }
   }
   if(this->chromosome.first != chromosomeOld.first)
   {
       this->fitness = funcionObjetivo(Individual::datos, Individual::matrizRest, this->chromosome.second, this->chromosome.first, Individual::LAMBDA);
       return true;
   }
   else
   {
       return false;
   }
}

void Individual::operator=(const Individual &ind)
{
    this->chromosome = ind.chromosome;
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
        if(this->chromosome.first == ind.chromosome.first)
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
