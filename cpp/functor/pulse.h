#ifndef FUNC_H
#define FUNC_H

#include <cmath>
//#include <iostream>

// pagina 75 lezione Giacomini

/* funzione di fit per la pulse-shape:

F(ti) = A ( 1 + dti/(alpha * beta))^alpha exp(-dti / beta)

con dti = ti - tmax


alpha = 1.05  laser runs
beta  = 2.21

alpha = 0.86  electrons runs
beta  = 2.20

---------------------------------------*/

// la domanda e': si puo' passare anche parametri alla funzione? 
// questo ha a che fare con gli adapters?


class NNpulse {

  public:

    void operator()() {}
    double operator()(double x)
      {
	alpha = 1.05;
	beta = 2.21;
        A = 1;
        tmax = 4.;
        tmax = 0.;  // nel caso in cui Tmax sia gia' considerato in
	            // create_model.cpp tmax qui deve rimanere nullo

        // funzione temporanea
//        return 3. * sin(x);      
//        return x;      
//        return x * x;      
//        return sqrt(x);      
//        return 10 * exp (-1 * x * x / 10);      

//        cout << (x - tmax) << endl;
        return A * pow( 1 + (x - tmax)/(alpha * beta), alpha ) 
	       * exp(-1 * (x - tmax) / beta);

//          if (x > tmax) return A * exp (-1 * (x - tmax));
//          return A * exp (-1 * (x - tmax) * (x - tmax));
      }

  private:
     
     double alpha;
     double beta;
     double A;
     double tmax;

};

#endif
