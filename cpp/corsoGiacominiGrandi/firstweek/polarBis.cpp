#include <iostream>
#include <cmath>
#include "vector2D.h"

int main()
  {
    double x,y;
    x =  y = 0;
    cout << "Primo numero: "<< endl;
    cin >> x;
    cout << "Secondo numero: "<< endl;
    cin >> y;
    vector2D v(x,y);
    cout << "r = " << v.r() << ", phi = " << v.phi() << endl;  
    vector2D w = v;
    cout << "r = " << w.r() << ", phi = " << w.phi() << endl;  
    stampa(w);
    stampa2(w);
    cout << w << endl;
    return 0;
  }
