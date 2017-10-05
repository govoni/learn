#!/usr/bin/perl

use Math::Trig;

#$prima = $ARGV[0];

# $templateConfig = $ARGV[0] ;
# $castorfolder = $ARGV[1] ;
# $etaIndexStart =  $ARGV[2] ;
# $etaIndexEnd =  $ARGV[3] ;
# $etaIndexWidth =  $ARGV[4] ;
# $phiStart = 0 ;
# $phiEnd = 0 ;
# $phiWidth = 0 ;
# $queue = "8nm" ;

$templateConfig = "" ;
$castorfolder = "" ;
$etaIndexStart = 0 ;
$etaIndexEnd = 20 ; 
$etaIndexWidth = 5 ;
$dbfile = "db_template.txt" ;

# $queue = "8nm" ;

#PG loop over eta
for ($etaIndex=$etaIndexStart ; $etaIndex < $etaIndexEnd ; )
{
  #PG find min and max in the eta domain
  $etaMax = -1 * log (tan (atan2 ( 24.7 * (10 + ($etaIndex-$etaIndexWidth)),3200.)/2)) ;
  print "STEP etaIndex : ".$etaIndex." eta value : ".$etaMax."\n" ;
  $etaIndex += $etaIndexWidth ;
  $etaMin = -1 * log (tan (atan2 ( 24.7 * (10 + ($etaIndex+$etaIndexWidth)),3200.)/2)) ;
  print "     etaIndex : ".$etaIndex." eta value : ".$etaMin."\n" ;
  #PG look for the root files in the txt DB file
  $inputfiles = "" ;
  open (DBFILE,$dbfile) ;
  while (<DBFILE>) 
    {
#      print "Line $. is : $_";
      @linea = split (/ /,$_) ;  
      chomp ($linea[2]) ;
#      print $linea[0]."\n";
      if ($linea[0] < $etaMax && $linea[1] > $etaMin)
        {
          $inputfiles .= "'".$linea[2]."'," ;
#          print "    ".$etaMin." ".$etaMax." <- ".$_." <- ".$linea[2]."\n" ;
        }
    }
  close (DBFILE) ;    
  chop ($inputfiles) ; #PG get rid of the last comma
  print "   insomma:\n   ".$inputfiles."\n" ;                                                                  
} #PG loop over eta



