#include <iostream> 
#include <vector> 
//#include <iterator> 
#include <algorithm> 

int main() 
{ 
  int cont = 0;
  int N = 10;
  std::vector<int> v;
  std::vector<int>::size_type capacity = 0;
  //v.reserve(200);
  for (int i=0; i<N; ++i)
    {
      v.push_back(i); 
      if (capacity < v.capacity())
	{
	  capacity = v.capacity();
	  std::cout << capacity << "\t";
	}
    }

std::cout << endl;
  
for(std::vector<int>::iterator it = v.begin();
   it != v.end();
   ++it)
  {
     std::cout << *it << "\t";
  }

std::cout << endl;

v.insert(v.begin()+5,20); // faccio qualcosa di dannoso
std::replace(v.begin(),v.end(),20,30);

/*
for(std::vector<int>::iterator it = v.begin();
   it != v.end();
   ++it)
  {
     std::cout << *it << "\t";
     if (*it == 5) 
       v.insert(it,20); // faccio qualcosa di dannoso
  }
*/

std::cout << endl;
  
for(std::vector<int>::iterator it = v.begin();
   it != v.end();
   ++it)
  {
     std::cout << *it << "\t";
  }

std::cout << endl;
}
