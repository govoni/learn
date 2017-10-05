#include <iostream>
#include <cmath>

int main()
  {
    double x,y;
    x =  y = 0;
    cout << "Primo numero: "<< endl;
    cin >> x;
    cout << "Secondo numero: "<< endl;
    cin >> y;
    double r,phi; 
    r = phi = 0;
    r = pow(x*x + y*y, 0.5);
    if (r != 0) 
      {
        phi = atan2 (y,x);
	cout << "r = " << r << endl;
	cout << "phi = " << phi << endl;
      }
    else cout << "Il punto e' l'origine" << endl;
    return 0;
  }
