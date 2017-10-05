class soldato
{
  public:
    soldato();
    soldato(double posizioneX, double posizioneY, int forza = 10, int vita = 10);
    ~soldato();
    virtual void moveMe(double arrivoX, double arrivoY);
    virtual void attacco() = 0;

  protected:
    double posizioneX_;
    double posizioneY_;
    int forza_;
    int vita_;
};

soldato::soldato(double posizioneX, double posizioneY, int forza, int vita):
         posizioneX_(posizioneX), posizioneY_(posizioneY),
	 forza_(forza), vita_(vita)
 {
   cout << "Nato un soldato\n"; 
 }

soldato::~soldato()
  {
    cout << "Morto un soldato\n";
  }


virtual void moveMe(float* arrivo)
  {
    cout << "Muovo un soldato\n";
  }
