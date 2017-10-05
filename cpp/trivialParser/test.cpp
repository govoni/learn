#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <cmath>

#include "TrivialParser.h"


int main (int argc , char* argv[])
{
  if (argc < 2)
    std::cerr << "usage:\n" << argv[0]
              << " config_file\n" ;
  trivialParser parser (argv[1]) ;
  parser.getVal ("numOfVars") ;

  return 0 ;
}
