#include "lecture.h"
#include <fstream>

int main()
{
  ifstream leggo("connections.dat");
  cout << leggo.eof() << endl;
}
