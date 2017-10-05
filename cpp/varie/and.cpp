#include <iostream>

int main ()
{
  for (int i=0 ; i<9 ; ++i)
    {
      std::cout << i << " " << (i & 2) << std::endl ;
    }
  return 0 ;
}