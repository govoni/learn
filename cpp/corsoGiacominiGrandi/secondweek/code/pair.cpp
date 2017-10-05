#include <utility>

struct A
{
  A(const A&);
  A& operator=(const A&);
};

A make_A();

int main()
{
  std::pair<A, A> p(make_A(),make_A());
}

