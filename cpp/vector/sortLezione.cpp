#include <vector>
#include <iostream>

 #include <algorithm>
 #include <functional>

struct stampa: 
   public std::unary_function<double, void>
{
  void operator() (double& x)
    {
      std::cout << x << "\n" ;
    }
} ;


 struct mySort: 
 public std::binary_function<int, int, bool>
 {
   bool operator() (int x, int y)
     {
       return x < y ;
     }
 } ;


int main ()
{
  std::vector<int> caos ;
  caos.push_back (1) ;
  caos.push_back (3) ;
  caos.push_back (9) ;
  caos.push_back (6) ;
  caos.push_back (2) ;
  caos.push_back (8) ;
  caos.push_back (6) ;
  
  for_each (vectorDiReali.begin (), 
            vectorDiReali.end (), 
            stampa ()) ;

  sort (caos.begin (), caos.end (), mySort ()) ;

  sort (vectorDiReali.begin (), 
        vectorDiReali.end ()) ;

  sort (vectorDiReali.begin (), 
        vectorDiReali.end (), 
        mySort ()) ;

  std::cout << std::endl ;

  for_each (caos.begin (), caos.end (), stampa ()) ;
  
}

