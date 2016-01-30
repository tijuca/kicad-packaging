/*********************************************************/
/* class TEXTE_PCB : texts on copper or technical layers */
/*********************************************************/

#include "fctsys.h"
#include "wxstruct.h"
#include "gr_basic.h"
#include "common.h"
#include "kicad_string.h"

#include "pcbnew.h"


/*******************/
/* class TEXTE_PCB */
/*******************/

TEXTE_PCB::TEXTE_PCB( BOARD_ITEM* parent ) :
    BOARD_ITEM( parent, TYPE_TEXTE ),
    EDA_TextStruct()
{
}


/* Destructeur */
TEXTE_PCB:: ~TEXTE_PCB()
{
}


/* copie de stucture */
void TEXTE_PCB::Copy( TEXTE_PCB* source )
{
    m_Parent    = source->m_Parent;
    Pback       = Pnext = NULL;
    m_Mirror    = source->m_Mirror;
    m_Size      = source->m_Size;
    m_Orient    = source->m_Orient;
    m_Pos       = source->m_Pos;
    m_Layer     = source->m_Layer;
    m_Width     = source->m_Width;
    m_Attributs = source->m_Attributs;
    m_Italic    = source->m_Italic;
    m_HJustify  = source->m_HJustify;
    m_VJustify  = source->m_VJustify;

    m_Text = source->m_Text;
}


/****************************************************************/
int TEXTE_PCB::ReadTextePcbDescr( FILE* File, int* LineNum )
/****************************************************************/
{
    char text[1024], Line[1024];
    char  style[256];

    while( GetLine( File, Line, LineNum ) != NULL )
    {
        if( strnicmp( Line, "$EndTEXTPCB", 11 ) == 0 )
            return 0;
        if( strncmp( Line, "Te", 2 ) == 0 ) /* Texte */
        {
            ReadDelimitedText( text, Line + 2, sizeof(text) );
            m_Text = CONV_FROM_UTF8( text );
            continue;
        }
        if( strncmp( Line, "Po", 2 ) == 0 )
        {
            sscanf( Line + 2, " %d %d %d %d %d %d",
                    &m_Pos.x, &m_Pos.y, &m_Size.x, &m_Size.y,
                    &m_Width, &m_Orient );
            // Ensure the text has minimal size to see this text on screen:
            if ( m_Size.x < 5 ) m_Size.x = 5;
            if ( m_Size.y < 5 ) m_Size.y = 5;
            continue;
        }
        if( strncmp( Line, "De", 2 ) == 0 )
        {
			style[0] = 0;
			int normal_display = 1;
            sscanf( Line + 2, " %d %d %lX %s\n", &m_Layer, &normal_display,
                    &m_TimeStamp, style );

			m_Mirror = normal_display ? false : true;

            if( m_Layer < FIRST_COPPER_LAYER )
                m_Layer = FIRST_COPPER_LAYER;
            if( m_Layer > LAST_NO_COPPER_LAYER )
                m_Layer = LAST_NO_COPPER_LAYER;

			if ( strnicmp( style, "Italic", 6) == 0 )
				m_Italic = 1;
			else
				m_Italic = 0;
            continue;
        }
    }

    return 1;
}


/*****************************************/
bool TEXTE_PCB::Save( FILE* aFile ) const
/*****************************************/
{
    if( GetState( DELETED ) )
        return true;

    if( m_Text.IsEmpty() )
        return true;

    bool rc = false;
	const char * style = m_Italic ? "Italic" : "Normal";

    if( fprintf( aFile, "$TEXTPCB\n" ) != sizeof("$TEXTPCB\n")-1 )
        goto out;

    fprintf( aFile, "Te \"%s\"\n", CONV_TO_UTF8( m_Text ) );
    fprintf( aFile, "Po %d %d %d %d %d %d\n",
             m_Pos.x, m_Pos.y, m_Size.x, m_Size.y, m_Width, m_Orient );
    fprintf( aFile, "De %d %d %lX %s\n", m_Layer,
		m_Mirror ? 0 : 1,
		m_TimeStamp, style );

    if( fprintf( aFile, "$EndTEXTPCB\n" ) != sizeof("$EndTEXTPCB\n")-1 )
        goto out;

    rc = true;
out:
    return rc;
}




/**********************************************************************/
void TEXTE_PCB::Draw( WinEDA_DrawPanel* panel, wxDC* DC,
                      int DrawMode, const wxPoint& offset )
/**********************************************************************/

/*
 *  DrawMode = GR_OR, GR_XOR.., -1 si mode courant.
 */
{
    int color = g_DesignSettings.m_LayerColor[m_Layer];

    if( color & ITEM_NOT_SHOW )
        return;

    EDA_TextStruct::Draw( panel, DC, offset, (EDA_Colors) color,
                         DrawMode, (GRFillMode)DisplayOpt.DisplayDrawItems,
                         (g_AnchorColor & ITEM_NOT_SHOW) ? UNSPECIFIED_COLOR : (EDA_Colors)g_AnchorColor );
}



// see class_pcb_text.h
void TEXTE_PCB::Display_Infos( WinEDA_DrawFrame* frame )
{
    wxString msg;

    BOARD*      board;
    BOARD_ITEM* parent = (BOARD_ITEM*) m_Parent;
    wxASSERT( parent );

    if( parent->Type() == TYPE_COTATION )
        board = (BOARD*) parent->GetParent();
    else
        board = (BOARD*) parent;
    wxASSERT( board );

    frame->MsgPanel->EraseMsgBox();

    if( m_Parent && m_Parent->Type() == TYPE_COTATION )
        Affiche_1_Parametre( frame, 1, _( "COTATION" ), m_Text, DARKGREEN );
    else
        Affiche_1_Parametre( frame, 1, _( "PCB Text" ), m_Text, DARKGREEN );

    Affiche_1_Parametre( frame, 28, _( "Layer" ),
                         board->GetLayerName( m_Layer ),
                         g_DesignSettings.m_LayerColor[m_Layer] & MASKCOLOR );

    Affiche_1_Parametre( frame, 36, _( "Mirror" ), wxEmptyString, GREEN );
    if( ! m_Mirror )
        Affiche_1_Parametre( frame, -1, wxEmptyString, _( "No" ), DARKGREEN );
    else
        Affiche_1_Parametre( frame, -1, wxEmptyString, _( "Yes" ), DARKGREEN );

    msg.Printf( wxT( "%.1f" ), (float) m_Orient / 10 );
    Affiche_1_Parametre( frame, 43, _( "Orient" ), msg, DARKGREEN );

    valeur_param( m_Width, msg );
    Affiche_1_Parametre( frame, 50, _( "Width" ), msg, MAGENTA );

    valeur_param( m_Size.x, msg );
    Affiche_1_Parametre( frame, 60, _( "H Size" ), msg, RED );

    valeur_param( m_Size.y, msg );
    Affiche_1_Parametre( frame, 70, _( "V Size" ), msg, RED );
}


#if defined(DEBUG)

/**
 * Function Show
 * is used to output the object tree, currently for debugging only.
 * @param nestLevel An aid to prettier tree indenting, and is the level
 *          of nesting of this object within the overall tree.
 * @param os The ostream& to output to.
 */
void TEXTE_PCB::Show( int nestLevel, std::ostream& os )
{
    // for now, make it look like XML:
    NestedSpace( nestLevel, os ) << '<' << GetClass().Lower().mb_str() <<
        " string=\"" << m_Text.mb_str() << "\"/>\n";

//    NestedSpace( nestLevel, os ) << "</" << GetClass().Lower().mb_str() << ">\n";
}

#endif
