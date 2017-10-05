#include <iostream>

template<typename T1, typename T2 /* = double */>
void fun(T1 t1, T2 t2)
{
  std::cout << "fun(T1, T2)\n";
}

/*
template<typename T>
void fun<T, int>(T t, int i) 
{
  std::cout << "fun(T1, int)\n";
}
*/

template<>
void fun<int, int>(int i, int j)
{
  std::cout << "fun(int, int)\n";
}

template<typename T1, typename T2>
void fun(T1* i, T2 const* j)
{
  std::cout << "fun(T1*, T2 const*)\n";
}

template<int N>
int f() { return N; }

int main()
{
  fun(1., 2.);
  fun(1., 2);
  fun(1, 2);
  fun<double, double>(1, 2);
  int i;
  const int j = 0;
  fun(&i, &j);
  return f<10>() + f<5>();
}
