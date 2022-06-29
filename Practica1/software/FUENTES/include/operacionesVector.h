#ifndef __OPERACIONESVECTOR_PPIO_H
#define __OPERACIONESVECTOR_PPIO_H

#include <vector>
#include <iostream>

template<class T>
std::ostream& operator<<(std::ostream & os, const std::vector<T> &V);

template<class T>
std::ostream& operator<<(std::ostream & os, const std::pair<T,T> &V);

void restaVector(std::vector<float> &a, std::vector<float> &b, std::vector<float> &resultado);
float moduloVector(std::vector<float> &v);
// float sumaCuadradosVector(std::vector<float> &v);
void borrarPorValor(std::vector<int> &v, int valor);

#endif