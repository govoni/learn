/*
c++ testNormal.cpp -o testNormal \
   -I/Users/govoni/private/job/boost/include \
   /Users/govoni/private/job/boost/lib/libboost_math*.a
*/

#include <boost/math/distributions/normal.hpp> // for normal_distribution
using boost::math::normal; // typedef provides default type is double.

#include <iostream>
using std::cout; using std::endl; using std::left; using std::showpoint; using std::noshowpoint;
#include <iomanip>
using std::setw; using std::setprecision;
#include <limits>
using std::numeric_limits;

int main ()
{
  cout << "Example: Normal distribution, Miscellaneous Applications.";

  // Let's start by printing some traditional tables.

  double step = 1. ; // in z 
  double range = 4 ; // min and max z = -range to +range.
  int precision = 17 ; // traditional tables are only computed to much lower precision.

  // Construct a standard normal distribution s
  normal s ; // (default mean = zero, and standard deviation = unity)
  cout << "Standard normal distribution, mean = "<< s.mean ()
       << ", standard deviation = " << s.standard_deviation () << endl ;
       
  // First the probability distribution function (pdf).

  cout << "Probability distribution function values" << endl;
  cout << "  z " "      pdf " << endl;
  cout.precision (5) ;
  for (double z = -range; z < range + step; z += step)
    {
      cout << left << setprecision (3) << setw (6) << z << " " 
           << setprecision (precision) << setw (12) << pdf (s,z) << endl ;
    }
  cout.precision (6) ; // default
  
  
  // And the area under the normal curve from -° up to z, the cumulative distribution function (cdf).
  
  // For a standard normal distribution 
  cout << "Standard normal mean = "<< s.mean()
       << ", standard deviation = " << s.standard_deviation() << endl;
  cout << "Integral (area under the curve) from - infinity up to z " << endl;
  cout << "  z " "      cdf " << endl;
  for (double z = -range; z < range + step; z += step)
    {
      cout << left << setprecision(3) << setw(6) << z << " " 
           << setprecision(precision) << setw(12) << cdf(s, z) << endl;
    }
  cout.precision(6); // default

  // Pretty-printing a traditional 2-dimensional table is left as an exercise for the student, 
  // but why bother now that the Math Toolkit lets you write

  double z = 2.; 
  cout << "Area for z = " << z << " is " << cdf(s, z) << endl; // to get the area for z.

  // Correspondingly, we can obtain the traditional 'critical' values for significance levels. 
  // For the 95% confidence level, the significance level usually called alpha, is 0.05 = 1 - 0.95 
  // (for a one-sided test), so we can write
  
  cout << "95% of area has a z below " << quantile (s, 0.95) << endl ;
  // 95% of area has a z below 1.64485

  // and a two-sided test (a comparison between two levels, rather than a one-sided test)
  
  cout << "95% of area has a z between " << quantile (s, 0.975)
       << " and " << -quantile (s, 0.975) << endl;
  // 95% of area has a z between 1.95996 and -1.95996

  // First, define a table of significance levels: these are the probabilities 
  // that the true occurrence frequency lies outside the calculated interval.

  // It is convenient to have an alpha level for the probability that z lies outside 
  // just one standard deviation. This will not be some nice neat number like 0.05, 
  // but we can easily calculate it,

  double alpha1 = cdf(s, -1) * 2; // 0.3173105078629142
  cout << setprecision(17) << "Significance level for z == 1 is " << alpha1 << endl;
  //and place in our array of favorite alpha values.
  
  double alpha[] = {0.3173105078629142, // z for 1 standard deviation.
    0.20, 0.1, 0.05, 0.01, 0.001, 0.0001, 0.00001 };

  // Confidence value as % is (1 - alpha) * 100 (so alpha 0.05 == 95% confidence) 
  // that the true occurrence frequency lies inside the calculated interval.
  
  cout << "level of significance (alpha)" << setprecision(4) << endl;
  cout << "2-sided       1 -sided          z(alpha) " << endl;
  for (int i = 0; i < sizeof(alpha)/sizeof(alpha[0]); ++i)
    {
      cout << setw(15) << alpha[i] << setw(15) << alpha[i] /2 
           << setw(10) << quantile(complement(s,  alpha[i]/2)) << endl;
      // Use quantile(complement(s, alpha[i]/2)) to avoid potential loss of accuracy from quantile(s,  1 - alpha[i]/2) 
    }
  cout << endl;

  // Armed with the cumulative distribution function, we can easily calculate 
  // the easy to remember proportion of values that lie within 
  // 1, 2 and 3 standard deviations from the mean.
  
  cout.precision(3);
  cout << showpoint << "cdf(s, s.standard_deviation()) = "
    << cdf(s, s.standard_deviation()) << endl;  // from -infinity to 1 sd
  cout << "cdf(complement(s, s.standard_deviation())) = "
    << cdf(complement(s, s.standard_deviation())) << endl;
  cout << "Fraction 1 standard deviation within either side of mean is "
    << 1 -  cdf(complement(s, s.standard_deviation())) * 2 << endl;
  cout << "Fraction 2 standard deviations within either side of mean is "
    << 1 -  cdf(complement(s, 2 * s.standard_deviation())) * 2 << endl;
  cout << "Fraction 3 standard deviations within either side of mean is "
    << 1 -  cdf(complement(s, 3 * s.standard_deviation())) * 2 << endl;
  // To a useful precision, the 1, 2 & 3 percentages are 68, 95 and 99.7, 
  // and these are worth memorising as useful 'rules of thumb', as, for example, in standard deviation:
  
  return 0. ;
}


/*


To obtain a standard normalized gaussian, use "gausn" instead of "gaus". The expression "gausn" is a substitute for
     [0]*exp(-0.5*((x-[1])/[2])**2)/sqrt(2*pi*[2])
The "gaus" and "gausn" functions use now TMath::Gaus.


*/