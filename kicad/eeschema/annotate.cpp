/**************************************/
/* annotate.cpp: component annotation */
/**************************************/

#include "fctsys.h"
#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "netlist.h"
#include "protos.h"

/* Local Functions*/
static int  ListeComposants( CmpListStruct* BaseListeCmp,
                             DrawSheetPath* sheet );
static void BreakReference( CmpListStruct* BaseListeCmp, int NbOfCmp );
static void ReAnnotateComponents( CmpListStruct* BaseListeCmp, int NbOfCmp );
static void ComputeReferenceNumber( CmpListStruct* BaseListeCmp, int NbOfCmp );
int         GetLastReferenceNumber( CmpListStruct* Objet,
                                    CmpListStruct* BaseListeCmp,
                                    int            NbOfCmp );
static int  ExistUnit( CmpListStruct* Objet, int Unit,
                       CmpListStruct* BaseListeCmp, int NbOfCmp );


/******************************************************/
void WinEDA_SchematicFrame::UpdateSheetNumberAndDate()
/******************************************************/

/* Set a sheet number, the sheet count for sheets in the whole schematic
 * and update the date in all screens
 */
{
    wxString       date = GenDate();
    EDA_ScreenList s_list;

    // Set the date
    for ( SCH_SCREEN * screen = s_list.GetFirst(); screen != NULL; screen = s_list.GetNext() )
        screen->m_Date = date;

    // Set sheet counts
    SetSheetNumberAndCount();
}


/*****************************************************************************
 * Used to annotate the power symbols, before testing erc or computing
 * netlist when a component reannotation is not necessary
 *
 * In order to avoid conflicts the reference number starts with a 0. A
 * PWR with id 12 is named PWR12 in global annotation and PWR012 by the
 * Power annotation.
 ****************************************************************************/
void ReAnnotatePowerSymbolsOnly( void )
{
    /* Build the screen list (screen, not sheet) */
    EDA_SheetList  SheetList( NULL );


    DrawSheetPath* sheet;
    int            CmpNumber = 1;

    for( sheet = SheetList.GetFirst();
        sheet != NULL;
        sheet = SheetList.GetNext() )
    {
        EDA_BaseStruct* DrawList = sheet->LastDrawList();
        for( ; DrawList != NULL; DrawList = DrawList->Pnext )
        {
            if( DrawList->Type() != TYPE_SCH_COMPONENT )
                continue;
            SCH_COMPONENT*          DrawLibItem =
                (SCH_COMPONENT*) DrawList;
            EDA_LibComponentStruct* Entry =
                FindLibPart(
                    DrawLibItem->m_ChipName.GetData(), wxEmptyString,
                    FIND_ROOT );
            if( (Entry == NULL) || (Entry->m_Options != ENTRY_POWER) )
                continue;

            //DrawLibItem->ClearAnnotation(sheet); this clears all annotation :(
            wxString refstr = DrawLibItem->m_PrefixString;

            //str will be "C?" or so after the ClearAnnotation call.
            while( refstr.Last() == '?' )
                refstr.RemoveLast();

            if( !refstr.StartsWith( wxT( "#" ) ) )
                refstr = wxT( "#" ) + refstr;
            refstr << wxT( "0" ) << CmpNumber;
            DrawLibItem->SetRef( sheet, refstr );
            CmpNumber++;
        }
    }
}


/* qsort function to annotate items by their position.
 *  Components are sorted
 *      by reference
 *      if same reference: by sheet
 *          if same sheet, by X pos
 *          	  if same X pos, by Y pos
 *          		if same Y pos, by time stamp
 */
int AnnotateBy_X_Position( const void* o1, const void* o2 )
{
    CmpListStruct* item1 = (CmpListStruct*) o1;
    CmpListStruct* item2 = (CmpListStruct*) o2;

    int            ii = strnicmp( item1->m_TextRef, item2->m_TextRef, 32 );

    if( ii == 0 )
        ii = item1->m_SheetList.Cmp( item2->m_SheetList );
    if( ii == 0 )
        ii = item1->m_Pos.x - item2->m_Pos.x;
    if( ii == 0 )
        ii = item1->m_Pos.y - item2->m_Pos.y;
    if( ii == 0 )
        ii = item1->m_TimeStamp - item2->m_TimeStamp;

    return ii;
}


/* qsort function to annotate items by their position.
 *  Components are sorted
 *      by reference
 *      if same reference: by sheet
 *          if same sheet, by Y pos
 *          	  if same Y pos, by X pos
 *          		if same X pos, by time stamp
 */
int AnnotateBy_Y_Position( const void* o1, const void* o2 )
{
    CmpListStruct* item1 = (CmpListStruct*) o1;
    CmpListStruct* item2 = (CmpListStruct*) o2;

    int            ii = strnicmp( item1->m_TextRef, item2->m_TextRef, 32 );

    if( ii == 0 )
        ii = item1->m_SheetList.Cmp( item2->m_SheetList );
    if( ii == 0 )
        ii = item1->m_Pos.y - item2->m_Pos.y;
    if( ii == 0 )
        ii = item1->m_Pos.x - item2->m_Pos.x;
    if( ii == 0 )
        ii = item1->m_TimeStamp - item2->m_TimeStamp;

    return ii;
}


/*****************************************************************************
* qsort function to annotate items by value
*  Components are sorted
*      by reference
*      if same reference: by value
*          if same value: by unit number
*              if same unit number, by sheet
*                  if same sheet, by time stamp
*****************************************************************************/
int AnnotateByValue( const void* o1, const void* o2 )
{
    CmpListStruct* item1 = (CmpListStruct*) o1;
    CmpListStruct* item2 = (CmpListStruct*) o2;

    int            ii = strnicmp( item1->m_TextRef, item2->m_TextRef, 32 );

    if( ii == 0 )
        ii = strnicmp( item1->m_TextValue, item2->m_TextValue, 32 );
    if( ii == 0 )
        ii = item1->m_Unit - item2->m_Unit;
    if( ii == 0 )
        ii = item1->m_SheetList.Cmp( item2->m_SheetList );
    if( ii == 0 )
        ii = item1->m_Pos.x - item2->m_Pos.x;
    if( ii == 0 )
        ii = item1->m_Pos.y - item2->m_Pos.y;
    if( ii == 0 )
        ii = item1->m_TimeStamp - item2->m_TimeStamp;

    return ii;
}


/**************************************************************************************/
void WinEDA_SchematicFrame::DeleteAnnotation( bool aCurrentSheetOnly, bool aRedraw )
/**************************************************************************************/

/** Function DeleteAnnotation
 * Remove current component annotations
 * @param aCurrentSheetOnly : if false: remove all annotations, else remove annotation relative to the current sheet only
 * @param aRedraw : true to refresh display
 */
{
    EDA_BaseStruct* strct;
    SCH_SCREEN*     screen;
    EDA_ScreenList  ScreenList;

    screen = ScreenList.GetFirst();

    if( aCurrentSheetOnly )
        screen = GetScreen();

    if( screen == NULL )
        return;
    while( screen )
    {
        strct = screen->EEDrawList;
        for( ; strct; strct = strct->Pnext )
        {
            if( strct->Type() == TYPE_SCH_COMPONENT )
            {
                if( aCurrentSheetOnly )
                    ( (SCH_COMPONENT*) strct )->ClearAnnotation( m_CurrentSheet );
                else
                    ( (SCH_COMPONENT*) strct )->ClearAnnotation( NULL );
            }
        }

        screen->SetModify();
        if( aCurrentSheetOnly )
            break;
        screen = ScreenList.GetNext();
    }


    //update the References
    m_CurrentSheet->UpdateAllScreenReferences();

    if( aRedraw )
        DrawPanel->Refresh( true );
}


/*****************************************************************************
* AnnotateComponents:
*
*  Compute the annotation of the components for the whole project, or the
*  current sheet only.  All the components or the new ones only will be
*  annotated.
* @param parent = Schematic frame
* @param annotateSchematic : true = entire schematic annotation, false = current scheet only
* @param sortOption : 0 = annotate by sorting X position,
*                     1 = annotate by sorting Y position,
*                     2 = annotate by sorting value
* @param resetAnnotation : true = remove previous annotation false = anotate new components only
*****************************************************************************/
void AnnotateComponents( WinEDA_SchematicFrame* parent,
                         bool                   annotateSchematic,
                         int                    sortOption,
                         bool                   resetAnnotation )
{
    int            ii, NbOfCmp;
    DrawSheetPath* sheet;
    CmpListStruct* BaseListeCmp;

    wxBusyCursor   dummy;

    /* If it is an annotation for all the components, reset previous
     * annotation: */
    if( resetAnnotation )
        parent->DeleteAnnotation( !annotateSchematic, false );

    /* Build the sheet list */
    EDA_SheetList SheetList( g_RootSheet );

    /* Update the sheet number, sheet count and date */
    parent->UpdateSheetNumberAndDate();

    /* First pass: Component counting */
    ii    = 0;
    sheet = parent->GetSheet();
    if( annotateSchematic )
    {
        NbOfCmp = 0;
        for( sheet = SheetList.GetFirst();
            sheet != NULL;
            sheet = SheetList.GetNext() )
            NbOfCmp += ListeComposants( NULL, sheet );
    }
    else
        NbOfCmp = ListeComposants( NULL, sheet );

    if( NbOfCmp == 0 )
        return;

    BaseListeCmp = (CmpListStruct*) MyZMalloc( NbOfCmp * sizeof(CmpListStruct) );

    /* Second pass : Init data tables */
    if( annotateSchematic )
    {
        ii = 0;
        for( sheet = SheetList.GetFirst();
            sheet != NULL;
            sheet = SheetList.GetNext() )
            ii += ListeComposants( BaseListeCmp + ii, sheet );
    }
    else
        ii = ListeComposants( BaseListeCmp, sheet );

    if( ii != NbOfCmp )
        DisplayError( parent, wxT( "Internal error in AnnotateComponents()" ) );

    /* Break full components reference in name (prefix) and number:
     * example: IC1 become IC, and 1 */
    BreakReference( BaseListeCmp, NbOfCmp );

    switch( sortOption )
    {
    case 0:
        qsort( BaseListeCmp, NbOfCmp, sizeof(CmpListStruct),
            ( int( * ) ( const void*, const void* ) )AnnotateBy_X_Position );
        break;

    case 1:
        qsort( BaseListeCmp, NbOfCmp, sizeof(CmpListStruct),
            ( int( * ) ( const void*, const void* ) )AnnotateBy_Y_Position );
        break;

    case 2:
        qsort( BaseListeCmp, NbOfCmp, sizeof(CmpListStruct),
            ( int( * ) ( const void*, const void* ) )AnnotateByValue );
        break;
    }

    /* Recalculate reference numbers */
    ComputeReferenceNumber( BaseListeCmp, NbOfCmp );
    ReAnnotateComponents( BaseListeCmp, NbOfCmp );

    MyFree( BaseListeCmp );
    BaseListeCmp = NULL;

    /* Final control */
    CheckAnnotate( parent, !annotateSchematic );
    parent->DrawPanel->Refresh( true );
}


/*****************************************************************************
* if BaseListeCmp == NULL : count components
*  else update data table BaseListeCmp
*****************************************************************************/
int ListeComposants( CmpListStruct* BaseListeCmp, DrawSheetPath* sheet )
{
    int                     NbrCmp   = 0;
    EDA_BaseStruct*         DrawList = sheet->LastDrawList();
    SCH_COMPONENT*          DrawLibItem;
    EDA_LibComponentStruct* Entry;

    for(  ; DrawList;   DrawList = DrawList->Pnext )
    {
        if( DrawList->Type() == TYPE_SCH_COMPONENT )
        {
            DrawLibItem = (SCH_COMPONENT*) DrawList;
            Entry = FindLibPart( DrawLibItem->m_ChipName.GetData(),
                wxEmptyString,
                FIND_ROOT );
            if( Entry == NULL )
                continue;

            if( BaseListeCmp == NULL )      /* Items counting only */
            {
                NbrCmp++;
                continue;
            }

            BaseListeCmp[NbrCmp].m_Cmp         = DrawLibItem;
            BaseListeCmp[NbrCmp].m_NbParts     = Entry->m_UnitCount;
            BaseListeCmp[NbrCmp].m_Unit        = DrawLibItem->GetUnitSelection( sheet ); // DrawLibItem->m_Multi;
            BaseListeCmp[NbrCmp].m_PartsLocked = Entry->m_UnitSelectionLocked;
            BaseListeCmp[NbrCmp].m_SheetList   = *sheet;
            BaseListeCmp[NbrCmp].m_IsNew       = FALSE;
            BaseListeCmp[NbrCmp].m_Pos = DrawLibItem->m_Pos;
            BaseListeCmp[NbrCmp].m_TimeStamp = DrawLibItem->m_TimeStamp;

            if( DrawLibItem->GetRef( sheet ).IsEmpty() )
                DrawLibItem->SetRef( sheet, wxT( "DefRef?" ) );

            strncpy( BaseListeCmp[NbrCmp].m_TextRef,
                CONV_TO_UTF8( DrawLibItem->GetRef( sheet ) ), 32 );

            BaseListeCmp[NbrCmp].m_NumRef = -1;

            if( DrawLibItem->m_Field[VALUE].m_Text.IsEmpty() )
                DrawLibItem->m_Field[VALUE].m_Text = wxT( "~" );

            strncpy( BaseListeCmp[NbrCmp].m_TextValue,
                CONV_TO_UTF8( DrawLibItem->m_Field[VALUE].m_Text ), 32 );
            NbrCmp++;
        }
    }

    return NbrCmp;
}


/*****************************************************************************
* Update the reference component for the schematic project (or the current
* sheet)
*****************************************************************************/
static void ReAnnotateComponents( CmpListStruct* BaseListeCmp, int NbOfCmp )
{
    int            ii;
    char*          Text;
    SCH_COMPONENT* DrawLibItem;

    /* Reattribution des numeros */
    for( ii = 0; ii < NbOfCmp; ii++ )
    {
        Text = BaseListeCmp[ii].m_TextRef;
        DrawLibItem = BaseListeCmp[ii].m_Cmp;

        if( BaseListeCmp[ii].m_NumRef < 0 )
            strcat( Text, "?" );
        else
            sprintf( Text + strlen( Text ), "%d", BaseListeCmp[ii].m_NumRef );

        DrawLibItem->SetRef( &(BaseListeCmp[ii].m_SheetList),
            CONV_FROM_UTF8( Text ) );
        DrawLibItem->m_Multi = BaseListeCmp[ii].m_Unit;
        DrawLibItem->SetUnitSelection( &(BaseListeCmp[ii].m_SheetList), DrawLibItem->m_Multi );
    }
}


/*****************************************************************************
* Split component reference designators into a name (prefix) and number.
* Example: IC1 becomes IC and 1 in the .m_NumRef member.
* For multi part per package components not already annotated, set .m_Unit
* to a max value (0x7FFFFFFF).
*
* @param BaseListeCmp = list of component
* @param NbOfCmp   = item count in the list
*****************************************************************************/
void BreakReference( CmpListStruct* BaseListeCmp, int NbOfCmp )
{
    int   ii, ll;
    char* Text;

    for( ii = 0; ii < NbOfCmp; ii++ )
    {
        BaseListeCmp[ii].m_NumRef = -1;
        Text = BaseListeCmp[ii].m_TextRef;
        ll   = strlen( Text ) - 1;
        if( Text[ll] == '?' )
        {
            BaseListeCmp[ii].m_IsNew = true;
            if( !BaseListeCmp[ii].m_PartsLocked )
                BaseListeCmp[ii].m_Unit = 0x7FFFFFFF;
            Text[ll] = 0;
            continue;
        }

        if( isdigit( Text[ll] ) == 0 )
        {
            BaseListeCmp[ii].m_IsNew = true;
            if( !BaseListeCmp[ii].m_PartsLocked )
                BaseListeCmp[ii].m_Unit = 0x7FFFFFFF;
            continue;
        }

        while( ll >= 0 )
        {
            if( (Text[ll] <= ' ' ) || isdigit( Text[ll] ) )
                ll--;
            else
            {
                if( isdigit( Text[ll + 1] ) )
                    BaseListeCmp[ii].m_NumRef = atoi( &Text[ll + 1] );
                Text[ll + 1] = 0;
                break;
            }
        }
    }
}


/*****************************************************************************
* Compute the reference number for components without reference number
*  Compute .m_NumRef member
*****************************************************************************/
static void ComputeReferenceNumber( CmpListStruct* BaseListeCmp, int NbOfCmp )
{
    int            ii, jj, LastReferenceNumber, NumberOfUnits, Unit;
    const char*    Text, * RefText, * ValText;
    CmpListStruct* ObjRef, * ObjToTest;

    /* Components with an invisible reference (power...) always are
     * re-annotated */
    for( ii = 0; ii < NbOfCmp; ii++ )
    {
        Text = BaseListeCmp[ii].m_TextRef;
        if( *Text == '#' )
        {
            BaseListeCmp[ii].m_IsNew  = true;
            BaseListeCmp[ii].m_NumRef = 0;
        }
    }

    ValText = RefText = ""; LastReferenceNumber = 1;
    for( ii = 0; ii < NbOfCmp; ii++ )
    {
        ObjRef = &BaseListeCmp[ii];
        if( BaseListeCmp[ii].m_Flag )
            continue;

        Text = BaseListeCmp[ii].m_TextRef;
        if( strnicmp( RefText, Text, 32 ) != 0 ) /* Nouveau Identificateur */
        {
            RefText = BaseListeCmp[ii].m_TextRef;
            LastReferenceNumber = GetLastReferenceNumber( BaseListeCmp + ii,
                BaseListeCmp,
                NbOfCmp );
        }

        /* Annotation of one part per package components (trivial case)*/
        if( BaseListeCmp[ii].m_NbParts <= 1 )
        {
            if( BaseListeCmp[ii].m_IsNew )
            {
                LastReferenceNumber++;
                BaseListeCmp[ii].m_NumRef = LastReferenceNumber;
            }
            BaseListeCmp[ii].m_Unit  = 1;
            BaseListeCmp[ii].m_Flag  = 1;
            BaseListeCmp[ii].m_IsNew = FALSE;
            continue;
        }

        /* Annotation of multi-part components ( n parts per package )
         * (complex case) */
        ValText = BaseListeCmp[ii].m_TextValue;
        NumberOfUnits = BaseListeCmp[ii].m_NbParts;

        if( BaseListeCmp[ii].m_IsNew )
        {
            LastReferenceNumber++;
            BaseListeCmp[ii].m_NumRef = LastReferenceNumber;

            if( !BaseListeCmp[ii].m_PartsLocked )
                BaseListeCmp[ii].m_Unit = 1;
            BaseListeCmp[ii].m_Flag = 1;
        }

        for( Unit = 1; Unit <= NumberOfUnits; Unit++ )
        {
            if( BaseListeCmp[ii].m_Unit == Unit )
                continue;
            jj = ExistUnit( BaseListeCmp + ii, Unit, BaseListeCmp, NbOfCmp );
            if( jj >= 0 )
                continue; /* Unit exists for this reference */

            /* Search a component to annotate ( same prefix, same value) */
            for( jj = ii + 1; jj < NbOfCmp; jj++ )
            {
                ObjToTest = &BaseListeCmp[jj];
                if( BaseListeCmp[jj].m_Flag )
                    continue;
                Text = BaseListeCmp[jj].m_TextRef;
                if( strnicmp( RefText, Text, 32 ) != 0 )
                    continue; // references are different
                Text = BaseListeCmp[jj].m_TextValue;
                if( strnicmp( ValText, Text, 32 ) != 0 )
                    continue; // values are different
                if( !BaseListeCmp[jj].m_IsNew )
                {
                    continue;
                }

                /* Component without reference number found, annotate it if
                 * possible */
                if( !BaseListeCmp[jj].m_PartsLocked
                   || (BaseListeCmp[jj].m_Unit == Unit) )
                {
                    BaseListeCmp[jj].m_NumRef = BaseListeCmp[ii].m_NumRef;
                    BaseListeCmp[jj].m_Unit   = Unit;
                    BaseListeCmp[jj].m_Flag   = 1;
                    BaseListeCmp[jj].m_IsNew  = FALSE;
                    break;
                }
            }
        }
    }
}


/*****************************************************************************
* Search the last used (greatest) reference number in the component list
* for the prefix reference given by Objet
* The component list must be sorted.
*
* @param Objet = reference item ( Objet->m_TextRef is the search pattern)
* @param BaseListeCmp = list of items
* @param NbOfCmp = items count in list of items
*****************************************************************************/
int GetLastReferenceNumber( CmpListStruct* Objet,
                            CmpListStruct* BaseListeCmp,
                            int            NbOfCmp )
{
    CmpListStruct* LastObjet  = BaseListeCmp + NbOfCmp;
    int            LastNumber = 0;
    const char*    RefText;

    RefText = Objet->m_TextRef;
    for( ; Objet < LastObjet; Objet++ )
    {
        /* Nouveau Identificateur */
        if( strnicmp( RefText, Objet->m_TextRef, 32 ) != 0 )
            break;
        if( LastNumber < Objet->m_NumRef )
            LastNumber = Objet->m_NumRef;
    }

    return LastNumber;
}


/*****************************************************************************
* Search in the sorted list of components, for a given componen,t an other component
* with the same reference and a given part unit.
* Mainly used to manage multiple parts per package components
* @param Objet = the given CmpListStruct* item to test
* @param Unit = the given unit number to search
* @param BaseListeCmp = list of items to examine
* @param NbOfCmp = size of list
* @return index in BaseListeCmp if found or -1 if not found
*****************************************************************************/
static int ExistUnit( CmpListStruct* Objet, int Unit,
                      CmpListStruct* BaseListeCmp, int NbOfCmp )
{
    CmpListStruct* EndList = BaseListeCmp + NbOfCmp;
    char*          RefText, * ValText;
    int            NumRef, ii;
    CmpListStruct* ItemToTest;

    RefText = Objet->m_TextRef;
    ValText = Objet->m_TextValue;
    NumRef  = Objet->m_NumRef;
    for( ItemToTest = BaseListeCmp, ii = 0;
         ItemToTest < EndList;
         ItemToTest++, ii++ )
    {
        if( Objet == ItemToTest )                                   // Do not compare with itself !
            continue;
        if( ItemToTest->m_IsNew )                                   // Not already with an updated reference
            continue;
        if( ItemToTest->m_NumRef != NumRef )                        // Not the same reference number (like 35 in R35)
            continue;
        if( strnicmp( RefText, ItemToTest->m_TextRef, 32 ) != 0 )   // Not the same reference prefix
            continue;
        if( ItemToTest->m_Unit == Unit )                            // A part with the same reference and the given unit is found
        {
            return ii;
        }
    }

    return -1;
}


/*****************************************************************************
*
* Function CheckAnnotate
* @return component count ( which are not annotated or have the same
*  reference (duplicates))
* @param oneSheetOnly : true = search is made only in the current sheet
*                       false = search in whole hierarchy (usual search).
*
*****************************************************************************/
int CheckAnnotate( WinEDA_SchematicFrame* frame, bool oneSheetOnly )
{
    int            ii, error, NbOfCmp;
    DrawSheetPath* sheet;
    CmpListStruct* ListeCmp = NULL;
    wxString       Buff;
    wxString       msg, cmpref;

    /* build the screen list */
    EDA_SheetList  SheetList( NULL );

    g_RootSheet->m_AssociatedScreen->SetModify();
    ii = 0;

    /* first pass : count composents */
    if( !oneSheetOnly )
    {
        NbOfCmp = 0;
        for( sheet = SheetList.GetFirst();
            sheet != NULL;
            sheet = SheetList.GetNext() )
            NbOfCmp += ListeComposants( NULL, sheet );
    }
    else
        NbOfCmp = ListeComposants( NULL, frame->GetSheet() );

    if( NbOfCmp == 0 )
    {
        return 0;
    }


    /* Second pass : create the list of components */
    ListeCmp = (CmpListStruct*) MyZMalloc( NbOfCmp * sizeof(CmpListStruct) );

    if( !oneSheetOnly )
    {
        ii = 0;
        for( sheet = SheetList.GetFirst();
            sheet != NULL;
            sheet = SheetList.GetNext() )
            ii += ListeComposants( ListeCmp + ii, sheet );
    }
    else
        ListeComposants( ListeCmp, frame->GetSheet() );

    qsort( ListeCmp, NbOfCmp, sizeof(CmpListStruct), AnnotateByValue );

    /* Break full components reference in name (prefix) and number: example:
     * IC1 become IC, and 1 */
    BreakReference( ListeCmp, NbOfCmp );

    /* count not yet annotated items */
    error = 0;
    for( ii = 0; ii < NbOfCmp - 1; ii++ )
    {
        msg.Empty();
        Buff.Empty();

        if( ListeCmp[ii].m_IsNew )
        {
            if( ListeCmp[ii].m_NumRef >= 0 )
                Buff << ListeCmp[ii].m_NumRef;
            else
                Buff = wxT( "?" );

            cmpref = CONV_FROM_UTF8( ListeCmp[ii].m_TextRef );
            msg.Printf( _( "item not annotated: %s%s" ),
                cmpref.GetData(), Buff.GetData() );

            if( (ListeCmp[ii].m_Unit > 0) && (ListeCmp[ii].m_Unit < 0x7FFFFFFF) )
            {
                Buff.Printf( _( "( unit %d)" ), ListeCmp[ii].m_Unit );
                msg << Buff;
            }
            DisplayError( NULL, msg );
            error++;
            break;
        }

        // Annotate error
        if( MAX( ListeCmp[ii].m_NbParts, 1 ) < ListeCmp[ii].m_Unit  )
        {
            if( ListeCmp[ii].m_NumRef >= 0 )
                Buff << ListeCmp[ii].m_NumRef;
            else
                Buff = wxT( "?" );

            cmpref = CONV_FROM_UTF8( ListeCmp[ii].m_TextRef );
            msg.Printf( _( "Error item %s%s" ), cmpref.GetData(),
                Buff.GetData() );

            Buff.Printf( _( " unit %d and no more than %d parts" ),
                ListeCmp[ii].m_Unit, ListeCmp[ii].m_NbParts );
            msg << Buff;
            DisplayError( frame, msg );
            error++;
            break;
        }
    }

    if( error )
        return error;

    // count the duplicated elements (if all are annotated)
    for( ii = 0; (ii < NbOfCmp - 1) && (error < 4); ii++ )
    {
        msg.Empty();
        Buff.Empty();

        if( (stricmp( ListeCmp[ii].m_TextRef,
                 ListeCmp[ii + 1].m_TextRef ) != 0)
           || ( ListeCmp[ii].m_NumRef != ListeCmp[ii + 1].m_NumRef ) )
            continue;

        /* Same reference found */

        /* If same unit, error ! */
        if( ListeCmp[ii].m_Unit == ListeCmp[ii + 1].m_Unit )
        {
            if( ListeCmp[ii].m_NumRef >= 0 )
                Buff << ListeCmp[ii].m_NumRef;
            else
                Buff = wxT( "?" );

            cmpref = CONV_FROM_UTF8( ListeCmp[ii].m_TextRef );
            msg.Printf( _( "Multiple item %s%s" ),
                cmpref.GetData(), Buff.GetData() );

            if( (ListeCmp[ii].m_Unit > 0) && (ListeCmp[ii].m_Unit < 0x7FFFFFFF) )
            {
                Buff.Printf( _( " (unit %d)" ), ListeCmp[ii].m_Unit );
                msg << Buff;
            }
            DisplayError( frame, msg );
            error++;
            continue;
        }

        /* Test error if units are different but number of parts per package
         * too hight (ex U3 ( 1 part) and we find U3B the is an error) */
        if( ListeCmp[ii].m_NbParts != ListeCmp[ii + 1].m_NbParts )
        {
            if( ListeCmp[ii].m_NumRef >= 0 )
                Buff << ListeCmp[ii].m_NumRef;
            else
                Buff = wxT( "?" );

            cmpref = CONV_FROM_UTF8( ListeCmp[ii].m_TextRef );
            msg.Printf( _( "Multiple item %s%s" ),
                cmpref.GetData(), Buff.GetData() );

            if( (ListeCmp[ii].m_Unit > 0) && (ListeCmp[ii].m_Unit < 0x7FFFFFFF) )
            {
                Buff.Printf( _( " (unit %d)" ), ListeCmp[ii].m_Unit );
                msg << Buff;
            }

            DisplayError( frame, msg );
            error++;
        }

        /* Error if values are different between units, for the same reference */
        if( stricmp( ListeCmp[ii].m_TextValue,
                ListeCmp[ii + 1].m_TextValue ) != 0 )
        {
            wxString nextcmpref, cmpvalue, nextcmpvalue;
            cmpref       = CONV_FROM_UTF8( ListeCmp[ii].m_TextRef );
            nextcmpref   = CONV_FROM_UTF8( ListeCmp[ii + 1].m_TextRef );
            cmpvalue     = CONV_FROM_UTF8( ListeCmp[ii].m_TextValue );
            nextcmpvalue = CONV_FROM_UTF8( ListeCmp[ii + 1].m_TextValue );
            msg.Printf( _( "Diff values for %s%d%c (%s) and %s%d%c (%s)" ),
                cmpref.GetData(),
                ListeCmp[ii].m_NumRef,
                ListeCmp[ii].m_Unit + 'A' - 1,
                cmpvalue.GetData(), nextcmpref.GetData(),
                ListeCmp[ii + 1].m_NumRef,
                ListeCmp[ii + 1].m_Unit + 'A' - 1,
                nextcmpvalue.GetData() );

            DisplayError( frame, msg );
            error++;
        }
    }

    MyFree( ListeCmp );
    return error;
}
