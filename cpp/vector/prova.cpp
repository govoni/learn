#include <vector>
#include <iostream>

int main()
{
  std::vector<float> vettore;
  cout << &vettore.back() << endl;
  if (vettore.end() != NULL) cout << vettore.end() << endl; else cout << "NULL" << endl;
  cout << vettore.end()-1 << endl;
  for (int i = 0; i<10; ++i)
    {
      vettore.push_back(2*i);
    }
  cout << "c-like iteration:\n";
  int j =0;
  for (int i = 0; i<vettore.size(); ++i, ++j)
    {
      cout << vettore[i] << "\t";
    }
  cout << endl << j << endl;
  cout << "\nc++-like iteration:\n";
  for (vector<float>::iterator it = vettore.begin();
      it != vettore.end();
      ++it)
    {
      cout << *it << "\t";
    }
  cout << *vettore.end() << endl;
  cout << vettore.back() << endl;

  cout << "\nc++-like iteration, non tutto:\n";
  for (vector<float>::iterator it = vettore.begin();
      it != vettore.end() - 1;
      ++it)
    {
      cout << *it << "\t";
      cout << it - vettore.begin() << " <--\n";
    }
  vector<float>::iterator it = vettore.begin();
  cout << "\nquattro: " << *(it + 3) <<"\n"; 

  std::vector<float> vettore2;
  for (int i = 0; i<10; ++i)
    {
      vettore2.push_back();
      *(vettore2.begin()+i) = 3*i;
    }
  for (vector<float>::iterator it = vettore2.begin();
      it != vettore2.end();
      ++it)
    {
      cout << *it << "\t";
    }
  

  cout << vettore.size() << endl;

  cout << vettore2.size() << endl;
  vettore2.erase(vettore.end());
  cout << vettore2.size() << endl;
  return 0;
}
