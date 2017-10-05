//prova passaggio argomenti alle funzioni
#include <iostream>

class passing{

public:

  int Set1(const int val) {var1 = val; return 0;}
  int Set2(const int& val) {var2 = val; return 0;}
  int Set3(const int* val) {var3 = *val; return 0;}
  int Set4(int& val) {var4 = val--; return 0;}
  int Get1() const {return var1;}
  int Get2() const {return var2;}
  int Get3() const {return var3;}
  int Get4() const {return var4;}

private:

  int var1;
  int var2;
  int var3;
  int var4;

};


passing& creation()
{
  passing dummy;
  int val = 15;
  dummy.Set1(val);
  dummy.Set2(val);
  dummy.Set3(&val);
  return dummy;
}


int main()
{
  int val = 10;
  int* valp = new int(val);
//  *valp = 10;
  passing prova;
  prova.Set1(val);
  prova.Set2(val);
  prova.Set3(&val);
  prova.Set4(*valp);
  
  cout << "Get1: " << prova.Get1() << "\n";
  cout << "Get2: " << prova.Get2() << "\n";
  cout << "Get3: " << prova.Get3() << "\n";
  cout << "Get4: " << prova.Get4() << "\n";
  cout << "valp: " << *valp << endl;
 
  passing prova1 = creation();
  cout << "Get1: " << prova1.Get1() << "\n";
  cout << "Get2: " << prova1.Get2() << "\n";
  cout << "Get3: " << prova1.Get3() << "\n";
 
}
