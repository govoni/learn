#!/usr/bin/perl  

$NOMEDIR = "prove" ;
$OUT = system ("test -e $NOMEDIR") ;
print $OUT."\n" ;
if (system ("test -e $NOMEDIR") == 0) 
  {
    system ("rm -rf $NOMEDIR.old") ;
    system ("mv $NOMEDIR $NOMEDIR.old") ;
  }
mkdir($NOMEDIR, 0755) || die "Cannot mkdir newdir: $!";      



