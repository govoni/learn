#include <utility>

void f(std::pair<int,int>);

int main()
{
  f(std::pair<int,int>(1,1));
  f(std::make_pair(1,1));
}

