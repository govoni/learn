#include <iostream>

template<typename T1, typename T2 = int>
struct A {
  T1 t1;
  T2 t2;
  A() { std::cout << "A<T1, T2>\n"; }
};

template<typename T>
struct A<T, char> {
  T t;
  A() { std::cout << "A<T, char>\n"; }
};

template<>
struct A<char, char> {
  A() { std::cout << "A<char, char>\n"; }
};

template<typename T1, typename T2>
struct A<T1*, T2> {
  A() { std::cout << "A<T1*, T2>\n"; }
};

int main()
{
  A<int> a;
  A<int, char> b;
  A<char, char> c;
  A<int*> d;
  A<int*, char> e;
}
