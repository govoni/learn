#include <iostream>
#include <string>
#include <algorithm>

// /usr/include/g++-3/std/bastring.h

int main(int argc, char* argv[])
{
  assert(argc==2);
  string s;
  string c(argv[1]);
  while (getline(cin,s))
   {
    string::size_type tmp=s.find(c);
    if(tmp != string::npos)
      {
	cout<<s.substr(0,tmp)<<"****";
	cout<<s.substr(tmp+c.size(),string::npos)<<"\n";
      }
   } 
  cout << endl;
}

