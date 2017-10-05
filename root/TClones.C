int TClones()
{
  TClonesArray a("int", 1000);                                   
  for (int i = 0; i < 1000; ++i) 
    {   
       a.Add(i);                                
    }                                                              
  a.Delete();                                                    
}
