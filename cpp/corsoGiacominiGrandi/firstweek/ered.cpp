#include <iostream>

class B
{
  public:
    B(int bpro,int bpri):bpro_(bpro),bpri_(bpri){}
    virtual ~B() {}
    void f1() 
      {
        cout << "eseguo f1 base:\n";
	cout << "bpri_ " << bpri_ << "\n";
      }
    virtual void f2() 
      {
        cout << "eseguo f2 base:\n";
    	cout << "bpri_ " << bpri_ << "\n";
	cout << "bpro_ " << bpro_ << "\n";
      }
    virtual void f3() = 0;

  protected:
    int bpro_;

  private:
    int bpri_;
};

class D: public B
{
  public:
    D(int bpro, int bpri, int dpro, int dpri):
     B(bpro,bpri),
     dpro_(dpro), dpri_(dpri){}
    virtual ~D() {}
    void f3() 
      {
	cout << "eseguo f3 derivata\n";
    	cout << "dpri_ " << dpri_ << "\n";
	cout << "dpro_ " << dpro_ << "\n";
	cout << "bpro_ " << bpro_ << "\n";
      }
    
  protected:
    int dpro_;
  
  private:
    int dpri_;
  
};

class D1: public D
{
  public:
    D1(int bpro, int bpri, int dpro, int dpri):
     D(bpro,bpri,dpro,dpri){};
    ~D1() {}
    void f2() 
      {
	cout << "eseguo f2 derivata\n";
	cout << "dpro_ " << dpro_ << "\n";
	cout << "bpro_ " << bpro_ << "\n";
      }
};


// main program
//-------------

int main()
  {
    D d(1,2,3,4);
    D1 d1(5,6,7,8);
    
    cout << " -> d <-\n";

    d.f1();
    d.f2();
    d.f3();
    
    cout << " -> d1 <-\n";

    d1.f1();
    d1.f2();
    d1.f3();
    
    cout << "bye" << endl;
         
    return 0;
  }
