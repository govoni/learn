#!/usr/bin/perl

#print "sostituisco\n";
$prima = $ARGV[0];
$dopo = $ARGV[1];
$cosa = $ARGV[2];

#$cosa = "*";
#$prima = "co";
#$dopo =  "cococ";

print ("sostituisco ".$prima.' -> '.$dopo.' in '.$cosa."\n");
#tolgo gli accapo
$cosa =~s/\n//;
$prima =~s/\n//;
$dopo =~s/\n//;
print("files:\n");
foreach $elem (`ls $cosa`)
  {
    
    if ($elem !~ "sostituisci.pl")
      { 
        print ("\t".$elem);
        #tolgo gli accapo
        $elem =~s/\n//;
        system("cat $elem | sed -e 's/$prima/$dopo/g' > $elem.2");
        system("mv $elem.2 $elem");
      }
  }
