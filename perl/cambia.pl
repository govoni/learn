#!/usr/bin/perl

#print "sostituisco\n";

for ($i = 3; $i < 30; ++$i)
{

  $dir = "KalibatchCorretto.job".$i;
  $file = "batchCorretto.job".$i.".Map.Vector";
  $filePoi = "batchCorretto.job.Map.Vector";
  
  system("cp ".$dir."/".$file." ".$dir."/".$filePoi."\n");
  system("rm ".$dir."/".$file."\n");

  $file = "batchCorretto.job".$i.".Map.Matrix";
  $filePoi = "batchCorretto.job.Map.Matrix";

  system("cp ".$dir."/".$file." ".$dir."/".$filePoi."\n");
  system("rm ".$dir."/".$file."\n");

}
