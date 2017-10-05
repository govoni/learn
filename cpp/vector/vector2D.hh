//#ifndef vector2D_h
//#define vector2D_h

class vector2D 
{
public:
  vector2D;
  vector2D (const vector2D & w) {x_ = w.x(); y_=w.y();};
  vector2D(double x = 0, double y = 0): x_(x), y_(y) {};
  int SetX(double x) {x_ = x; return 0;};
  int SetY(double y) {y_ = y; return 0;};
  double x() {return x_;};
  double y() {return y_;};
  double r() {return sqrt(x_*x_+y_*y_);};
  double phi() {return atan2(y_,x_);};
  
private:
  double x_;
  double y_;
};

//#endif
