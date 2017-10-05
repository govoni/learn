#include <memory>
#include <iostream>

int main()
{
  int * uno = new int(1);
  cout << "uno: " << *uno << endl;
  auto_ptr<int>  pippo(new int(5));
  cout << *pippo;
//  pippo.erase(); // non funziona
//  delete pippo; // non funziona
//  pippo.delete; // non funziona 
  pippo.reset();
//  cout << pippo;
  int LIMITE;
//  for (int i=0; i<LIMITE; ++i)
  for (int i=0;;++i)
  {
    //if (! i%10) cout << "*"<<endl;  
    auto_ptr<int>  pippo(new int(5));
  }
}
