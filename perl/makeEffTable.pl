

@Wenj ; # vettore vuoto

# leggo file
open (EFFFILE,$dbfile) ;
# loop sul file
while (<EFFFILE>) 
  {
    # trasformo una linea in un vettore
    @linea = split (/ /,$_) ;  
    $linea[0] ; # primo elemento della linea
    $linea[1] ; # secondo elemento della linea 
    push(@Wenj, $linea[0]); #inserisci elemento in un array
    # ....
  }
close (EFFFILE) ;    

@names = {"VBF","Wenj","Wmnj","Zeej","Zmmj","top","total"} ;


