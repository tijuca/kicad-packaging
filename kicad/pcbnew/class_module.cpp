/****************************************************/
/* class_module.cpp : fonctions de la classe MODULE */
/****************************************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "wxstruct.h"
#include "common.h"
#include "plot_common.h"
#include "pcbnew.h"
#include "trigo.h"

#ifdef PCBNEW
#include "autorout.h"
#include "drag.h"
#endif

#ifdef CVPCB
#include "cvpcb.h"
#endif

#include "3d_struct.h"
#include "protos.h"

/*********************************************************************************/
void MODULE::DrawAncre( WinEDA_DrawPanel* panel, wxDC* DC, const wxPoint& offset,
                        int dim_ancre, int draw_mode )
/*********************************************************************************/

/* trace de l'ancre (croix verticale)
 *  (doit etre fait apres les pads,
 *  car le trace du trou efface tout donc peut etre l'ancre */
{
    int zoom = panel->GetZoom();
    int anchor_size = dim_ancre * zoom;

    GRSetDrawMode( DC, draw_mode );

    if( (g_AnchorColor & ITEM_NOT_SHOW) == 0 )
    {
        GRLine( &panel->m_ClipBox, DC,
            m_Pos.x - offset.x - anchor_size, m_Pos.y - offset.y,
            m_Pos.x - offset.x + anchor_size, m_Pos.y - offset.y,
            0, g_AnchorColor );
        GRLine( &panel->m_ClipBox, DC,
            m_Pos.x - offset.x, m_Pos.y - offset.y - anchor_size,
            m_Pos.x - offset.x, m_Pos.y - offset.y + anchor_size,
            0, g_AnchorColor );
    }
}


/*************************************************/
/* Class MODULE : description d'un composant pcb */
/*************************************************/

/* Constructeur de la classe MODULE */
MODULE::MODULE( BOARD* parent ) :
    BOARD_ITEM( parent, TYPEMODULE )
{
    m_Pads         = NULL;
    m_Drawings     = NULL;
    m_3D_Drawings  = NULL;
    m_Attributs    = MOD_DEFAULT;
    m_Layer        = CMP_N;
    m_Orient       = 0;
    m_ModuleStatus = 0;
    flag = 0;
    m_CntRot90 = m_CntRot180 = 0;
    m_Surface  = 0;
    m_Link = 0;
    m_LastEdit_Time = time( NULL );
    m_Reference = new TEXTE_MODULE( this, TEXT_is_REFERENCE );
    m_Reference->Pback = this;
    m_Value = new TEXTE_MODULE( this, TEXT_is_VALUE );
    m_Value->Pback = this;
    m_3D_Drawings  = new Struct3D_Master( this );
}


/* Destructeur */
MODULE::~MODULE()
{
    D_PAD*          Pad;
    EDA_BaseStruct* Struct, * NextStruct;

    delete m_Reference;
    delete m_Value;
    for( Struct = m_3D_Drawings;  Struct != NULL;  Struct = NextStruct )
    {
        NextStruct = Struct->Pnext;
        delete Struct;
    }

    /* effacement des pads */
    for( Pad = m_Pads; Pad != NULL; Pad = (D_PAD*) NextStruct )
    {
        NextStruct = Pad->Pnext;
        delete Pad;
    }

    /* effacement des elements de trace */
    for( Struct = m_Drawings; Struct != NULL; Struct = NextStruct )
    {
        NextStruct = Struct->Pnext;

        switch( ( Struct->Type() ) )
        {
        case TYPEEDGEMODULE:
            delete (EDGE_MODULE*) Struct;
            break;

        case TYPETEXTEMODULE:
            delete (TEXTE_MODULE*) Struct;
            break;

        default:
            DisplayError( NULL, wxT( "Warn: ItemType not handled in delete MODULE" ) );
            NextStruct = NULL;
            break;
        }
    }
}


/*********************************/
void MODULE::Copy( MODULE* Module )
/*********************************/
{
    D_PAD* pad, * lastpad;

    m_Pos           = Module->m_Pos;
    m_Layer         = Module->m_Layer;
    m_LibRef        = Module->m_LibRef;
    m_Attributs     = Module->m_Attributs;
    m_Orient        = Module->m_Orient;
    m_BoundaryBox   = Module->m_BoundaryBox;
    m_PadNum        = Module->m_PadNum;
    m_CntRot90      = Module->m_CntRot90;
    m_CntRot180     = Module->m_CntRot180;
    m_LastEdit_Time = Module->m_LastEdit_Time;
    m_Path          = Module->m_Path; //is this correct behavior?
    m_TimeStamp     = GetTimeStamp();

    /* Copy des structures auxiliaires: Reference et value */
    m_Reference->Copy( Module->m_Reference );
    m_Value->Copy( Module->m_Value );

    /* Copie des structures auxiliaires: Pads */
    lastpad = NULL; pad = Module->m_Pads;
    for( ; pad != NULL; pad = (D_PAD*) pad->Pnext )
    {
        D_PAD* newpad = new D_PAD( this );
        newpad->Copy( pad );

        if( m_Pads == NULL )
        {
            newpad->Pback = this;
            m_Pads = (D_PAD*) newpad;
        }
        else
        {
            newpad->Pback  = lastpad;
            lastpad->Pnext = newpad;
        }
        lastpad = newpad;
    }

    /* Copy des structures auxiliaires: Drawings */
    BOARD_ITEM* OldStruct = Module->m_Drawings;
    BOARD_ITEM* NewStruct, * LastStruct = NULL;
    for( ; OldStruct; OldStruct = OldStruct->Next() )
    {
        NewStruct = NULL;

        switch( OldStruct->Type() )
        {
        case TYPETEXTEMODULE:
            NewStruct = new TEXTE_MODULE( this );
            ( (TEXTE_MODULE*) NewStruct )->Copy( (TEXTE_MODULE*) OldStruct );
            break;

        case TYPEEDGEMODULE:
            NewStruct = new EDGE_MODULE( this );
            ( (EDGE_MODULE*) NewStruct )->Copy( (EDGE_MODULE*) OldStruct );
            break;

        default:
            DisplayError( NULL, wxT( "Internal Err: CopyModule: type indefini" ) );
            break;
        }

        if( NewStruct == NULL )
            break;
        if( m_Drawings == NULL )
        {
            NewStruct->Pback = this;
            m_Drawings = NewStruct;
        }
        else
        {
            NewStruct->Pback  = LastStruct;
            LastStruct->Pnext = NewStruct;
        }
        LastStruct = NewStruct;
    }

    /* Copy des elements complementaires Drawings 3D */
    m_3D_Drawings->Copy( Module->m_3D_Drawings );

    Struct3D_Master* Struct3D, * NewStruct3D, * CurrStruct3D;

    Struct3D     = (Struct3D_Master*) Module->m_3D_Drawings->Pnext;
    CurrStruct3D = m_3D_Drawings;
    for( ; Struct3D != NULL; Struct3D = (Struct3D_Master*) Struct3D->Pnext )
    {
        NewStruct3D = new Struct3D_Master( this );
        NewStruct3D->Copy( Struct3D );
        CurrStruct3D->Pnext = NewStruct3D;
        NewStruct3D->Pback  = CurrStruct3D;
        CurrStruct3D = NewStruct3D;
    }

    /* Copie des elements complementaires */
    m_Doc     = Module->m_Doc;
    m_KeyWord = Module->m_KeyWord;
}


/* supprime du chainage la structure Struct
 *  les structures arrieres et avant sont chainees directement
 */
void MODULE::UnLink()
{
    /* Modification du chainage arriere */
    if( Pback )
    {
        if( Pback->Type() != TYPEPCB )
        {
            Pback->Pnext = Pnext;
        }
        else                                /* Le chainage arriere pointe sur la structure "Pere" */
        {
            if( GetState( DELETED ) )       // A REVOIR car Pback = NULL si place en undelete
            {
                if( g_UnDeleteStackPtr )
                    g_UnDeleteStack[g_UnDeleteStackPtr - 1] = Next();
            }
            else
                ( (BOARD*) Pback )->m_Modules = (MODULE*) Pnext;
        }
    }

    /* Modification du chainage avant */
    if( Pnext )
        Pnext->Pback = Pback;

    Pnext = Pback = NULL;
}


/**********************************************************/
void MODULE::Draw( WinEDA_DrawPanel* panel, wxDC* DC,
                   int draw_mode, const wxPoint& offset )
/**********************************************************/

/** Function Draw
 *  Draws the footprint to the current Device Context
 *  @param panel = The active Draw Panel (used to know the clip box)
 *  @param DC = current Device Context
 *  @param offset = draw offset (usually wxPoint(0,0)
 *  @param draw_mode =  GR_OR, GR_XOR, GR_AND
 */
{
    if( (m_Flags & DO_NOT_DRAW) )
        return;

    /* Draw pads */
    D_PAD*  pt_pad = m_Pads;
    for( ; pt_pad != NULL; pt_pad = (D_PAD*) pt_pad->Pnext )
    {
        if( pt_pad->m_Flags & IS_MOVED )
            continue;
        pt_pad->Draw( panel, DC, draw_mode, offset );
    }

    // Draws foootprint anchor
    DrawAncre( panel, DC, offset, DIM_ANCRE_MODULE, draw_mode );

    /* Draw graphic items */
    if( !(m_Reference->m_Flags & IS_MOVED) )
        m_Reference->Draw( panel, DC, draw_mode, offset );

    if( !(m_Value->m_Flags & IS_MOVED) )
        m_Value->Draw( panel, DC, draw_mode, offset );

    for( BOARD_ITEM* item = m_Drawings;  item;  item = item->Next() )
    {
        if( item->m_Flags & IS_MOVED )
            continue;

        switch( item->Type() )
        {
        case TYPETEXTEMODULE:
        case TYPEEDGEMODULE:
            item->Draw( panel, DC, draw_mode, offset );
            break;

        default:
            break;
        }
    }
}


/**************************************************************/
void MODULE::DrawEdgesOnly( WinEDA_DrawPanel* panel, wxDC* DC,
                            const wxPoint& offset, int draw_mode )
/**************************************************************/

/** Function DrawEdgesOnly
 *  Draws the footprint edges only to the current Device Context
 *  @param panel = The active Draw Panel (used to know the clip box)
 *  @param DC = current Device Context
 *  @param offset = draw offset (usually wxPoint(0,0)
 *  @param draw_mode =  GR_OR, GR_XOR, GR_AND
 */
{
    for( BOARD_ITEM* item = m_Drawings;  item;  item = item->Next() )
    {
        switch( item->Type() )
        {
        case TYPEEDGEMODULE:
            item->Draw( panel, DC, draw_mode, offset );
            break;

        default:
            break;
        }
    }
}


/**************************************/
bool MODULE::Save( FILE* aFile ) const
/**************************************/
{
    char        statusTxt[8];
    BOARD_ITEM* item;

    if( GetState( DELETED ) )
        return true;

    bool rc = false;

    fprintf( aFile, "$MODULE %s\n", CONV_TO_UTF8( m_LibRef ) );

    // Generation des coord et caracteristiques
    memset( statusTxt, 0, sizeof(statusTxt) );
    if( IsLocked() )
        statusTxt[0] = 'F';
    else
        statusTxt[0] = '~';

    if( m_ModuleStatus & MODULE_is_PLACED )
        statusTxt[1] = 'P';
    else
        statusTxt[1] = '~';

    fprintf( aFile, "Po %d %d %d %d %8.8lX %8.8lX %s\n",
        m_Pos.x, m_Pos.y,
        m_Orient, m_Layer, m_LastEdit_Time,
        m_TimeStamp, statusTxt );

    fprintf( aFile, "Li %s\n", CONV_TO_UTF8( m_LibRef ) );

    if( !m_Doc.IsEmpty() )
    {
        fprintf( aFile, "Cd %s\n", CONV_TO_UTF8( m_Doc ) );
    }

    if( !m_KeyWord.IsEmpty() )
    {
        fprintf( aFile, "Kw %s\n", CONV_TO_UTF8( m_KeyWord ) );
    }

    fprintf( aFile, "Sc %8.8lX\n", m_TimeStamp );
    fprintf( aFile, "AR %s\n", CONV_TO_UTF8( m_Path ) );
    fprintf( aFile, "Op %X %X 0\n", m_CntRot90, m_CntRot180 );

    // attributes
    if( m_Attributs != MOD_DEFAULT )
    {
        fprintf( aFile, "At " );
        if( m_Attributs & MOD_CMS )
            fprintf( aFile, "SMD " );
        if( m_Attributs & MOD_VIRTUAL )
            fprintf( aFile, "VIRTUAL " );
        fprintf( aFile, "\n" );
    }

    // save reference
    if( !m_Reference->Save( aFile ) )
        goto out;

    // save value
    if( !m_Value->Save( aFile ) )
        goto out;

    // save drawing elements
    for( item = m_Drawings;  item;  item = item->Next() )
    {
        switch( item->Type() )
        {
        case TYPETEXTEMODULE:
        case TYPEEDGEMODULE:
            if( !item->Save( aFile ) )
                goto out;
            break;

        default:
#if defined (DEBUG)
            printf( "MODULE::Save() ignoring type %d\n", item->Type() );
#endif
            break;
        }
    }

    // save the pads
    for( item = m_Pads;  item;  item = item->Next() )
        if( !item->Save( aFile ) )
            goto out;

    // Generation des informations de trac�3D
    Write_3D_Descr( aFile );

    fprintf( aFile, "$EndMODULE  %s\n", CONV_TO_UTF8( m_LibRef ) );

    rc = true;
out:
    return rc;
}


/***************************************/
int MODULE::Write_3D_Descr( FILE* File ) const
/***************************************/

/* Sauvegarde de la description 3D du MODULE
 */
{
    char             buf[512];
    Struct3D_Master* Struct3D = m_3D_Drawings;

    for( ; Struct3D != NULL; Struct3D = (Struct3D_Master*) Struct3D->Pnext )
    {
        if( !Struct3D->m_Shape3DName.IsEmpty() )
        {
            fprintf( File, "$SHAPE3D\n" );

            fprintf( File, "Na \"%s\"\n", CONV_TO_UTF8( Struct3D->m_Shape3DName ) );

            sprintf( buf, "Sc %lf %lf %lf\n",
                Struct3D->m_MatScale.x,
                Struct3D->m_MatScale.y,
                Struct3D->m_MatScale.z );
            fprintf( File, to_point( buf ) );

            sprintf( buf, "Of %lf %lf %lf\n",
                Struct3D->m_MatPosition.x,
                Struct3D->m_MatPosition.y,
                Struct3D->m_MatPosition.z );
            fprintf( File, to_point( buf ) );

            sprintf( buf, "Ro %lf %lf %lf\n",
                Struct3D->m_MatRotation.x,
                Struct3D->m_MatRotation.y,
                Struct3D->m_MatRotation.z );
            fprintf( File, to_point( buf ) );

            fprintf( File, "$EndSHAPE3D\n" );
        }
    }

    return 0;
}


/****************************************************/
int MODULE::Read_3D_Descr( FILE* File, int* LineNum )
/****************************************************/

/* Lecture de la description d'un MODULE (format Ascii)
 *  la 1ere ligne de descr ($MODULE) est supposee etre deja lue
 *  retourne 0 si OK
 */
{
    char             Line[1024];
    char*            text     = Line + 3;
    Struct3D_Master* Struct3D = m_3D_Drawings;

    if( !Struct3D->m_Shape3DName.IsEmpty() )
    {
        Struct3D_Master* NewStruct3D;
        while( Struct3D->Pnext )
            Struct3D = (Struct3D_Master*) Struct3D->Pnext;

        Struct3D->Pnext    = NewStruct3D = new Struct3D_Master( this );
        NewStruct3D->Pback = Struct3D;
        Struct3D = NewStruct3D;
    }

    while( GetLine( File, Line, LineNum, sizeof(Line) - 1 ) != NULL )
    {
        switch( Line[0] )
        {
        case '$':       // Fin de description
            if( Line[1] == 'E' )
                return 0;
            return 1;

        case 'N':       // Shape File Name
        {
            char buf[512];
            ReadDelimitedText( buf, text, 512 );
            Struct3D->m_Shape3DName = CONV_FROM_UTF8( buf );
            break;
        }

        case 'S':       // Scale
            sscanf( text, "%lf %lf %lf\n",
                &Struct3D->m_MatScale.x,
                &Struct3D->m_MatScale.y,
                &Struct3D->m_MatScale.z );
            break;

        case 'O':       // Offset
            sscanf( text, "%lf %lf %lf\n",
                &Struct3D->m_MatPosition.x,
                &Struct3D->m_MatPosition.y,
                &Struct3D->m_MatPosition.z );
            break;

        case 'R':       // Rotation
            sscanf( text, "%lf %lf %lf\n",
                &Struct3D->m_MatRotation.x,
                &Struct3D->m_MatRotation.y,
                &Struct3D->m_MatRotation.z );
            break;

        default:
            break;
        }
    }

    return 1;
}


/**************************************************/
int MODULE::ReadDescr( FILE* File, int* LineNum )
/**************************************************/

/* Lecture de la description d'un MODULE (format Ascii)
 *  la 1ere ligne de descr ($MODULE) est supposee etre deja lue
 *  retourne 0 si OK
 */
{
    D_PAD*          LastPad = NULL, * ptpad;
    EDA_BaseStruct* LastModStruct = NULL;
    EDGE_MODULE*    DrawSegm;
    TEXTE_MODULE*   DrawText;
    char            Line[256], BufLine[256], BufCar1[128], * PtLine;
    int             itmp1, itmp2;

    while( GetLine( File, Line, LineNum, sizeof(Line) - 1 ) != NULL )
    {
        if( Line[0] == '$' )
        {
            if( Line[1] == 'E' )
                break;
            if( Line[1] == 'P' )
            {
                ptpad = new D_PAD( this );
                ptpad->ReadDescr( File, LineNum );
                RotatePoint( &ptpad->m_Pos.x, &ptpad->m_Pos.y, m_Orient );
                ptpad->m_Pos.x += m_Pos.x;
                ptpad->m_Pos.y += m_Pos.y;

                if( LastPad == NULL )
                {
                    ptpad->Pback = (EDA_BaseStruct*) this;
                    m_Pads = ptpad;
                }
                else
                {
                    ptpad->Pback   = (EDA_BaseStruct*) LastPad;
                    LastPad->Pnext = (EDA_BaseStruct*) ptpad;
                }
                LastPad = ptpad;
                continue;
            }
            if( Line[1] == 'S' )
                Read_3D_Descr( File, LineNum );
        }

        if( strlen( Line ) < 4 )
            continue;

        PtLine = Line + 3;

        /* Pointe 1er code utile de la ligne */
        switch( Line[0] )
        {
        case 'P':
            memset( BufCar1, 0, sizeof(BufCar1) );
            sscanf( PtLine, "%d %d %d %d %lX %lX %s",
                &m_Pos.x, &m_Pos.y,
                &m_Orient, &m_Layer,
                &m_LastEdit_Time, &m_TimeStamp, BufCar1 );

            m_ModuleStatus = 0;
            if( BufCar1[0] == 'F' )
                SetLocked( true );
            if( BufCar1[1] == 'P' )
                m_ModuleStatus |= MODULE_is_PLACED;
            break;

        case 'L':       /* Li = Lecture du nom librairie du module */
            *BufLine = 0;
            sscanf( PtLine, " %s", BufLine );
            m_LibRef = CONV_FROM_UTF8( BufLine );
            break;

        case 'S':
            sscanf( PtLine, " %lX", &m_TimeStamp );
            break;


        case 'O':       /* (Op)tions de placement auto */
            itmp1 = itmp2 = 0;
            sscanf( PtLine, " %X %X", &itmp1, &itmp2 );

            m_CntRot180 = itmp2 & 0x0F;
            if( m_CntRot180 > 10 )
                m_CntRot180 = 10;

            m_CntRot90 = itmp1 & 0x0F;
            if( m_CntRot90 > 10 )
                m_CntRot90 = 0;
            itmp1 = (itmp1 >> 4) & 0x0F;
            if( itmp1 > 10 )
                itmp1 = 0;
            m_CntRot90 |= itmp1 << 4;
            break;

        case 'A':
            if( Line[1] == 't' )
            {
                /* At = (At)tributs du module */
                if( strstr( PtLine, "SMD" ) )
                    m_Attributs |= MOD_CMS;
                if( strstr( PtLine, "VIRTUAL" ) )
                    m_Attributs |= MOD_VIRTUAL;
            }
            if( Line[1] == 'R' )
            {
                //alternate reference, e.g. /478C2408/478AD1B6
                sscanf( PtLine, " %s", BufLine );
                m_Path = CONV_FROM_UTF8( BufLine );
            }
            break;

        case 'T':    /* Read a footprint text description (ref, value, or drawing */
            sscanf( Line + 1, "%d", &itmp1 );
            if( itmp1 == TEXT_is_REFERENCE )
                DrawText = m_Reference;
            else if( itmp1 == TEXT_is_VALUE )
                DrawText = m_Value;
            else        /* text is a drawing */
            {
                DrawText = new TEXTE_MODULE( this );
                if( LastModStruct == NULL )
                {
                    DrawText->Pback = this;
                    m_Drawings = DrawText;
                }
                else
                {
                    DrawText->Pback      = LastModStruct;
                    LastModStruct->Pnext = DrawText;
                }
                LastModStruct = DrawText;
            }

            DrawText->ReadDescr( Line, File, LineNum );
            break;

        case 'D':    /* lecture du contour */
            DrawSegm = new EDGE_MODULE( this );

            if( LastModStruct == NULL )
            {
                DrawSegm->Pback = this;
                m_Drawings = DrawSegm;
            }
            else
            {
                DrawSegm->Pback      = LastModStruct;
                LastModStruct->Pnext = DrawSegm;
            }

            LastModStruct = DrawSegm;
            DrawSegm->ReadDescr( Line, File, LineNum );
            DrawSegm->SetDrawCoord();
            break;

        case 'C':    /* Lecture de la doc */
            m_Doc = CONV_FROM_UTF8( StrPurge( PtLine ) );
            break;

        case 'K':    /* Lecture de la liste des mots cle */
            m_KeyWord = CONV_FROM_UTF8( StrPurge( PtLine ) );
            break;

        default:
            break;
        }
    }

    /* Recalculate the bounding box */
    Set_Rectangle_Encadrement();
    return 0;
}


/*************************************************/
void MODULE::SetPosition( const wxPoint& newpos )
/*************************************************/

// replace le module en position newpos
{
    int deltaX = newpos.x - m_Pos.x;
    int deltaY = newpos.y - m_Pos.y;

    /* deplacement de l'ancre */
    m_Pos.x += deltaX;
    m_Pos.y += deltaY;

    /* deplacement de la reference */
    m_Reference->m_Pos.x += deltaX;
    m_Reference->m_Pos.y += deltaY;

    /* deplacement de la Valeur */
    m_Value->m_Pos.x += deltaX;
    m_Value->m_Pos.y += deltaY;

    /* deplacement des pastilles */
    D_PAD*          pad = m_Pads;
    for( ; pad != NULL; pad = (D_PAD*) pad->Pnext )
    {
        pad->m_Pos.x += deltaX;
        pad->m_Pos.y += deltaY;
    }

    /* deplacement des dessins de l'empreinte : */
    EDA_BaseStruct* PtStruct = m_Drawings;
    for( ; PtStruct != NULL; PtStruct = PtStruct->Pnext )
    {
        switch( PtStruct->Type() )
        {
        case TYPEEDGEMODULE:
        {
            EDGE_MODULE* pt_edgmod = (EDGE_MODULE*) PtStruct;
            pt_edgmod->SetDrawCoord();
            break;
        }

        case TYPETEXTEMODULE:
        {
            TEXTE_MODULE* pt_texte = (TEXTE_MODULE*) PtStruct;
            pt_texte->m_Pos.x += deltaX;
            pt_texte->m_Pos.y += deltaY;
            break;
        }

        default:
            DisplayError( NULL, wxT( "Type Draw Indefini" ) ); break;
        }
    }

    Set_Rectangle_Encadrement();
}


/*********************************************/
void MODULE::SetOrientation( int newangle )
/*********************************************/

/* Tourne de newangle (en 0.1 degres) le module
 */
{
    int px, py;

    newangle -= m_Orient;       // = delta de rotation

    m_Orient += newangle;
    NORMALIZE_ANGLE_POS( m_Orient );

    /* deplacement et rotation des pastilles */
    D_PAD* pad = m_Pads;
    for( ; pad != NULL; pad = (D_PAD*) pad->Pnext )
    {
        px = pad->m_Pos0.x;
        py = pad->m_Pos0.y;

        pad->m_Orient += newangle; /* change m_Orientation */
        NORMALIZE_ANGLE_POS( pad->m_Orient );

        RotatePoint( &px, &py, (int) m_Orient );
        pad->m_Pos.x = m_Pos.x + px;
        pad->m_Pos.y = m_Pos.y + py;
    }

    /* mise a jour de la reference et de la valeur*/
    m_Reference->SetDrawCoord();
    m_Value->SetDrawCoord();

    /* deplacement des contours et textes de l'empreinte : */
    EDA_BaseStruct* PtStruct = m_Drawings;
    for( ; PtStruct != NULL; PtStruct = PtStruct->Pnext )
    {
        if( PtStruct->Type() == TYPEEDGEMODULE )
        {
            EDGE_MODULE* pt_edgmod = (EDGE_MODULE*) PtStruct;
            pt_edgmod->SetDrawCoord();
        }
        if( PtStruct->Type() == TYPETEXTEMODULE )
        {
            /* deplacement des inscriptions : */
            TEXTE_MODULE* pt_texte = (TEXTE_MODULE*) PtStruct;
            pt_texte->SetDrawCoord();
        }
    }

    /* Recalcul du rectangle d'encadrement */
    Set_Rectangle_Encadrement();
}


/************************************************/
void MODULE::Set_Rectangle_Encadrement()
/************************************************/

/* Mise a jour du rectangle d'encadrement du module
 *  Entree : pointeur sur module
 *  Le rectangle d'encadrement est le rectangle comprenant les contours et les
 *  pads.
 *  Le rectangle est calcule:
 *      pour orient 0
 *      en coord relatives / position ancre
 */
{
    EDGE_MODULE* pt_edge_mod;
    D_PAD*       pad;
    int          width;
    int          cx, cy, uxf, uyf, rayon;
    int          xmax, ymax;


    /* Init des pointeurs */
    pt_edge_mod = (EDGE_MODULE*) m_Drawings;

    /* Init des coord du cadre a une valeur limite non nulle */
    m_BoundaryBox.m_Pos.x = -500; xmax = 500;
    m_BoundaryBox.m_Pos.y = -500; ymax = 500;

    /* Contours: Recherche des coord min et max et mise a jour du cadre */
    for( ; pt_edge_mod != NULL; pt_edge_mod = (EDGE_MODULE*) pt_edge_mod->Pnext )
    {
        if( pt_edge_mod->Type() != TYPEEDGEMODULE )
            continue;
        width = pt_edge_mod->m_Width / 2;

        switch( pt_edge_mod->m_Shape )
        {
        case S_ARC:
        case S_CIRCLE:
        {
            cx     = pt_edge_mod->m_Start0.x; cy = pt_edge_mod->m_Start0.y;  // centre
            uxf    = pt_edge_mod->m_End0.x; uyf = pt_edge_mod->m_End0.y;
            rayon  = (int) hypot( (double) (cx - uxf), (double) (cy - uyf) );
            rayon += width;
            m_BoundaryBox.m_Pos.x = MIN( m_BoundaryBox.m_Pos.x, cx - rayon );
            m_BoundaryBox.m_Pos.y = MIN( m_BoundaryBox.m_Pos.y, cy - rayon );
            xmax = MAX( xmax, cx + rayon );
            ymax = MAX( ymax, cy + rayon );
            break;
        }

        default:
            m_BoundaryBox.m_Pos.x = MIN( m_BoundaryBox.m_Pos.x, pt_edge_mod->m_Start0.x - width );
            m_BoundaryBox.m_Pos.x = MIN( m_BoundaryBox.m_Pos.x, pt_edge_mod->m_End0.x - width );
            m_BoundaryBox.m_Pos.y = MIN( m_BoundaryBox.m_Pos.y, pt_edge_mod->m_Start0.y - width );
            m_BoundaryBox.m_Pos.y = MIN( m_BoundaryBox.m_Pos.y, pt_edge_mod->m_End0.y - width );
            xmax = MAX( xmax, pt_edge_mod->m_Start0.x + width );
            xmax = MAX( xmax, pt_edge_mod->m_End0.x + width );
            ymax = MAX( ymax, pt_edge_mod->m_Start0.y + width );
            ymax = MAX( ymax, pt_edge_mod->m_End0.y + width );
            break;
        }
    }

    /* Pads:  Recherche des coord min et max et mise a jour du cadre */
    for( pad = m_Pads; pad != NULL; pad = (D_PAD*) pad->Pnext )
    {
        rayon = pad->m_Rayon;
        cx    = pad->m_Pos0.x; cy = pad->m_Pos0.y;
        m_BoundaryBox.m_Pos.x = MIN( m_BoundaryBox.m_Pos.x, cx - rayon );
        m_BoundaryBox.m_Pos.y = MIN( m_BoundaryBox.m_Pos.y, cy - rayon );
        xmax = MAX( xmax, cx + rayon );
        ymax = MAX( ymax, cy + rayon );
    }

    m_BoundaryBox.SetWidth( xmax - m_BoundaryBox.m_Pos.x );
    m_BoundaryBox.SetHeight( ymax - m_BoundaryBox.m_Pos.y );
}


/****************************************/
void MODULE::SetRectangleExinscrit()
/****************************************/

/*	Analogue a MODULE::Set_Rectangle_Encadrement() mais en coord reelles:
 *  Mise a jour du rectangle d'encadrement reel du module c.a.d en coord PCB
 *  Entree : pointeur sur module
 *  Le rectangle d'encadrement est le rectangle comprenant les contours et les
 *  pads.
 *  Met egalement a jour la surface (.m_Surface) du module.
 */
{
    EDGE_MODULE* EdgeMod;
    D_PAD*       Pad;
    int          width;
    int          cx, cy, uxf, uyf, rayon;
    int          xmax, ymax;

    m_RealBoundaryBox.m_Pos.x = xmax = m_Pos.x;
    m_RealBoundaryBox.m_Pos.y = ymax = m_Pos.y;

    /* Contours: Recherche des coord min et max et mise a jour du cadre */
    EdgeMod = (EDGE_MODULE*) m_Drawings;
    for( ; EdgeMod != NULL; EdgeMod = (EDGE_MODULE*) EdgeMod->Pnext )
    {
        if( EdgeMod->Type() != TYPEEDGEMODULE )
            continue;
        width = EdgeMod->m_Width / 2;

        switch( EdgeMod->m_Shape )
        {
        case S_ARC:
        case S_CIRCLE:
        {
            cx     = EdgeMod->m_Start.x; cy = EdgeMod->m_Start.y;  // centre
            uxf    = EdgeMod->m_End.x; uyf = EdgeMod->m_End.y;
            rayon  = (int) hypot( (double) (cx - uxf), (double) (cy - uyf) );
            rayon += width;
            m_RealBoundaryBox.m_Pos.x = MIN( m_RealBoundaryBox.m_Pos.x, cx - rayon );
            m_RealBoundaryBox.m_Pos.y = MIN( m_RealBoundaryBox.m_Pos.y, cy - rayon );
            xmax = MAX( xmax, cx + rayon );
            ymax = MAX( ymax, cy + rayon );
            break;
        }

        default:
            m_RealBoundaryBox.m_Pos.x = MIN( m_RealBoundaryBox.m_Pos.x, EdgeMod->m_Start.x - width );
            m_RealBoundaryBox.m_Pos.x = MIN( m_RealBoundaryBox.m_Pos.x, EdgeMod->m_End.x - width );
            m_RealBoundaryBox.m_Pos.y = MIN( m_RealBoundaryBox.m_Pos.y, EdgeMod->m_Start.y - width );
            m_RealBoundaryBox.m_Pos.y = MIN( m_RealBoundaryBox.m_Pos.y, EdgeMod->m_End.y - width );
            xmax = MAX( xmax, EdgeMod->m_Start.x + width );
            xmax = MAX( xmax, EdgeMod->m_End.x + width );
            ymax = MAX( ymax, EdgeMod->m_Start.y + width );
            ymax = MAX( ymax, EdgeMod->m_End.y + width );
            break;
        }
    }

    /* Pads:  Recherche des coord min et max et mise a jour du cadre */
    for( Pad = m_Pads; Pad != NULL; Pad = (D_PAD*) Pad->Pnext )
    {
        rayon = Pad->m_Rayon;
        cx    = Pad->m_Pos.x; cy = Pad->m_Pos.y;
        m_RealBoundaryBox.m_Pos.x = MIN( m_RealBoundaryBox.m_Pos.x, cx - rayon );
        m_RealBoundaryBox.m_Pos.y = MIN( m_RealBoundaryBox.m_Pos.y, cy - rayon );
        xmax = MAX( xmax, cx + rayon );
        ymax = MAX( ymax, cy + rayon );
    }

    m_RealBoundaryBox.SetWidth( xmax - m_RealBoundaryBox.m_Pos.x );
    m_RealBoundaryBox.SetHeight( ymax - m_RealBoundaryBox.m_Pos.y );
    m_Surface = ABS( (float) m_RealBoundaryBox.GetWidth() * m_RealBoundaryBox.GetHeight() );
}


/**
 * Function GetBoundingBox
 * returns the full bounding box of this Footprint, including texts
 * Mainly used to redraw the screen area occuped by the footprint
 */
EDA_Rect MODULE::GetBoundingBox()
{
    // Calculate area without text fields:
    SetRectangleExinscrit();
    EDA_Rect      area = m_RealBoundaryBox;

    // Calculate extended area including text field:
    EDA_Rect      text_area;
    text_area = m_Reference->GetBoundingBox();
    area.Merge( text_area );

    text_area = m_Value->GetBoundingBox();
    area.Merge( text_area );

    EDGE_MODULE* EdgeMod = (EDGE_MODULE*) m_Drawings;
    for( ; EdgeMod != NULL; EdgeMod = (EDGE_MODULE*) EdgeMod->Pnext )
    {
        if( EdgeMod->Type() != TYPETEXTEMODULE )
            continue;
        text_area = ((TEXTE_MODULE*)EdgeMod)->GetBoundingBox();
        area.Merge( text_area );
    }

    // Add the Clearence shape size: (shape around the pads when the clearence is shown
    // Not optimized, but the draw cost is small (perhaps smaller than optimization)
    area.Inflate(g_DesignSettings.m_TrackClearence, g_DesignSettings.m_TrackClearence);

    return area;
}


/*******************************************************/
void MODULE::Display_Infos( WinEDA_DrawFrame* frame )
/*******************************************************/
{
    int      nbpad;
    char     bufcar[512], Line[512];
    int      pos;
    bool     flag = FALSE;
    wxString msg;
    BOARD*   board = (BOARD*) m_Parent;

    frame->MsgPanel->EraseMsgBox();    /* Effacement de la zone message */
    if( frame->m_Ident != PCB_FRAME )
        flag = TRUE;
    pos = 1;
    Affiche_1_Parametre( frame, pos, m_Reference->m_Text, m_Value->m_Text, DARKCYAN );

    /* Affiche signature temporelle ou date de modif (en edition de modules) */
    pos += 6;
    if( flag ) // Affichage date de modification (utile en Module Editor)
    {
        strcpy( Line, ctime( &m_LastEdit_Time ) );
        strtok( Line, " \n\r" );
        strcpy( bufcar, strtok( NULL, " \n\r" ) ); strcat( bufcar, " " );
        strcat( bufcar, strtok( NULL, " \n\r" ) ); strcat( bufcar, ", " );
        strtok( NULL, " \n\r" );
        strcat( bufcar, strtok( NULL, " \n\r" ) );
        msg = CONV_FROM_UTF8( bufcar );
        Affiche_1_Parametre( frame, pos, _( "Last Change" ), msg, BROWN );
        pos += 4;
    }
    else
    {
        msg.Printf( wxT( "%8.8lX" ), m_TimeStamp );
        Affiche_1_Parametre( frame, pos, _( "Netlist path" ), /*msg*/ m_Path, BROWN );
    }

    pos += 12;
    Affiche_1_Parametre( frame, pos, _( "Layer" ), board->GetLayerName( m_Layer ), RED );

    pos += 6;
    EDA_BaseStruct* PtStruct = m_Pads;
    nbpad = 0;
    while( PtStruct )
    {
        nbpad++;
        PtStruct = PtStruct->Pnext;
    }

    msg.Printf( wxT( "%d" ), nbpad );
    Affiche_1_Parametre( frame, pos, _( "Pads" ), msg, BLUE );

    pos += 4;
    msg  = wxT( ".." );
    if( IsLocked() )
        msg[0] = 'L';
    if( m_ModuleStatus & MODULE_is_PLACED )
        msg[1] = 'P';
    Affiche_1_Parametre( frame, pos, _( "Stat" ), msg, MAGENTA );

    pos += 4;
    msg.Printf( wxT( "%.1f" ), (float) m_Orient / 10 );
    Affiche_1_Parametre( frame, pos, _( "Orient" ), msg, BROWN );

    pos += 5;
    Affiche_1_Parametre( frame, pos, _( "Module" ), m_LibRef, BLUE );

    pos += 9;
    Affiche_1_Parametre( frame, pos, _( "3D-Shape" ),
        m_3D_Drawings->m_Shape3DName, RED );

    pos += 14;
    wxString doc     = _( "Doc:  " ) + m_Doc;
    wxString keyword = _( "KeyW: " ) + m_KeyWord;
    Affiche_1_Parametre( frame, pos, doc, keyword, BLACK );
}


/**
 * Function HitTest
 * tests if the given wxPoint is within the bounds of this object.
 * @param refPos A wxPoint to test
 * @return bool - true if a hit, else false
 */
bool MODULE::HitTest( const wxPoint& refPos )
{
    /* Calcul des coord souris dans le repere module */
    int spot_cX = refPos.x - m_Pos.x;
    int spot_cY = refPos.y - m_Pos.y;

    RotatePoint( &spot_cX, &spot_cY, -m_Orient );

    /* la souris est-elle dans ce rectangle : */
    if( m_BoundaryBox.Inside( spot_cX, spot_cY ) )
        return true;

    return false;
}


/**
 * Function HitTest (overlayed)
 * tests if the given EDA_Rect intersect the bounds of this object.
 * @param refArea : the given EDA_Rect
 * @return bool - true if a hit, else false
 */
bool MODULE::HitTest( EDA_Rect& refArea )
{
    bool is_out_of_box = false;

    SetRectangleExinscrit();

    if( m_RealBoundaryBox.m_Pos.x < refArea.GetX() )
        is_out_of_box = true;
    if( m_RealBoundaryBox.m_Pos.y < refArea.GetY() )
        is_out_of_box = true;
    if( m_RealBoundaryBox.GetRight() > refArea.GetRight() )
        is_out_of_box = true;
    if( m_RealBoundaryBox.GetBottom() > refArea.GetBottom() )
        is_out_of_box = true;

    return is_out_of_box ? false : true;
}


D_PAD* MODULE::FindPadByName( const wxString& aPadName ) const
{
    wxString buf;

    for( D_PAD* pad = m_Pads;  pad;  pad = pad->Next() )
    {
        pad->ReturnStringPadName( buf );
#if 1
        if( buf.CmpNoCase( aPadName ) == 0 )    // why case insensitive?
#else
        if( buf == aPadName )
#endif

            return pad;
    }

    return NULL;
}


// see class_module.h
SEARCH_RESULT MODULE::Visit( INSPECTOR* inspector, const void* testData,
                             const KICAD_T scanTypes[] )
{
    KICAD_T        stype;
    SEARCH_RESULT  result = SEARCH_CONTINUE;
    const KICAD_T* p    = scanTypes;
    bool           done = false;

#if 0 && defined (DEBUG)
    std::cout << GetClass().mb_str() << ' ';
#endif

    while( !done )
    {
        stype = *p;

        switch( stype )
        {
        case TYPEMODULE:
            result = inspector->Inspect( this, testData );  // inspect me
            ++p;
            break;

        case TYPEPAD:
            result = IterateForward( m_Pads, inspector, testData, p );
            ++p;
            break;

        case TYPETEXTEMODULE:
            result = inspector->Inspect( m_Reference, testData );
            if( result == SEARCH_QUIT )
                break;

            result = inspector->Inspect( m_Value, testData );
            if( result == SEARCH_QUIT )
                break;

            // m_Drawings can hold TYPETEXTMODULE also, so fall thru

        case TYPEEDGEMODULE:
            result = IterateForward( m_Drawings, inspector, testData, p );

            // skip over any types handled in the above call.
            for( ; ; )
            {
                switch( stype = *++p )
                {
                case TYPETEXTEMODULE:
                case TYPEEDGEMODULE:
                    continue;

                default:
                    ;
                }

                break;
            }

            break;

        default:
            done = true;
            break;
        }

        if( result == SEARCH_QUIT )
            break;
    }

    return result;
}


#if defined (DEBUG)

/**
 * Function Show
 * is used to output the object tree, currently for debugging only.
 * @param nestLevel An aid to prettier tree indenting, and is the level
 *          of nesting of this object within the overall tree.
 * @param os The ostream& to output to.
 */
void MODULE::Show( int nestLevel, std::ostream& os )
{
    BOARD* board = (BOARD*) m_Parent;

    // for now, make it look like XML, expand on this later.
    NestedSpace( nestLevel, os ) << '<' << GetClass().Lower().mb_str() <<
    " ref=\"" << m_Reference->m_Text.mb_str() << '"' <<
    " value=\"" << m_Value->m_Text.mb_str() << '"' <<
    " layer=\"" << board->GetLayerName( m_Layer ).mb_str() << '"' <<
    ">\n";

    NestedSpace( nestLevel + 1, os ) <<
    "<boundingBox" << m_BoundaryBox.m_Pos << m_BoundaryBox.m_Size << "/>\n";

    NestedSpace( nestLevel + 1, os ) << "<orientation tenths=\"" << m_Orient << "\"/>\n";

    EDA_BaseStruct* p;

    NestedSpace( nestLevel + 1, os ) << "<mpads>\n";
    p = m_Pads;
    for( ; p; p = p->Pnext )
        p->Show( nestLevel + 2, os );

    NestedSpace( nestLevel + 1, os ) << "</mpads>\n";

    NestedSpace( nestLevel + 1, os ) << "<mdrawings>\n";
    p = m_Drawings;
    for( ; p; p = p->Pnext )
        p->Show( nestLevel + 2, os );

    NestedSpace( nestLevel + 1, os ) << "</mdrawings>\n";

    p = m_Son;
    for( ; p;  p = p->Pnext )
    {
        p->Show( nestLevel + 1, os );
    }

    NestedSpace( nestLevel, os ) << "</" << GetClass().Lower().mb_str() << ">\n";
}


#endif
