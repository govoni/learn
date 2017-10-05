#include <algorithm>
#include <set>
#include <iostream>
#include <iterator>

using namespace std;

bool greater_than_one_billion(int n) { return n > 1000000000; }
class greater_than
{
  int m_c;
public:
  greater_than(int c): m_c(c) {}
  bool operator()(int n) { return n > m_c; } 
};

int
main()
{
  set<int> s;
  generate_n(inserter(s, s.begin()), 10, rand);
  copy(s.begin(), s.end(), ostream_iterator<int>(cout, "\n"));

  {
    set<int>::iterator it = find_if(s.begin(), s.end(), greater_than_one_billion);
    if (it != s.end()) {
      cout << "first number > one billion: " << *it << '\n';
    } else {
      cout << "no number > one billion\n";
    }
  }
  {
    const int two_billion = 2000000000;
    set<int>::iterator it = find_if(s.begin(), s.end(), greater_than(two_billion));
    if (it != s.end()) {
      cout << "first number > " << two_billion << ": " << *it << '\n';
    } else {
      cout << "no number > " << two_billion << '\n';
    }
  }
}
