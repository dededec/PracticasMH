#include "genetico.h"
#include "random.h"
#include "operacionesVector.h"
#include <random>
#include <algorithm>
#include <vector>
#include <iostream>
#include <chrono>
#include <bits/stdc++.h>

template<class T>
bool findBool(std::vector<T> &v, T &valor)
{
    for(auto it=v.begin(), it2 = v.rbegin(); it != v.end() && it2 != v.rend(); ++it, ++it2)
    {
        if(((*it) == valor) || ((*it2) == valor))
        {
            return true;
        }
    }

    return false;
}

template<class T>
std::ostream& operator<<(std::ostream & os, const std::vector<T> &V)
{
	  
	  
	  typename std::vector<T>::const_iterator it;
	  
	  for (it=V.begin(); it!=V.end(); ++it) os << (*it) << " | ";

      os<< std::endl << "********************************************"<<std::endl;
	  
	  return os;
}


// create chromosome or std::vector of genes 
std::vector<int> create_genome(int numDatos, int numClusters) 
{ 
    int numAleatorio, limite = numClusters-1;
    int contadores[numClusters] = { 0 };
    bool repetir;
    std::vector<int> gnome = std::vector<int>(numDatos);

    do
    {
        repetir = false;
        for(int i=0;i<numDatos;++i) 
        {
            int numAleatorio = Randint(0, numClusters-1);
            gnome[i] = numAleatorio;
            
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

    return gnome; 
}

Individual selection(Individual &i, Individual &j)
{
    if(i.getFitness() < j.getFitness())
    {
        return i;
    }
    else
    {
        return j;
    }
}

int selection(int &i, int &j, std::vector<Individual> &population)
{
    if(population[i].getFitness() < population[j].getFitness())
    {
        return i;
    }
    else
    {
        return j;
    }
}

Individual AGG(Individual(Individual::*fp)(Individual), std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, int &numClusters, float &LAMBDA)
{
    std::chrono::high_resolution_clock::time_point inicial = std::chrono::high_resolution_clock::now();

    int generation = 0; 
    float Pc = 0.7;
    float Pm = 0.001;
    
    float expectedCrosses = Pc * POPULATION_SIZE_AG;

    std::vector<Individual> population; 
    // Realizamos un elemento con este constructor para establecer las variables de clase
    std::vector<int> gnome = create_genome(datos.size(), numClusters);
    population.emplace_back(Individual(gnome, datos, matrizRest, numClusters, POPULATION_SIZE_AG, LAMBDA));


    // std::cout << "----------------------- SOLUCION INICIAL: --------------------------------\n";
    // std::cout<< "Funcion objetivo: " << population[0].getFitness() << "\n";
    // auto distribCluster = population[0].getChromosome().first;
    // std::cout<< "Infeasibility: " << infeasibility(matrizRest, distribCluster) << "\n";
    // std::cout<< "----------------------------------------------------------------------------" << "\n"; 

    // create initial population 
    for(int i = 1;i<POPULATION_SIZE_AG;i++) 
    { 
        gnome = create_genome(datos.size(), numClusters);
        population.emplace_back(Individual(gnome)); 
    } 
    
    int evaluaciones = 0;
    while(evaluaciones < 100000) 
    { 
        // sort the population in increasing order of fitness score 
        sort(population.begin(), population.end()); 
  
        Individual best = population[0];
  
        // Otherwise generate new offsprings for new generation 
        std::vector<Individual> new_generation; 

        // std::cout << "SELECCION\n";
        /********************* SELECCION: *******************
         * AGG: Tantos torneos como usuarios.
        */
        //AGG
        
        int* parents = new int[POPULATION_SIZE_AG];
        int random1, random2;
        for(int i=0; i < POPULATION_SIZE_AG; ++i)
        {
            do
            {
                random1 = Randint(0, POPULATION_SIZE_AG -1);
                random2 = Randint(0, POPULATION_SIZE_AG -1);
            } while (random1 == random2);

            parents[i] = selection(random1,random2, population);
        }

        // std::cout << "CRUCE\n";
        /********************* CRUCE: *******************
         * Nº esperado cruces = Pc · M/2
         * Pc es 0.7 en AGG y 1 en AGE (Siempre se cruzan)
         * AQUI ES AGG, ya que Pc = 0.7
        */
        int len = expectedCrosses * 2;
        std::vector<Individual> descendencia;
        int lastParent;
        for(int i = 0;i < expectedCrosses; i+=2) 
        { 
            //Emparejamiento fijo (0-1, 2-3, ...)
            /************************************/
            Individual parent1 = population[parents[i]]; 
            Individual parent2 = population[parents[i+1]];
            lastParent = i+1;
            /************************************/
            Individual offspring = (parent1.*fp)(parent2);
            // Individual offspring = parent1.mateSF(parent2);
            ++evaluaciones; //Cada vez que creamos un hijo, realizamos una evaluación
            if(offspring.repair()) ++evaluaciones; /** REPARACION: **/  //Si reparamos, hemos recalculado f
            descendencia.push_back(offspring);
            
            offspring = (parent2.*fp)(parent1);
            // offspring = parent2.mateSF(parent1);
            ++evaluaciones; //Cada vez que creamos un hijo, realizamos una evaluación
            if(offspring.repair()) ++evaluaciones; /** REPARACION: **/ 
            descendencia.push_back(offspring);

        } 
        
        len = POPULATION_SIZE_AG - descendencia.size();
        new_generation = descendencia;
        //Como solo generamos 0.7 * POPULATION_SIZE_AG descendientes, rellenamos con padres.
        for(int i=0;i<len;++i)
        {
            // Cogemos a partir del padre nº expectedCrosses (al cruzar solo nos quedamos con los padres desde
            // 0 hasta expectedCrosses-1)
            new_generation.push_back(population[parents[lastParent+1 + i]]);
        }

        delete [] parents; //Ya no usamos padres

        // std::cout << "MUTACION\n";
        /******************** MUTACION: *******************
            Tenemos (Pc * nº_genes_total) mutaciones que hacer.
            Generamos un aleatorio entre 0 y num_genes * num_cromosomas
            Cambiamos el gen aleatorio mod num_genes del cromosoma
            aleatorio mod num_cromosomas.
        */

        int numGenesTotal = POPULATION_SIZE_AG * datos.size();
        float expectedMutations = Pm * numGenesTotal;
        for(int i=0; i < expectedMutations; ++i)
        {
            /*
            Generas número aleatorio entre 0 y numGenesTotal
            Cromosoma a cambiar = n mod descendencia.size()
            Gen a cambiar = n mod numGenes en un cromosoma
            */
            int random, chromosome, gen;
            do
            {
                random = Randint(0, numGenesTotal);
                chromosome = random % new_generation.size();
                gen = random % datos.size();

            }while(new_generation[chromosome].mutate(gen) == false);
            ++evaluaciones; //Cuando mutamos, actualizamos f

        }
        // std::cout << "ELITISMO\n";
        /******************* ELITISMO: ***********************
         *  AGG: Si perdemos a best en el reemplazamiento, lo metemos por
         * el peor elemento de la nueva generación
        */ 

       if(findBool(new_generation, best) == false) //Si no está best
       {
           float max=0;
           std::vector<Individual>::iterator worstElement;
           for(auto it = new_generation.begin(); it != new_generation.end(); ++it)
           {
               if((*it).getFitness() > max)
               {
                   max=(*it).getFitness();
                   worstElement = it;
               }
           }

            new_generation.erase(worstElement);
            new_generation.push_back(best);
       }

        // std::cout << "REEMPLAZAMIENTO\n";
        /******************** REEMPLAZAMIENTO: *******************
         * AGG: La descendencia sustituye a la actual. Para conservar
         * el elitismo, si la mejor solución de la generación anterior
         * no sobrevive, sustituye directamente a la peor de la nueva.
        */
        population = new_generation; 
        
        new_generation.clear();
        generation++; 
    } 

    std::chrono::high_resolution_clock::time_point final = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duracion = std::chrono::duration_cast<std::chrono::duration<double>>(final-inicial);

    // sort the population in increasing order of fitness score 
    sort(population.begin(), population.end());

    // std::cout<< "Generation: " << generation << "\n"; 
    
    auto distribCluster = population[0].getChromosome().first;
    auto clusters = population[0].getChromosome().second;
    std::cout<< "TaC: " << C(datos, clusters);
    std::cout<< "Inf: " << infeasibility(matrizRest, distribCluster);
    std::cout<< "Fun: " << population[0].getFitness() << "\n";
    std::cout << "\nTmp: " << duracion.count()  ;

    return population[0];
}

/****************************************** AGE: ******************************************/

Individual AGE(Individual(Individual::*fp)(Individual), std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, int &numClusters, float &LAMBDA)
{   

    std::chrono::high_resolution_clock::time_point inicial = std::chrono::high_resolution_clock::now();
    // current generation 
    int generation = 0; 
    float Pc = 1;
    float Pm = 0.001;

    std::vector<Individual> population; 
    // Realizamos un elemento con este constructor para establecer las variables de clase
    std::vector<int> gnome = create_genome(datos.size(), numClusters);
    population.emplace_back(Individual(gnome, datos, matrizRest, numClusters, POPULATION_SIZE_AG, LAMBDA));


    // std::cout << "----------------------- SOLUCION INICIAL: --------------------------------\n";
    // std::cout<< "Funcion objetivo: "<< population[0].getFitness() << "\n";
    // auto distribCluster = population[0].getChromosome().first;
    // std::cout<< "Infeasibility: " << infeasibility(matrizRest, distribCluster) << "\n";
    // std::cout<< "----------------------------------------------------------------------------" << "\n"; 

    // create initial population 
    for(int i = 1;i<POPULATION_SIZE_AG;i++) 
    { 
        gnome = create_genome(datos.size(), numClusters);
        population.emplace_back(Individual(gnome)); 
    } 
    
    int evaluaciones = 0;
    while(evaluaciones < 100000) 
    { 
        // sort the population in increasing order of fitness score 
        // sort(population.begin(), population.end()); 
  
        // Otherwise generate new offsprings for new generation 
        std::vector<Individual> new_generation; 


        /********************* SELECCION: *******************
         * AGE: Dos veces para elegir dos padres.
        */
        std::vector<int> parents; //Indice de los padres en la poblacion
        int random1, random2;
        for(int i=0; i < 2; ++i)
        {
            do
            {
                random1 = Randint(0, POPULATION_SIZE_AG -1);
                random2 = Randint(0, POPULATION_SIZE_AG -1);
            } while (random1 == random2);

            parents.push_back(selection(random1,random2, population));
        }

        /********************* CRUCE: ********************/

        std::vector<Individual> descendencia;

        /************************************/
        Individual parent1 = population[parents[0]]; 
        Individual parent2 = population[parents[1]]; 
        /************************************/
        
        Individual offspring = (parent1.*fp)(parent2);
        ++evaluaciones; //Cada vez que creamos un hijo, realizamos una evaluación
        if(offspring.repair()) ++evaluaciones; /** REPARACION: **/ //Si reparamos, hemos recalculado f
        descendencia.push_back(offspring);

        offspring = (parent2.*fp)(parent1);
        ++evaluaciones; //Cada vez que creamos un hijo, realizamos una evaluación
        if(offspring.repair()) ++evaluaciones; /** REPARACION: **/ //Si reparamos, hemos recalculado f
        descendencia.push_back(offspring);


        /******************** MUTACION: *******************
            Tenemos (Pc * nº_genes_total) mutaciones que hacer.
            Generamos un aleatorio entre 0 y num_genes * num_cromosomas
            Cambiamos el gen aleatorio mod num_genes del cromosoma
            aleatorio mod num_cromosomas.
        */
        int numGenesTotal = descendencia.size() * datos.size();
        float expectedMutations = Pm * numGenesTotal;
        for(int i=0; i < expectedMutations; ++i)
        {
            
            /*
            Generas número aleatorio entre 0 y numGenesTotal
            Cromosoma a cambiar = n mod descendencia.size()
            Gen a cambiar = n mod numGenes en un cromosoma
            */
            int random, chromosome, gen;
            do
            {
                random = Randint(0, numGenesTotal);
                chromosome = random%descendencia.size();
                gen = random % datos.size();

            }while(descendencia[chromosome].mutate(gen) == false);
            ++evaluaciones;
        }

        /******************** REEMPLAZAMIENTO: *******************
         * 
         * AGE: Los dos descendencientes generados sustituyen a los dos
         * peores de la población actual si son mejores que ellos (Si 1 es mejor
         * que a, se sustituye, y lo mismo con 2 y b)
         * 
        */

       new_generation = population;

       for(int i=0; i<2; ++i) //La descendencia la cambiamos por los peores (Si es que es mejor que los peores)
       {
           float max=0;
           std::vector<Individual>::iterator worstElement;
           for(auto it = new_generation.begin(); it != new_generation.end(); ++it)
           {
               if((*it).getFitness() > max)
               {
                   max=(*it).getFitness();
                   worstElement = it;
               }
           }

           // Miramos los hijos que haya a ver si hay alguno mejor que el peor.
           for(int j=0; j<descendencia.size(); ++j)
           {
                if(descendencia[j] < *worstElement)
                {
                    new_generation.erase(worstElement);
                    new_generation.push_back(descendencia[j]);
                    descendencia.erase(descendencia.begin()+j);
                    break;
                }
           }
       }

        population = new_generation; 
        generation++; 
    } 

    std::chrono::high_resolution_clock::time_point final = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duracion = std::chrono::duration_cast<std::chrono::duration<double>>(final-inicial);

    // sort the population in increasing order of fitness score 
    sort(population.begin(), population.end());

    // std::cout<< "Generation: " << generation << "\n";
    auto distribCluster = population[0].getChromosome().first;
    auto clusters = population[0].getChromosome().second;
    std::cout<< "TaC: " << C(datos, clusters);
    std::cout<< "\nInf: " << infeasibility(matrizRest, distribCluster);
    std::cout<< "\nFun: " << population[0].getFitness();
    std::cout<< "\nTmp: " << duracion.count()  ;

    return population[0];
}

void BLS(Individual &S, int numFallos, int &evaluaciones)
{

    std::pair<std::vector<int>,std::vector<std::vector<int>>> chromosome = S.getChromosome(); // Copio el cromosoma original

    std::vector<int> rsi(S.getChromosome().first.size(), 0); // Genero indices aleatorios
    for(int i=0; i<rsi.size(); ++i) rsi[i] = i;
    std::random_shuffle(rsi.begin(), rsi.end());
    
    Individual min;
    int len = chromosome.first.size();
    int fallos = 0;
    bool mejora = true;
    int i=0;
    while((mejora || fallos<numFallos) && i<len)
    {
        // std::cout << "i: " << i << " - len: " << len << "\n";
        // std::cout << "fallos: " << fallos << " - numFallos: " << numFallos << "\n";
        // std::cout << "mejora: " << mejora << "\n";

        mejora = false;
        auto auxChromosome = chromosome; //Copio chromosome en el momento
        min = Individual(auxChromosome);
        ++evaluaciones;
        for(int i=0; i<rsi.size(); ++i) // Para cada indice
        {
            

            for(int j=0; j<Individual::numClusters; ++j)
            {
                // Generamos individuo con cluster cambiado
                auxChromosome.first[rsi[i]] = j;
                Individual comp(auxChromosome);
                ++evaluaciones;

                if(comp < min)
                {
                    min = comp;
                    mejora = true; //Hay un cambio, por lo que hay mejora
                }
            }
        }

        if(!mejora) // Si no hemos tenido algun cambio (Cuando hay cambio, hay mejora, por lo que mejora=true)
        {
            ++fallos;
        }

        chromosome = min.getChromosome();
        ++i;
    }

    S = min;
}



Individual AM_1(float porBLS, int interBLS, Individual(Individual::*fp)(Individual), std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, int &numClusters, float &LAMBDA)
{
    std::chrono::high_resolution_clock::time_point inicial = std::chrono::high_resolution_clock::now();
    // std::cout << "INICIO\n";
    int generation = 0; 
    float Pc = 0.7;
    float Pm = 0.001;

    int iter=1;
    
    float expectedCrosses = Pc * POPULATION_SIZE_AM;

    int porcentajeBLS = porcentajeBLS;
    int intervaloBLS = intervaloBLS;

    std::vector<Individual> population; 
    // Realizamos un elemento con este constructor para establecer las variables de clase
    std::vector<int> gnome = create_genome(datos.size(), numClusters);
    population.emplace_back(Individual(gnome, datos, matrizRest, numClusters, POPULATION_SIZE_AM, LAMBDA));


    // std::cout << "----------------------- SOLUCION INICIAL: --------------------------------\n";
    // std::cout<< "Funcion objetivo: " << population[0].getFitness() << "\n";
    // auto distribCluster = population[0].getChromosome().first;
    // std::cout<< "Infeasibility: " << infeasibility(matrizRest, distribCluster) << "\n";
    // std::cout<< "----------------------------------------------------------------------------" << "\n"; 

    // create initial population 
    for(int i = 1;i<POPULATION_SIZE_AM;i++) 
    { 
        gnome = create_genome(datos.size(), numClusters);
        population.emplace_back(Individual(gnome)); 
    } 
    
    int evaluaciones = 0;
    while(evaluaciones < 100000) 
    { 
        if(iter == intervaloBLS) //Aplicamos BLS
        {
            iter = 0;
            float expectedBLS = porcentajeBLS * POPULATION_SIZE_AM;

            std::vector<int> indices(POPULATION_SIZE_AM, 0); //Hago un shuffle de los indices para acceder aleatoriamente a los datos
            for(int i=0; i<indices.size(); ++i) indices[i] = i;
            std::random_shuffle(indices.begin(), indices.end());


            for(int i=0; i<expectedBLS; ++i)
            {
                BLS(population[indices[i]], 0.1*datos.size(), evaluaciones);
            }
        }

        // sort the population in increasing order of fitness score 
        sort(population.begin(), population.end()); 
  
        Individual best = population[0];
  
        // Otherwise generate new offsprings for new generation 
        std::vector<Individual> new_generation; 

        // std::cout << "SELECCION\n";
        /********************* SELECCION: *******************
         * AGG: Tantos torneos como usuarios.
        */
        //AGG
        
        std::vector<int> parents; //Indice de los padres en la poblacion
        int random1, random2;
        for(int i=0; i < POPULATION_SIZE_AM; ++i)
        {
            do
            {
                random1 = Randint(0, POPULATION_SIZE_AM -1);
                random2 = Randint(0, POPULATION_SIZE_AM -1);
            } while (random1 == random2);

            parents.push_back(selection(random1,random2, population));
        }

        // std::cout << "CRUCE\n";
        /********************* CRUCE: *******************
         * Nº esperado cruces = Pc · M/2
         * Pc es 0.7 en AGG y 1 en AGE (Siempre se cruzan)
         * AQUI ES AGG, ya que Pc = 0.7
        */
        std::vector<Individual> descendencia;
        int lastParent;
        for(int i = 0;i < expectedCrosses; i+=2) 
        { 
            //Emparejamiento fijo (0-1, 2-3, ...)
            /************************************/
            Individual parent1 = population[parents[i]]; 
            Individual parent2 = population[parents[i+1]];
            lastParent = i+1;
            /************************************/
            Individual offspring = (parent1.*fp)(parent2);
            // Individual offspring = parent1.mateSF(parent2);
            ++evaluaciones; //Cada vez que creamos un hijo, realizamos una evaluación
            if(offspring.repair()) ++evaluaciones; /** REPARACION: **/  //Si reparamos, hemos recalculado f
            descendencia.push_back(offspring);
            
            offspring = (parent2.*fp)(parent1);
            // offspring = parent2.mateSF(parent1);
            ++evaluaciones; //Cada vez que creamos un hijo, realizamos una evaluación
            if(offspring.repair()) ++evaluaciones; /** REPARACION: **/ 
            descendencia.push_back(offspring);

        } 
        // std::cout << "REEMPLAZAMIENTO\n";
        /******************** REEMPLAZAMIENTO: *******************
         * AGG: La descendencia sustituye a la actual. Para conservar
         * el elitismo, si la mejor solución de la generación anterior
         * no sobrevive, sustituye directamente a la peor de la nueva.
        */

        new_generation = descendencia;
        int len = POPULATION_SIZE_AM - descendencia.size();
        //Como solo generamos 0.7 * POPULATION_SIZE_AG descendientes, rellenamos con padres.
        for(int i=0;i<len;++i)
        {
            // Cogemos a partir del padre nº expectedCrosses (al cruzar solo nos quedamos con los padres desde
            // 0 hasta expectedCrosses-1)
            new_generation.push_back(population[parents[lastParent+1 + i]]);
        }

        // std::cout << "MUTACION\n";
        /******************** MUTACION: *******************
            Tenemos (Pc * nº_genes_total) mutaciones que hacer.
            Generamos un aleatorio entre 0 y num_genes * num_cromosomas
            Cambiamos el gen aleatorio mod num_genes del cromosoma
            aleatorio mod num_cromosomas.
        */

        int numGenesTotal = POPULATION_SIZE_AM * datos.size();
        float expectedMutations = Pm * numGenesTotal;
        for(int i=0; i < expectedMutations; ++i)
        {
            /*
            Generas número aleatorio entre 0 y numGenesTotal
            Cromosoma a cambiar = n mod descendencia.size()
            Gen a cambiar = n mod numGenes en un cromosoma
            */
            int random, chromosome, gen;
            do
            {
                random = Randint(0, numGenesTotal);
                chromosome = random % new_generation.size();
                gen = random % datos.size();

            }while(new_generation[chromosome].mutate(gen) == false);
            ++evaluaciones; //Cuando mutamos, actualizamos f

        }
        // std::cout << "ELITISMO\n";
        /******************* ELITISMO: ***********************
         *  AGG: Si perdemos a best en el reemplazamiento, lo metemos por
         * el peor elemento de la nueva generación
        */ 

       if(findBool(new_generation, best) == false) //Si no está best
       {
           float max=0;
           std::vector<Individual>::iterator worstElement;
           for(auto it = new_generation.begin(); it != new_generation.end(); ++it)
           {
               if((*it).getFitness() > max)
               {
                   max=(*it).getFitness();
                   worstElement = it;
               }
           }

            new_generation.erase(worstElement);
            new_generation.push_back(best);
       }

        population = new_generation; 
        generation++; 
        ++iter;
    } 

    std::chrono::high_resolution_clock::time_point final = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duracion = std::chrono::duration_cast<std::chrono::duration<double>>(final-inicial);

    // sort the population in increasing order of fitness score 
    sort(population.begin(), population.end());

    // std::cout<< "Generation: " << generation << "\n"; 
    auto distribCluster = population[0].getChromosome().first;
    auto clusters = population[0].getChromosome().second;
    std::cout<< "TaC: " << C(datos, clusters);
    std::cout<< "\nInf: " << infeasibility(matrizRest, distribCluster);
    std::cout<< "\nFun: " << population[0].getFitness();
    std::cout<< "\nTmp: " << duracion.count()  ;

    return population[0];
}

Individual AM_2(float porBLS, int interBLS, Individual(Individual::*fp)(Individual), std::vector<std::vector<float>> &datos, std::vector<std::vector<int>> &matrizRest, int &numClusters, float &LAMBDA)
{
    std::chrono::high_resolution_clock::time_point inicial = std::chrono::high_resolution_clock::now();
    // std::cout << "INICIO\n";
    int generation = 0; 
    float Pc = 0.7;
    float Pm = 0.001;

    int iter=1;
    
    float expectedCrosses = Pc * POPULATION_SIZE_AM;

    int porcentajeBLS = porcentajeBLS;
    int intervaloBLS = intervaloBLS;

    std::vector<Individual> population; 
    // Realizamos un elemento con este constructor para establecer las variables de clase
    std::vector<int> gnome = create_genome(datos.size(), numClusters);
    population.emplace_back(Individual(gnome, datos, matrizRest, numClusters, POPULATION_SIZE_AM, LAMBDA));


    // std::cout << "----------------------- SOLUCION INICIAL: --------------------------------\n";
    // std::cout<< "Funcion objetivo: " << population[0].getFitness() << "\n";
    // auto distribCluster = population[0].getChromosome().first;
    // std::cout<< "Infeasibility: " << infeasibility(matrizRest, distribCluster) << "\n";
    // std::cout<< "----------------------------------------------------------------------------" << "\n"; 

    // create initial population 
    for(int i = 1;i<POPULATION_SIZE_AM;i++) 
    { 
        gnome = create_genome(datos.size(), numClusters);
        population.emplace_back(Individual(gnome)); 
    } 
    
    int evaluaciones = 0;
    while(evaluaciones < 100000) 
    { 

        // sort the population in increasing order of fitness score 
        sort(population.begin(), population.end()); 

        if(iter == intervaloBLS) //Aplicamos BLS
        {
            iter = 0;
            float expectedBLS = porcentajeBLS * POPULATION_SIZE_AM;

            for(int i=0; i<expectedBLS; ++i)
            {
                BLS(population[i], 0.1*datos.size(), evaluaciones);
            }
        }
  
        Individual best = population[0];
  
        // Otherwise generate new offsprings for new generation 
        std::vector<Individual> new_generation; 

        // std::cout << "SELECCION\n";
        /********************* SELECCION: *******************
         * AGG: Tantos torneos como usuarios.
        */
        //AGG
        
        std::vector<int> parents; //Indice de los padres en la poblacion
        int random1, random2;
        for(int i=0; i < POPULATION_SIZE_AM; ++i)
        {
            do
            {
                random1 = Randint(0, POPULATION_SIZE_AM -1);
                random2 = Randint(0, POPULATION_SIZE_AM -1);
            } while (random1 == random2);

            parents.push_back(selection(random1,random2, population));
        }

        // std::cout << "CRUCE\n";
        /********************* CRUCE: *******************
         * Nº esperado cruces = Pc · M/2
         * Pc es 0.7 en AGG y 1 en AGE (Siempre se cruzan)
         * AQUI ES AGG, ya que Pc = 0.7
        */
        std::vector<Individual> descendencia;
        int lastParent;
        for(int i = 0;i < expectedCrosses; i+=2) 
        { 
            //Emparejamiento fijo (0-1, 2-3, ...)
            /************************************/
            Individual parent1 = population[parents[i]]; 
            Individual parent2 = population[parents[i+1]];
            lastParent = i+1;
            /************************************/
            Individual offspring = (parent1.*fp)(parent2);
            // Individual offspring = parent1.mateSF(parent2);
            ++evaluaciones; //Cada vez que creamos un hijo, realizamos una evaluación
            if(offspring.repair()) ++evaluaciones; /** REPARACION: **/  //Si reparamos, hemos recalculado f
            descendencia.push_back(offspring);
            
            offspring = (parent2.*fp)(parent1);
            // offspring = parent2.mateSF(parent1);
            ++evaluaciones; //Cada vez que creamos un hijo, realizamos una evaluación
            if(offspring.repair()) ++evaluaciones; /** REPARACION: **/ 
            descendencia.push_back(offspring);

        } 
        // std::cout << "REEMPLAZAMIENTO\n";
        /******************** REEMPLAZAMIENTO: *******************
         * AGG: La descendencia sustituye a la actual. Para conservar
         * el elitismo, si la mejor solución de la generación anterior
         * no sobrevive, sustituye directamente a la peor de la nueva.
        */

        new_generation = descendencia;
        int len = POPULATION_SIZE_AM - descendencia.size();
        //Como solo generamos 0.7 * POPULATION_SIZE_AG descendientes, rellenamos con padres.
        for(int i=0;i<len;++i)
        {
            // Cogemos a partir del padre nº expectedCrosses (al cruzar solo nos quedamos con los padres desde
            // 0 hasta expectedCrosses-1)
            new_generation.push_back(population[parents[lastParent+1 + i]]);
        }

        // std::cout << "MUTACION\n";
        /******************** MUTACION: *******************
            Tenemos (Pc * nº_genes_total) mutaciones que hacer.
            Generamos un aleatorio entre 0 y num_genes * num_cromosomas
            Cambiamos el gen aleatorio mod num_genes del cromosoma
            aleatorio mod num_cromosomas.
        */

        int numGenesTotal = POPULATION_SIZE_AM * datos.size();
        float expectedMutations = Pm * numGenesTotal;
        for(int i=0; i < expectedMutations; ++i)
        {
            /*
            Generas número aleatorio entre 0 y numGenesTotal
            Cromosoma a cambiar = n mod descendencia.size()
            Gen a cambiar = n mod numGenes en un cromosoma
            */
            int random, chromosome, gen;
            do
            {
                random = Randint(0, numGenesTotal);
                chromosome = random % new_generation.size();
                gen = random % datos.size();

            }while(new_generation[chromosome].mutate(gen) == false);
            ++evaluaciones; //Cuando mutamos, actualizamos f

        }
        // std::cout << "ELITISMO\n";
        /******************* ELITISMO: ***********************
         *  AGG: Si perdemos a best en el reemplazamiento, lo metemos por
         * el peor elemento de la nueva generación
        */ 

       if(findBool(new_generation, best) == false) //Si no está best
       {
           float max=0;
           std::vector<Individual>::iterator worstElement;
           for(auto it = new_generation.begin(); it != new_generation.end(); ++it)
           {
               if((*it).getFitness() > max)
               {
                   max=(*it).getFitness();
                   worstElement = it;
               }
           }

            new_generation.erase(worstElement);
            new_generation.push_back(best);
       }

        population = new_generation; 
        generation++; 
        ++iter;
    } 

    std::chrono::high_resolution_clock::time_point final = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duracion = std::chrono::duration_cast<std::chrono::duration<double>>(final-inicial);

    // sort the population in increasing order of fitness score 
    sort(population.begin(), population.end());

    // std::cout<< "Generation: " << generation << "\n"; 
    auto distribCluster = population[0].getChromosome().first;
    auto clusters = population[0].getChromosome().second;
    std::cout<< "TaC: " << C(datos, clusters);
    std::cout<< "\nInf: " << infeasibility(matrizRest, distribCluster);
    std::cout<< "\nFun: " << population[0].getFitness();
    std::cout<< "\nTmp: " << duracion.count()  ;

    return population[0];
}