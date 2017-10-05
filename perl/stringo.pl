#!/usr/bin/perl

$input1 = $ARGV[0];
$output1 = $ARGV[1];

open(INPUT1,$input1) or die "cannot open $input1 for read :$!";
open(OUTPUT1,">$output1");
$prec = "ciao\n";
while (<INPUT1>)
  {
    $riga = $_;
      if ($riga!~$prec)
        {
             print OUTPUT1 $riga;
             $prec = $riga;
        }

  }
close (INPUT1);
close (OUTPUT1);

