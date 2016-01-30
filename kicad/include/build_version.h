/* Date for kicad build version */

#ifndef KICAD_BUILD_VERSION
#define KICAD_BUILD_VERSION

#define BUILD_VERSION wxT("(20090216-final)")

COMMON_GLOBL wxString g_BuildVersion
#ifdef EDA_BASE
#  ifdef HAVE_SVN_VERSION
#    include "config.h"
     (wxT(KICAD_SVN_VERSION))
#  else
     (BUILD_VERSION) /* main program version */
#  endif
#endif
;

COMMON_GLOBL wxString g_BuildAboutVersion
#ifdef EDA_BASE
#  if defined(HAVE_SVN_VERSION) || defined(HAVE_SVN_REVISION)
#    include "config.h"
     (wxT(KICAD_ABOUT_VERSION))
#  else
     (BUILD_VERSION) /* svn date & rev (normally overridden) */
#  endif
#endif
;


#endif	// KICAD_BUILD_VERSION
