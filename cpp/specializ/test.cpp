#include "templateClass.h"

#include<iostream>

int main () 
{
  templateClass<double> cicca ;
  std::cout << "setme : " << cicca.setme (14.3) << std::endl ;
  templateClass<int> pippo ;
  std::cout << "setme : " << pippo.fifo (14)->setme (12) << std::endl ;
  std::cout << "leggimi : " << cicca.leggimi () << std::endl ;
  templateClass<float> smart ;
  smart.setme (21.5) ;
  std::cout << "leggimi : " << smart.leggimi () << std::endl ;

}