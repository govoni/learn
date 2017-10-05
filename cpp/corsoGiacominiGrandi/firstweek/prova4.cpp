#include<iostream>

int main(){
  int j=0;
  do
    {
    cout << j << endl;
    if (j  % 2 == 0) continue;
    }
  while (++j < 20);
  return 0;
}
  
  
  
