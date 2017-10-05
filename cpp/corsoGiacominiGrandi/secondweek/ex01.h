class A
{
  public:
    A() {}
  private:
    a(a constA&);
    A& operator=(A const&);
};

int main()
{
  A a();
  A b(a);
}

/*
per imepdire la copia della classe dichiaro il copy constructor e l'operatore =
come private, poi non li implemento.
In questo modo, se definisco una classe friend di A - che puo' usare i private -
non riesce ad usare copy ctor o =, perche' non essendoci implementazione si
arriva ad un errore di linking
*/
