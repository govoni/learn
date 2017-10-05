#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <functional>

int main()
{
  std::vector<int> v;
  std::generate_n(back_inserter(v), 10, rand);
  if (std::find_if(v.begin(), v.end(), bind2nd(std::greater<int>(), 5))
      != v.end()) {
    std::cout << "found an element greater than 5\n";
  }
  std::replace(v.begin(), v.end(), *v.begin(), 8);
  std::sort(v.begin(), v.end());
  std::copy(v.begin(), v.end(), ostream_iterator<int>(cout, "\n"));
}

