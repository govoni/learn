#include <algorithm>
#include <set>
#include <iostream>
#include <iterator>

using namespace std;

int
main()
{
  set<int> s;
  generate_n(inserter(s, s.begin()), 10, rand);
  copy(s.begin(), s.end(), ostream_iterator<int>(cout, "\n"));

  const int two_billion = 2000000000;
  set<int>::iterator it = find_if(s.begin(), s.end(), bind2nd(greater<int>(), two_billion));
  if (it != s.end()) {
    cout << "first number > " << two_billion << ": " << *it << '\n';
  } else {
    cout << "no number > " << two_billion << '\n';
  }
}
