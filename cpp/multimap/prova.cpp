#include <map>
//#include <pair>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>


class passing{

public:

  passing(int val): var1(val) {;}
  int Get1() const {return var1;}

private:

  int var1;

};


passing& CreatePassing ()
{
   passing dummy(random());
   return dummy;
}


//-------------------------------------------------------------//


int main()
{
  std::multimap<int, float> mappa;
  for (int i = 0; i<10; ++i)
    {
      mappa.insert(std::multimap<int,float>::value_type(i,i*i));
    }

  mappa.insert(std::multimap<int,float>::value_type(5,4.4));
  for (multimap<int,float>::iterator it = mappa.begin();
      it != mappa.end();
      ++it)
    {
       cout << it->first << "\t" << it->second << "\n";
    }
  cout << mappa.size() << endl;

  typedef std::multimap<int,float>::iterator iter; 
  pair<iter,iter> coppia = mappa.equal_range(5);  

  for (multimap<int,float>::iterator it = coppia.first;
      it != coppia.second;
      ++it)
    {
       cout << it->first << "\t" << it->second << "\n";
    }

//  std::multimap<int, float>::iterator it = mappa.begin();
//  for (int i=0; i<4; ++i) ++it;
//  cout << "find: " << find(mappa.begin(),mappa.end(),4)->first << endl;
//  mappa.erase(it,mappa.end());
//  cout << mappa.size() << endl;

// esercizio con passing

  multimap <int, passing> secondaMappa;

  int MAX = 10;
  for (int i=0; i<MAX; ++i)
    {
      passing dummy = CreatePassing();
      secondaMappa.insert(std::multimap<int,passing>::value_type(i,dummy));
    }
 
  return 0;
}
