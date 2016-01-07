#! /bin/sh

# $1 == --upstream-version; $2 == <version-number>; $3 == <source_tarball>

if [ "$1" != --upstream-version ]; then
    echo '$1 must be --upstream-version; exiting.'
    exit 1
fi

package=$(echo $3 | sed 's%.*/\(.*\)_.*%\1%')
version=$2
debian_orig_tarball=$3

debian_newdir=${package}-${version}
pwd=$(pwd)

cd $(dirname $debian_orig_tarball)
wget http://downloads.kicad-pcb.org/libraries/kicad-library-${version}.tar.gz
wget http://downloads.kicad-pcb.org/libraries/kicad-footprints-${version}.tar.gz
wget http://downloads.kicad-pcb.org/docs/kicad-doc-${version}.tar.gz

tar xJf $(basename $debian_orig_tarball)
rm $(basename $debian_orig_tarball)
cd $debian_newdir
tar xzf ../kicad-library-${version}.tar.gz
mv kicad-library-${version} library
tar xzf ../kicad-footprints-${version}.tar.gz
mv kicad-footprints-${version} modules
tar xzf ../kicad-doc-${version}.tar.gz
mv kicad-doc-${version} doc

echo "made $debian_newdir, $debian_newdir/library, $debian_newdir/modules, $debian_newdir/doc"
echo "========== Compressing the archive ======================="
size=$(du -s . | awk '{print $1}')
cd ..
tar cf - $debian_newdir | bar -s ${size}k | lzma -z > $(basename $debian_orig_tarball)
echo "Sizes:"
echo "Core:    "$(du --exclude=doc --exclude=library --exclude modules -sh $debian_newdir)
echo "Doc:     "$(du -sh $debian_newdir/doc)
echo "Library: "$(du -sh $debian_newdir/library)


