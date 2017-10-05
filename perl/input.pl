#!/usr/bin/perl

# how to read input from the command line:
# the vector @ARGV is created by default, I think
#es.:   if ($#ARGV >= 0) { $who = join(' ', @ARGV); }
#       else { $who = 'World'; }
#       print "Hello, $who!\n";

$run_path = './';
$run_file = $run_path . 'run' . $ARGV[0];
$rootdb_path = '/afs/cern.ch/user/k/koblitz/scratch0/h4/';
$rootdb_file = $rootdb_path . 'rootdb2003.root';
$ped_path = '/afs/cern.ch/user/k/koblitz/scratch0/h4/';
$ped_file = $ped_path . 'Ped2003.root';
$exppulseshape_path = '/afs/cern.ch/user/k/koblitz/scratch0/h4/';
$exppulseshape_file = $exppulseshape_path . 'Shapes.root';

#file utilizzabile per test: /afs/cern.ch/user/k/koblitz/scratch0/h4/ecs63138

# create the config file for this run
# -----------------------------------

$config_file = 'config';        # Name the file
open(CONFIG, ">$config_file");  # Open for output

# write the config file following its syntax
# and insert the variables that have been read

print CONFIG "[General]\n";
print CONFIG "#BatchMode\n";
print CONFIG "\n";

print CONFIG "[Input]\n";
print CONFIG "Files = $run_file\n";
print CONFIG "EventHeader\n";
print CONFIG "RawTowers = BRawTower_63\n";
print CONFIG "RawTdcCONFIG = BRawTdcCONFIG\n";
print CONFIG "RunCONFIG\n";
print CONFIG "\n";

print CONFIG "[RootDB]\n";
print CONFIG "H4RootDB = $rootdb_file\n";
print CONFIG "#H4SQLDB  = suncms100.cern.ch, WWW, myWeb01\n";
print CONFIG "\n";

print CONFIG "[Towers]\n";
print CONFIG "TimeSamples = 14\n";
print CONFIG "\n";

print CONFIG "[Action]\n";
print CONFIG "Calculate=PulseShape # another comment\n";
print CONFIG "\n";
print CONFIG "#Pedestal CONFIG \n";

print CONFIG "[Pedestals]\n";
print CONFIG "FileName = $ped_file\n";
print CONFIG "TreeName = TPedestals\n";
print CONFIG "#TowerNames = Tower_Peds_05.\n";
print CONFIG "TowerNames = Tower_Peds_63.\n";
print CONFIG "\n";

print CONFIG "[Output]\n";
print CONFIG "# File = output.root\n";
print CONFIG "# Tree = T01\n";
print CONFIG "# Tree = myTree\n";
print CONFIG "H4Hodoscope\n";
print CONFIG "H4RecEnergy\n";
print CONFIG "\n";

print CONFIG "[PulseFitWeights]\n";
print CONFIG "Method = 0 # fit amplitude only\n";
print CONFIG "#Method = 1 # fit amplitude + residual pedestal\n";
print CONFIG "#Method = 2 # fit amplitude + time of max\n";
print CONFIG "#Method = 3 # fit amplitude + residual pedestal + time of max\n";
print CONFIG "nSamplesBeforeMax = 1\n";
print CONFIG "nSamplesAfterMax = 3\n";
print CONFIG "sampleMin = 2\n";
print CONFIG "sampleMax = 10\n";
print CONFIG "\n";

print CONFIG "[PulseRec]\n";
print CONFIG "ExpPulseShapeFile = $exppulseshape_file\n";
print CONFIG "\n";

print CONFIG "[RecEnergy]\n";
print CONFIG "Width = 5\n";
print CONFIG "Height = 5\n";
print CONFIG "\n";

close(CONFIG);

# launch the DV code with the proper config

$DVprog_path = './';
$DVprog_file = $DVprog_path . 'primo';
system ("$DVprog_file $config_file");
