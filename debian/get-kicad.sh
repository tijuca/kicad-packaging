#!/bin/sh

echo "========== Getting the revision number ==================="
revision=$(bzr log lp:kicad/stable| head -n 2 | grep revno| awk '{print $2}')
version=0.$(date +%Y%m%d)+bzr$revision
echo "version = $version"
package=kicad-$version
origtargz=kicad_$version.orig.tar.gz

echo "========== Getting the stable core ======================="
bzr export $package         lp:kicad/stable
echo "========== Getting the documents ========================="
bzr export $package/doc     lp:~kicad-developers/kicad/doc
echo "========== Getting the libraries ========================="
bzr export $package/library lp:~kicad-product-committers/kicad/library
echo "========== Getting the legacy footprints ================="
wget https://github.com/georgesk/kicad-legacy/archive/master.zip
unzip master.zip
(cd kicad-legacy-master; tar cf - modules) | tar xf - -C $package/library
rm -rf master.zip kicad-legacy-master

echo "========== Compressing the archive ======================="
tar czf $origtargz $package
echo "Sizes:"
echo "Core:    "$(du --exclude=doc --exclude=library -sh $package)
echo "Doc:     "$(du -sh $package/doc)
echo "Library: "$(du -sh $package/library)

