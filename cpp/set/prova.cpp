#include <set>
#include <iostream>

int main()
{
  std::set<double> insieme_set;
  for (int i = 0; i<10; ++i)
    {
      insieme_set.insert(2*i);
    }
    /*
  cout << "c-like iteration:\n";
  for (int i = 0; i<insieme_set.size(); ++i)
    {
      cout << insieme_set[i] << "\t";
    }
    */
  cout << "\nc++-like iteration:\n";
  for (set<double>::iterator it = insieme_set.begin();
      it != insieme_set.end();
      ++it)
    {
      cout << *it << "\t";
    }
//  cout << insieme_set.back() << endl;

/*
  cout << "\nc++-like iteration, non tutto:\n";
  for (set<double>::iterator it = insieme_set.begin();
      it != insieme_set.end() - 1;
      ++it)
    {
      cout << *it << "\t";
    }
*/

  set<double>::iterator it = insieme_set.begin();
 
 for (int i=0; i<3; i++) it++;
 cout << "\nquattro: " << *it <<"\n"; 

 cout << "rend(): " << *insieme_set.rend() << "\n";
 cout << "rbegin(): " << *insieme_set.rbegin() << "\n";
// cout << "last(): " << *insieme_set.last() << "\n";
 cout << "end()--: " << *(--insieme_set.end()) << "\n";


 cout << "secondo set creato\n";
 std::set<double> insieme_set2;
/*
  for (int i = 0; i<10; ++i)
    {
      insieme_set2.insert();
      *(insieme_set2.begin()+i) = 3*i;
    }
    */
  for (set<double>::iterator it = insieme_set2.begin();
      it != insieme_set2.end();
      ++it)
    {
      cout << *it << "\t";
    }
  

  cout << insieme_set.size() << endl;
  return 0;
}
