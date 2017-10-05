// reading binary file

#include <iostream.h>
#include <fstream.h> 
const char * filename = "example.txt"; 

int main () {
  char * buffer;
  long size;
  ifstream file (filename, ios::in|ios::binary|ios::ate);
  size = file.tellg();
  file.seekg (0, ios::beg);
  buffer = new char [size];
  file.read (buffer, size);
  file.close();   
  cout << "the complete file is in a buffer";   
  delete[] buffer;
  return 0;
}
