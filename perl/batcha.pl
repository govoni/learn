#!/usr/bin/perl

$batch_file   = $ARGV[0];
$orcarc_file  = $ARGV[1];
$root_file    = $ARGV[2];
$first_event  = $ARGV[3];
$nb_events    = $ARGV[4];
$program_name = $ARGV[5];

open(BATCH_JOB, ">$batch_file");  # Open for output

print BATCH_JOB "eval `scram runtime -csh`\n";
print BATCH_JOB "rm -f " . $orcarc_file . "\n";
print BATCH_JOB "cat > " . $orcarc_file . " <<@EOI\n";
print BATCH_JOB "# number of events to be analyzed\n";
print BATCH_JOB "# -------------------------------\n";
print BATCH_JOB "FirstEvent = " . $first_event . "\n";
print BATCH_JOB "MaxEvents = " . $nb_events . "\n";
print BATCH_JOB "#\n";
print BATCH_JOB "# verbosity infoes\n";
print BATCH_JOB "# ----------------\n";
print BATCH_JOB "#Verbose:debug = true\n";
print BATCH_JOB "Verbose:debug = false\n";
print BATCH_JOB "#\n";
print BATCH_JOB "# matching stuff\n";
print BATCH_JOB "# --------------\n";
print BATCH_JOB "#DeltaEta, DeltaPhi;\n";
print BATCH_JOB "TrackTest:DeltaEta = 0.05\n";
print BATCH_JOB "TrackTest:DeltaPhi = 0.05\n";
print BATCH_JOB "TrackTest:DeltaEtaMCE = 0.05\n";
print BATCH_JOB "TrackTest:DeltaPhiMCE = 0.05\n";
print BATCH_JOB "TrackTest:DeltaEtaMCT = 0.05\n";
print BATCH_JOB "TrackTest:DeltaPhiMCT = 0.05\n";
print BATCH_JOB "# EgammaMC KineHBlock\n";
print BATCH_JOB "TrackTest:MCRecUnit = EgammaMC\n";
print BATCH_JOB "#\n";
print BATCH_JOB "# TreeName\n";
print BATCH_JOB "# --------\n";
print BATCH_JOB "# (where to save data)\n";
print BATCH_JOB "TrackTest:TreeName = " . $root_file . "\n";
print BATCH_JOB "#\n";
print BATCH_JOB "# for the Egamma clustering\n";
print BATCH_JOB "# -------------------------\n";
print BATCH_JOB "# (in EgammaLogCShape.cc position reconstruction parameters)\n";
print BATCH_JOB "EGLCShape:logW = false\n";
print BATCH_JOB "EGLCShape:X0 = 0.89\n";
print BATCH_JOB "EGLCShape:T0 = 6.2\n";
print BATCH_JOB "EGLCShape:CutOff = 4.\n";
print BATCH_JOB "#\n";
print BATCH_JOB "# for the Egamma tracking\n";
print BATCH_JOB "# -----------------------\n";
print BATCH_JOB "EgammaEvent:forceNotify=1\n";
print BATCH_JOB "ExtraPackages=EgammaRootSupport:EgammaPhoton:EgammaElectron:EgammaOfflineReco\n";
print BATCH_JOB "Configuration:List = 1\n";
print BATCH_JOB "Configuration:History = 1\n";
print BATCH_JOB "E*:Request=Auto\n";
print BATCH_JOB "C*:Request=Auto\n";
print BATCH_JOB "H*:Request=Nop\n";
print BATCH_JOB "HD:Request=Nop\n";
print BATCH_JOB "CH:Request=Nop\n";
print BATCH_JOB "HH:Request=Nop\n";
print BATCH_JOB "#\n";
print BATCH_JOB "# file catalogue information\n";
print BATCH_JOB "# --------------------------\n";
print BATCH_JOB "InputFileCatalogURL =\@{xmlcatalog_file:POOL1.4_Catalogue_PCP.eg_NoPU752_TkMu_g133_OSC.eg03_ele_pt35.xml}\@\n";
print BATCH_JOB "InputCollections=/System/eg_NoPU752_TkMu_g133_OSC/eg03_ele_pt35/eg03_ele_pt35/\n";
print BATCH_JOB "@EOI\n";
print BATCH_JOB "rehash\n";
# come si lancia un eseguibile con un orcarc diverso?
print BATCH_JOB $program_name." -c ".$orcarc_file."\n";

close(BATCH_JOB);


