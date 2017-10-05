/*
c++ -I path/to/boost_1_42_0 firstTest.cpp -o firstTest \
   -I/Users/govoni/private/job/boost/include \
   /Users/govoni/private/job/boost/lib/libboost_regex.a
*/

#include <boost/regex.hpp>
#include <iostream>
#include <string>

int main()
{
    std::string line;
    boost::regex pat( "^Subject: (Re: |Aw: )*(.*)" );

    while (std::cin)
    {
        std::getline(std::cin, line);
        boost::smatch matches;
        if (boost::regex_match(line, matches, pat))
            std::cout << matches[2] << std::endl;
    }
}
