#include <iostream>
#include <sstream>
#include <string>

int main()
{
  std::string pippo = "vediamo se " ;
  std::cout << pippo+"funziona" << std::endl ; 
  std::cout << (pippo+"funziona ancora").c_str ()  << std::endl ; 


}
