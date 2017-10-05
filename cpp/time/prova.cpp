// tratto da  
// http://www.cplusplus.com


/* clock example: countdown */
#include <stdio.h>
#include <ctime>
#include <iostream>

void wait ( int seconds )
{
  clock_t endwait;
  endwait = clock () + seconds * CLK_TCK ;
  cout << CLK_TCK << "\t";
  cout << clock() << "\t" << endwait << "\t";
  while (clock() < endwait) {}
}

int main ()
{
  int n;
  printf ("Starting countdown...\n");
  for (n=10; n>0; n--)
  {
    printf ("%d\n",n);
    wait (1000);
  }
  printf ("FIRE!!!\n");
  return 0;
}
