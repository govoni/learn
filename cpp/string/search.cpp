#include <iostream>
#include <sstream>
#include <string>

string chop(string input);


int main()
{
  string prova = "pathpat/hfile";
  cout << chop(prova) << endl;
}

string chop(string input)
{
  int memo = 0;
  for (int i=0; i<input.length(); ++i)
    {
       if (input[i] == '/') memo = i+1;
    }
  return input.erase(0,memo);
}
