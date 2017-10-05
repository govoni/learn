#include <iostream>
#include <exception>

struct TIsZero: public std::exception {
  const char* what() const { return "T is zero"; }
};

struct T {
  int i;
  T(): i(1) {}
};

int f(T* p) {
  if (p == 0) throw TIsZero();
  return p->i;
}

int main()
{
  T t;

  try {
    std::cout << "f(&t) = " << f(&t) << '\n';
    std::cout << "f(0) = " << f(0) << '\n';
  } catch (TIsZero& ex) {
    std::cout << ex.what() << '\n';
  } catch (...) {
    std::cout << "unkown exception\n";
  }
}
