#include <vector>
#include <iostream>
#include "vector2D.h"

int main()
{
  std::vector<vector2D> vettore;
  for (int i = 0; i<10; ++i)
    {
      vettore.push_back(2*i,i);
    }
  cout << "\nc++-like iteration:\n";
  for (vector<vector2D>::iterator it = vettore.begin();
      it != vettore.end();
      ++it)
    {
      cout << "(" << it->x() << "," << it->y() << ")" << "\t";
    }
  return 0;
}
