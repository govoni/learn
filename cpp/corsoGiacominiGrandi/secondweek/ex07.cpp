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
//     string::size_type size = distance(c.begin(),c.end());
     
     string::iterator beginpos = s.begin();
     string::iterator it;
     do
       {
	 it = search(beginpos,s.end(),c.begin(),c.end());
	 if (it != s.end())
           {
//	     std::copy(beginpos + c.size(), it, ostream_iterator<char>(cout, "\t"));
	     beginpos = it + c.size();
	     std::cout << "*"; 
	   }
       } while (it != s.end()); 
   }
  std::cout << endl;
}

