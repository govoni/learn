{
  TArrayD x ;
  //come il std::vector::push_back ()
  for (int i=0; i<10 ; ++i)
    {
      x.Set (i+1) ;
      x[i] = i*i ;    
    }
  for (int i=0; i<10 ; ++i)
    {
      cout << "elem " << i
           << ":\t" << x[i] << "\n" ;
    }
  cout << endl ;
}
