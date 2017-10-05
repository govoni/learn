#include <iostream>
#include <sstream>
#include <string>

std::string chop(std::string input);


int main()
{
  std::string prova = "pathpat/patpath2/hfile";
  std::cout << chop(prova) << std::endl;
  int to = prova.rfind ("/",prova.size ()) ;
  prova.erase (0,to+1) ;
  std::cout << prova << std::endl;
  
}

std::string chop (std::string input)
{
  int memo = 0;
  for (int i=0; i<input.length(); ++i)
    {
       if (input[i] == '/') memo = i+1;
    }
  return input.erase (0,memo) ;
}
