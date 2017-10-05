#include <iostream>
// pass a function to a funxtion

double esegui (double x, int k, double f (double, int)) ;

double func1 (double x, int k) ;
double func2 (double x, int k) ;

int main ()
{
 std::cout << "hello" << std::endl ; 
 std::cout << "1: " << esegui (10,2,func1) << std::endl ; 
 std::cout << "2: " << esegui (10,2,func2) << std::endl ; 
 std::cout << "bye" << std::endl ; 


}

double func1 (double x, int k) 
{
  return k*x ;
}

double func2 (double x, int k) 
{
  return k+x ;
}

double esegui (double x, int k, double f (double, int)) 
{
  return f (x,k) ;

}
