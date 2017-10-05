#include <iostream>
#include <vector>

class leaf{

public:

  leaf(const int val):_var1(val) {};
  leaf(const leaf& original):_var1(original._var1) {};
  int Get1() const {return _var1;};

private:

  int _var1;

};


// --------------------------------------------------------------


class plant{

public:
 
  plant::plant(const int val);

//protected:

  plant(const plant& original);
//  plant& operator=(const plant& original);

public:

  ~plant();

  typedef std::vector<leaf*>::iterator iterator;
  typedef std::vector<leaf*>::const_iterator const_iterator;
  iterator begin() {return _branch.begin();}
  const_iterator begin() const {return _branch.begin();}
  iterator end() {return _branch.end();}
  const_iterator end() const {return _branch.end();}

  int Get2() const {return _var2;}

private:

  std::vector<leaf*> _branch;
  int _var2;

};


// --------------------------------------------------------------


// TEST MAIN
// ---------

int main()
{
  int val = 10;
  plant orange(val);
  plant apple(orange);
  cout << "qui\n";
  try 
    {
      throw orange;
    }
  catch (plant& e){};

  cout << "all DONE\n";
  cout << endl;
}


// --------------------------------------------------------------


// plant
// -----

plant::plant(const int val): _var2(val)
{
  for (int i=0; i< _var2; ++i)
    {
      _branch.push_back(new leaf(2*i));
    } 
}


// --------------------------------------------------------------


plant::plant(const plant& original): _var2(original._var2)
{

  for (vector<leaf*>::const_iterator it = original.begin();   
      it != original.end();
      ++it)
    {
       _branch.push_back(new leaf(**it));                     
    }  
  cout << "cpy\n";
}


// --------------------------------------------------------------


/*
plant& operator=(const plant& original)
{
}
*/


// --------------------------------------------------------------


plant::~plant()
{
  for (std::vector<leaf*>::iterator it = _branch.begin();
       it != _branch.end();
       ++it)
    {
      delete *it;
    }
}

