//prova passaggio argomenti alle funzioni
#include <iostream>

class passing
{

public:

  int Set1(const int val) {var1 = val; return 0;}
  int Get1() const {return var1;}

private:

  int var1;
};


class figlio: public passing
{

public:

  int metodo() const {return passing::Get1();}

};


int main()
{
   figlio tento;
   int valore = 7;
   tento.Set1(valore);
   cout << tento.metodo() << endl;
 
}
