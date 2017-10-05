#include "pulse.h"
#include <iostream>

int main()
{
  NNpulse F;
  
  // entrambi i metodi che seguono funzionano:
  
//  double max = F(10.);
//  double max = NNpulse()(10.);
	
  double Amax = 10.;
  double Tmax = 5.;
  for (int j=0; j<Amax; j++)
    {
      for (int i=0; i<50; i++)
	{
//	  if (F(i) > j) cout << "*";
	  if (NNpulse()(i) > j) cout << "*";
	  else cout << " ";
	}
      cout << "\n";
    }
}
