#include "layer.h"
#include <stdio.h>
#include <fstream.h>

int main()
{
  FILE *connections = fopen ("connections.dat","r");
  FILE *parameters = fopen ("parameters.dat","r");

  NNlayer * zerozero = new NNlayer;

  cout << "zerozero\n";

  NNlayer * zero = new NNlayer(0);
  
  cout << "zero\n";
   
  NNlayer * uno = new NNlayer(0,connections,parameters);
  NNlayer * due = new NNlayer(1,connections,parameters,due);

  cout << "total\n";

  fclose (connections);  
  fclose (parameters);  
}
