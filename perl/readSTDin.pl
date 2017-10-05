#!/usr/bin/perl

# to read env variables
use Env;
use Env qw(CMSSW_BASE CMSSW_RELEASE_BASE);

#$toFind = $CMSSW_BASE."/src/" ;
#$toFind = chop ($toFind) ;
#$toFind .= "/src/" ;
#$toFind = `pwd`."/src/" ;
#print $toFind ;

while( <> ){
  # stringa =~ /regexp/
  if ($_ =~ /.*#include  *"(.*)"/) 
    {
#      print "`--> detecting file to include: ".$1."\n" ;
      # se il file incluso esiste o sta nella pwd
      if (-e $1)
        {
#          print "   `--> il file ".$1." esiste\n" ;        
        } elsif (-e $CMSSW_BASE."/src/".$1) {
          $toFind = $CMSSW_BASE."/src/".$1 ;
#        } elsif (-e $CMSSW_BASE.$1) {
#          $toFind = $CMSSW_BASE.$1 ;
          print "# parsing : ".$toFind."\n" ;
          print `cat $toFind | ./readSTDin.pl` ;
#          print `cat $toFind` ;
#          print "TEST ".$toFind."\n" ;
           # se il file sta nella local CMSSW release
        } elsif (-e $CMSSW_RELEASE_BASE."/src/".$1) { 
          # se il file sta nella CMSSW release uffciale
          $toFind = $CMSSW_RELEASE_BASE."/src/".$1 ;
          print "# parsing : ".$toFind."\n" ;
          print `cat $toFind | ./readSTDin.pl` ;
#          print "TEST ".$toFind ;
        }
    }
  else 
    {
       print $_ ;
    }  
}

