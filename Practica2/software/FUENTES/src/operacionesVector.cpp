#include "operacionesVector.h"
#include <vector>
#include <cmath>
#include <numeric>


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
    Calcula a-b y lo almacena en resultado
*/
void restaVector(std::vector<float> &a, std::vector<float> &b, std::vector<float> &resultado)
{
    resultado.clear();
    
    for(int i=0; i< a.size(); ++i)
    {
        resultado.emplace_back(a[i] - b[i]);
    }
}

/*
    Calcula el módulo de un vector
*/
float moduloVector(std::vector<float> &v)
{
    return std::sqrt( std::inner_product( v.begin(), v.end(), v.begin(), 0.0 ) );
}

/*
    Calcula la suma de cuadrados de los elementos de un vector
*/
// float sumaCuadradosVector(std::vector<float> &v)
// {
//     return std::inner_product( v.begin(), v.end(), v.begin(), 0.0 );
// }

void borrarPorValor(std::vector<int> &v, int valor)
{
    std::vector<int>::iterator it;
    std::vector<int>::reverse_iterator it2;

    int i=0;
    for(it = v.begin(), it2 = v.rbegin();i<=v.size()/2 && it != v.end() && it2 != v.rend(); ++it, ++it2, ++i)
    {
        if((*it) == valor)
        {
            v.erase(it);
            break;   
        }
        else if( (*it2) == valor )
        {
            v.erase((it2 + 1).base()); //Un reverse_iterator verdaderamente es un iterator; base() nos devuelve ese iterator, pero hay que sumar 1 ya que el reverse tiene un offset respecto al base().
            break;
        }

    }
}

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

// template<class T>
// bool findBool(std::vector<T> &v, T &valor)
// {
//     for(auto it=v.begin(), it2 = v.rbegin(); it != v.end() && it2 != v.rend(); ++it, ++it2)
//     {
//         if(((*it) == valor) || ((*it2) == valor))
//         {
//             return true;
//         }
//     }

//     return false;
// }

template<class T>
int sortedFind(std::vector<T> &v, T &valor)
{
    int L = 0;
    int R = v.size()-1;
    while(L <= R)
    {
        int m = (L+R)/2;
        if(v[m] < valor)
            L = m+1;
        else if(v[m] > valor)
            R = m-1;
        else
            return m;
    }

    return -1;
}