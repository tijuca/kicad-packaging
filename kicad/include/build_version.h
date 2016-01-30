/* Date for kicad build version */

#ifndef KICAD_BUILD_VERSION
#define KICAD_BUILD_VERSION

COMMON_GLOBL wxString g_BuildVersion
#ifdef EDA_BASE
#  ifdef HAVE_SVN_VERSION
#    include "config.h"
     (wxT(KICAD_SVN_VERSION))
#  else
     (wxT("(20080825c)")) /* main program version */
#  endif
#endif
;

COMMON_GLOBL wxString g_BuildAboutVersion
#ifdef EDA_BASE
#  if defined(HAVE_SVN_VERSION) || defined(HAVE_SVN_REVISION)
#    include "config.h"
     (wxT(KICAD_ABOUT_VERSION))
#  else
     (wxT("(20080825c-final)")) /* svn date & rev (normally overridden) */
#  endif
#endif
;


#endif	// KICAD_BUILD_VERSION
