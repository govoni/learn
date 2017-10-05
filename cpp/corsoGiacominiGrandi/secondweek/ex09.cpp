#include <iostream> 
#include <vector> 
#include <iterator> 
#include <algorithm> 
#include <functional>


// il mio funtore adaptable, sia ereditando fa unary_function che no. 

class maggiore: public unary_function<int,bool> 
{
public:
 maggiore(int threshold): threshold_(threshold){}
 bool maggiore::operator()(int number) const {return number > threshold_;}
// typedef int argument_type;
// typedef bool result_type;
private:
 int threshold_;
};

// voglio usare il funtore stl, ma greather_than e' binario; devo
// adattarlo a funzione unaria.

// bind1st blocca il primo argomento
// bind2nd blocca il primo argomento <- uso questo in questo caso

// bind2nd(greater_than<int>(),5)


int main() 
{ 
  int N = 10;
  std::vector<int> v;
  for (int i=1; i<N; ++i)
    {
      v.push_back(i); 
    }
    
//  std::vector<int>::iterator it = find_if(v.begin(),v.end(),not1(maggiore(5)));
  std::vector<int>::iterator it = find_if(v.begin(),v.end(),
                                 bind2nd(std::greater<int>(),5));
  if (it != v.end()) cout << *it;
//  cout << std::greater<int>()(3,4);
  cout << endl;
}

