#include <iostream>

struct A
{
  bool operator==(A const& rhs) const
  {
    std::cout << "A.operator==()\n";
  }
};

bool operator==(A const& lhs, A const& rhs)
{
  cout << "free operator==()\n\n";
}

int
main()
{
  A a;
  A b;
  a.operator==(b);
  a == b;
}


