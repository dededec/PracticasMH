#include "random.h"
#include "individual.h"
#include "nba.h"
#include "operacionesVector.h"

#include <bits/stdc++.h> 
#include <iostream>
#include <fstream>
#include <string>
#include <random>

using namespace std; 
  
void datosAMatriz(ifstream &archivo, vector<vector<int>> &matriz);
void datosAMatriz(ifstream &archivo, vector<vector<float>> &matriz);
template<class T>
typename std::vector<T>::iterator find(std::vector<T> &v, T &valor);

int main(int argc, char *argv[]) 
{ 
    if(argc < 5)
    {
        cerr << "Error en el nº de argumentos" << endl
        << "(Ruta del programa) (archivoDatos) (archivoRestricciones) (numeroClusters) (Semilla)" << endl << flush;
        return -1;
    }

    srand(atoi(argv[4])); 

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


    std::chrono::high_resolution_clock::time_point inicial = std::chrono::high_resolution_clock::now();

    Individual solucion = NBADraftBL(datos, matrizRest, numClusters, LAMBDA, 30, 5, 2);
    std::chrono::high_resolution_clock::time_point final = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duracion = std::chrono::duration_cast<std::chrono::duration<double>>(final-inicial);

    auto distribCluster = solucion.getSolucion().first;
    cout << "Solución: \n" << distribCluster << endl;
    auto clusters = solucion.getSolucion().second;
    cout << "\nTasa_C: " << C(datos, clusters) << "\nTasa_Inf: " << infeasibility(matrizRest, distribCluster) << "\nAgg.: " << solucion.getFitness() << "\nT: " << duracion.count() << "\n\n";

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