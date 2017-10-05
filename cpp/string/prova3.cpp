#include <iostream>
#include <sstream>
#include <string>

int main()
{
  /*
  ostringstream format_message;
  format_message << "Maximum X   : " << 10 << endl;
  cout << format_message.str();
//  format_message.close();
  format_message.str("");
  format_message << "Maximum X   : " << 20 << endl;
  cout << format_message.str();
  cout << endl;

  stringstream picture_name_ps;
  stringstream picture_name_gif;
  picture_name_ps << "PEDSsample_E.ps";                
  picture_name_gif << "PEDSsample_E.gif";                
  stringstream command;
  command << "convert ";
  command << picture_name_ps.str() << " " << picture_name_gif.str(); 
  cout << command.str()<< endl;     
  */

  stringstream command;
  //command << "convert ";
  for (int i=0; i<10; ++i)
    {
      command << i << "\t";
      cout << command.tellp() << "  o  ";
    }
  cout << "\ninput: " << command.str() << endl;

  //char primo[80];
  
  //primo << command;
  
  
  int provo;
  cout << command.tellg() << "<---\n";
//  command.seekp(0,ios::beg);
  command.seekg(0,ios::beg);
  cout << command.tellg() << "<---\n";

  for (int i=0; i<5; ++i) 
    {
      command >> provo;
      cout << provo << "\t";
    }
  cout << endl;  
  
}
