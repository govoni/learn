#include "lecture.h"
#include <string>
#include <vector>

int main()
{
  vector<int> contenuto;
  leggiFile<int> ("connections.dat",&contenuto);
  for (std::vector<int>::iterator it = contenuto.begin();
       it != contenuto.end();
       ++it)
    cout << *it << "\t";
  cout << endl;
}
