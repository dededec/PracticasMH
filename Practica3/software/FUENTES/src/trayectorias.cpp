#include "trayectorias.h"
#include "busquedaLocal.h"
#include <vector>
#include "random.h"
#include <math.h>

#include<iostream>

bool metrop (float de, float t)
/* Implementacion del criterio de Metropolis. Devuelve 1 si el vecino
   es aceptado y 0 en caso contrario. Acepta automaticamente dicho
   vecino si es mejor que la solucion actual (es decir, si
   de = F(Sactual) - F(Snueva) es negativo). En caso contrario, lo
   acepta o rechaza probabilisticamente en funcion del valor de la
   temperatura, t, y la diferencia de costos, de. */

{
 return de < 0.0 || Rand() < exp(-de/t);
}

Individual enfriamientoSimulado(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, 
        int &numClusters, float &LAMBDA)
{
    // Generar solución aleatoria
    Individual solucionActual, solucionMejor;
    float t_final = 1/1000;
    float t_actual;
    
    do
    {
        solucionActual.inicializarAleatorio(numClusters, LAMBDA, datos, matrizRest);
        t_actual = 0.3 * solucionActual.getFitness() / -log(0.3);
    
    }while(t_actual < t_final);
        
    solucionMejor = solucionActual;

    // Calculamos parámetros
    int maxVecinos = datos.size() * 10;
    int maxExitos = datos.size(); // 0.1 * maxVecinos = 0.1 * 10 * datos.size() = datos.size()
    int maxEvaluaciones = 100000;
    int numEnfriamientos = maxEvaluaciones / maxVecinos;

    float beta = (t_actual - t_final) / (numEnfriamientos*t_final*t_actual);

    int evaluaciones = 0;
    //Bucle exterior
    for(int j=0; j<numEnfriamientos && evaluaciones<maxEvaluaciones; ++j)
    {
        int numExitos = 0;
        for(int i=0; i<maxVecinos; ++i) // Bucle interior
        {
            Individual vecino = solucionActual.generarVecino(); // Generamos vecino (solucionActual no cambia)
            ++evaluaciones;
            float dif = vecino.getFitness() - solucionActual.getFitness();
            bool acepta = metrop(dif, t_actual);

            if(acepta)
            {
                ++numExitos;
                solucionActual = vecino;
                if(solucionActual < solucionMejor)
                {
                    solucionMejor = solucionActual;
                }
            }
            if(numExitos >= maxExitos) break;
        }

        if(numExitos <= 0) break;

        t_actual /= (1+beta*t_actual);
    }

    return solucionMejor;

}

Individual enfriamientoSimulado(Individual &ind)
{
    Individual solucionActual, solucionMejor;
    float t_final = 1/1000;
    float t_actual;
    solucionActual = ind;
    t_actual = 0.3 * solucionActual.getFitness() / -log(0.3);
        
    solucionMejor = solucionActual;

    // Calculamos parámetros
    int maxVecinos = Individual::datos.size() * 10;
    int maxExitos = maxVecinos * 0.1; // 0.1 * maxVecinos = 0.1 * 10 * datos.size() = datos.size()
    int maxEvaluaciones = 10000;
    int numEnfriamientos = maxEvaluaciones / maxVecinos;

    float beta = (t_actual - t_final) / (numEnfriamientos*t_final*t_actual);

    int evaluaciones = 0;
    //Bucle exterior
    for(int j=0; j<numEnfriamientos && evaluaciones<maxEvaluaciones; ++j)
    {
        int numExitos = 0;
        for(int i=0; i<maxVecinos; ++i) // Bucle interior
        {
            Individual vecino = solucionActual.generarVecino(); // Generamos vecino (solucionActual no cambia)
            ++evaluaciones;
            float dif = vecino.getFitness() - solucionActual.getFitness();
            bool acepta = metrop(dif, t_actual);

            if(acepta)
            {
                ++numExitos;
                solucionActual = vecino;
                if(solucionActual < solucionMejor)
                {
                    solucionMejor = solucionActual;
                }
            }
            if(numExitos >= maxExitos) break;
        }

        if(numExitos <= 0) break;

        t_actual /= (1+beta*t_actual);
    }

    return solucionMejor;

}


Individual BMB(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, 
        int &numClusters, float &LAMBDA)
{

    Individual solucionActual = busquedaLocal(datos, matrizRest, numClusters, LAMBDA);
    Individual solucionMejor = solucionActual;

    for(int i=0; i<9; ++i)
    {
        Individual solucionActual = busquedaLocal(datos, matrizRest, numClusters, LAMBDA);
        if(solucionActual < solucionMejor)
        {
            solucionMejor = solucionActual;
        }
    }
    return solucionMejor;
}

Individual mutacion(Individual &ind)
{
    int contadores[Individual::numClusters] = { 0 };

    auto aux = ind.getSolucion().first;
    int inicio = Randint(0, aux.size()-1);
    int mas = Randint(0, 1);

    int fin;

    do{
        if(mas == 0)
        {
            fin = inicio - 0.1*aux.size();
            if(fin < 0) mas = 1;
        }
        if(mas == 1)
        {
            fin = inicio + 0.1*aux.size();
            if(fin >= aux.size()) mas = 0;
        }
    }while(!(fin >= 0 && fin < aux.size()));

    std::vector<int> mutado(aux.size(),-1);


    int i=(fin+1)%mutado.size(); //Si fin es size-1, i tiene que ser 0
    while(i != inicio) // Copiamos los elementos fuera del segmento
    {
        mutado[i] = aux[i];
        ++contadores[mutado[i]];
        i = (i+1)%mutado.size();
    }
    
    bool valido;
    do
    {
        i = inicio;
        valido = true;
        while(i != (fin+1)%mutado.size())
        {
            mutado[i] = Randint(0, Individual::numClusters-1);
            ++contadores[mutado[i]];
            i = (i+1)%mutado.size();
        }

        for(int j=0; j<Individual::numClusters; ++j)
        {
            if(contadores[j] == 0) valido = false;
        }

    }while(!valido);



    return Individual(mutado);
}


Individual ILS(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, 
        int &numClusters, float &LAMBDA)
{
    // Generamos solución aleatoria
    Individual solucionActual, solucionMejor;
    solucionActual.inicializarAleatorio(numClusters, LAMBDA, datos, matrizRest);
    solucionMejor = solucionActual;

    solucionActual = busquedaLocal(solucionActual);
    if(solucionActual < solucionMejor) solucionMejor = solucionActual;

    for(int i=0; i< 9; ++i)
    {
        // Mutar mejorSolucion
        Individual mutado = mutacion(solucionMejor);
        
        // BL sobre mutado
        Individual newSolucion = busquedaLocal(mutado);

        //Criterio de aceptación:
        if(newSolucion < solucionMejor)
        {
            solucionMejor = newSolucion;
        }
    }

    return solucionMejor;

}


Individual ILS_ES(std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, 
        int &numClusters, float &LAMBDA)
{
    // Generamos solución aleatoria
    Individual solucionActual, solucionMejor;
    solucionActual.inicializarAleatorio(numClusters, LAMBDA, datos, matrizRest);
    solucionMejor = solucionActual;

    solucionActual = enfriamientoSimulado(solucionActual);
    if(solucionActual < solucionMejor) solucionMejor = solucionActual;

    for(int i=0; i< 9; ++i)
    {
        // Mutar mejorSolucion
        Individual mutado = mutacion(solucionMejor);
        
        // BL sobre mutado
        Individual newSolucion = enfriamientoSimulado(mutado);

        //Criterio de aceptación:
        if(newSolucion < solucionMejor)
        {
            solucionMejor = newSolucion;
        }
    }

    return solucionMejor;

}



