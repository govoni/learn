#include <vector>
#include <iostream>

void passaggio(std::vector<double>::const_iterator it)
{
   cout << *(--it) << endl;
}

int main()
{
  std::vector<double> vettore;
  for (int i = 0; i<10; ++i)
    {
      vettore.push_back(2*i);
      passaggio(vettore.end());
      cout << vettore.back() << "<-" << endl;
    }

}
