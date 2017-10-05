#include <iostream>
#include <string>

template <class T> 
void rileggo ()
{
  T lettura ;
  std::cin >> lettura ;
  std::cout << lettura << "\n" ;
}


template <class T> 
T somma (T uno, T due)
{
  return uno + due ;
}

int main ()
{
  std::cout << "leggo e riscrivo, inserisci\n" ;
  rileggo<int> () ;
  rileggo<std::string> () ;
  
  int ia = 2 ;
  int ib = 4 ;
  std::cout << somma (ia, ib) << "\n" ;

  double da = 2 ;
  double db = 4 ;
  std::cout << somma (da, db) << "\n" ;

//  somma (ia, db) ;

  std::cout << somma<int> (ia, db) << "\n" ;

  return 0 ;
}

