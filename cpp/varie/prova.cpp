#include <iostream>

int main()
{
  cout << "ecco " << 0xfff << endl;
  int conto = 1;
  for (int i=0; i<10; ++i)
    {
      conto *= 2;
      double provo = (conto) & 0xfff;
      cout << provo << endl;
    }
}
