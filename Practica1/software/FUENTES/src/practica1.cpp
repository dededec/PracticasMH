#include "random.h"
#include "greedy.h"
#include "objetivo.h"
#include "busquedaLocal.h"


#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void datosAMatriz(ifstream &archivo, vector<vector<int>> &matriz);
void datosAMatriz(ifstream &archivo, vector<vector<float>> &matriz);

template<class T>
std::ostream& operator<<(std::ostream & os, const std::vector<T> &V);


int main(int argc, char *argv[])
{
    int versionCOPKM = 0;
    if(argc < 5)
    {
        cerr << "Error en el nº de argumentos" << endl
        << "(Ruta del programa) (archivoDatos) (archivoRestricciones) (numeroClusters) (Semilla) OPCIONAL(VersionGreedy)OPCIONAL" << endl << flush;
        return -1;
    }
    else if(argc == 6) versionCOPKM = atoi(argv[5]);
    

    Set_random(atoi(argv[4]));
    srand(Get_random());

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

    datosAMatriz(archivoRestricciones,matrizRest);
    datosAMatriz(archivoDatos, datos);

    

    int numClusters = atoi(argv[3]);
    vector<vector<int>> clusters;
    vector<int> distribCluster;
    vector<vector<float>> centroides;
    const float LAMBDA = calcularLambda(datos,matrizRest);

    // std::cout << "********************************" << std::endl;
    // std::cout << "LAMBDA: " << LAMBDA << std::endl;
    // std::cout << "********************************" << std::endl;


    busquedaLocal(datos, matrizRest, numClusters, distribCluster, clusters, centroides, LAMBDA);
    // COPKM(datos, matrizRest, numClusters, distribCluster, clusters, centroides, LAMBDA);
  
}


/*************************************************************************************************************/
// IMPLEMENTACIÓN DE FUNCIONES
/*************************************************************************************************************/

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