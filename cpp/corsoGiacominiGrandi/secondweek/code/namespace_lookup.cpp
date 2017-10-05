namespace A {
struct S {};
void f(S s) {};
}

int main()
{
  { // explicit identification of the namespace
    A::S s;
    A::f(s);
  }

  { // using declaration
    using A::S;
    S s;
    A::f(s);
  }

  { // using directive
    using namespace A;
    S s;
    f(s);
  }

  { // koening lookup
    A::S s;
    f(s);
  }
}

