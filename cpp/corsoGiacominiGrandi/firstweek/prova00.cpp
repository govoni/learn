// hello world program

#include<iostream>

int main()
{
  char *nome = new char[10];
  cout << "ciao mondo" << endl;
  cout << "whaz your name?" << endl;
  cin >> nome;
  cout << nome <<", vero? " << endl;
  //cout << '\a' << endl;
  delete [] nome;
  return 0;
}
