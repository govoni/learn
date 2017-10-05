#include <iostream>
#include <cmath>
 
using namespace std;
 
double temp2;
 
int main(){
 
    cout << "\n enter something: ";
    cin >> temp2;
 
 
    if(std::isnan(temp2)){
        cout << temp2 << " is not a number \n";
    }else{
        cout << temp2 << " is a number \n";
    }
    return 0;
 
}
