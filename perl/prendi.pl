#!/usr/bin/perl

#print "sostituisco\n";
$nome = $ARGV[0];
$da = $ARGV[1];
$a = $ARGV[2];
$start = $ARGV[3];
$stop = $ARGV[4];
$options = $ARGV[5];

for ($i = $start - 1; $i < $stop + 1; ++$i)
{
  $file = $da.$nome.$i;
  system("rfcpdir ".$options." ".$file." ".$a."\n");
  print "rfcp ".$options." ".$file." ".$a."\n";
}
