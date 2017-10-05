#!/usr/local/bin/perl

# come lanciare:
# perl check.pl pippo1 pippo coda

open(LANCIATI, $ARGV[0]);
open(BUONI, $ARGV[1]);

$queue = "pippo" ;
if (@ARGV > 2) 
  {
    $queue = $ARGV[2] ;
  }
@lanciati = <LANCIATI>;
@buoni = <BUONI>;

for ($j = 0; $j < @lanciati; ++$j)
  {
    my $element = $lanciati[$j] ;
    if (grep {$_ eq $element} @buoni) 
      {
      	chomp ($element) ; 
      	print "Job ".$element." done!\n" ; 
      }
    else
      { 
      	chomp ($element) ; 
      	$jobName = "FIXME.job" ;
      	$outputdat = "outputdatFIXME" ; #metti il full path
      	$outputtop = "outputtopFIXME" ; #metti il full path
      	print "Job ".$element." NOT done, resubmitting... \n" ;
      	$command = "bsub -cwd ./LSF -q ".$queue." \" cd \$PWD ; " ;
      	$command = $command."./mcfm REAL_TEST/".$jobName." ; " ;
      	$command = $command."mv ".$outputdat." outputFolder ; " ;      	
      	$command = $command."mv ".$outputtop." outputFolder" ;      	
      	$command = $command."\"\n" ;
      	print " --> ".$command ;
#      	system ($command) ;
      }  

  }
