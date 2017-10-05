// c++ -o prova03 prova03.cpp

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std ;

int main ()
{
  FILE* pipe = popen ("touch pippo", "r") ;
  if  (pipe)
    {
      char buffer[128] ;
      while (!feof (pipe))
        {
          if (fgets (buffer, 128, pipe) != NULL) {}
        }
      pclose (pipe) ;
      buffer[strlen (buffer)-1] = '\0' ;
    }

  return 0  ;
}

