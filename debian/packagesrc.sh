#!/bin/bash

ver=`cat ../trunk/debian/changelog | head -n1 | cut -f2 -d\( | cut -f1 -d-`
debver=`cat ../trunk/debian/changelog | head -n1 | cut -f2 -d- | cut -f1 -d\)`
svnpath="../../trunk" #extra .. as we'll be down another dir when used
signer="richardaburton@gmail.com"

mkdir kicad-${ver}.orig

if [ "${debver}" = "1" ]; then

    cd kicad-${ver}.orig
    svn export ${svnpath}/kicad
    svn export ${svnpath}/kicad-doc
    svn export ${svnpath}/kicad-library
    # remove any unwanted stuff
    rm -rf kicad/kicad/minizip
    rm -rf kicad-doc/doc/help/{de,es,it,pt,ru}
    cd ..

    cp -a kicad-${ver}.orig kicad-${ver}

else
    tar zxf kicad_${ver}.orig.tar.gz
    mv kicad_${ver}.orig kicad_${ver}
fi

cd kicad-${ver}
svn export ${svnpath}/debian
cd ..

dpkg-source -b kicad-${ver}
debsign -m${signer} kicad_${ver}-${debver}.dsc

#rm -rf kicad-${ver}

