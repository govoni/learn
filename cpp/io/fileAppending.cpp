#include <iostream>
#include <fstream>
//#include <cstdio>

int main ()
{
  char fil[80] = "test.txt" ;
//  sprintf (fil,"test.txt") ;
//  std::fstream outfile (fil,std::ios_base::out | std::ios_base::app) ;
  std::ofstream outfile (fil,std::ios_base::app) ;
//  std::fstream outfile (fil) ;
  if (!outfile) 
    {
      std::cout << "WARNING cannot open " << fil << " for appending\n";
      outfile.open (fil,std::ios_base::out) ;
      if (!outfile) 
        {
          std::cerr << "ERROR cannot open " << fil << " for output\n";
          exit (-1) ;
        }
    }
  outfile << "pippapippa\n" ;
  outfile << "pippapippa" << "\n" ;
  outfile.close () ;
}


