	/********************/
	/* includes systeme */
	/********************/
#ifndef FCTSYS_H
#define FCTSYS_H

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <stdio.h>
#ifdef __FreeBSD__
#include <stdlib.h>
#else
# ifndef __DARWIN__
# include <malloc.h>		// MacOSX (DARWIN): malloc() and free() are in stdlib.h
#endif
#endif
#include <time.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#ifdef __WXMAC__
#include <Carbon/Carbon.h>
#endif

#ifndef M_PI
#define M_PI 3.141592653
#endif

#define PCB_INTERNAL_UNIT 10000		//  PCBNEW internal unit = 1/10000 inch
#define EESCHEMA_INTERNAL_UNIT 1000	//  EESCHEMA internal unit = 1/1000 inch

#include "wxstruct.h"
#include "gr_basic.h"

#ifdef __UNIX__
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif

#ifdef __WINDOWS__
#define DIR_SEP '\\'
#define STRING_DIR_SEP wxT("\\")
#else
#define DIR_SEP '/'
#define STRING_DIR_SEP wxT("/")
#endif

#define UNIX_STRING_DIR_SEP wxT("/")
#define WIN_STRING_DIR_SEP wxT("\\")


#endif /* FCTSYS_H */


