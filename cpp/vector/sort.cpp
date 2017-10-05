#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>

struct stampa: public std::unary_function<int, void>
{
  void operator() (int& x)
    {
      std::cout << x << "\n" ;
    }
} ;

struct mySort: public std::binary_function<int, int, bool>
{
  bool operator() (int x, int y)
    {
      return x < y ;
    }
} ;

struct myEq: public std::binary_function<int, int, bool>
{
  bool operator() (int x, int y)
    {
      return x == y ;
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
  for_each (caos.begin (), caos.end (), stampa ()) ;

  sort (caos.begin (), caos.end (), mySort ()) ;
  std::cout << "\n----" << std::endl ;
  for_each (caos.begin (), caos.end (), stampa ()) ;

  sort (caos.rbegin (), caos.rend (), mySort ()) ;
  std::cout << "\n----" << std::endl ;
  for_each (caos.begin (), caos.end (), stampa ()) ;

  std::vector<int>::iterator new_end = unique (caos.begin (), caos.end (), myEq ()) ;
  std::cout << "\n----" << std::endl ;
  for_each (caos.begin (), new_end, stampa ()) ;
  
  
//  http://www2.roguewave.com/support/docs/leif/sourcepro/html/stdlibref/copy.html
//    // Use insert iterator to copy into empty vector.
//    std::copy(v1.begin(), v1.end(), std::back_inserter(v4));

//  // Copy all four vectors to cout.
//  std::ostream_iterator<std::vector<int>::value_type, char,
//           std::char_traits<char> > out(std::cout, " ");
//
//  std::copy (caos.begin () , caos.end () , out) ;
//  std::cout << std::endl ;
  
}

