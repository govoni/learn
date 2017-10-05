#include <fstream>
#include <string>

int main()
{
  string file_name;
  cout << "quale file controllo?" << endl;
  cin >> file_name;
  fstream prova;
  prova.open(file_name.c_str(),ios::in);
  if (!prova)
    {    
      cout << "il file non esiste, ora lo creo" << endl;
      prova.open(file_name.c_str(),ios::out);
      prova << "ciao!!" << endl;
    }
  else
    {
      prova.close();
      prova.open(file_name.c_str(),ios::app);
      prova << "caio???" << endl;
    }
  return 0;
}
