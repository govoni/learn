#include <iostream>
#include <vector>
#include <set>
#include <list>

template<typename InputIterator, typename OutputIterator>
OutputIterator
cpy(InputIterator first, InputIterator last,
     OutputIterator result) {
  while (first != last) *result++ = *first++;
  return result;
}

int main()
{
  int numbers[] = {4, 2, 3, 1, 5};
  int num_elements = sizeof(numbers) / sizeof(numbers[0]);

  std::cout << "numbers: ";
  for (int i = 0; i < num_elements; ++i) {
    std::cout << numbers[i] << " ";
  }
  std::cout << '\n';

  std::vector<int> v(num_elements); // reserve space!
  cpy(numbers, numbers + num_elements, v.begin());
  std::cout << "vector:  ";
  for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << '\n';

  std::set<int> s(numbers, numbers + num_elements);
  std::cout << "set:     ";
  for (std::set<int>::iterator it = s.begin(); it != s.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << '\n';

  std::list<int> l(num_elements); // reserve space!
  cpy(s.begin(), s.end(), l.begin());
  std::cout << "list:    ";
  for (std::list<int>::iterator it = l.begin(); it != l.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << '\n';
}

