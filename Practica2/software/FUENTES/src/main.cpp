//Code from https://www.geeksforgeeks.org/genetic-algorithms/
//Author: Atul Kumar

// C++ program to create target string, starting from 
// random string using Genetic Algorithm 
  

#include "individual.h"
#include "genetico.h"
#include "objetivo.h"
#include "random.h"
#include "operacionesVector.h"

#include <bits/stdc++.h> 
#include <iostream>
#include <fstream>
#include <string>

using namespace std; 
  
void datosAMatriz(ifstream &archivo, vector<vector<int>> &matriz);
void datosAMatriz(ifstream &archivo, vector<vector<float>> &matriz);
template<class T>
typename std::vector<T>::iterator find(std::vector<T> &v, T &valor);

// // Number of individuals in each generation 
// #define POPULATION_SIZE 50 
  
// Driver code 
int main(int argc, char *argv[]) 
{ 
    if(argc < 5)
    {
        cerr << "Error en el nº de argumentos" << endl
        << "(Ruta del programa) (archivoDatos) (archivoRestricciones) (numeroClusters) (Semilla)" << endl << flush;
        return -1;
    }

    srand((unsigned)(time(0))); 

    vector<vector<float>> datos; //Valores
    vector<vector<int>> matrizRest; //Restricciones

    ifstream archivoDatos, archivoRestricciones;

    archivoDatos.open(argv[1]);
    if (!archivoDatos)
    {
        cerr << "No puedo abrir el fichero " << argv[1] << endl;
        return -1;
    }

    archivoRestricciones.open(argv[2]);
    if(!archivoRestricciones)
    {
        cerr << "No se pudo abrir el fichero " << argv[2] << endl;
        return -1;
    }

    int numClusters = atoi(argv[3]);

    Set_random(atoi(argv[4]));
    srand(Get_random());

    datosAMatriz(archivoRestricciones, matrizRest);
    datosAMatriz(archivoDatos, datos);
    
    float LAMBDA = calcularLambda(datos, matrizRest);

    float porcentajeBLS = 1.0;
    int intervaloBLS = 10;

    
    /*************** AGG: ***************/
    std::cout << "AGGs: ------------------------------------\n";
    cout << "Cruce uniforme:\n";
    AGG(&Individual::mateUN ,datos, matrizRest, numClusters, LAMBDA);
    std::cout << "\n==========================================\n";
    cout << "Cruce segmento fijo:\n";
    AGG(&Individual::mateSF ,datos, matrizRest, numClusters, LAMBDA);

    std::cout << "\n\n==========================================\n";
    std::cout << "==========================================\n\n";

    /*************** AGE: ***************/
    std::cout << "AGEs: ------------------------------------\n";
    cout << "Cruce uniforme:\n";
    AGE(&Individual::mateUN ,datos, matrizRest, numClusters, LAMBDA);
    std::cout << "\n==========================================\n";
    cout << "Cruce segmento fijo:\n";
    AGE(&Individual::mateSF ,datos, matrizRest, numClusters, LAMBDA);
    std::cout << '\n';

    std::cout << "\n==========================================\n";
    std::cout << "==========================================\n\n";

    /*************** AM: ***************/
    std::cout << "AMs(con AGG-SF): -------------------------\n";
    cout << "Se aplica a toda la poblacion:\n";
    AM_1(1.0, 10, &Individual::mateSF, datos, matrizRest, numClusters, LAMBDA);
    std::cout << "\n==========================================\n";
    cout << "Se aplica al 10% de la poblacion:\n";
    AM_1(0.1, 10, &Individual::mateSF, datos, matrizRest, numClusters, LAMBDA);
    std::cout << "\n==========================================\n";
    cout << "Se aplica al 10% de los mejores cromosomas de la poblacion:\n";
    AM_2(0.1, 10, &Individual::mateSF, datos, matrizRest, numClusters, LAMBDA);
}

/*************************************************************************************************************/
// IMPLEMENTACIÓN DE FUNCIONES
/*************************************************************************************************************/

template<class T>
typename std::vector<T>::iterator find(std::vector<T> &v, T &valor)
{
    for(auto it=v.begin(), it2 = v.rbegin(); it != v.end() && it2 != v.rend(); ++it, ++it2)
    {
        if((*it) == valor)
        {
            return it;
        }
        else if( (*it2) == valor )
        {
            return (it2 + 1).base();
        }
    }

    return v.end();
}


void datosAMatriz(ifstream &archivo, vector<vector<int>> &matriz)
{
    string datosLeidos, nuevaLinea;
    matriz.emplace_back(); //push_back(vector<int>())

    while(!archivo.eof())
    {
        getline(archivo,datosLeidos,',');
        
        nuevaLinea.clear();
        
        size_t posicion = datosLeidos.find("\n");
        if(posicion != string::npos)
        {
            nuevaLinea = datosLeidos.substr(posicion,datosLeidos.length()-1);
            datosLeidos.erase(posicion,datosLeidos.length()-1);
        }

        int aux = stoi(datosLeidos);
        matriz[matriz.size()-1].emplace_back(aux);

        if(!nuevaLinea.empty() && nuevaLinea != "\n")
        {
            matriz.emplace_back();
            matriz[matriz.size()-1].emplace_back(stoi(nuevaLinea));
        }        
    }

}


void datosAMatriz(ifstream &archivo, vector<vector<float>> &matriz)
{
    string datosLeidos, nuevaLinea;
    matriz.emplace_back();

    while(!archivo.eof())
    {
        getline(archivo,datosLeidos,',');

        nuevaLinea.clear();
        
        size_t posicion = datosLeidos.find("\n");
        if(posicion != string::npos)
        {
            nuevaLinea = datosLeidos.substr(posicion,datosLeidos.length()-1);
            datosLeidos.erase(posicion,datosLeidos.length()-1);
        }

        float aux = stof(datosLeidos);
        matriz.back().emplace_back(aux);

        if(!nuevaLinea.empty() && nuevaLinea != "\n")
        {
            matriz.emplace_back();
            matriz[matriz.size()-1].emplace_back(stof(nuevaLinea));
        }   
    }
}

template<class T>
std::ostream& operator<<(std::ostream & os, const std::vector<T> &V)
{
	  
	  
	  typename std::vector<T>::const_iterator it;
	  
	  for (it=V.begin(); it!=V.end(); ++it) os << (*it) << " | ";

      os<< std::endl << "********************************************"<<std::endl;
	  
	  return os;
}