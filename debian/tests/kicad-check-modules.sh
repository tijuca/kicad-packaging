#!/bin/bash

KICAD_MODULES="/usr/share/kicad/modules"
KICAD_TEMPLATE="/usr/share/kicad/template"

FPL_TABLE_PRETTY=$(cat ${KICAD_TEMPLATE}/fp-lib-table | tr '/' ' ' | tr ')(' ' ' | grep ".pretty" | awk '{print $8}')
FPL_TABLE_PRETTY_COUNT=$(grep -c ".pretty)" ${KICAD_TEMPLATE}/fp-lib-table)
FPL_TABLE_PRETTY_DIRECTORY_COUNT=$(find ${KICAD_MODULES} -type d -name "*pretty" | wc -l)
FPL_TABLE_PRETTY_LINK_COUNT=$(find ${KICAD_MODULES} -type l -name "*pretty" | wc -l)

# Using the from *current* installed changelog for assuming version(s).
CHANGELOG=${CHANGELOG:-"/usr/share/doc/kicad-common/changelog.Debian.gz"}
# And picking up the previous released version.
OLD_VERSION=$(zgrep "^kicad" "${CHANGELOG}" | head -n2 | tail -n1 | awk '{print $2}' | sed 's/(//;s/)//')
HOST_ARCH=$(dpkg-architecture -qDEB_HOST_ARCH)
KICAD_COMMON_OLD_DEB="kicad-common_${OLD_VERSION}_${HOST_ARCH}.deb"

show_additional_info() {
echo "Some additional statistics:"
echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~"
echo "Count actual footprint directories (*.pretty): ${FPL_TABLE_PRETTY_DIRECTORY_COUNT}"
echo "Count of actual entries in fp-lib-table      : ${FPL_TABLE_PRETTY_COUNT}"
echo "Count footprint symlinks (aka old folders)   : ${FPL_TABLE_PRETTY_LINK_COUNT}"
echo
if [ "${LINK_LIST}" != "" ]; then
    echo "Currently the following old footprint names are linked to new folders:"
    echo
    echo "${LINK_LIST}" | tr ':' '\n'
fi
}

echo
echo "++++++++++++++++++++++++++++++++++++++++++"
echo "+   Checking usability of kicad-common   +"
echo "++++++++++++++++++++++++++++++++++++++++++"

# Check if we have a acceptable count of Footprint Libraries.
echo
echo "Test: We have at least the same or a greater count of Footprint"
echo "Libraries as referenced in fp-lib-table?"
if [ "${FPL_TABLE_PRETTY_DIRECTORY_COUNT}" -ge "${FPL_TABLE_PRETTY_COUNT}" ]; then
    echo "    YES"
else
    echo "    NO"
    ERROR=1
fi

# Check if a referenced Footprint Library in fp-lib-table can be found.
echo
echo "Test: Are all referenced Footprint Libraries in fp-lib-table found?"
MISSING_FPL_TABLE_PRETTY_REFERENCE=""
for REFERENCE in ${FPL_TABLE_PRETTY}; do
    if [ ! -d "${KICAD_MODULES}/${REFERENCE}" ]; then
        MISSING_FPL_TABLE_PRETTY_REFERENCE="${MISSING_FPL_TABLE_PRETTY_REFERENCE} ${REFERENCE}"
        ERROR=1
    fi
done
if [ "${ERROR}" = "1" ]; then
    echo "    NO"
    for TABLE_PRETTY_REFERENCE in ${MISSING_FPL_TABLE_PRETTY_REFERENCE}; do
        echo "        ${TABLE_PRETTY_REFERENCE} couldn't be found!"
    done
else
    echo "    YES"
fi

if [ "${ERROR}" = "1" ]; then
    echo
    echo "++++++++++++++++++++++++++++++++++++++++++"
    echo "+    Further testing isn't meaningful!   +"
    echo "+     Please fix above issues first!     +"
    echo "++++++++++++++++++++++++++++++++++++++++++"
    echo
    show_additional_info
    exit 1
fi

# Check if Footprint Libraries from the previous version can be found.
if [ "${OLD_VERSION}" = "" ]; then
    echo
    echo "Failed to get the old version from kicad-common!"
    exit
fi
echo
echo "Test: Old references of fp-lib-table from ${OLD_VERSION} are still functional?"

if [ ! -f "/tmp/${KICAD_COMMON_OLD_DEB}" ]; then
    if ! wget -P /tmp "http://httpredir.debian.org/debian/pool/main/k/kicad/${KICAD_COMMON_OLD_DEB}" 2> wget.log; then
	cat wget.log
	exit
	fi
fi

rm -rf /tmp/kicad-common
dpkg -x "/tmp/${KICAD_COMMON_OLD_DEB}" /tmp/kicad-common

# Getting the old content of *.pretty folders.
OLD_FPL_TABLE_PRETTY=$(cd /tmp/kicad-common/usr/share/kicad/modules && ls -d *.pretty)

MISSING_PRETTY_LIBRARY=""
for OLD_PRETTY_ENTRY in ${OLD_FPL_TABLE_PRETTY}; do
    # Check if the old library is existing in new version, if empty the
    # old library isn't existing any longer nor is a symlink to some thing
    # equivalent and needs to be fixed!
    FOLDER=$(readlink -e "${KICAD_MODULES}/${OLD_PRETTY_ENTRY}")
    FOLDERCHECK="${FOLDER##*\/}"
    if [ "${FOLDERCHECK}" = "" ]; then
        MISSING_PRETTY_LIBRARY="${MISSING_PRETTY_LIBRARY} ${OLD_PRETTY_ENTRY}"
        ERROR=1
    else
        if [ "${OLD_PRETTY_ENTRY}" != "${FOLDERCHECK}" ]; then
        # The old library is linked to a new folder.
        LINK_LIST="${LINK_LIST}${OLD_PRETTY_ENTRY} -> ${FOLDERCHECK}:"
        fi
    fi
done

if [ "${ERROR}" = "1" ]; then
    echo "    NO"
    echo "        The following old folder(s) aren't linked to some valid library folder in the new version!"
    for ENTRY in ${MISSING_PRETTY_LIBRARY}; do
        echo "        ${ENTRY}"
    done
else
    echo "    YES"
fi

if [ "${ERROR}" = "1" ]; then
    echo
    echo "++++++++++++++++++++++++++++++++++++++++++"
    echo "+ At least one of the checks is failing! +"
    echo "+           Please fix this!             +"
    echo "++++++++++++++++++++++++++++++++++++++++++"
    echo
    show_additional_info
    exit 1
else
    echo
    echo "++++++++++++++++++++++++++++++"
    echo "+ All checks are successful! +"
    echo "+             Yeah!          +"
    echo "++++++++++++++++++++++++++++++"
    echo
    show_additional_info
fi

exit 0
