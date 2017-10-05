#include <map>
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


int main()
{
  std::map<int, float> mappa;
  for (int i = 0; i<10; ++i)
    {
      mappa[rand()]=i*i;
    }
  for (map<int,float>::iterator it = mappa.begin();
      it != mappa.end();
      ++it)
    {
       cout << it->first << "\t" << it->second << "\n";
    }
  cout << mappa.size() << endl;

  std::map<int, float>::iterator it = mappa.begin();
  for (int i=0; i<4; ++i) ++it;
  cout << "find: " << find(mappa.begin(),mappa.end(),4)->first << endl;
  mappa.erase(it,mappa.end());
  cout << mappa.size() << endl;

// esercizio con passing

  map <int, passing> secondaMappa;

  int MAX = 10;
  for (int i=0; i<MAX; ++i)
    {
      passing dummy = CreatePassing();
      if ( ! secondaMappa.count(i) )
	{
	  // _population[beauty] = dummy;  // metodo dispendioso mnemonicamente 
	  secondaMappa.insert(
	    std::map<int,passing>::value_type(i,dummy)
	  );
	}
      else
	{
	  secondaMappa[i] = dummy;
	  --i;
	}
    }
 
  return 0;
}
