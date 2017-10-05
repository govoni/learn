
#open CONFIG, '<', $ARGV[0] or die $! ;

open (USERCONFIG,$ARGV[0]) ;

while (<USERCONFIG>)
  {
    chomp; 
    s/#.*//;                # no comments
    s/^\s+//;               # no leading white
    s/\s+$//;               # no trailing white
#    next unless length;     # anything left?
    my ($var, $value) = split(/\s*=\s*/, $_, 2);    $User_Preferences{$var} = $value;  }

print $User_Preferences{"nome"}."\n" ;
print $User_Preferences{"proverbio"}."\n" ;
if ($User_Preferences{"pippo"} == "") {print "WARNING pippo not found \n"} ;
#print $User_Preferences{"pippo"}."\n" ;