#include <stdio.h>
#include <fstream.h>

int main () {
  ofstream examplefile ("example.txt");
  char* str;
  double num1=100;
  double num2=100;
  sprintf(str,"%g %g\n",num1,num2);
  if (examplefile.is_open())
  {
    examplefile << "str";
    examplefile.close();
  }
  return 0;
}
