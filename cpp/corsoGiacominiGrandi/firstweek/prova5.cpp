#include<iostream>

int main(){
  float a;
    cout<<"scrivi un numero non intero"<<endl;
    cin>>a;
  int i=static_cast<int>(a);
  float d=a-i;
  cout<<"la parte intera del tuo numero e` "<<i<<endl;
  cout<<"la parte decimale del tuo numero e` "<<d<<endl;
} 
