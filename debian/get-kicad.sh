#!/bin/sh

echo "========== Getting the revision number ==================="
revision=$(bzr log lp:kicad/stable| head -n 2 | grep revno| awk '{print $2}')
version=0.$(date +%Y%m%d)+bzr$revision
echo "version = $version"
package=kicad-$version
origtarxz=kicad_$version.orig.tar.xz

echo "========== Getting the stable core ======================="
bzr export $package         lp:kicad/stable
echo "========== Getting the documents ========================="
bzr export $package/doc     lp:~kicad-developers/kicad/doc
echo "========== Getting the legacy libraries (bzr292) ========="
bzr export -r 292 $package/library lp:~kicad-product-committers/kicad/library

echo "========== Compressing the archive ======================="
tar cJf $origtarxz $package
echo "Sizes:"
echo "Core:    "$(du --exclude=doc --exclude=library -sh $package)
echo "Doc:     "$(du -sh $package/doc)
echo "Library: "$(du -sh $package/library)

