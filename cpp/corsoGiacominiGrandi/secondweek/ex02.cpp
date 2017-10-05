#include <iostream> 
#include <vector> 
#include <set>
#include <list>
#include <multiset.h>
#include <iterator> 
#include <algorithm> 
#include <functional> 

int main() 

{ 
  char c;
  int cont = 0;
  std::vector<char> v;
  std::set<char> s;
  std::list<char> l;
  std::list<char> l1;
  std::multiset<char> ms;
  while(cin >> c) { 
//    if (c=='q') break;
    v.push_back(c); 
    s.insert(c);
    l.push_back(c);
    ms.insert(c);
    l1.push_front(c);
    ++cont;
  }
//   if (std::find_if(v.begin(), v.end(), 
//     bind2nd(std::greater<char>(), 5)) 
//     != v.end()) { 
//       std::cout << "found an element greater than 5\n"; 
//   } 
//  std::replace(v.begin(), v.end(), *v.begin(), 8); 
  std::sort(v.begin(), (v.begin()+(cont/2)) ); 
  std::copy(v.begin(), v.end(), ostream_iterator<char>(cout, "\t"));
  std::cout << endl; 
  std::copy(s.begin(), s.end(), ostream_iterator<char>(cout, "\t"));
  std::cout << endl; 
  std::copy(l.begin(), l.end(), ostream_iterator<char>(cout, "\t"));
  std::cout << endl; 
  std::copy(l1.begin(), l1.end(), ostream_iterator<char>(cout, "\t"));
  std::cout << endl; 
  std::copy(ms.begin(), ms.end(), ostream_iterator<char>(cout, "\t"));
  std::cout << endl; 
}
