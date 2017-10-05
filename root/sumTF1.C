#include <vector>
#include "TF1.h"

struct SumTF1 { 

   SumTF1(const std::vector<TF1 *> & flist) : fFuncList(flist) {}
   
   double operator() (const double * x, const double *p) {
      double result = 0;
      for (unsigned int i = 0; i < fFuncList.size(); ++i) 
         result += fFuncList[i]->EvalPar(x,p); 
      return result; 
   } 
   
   std::vector<TF1*> fFuncList; 
      
};


int main() { 
   std::vector<TF1 *> v; 
   v.push_back(new TF1("f1","sin(x)",0,10));
   v.push_back(new TF1("f1","cos(x)",0,10));

   TF1 * fsum = new TF1("fsum",SumTF1(v),0,10,0);
   fsum->Draw();
               
}
