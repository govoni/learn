#!/usr/bin/python

import commands
import fileinput
import sys
import os




def execute (command, printIt = 0, doIt = 1) :
    if doIt : 
        if printIt : print (command)
        commandOutput = commands.getstatusoutput (command)
        if printIt : print commandOutput[1]
        return commandOutput[0]
    else :    print ('    jobs not submitted')
    return 1
    
    
# ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


if __name__ == "__main__":

    print '--------- default'
    execute ('ls')
    print '--------- 1'
    execute ('ls', 1)
    print '--------- 1 1'
    execute ('ls',1, 1) 
    print '--------- 1 0'
    execute ('ls', 1, 0)
    print '--------- 0 0'
    execute ('ls', 0, 0)
    
