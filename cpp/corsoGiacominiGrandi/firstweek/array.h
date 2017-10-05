// array.h
//template <typename T=int> 
class Array
{
public:
  explicit Array(int N);
  int& operator[] (int index);
  int size() const {return numero};
  ~Array();
  
private:
  int* vettore;
  int numero;

};
