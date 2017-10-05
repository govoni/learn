// reading a text file
// line by line

//#include <cstdio>
//#include <iostream>
#include <fstream>
#include <sstream>
//#include <cstdlib> 

int main (int argc, char**argv)
{
  double num1=100;
  ifstream examplefile (argv[1]);
  if (! examplefile.is_open())
  { cout << "Error opening file"; exit (1); }   
  while (! examplefile.eof() )
    {
      char buffer[256];
      examplefile.getline (buffer,256);
      stringstream linea(buffer);
      string nome;
      linea >> nome;
      cout << nome << "\t";
      while (!linea.eof())
	{
          num1 = -1;
	  linea >> num1;
	  if (num1 != -1) cout << num1 << "\t";
	}
      cout << "\n";
    }
  cout << endl;
  return 0;
}
