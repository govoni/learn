#!/usr/bin/perl

$stuff="lista.txt";
open STUFF, $stuff or die "Cannot open $stuff for read :$!";
while (<STUFF>) 
  {
    #print "Line $. is : $_";
    @linea = $_;

    for ($x=0; $x <= $#linea; $x++)
     {
        print "$linea[$x]\n"; 
     }
  }
