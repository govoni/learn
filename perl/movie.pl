#!/usr/bin/perl

# make animated gifs

$start = $ARGV[0] ;
$end = $ARGV[1] ;
$slices = $ARGV[3] ;
$delay = $argv[4] ;
$generic = $ARGV[5] ;

$calibDiff = "calibDiff" ;
$calibProd = "calibProd" ;
$calibCoeff = "calibCoeff" ;
$miscalibCoeff = "miscalibCoeff" ;

$action = "convert -delay=".$delay." " ;
$calibDiffCommand = $action ;
$calibProdCommand = $action ;
$calibCoeffCommand = $action ;
$miscalibCoeffCommand = $action ;

$slice = "0" ; #in futuro in ciclo!
for ($i = $start; $i < $end; ++$i)
{
  $common = "_".$slice."_L_".$i.".eps " ;
  $calibDiffCommand = $calibDiffCommand.$calibDiff.$common ;
  $calibProdCommand = $calibProdCommand." ".$calibProd.$common ;
  $calibCoeffCommand = $calibCoeffCommand." ".$calibCoeff.$common ;
  $miscalibCoeffCommand = $miscalibCoeffCommand." ".$miscalibCoeff.$common ;
}
$common = ".gif\n" ;
$calibDiffCommand = $calibDiffCommand.$calibDiff.$common ;
$calibProdCommand = $calibProdCommand." ".$calibProd.$common ;
$calibCoeffCommand = $calibCoeffCommand." ".$calibCoeff.$common ;
$miscalibCoeffCommand = $miscalibCoeffCommand." ".$miscalibCoeff.$common ;

print $calibDiffCommand ;
print $calibProdCommand ;
print $calibCoeffCommand ;
print $miscalibCoeffCommand ;
#system ($calibDiffCommand) ;
#system ($calibProdCommand) ;
#system ($calibCoeffCommand) ;
#system ($miscalibCoeffCommand) ;

