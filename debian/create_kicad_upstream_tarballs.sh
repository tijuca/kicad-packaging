#!/bin/bash

# create_kicad_upstream_tarballs.sh
# Helper script for creating the needed main and additional tarballs for then
# kicad source package. Upstream is packing some sourceless and some non DFSG
# parts into their tarballs. As long as this happen we need to build own DFSG
# clean tarballs with a additional naming 'dfsg[version]'.
# The source is separated into the main tarball (1) and additional three
# parts (2-4):
#
# 1. The KiCad GUI soure      ->  kicad_[x]+dfsg[x].orig.tar.xz
# 2. The KiCad documentation  ->  kicad_[x]+dfsg[x].orig-doc.tar.xz
# 3. The KiCad localization   ->  kicad_[x]+dfsg[x].orig-18n.tar.xz
# 4. The KiCad libraries      ->  kicad_[x]+dfsg[x].orig-libraries.tar.xz
#
# This script is creating all needed tarballs and checks for non DFSG content.
# We can filter out more non DFSG related parts while importing with
# git-buildpackage. See debian/gbp.conf option 'filter'.q

# Initialize our own variables:
VERBOSE=0
CURDIR_FULL=`pwd`
CURDIR=$(basename `pwd`)
EXIT_SUCCESS=0
EXIT_FAILURE=1
DFSG=
WGET_OPTIONS="-nc -c -q"
# base package name
SRCPKG="kicad"

# local functions
usage () {
cat << EOF

Usage: ${0##*/} [-h|-v] VERSION

    -h         display this help and exit
    -v         verbose mode, increase the output messages

    [VERSION]  tagged upstream version of KiCad (like a tag '4.0.4')

Examples:
  ${0##*/} 4.0.4

    Download all the required upstream stuff silently as possible and prepares
    the needed tarballs in /tmp .

  ${0##*/} -v 4.0.4

    The same as above just with some more output while getting the various
    upstream archives and on local operations while preparing the needed
    tarballs. Mostly useful for debugging if something is going wrong.

EOF
}

debug () {
if [ "${VERBOSE}" = "1" ]; then
    echo "DEBUG -> $1"
fi
}

fail () {
    echo $*
    exit ${EXIT_FAILURE}
}

print_project_decoration () {
    echo "# ${1} #"
    printf '%*s' ${2} | tr ' ' '#'
    echo
}

check_sourceless_files () {
echo
echo "Checking for non DFSG compliant content in $@ ..."
SOURCE_SUFFIXES="odt odp odg odm sxw"
for SCANDIR in $@; do
    find ${SCANDIR} -name '*.pdf' -print |
    while read FOUND_FILE; do
        FOUND_SOURCE=0
        dir=$(dirname "${FOUND_FILE}")
        b=$(basename "${FOUND_FILE}")
        for SUFFIX in ${SOURCE_SUFFIXES}; do
            # replacing '*.pdf' with pattern
            SOURCE_PATTERN="${b/.pdf/*.${SUFFIX}}"
            # counting if we found a pattern match
            FOUND_PATTERN_COUNT=$(find ${SCANDIR} -iname "${SOURCE_PATTERN}")
            if [ -n "${FOUND_PATTERN_COUNT}" ]; then
                ## yes, we found a source file
                FOUND_SOURCE=1
            fi
        done
        if [ -n "$(find ${SCANDIR} -iname '${b/.pdf/}')" ]; then
            ## we found a directory
            FOUND_SOURCE=1
        fi
        if [ "${FOUND_SOURCE}" = 0 ]; then
            debug "Erasing sources binary file: ${FOUND_FILE}" #| tee -a debian/erased_pdfs.log
            rm "${FOUND_FILE}"
            # ugly workaround, setting DFSG flag
            touch ${TMPDIR}/dfsg
        fi
    done
done
}

########################
# We are starting here #
########################

# check for wget and curl
test -f /usr/bin/wget || fail "wget is missing, please install first!"
test -f /usr/bin/curl || fail "curl is missing, please install first!"

# check if we are inside kicad/ and have a git environment
if [ "${CURDIR}" != "${SRCPKG}" ]; then
    echo "Not in ${SRCPKG}/.."
    echo "Your are in the correct directory? You currently in ${CURDIR_FULL} !"
    exit ${EXIT_FAILURE}
else
    if [ ! -d .git ]; then
        echo "no directory .git/ found! You are in the correct directory?"
        exit ${EXIT_FAILURE}
    fi
fi

# we have no options found?
if [ $(($#)) -lt 1 ]; then
    echo "You need at least one option!" >&2
    echo
    usage ${EXIT_FAILURE}
fi

OPTIND=1 # Reset is necessary if getopts was used previously in the script. It is a good idea to make this local in a function.
while getopts "hv?" opt; do
    case "$opt" in
        h)  HELP=1
            usage
            exit
            ;;
        v)  echo "[[ ... using verbose mode ... ]]"
            VERBOSE=1
            # make wget a bit more noisy
            WGET_OPTIONS="-nc -c"
            ;;
        '?')
            usage >&2
            exit 1
            ;;
    esac
done

# shift found options
shift $(( OPTIND - 1 ))

# looping the arguments, we should have at least only one without an option!
for ARG; do
    ARG_COUNT=`expr ${ARG_COUNT} + 1`
    debug "given argument: ${ARG}"
    debug "ARG_COUNT = ${ARG_COUNT}"
done

# if there is no argument left or given error out!
if [ "${ARG_COUNT}" = "0" -o "${ARG_COUNT}" = "" ]; then
    echo "missing argument for VERSION!"
    exit ${EXIT_FAILURE}

# we have to many arguments, error out
elif [ "${ARG_COUNT}" != "1" ]; then
    echo "${ARG_COUNT}"
    echo "more than one argument for VERSION given!"
    exit ${EXIT_FAILURE}
fi

# o.k. the last argument should be the version
VERSION=${ARG}
# check for a valid version
if [[ ${VERSION} = *.*.* ]]; then
    # splitting the given version
    KICAD_MINOR_MICRO=${VERSION#*.}
    KICAD_MAJOR=${VERSION%.*.*}
    KICAD_MICRO=${KICAD_MINOR_MICRO#*.}
    KICAD_MINOR=${KICAD_MINOR_MICRO%.*}
else
    echo "The given Version '${VERSION}' looks quite unusual!"
    echo "Will do nothing!"
    exit ${EXIT_FAILURE}
fi
# more insane checking
if [ "${KICAD_MAJOR}" = "" -o "${KICAD_MINOR}" = "" -o "${KICAD_MICRO}" = "" ]; then
    echo "There is something wrong with the given version!"
    exit ${EXIT_FAILURE}
fi

TMPDIR=/tmp/${SRCPKG}-tmp
TARBALLDIR=${TMPDIR}/tarballs
UPSTREAM_LIBRARIES=${TMPDIR}/libraries

# create temporary folder for data collection
mkdir -p ${TMPDIR}
mkdir -p ${TARBALLDIR}
mkdir -p ${UPSTREAM_LIBRARIES}

echo

#######################################################
# collecting and preparing data for KiCad GUI tarball #
#######################################################

# we got a URL which uses Major.Minor subdirectories e.g.
# https://launchpad.net/kicad/4.0/4.0.4/+download/kicad-4.0.4.tar.xz
KICAD_MAIN_DL="https://launchpad.net/${SRCPKG}/${KICAD_MAJOR}.${KICAD_MINOR}/${KICAD_MAJOR}.${KICAD_MINOR}.${KICAD_MICRO}/+download/${SRCPKG}-${KICAD_MAJOR}.${KICAD_MINOR}.${KICAD_MICRO}.tar.xz"
KICAD_UPSTREAM_TARBALL=${TARBALLDIR}/${SRCPKG}-${VERSION}.tar.xz

echo "Trying to load KiCad version ${VERSION} ..."
wget ${WGET_OPTIONS} ${KICAD_MAIN_DL} -O ${KICAD_UPSTREAM_TARBALL}
debug "create ${TMPDIR}/${SRCPKG}-${VERSION}"
mkdir -p ${TMPDIR}/${SRCPKG}-${VERSION}
debug "extracting ${KICAD_UPSTREAM_TARBALL} into ${TMPDIR}/${SRCPKG}-${VERSION} ... "
rm -rf ${TMPDIR}/${SRCPKG}-${VERSION}/*
tar xJf ${KICAD_UPSTREAM_TARBALL} --strip-components=1 -C ${TMPDIR}/${SRCPKG}-${VERSION}
echo "Done"

# checking if we need DFSG cleanout, some binary documents may have no sources
check_sourceless_files ${TMPDIR}/${SRCPKG}-${VERSION}/Documentation ${TMPDIR}/${SRCPKG}-${VERSION}/pcb_calculator
if [ -f ${TMPDIR}/dfsg ]; then
    DFSG="+dfsg1"
fi

# checking for CC-BY-2.0 licensed files, they not DFSG compatible
echo "Searching for CC-BY-2.0 licensed files ..."
for CC2_FILE_A in $(grep -l 'creativecommons.*by.*2\.0' ${TMPDIR}/${SRCPKG}-${VERSION}/bitmaps_png/sources/*.svg); do
    rm ${CC2_FILE_A}
    CC2_FILE_B=$(basename ${CC2_FILE_A}| sed 's/svg$/cpp/')
    find ${TMPDIR}/${SRCPKG}-${VERSION}/bitmaps_png -name ${CC2_FILE_B} -exec rm {} \;
    debug "removed ${CC2_FILE_A}"
    debug "removed ${TMPDIR}/${SRCPKG}-${VERSION}/bitmaps_png/cpp_26/${CC2_FILE_B}"
done
echo

#################################################################
# collecting and preparing data for KiCad documentation tarball #
#################################################################

# The documentation repository is on GitHub and got tags we can use for
# downloading. e.g. https://github.com/KiCad/kicad-doc/archive/4.0.4.tar.gz

KICAD_DOC_DL="github.com/KiCad/${SRCPKG}-doc/archive/${KICAD_MAJOR}.${KICAD_MINOR}.${KICAD_MICRO}.tar.gz"
KICAD_DOC_TARBALL=${TARBALLDIR}/${SRCPKG}-doc-${VERSION}.tar.gz

echo "Trying to load KiCad Documentation ${VERSION} ..."
wget ${WGET_OPTIONS} ${KICAD_DOC_DL} -O ${KICAD_DOC_TARBALL}
debug "create ${TMPDIR}/doc"
mkdir -p ${TMPDIR}/doc
debug "extracting ${KICAD_DOC_TARBALL} into ${TMPDIR}/doc ... "
rm -rf ${TMPDIR}/doc/*
tar xzf ${KICAD_DOC_TARBALL} --strip-components=1 -C ${TMPDIR}/doc
echo "Done"

# checking if we need DFSG cleanout, some binary documents may have no sources
check_sourceless_files ${TMPDIR}/doc
if [ -f ${TMPDIR}/dfsg ]; then
    DFSG="+dfsg1"
fi
echo

###########################################################
# collecting and preparing data for the KiCad 18n tarball #
###########################################################

# The i18n repository is on GitHub and got tags we can use for
# downloading. e.g. https://github.com/KiCad/kicad-i18n/archive/4.0.4.tar.gz

KICAD_I18N_DL="github.com/KiCad/${SRCPKG}-i18n/archive/${KICAD_MAJOR}.${KICAD_MINOR}.${KICAD_MICRO}.tar.gz"
KICAD_I18N_TARBALL=${TARBALLDIR}/${SRCPKG}-i18n-${VERSION}.tar.gz

echo "Trying to load KiCad i18n ${VERSION} ..."
wget ${WGET_OPTIONS} ${KICAD_I18N_DL} -O ${KICAD_I18N_TARBALL}
debug "create ${TMPDIR}/i18n"
mkdir -p ${TMPDIR}/i18n
debug "extracting ${KICAD_I18N_TARBALL} into ${TMPDIR}/i18n ... "
rm -rf ${TMPDIR}/i18n/*
tar xzf ${KICAD_I18N_TARBALL} --strip-components=1 -C ${TMPDIR}/i18n
echo "Done"

# checking if we need DFSG cleanout, some binary documents may have no sources
check_sourceless_files ${TMPDIR}/i18n
if [ -f ${TMPDIR}/dfsg ]; then
    DFSG="+dfsg1"
fi
echo

#################################################################
# collecting and preparing data for the KiCad libraries tarball #
#################################################################

# The various component, footprint and 3D-model libraries are available one
# GitHub in various repositories. Uncomment the following two lines to get
# a dynamically created list of repositories. Otherwise it's currently better
# touse a the static list as some repositories currently empty like:
# Connectors_Amphenol.pretty
# Connectors_TE-Connectivity.pretty

#PRETTY_REPOS=$(curl https://api.github.com/orgs/KiCad/repos?per_page=2000 2> /dev/null | sed -r 's:.+ "full_name".*"KiCad/(.+\.pretty)",:\1:p;d')
#PRETTY_REPOS=$(echo $PRETTY_REPOS | tr " " "\n" | sort)

PRETTY_REPOS="\
Air_Coils_SML_NEOSID.pretty
Battery_Holders.pretty
Buttons_Switches_SMD.pretty
Buttons_Switches_ThroughHole.pretty
Buzzers_Beepers.pretty
Capacitors_SMD.pretty
Capacitors_Tantalum_SMD.pretty
Capacitors_ThroughHole.pretty
Choke_Axial_ThroughHole.pretty
Choke_Common-Mode_Wurth.pretty
Choke_Radial_ThroughHole.pretty
Choke_SMD.pretty
Choke_Toroid_ThroughHole.pretty
Connectors_Harwin.pretty
Connectors_Hirose.pretty
Connectors_JAE.pretty
Connectors_JST.pretty
Connectors_Molex.pretty
Connectors_Multicomp.pretty
Connectors_Phoenix.pretty
Connectors_Samtec.pretty
Connect.pretty
Converters_DCDC_ACDC.pretty
Crystals.pretty
Diodes_SMD.pretty
Diodes_ThroughHole.pretty
Discret.pretty
Display.pretty
Displays_7-Segment.pretty
Divers.pretty
EuroBoard_Outline.pretty
Fiducials.pretty
Filters_HF_Coils_NEOSID.pretty
Fuse_Holders_and_Fuses.pretty
Hall-Effect_Transducers_LEM.pretty
Heatsinks.pretty
Housings_DFN_QFN.pretty
Housings_DIP.pretty
Housings_QFP.pretty
Housings_SIP.pretty
Housings_SOIC.pretty
Housings_SSOP.pretty
Inductors_NEOSID.pretty
Inductors.pretty
IR-DirectFETs.pretty
Labels.pretty
LEDs.pretty
Measurement_Points.pretty
Measurement_Scales.pretty
Mechanical_Sockets.pretty
Microwave.pretty
Mounting_Holes.pretty
NF-Transformers_ETAL.pretty
Oddities.pretty
Opto-Devices.pretty
Oscillators.pretty
PFF_PSF_PSS_Leadforms.pretty
Pin_Headers.pretty
Potentiometers.pretty
Power_Integrations.pretty
Relays_ThroughHole.pretty
Resistors_SMD.pretty
Resistors_ThroughHole.pretty
Resistors_Universal.pretty
RF_Modules.pretty
Shielding_Cabinets.pretty
SMD_Packages.pretty
Sockets_BNC.pretty
Sockets_Mini-Universal.pretty
Sockets_MOLEX_KK-System.pretty
Sockets.pretty
Socket_Strips.pretty
Sockets_WAGO734.pretty
Symbols.pretty
Terminal_Blocks.pretty
TO_SOT_Packages_SMD.pretty
TO_SOT_Packages_THT.pretty
Transformers_CHK.pretty
Transformers_SMPS_ThroughHole.pretty
Transistors_OldSowjetAera.pretty
Valves.pretty
Varistors.pretty
Wire_Connections_Bridges.pretty
Wire_Pads.pretty
"

# uncomment the follwing variable if you not want to load the long list in
# case for testing something. Booth repositories are different as one has a
# taged versions, the other don't.
#PRETTY_REPOS="\
#Air_Coils_SML_NEOSID.pretty
#Battery_Holders.pretty"

for PROJECT in kicad-library $PRETTY_REPOS; do
#for PROJECT in $PRETTY_REPOS; do
    print_project_decoration $PROJECT $(expr 4 + ${#PROJECT})
    # check first if we can get a tagged archive, otherwise we take the current master
    RET=`curl -L --silent https://github.com/KiCad/${PROJECT}/tags | grep ${VERSION}.tar.gz | awk '{print $2}' | tr '<>"' ' ' | awk '{print $2}'`
    if [ "$RET" != "" ]; then
        echo "Trying to load tagged version ${VERSION}."
        wget ${WGET_OPTIONS} https://github.com/KiCad/${PROJECT}/archive/${VERSION}.tar.gz -O ${TARBALLDIR}/${PROJECT}-${VERSION}.tar.gz
        debug "create ${UPSTREAM_LIBRARIES}/${PROJECT}"
        mkdir -p ${UPSTREAM_LIBRARIES}/${PROJECT}
        debug "extracting ${TARBALLDIR}/${PROJECT}-${VERSION}.tar.gz into ${UPSTREAM_LIBRARIES}/${PROJECT} ... "
        rm -rf ${UPSTREAM_LIBRARIES}/${PROJECT}/*
        tar xzf ${TARBALLDIR}/${PROJECT}-${VERSION}.tar.gz --strip-components=1 -C ${UPSTREAM_LIBRARIES}/${PROJECT}
        echo "Done"
    else
        echo "No tagged version found, trying master instead."
        wget ${WGET_OPTIONS} https://github.com/KiCad/${PROJECT}/archive/master.tar.gz -O ${TARBALLDIR}/${PROJECT}.master.tar.gz
        debug "create ${UPSTREAM_LIBRARIES}/${PROJECT}"
        mkdir -p ${UPSTREAM_LIBRARIES}/${PROJECT}
        debug "extracting ${TARBALLDIR}/${PROJECT}.master.tar.gz into ${UPSTREAM_LIBRARIES}/${PROJECT} ... "
        rm -rf ${UPSTREAM_LIBRARIES}/${PROJECT}/*
        tar xzf ${TARBALLDIR}/${PROJECT}.master.tar.gz --strip-components=1 -C ${UPSTREAM_LIBRARIES}/${PROJECT}
        echo "Done"
    fi
    echo
done

# cleaning out VCS control files, don't care about not to scanned folders
echo "removing VCS related control files before packing tarballs ..."
for CLEANING in `find ${TMPDIR} -type f \( -name ".gitignore" -o -name "*.bzrignore" \)`; do
    debug "remove ${CLEANING}"
    rm ${CLEANING}
done
echo

#########################
# building the tarballs #
#########################

# create ${SRCPKG}_${VERSION}${DFSG}.orig-libraries.tar.xz
echo "building ${TMPDIR}/${SRCPKG}_${VERSION}${DFSG}.orig.tar.xz"
cd ${TMPDIR}/${SRCPKG}-${VERSION} ;\
rm -f ../${SRCPKG}_${VERSION}${DFSG}.orig.tar.xz ;\
tar -Jcf ../${SRCPKG}_${VERSION}${DFSG}.orig.tar.xz . ;\
#ls -la ../${SRCPKG}_${VERSION}${DFSG}.orig.tar.xz

# create ${SRCPKG}_${VERSION}${DFSG}.orig-libraries.tar.xz
echo "building ${TMPDIR}/${SRCPKG}_${VERSION}${DFSG}.orig-doc.tar.xz"
cd ${TMPDIR} ;\
rm -f ${SRCPKG}_${VERSION}${DFSG}.orig-doc.tar.xz ;\
tar -Jcf ${SRCPKG}_${VERSION}${DFSG}.orig-doc.tar.xz doc/ ;\
#ls -la ${SRCPKG}_${VERSION}${DFSG}.orig-doc.tar.xz

# create ${SRCPKG}_${VERSION}${DFSG}.orig-libraries.tar.xz
echo "building ${TMPDIR}/${SRCPKG}_${VERSION}${DFSG}.orig-i18n.tar.xz"
cd ${TMPDIR} ;\
rm -f ${SRCPKG}_${VERSION}${DFSG}.orig-i18n.tar.xz ;\
tar -Jcf ${SRCPKG}_${VERSION}${DFSG}.orig-i18n.tar.xz i18n/ ;\
#ls -la ${SRCPKG}_${VERSION}${DFSG}.orig-i18n.tar.xz

# create ${SRCPKG}_${VERSION}${DFSG}.orig-libraries.tar.xz
echo "building ${TMPDIR}/${SRCPKG}_${VERSION}${DFSG}.orig-libraries.tar.xz"
cd ${TMPDIR} ;\
tar -Jcf ${SRCPKG}_${VERSION}${DFSG}.orig-libraries.tar.xz libraries/ ;\
#ls -la ${SRCPKG}_${VERSION}${DFSG}.orig-libraries.tar.xz

exit ${EXIT_SUCCESS}
