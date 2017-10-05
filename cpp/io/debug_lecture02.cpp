#include "lecture.h"
#include <string>
#include <vector>

int main()
{
  vector<int> contenuto;
  leggoFile<int> leggo("connections.dat",&contenuto);
  for (std::vector<int>::iterator it = contenuto.begin();
       it != contenuto.end();
       ++it)
    cout << *it << "\t";
  cout << endl;
}
