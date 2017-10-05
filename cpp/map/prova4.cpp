#include <map>
#include <iostream>
//#include <cstdio>
//#include <cstdlib>
//#include <algorithm>

int main()
{
  std::map<int, float> mappa;
  for (int i = 0; i<20; i+=2)
    {
      mappa.insert(pair<int,float>(i,i*i));
    }

  std::cout << "dopo: " << (mappa.lower_bound(5))->first << std::endl;
  std::cout << "prima: " << (--(mappa.upper_bound(5)))->first << std::endl;

  return 0;
}
