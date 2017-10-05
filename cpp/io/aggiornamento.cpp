//#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <string>

int main()
{
  int dbuffer = 0;
  float fbuffer = 0.;
  char S_connections[80];
  char S_parameters[80];
  
/*
  cout << endl;
  cout << "insert the connections file name:\n";
  cin >> S_connections;
  cout << endl;
*/
/*
  ofstream *connections = new ofstream(S_connections);
  assert(connections != NULL);
*/
  string St_connections = "pippero";
  fstream *connections = new fstream(St_connections.c_str(),ios::out);

  connections->seekg(3,ios::beg); 
  *connections << "ciao" << "\t";
  cout << "pausa" << endl;
  cin >> dbuffer;
  *connections << dbuffer << endl; 
  delete connections;

//  cout << S_connections << " done\n";

}
