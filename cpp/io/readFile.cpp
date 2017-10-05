#include "readFile.h"
#include <vector>
#include <iostream>

int main ()
{
  std::vector<int> lista ;
  extract (&lista,"testFile.txt") ;
  for (std::vector<int>::const_iterator it = lista.begin () ;
       it != lista.end () ;
       ++it)
       {
         std::cout << *it << "\t" ;
       }
  std::cout << std::endl ;
}