#include "Array.h"

Array::Array(int N):numero(N)
  {
    vettore = new int(numero);
  }
  
Array::~Array(int N)
  {
    delete [] vettore;
  }
  
int& Array::operator[] (int index)
  {
    if (index < 0 || index >= numero) 
     {
       cout << "Index out of range, returning last element" << endl; 
       cerr << "Index out of range, returning last element" << endl; 
       index = number - 1;  
     }
    return (vettore + index);
  }
