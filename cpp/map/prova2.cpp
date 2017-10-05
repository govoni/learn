#include <map>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <memory>


class passing{

public:

  passing(int val): var1(val) {;}
  int Get1() const {return var1;}

private:

  int var1;

};


passing* CreatePassing ()
{
   passing* dummy = new passing(random());
   return dummy;
}


int main()
{

  map <int, passing> Mappa;

  int MAX = 10;
  for (int i=0; i<MAX; ++i)
    {
      auto_ptr<passing> dummy(CreatePassing());
      if ( ! Mappa.count(i) )
	{
	  // _population[beauty] = dummy;  // metodo dispendioso mnemonicamente 
	  Mappa.insert(
	    std::map<int,passing>::value_type(i,*dummy)
	  );
	}
      else
	{
	  Mappa[i] = *dummy;
	  --i;
	}
    }
 
  return 0;
}
