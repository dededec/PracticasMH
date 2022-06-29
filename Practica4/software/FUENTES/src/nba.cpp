// TODO:
// ? Preguntas
//// texto tachado
// ! ¿Qué pasaría con empates?
// * HIGHLIGHTED

#include "busquedaLocal.h"

#include "nba.h"
#include "random.h"
#include "operacionesVector.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>

#include <random>
#include <iostream>


template<class T>
std::ostream& operator<<(std::ostream & os, const std::vector<T> &V)
{
	  
	  
	  typename std::vector<T>::const_iterator it;
	  
	  for (it=V.begin(); it!=V.end(); ++it) os << (*it) << " | ";

      os<< std::endl << "********************************************"<<std::endl;
	  
	  return os;
}

template<class T>
std::ostream& operator<<(std::ostream & os, const std::pair<T,T> &V)
{
    os << '(' << V.first << " , " << V.second << ')';
	return os;
}


/*
Aplica sigmoide(val,min)
@param min ayuda a ralentizar ligeramente cómo se acerca el sigmoide a valores cercanos a 1 ó 0
Razon de uso: Quiero una función que premie una diferencia grande entre valores de f, pero sin llegar a coger una solución con 100% de probabilidad, para dar una oportunidad a la otra
El sigmoide da eso ya que nos da una probabilidad entre 0 y 1 pero tiene 1 y 0 como asíntota.
*/
float probabilidad(float val, float min) { return 1 / (1+exp(-val/(min*0.1))); }

int jugarPartido(int i, int j, std::vector<Individual> &equipos)
{
    int ganador = i;

    float mejora = equipos[i].getFitness() - equipos[j].getFitness();
    float menor = std::min(equipos[i].getFitness(), equipos[j].getFitness());

    float probab = probabilidad(mejora, menor); // Probabilidad de que gane equipo[j]
    float random = Randfloat(0.0, 1.0);

    if(random < probab) ganador = j;

    return ganador;
}

std::vector<int> partidosGanados; // Declarado aquí para poder ordenar clasificación respecto de este vector

int compare (const void * a, const void * b) {
    double diff = partidosGanados[*(int*)a] - partidosGanados[*(int*)b];
    return  (0 < diff) - (diff < 0);
}


Individual NBADraft(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, 
        int &numClusters, float &LAMBDA, int numEquipos, int numTemporadas, int numPartidos)
{
    Individual::datos = datos;
    Individual::matrizRest = matrizRest;
    Individual::LAMBDA = LAMBDA;
    Individual::numClusters = numClusters;

    partidosGanados.resize(numEquipos);


    auto mejorF = MAXFLOAT;
    Individual mejorSolucion;

    // Creamos equipos (sol. aleatorias)
    std::vector<Individual> equipos; 
    for(int i=0; i< numEquipos; ++i)
    {
        Individual aux;
        aux.inicializarAleatorio();
        equipos.emplace_back(aux);

        if(aux.getFitness() < mejorF)
        {
            mejorF = aux.getFitness();
            mejorSolucion = aux;
        }
    }

    // ************************************* JUGAMOS X TEMPORADAS + DRAFT *************************************

    int temporadasJugadas = 0;
    while(temporadasJugadas < numTemporadas)
    {
        for(int i=0; i<numEquipos; ++i) partidosGanados[i] = 0;

        // ****************** TEMPORADA REGULAR ******************

        // Jugamos los partidos
        for(int i=0; i<numEquipos-1; ++i)
        {
            for(int j=i+1; j<numEquipos; ++j)
            {
                for(int k=0; k<numPartidos; ++k)
                {
                    int ganador = jugarPartido(i, j, equipos);
                    ++partidosGanados[ganador];
                }
            }
        }


        // Clasificamos los equipos según el nº de partidos ganados
        int clasificacion[numEquipos] = { 0 }; 
        std::vector<float> aux(numEquipos);
        for(int i=0; i<numEquipos; ++i) aux[i] = partidosGanados[i];
        for(int i=0; i<numEquipos; ++i)
        {
            int indiceMayor = 0;
            int maxPartidos = aux[0];
            for(int j=1; j<numEquipos; ++j)
            {
                if(aux[j] > maxPartidos)
                {
                    indiceMayor = j;
                    maxPartidos = aux[j];
                }
                else if(aux[j] == maxPartidos) // Si hay empate, se desempata por mejor valor de f
                {
                    if(equipos[j].getFitness() < equipos[indiceMayor].getFitness())
                    {
                        indiceMayor = j;
                        maxPartidos = aux[j];
                    }
                }
            }

            clasificacion[indiceMayor] = i;
            aux[indiceMayor] = -1;
        }


        // ****************** LLEGA EL DRAFT ******************
        
        // Asignamos elecciones
        std::vector<int> indicesSorteo(numEquipos);
        // for(int i=0; i<numEquipos;++i) indicesSorteo[i] = i; //! ORDEN ALEATORIO
        // std::shuffle(indicesSorteo.begin(), indicesSorteo.end(), std::default_random_engine(Get_random()));
        
        for(int i=0; i<numEquipos; ++i) indicesSorteo[i] = clasificacion[numEquipos-1-i]; //! ORDEN INVERSO A CLASIFICACION
        // for(int i=0; i<numEquipos; ++i) indicesSorteo[i] = clasificacion[i]; //! ORDEN IGUAL A CLASIFICACION

        // Generamos soluciones a draftear
        std::vector<Individual> drafteados;
        for(int i=0; i<numEquipos; ++i)
        {
            Individual aux;
            aux.inicializarAleatorio();
            drafteados.emplace_back(aux);
        }

        std::sort(drafteados.begin(), drafteados.end()); //! ORDEN ASCENDENTE, EL ÚLTIMO ES EL PEOR (MAYOR F)

        // Se realiza el draft -> Se cruza cada equipo con su asignado en el draft
        for(int i=0; i<numEquipos; ++i)
        {
            equipos[indicesSorteo[i]] = equipos[indicesSorteo[i]].mateSF(drafteados[i]);
        }
        drafteados.clear();
 
        // ****************** ACTUALIZAMOS MEJOR SOLUCIÓN ENCONTRADA HASTA EL MOMENTO ******************

        std::sort(equipos.begin(), equipos.end());
        if(equipos[0].getFitness() < mejorSolucion.getFitness())
        {
            mejorSolucion = equipos[0];
        }

        ++temporadasJugadas;
    }
    
    return mejorSolucion;
}




Individual NBADraftBL(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, 
        int &numClusters, float &LAMBDA, int numEquipos, int numTemporadas, int numPartidos)
{
    Individual::datos = datos;
    Individual::matrizRest = matrizRest;
    Individual::LAMBDA = LAMBDA;
    Individual::numClusters = numClusters;

    partidosGanados.resize(numEquipos);


    auto mejorF = MAXFLOAT;
    Individual mejorSolucion;

    // Creamos equipos (sol. aleatorias)
    std::vector<Individual> equipos; 
    for(int i=0; i< numEquipos; ++i)
    {
        Individual aux;
        aux.inicializarAleatorio();
        equipos.emplace_back(aux);

        if(aux.getFitness() < mejorF)
        {
            mejorF = aux.getFitness();
            mejorSolucion = aux;
        }
    }

    // ************************************* JUGAMOS X TEMPORADAS + DRAFT *************************************

    int temporadasJugadas = 0;
    while(temporadasJugadas < numTemporadas)
    {
        for(int i=0; i<numEquipos; ++i) partidosGanados[i] = 0;

        // ****************** TEMPORADA REGULAR ******************

        // Jugamos los partidos
        for(int i=0; i<numEquipos-1; ++i)
        {
            for(int j=i+1; j<numEquipos; ++j)
            {
                for(int k=0; k<numPartidos; ++k)
                {
                    int ganador = jugarPartido(i, j, equipos);
                    ++partidosGanados[ganador];
                }
            }
        }


        // Clasificamos los equipos según el nº de partidos ganados
        int clasificacion[numEquipos] = { 0 }; 
        std::vector<float> aux(numEquipos);
        for(int i=0; i<numEquipos; ++i) aux[i] = partidosGanados[i];
        for(int i=0; i<numEquipos; ++i)
        {
            int indiceMayor = 0;
            int maxPartidos = aux[0];
            for(int j=1; j<numEquipos; ++j)
            {
                if(aux[j] > maxPartidos)
                {
                    indiceMayor = j;
                    maxPartidos = aux[j];
                }
                else if(aux[j] == maxPartidos) // Si hay empate, se desempata por mejor valor de f
                {
                    if(equipos[j].getFitness() < equipos[indiceMayor].getFitness())
                    {
                        indiceMayor = j;
                        maxPartidos = aux[j];
                    }
                }
            }

            clasificacion[i] = indiceMayor; //! EL PUESTO i ES EL EQUIPO INDICE MAYOR
            aux[indiceMayor] = -1;
        }


        // ****************** LLEGA EL DRAFT ******************
        
        // Asignamos elecciones
        std::vector<int> indicesSorteo(numEquipos);

        // for(int i=0; i<numEquipos;++i) indicesSorteo[i] = i; //! ORDEN ALEATORIO
        // std::shuffle(indicesSorteo.begin(), indicesSorteo.end(), std::default_random_engine(Get_random()));

        // for(int i=0; i<numEquipos; ++i) indicesSorteo[i] = clasificacion[numEquipos-1-i]; //!ORDEN INVERSO A CLASIFICACION
        for(int i=0; i<numEquipos; ++i) indicesSorteo[i] = clasificacion[i]; //! ORDEN IGUAL A CLASIFICACION


        // Generamos soluciones a draftear
        std::vector<Individual> drafteados;
        for(int i=0; i<numEquipos; ++i)
        {
            Individual aux;
            aux.inicializarAleatorio();
            drafteados.emplace_back(aux);
        }

        std::sort(drafteados.begin(), drafteados.end()); //! ORDEN ASCENDENTE, EL ÚLTIMO ES EL PEOR

        // Se realiza el draft -> Se cruza cada equipo con su asignado en el draft
        for(int i=0; i<numEquipos; ++i)
        {
            equipos[indicesSorteo[i]] = equipos[indicesSorteo[i]].mateSF(drafteados[i]); //! CRUZAMOS EL PEOR CON EL MEJOR JUGADOR
        }
        drafteados.clear();
        
        
        // ****************** ENTRENAMIENTO PARA LA SIGUIENTE TEMPORADA (LOCAL SEARCH) ******************

        std::sort(equipos.begin(), equipos.end()); //! SE ORDENAN DE MEJOR SOLUCION A PEOR
        
        // for(int i=0; i<numEquipos; ++i) //! SE APLICA A TODOS
        // {
        //     Individual aux = equipos[i];
        //     equipos[i] = busquedaLocal(aux);
        // }

        for(int i=0; i<numEquipos/3; ++i) //! SE APLICA A LOS N/3 MEJORES
        {
            Individual aux = equipos[i];
            equipos[i] = busquedaLocal(aux);
        }

        // for(int i=0; i<numEquipos/2; ++i) //! SE APLICA A LOS N/2 MEJORES
        // {
        //     Individual aux = equipos[i];
        //     equipos[i] = busquedaLocal(aux);
        // }

        // int inicio = 2*numEquipos/3;
        // for(int i=inicio; i<numEquipos; ++i) //! SE APLICA A LOS N/3 PEORES (De más a menos valor de f)
        // {
        //     Individual aux = equipos[i];
        //     equipos[i] = busquedaLocal(aux);
        // }
 
        // ****************** ACTUALIZAMOS MEJOR SOLUCIÓN ENCONTRADA HASTA EL MOMENTO ******************

        std::sort(equipos.begin(), equipos.end());
        if(equipos[0].getFitness() < mejorSolucion.getFitness())
        {
            mejorSolucion = equipos[0];
        }

        ++temporadasJugadas;
    }
    
    return mejorSolucion;
}