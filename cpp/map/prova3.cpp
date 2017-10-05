#include <map>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>



int main()
{
  std::map<int, float> mappa;
  for (int i = 0; i<20; i+=2)
    {
      mappa.insert(pair<int,float>(i,i*i));
    }


  std::map<int, float>::iterator it = mappa.begin();
  for (int i=0; i<4; ++i) ++it;
/*
  cout << "find: " << find(mappa.begin(),mappa.end(),4)->first << endl;
*/
  mappa.erase(it,mappa.end());
  cout << mappa.size() << endl;
  
 // morale: find non funziona con una mappa
 
 
  return 0;
}
