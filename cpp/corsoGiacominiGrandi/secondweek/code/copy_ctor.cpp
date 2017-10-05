class A
{
  friend int main();
public:
  A() {}
private:
  A(A const&);
  A& operator=(A const&);
};

int
main()
{
  A a;
  A b(a);
}

