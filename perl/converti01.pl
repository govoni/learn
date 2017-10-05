#!/usr/bin/perl

print "ciao\n";
open (PROVA, "ls |");
while ($_=<PROVA>)
#foreach $_ (<"ls">)
  {
     print $_;
     open(IN1,$_);
     if ($_ !~ "converti01.pl")
       {
	@p = <IN1>;
	`rm $_`;
	open(out,">>$_");
	foreach $riga (@p)
	  {
#	     $riga =~ s/eturn 0/eturn 2/g;
#	     $riga =~ s/eturn 1/eturn 0/g;
	     $riga =~ s/eturn 2/eturn 1/g;
             print out $riga;
#            print "replace\n";
	  }
      }
    close (IN1);
    close (out);
  }
 close (PROVA);  
