#!/bin/sh

LPWD=`pwd`
cd $1
DEST=`pwd`"/tmva"

# from now on we work only with absolute paths LPWD and DEST

echo " take TMVA package from $LPWD and put it to $DEST"

mkdir -p $DEST/src
mkdir -p $DEST/inc
mkdir -p $DEST/test

cp $LPWD/src/*.h $DEST/inc/
rm -f $DEST/inc/TMVA_Dict.h

cd  $LPWD/src/
for file in `ls -1 *.cxx`
do
     cp ${file}  $DEST/src/
done


# everything from the macros
cp $LPWD/examples/data/toy_sigbkg.root $DEST/test/tmva_example.root
cp $LPWD/macros/*.C $DEST/test/.
cp $LPWD/macros/README $DEST/test/.

# the python example
cp $LPWD/python/TMVAnalysis.py $DEST/test/.
cp $LPWD/python/TMVApplication.py $DEST/test/.

# and the images
cp $LPWD/macros/*.gif $DEST/test/.
cp $LPWD/macros/*.png $DEST/test/.

#cp $LPWD/ROOTModule.mk  $DEST/Module.mk
#cp $LPWD/ROOTtestMakefile $DEST/test/Makefile
