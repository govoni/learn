// c++ -o prova05 prova05.cpp
// http://www.sw-at.com/blog/2011/03/23/popen-execute-shell-command-from-cc/
// FUNZIONA

#include <iostream>
#include <stdio.h>
#include <string>
#include <map>

using namespace std;

pair<int, string>
execute (const string & command) 
{
    FILE *in;
    char buff[512] ;
    if (!(in = popen (command.c_str (), "r")))
      {
        return pair<int, string> (-99, "") ;
      }

    std::string result, tempo ;
    while (fgets (buff, sizeof (buff), in)!=NULL)
      {
        tempo = buff ;
//        result += tempo.substr (0, tempo.size () - 1) ;
        result += tempo ;
      }
    int returnCode = pclose (in) ;
 
    return pair<int, string> (returnCode, result) ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


int main (int argc, char ** argv) 
{
  string command = "ls -sail" ;
  if (argc > 1) command = argv[1] ;
  pair<int, string>  output = execute (command) ;
  cout << "return code: " << output.first << endl ;
  cout << output.second << endl ;
  return 0 ;

}