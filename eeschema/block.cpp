/****************************************************/
/*	BLOCK.CPP										*/
/* Gestion des Operations sur Blocks et Effacements */
/****************************************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"

#include "protos.h"

/* Variables Locales */

/* Fonctions exportees */

/* Fonctions Locales */
static EDA_BaseStruct*      CopyStruct( WinEDA_DrawPanel* panel, wxDC* DC, BASE_SCREEN* screen,
                                        EDA_BaseStruct* DrawStruct );
static void                 CollectStructsToDrag( SCH_SCREEN* screen );
static void                 AddPickedItem( SCH_SCREEN* screen, wxPoint position );
static LibEDA_BaseStruct*   GetNextPinPosition( EDA_SchComponentStruct* DrawLibItem,
                                                wxPoint & position );
static void                 DrawMovingBlockOutlines( WinEDA_DrawPanel* panel, wxDC* DC, bool erase );
static EDA_BaseStruct*      SaveStructListForPaste( EDA_BaseStruct* DrawStruct );
static bool                 MirrorStruct( WinEDA_DrawPanel* panel, wxDC* DC,
                                          EDA_BaseStruct* DrawStruct, wxPoint& Center );
static void                 MirrorOneStruct( EDA_BaseStruct* DrawStruct, wxPoint& Center );

/*************************************************************************/
int WinEDA_SchematicFrame::ReturnBlockCommand( int key )
/*************************************************************************/

/* Return the block command (BLOCK_MOVE, BLOCK_COPY...) corresponding to
 *  the key (ALT, SHIFT ALT ..)
 */
{
    int cmd;

    switch( key )
    {
    default:
        cmd = key & 0xFF;
        break;

    case 0:
        cmd = BLOCK_MOVE;
        break;

    case GR_KB_ALT:
    case GR_KB_SHIFT:
        cmd = BLOCK_COPY;
        break;

    case GR_KB_CTRL:
        cmd = BLOCK_DRAG;
        break;

    case GR_KB_SHIFTCTRL:
        cmd = BLOCK_DELETE;
        break;

    case MOUSE_MIDDLE:
        cmd = BLOCK_ZOOM;
        break;
    }

    return cmd;
}


/*************************************************/
void WinEDA_SchematicFrame::InitBlockPasteInfos()
/*************************************************/

/* Init the parameters used by the block paste command
 */
{
    DrawBlockStruct* block = & GetScreen()->BlockLocate;

    block->m_BlockDrawStruct = g_BlockSaveDataList;
    DrawPanel->ManageCurseur = DrawMovingBlockOutlines;
}


/******************************************************/
void WinEDA_SchematicFrame::HandleBlockPlace( wxDC* DC )
/******************************************************/

/* Routine to handle the BLOCK PLACE commande
 *  Last routine for block operation for:
 *  - block move & drag
 *  - block copie & paste
 */
{
    bool             err   = FALSE;
    DrawBlockStruct* block = & GetScreen()->BlockLocate;

    EDA_BaseStruct*  NewStruct = NULL;

    if( DrawPanel->ManageCurseur == NULL )
    {
        err = TRUE;
        DisplayError( this, wxT( "HandleBlockPLace() : ManageCurseur = NULL" ) );
    }

    if( block->m_BlockDrawStruct == NULL )
    {
        wxString msg;
        err = TRUE;
        msg.Printf( wxT( "HandleBlockPLace() : m_BlockDrawStruct = NULL (cmd %d, state %d)" ),
                    block->m_Command, block->m_State );
        DisplayError( this, msg );
    }

    block->m_State = STATE_BLOCK_STOP;

    switch( block->m_Command )
    {
    case  BLOCK_IDLE:
        err = TRUE;
        break;

    case BLOCK_DRAG:        /* Drag */
    case BLOCK_MOVE:        /* Move */
        if( DrawPanel->ManageCurseur )
            DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );

        SaveCopyInUndoList( block->m_BlockDrawStruct, IS_CHANGED );

        MoveStruct( DrawPanel, DC, block->m_BlockDrawStruct );
        block->m_BlockDrawStruct = NULL;
        break;

    case BLOCK_COPY:                /* Copy */
    case BLOCK_PRESELECT_MOVE:      /* Move with preselection list*/
        if( DrawPanel->ManageCurseur )
            DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );

        NewStruct = CopyStruct( DrawPanel, DC, GetScreen(), block->m_BlockDrawStruct );

        SaveCopyInUndoList( NewStruct,
                            (block->m_Command == BLOCK_PRESELECT_MOVE) ? IS_CHANGED : IS_NEW );

        block->m_BlockDrawStruct = NULL;
        break;

    case BLOCK_PASTE:     /* Paste (recopie du dernier bloc sauve */
        if( DrawPanel->ManageCurseur )
            DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
        PasteStruct( DC );
        block->m_BlockDrawStruct = NULL;
        break;

    case BLOCK_ZOOM:        // Handled by HandleBlockEnd()
    case BLOCK_DELETE:
    case BLOCK_SAVE:
    case BLOCK_ROTATE:
    case BLOCK_MIRROR_X:
    case BLOCK_MIRROR_Y:
    case BLOCK_INVERT:
    case BLOCK_ABORT:
    case BLOCK_SELECT_ITEMS_ONLY:
        break;
    }

    GetScreen()->SetModify();

    /* clear struct.m_Flags  */
    EDA_BaseStruct* Struct;
    for( Struct = GetScreen()->EEDrawList; Struct != NULL; Struct = Struct->Pnext )
        Struct->m_Flags = 0;

    DrawPanel->ManageCurseur = NULL;
    DrawPanel->ForceCloseManageCurseur = NULL;
    block->m_Flags   = 0;
    block->m_State   = STATE_NO_BLOCK;
    block->m_Command = BLOCK_IDLE;
    GetScreen()->SetCurItem( NULL );

    TestDanglingEnds( GetScreen()->EEDrawList, DC );

    if( block->m_BlockDrawStruct )
    {
        DisplayError( this, wxT( "HandleBlockPLace() error: DrawStruct != Null" ) );
        block->m_BlockDrawStruct = NULL;
    }

    SetToolID( m_ID_current_state, DrawPanel->m_PanelDefaultCursor, wxEmptyString );
}


/****************************************************/
int WinEDA_SchematicFrame::HandleBlockEnd( wxDC* DC )
/****************************************************/

/* Routine de gestion de la commande BLOCK END
 *  retourne :
 *  0 si aucun composant selectionne
 *  1 sinon
 *  -1 si commande termin�e et composants trouv�s (block delete, block save)
 */
{
    int              ii = 0;
    bool             zoom_command = FALSE;
    DrawBlockStruct* block = & GetScreen()->BlockLocate;

    if( block->m_BlockDrawStruct )
    {
        BlockState   state   = block->m_State;
        CmdBlockType command = block->m_Command;
        if( DrawPanel->ForceCloseManageCurseur )
            DrawPanel->ForceCloseManageCurseur( DrawPanel, DC );
        block->m_State   = state;
        block->m_Command = command;
        DrawPanel->ManageCurseur = DrawAndSizingBlockOutlines;
        DrawPanel->ForceCloseManageCurseur = AbortBlockCurrentCommand;
        GetScreen()->m_Curseur.x = block->GetRight();
        GetScreen()->m_Curseur.y = block->GetBottom();
        if( block->m_Command != BLOCK_ABORT )
            DrawPanel->MouseToCursorSchema();
    }

    if( DrawPanel->ManageCurseur != NULL )
        switch( block->m_Command )
        {
        case  BLOCK_IDLE:
            DisplayError( this, wxT( "Error in HandleBlockPLace()" ) );
            break;

        case BLOCK_DRAG: /* Drag */
            BreakSegmentOnJunction( GetScreen() );

        case BLOCK_MOVE:    /* Move */
        case BLOCK_COPY:    /* Copy */
            block->m_BlockDrawStruct =
                PickStruct( GetScreen()->BlockLocate, GetScreen()->EEDrawList, SEARCHALL );

        case BLOCK_PRESELECT_MOVE: /* Move with preselection list*/
            if( block->m_BlockDrawStruct != NULL )
            {
                ii = 1;
                CollectStructsToDrag( GetScreen() );
                DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
                DrawPanel->ManageCurseur = DrawMovingBlockOutlines;
                DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
                block->m_State = STATE_BLOCK_MOVE;
            }
            else
            {
                DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
                DrawPanel->ManageCurseur = NULL;
                DrawPanel->ForceCloseManageCurseur = NULL;
            }
            break;

        case BLOCK_DELETE: /* Delete */
            block->m_BlockDrawStruct =
                PickStruct( GetScreen()->BlockLocate,
                            GetScreen()->EEDrawList, SEARCHALL );
            DrawAndSizingBlockOutlines( DrawPanel, DC, FALSE );
            if( block->m_BlockDrawStruct != NULL )
            {
                ii = -1;
                DeleteStruct( DrawPanel, DC, block->m_BlockDrawStruct );
                GetScreen()->SetModify();
            }
            block->m_BlockDrawStruct = NULL;
            TestDanglingEnds( GetScreen()->EEDrawList, DC );
            break;

        case BLOCK_SAVE: /* Save */
            block->m_BlockDrawStruct =
                PickStruct( GetScreen()->BlockLocate,
                            GetScreen()->EEDrawList, SEARCHALL );
            DrawAndSizingBlockOutlines( DrawPanel, DC, FALSE );
            if( block->m_BlockDrawStruct != NULL )
            {
                wxPoint         oldpos = GetScreen()->m_Curseur;
                GetScreen()->m_Curseur = wxPoint( 0, 0 );
                EDA_BaseStruct* DrawStructCopy =
                    SaveStructListForPaste( block->m_BlockDrawStruct );
                PlaceStruct( GetScreen(), DrawStructCopy );
                GetScreen()->m_Curseur = oldpos;
                ii = -1;
            }
            block->m_BlockDrawStruct = NULL;
            break;

        case BLOCK_PASTE:
            block->m_State = STATE_BLOCK_MOVE;
            break;

        case BLOCK_INVERT: /* pcbnew only! */
            break;

        case BLOCK_ROTATE:
        case BLOCK_MIRROR_X:
        case BLOCK_MIRROR_Y:
            break;

        case BLOCK_ZOOM: /* Window Zoom */
            zoom_command = TRUE;
            break;

        case BLOCK_SELECT_ITEMS_ONLY:   /* Not used */
        case BLOCK_ABORT:               /* not executed here */
            break;
        }

    if( block->m_Command  == BLOCK_ABORT )
    {   /* clear struct.m_Flags  */
        EDA_BaseStruct* Struct;
        for( Struct = GetScreen()->EEDrawList; Struct != NULL; Struct = Struct->Pnext )
            Struct->m_Flags = 0;
    }

    if( ii <= 0 )
    {
        block->m_Flags   = 0;
        block->m_State   = STATE_NO_BLOCK;
        block->m_Command = BLOCK_IDLE;
        DrawPanel->ManageCurseur = NULL;
        DrawPanel->ForceCloseManageCurseur = NULL;
        GetScreen()->SetCurItem( NULL );
        SetToolID( m_ID_current_state, DrawPanel->m_PanelDefaultCursor, wxEmptyString );
    }

    if( zoom_command )
        Window_Zoom( GetScreen()->BlockLocate );

    return ii;
}


/***********************************************************************/
void WinEDA_SchematicFrame::HandleBlockEndByPopUp( int Command, wxDC* DC )
/***********************************************************************/

/* Routine de gestion de la commande BLOCK END by PopUp
 *  Appelee apres HandleBlockEnd.
 *  A partir de la commande bloc move, peut executer une commande autre que bloc move.
 */
{
    int ii = 0;
    DrawBlockStruct* block = & GetScreen()->BlockLocate;

    if( block->m_Command != BLOCK_MOVE )
        return;
    if( Command == BLOCK_MOVE )
        return;

    block->m_Command = (CmdBlockType) Command;
    block->SetMessageBlock( this );

    switch( block->m_Command )
    {
    case BLOCK_COPY:     /* move to copy */
        block->m_State = STATE_BLOCK_MOVE;
        ii = 1;
        break;

    case BLOCK_DRAG:     /* move to Drag */

        /* Effacement de la liste des structures de pointage,
         *  qui est devenue erronnee */
        if( DrawPanel->ManageCurseur )
            DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
        if( block->m_BlockDrawStruct )
        {
            if( block->m_BlockDrawStruct->Type() == DRAW_PICK_ITEM_STRUCT_TYPE )
            {       /* Delete the picked wrapper if this is a picked list. */
                DrawPickedStruct* PickedList;
                PickedList = (DrawPickedStruct*) block->m_BlockDrawStruct;
                PickedList->DeleteWrapperList();
            }
            block->m_BlockDrawStruct = NULL;
        }
        BreakSegmentOnJunction( GetScreen() );
        block->m_BlockDrawStruct =
            PickStruct( GetScreen()->BlockLocate,
                        GetScreen()->EEDrawList, SEARCHALL );
        if( block->m_BlockDrawStruct != NULL )
        {
            ii = 1;
            CollectStructsToDrag( GetScreen() );
            if( DrawPanel->ManageCurseur )
                DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
            block->m_State = STATE_BLOCK_MOVE;
        }
        break;

    case BLOCK_DELETE:     /* move to Delete */
        if( DrawPanel->ManageCurseur )
            DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
        if( block->m_BlockDrawStruct != NULL )
        {
            ii = -1;
            DeleteStruct( DrawPanel, DC, block->m_BlockDrawStruct );
            GetScreen()->SetModify();
        }
        TestDanglingEnds( GetScreen()->EEDrawList, DC );
        break;

    case BLOCK_SAVE:     /* Save */
        if( DrawPanel->ManageCurseur )
            DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
        if( block->m_BlockDrawStruct != NULL )
        {
            wxPoint         oldpos = GetScreen()->m_Curseur;
            GetScreen()->m_Curseur = wxPoint( 0, 0 );
            EDA_BaseStruct* DrawStructCopy =
                SaveStructListForPaste( block->m_BlockDrawStruct );
            PlaceStruct( GetScreen(), DrawStructCopy );
            GetScreen()->m_Curseur = oldpos;
            ii = -1;
        }
        break;

    case BLOCK_ZOOM:     /* Window Zoom */
        DrawPanel->ForceCloseManageCurseur( DrawPanel, DC );
        DrawPanel->SetCursor( DrawPanel->m_PanelCursor = DrawPanel->m_PanelDefaultCursor );
        Window_Zoom( GetScreen()->BlockLocate );
        break;


    case BLOCK_ROTATE:
        break;

    case BLOCK_MIRROR_X:
    case BLOCK_MIRROR_Y:
        if( DrawPanel->ManageCurseur )
            DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
        if( block->m_BlockDrawStruct != NULL )
        {
            SaveCopyInUndoList( block->m_BlockDrawStruct, IS_CHANGED );

            ii = -1;
            /* Compute the mirror centre and put it on grid */
            wxPoint Center = block->Centre();
            PutOnGrid( &Center );
            MirrorStruct( DrawPanel, DC, block->m_BlockDrawStruct, Center );
            GetScreen()->SetModify();
        }
        TestDanglingEnds( GetScreen()->EEDrawList, DC );
        break;

    default:
        break;
    }

    if( ii <= 0 )
    {
        block->m_BlockDrawStruct = NULL;
        block->m_Flags   = 0;
        block->m_State   = STATE_NO_BLOCK;
        block->m_Command = BLOCK_IDLE;
        DrawPanel->ManageCurseur = NULL;
        DrawPanel->ForceCloseManageCurseur = NULL;
        GetScreen()->SetCurItem( NULL );
        SetToolID( m_ID_current_state, DrawPanel->m_PanelDefaultCursor, wxEmptyString );
    }
}


/************************************************************************/
static void DrawMovingBlockOutlines( WinEDA_DrawPanel* panel, wxDC* DC,
                                     bool erase )
/************************************************************************/

/* Retrace le contour du block de recherche de structures
 *  L'ensemble du block suit le curseur
 */
{
    DrawBlockStruct*  PtBlock;
    DrawPickedStruct* PickedList;
    BASE_SCREEN*      screen = panel->m_Parent->GetScreen();

    PtBlock = &panel->GetScreen()->BlockLocate;
    GRSetDrawMode( DC, g_XorMode );

    /* Effacement ancien cadre */
    if( erase && PtBlock->m_BlockDrawStruct )
    {
        PtBlock->Offset( PtBlock->m_MoveVector );
        PtBlock->Draw( panel, DC );
        PtBlock->Offset( -PtBlock->m_MoveVector.x, -PtBlock->m_MoveVector.y );

        /* Effacement ancien affichage */
        if( PtBlock->m_BlockDrawStruct->Type() == DRAW_PICK_ITEM_STRUCT_TYPE )
        {
            PickedList = (DrawPickedStruct*) PtBlock->m_BlockDrawStruct;
            while( PickedList )
            {
                DrawStructsInGhost( panel,
                                    DC,
                                    PickedList->m_PickedStruct,
                                    PtBlock->m_MoveVector.x,
                                    PtBlock->m_MoveVector.y );
                PickedList = (DrawPickedStruct*) PickedList->Pnext;
            }
        }
        else
            DrawStructsInGhost( panel,
                                DC,
                                PtBlock->m_BlockDrawStruct,
                                PtBlock->m_MoveVector.x,
                                PtBlock->m_MoveVector.y );
    }

    /* Redessin nouvel affichage */

    PtBlock->m_MoveVector.x = screen->m_Curseur.x - PtBlock->m_BlockLastCursorPosition.x;
    PtBlock->m_MoveVector.y = screen->m_Curseur.y - PtBlock->m_BlockLastCursorPosition.y;

    GRSetDrawMode( DC, g_XorMode );
    PtBlock->Offset( PtBlock->m_MoveVector );
    PtBlock->Draw( panel, DC );
    PtBlock->Offset( -PtBlock->m_MoveVector.x, -PtBlock->m_MoveVector.y );

    if( PtBlock->m_BlockDrawStruct )
    {
        if( PtBlock->m_BlockDrawStruct->Type() == DRAW_PICK_ITEM_STRUCT_TYPE )
        {
            PickedList = (DrawPickedStruct*) PtBlock->m_BlockDrawStruct;
            while( PickedList )
            {
                DrawStructsInGhost( panel,
                                    DC,
                                    PickedList->m_PickedStruct,
                                    PtBlock->m_MoveVector.x,
                                    PtBlock->m_MoveVector.y );
                PickedList = (DrawPickedStruct*) PickedList->Pnext;
            }
        }
        else
            DrawStructsInGhost( panel,
                                DC,
                                PtBlock->m_BlockDrawStruct,
                                PtBlock->m_MoveVector.x,
                                PtBlock->m_MoveVector.y );
    }
}


/*****************************************************************************
* Routine to move an object(s) to a new position.							 *
* If DrawStruct is of type DrawPickedStruct, a list of objects picked is	 *
* assumed, otherwise exactly one structure is assumed been picked.			 *
*****************************************************************************/
bool MoveStruct( WinEDA_DrawPanel* panel, wxDC* DC, EDA_BaseStruct* DrawStruct )
{
    DrawPickedStruct* PickedList = NULL;

    if( !DrawStruct )
        return FALSE;

    if( DrawStruct->Type() == DRAW_PICK_ITEM_STRUCT_TYPE )
    {
        if( DC )
            RedrawStructList( panel, DC, DrawStruct, g_XorMode );
        PlaceStruct( panel->GetScreen(), DrawStruct );    /* Place it in its new position. */
        if( DC )
            RedrawStructList( panel, DC, DrawStruct, GR_DEFAULT_DRAWMODE );

        /* Free the wrapper DrawPickedStruct chain: */
        PickedList = (DrawPickedStruct*) DrawStruct;
        PickedList->DeleteWrapperList();
    }
    else
    {
        if( DC )
            RedrawOneStruct( panel, DC, DrawStruct, g_XorMode );
        PlaceStruct( panel->GetScreen(), DrawStruct );        /* Place it in its new position. */
        if( DC )
            RedrawOneStruct( panel, DC, DrawStruct, GR_DEFAULT_DRAWMODE );
    }
    return TRUE;
}


static void MirrorYPoint( wxPoint& point, wxPoint& Center )
{
    point.x -= Center.x;
    point.x  = -point.x;
    point.x += Center.x;
}


/**************************************************************/
void MirrorOneStruct( EDA_BaseStruct* DrawStruct, wxPoint& Center )
/**************************************************************/

/* Given a structure rotate it to 90 degrees refer to the Center point.
 */
{
    int dx, ii, * Points;
    DrawPolylineStruct*     DrawPoly;
    DrawJunctionStruct*     DrawConnect;
    EDA_DrawLineStruct*     DrawSegment;
    DrawBusEntryStruct*     DrawRaccord;
    EDA_SchComponentStruct* DrawLibItem;
    DrawSheetStruct*        DrawSheet;
    DrawSheetLabelStruct*   DrawSheetLabel;
    DrawMarkerStruct*       DrawMarker;
    DrawNoConnectStruct*    DrawNoConnect;
    DrawTextStruct*         DrawText;
    wxPoint px;

    if( !DrawStruct )
        return;

    switch( DrawStruct->Type() )
    {
    case TYPE_NOT_INIT:
        break;

    case DRAW_POLYLINE_STRUCT_TYPE:
        DrawPoly = (DrawPolylineStruct*) DrawStruct;
        Points   = DrawPoly->m_Points;
        for( ii = 0; ii < DrawPoly->m_NumOfPoints; ii++ )
        {
            wxPoint point;
            point.x = Points[ii * 2]; point.y = Points[ii * 2 + 1];
            MirrorYPoint( point, Center );
            Points[ii * 2] = point.x; Points[ii * 2 + 1] = point.y;
        }

        break;

    case DRAW_SEGMENT_STRUCT_TYPE:
        DrawSegment = (EDA_DrawLineStruct*) DrawStruct;
        if( (DrawSegment->m_Flags & STARTPOINT) == 0 )
        {
            MirrorYPoint( DrawSegment->m_Start, Center );
        }
        if( (DrawSegment->m_Flags & ENDPOINT) == 0 )
        {
            MirrorYPoint( DrawSegment->m_End, Center );
        }
        break;

    case DRAW_BUSENTRY_STRUCT_TYPE:
        DrawRaccord = (DrawBusEntryStruct*) DrawStruct;
        MirrorYPoint( DrawRaccord->m_Pos, Center );
        break;

    case DRAW_JUNCTION_STRUCT_TYPE:
        DrawConnect = (DrawJunctionStruct*) DrawStruct;
        MirrorYPoint( DrawConnect->m_Pos, Center );
        break;

    case DRAW_MARKER_STRUCT_TYPE:
        DrawMarker = (DrawMarkerStruct*) DrawStruct;
        MirrorYPoint( DrawMarker->m_Pos, Center );
        break;

    case DRAW_NOCONNECT_STRUCT_TYPE:
        DrawNoConnect = (DrawNoConnectStruct*) DrawStruct;
        MirrorYPoint( DrawNoConnect->m_Pos, Center );
        break;

    case DRAW_TEXT_STRUCT_TYPE:
    case DRAW_LABEL_STRUCT_TYPE:

        // Text is not really mirrored; it is moved to a suitable position
        // which is the closest position for a true mirrored text
        // The center position is mirrored and the text is moved for half horizontal len
        DrawText = (DrawTextStruct*) DrawStruct;
        px = DrawText->m_Pos;
        if( DrawText->m_Orient == 0 )           /* horizontal text */
            dx = DrawText->Len_Size() / 2;
        else if( DrawText->m_Orient == 2 )      /* invert horizontal text*/
            dx = -DrawText->Len_Size() / 2;
        else
            dx = 0;
        px.x += dx;
        MirrorYPoint( px, Center );
        px.x -= dx;

        g_EDA_Appl->m_SchematicFrame->PutOnGrid( &px );
        DrawText->m_Pos.x = px.x;
        break;

    case DRAW_GLOBAL_LABEL_STRUCT_TYPE:

        // Text is not really mirrored: Orientation is changed
        DrawText = (DrawGlobalLabelStruct*) DrawStruct;
        if( DrawText->m_Orient == 0 )           /* horizontal text */
            DrawText->m_Orient = 2;
        else if( DrawText->m_Orient == 2 )      /* invert horizontal text*/
            DrawText->m_Orient = 0;

        px = DrawText->m_Pos;
        MirrorYPoint( px, Center );
        g_EDA_Appl->m_SchematicFrame->PutOnGrid( &px );
        DrawText->m_Pos.x = px.x;
        break;

    case DRAW_LIB_ITEM_STRUCT_TYPE:
        DrawLibItem = (EDA_SchComponentStruct*) DrawStruct;
        dx = DrawLibItem->m_Pos.x;
        g_EDA_Appl->m_SchematicFrame->CmpRotationMiroir( DrawLibItem,
                                                     NULL, CMP_MIROIR_Y );
        MirrorYPoint( DrawLibItem->m_Pos, Center );
        dx -= DrawLibItem->m_Pos.x;
        for( ii = 0; ii < NUMBER_OF_FIELDS; ii++ )
        {       /* move the fields to the new position because the component itself has moved */
            DrawLibItem->m_Field[ii].m_Pos.x -= dx;
        }

        break;

    case DRAW_SHEET_STRUCT_TYPE:
        DrawSheet = (DrawSheetStruct*) DrawStruct;
        MirrorYPoint( DrawSheet->m_Pos, Center );
        DrawSheet->m_Pos.x -= DrawSheet->m_Size.x;

        DrawSheetLabel = DrawSheet->m_Label;
        while( DrawSheetLabel != NULL )
        {
            MirrorYPoint( DrawSheetLabel->m_Pos, Center );
            DrawSheetLabel->m_Edge = DrawSheetLabel->m_Edge ? 0 : 1;
            DrawSheetLabel = (DrawSheetLabelStruct*) DrawSheetLabel->Pnext;
        }

        break;

    case DRAW_SHEETLABEL_STRUCT_TYPE:
        DrawSheetLabel = (DrawSheetLabelStruct*) DrawStruct;
        MirrorYPoint( DrawSheetLabel->m_Pos, Center );
        break;

    case DRAW_PICK_ITEM_STRUCT_TYPE:
        break;

    default:
        break;
    }
}


/*****************************************************************************
* Routine to Mirror an object(s).							 *
* If DrawStruct is of type DrawPickedStruct, a list of objects picked is	 *
* assumed, otherwise exactly one structure is assumed been picked.			 *
*****************************************************************************/
bool MirrorStruct( WinEDA_DrawPanel* panel, wxDC* DC, EDA_BaseStruct* DrawStruct, wxPoint& Center )
{
    DrawPickedStruct* PickedList = NULL;
    DrawPickedStruct* DrawStructs;

    if( !DrawStruct )
        return FALSE;

    if( DrawStruct->Type() == DRAW_PICK_ITEM_STRUCT_TYPE )
    {
        if( DC )
            RedrawStructList( panel, DC, DrawStruct, g_XorMode );
        DrawStructs = (DrawPickedStruct*) DrawStruct;
        while( DrawStructs )
        {
            MirrorOneStruct( DrawStructs->m_PickedStruct, Center );
            DrawStructs->m_PickedStruct->m_Flags = 0;
            DrawStructs = (DrawPickedStruct*) DrawStructs->Pnext;
        }

        if( DC )
            RedrawStructList( panel, DC, DrawStruct, GR_DEFAULT_DRAWMODE );

        /* Free the wrapper DrawPickedStruct chain: */
        PickedList = (DrawPickedStruct*) DrawStruct;
        PickedList->DeleteWrapperList();
    }
    else
    {
        if( DC )
            RedrawOneStruct( panel, DC, DrawStruct, g_XorMode );
        MirrorOneStruct( DrawStruct, Center );        /* Place it in its new position. */
        if( DC )
            RedrawOneStruct( panel, DC, DrawStruct, GR_DEFAULT_DRAWMODE );
        DrawStruct->m_Flags = 0;
    }


    return TRUE;
}


/*****************************************************************************/
static EDA_BaseStruct* CopyStruct( WinEDA_DrawPanel* panel, wxDC* DC, BASE_SCREEN* screen,
                                   EDA_BaseStruct* DrawStruct )
/*****************************************************************************/

/* Routine to copy a new entity of an object and reposition it.
 *  If DrawStruct is of type DrawPickedStruct, a list of objects picked is
 *  assumed, otherwise exactly one structure is assumed been picked.
 *  Return the new created struct
 */
{
    EDA_BaseStruct*   NewDrawStruct;
    DrawPickedStruct* PickedList = NULL;

    if( !DrawStruct )
        return FALSE;

    NewDrawStruct = DuplicateStruct( DrawStruct );
    if( NewDrawStruct == NULL )
        return NULL;

    PlaceStruct( screen, NewDrawStruct );
    /* Draw the new structure and chain it in: */
    if( NewDrawStruct->Type() == DRAW_PICK_ITEM_STRUCT_TYPE )
    {
        PickedList = (DrawPickedStruct*) NewDrawStruct;
        while( PickedList )  // Clear annotation for new components
        {
            EDA_BaseStruct* Struct = PickedList->m_PickedStruct;

            switch( Struct->Type() )
            {
            case DRAW_LIB_ITEM_STRUCT_TYPE:
            {
                ( (EDA_SchComponentStruct*) Struct )->m_TimeStamp = GetTimeStamp();
                ( (EDA_SchComponentStruct*) Struct )->ClearAnnotation();
            }
                break;

            case DRAW_SHEET_STRUCT_TYPE:
            {
                DrawSheetStruct* sheet = (DrawSheetStruct*) Struct;
                sheet->m_TimeStamp = GetTimeStamp();
                sheet->m_UndoList  = NULL;
                sheet->m_RedoList  = NULL;
                sheet->EEDrawList  = NULL;
                sheet->m_Son = NULL;
                sheet->m_SheetName.Printf( wxT( "%8.8lX" ), sheet->m_TimeStamp );
                sheet->m_FileName = sheet->m_SheetName + wxT( ".sch" );
                break;
            }
            
            default:
                ;
            }

            SetStructFather( Struct, screen );
            PickedList = (DrawPickedStruct*) PickedList->Pnext;
        }

        RedrawStructList( panel, DC, NewDrawStruct, GR_DEFAULT_DRAWMODE );
        /* Chain the new items */
        PickedList = (DrawPickedStruct*) NewDrawStruct;
        while( PickedList )
        {
            PickedList->m_PickedStruct->Pnext = screen->EEDrawList;
            screen->EEDrawList = PickedList->m_PickedStruct;
            PickedList = PickedList->Next();
        }
    }
    else
    {
        switch( NewDrawStruct->Type() )
        {
        case DRAW_POLYLINE_STRUCT_TYPE:
        case DRAW_JUNCTION_STRUCT_TYPE:
        case DRAW_SEGMENT_STRUCT_TYPE:
        case DRAW_BUSENTRY_STRUCT_TYPE:
        case DRAW_TEXT_STRUCT_TYPE:
        case DRAW_LABEL_STRUCT_TYPE:
        case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
        case DRAW_SHEETLABEL_STRUCT_TYPE:
        case DRAW_PICK_ITEM_STRUCT_TYPE:
        case DRAW_MARKER_STRUCT_TYPE:
        case DRAW_NOCONNECT_STRUCT_TYPE:
        default:
            break;

        case DRAW_SHEET_STRUCT_TYPE:
        {
            DrawSheetStruct* sheet = (DrawSheetStruct*) NewDrawStruct;
            sheet->m_TimeStamp = GetTimeStamp();
            sheet->m_UndoList  = NULL;
            sheet->m_RedoList  = NULL;
            sheet->EEDrawList  = NULL;
            sheet->m_Son = NULL;
            sheet->m_SheetName.Printf( wxT( "%8.8lX" ), sheet->m_TimeStamp );
            sheet->m_FileName = sheet->m_SheetName + wxT( ".sch" );
            break;
        }

        case DRAW_LIB_ITEM_STRUCT_TYPE:
            ( (EDA_SchComponentStruct*) NewDrawStruct )->m_TimeStamp = GetTimeStamp();
            ( (EDA_SchComponentStruct*) NewDrawStruct )->ClearAnnotation();
            break;
        }

        RedrawOneStruct( panel, DC, NewDrawStruct, GR_DEFAULT_DRAWMODE );

        SetStructFather( NewDrawStruct, screen );
        NewDrawStruct->Pnext = screen->EEDrawList;
        screen->EEDrawList   = NewDrawStruct;
    }

    /* Free the original DrawPickedStruct chain: */
    if( DrawStruct->Type() == DRAW_PICK_ITEM_STRUCT_TYPE )
    {
        PickedList = (DrawPickedStruct*) DrawStruct;
        PickedList->DeleteWrapperList();
    }

    return NewDrawStruct;
}


/*********************************************************************************/
void DeleteStruct( WinEDA_DrawPanel* panel, wxDC* DC, EDA_BaseStruct* DrawStruct )
/*********************************************************************************/

/* Routine to delete an object from global drawing object list.
 *  Object is put in Undo list
 */
{
    SCH_SCREEN*            screen     = (SCH_SCREEN*) panel->GetScreen();
    DrawPickedStruct*      PickedList = NULL;
    WinEDA_SchematicFrame* frame = (WinEDA_SchematicFrame*) panel->m_Parent;

    if( !DrawStruct )
        return;


    if( DrawStruct->Type() == DRAW_SHEETLABEL_STRUCT_TYPE )
    {   /* Cette stucture est rattachee a une feuille, et n'est pas
         *  accessible par la liste globale directement */
        frame->SaveCopyInUndoList( ( (DrawSheetLabelStruct*) DrawStruct )->m_Parent, IS_CHANGED );
        frame->DeleteSheetLabel( DC, (DrawSheetLabelStruct*) DrawStruct );
        return;
    }

    if( DrawStruct->Type() == DRAW_PICK_ITEM_STRUCT_TYPE )
    {
        /* Unlink all picked structs from current EEDrawList */
        PickedList = (DrawPickedStruct*) DrawStruct;
        while( PickedList )
        {
            screen->RemoveFromDrawList( PickedList->m_PickedStruct );
            PickedList->m_PickedStruct->Pnext     =
                PickedList->m_PickedStruct->Pback = NULL;
            PickedList->m_PickedStruct->m_Flags   = IS_DELETED;
            PickedList = PickedList->Next();
        }

        RedrawStructList( panel, DC, DrawStruct, g_XorMode );

        /* Removed items are put to the Undo list */
        frame->SaveCopyInUndoList( DrawStruct, IS_DELETED );
    }
    else    /* structure classique */
    {
        screen->RemoveFromDrawList( DrawStruct );

        RedrawOneStruct( panel, DC, DrawStruct, g_XorMode );
        /* Unlink the structure */
        DrawStruct->Pnext = DrawStruct->Pback = NULL;   // Only one struct -> no link
        frame->SaveCopyInUndoList( DrawStruct, IS_DELETED );
    }
}


/*****************************************************************/
EDA_BaseStruct* SaveStructListForPaste( EDA_BaseStruct* DrawStruct )
/*****************************************************************/

/* Routine to Save an object from global drawing object list.
 *  This routine is the same as delete but:
 *  - the original list is NOT removed.
 *  - List is saved in g_BlockSaveDataList
 */
{
    DrawPickedStruct* PickedList;
    EDA_BaseStruct*   DrawStructCopy;

    if( !DrawStruct )
        return NULL;

    /* Make a copy of the original picked item. */
    DrawStructCopy = DuplicateStruct( DrawStruct );

    if( DrawStruct->Type() == DRAW_PICK_ITEM_STRUCT_TYPE )
    {
        /* Delete the picked wrapper if this is a picked list. */
        PickedList = (DrawPickedStruct*) DrawStruct;
        PickedList->DeleteWrapperList();
    }

    /* And delete old list and save the new list: */
    if( g_BlockSaveDataList ) /* Delete last deleted item or item list */
    {
        EDA_BaseStruct* item = g_BlockSaveDataList, * next_item;
        while( item )
        {
            next_item = item->Pnext;
            delete item;
            item = next_item;
        }
    }

    g_BlockSaveDataList      = DrawStructCopy;
    DrawStructCopy->m_Parent = NULL;

    return DrawStructCopy;
}


/*****************************************************************************
* Routine to paste a structure from the g_BlockSaveDataList stack.						 *
*	This routine is the same as undelete but original list is NOT removed.	 *
*****************************************************************************/
void WinEDA_SchematicFrame::PasteStruct( wxDC* DC )
{
    EDA_BaseStruct*   DrawStruct;
    DrawPickedStruct* PickedList = NULL;

    if( g_BlockSaveDataList == NULL )
    {
        DisplayError( this, wxT( "No struct to paste" ) );
        return;
    }

    DrawStruct = DuplicateStruct( g_BlockSaveDataList );

    PlaceStruct( GetScreen(), DrawStruct );

    RedrawStructList( DrawPanel, DC, DrawStruct, GR_DEFAULT_DRAWMODE );

    // Clear annotation and init new time stamp for the new components:
    if( DrawStruct->Type() == DRAW_PICK_ITEM_STRUCT_TYPE )
    {
        for( PickedList = (DrawPickedStruct*) DrawStruct; PickedList != NULL; ) // Clear annotation for new components
        {
            EDA_BaseStruct* Struct = PickedList->m_PickedStruct;
            if( Struct->Type() == DRAW_LIB_ITEM_STRUCT_TYPE )
            {
                ( (EDA_SchComponentStruct*) Struct )->m_TimeStamp = GetTimeStamp();
                ( (EDA_SchComponentStruct*) Struct )->ClearAnnotation();
                SetStructFather( Struct, GetScreen() );
            }
            PickedList = (DrawPickedStruct*) PickedList->Pnext;
        }

        RedrawStructList( DrawPanel, DC, DrawStruct, GR_DEFAULT_DRAWMODE );
        for( PickedList = (DrawPickedStruct*) DrawStruct; PickedList != NULL; )
        {
            EDA_BaseStruct* Struct = PickedList->m_PickedStruct;
            Struct->Pnext = GetScreen()->EEDrawList;
            SetStructFather( Struct, GetScreen() );
            GetScreen()->EEDrawList = Struct;
            PickedList = (DrawPickedStruct*) PickedList->Pnext;
        }

        /* Save wrapper list in undo stack */
        SaveCopyInUndoList( DrawStruct, IS_NEW );
    }
    else
    {
        if( DrawStruct->Type() == DRAW_LIB_ITEM_STRUCT_TYPE )
        {
            ( (EDA_SchComponentStruct*) DrawStruct )->m_TimeStamp = GetTimeStamp();
            ( (EDA_SchComponentStruct*) DrawStruct )->ClearAnnotation();
        }
        SetStructFather( DrawStruct, GetScreen() );
        RedrawOneStruct( DrawPanel, DC, DrawStruct, GR_DEFAULT_DRAWMODE );
        DrawStruct->Pnext = GetScreen()->EEDrawList;
        GetScreen()->EEDrawList = DrawStruct;
        SaveCopyInUndoList( DrawStruct, IS_NEW );
    }

    /* clear .m_Flags member for all items */
    EDA_BaseStruct* Struct;
    for( Struct = GetScreen()->EEDrawList; Struct != NULL; Struct = Struct->Pnext )
        Struct->m_Flags = 0;

    GetScreen()->SetModify();

    return;
}


/*****************************************************************************
* Routine to place a given object.											 *
*****************************************************************************/
bool PlaceStruct( BASE_SCREEN* screen, EDA_BaseStruct* DrawStruct )
{
    DrawPickedStruct* DrawStructs;
    wxPoint           move_vector;

    if( !DrawStruct )
        return FALSE;

    move_vector.x = screen->m_Curseur.x -
                    screen->BlockLocate.m_BlockLastCursorPosition.x;
    move_vector.y = screen->m_Curseur.y -
                    screen->BlockLocate.m_BlockLastCursorPosition.y;

    switch( DrawStruct->Type() )
    {
    default:
    case TYPE_NOT_INIT:
        return FALSE;

    case DRAW_POLYLINE_STRUCT_TYPE:
    case DRAW_JUNCTION_STRUCT_TYPE:
    case DRAW_SEGMENT_STRUCT_TYPE:
    case DRAW_BUSENTRY_STRUCT_TYPE:
    case DRAW_TEXT_STRUCT_TYPE:
    case DRAW_LABEL_STRUCT_TYPE:
    case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
    case DRAW_LIB_ITEM_STRUCT_TYPE:
    case DRAW_SHEET_STRUCT_TYPE:
    case DRAW_SHEETLABEL_STRUCT_TYPE:
    case DRAW_MARKER_STRUCT_TYPE:
    case DRAW_NOCONNECT_STRUCT_TYPE:
        MoveOneStruct( DrawStruct, move_vector );
        break;

    case DRAW_PICK_ITEM_STRUCT_TYPE:
        DrawStructs = (DrawPickedStruct*) DrawStruct;
        while( DrawStructs )
        {
            MoveOneStruct( DrawStructs->m_PickedStruct, move_vector );
            DrawStructs = (DrawPickedStruct*) DrawStructs->Pnext;
        }

        break;
    }

    return TRUE;
}


/**************************************************************************/
void MoveOneStruct( EDA_BaseStruct* DrawStruct, const wxPoint& move_vector )
/*************************************************************************/

/* Given a structure move it by Dx, Dy.
 */
{
    int ii, * Points;
    DrawPolylineStruct*     DrawPoly;
    DrawJunctionStruct*     DrawConnect;
    EDA_DrawLineStruct*     DrawSegment;
    DrawBusEntryStruct*     DrawRaccord;
    EDA_SchComponentStruct* DrawLibItem;
    DrawSheetStruct*        DrawSheet;
    DrawSheetLabelStruct*   DrawSheetLabel;
    DrawMarkerStruct*       DrawMarker;
    DrawNoConnectStruct*    DrawNoConnect;

    if( !DrawStruct )
        return;

    switch( DrawStruct->Type() )
    {
    case TYPE_NOT_INIT:
        break;

    case DRAW_POLYLINE_STRUCT_TYPE:
        DrawPoly = (DrawPolylineStruct*) DrawStruct;
        Points   = DrawPoly->m_Points;
        for( ii = 0; ii < DrawPoly->m_NumOfPoints; ii++ )
        {
            Points[ii * 2]     += move_vector.x;
            Points[ii * 2 + 1] += move_vector.y;
        }

        break;

    case DRAW_SEGMENT_STRUCT_TYPE:
        DrawSegment = (EDA_DrawLineStruct*) DrawStruct;
        if( (DrawSegment->m_Flags & STARTPOINT) == 0 )
        {
            DrawSegment->m_Start += move_vector;
        }
        if( (DrawSegment->m_Flags & ENDPOINT) == 0 )
        {
            DrawSegment->m_End += move_vector;
        }
        break;

    case DRAW_BUSENTRY_STRUCT_TYPE:
        DrawRaccord = (DrawBusEntryStruct*) DrawStruct;
        DrawRaccord->m_Pos += move_vector;
        break;

    case DRAW_JUNCTION_STRUCT_TYPE:
        DrawConnect = (DrawJunctionStruct*) DrawStruct;
        DrawConnect->m_Pos += move_vector;
        break;

    case DRAW_MARKER_STRUCT_TYPE:
        DrawMarker = (DrawMarkerStruct*) DrawStruct;
        DrawMarker->m_Pos += move_vector;
        break;

    case DRAW_NOCONNECT_STRUCT_TYPE:
        DrawNoConnect = (DrawNoConnectStruct*) DrawStruct;
        DrawNoConnect->m_Pos += move_vector;
        break;

    case DRAW_TEXT_STRUCT_TYPE:
             #define DrawText ( (DrawTextStruct*) DrawStruct )
        DrawText->m_Pos += move_vector;
        break;

    case DRAW_LABEL_STRUCT_TYPE:
             #define DrawLabel ( (DrawLabelStruct*) DrawStruct )
        DrawLabel->m_Pos += move_vector;
        break;

    case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
             #define DrawGlobalLabel ( (DrawGlobalLabelStruct*) DrawStruct )
        DrawGlobalLabel->m_Pos += move_vector;
        break;

    case DRAW_LIB_ITEM_STRUCT_TYPE:
        DrawLibItem = (EDA_SchComponentStruct*) DrawStruct;
        DrawLibItem->m_Pos += move_vector;
        for( ii = 0; ii < NUMBER_OF_FIELDS; ii++ )
        {
            DrawLibItem->m_Field[ii].m_Pos += move_vector;
        }

        break;

    case DRAW_SHEET_STRUCT_TYPE:
        DrawSheet = (DrawSheetStruct*) DrawStruct;
        DrawSheet->m_Pos += move_vector;
        DrawSheetLabel    = DrawSheet->m_Label;
        while( DrawSheetLabel != NULL )
        {
            DrawSheetLabel->m_Pos += move_vector;
            DrawSheetLabel = DrawSheetLabel->Next();
        }

        break;

    case DRAW_SHEETLABEL_STRUCT_TYPE:
        DrawSheetLabel = (DrawSheetLabelStruct*) DrawStruct;
        DrawSheetLabel->m_Pos += move_vector;
        break;

    case DRAW_PICK_ITEM_STRUCT_TYPE:
        break;

    default:
        break;
    }
}


/************************************************************/
EDA_BaseStruct* DuplicateStruct( EDA_BaseStruct* DrawStruct )
/************************************************************/

/* Routine to create a new copy of given struct.
 *  The new object is not put in draw list (not linked)
 */
{
    EDA_BaseStruct* NewDrawStruct = NULL;

    if( DrawStruct == NULL )
    {
        DisplayError( NULL, wxT( "DuplicateStruct error: NULL struct" ) );
        return NULL;
    }

    switch( DrawStruct->Type() )
    {
    case DRAW_POLYLINE_STRUCT_TYPE:
        NewDrawStruct = ( (DrawPolylineStruct*) DrawStruct )->GenCopy();
        break;

    case DRAW_SEGMENT_STRUCT_TYPE:
        NewDrawStruct = ( (EDA_DrawLineStruct*) DrawStruct )->GenCopy();
        break;

    case DRAW_BUSENTRY_STRUCT_TYPE:
        NewDrawStruct = ( (DrawBusEntryStruct*) DrawStruct )->GenCopy();
        break;

    case DRAW_JUNCTION_STRUCT_TYPE:
        NewDrawStruct = ( (DrawJunctionStruct*) DrawStruct )->GenCopy();
        break;

    case DRAW_MARKER_STRUCT_TYPE:
        NewDrawStruct = ( (DrawMarkerStruct*) DrawStruct )->GenCopy();
        break;

    case DRAW_NOCONNECT_STRUCT_TYPE:
        NewDrawStruct = ( (DrawNoConnectStruct*) DrawStruct )->GenCopy();
        break;

    case DRAW_TEXT_STRUCT_TYPE:
        NewDrawStruct = ( (DrawTextStruct*) DrawStruct )->GenCopy();
        break;

    case DRAW_LABEL_STRUCT_TYPE:
        NewDrawStruct = ( (DrawLabelStruct*) DrawStruct )->GenCopy();
        break;

    case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
        NewDrawStruct = ( (DrawGlobalLabelStruct*) DrawStruct )->GenCopy();
        break;

    case DRAW_LIB_ITEM_STRUCT_TYPE:
        NewDrawStruct = ( (EDA_SchComponentStruct*) DrawStruct )->GenCopy();
        break;

    case DRAW_SHEET_STRUCT_TYPE:
        NewDrawStruct = ( (DrawSheetStruct*) DrawStruct )->GenCopy();
        break;

    case DRAW_PICK_ITEM_STRUCT_TYPE:
    {
        DrawPickedStruct* NewPickedItem, * PickedList = NULL,
        * LastPickedItem = NULL;
        PickedList = (DrawPickedStruct*) DrawStruct;
        while( PickedList )
        {
            NewPickedItem = new DrawPickedStruct();
            if( NewDrawStruct == NULL )
                NewDrawStruct = NewPickedItem;
            if( LastPickedItem )
                LastPickedItem->Pnext = NewPickedItem;
            LastPickedItem = NewPickedItem;
            NewPickedItem->m_PickedStruct =
                DuplicateStruct( PickedList->m_PickedStruct );
            PickedList = (DrawPickedStruct*) PickedList->Pnext;
        }

        break;
    }

    case DRAW_SHEETLABEL_STRUCT_TYPE:
    case DRAW_PART_TEXT_STRUCT_TYPE:
    case SCREEN_STRUCT_TYPE:
    default:
    {
        wxString msg;
        msg << wxT( "DuplicateStruct error: unexpected StructType " ) <<
        DrawStruct->Type() << wxT( " " ) << DrawStruct->GetClass();
        DisplayError( NULL, msg );
    }
        break;
    }

    NewDrawStruct->m_Image = DrawStruct;
    return NewDrawStruct;
}


/****************************************************/
static void CollectStructsToDrag( SCH_SCREEN* screen )
/****************************************************/
{
    DrawPickedStruct*   DrawStructs, * FirstPicked;
    EDA_BaseStruct*     Struct;
    EDA_DrawLineStruct* SegmStruct;
    int ox, oy, fx, fy;

    /* Set membre .m_Flags des segments */
    for( Struct = screen->EEDrawList; Struct != NULL; Struct = Struct->Pnext )
        Struct->m_Flags = 0;

    if( screen->BlockLocate.m_BlockDrawStruct->Type() == DRAW_SEGMENT_STRUCT_TYPE )
        screen->BlockLocate.m_BlockDrawStruct->m_Flags = SELECTED;

    else if( screen->BlockLocate.m_BlockDrawStruct->Type() == DRAW_PICK_ITEM_STRUCT_TYPE )
    {
        DrawStructs = (DrawPickedStruct*) screen->BlockLocate.m_BlockDrawStruct;
        while( DrawStructs )
        {
            Struct          = DrawStructs->m_PickedStruct;
            DrawStructs     = (DrawPickedStruct*) DrawStructs->Pnext;
            Struct->m_Flags = SELECTED;
        }
    }

    if( screen->BlockLocate.m_Command != BLOCK_DRAG )
        return;

    ox = screen->BlockLocate.GetX();
    oy = screen->BlockLocate.GetY();
    fx = screen->BlockLocate.GetRight();
    fy = screen->BlockLocate.GetBottom();

    if( fx < ox )
        EXCHG( fx, ox );
    if( fy < oy )
        EXCHG( fy, oy );

    /* Pour Drag Block: remise sous forme de liste de structure, s'il n'y
     *  a qu'un seul element ( pour homogeneiser les traitements ulterieurs */
    if( screen->BlockLocate.m_BlockDrawStruct->Type() != DRAW_PICK_ITEM_STRUCT_TYPE )
    {
        DrawStructs = new DrawPickedStruct( screen->BlockLocate.m_BlockDrawStruct );
        screen->BlockLocate.m_BlockDrawStruct = DrawStructs;
    }

    /* Suppression du deplacement des extremites de segments hors cadre
     *  de selection */
    DrawStructs = (DrawPickedStruct*) screen->BlockLocate.m_BlockDrawStruct;
    while( DrawStructs )
    {
        Struct      = DrawStructs->m_PickedStruct;
        DrawStructs = (DrawPickedStruct*) DrawStructs->Pnext;
        if( Struct->Type() == DRAW_SEGMENT_STRUCT_TYPE )
        {
            SegmStruct = (EDA_DrawLineStruct*) Struct;
            if( (SegmStruct->m_Start.x < ox) || (SegmStruct->m_Start.x > fx)
               || (SegmStruct->m_Start.y < oy) || (SegmStruct->m_Start.y > fy) )
                SegmStruct->m_Flags |= STARTPOINT;

            if( (SegmStruct->m_End.x < ox) || (SegmStruct->m_End.x > fx)
               || (SegmStruct->m_End.y < oy) || (SegmStruct->m_End.y > fy) )
                SegmStruct->m_Flags |= ENDPOINT;
        }
    }

    /* Recherche des elements complementaires a "dragger", c'est a dire les
     *  fils et connexions hors bloc relies a des pins ou entries elles meme
     *  draggees */

    FirstPicked = DrawStructs = (DrawPickedStruct*) screen->BlockLocate.m_BlockDrawStruct;
    while( DrawStructs )
    {
        Struct      = DrawStructs->m_PickedStruct;
        DrawStructs = (DrawPickedStruct*) DrawStructs->Pnext;
        if( Struct->Type() == DRAW_LIB_ITEM_STRUCT_TYPE )
        {
            LibEDA_BaseStruct* DrawItem;
            wxPoint pos;
            DrawItem = GetNextPinPosition( (EDA_SchComponentStruct*) Struct, pos );
            while( DrawItem )
            {
                if( (pos.x < ox) || (pos.x > fx) || (pos.y < oy) || (pos.y > fy) )
                    AddPickedItem( screen, pos );

                DrawItem = GetNextPinPosition( NULL, pos );
            }
        }

        if( Struct->Type() == DRAW_SHEET_STRUCT_TYPE )
        {
            DrawSheetLabelStruct* SLabel = ( (DrawSheetStruct*) Struct )->m_Label;
            while( SLabel )
            {
                if( SLabel->Type() == DRAW_SHEETLABEL_STRUCT_TYPE )
                    AddPickedItem( screen, SLabel->m_Pos );
                SLabel = (DrawSheetLabelStruct*) SLabel->Pnext;
            }
        }

        if( Struct->Type() == DRAW_BUSENTRY_STRUCT_TYPE )
        {
            DrawBusEntryStruct* item = (DrawBusEntryStruct*) Struct;
            AddPickedItem( screen, item->m_Pos );
            AddPickedItem( screen, item->m_End() );
        }
    }
}


/******************************************************************/
static void AddPickedItem( SCH_SCREEN* screen, wxPoint position )
/******************************************************************/
{
    DrawPickedStruct* DrawStructs;
    EDA_BaseStruct*   Struct;

    /* Examen de la liste des elements deja selectionnes */
    DrawStructs = (DrawPickedStruct*) screen->BlockLocate.m_BlockDrawStruct;
    while( DrawStructs )
    {
        Struct      = DrawStructs->m_PickedStruct;
        DrawStructs = (DrawPickedStruct*) DrawStructs->Pnext;

        switch( Struct->Type() )
        {
        case DRAW_SEGMENT_STRUCT_TYPE:
                #undef STRUCT
                #define STRUCT ( (EDA_DrawLineStruct*) Struct )
            if( STRUCT->m_Start == position )
                STRUCT->m_Flags &= ~STARTPOINT;

            if( STRUCT->m_End == position )
                STRUCT->m_Flags &= ~ENDPOINT;
            break;

        default:
            break;
        }
    }

    /* Examen de la liste des elements non selectionnes */

    Struct = screen->EEDrawList;
    while( Struct )
    {
        switch( Struct->Type() )
        {
        case TYPE_NOT_INIT:
            break;

        case DRAW_POLYLINE_STRUCT_TYPE:
            if( Struct->m_Flags & SELECTED )
                break;                                  /* Deja en liste */
            break;

        case DRAW_JUNCTION_STRUCT_TYPE:
                #undef STRUCT
                #define STRUCT ( (DrawJunctionStruct*) Struct )
            if( Struct->m_Flags & SELECTED )
                break;                                  /* Deja en liste */
            if( STRUCT->m_Pos != position )
                break;
            DrawStructs = new DrawPickedStruct( Struct );
            DrawStructs->Pnext =
                (DrawPickedStruct*) screen->BlockLocate.m_BlockDrawStruct;
            screen->BlockLocate.m_BlockDrawStruct = (EDA_BaseStruct*) DrawStructs;
            break;

        case DRAW_SEGMENT_STRUCT_TYPE:
                #undef STRUCT
                #define STRUCT ( (EDA_DrawLineStruct*) Struct )
            if( Struct->m_Flags & SELECTED )
                break;                                  /* Deja en liste */
            if( STRUCT->m_Start == position )
            {
                DrawStructs = new DrawPickedStruct( Struct );
                DrawStructs->Pnext =
                    (DrawPickedStruct*) screen->BlockLocate.m_BlockDrawStruct;
                screen->BlockLocate.m_BlockDrawStruct = (EDA_BaseStruct*) DrawStructs;
                Struct->m_Flags  = SELECTED | ENDPOINT | STARTPOINT;
                Struct->m_Flags &= ~STARTPOINT;
            }
            else if( STRUCT->m_End == position )
            {
                DrawStructs = new DrawPickedStruct( Struct );
                DrawStructs->Pnext =
                    (DrawPickedStruct*) screen->BlockLocate.m_BlockDrawStruct;
                screen->BlockLocate.m_BlockDrawStruct = (EDA_BaseStruct*) DrawStructs;
                Struct->m_Flags  = SELECTED | ENDPOINT | STARTPOINT;
                Struct->m_Flags &= ~ENDPOINT;
            }
            break;

        case DRAW_BUSENTRY_STRUCT_TYPE:
            break;

        case DRAW_TEXT_STRUCT_TYPE:
            break;

        case DRAW_LABEL_STRUCT_TYPE:
                #undef STRUCT
                #define STRUCT ( (DrawLabelStruct*) Struct )
            if( Struct->m_Flags & SELECTED )
                break;                                  /* Already in list */
            if( STRUCT->m_Pos != position )
                break;
            DrawStructs = new DrawPickedStruct( Struct );
            DrawStructs->Pnext =
                (DrawPickedStruct*) screen->BlockLocate.m_BlockDrawStruct;
            screen->BlockLocate.m_BlockDrawStruct = (EDA_BaseStruct*) DrawStructs;
            Struct->m_Flags |= SELECTED;
            break;

        case DRAW_GLOBAL_LABEL_STRUCT_TYPE:
                #undef STRUCT
                #define STRUCT ( (DrawGlobalLabelStruct*) Struct )
            if( Struct->m_Flags & SELECTED )
                break;                                  /* Already in list */
            if( STRUCT->m_Pos != position )
                break;
            DrawStructs = new DrawPickedStruct( Struct );
            DrawStructs->Pnext =
                (DrawPickedStruct*) screen->BlockLocate.m_BlockDrawStruct;
            screen->BlockLocate.m_BlockDrawStruct = (EDA_BaseStruct*) DrawStructs;
            Struct->m_Flags |= SELECTED;
            break;

        case DRAW_LIB_ITEM_STRUCT_TYPE:
            break;

        case DRAW_SHEET_STRUCT_TYPE:
            break;

        case DRAW_SHEETLABEL_STRUCT_TYPE:
            break;

        case DRAW_PICK_ITEM_STRUCT_TYPE:
            break;

        case DRAW_MARKER_STRUCT_TYPE:
                #undef STRUCT
                #define STRUCT ( (DrawMarkerStruct*) Struct )
            if( Struct->m_Flags & SELECTED )
                break;                                  /* Already in list */
            if( STRUCT->m_Pos != position )
                break;
            DrawStructs = new DrawPickedStruct( Struct );
            DrawStructs->Pnext =
                (DrawPickedStruct*) screen->BlockLocate.m_BlockDrawStruct;
            screen->BlockLocate.m_BlockDrawStruct = (EDA_BaseStruct*) DrawStructs;
            Struct->m_Flags |= SELECTED;
            break;

        case DRAW_NOCONNECT_STRUCT_TYPE:
                #undef STRUCT
                #define STRUCT ( (DrawNoConnectStruct*) Struct )
            if( Struct->m_Flags & SELECTED )
                break;                                  /* Already in list */
            if( STRUCT->m_Pos != position )
                break;
            DrawStructs = new DrawPickedStruct( Struct );
            DrawStructs->Pnext =
                (DrawPickedStruct*) screen->BlockLocate.m_BlockDrawStruct;
            screen->BlockLocate.m_BlockDrawStruct = (EDA_BaseStruct*) DrawStructs;
            Struct->m_Flags |= SELECTED;
            break;

        default:
            break;
        }
        Struct = Struct->Pnext;
    }
}


/*********************************************************************************/
static LibEDA_BaseStruct* GetNextPinPosition( EDA_SchComponentStruct* DrawLibItem,
                                              wxPoint & position )
/*********************************************************************************/
{
    EDA_LibComponentStruct* Entry;
    static LibEDA_BaseStruct* NextItem;
    static int Multi, convert, PartX, PartY, TransMat[2][2];
    LibEDA_BaseStruct* DEntry;
    int orient;
    LibDrawPin* Pin;

    if( DrawLibItem )
    {
        NextItem = NULL;
        if( ( Entry = FindLibPart( DrawLibItem->m_ChipName.GetData(), wxEmptyString,
                                   FIND_ROOT ) ) == NULL )
            return NULL;
        DEntry  = Entry->m_Drawings;
        Multi   = DrawLibItem->m_Multi;
        convert = DrawLibItem->m_Convert;
        PartX   = DrawLibItem->m_Pos.x;
        PartY   = DrawLibItem->m_Pos.y;
        memcpy( TransMat, DrawLibItem->m_Transform, sizeof(TransMat) );
    }
    else
        DEntry = NextItem;

    for( ; DEntry != NULL; DEntry = DEntry->Next() )
    {
        /* Elimination des elements non relatifs a l'unite */
        if( Multi && DEntry->m_Unit && (DEntry->m_Unit != Multi) )
            continue;
        if( convert && DEntry->m_Convert && (DEntry->m_Convert != convert) )
            continue;
        if( DEntry->Type() != COMPONENT_PIN_DRAW_TYPE )
            continue;

        Pin = (LibDrawPin*) DEntry;

        /* Calcul de l'orientation reelle de la Pin */
        orient = Pin->ReturnPinDrawOrient( TransMat );

        /* Calcul de la position du point de reference */
        position.x = PartX + (TransMat[0][0] * Pin->m_Pos.x)
             + (TransMat[0][1] * Pin->m_Pos.y);
        position.y = PartY + (TransMat[1][0] * Pin->m_Pos.x)
             + (TransMat[1][1] * Pin->m_Pos.y);
        NextItem = DEntry->Next();
        return DEntry;
    }

    NextItem = NULL;
    return NULL;
}
