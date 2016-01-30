/****************************************************/
/* fonctions de la classe MIRE (targets for photos) */
/****************************************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "pcbnew.h"


MIREPCB::MIREPCB( BOARD_ITEM* StructFather ) :
    BOARD_ITEM( StructFather, TYPEMIRE )
{
    m_Shape = 0;
    m_Size  = 5000;
}


MIREPCB::~MIREPCB()
{
}


/***************************/
void MIREPCB::UnLink()
/***************************/

/* supprime du chainage la structure Struct
 *  les structures arrieres et avant sont chainees directement
 */
{
    /* Modification du chainage arriere */
    if( Pback )
    {
        if( Pback->Type() != TYPEPCB )
        {
            Pback->Pnext = Pnext;
        }
        else /* Le chainage arriere pointe sur la structure "Pere" */
        {
            ( (BOARD*) Pback )->m_Drawings = (BOARD_ITEM*) Pnext;
        }
    }

    /* Modification du chainage avant */
    if( Pnext )
        Pnext->Pback = Pback;

    Pnext = Pback = NULL;
}


/**********************************/
void MIREPCB::Copy( MIREPCB* source )
/**********************************/
{
    m_Layer     = source->m_Layer;
    m_Width     = source->m_Width;
    m_Pos       = source->m_Pos;
    m_Shape     = source->m_Shape;
    m_Size      = source->m_Size;
    m_TimeStamp = GetTimeStamp();
}


/**************************************************************/
bool MIREPCB::ReadMirePcbDescr( FILE* File, int* LineNum )
/**************************************************************/

/* Lecture de la description de 1 segment type Drawing PCB
 */
{
    char Line[256];

    while( GetLine( File, Line, LineNum ) != NULL )
    {
        if( strnicmp( Line, "$End", 4 ) == 0 )
            return TRUE;                                /* fin de liste */
        if( Line[0] == 'P' )
        {
            sscanf( Line + 2, " %X %d %d %d %d %d %lX",
                    &m_Shape, &m_Layer,
                    &m_Pos.x, &m_Pos.y,
                    &m_Size, &m_Width, &m_TimeStamp );
            if( m_Layer < FIRST_NO_COPPER_LAYER )
                m_Layer = FIRST_NO_COPPER_LAYER;
            if( m_Layer > LAST_NO_COPPER_LAYER )
                m_Layer = LAST_NO_COPPER_LAYER;
        }
    }

    return FALSE;
}


#if 0 // replaced by Save()
/************************************************/
bool MIREPCB::WriteMirePcbDescr( FILE* File )
/************************************************/
{
    if( GetState( DELETED ) )
        return FALSE;

    fprintf( File, "$MIREPCB\n" );
    fprintf( File, "Po %X %d %d %d %d %d %8.8lX\n",
             m_Shape, m_Layer,
             m_Pos.x, m_Pos.y,
             m_Size, m_Width, m_TimeStamp );
    fprintf( File, "$EndMIREPCB\n" );
    return TRUE;
}
#endif


bool MIREPCB::Save( FILE* aFile ) const
{
    if( GetState( DELETED ) )
        return true;

    bool rc = false;
    
    if( fprintf( aFile, "$MIREPCB\n" ) != sizeof("$MIREPCB\n")-1 )
        goto out;
    
    fprintf( aFile, "Po %X %d %d %d %d %d %8.8lX\n",
             m_Shape, m_Layer,
             m_Pos.x, m_Pos.y,
             m_Size, m_Width, m_TimeStamp );
    
    if( fprintf( aFile, "$EndMIREPCB\n" ) != sizeof("$EndMIREPCB\n")-1 )
        goto out;
    
    rc = true;

out:    
    return rc;
}
    
    


/**********************************************************/
void MIREPCB::Draw( WinEDA_DrawPanel* panel, wxDC* DC,
                    const wxPoint& offset, int mode_color )
/**********************************************************/

/* Affichage de 1 mire : 2 segments + 1 cercle
 *  le cercle a pour rayon le demi rayon de la mire
 *  les 2 traits ont pour longueur le diametre de la mire
 */
{
    int rayon, ox, oy, gcolor, width;
    int dx1, dx2, dy1, dy2;
    int typeaff;
    int zoom;

    ox = m_Pos.x + offset.x;
    oy = m_Pos.y + offset.y;

    gcolor = g_DesignSettings.m_LayerColor[m_Layer];
    if( (gcolor & ITEM_NOT_SHOW) != 0 )
        return;

    zoom = panel->GetZoom();

    GRSetDrawMode( DC, mode_color );
    typeaff = DisplayOpt.DisplayDrawItems;
    width   = m_Width;
    if( width / zoom < 2 )
        typeaff = FILAIRE;

    /* Trace du cercle: */
    rayon = m_Size / 4;

    switch( typeaff )
    {
    case FILAIRE:
        width = 0;

    case FILLED:
        GRCircle( &panel->m_ClipBox, DC, ox, oy, rayon, width, gcolor );
        break;

    case SKETCH:
        GRCircle( &panel->m_ClipBox, DC, ox, oy, rayon + (width / 2), gcolor );
        GRCircle( &panel->m_ClipBox, DC, ox, oy, rayon - (width / 2), gcolor );
        break;
    }


    /* Trace des 2 traits */
    rayon = m_Size / 2;
    dx1   = rayon, dy1 = 0;
    dx2   = 0, dy2 = rayon;

    if( m_Shape ) /* Forme X */
    {
        dx1 = dy1 = (rayon * 7) / 5;
        dx2 = dx1; dy2 = -dy1;
    }

    switch( typeaff )
    {
    case FILAIRE:
    case FILLED:
        GRLine( &panel->m_ClipBox, DC, ox - dx1, oy - dy1,
                ox + dx1, oy + dy1, width, gcolor );
        GRLine( &panel->m_ClipBox, DC, ox - dx2, oy - dy2,
                ox + dx2, oy + dy2, width, gcolor );
        break;

    case SKETCH:
        GRCSegm( &panel->m_ClipBox, DC, ox - dx1, oy - dy1,
                 ox + dx1, oy + dy1,
                 width, gcolor );
        GRCSegm( &panel->m_ClipBox, DC, ox - dx2, oy - dy2,
                 ox + dx2, oy + dy2,
                 width, gcolor );
        break;
    }
}


/**
 * Function HitTest
 * tests if the given wxPoint is within the bounds of this object.
 * @param refPos A wxPoint to test
 * @return bool - true if a hit, else false
 */
bool MIREPCB::HitTest( const wxPoint& refPos )
{
    int dX    = refPos.x - m_Pos.x;
    int dY    = refPos.y - m_Pos.y;
    int rayon = m_Size / 2;

    return abs(dX)<=rayon && abs(dY)<=rayon;
}

