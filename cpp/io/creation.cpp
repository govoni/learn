#include <stdio.h>
#include <fstream.h>
#include <stdlib.h>
#include <cassert>
#include <string.h>

int main()
{
  int dbuffer = 0;
  float fbuffer = 0.;
  char S_connections[80];
  char S_parameters[80];
  
  cout << endl;
  cout << "insert the connections file name:\n";
  cin >> S_connections;
  cout << endl;
  cout << "insert the parameters file name:\n";
  cin >> S_parameters;
  cout << endl;

  ofstream *connections = new ofstream(S_connections);
  assert(connections != NULL);
  connections->seekp(0,ios::beg); 
  dbuffer = 3; 					// number of layers
  *connections << dbuffer << '\n'; 

  dbuffer = 3; 			                // number of neurons per layer
  *connections << dbuffer << '\t'; 
  dbuffer = 2; 
  *connections << dbuffer << '\t';
  dbuffer = 1; 
  *connections << dbuffer << '\n'; 
  
  dbuffer = 0;					// neuron 
  *connections << dbuffer << '\t'; 
  dbuffer = 1; 
  *connections << dbuffer << '\n'; 

  dbuffer = 1; 					// neuron 
  *connections << dbuffer << '\t'; 
  dbuffer = 2; 
  *connections << dbuffer << '\n'; 

  dbuffer = 0; 					// neuron 
  *connections << dbuffer << '\t'; 
  dbuffer = 1; 
  *connections << dbuffer << '\n'; 

  delete connections;

  cout << S_connections << " done\n";

  ofstream *parameters = new ofstream(S_parameters);
  assert(parameters != NULL);
  parameters->seekp(0,ios::beg); 
  dbuffer = 3; 
  *parameters << dbuffer << '\n';                // number of  layers 

  dbuffer = 3;                                   // number of neurons per layer
  *parameters << dbuffer << '\t';  
  dbuffer = 2; 
  *parameters << dbuffer << '\t';  
  dbuffer = 1; 
  *parameters << dbuffer << '\n';  

  for (int j=0; j<6; j++)                          
    {
      dbuffer = 2;                               // neuron
      *parameters << dbuffer << '\t';   
      fbuffer = 0.5; 
      *parameters << fbuffer << '\t';   
      fbuffer = 0.5; 
      *parameters << fbuffer << '\t';   
      fbuffer = 0.9; 
      *parameters << fbuffer << '\t';   
      fbuffer = 1.0; 
      *parameters << fbuffer << '\n';   
   }

  delete parameters;

  cout << S_parameters <<" done\n";
  
  }
