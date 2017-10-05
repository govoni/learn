#!/usr/bin/perl

print "ciao\n";
foreach $elem (<"ls">)
  {
    $file = "zzscrivo";
    open(IN1,$elem);
    if ($elem !~ "converti01.pl")
      {
	`rm $elem`;
	open(out,">>$elem");
	@p = <IN1>;
	foreach $riga (@p)
	  {
            $riga =~ s/\/afs\/cern.ch//g;
	    print out $riga;
	  }
      }
    close (IN1);
    close (out);
  }
