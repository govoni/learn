#include <iostream> 
#include <vector> 
#include <iterator> 
#include <algorithm> 
#include <functional>


bool is_even(int num)
{
  return num % 2 == 0;
}

bool is_odd(int num)
{
  return ! is_even(num);
}

int main() 
{ 
  int cont = 0;
  int N = 10;
  std::vector<int> v;
  for (int i=1; i<N; ++i)
    {
      v.push_back(i); 
    }
  std::vector<int>::iterator it = find_if(v.begin(),v.end(),not1(ptr_fun(is_even)));
  if (it != v.end()) 
   {
     std::cout << *it << "\n";
   }
  it = find_if(v.begin(),v.end(),is_even);
  if (it != v.end()) 
   {
     std::cout << *it;
   }
  std::cout << endl;
}

