#!/usr/bin/perl

$file = "./istruzioni.pie" ;

if (!-e $file)
{
  system ("echo cat converti.pl > ./istruzioni.pie\n") ;
#  exec ("scram b > ./istruzioni.pie\n") ;
}
else
{
  open(INFO, $file);		# Open the file
  @lines = <INFO>;		  # Read it into an array
  close(INFO);		     	# Close the file
  $salta = 0 ;
  foreach $linea (@lines)
    {
      if ($salta != 0)
        {
          print $linea ;
          system ("$linea\n") ;           
        }
      $salta = $salta +  1 ;
    }

#  system ("`cat ./istruzioni.pie`\n") ;
  print "-------------------------------------------------------------------\n" ;
  print " the compilation occurred NOT taking into accout BuildFile changes\n" ;
  print "-------------------------------------------------------------------\n" ;
}

