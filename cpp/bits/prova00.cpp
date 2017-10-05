// c++ -o prova00 prova00.cpp

#include <iostream>
#include <bitset>

using namespace std ; 

int main (int arcg, char ** argv)
{
  // size of types
  uint32_t word = 1026 ;
  cout << sizeof (word) << endl ;
  short shortword ;
  cout << sizeof (shortword) << endl ;
  char charword ;
  cout << sizeof (charword) << endl ;
  
  // masks
  uint32_t mask = 0 ;
  cout << " 0          " << mask << "\t\t" << bitset<32> (mask) << endl ;

  mask = 0x3 ;
  cout << " 0x3        " << mask << "\t\t" << bitset<32> (mask) << endl ;

  mask = 0xF ;
  cout << " 0xF        " << mask << "\t\t" << bitset<32> (mask) << endl ;
                       
  mask = 0x3F ;
  cout << " 0x3F       " << mask << "\t\t" << bitset<32> (mask) << endl ;
                       
  mask = 0xFF ;        
  cout << " 0xFF       " << mask << "\t\t" << bitset<32> (mask) << endl ;
                       
  mask = 0xFFF ;       
  cout << " 0xFFF      " << mask << "\t" << bitset<32> (mask) << endl ;
                       
  mask = 0xFFFF ;      
  cout << " 0xFFFF     " << mask << "\t" << bitset<32> (mask) << endl ;
                       
  mask = 0xFFFFF ;     
  cout << " 0xFFFFF    " << mask << "\t" << bitset<32> (mask) << endl ;
                       
  mask = 0xFFFFFF ;    
  cout << " 0xFFFFFF   " << mask << "\t" << bitset<32> (mask) << endl ;
                       
  mask = 0xFFFFFFF ;   
  cout << " 0xFFFFFFF  " << mask << "\t" << bitset<32> (mask) << endl ;
                       
  mask = 0x3FFFFFFF ;  
  cout << " 0x3FFFFFFF " << mask << "\t" << bitset<32> (mask) << endl ;
                       
  mask = 0xFFFFFFFF ;  
  cout << " 0xFFFFFFFF " << mask << "\t" << bitset<32> (mask) << endl ;

  cout << "           \t\t10987654321098765432109876543210\n" ;



  mask = 1026 ;
  cout << " 1026      " << mask << "\t" << bitset<32> (mask) << endl ;

  // shifts
  word = 1000 ; 
  cout << "shift " << word << endl ;
  uint32_t sword = word>>1 ; 
  cout << "shift " << word << endl ;
  cout << "shift " << sword << endl ;
  word << 16 ;  
  cout << "shift " << word << endl ;
  
  return 0 ;

}