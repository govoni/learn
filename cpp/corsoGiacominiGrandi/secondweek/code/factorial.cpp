#include <iostream>

template<int N>
struct factorial {
  static const int value = N * factorial<N-1>::value;
};

template<>
struct factorial<0> {
  static const int value = 1;
};

int main()
{
  // 10! = 3628800 (0x375F00)
  std::cout << factorial<10>::value << '\n';
}
