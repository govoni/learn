#include <vector>
#include <iostream>

using namespace std ;
int main()
{
  std::vector<float> vettore;
  for (int i = 0 ; i < 5 ; ++i) vettore.push_back (i) ;
  std::vector<float> * another = new std::vector<float> (vettore) ;
  cout << another->size() << endl ;
  cout << another->at (3) << endl ;
  return 0;
}
