#include <iostream>

template<typename InputIterator, typename OutputIterator>
OutputIterator
cpy(InputIterator first, InputIterator last,
     OutputIterator result) {
  while (first != last) *result++ = *first++;
  return result;
}

int main()
{
  char const* region2 = "ciao";
  char* region1;
#ifdef RESERVE
  region1 = new char[strlen(region2) + 1]; // reserve
#endif
  cpy(region2, region2 + strlen(region2) + 1, region1);

  std::cout << "region1 = " << region1 << '\n';
}

