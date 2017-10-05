#include "lecture.h"
#include <string>

int main()
{
  lecture leggo("connections.dat");
  string riga;
  for (int i=0; i<3; ++i) 
    {
      riga = leggo.GetLine();
      cout << riga << "\n";
    }
  cout << endl;
}
