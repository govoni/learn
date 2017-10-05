#include <stdio.h>
#include <fstream.h>
#include <stdlib.h>

int main()
{
  FILE *connections;
    if ((connections = fopen ("connections.dat","r")) == NULL)
    {
      cerr << "not possible to open the connections file\n";
      exit(1);
    }
  FILE *parameters;
  if ((parameters = fopen ("parameters.dat","r")) == NULL)
    {
      cerr << "not possible to open the parameters file\n";
      exit(1);
    }
  
  int dbuffer=0;
  fscanf(connections,"%d",dbuffer);
  cout << dbuffer << endl;
  fscanf(parameters,"%d",dbuffer);
  cout << dbuffer << endl;
  
  fclose (connections);  
  fclose (parameters);  
}
