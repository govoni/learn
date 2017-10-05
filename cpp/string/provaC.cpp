/*
c++ -o provaC provaC.cpp 
*/

#include <iostream>
#include <sstream>
#include <string>

int main (int argc, char** argv)
{
 
  int decisione = atoi (argv[1]) ; 
  char parola[20] ;

  if (decisione == 0) strcpy (parola, "zero") ;
  else                strcpy (parola, "uno") ;
  printf ("%s\n",parola) ;

  printf ("------------------\n") ;

  switch (decisione)
    {
      case 0 : 
        strcpy (parola, "zero") ;
        break ;
      case 1 : 
        strcpy (parola, "uno") ;
        break ;
      default : strcpy (parola, "default") ;
    }
  printf ("%s\n",parola) ;
  return 0 ;
}  
