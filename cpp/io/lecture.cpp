#include <stdio.h>
#include <fstream.h>
#include <stdlib.h>
#include <cassert>

int main()
{
   
  int dbuffer = 0;
  float fbuffer = 0.;
  ifstream *connections = new ifstream("connections.dat");
  assert(connections != NULL);
//  *connections >> dbuffer >> "\n"; 
  connections->seekg(0,ios::beg); 
  *connections >> fbuffer;
  cout << fbuffer << endl;
  connections->seekg(0.5*sizeof(float),ios::beg); 
  *connections >> fbuffer;
  cout << fbuffer << endl;
  connections->seekg(2*sizeof(float),ios::beg); 
  *connections >> fbuffer;
  cout << fbuffer << endl; 
  delete connections;
}
