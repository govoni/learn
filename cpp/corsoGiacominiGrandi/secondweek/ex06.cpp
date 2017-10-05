#include <iostream>
#include <string>
#include <algorithm>

// /usr/include/g++-3/std/bastring.h

int main(int argc, char* argv[])
{
  assert(argc==2);
  string s;
  string c(argv[1]);
  int cont=0;

  while (getline(cin,s))
   {  
   for(;;)
     {
       string::size_type tmp=s.find(c);
       if(tmp != string::npos)
	 {
	   cout << s.substr(0,tmp);
	   s = s.substr(tmp+c.size(),string::npos);
	   cout << "*";
	   ++cont;
	 }
       else break;
     }       
   } 
  cout << "\n" << cont << endl;
}

