#include <algorithm>
#include <vector>
#include <iostream>
#include <iterator>

using namespace std;

bool even(int n) { return n % 2 == 0; }

int
main()
{
  vector<int> v;
  generate_n(back_inserter(v), 10, rand);
  copy(v.begin(), v.end(), ostream_iterator<int>(cout, "\n"));
  vector<int>::iterator it = find_if(v.begin(), v.end(), even);
  if (it != v.end()) {
    cout << "first even number: " << *it << '\n';
  }
}

