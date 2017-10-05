#include "lecture.h"
#include <string>
#include <vector>

int main()
{
  vector<int> contenuto;
  ifstream filein("connections.dat");
  while (!filein.eof()) {
      leggiRiga<int> (filein,&contenuto);
      for (vector<int>::iterator it = contenuto.begin();
	   it != contenuto.end();
	   ++it)
	{
	  cout << it;
	}
      cout << endl;
  }
}
