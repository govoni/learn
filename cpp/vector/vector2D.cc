#include "vector2D.h"
#include <cmath>
#include <iostream>

  vector2D::vector2D(double x, double y): x_(x), y_(y)
   {
      cout <<"my constructor" << endl;   
   }
  vector2D::vector2D (const vector2D & w) :
      x_(w.x_), 
      y_(w.y_)
    {
      cout <<"copy constructor" << endl;
    }
//  int vector2D::SetX(double x) {x_ = x; return 0;}
//  int vector2D::SetY(double y) {y_ = y; return 0;}
  double vector2D::x() const 
  {return x_;}
  double vector2D::y() const
  {return y_;}
  double vector2D::r() const
  {return sqrt(x_*x_+y_*y_);}
  double vector2D::phi() const
  {return atan2(y_,x_);}
  ostream& operator <<(ostream& os, const vector2D& v)
    {
      return os << "(" << v.x_ << "," << v.y_ << ")";
    }  

  void stampa (vector2D w)
    {
      cout << "x = " << w.x() << endl;
    }

  void stampa2 (const vector2D& w)
    {
      cout << "x = " << w.x() << endl;
    }

