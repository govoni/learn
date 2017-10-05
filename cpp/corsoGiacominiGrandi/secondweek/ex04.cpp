#include <iostream>
#include <string>
#include <algorithm>

// /usr/include/g++-3/std/bastring.h

int main()
{
  string s;
  while (getline(cin,s))
   {
    if (s.find("replace") != string::npos) cout << s << "\n";
   } 
cout << endl;
}

