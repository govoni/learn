#ifndef templateClass_h
#define templateClass_h

template <typename TIPO>
class templateClass
{
  public :
     
     templateClass () {} ;
     ~templateClass () {} ;
     
     TIPO setme (TIPO oggetto) { valore = oggetto ; return valore ; } 
     templateClass<int> * fifo (int oggetto) {valore = oggetto ; return this ; }
     float leggimi () ;
     
  private : 
     
     TIPO valore ;

} ;

     template <>
     float templateClass<float>::leggimi () {return valore ; } ;


#endif
