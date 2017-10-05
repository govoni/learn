// c++ -o prova04 prova04.cpp
// http://www.sw-at.com/blog/2011/03/23/popen-execute-shell-command-from-cc/
// FUNZIONA

#include <iostream>
#include <stdio.h>

using namespace std;

int main () 
{
    FILE *in;
    char buff[512] ;

    if (!(in = popen ("ls -sail", "r")))
      {
        return 1;
      }

    while (fgets (buff, sizeof (buff), in)!=NULL)
      {
        cout << buff;
      }
    pclose (in) ;
 
    return 0;
}
