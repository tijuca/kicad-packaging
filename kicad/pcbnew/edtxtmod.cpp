/********************/
/* Edi module text. */
/********************/

#include "fctsys.h"
#include "gr_basic.h"
#include "common.h"
#include "class_drawpanel.h"

#include "pcbnew.h"
#include "drawtxt.h"
#include "trigo.h"
#include "protos.h"


static void Show_MoveTexte_Module( WinEDA_DrawPanel* panel,
                                   wxDC*             DC,
                                   bool              erase );
static void AbortMoveTextModule( WinEDA_DrawPanel* Panel, wxDC* DC );


wxPoint        MoveVector;              // Move vector for move edge, exported
                                        // to dialog_edit mod_text.cpp
static wxPoint TextInitialPosition;     // Mouse cursor initial position for
                                        // undo/abort move command
static int     TextInitialOrientation;  // module text initial orientation for
                                        // undo/abort move+rot command+rot


/* Add a new graphical text to the active module (footprint)
 *  Note there always are 2 texts: reference and value.
 *  New texts have the member TEXTE_MODULE.m_Type set to TEXT_is_DIVERS
 */
TEXTE_MODULE* WinEDA_BasePcbFrame::CreateTextModule( MODULE* Module, wxDC* DC )
{
    TEXTE_MODULE* Text;

    Text = new TEXTE_MODULE( Module );

    /* Add the new text object to the beginning of the draw item list. */
    if( Module )
        Module->m_Drawings.PushFront( Text );

    Text->m_Flags = IS_NEW;

    Text->m_Text = wxT( "text" );

    ModuleTextWidth = Clamp_Text_PenSize( ModuleTextWidth,
                                          MIN( ModuleTextSize.x,
                                               ModuleTextSize.y ), true );
    Text->m_Size  = ModuleTextSize;
    Text->m_Width = ModuleTextWidth;
    Text->m_Pos   = GetScreen()->m_Curseur;
    Text->SetLocalCoord();

    InstallTextModOptionsFrame( Text, NULL );
    DrawPanel->MouseToCursorSchema();

    Text->m_Flags = 0;
    if( DC )
        Text->Draw( DrawPanel, DC, GR_OR );

    Text->DisplayInfo( this );

    return Text;
}


/* Rotate text 90 degrees.
 */
void WinEDA_BasePcbFrame::RotateTextModule( TEXTE_MODULE* Text, wxDC* DC )
{
    if( Text == NULL )
        return;

    MODULE* module = (MODULE*) Text->GetParent();

    if( module && module->m_Flags == 0 && Text->m_Flags == 0 ) // prepare undo
                                                               // command
    {
        if( this->m_Ident == PCB_FRAME )
            SaveCopyInUndoList( module, UR_CHANGED );
    }

    // we expect MoveVector to be (0,0) if there is no move in progress
    Text->Draw( DrawPanel, DC, GR_XOR, MoveVector );

    Text->m_Orient += 900;
    while( Text->m_Orient >= 1800 )
        Text->m_Orient -= 1800;

    Text->Draw( DrawPanel, DC, GR_XOR, MoveVector );
    Text->DisplayInfo( this );

    if( module )
        module->m_LastEdit_Time = time( NULL );
    OnModify();
}


/*
 * Deletes text in module (if not the reference or value)
 */
void WinEDA_BasePcbFrame::DeleteTextModule( TEXTE_MODULE* Text )
{
    MODULE* Module;

    if( Text == NULL )
        return;

    Module = (MODULE*) Text->GetParent();

    if( Text->m_Type == TEXT_is_DIVERS )
    {
        DrawPanel->PostDirtyRect( Text->GetBoundingBox() );
        Text->DeleteStructure();
        OnModify();
        Module->m_LastEdit_Time = time( NULL );
    }
}


/*
 * Abort text move in progress.
 *
 * If a text is selected, its initial coordinates are regenerated.
 */
static void AbortMoveTextModule( WinEDA_DrawPanel* Panel, wxDC* DC )
{
    BASE_SCREEN*  screen = Panel->GetScreen();
    TEXTE_MODULE* Text   = (TEXTE_MODULE*) screen->GetCurItem();
    MODULE*       Module;

    Panel->ManageCurseur = NULL;
    Panel->ForceCloseManageCurseur = NULL;

    if( Text == NULL )
        return;

    Module = (MODULE*) Text->GetParent();

    Text->Draw( Panel, DC, GR_XOR, MoveVector );

    // If the text was moved (the move does not change internal data)
    // it could be rotated while moving. So set old value for orientation
    if( (Text->m_Flags & IS_MOVED) )
        Text->m_Orient = TextInitialOrientation;

    /* Redraw the text */
    Panel->PostDirtyRect( Text->GetBoundingBox() );

    // leave it at (0,0) so we can use it Rotate when not moving.
    MoveVector.x = MoveVector.y = 0;

    Text->m_Flags   = 0;
    Module->m_Flags = 0;

    screen->SetCurItem( NULL );
}


/* Start a text move.
 */
void WinEDA_BasePcbFrame::StartMoveTexteModule( TEXTE_MODULE* Text, wxDC* DC )
{
    MODULE* Module;

    if( Text == NULL )
        return;

    Module = (MODULE*) Text->GetParent();

    Text->m_Flags   |= IS_MOVED;
    Module->m_Flags |= IN_EDIT;

    MoveVector.x = MoveVector.y = 0;

    TextInitialPosition    = Text->m_Pos;
    TextInitialOrientation = Text->m_Orient;

    Text->DisplayInfo( this );

    SetCurItem( Text );
    DrawPanel->ManageCurseur = Show_MoveTexte_Module;
    DrawPanel->ForceCloseManageCurseur = AbortMoveTextModule;

    DrawPanel->ManageCurseur( DrawPanel, DC, TRUE );
}


/* Place the text a the cursor position when the left mouse button is clicked.
 */
void WinEDA_BasePcbFrame::PlaceTexteModule( TEXTE_MODULE* Text, wxDC* DC )
{
    if( Text != NULL )
    {
        DrawPanel->PostDirtyRect( Text->GetBoundingBox() );

        /* Update the coordinates for anchor. */
        MODULE* Module = (MODULE*) Text->GetParent();
        if( Module )
        {
            // Prepare undo command (a rotation can be made while moving)
            EXCHG( Text->m_Orient, TextInitialOrientation );
            if( m_Ident == PCB_FRAME )
                SaveCopyInUndoList( Module, UR_CHANGED );
            else
                SaveCopyInUndoList( Module, UR_MODEDIT );
            EXCHG( Text->m_Orient, TextInitialOrientation );

            // Set the new position for text.
            Text->m_Pos = GetScreen()->m_Curseur;
            wxPoint textRelPos = Text->m_Pos - Module->m_Pos;
            RotatePoint( &textRelPos, -Module->m_Orient );
            Text->m_Pos0    = textRelPos;
            Text->m_Flags   = 0;
            Module->m_Flags = 0;
            Module->m_LastEdit_Time = time( NULL );
            OnModify();

            /* Redraw text. */
            DrawPanel->PostDirtyRect( Text->GetBoundingBox() );
        }
        else
            Text->m_Pos = GetScreen()->m_Curseur;
    }

    // leave it at (0,0) so we can use it Rotate when not moving.
    MoveVector.x = MoveVector.y = 0;

    DrawPanel->ManageCurseur = NULL;
    DrawPanel->ForceCloseManageCurseur = NULL;
}


static void Show_MoveTexte_Module( WinEDA_DrawPanel* panel, wxDC* DC,
                                   bool erase )
{
    BASE_SCREEN*  screen = panel->GetScreen();
    TEXTE_MODULE* Text   = (TEXTE_MODULE*) screen->GetCurItem();

    if( Text == NULL )
        return;

    /* Undraw the text */
    if( erase )
        Text->Draw( panel, DC, GR_XOR, MoveVector );

    MoveVector = TextInitialPosition - screen->m_Curseur;

    /* Redraw the text */
    Text->Draw( panel, DC, GR_XOR, MoveVector );
}
