//prova passaggio argomenti alle funzioni
#include <iostream>

int* dnumber()
{
  int *num; 
  *num= 7;
  return num;
}

void* number()
{
  int *num; 
  *num= 7;
  return (void*) num;
}

int main()
{
  int *uno;
  *uno = 5;
  uno = (int*) number();
  cout << *uno << endl;
  double *due;
  due = (double*) number();
  due = (double*) dnumber();  // <- accetta anche questo!!
//  cout << *due << endl;
/*
  double* due;
  due = uno;
  due = (int*) uno;

// prova.cpp: In function `int main ()':
// prova.cpp:9: cannot convert `int *' to `double *' in assignment

*/
}
