#include <vector>
#include <map>
#include <iostream>

class BDouble 
{
  public :
     BDouble (double val = 0., unsigned int thres = 10) :
     m_thres (thres)
       {
         m_val.push_back (val) ;
         m_count.push_back (1) ;       
       }
          
     ~BDouble () {} ;
     
     double value () 
       {
         double sum = 0. ;
         for (int i=0 ; i<m_val.size () ; ++i)
           {
             sum += m_val[i] ;
             // val[i] = 0. ;
           }
         //*val.back = sum ;
         return sum ;
       }
     
     void operator+= (double val) 
       {
         for (int i=0 ; i<m_val.size () ; ++i)
           {
             m_val[i] += val ;
             ++m_count[i] ;
             if (m_count[i] < m_thres)
               {val = 0. ; break ;}
             val = m_val[i] ;
             m_val[i] = 0. ;
             m_count[i] = 0 ;             
           }
         if (val != 0)
           {
             m_val.push_back (val) ;
             m_count.push_back (1) ;
           } 
         return ;   
       }

  private :
  
    unsigned int m_thres ;
    std::vector<double> m_val ;
    std::vector<int> m_count ;
  
} ;


int main () 
{
  double somma = 0. ;
  BDouble bsomma (0) ;
  
  for (int i=0 ; i<1000000 ; ++i)
    {
      somma += i ;
      bsomma += i ;
    }
  
  std::cout << "somma = " << somma 
            << "\t bsomma = " << bsomma.value ()
            << std::endl ;  
    
}