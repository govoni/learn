// reading a text file

#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h> 

int main () {
  double num1=100;
  double num2=100;
  char buffer[256];
  ifstream examplefile ("example.txt");
  if (! examplefile.is_open())
  { cout << "Error opening file"; exit (1); }   
  while (! examplefile.eof() )
  {
    examplefile.getline (buffer,100);
    sscanf(buffer,"%g %g\n",num1,num2);
    cout << num1 << '\t' << num2 << endl;
  }
  return 0;
}
