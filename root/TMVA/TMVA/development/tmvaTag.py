#!/usr/bin/env python

import sys,os,sys, time

if __name__ == "__main__":

    if len(sys.argv)<2:
        print "Usage:"
        print "%s 1.2.3" % (sys.argv[0])
        #print "For your information, the last tag was : "
        sys.exit(1)

    newcvstag = sys.argv[1].split('.')
    if len(newcvstag) != 3:
        print "tag should have the format 1.2.3"
        sys.exit(1)


    infile  = file("src/Version.h","r")
    outfile = file("src/Version.new.h","w")
        
    lines = infile.readlines()

    ltime = time.localtime()

    version = tuple([int(x) for x in newcvstag]);
    
    for line in lines:
        if line.find('TMVA_RELEASE ')!=-1:
            print>>outfile, '#define TMVA_RELEASE      "%i.%i.%i"' % version
        elif line.find('TMVA_RELEASE_DATE ')!=-1:
            print>>outfile, '#define TMVA_RELEASE_DATE "%s"' % time.strftime('%b %d, %Y', ltime)
        elif line.find('TMVA_RELEASE_TIME ')!=-1:
            print>>outfile, '#define TMVA_RELEASE_TIME "%s"' % time.strftime('%X', ltime)
        elif line.find('TMVA_VERSION_CODE ')!=-1:
            code  = version[0] << 16
            code += version[1] << 8
            code += version[2]
            print>>outfile, '#define TMVA_VERSION_CODE %i' % code
        else:
            print>>outfile, line,

    infile.close()
    outfile.close()

    os.system('mv src/Version.new.h src/Version.h')

    print "I have just modified the file 'src/Version.h'. Next steps to be taken are: "
    cmds = ["cvs up", 'cvs ci -m "new release %s"' % sys.argv[1],
           'cvs tag V%02d-%02d-%02d' % tuple([int(x) for x in sys.argv[1].split('.')]) ]
    for cmd in cmds:
        print cmd

    print 'Shall I execute these steps ? (y/n)'
    answer = sys.stdin.read(1)

    if answer == 'y':
        for cmd in cmds:
            print '\n',cmd
            os.system( cmd )
