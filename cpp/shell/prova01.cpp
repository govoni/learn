// c++ -o prova01 prova01.cpp
//http://codereview.stackexchange.com/questions/42148/running-a-shell-command-and-getting-output
// NON COMPILA

#include <string>
#include <iostream>
#include <string>


std::string getCmdOutput (const std::string & mStr)
{
    std::string result, file ;
    FILE * pipe{popen(mStr.c_str(),"r")};
    char buffer[256] ;

    while (fgets (buffer, sizeof (buffer), pipe) != NULL)
    {
        file = buffer ;
        result += file.substr (0, file.size () - 1) ;
    }

    pclose (pipe) ;
    return result ;
}



int main ()
{

getCmdOutput (" ( sudo pacman -Syyuu )") ;
//auto output (getCmdOutput (R" ( echo /etc/pacman.conf )")) ;

}