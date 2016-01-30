		/**********************************************/
		/*	 Routines de gestion de la memoire		  */
		/**********************************************/

	/* Fichier memoire.cpp */

#include "fctsys.h"
#include "wxstruct.h"
#include "common.h"
#include "cvpcb.h"


/********************************/
void FreeMemoryModules(void)
/********************************/

/* Routine de liberation memoire de la liste des modules
	- remet a NULL BaseListePkg
	- remet a 0 nblib;
*/
{
STOREMOD * Module, * NextMod;

	if( BaseListePkg == NULL) return;

	for ( Module = BaseListePkg; Module != NULL; Module = NextMod)
	{
		NextMod = Module->Pnext;
		delete Module;
	}

	nblib = 0;
	BaseListePkg = NULL;
}

/***********************************/
void FreeMemoryComponants(void)
/***********************************/

/* Routine de liberation memoire de la liste des composants
	- remet a NULL BaseListeMod
	- remet a 0 nbcomp
*/
{
STORECMP * Cmp, * NextCmp;

	if( BaseListeCmp == NULL ) return;

	for( Cmp = BaseListeCmp; Cmp != NULL; Cmp = NextCmp )
	{
		NextCmp = Cmp->Pnext;
		delete Cmp;
	}

	nbcomp = 0;
	BaseListeCmp = NULL;
}

