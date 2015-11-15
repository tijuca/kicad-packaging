#!/bin/sh

repo=lp:kicad/4.0
tag=4.0.0-rc1
version=4.0.0~rc1

echo "========== Checking the tag of version $version ==================="
foundTag=$(bzr log $repo 2>/dev/null | head -n 10 | grep "tags: $tag" | awk '{print $2}')
if [ "$foundTag" = "$tag" ]; then
    echo "Good! found tag: $foundTag"
else
    echo "The tag in this repository is not which was expected: $tag != $foundTag"
    exit 1
fi

package=kicad-$version
origtarxz=kicad_$version.orig.tar.xz
rm -rf $package $origtargz

echo "========== Getting the core (version $version) ==========="
echo -n "BZR export: $repo ... "
bzr export $package  $repo 2>/dev/null
echo "[Done]."
echo "========== Downloading documentation ====================="
echo -n "Download: kicad-doc ... "
project=kicad-doc
wget --quiet https://github.com/KiCad/$project/archive/master.tar.gz
mkdir $package/doc
tar xzf master.tar.gz -C $package/doc
echo "$(date +%Y%m%d): downloaded https://github.com/KiCad/$project/archive/master.tar.gz" >> $package/doc/download.txt
rm master.tar.gz
echo "[Done]."
echo "====== Getting libraries (common and Pretty files) ======="
PRETTY_REPOS=$(curl https://api.github.com/orgs/KiCad/repos?per_page=2000 2> /dev/null | sed -r 's:.+ "full_name".*"KiCad/(.+\.pretty)",:\1:p;d')
echo "adding repositories TO_SOT_Packages_THT.pretty and TO_SOT_Packages_SMD.pretty"
PRETTY_REPOS=$PRETTY_REPOS TO_SOT_Packages_THT.pretty TO_SOT_Packages_SMD.pretty
PRETTY_REPOS=$(echo $PRETTY_REPOS | tr " " "\n" | sort)
mkdir $package/library
for project in kicad-library $PRETTY_REPOS; do
    echo -n "Download: $project ... "
    wget --quiet https://github.com/KiCad/$project/archive/master.tar.gz
    tar xzf master.tar.gz -C $package/library
    echo "$(date +%Y%m%d): downloaded https://github.com/KiCad/$project/archive/master.tar.gz" >> $package/library/download.txt
    rm master.tar.gz
    echo "[Done]."
done
echo "========== Compressing the archive ======================="
size=$(du -s $package | awk '{print $1}')
tar cf - $package | bar -s ${size}k | lzma -z > $origtarxz
echo "Sizes:"
echo "Core:    "$(du --exclude=doc --exclude=library -sh $package)
echo "Doc:     "$(du -sh $package/doc)
echo "Library: "$(du -sh $package/library)
