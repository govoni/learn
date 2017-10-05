#!/usr/bin/perl

#print "sostituisco\n";
$totale = $ARGV[0];
$passo = $ARGV[1];
$coda = $ARGV[2];
$eseguo = $ARGV[3];
$root_name = $ARGV[4];

$batch_name = 'batch.job';
$orcarc_name = 'orcarc';
$first = 0;

#ciclo sugli eventi per spacchettare
for ($i = 0; $i < $totale/$passo; ++$i)
{
  $root_file = $root_name . $i;
  $batch_file = $batch_name . $i;
  $orcarc_file = $orcarc_name . $i;

  system ("perl ./batcha.pl ".$batch_file." ".$orcarc_file." ".$root_file." ".$first." ".$totale.
  " ".$eseguo);
  system("bsub -q ".$coda." -J ".$eseguo." < ".$batch_file."\n");

  print "bsub -q".$coda." -J ".$eseguo." < ".$batch_file."\n";
  $first = $first + $passo;
}
