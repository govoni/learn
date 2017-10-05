#!/usr/bin/perl

#$prima = $ARGV[0];

$templateConfig = $ARGV[0] ;
$castorfolder = $ARGV[1] ;
$etaStart =  $ARGV[2] ;
$etaEnd =  $ARGV[3] ;
$etaWidth =  $ARGV[4] ;
$phiStart = 0 ;
$phiEnd = 0 ;
$phiWidth = 0 ;
$queue = "8nm" ;
# $templateConfig = "config_template.cfg" ;
# $castorfolder = "/castor/cern.ch/user/g/govoni/qualcosalatro/" ;
# $etaStart = 0 ;
# $etaEnd = 10 ;
# $etaWidth = 2 ;
# $phiStart = 0 ;
# $phiEnd = 0 ;
# $phiWidth = 0 ;
# $queue = "8nm" ;

#PG loop over eta
for ($eta=$etaStart ; $eta < $etaEnd ;)
{
  print "etaStart = ".$eta ;
  #PG create the name of the new cfg file
  $configName = "config_".$eta.".cfg" ;
  #PG change che eta start and end
  system("cat $templateConfig | sed -e 's/ETASTART/$eta/g' > $configName");
  $eta += $etaWidth ;
  system("cat $configName | sed -e 's/ETAEND/$eta/g' > tempo");
  system("mv tempo $configName");
  print ", etaEnd = ".$eta."\n" ;
  #PG change the ouptut file name
  $outfilename = "preSelected_".($eta-$etaWidth)."_".$eta.".root" ;  
  system("cat $configName | sed -e 's%OUTFILE%".$outfilename."%g' > tempo");
  system("mv tempo $configName");
  #PG crea e lancia il job al batch system
  $folder = `pwd` ;
  $folder =~s/\n//;
  $filename = "til_".$eta."_select.sh" ;
  open JOBFILE, ">".$filename ;
  print JOBFILE "\#! /bin/sh\n" ;
  print JOBFILE "cd $folder\n" ;
  print JOBFILE "eval `scramv1 run -csh`\n" ;
  print JOBFILE "cd /tmp\n" ;
  print JOBFILE "cmsRun ".$folder."/".$configName."\n" ;  
  print JOBFILE "rfcp outfilename $castorfolder\n" ;  
  close JOBFILE ;
  system("chmod 755 ".$filename);
  print "bsub -q ".$queue." ".$filename."\n";
#  system("bsub -q ".$queue." ".$filename);
}


#print "sostituisco\n";
#$prima = $ARGV[0];

