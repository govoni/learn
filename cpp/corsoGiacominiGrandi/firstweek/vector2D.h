#include<iostream>
#ifndef vector2D_h
#define vector2D_h

class vector2D 
{
public:
  vector2D (const vector2D & w);
  vector2D (double x = 0, double y = 0);
  friend ostream& operator <<(ostream& os, const vector2D& v);
//  int SetX(double x);
//  int SetY(double y);
  double x() const;
  double y() const;
  double r() const;
  double phi() const;
  
private:
  double x_;
  double y_;
};
  
void stampa (vector2D w);

void stampa2 (const vector2D& w);

ostream& operator <<(ostream& os, const vector2D& v);

#endif


