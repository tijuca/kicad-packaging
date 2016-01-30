/********************************************************/
/**** Routine de lecture et visu d'un fichier GERBER ****/
/********************************************************/


#include "fctsys.h"
#include "common.h"
#include "confirm.h"
#include "gerbview.h"
#include "pcbplot.h"
#include "protos.h"

#define IsNumber( x ) ( ( ( (x) >= '0' ) && ( (x) <='9' ) )   \
                       || ( (x) == '-' ) || ( (x) == '+' )  || ( (x) == '.' ) )

/* Format Gerber : NOTES :
 *  Fonctions preparatoires:
 *  Gn =
 *  G01			interpolation lineaire ( trace de droites )
 *  G02,G20,G21	Interpolation circulaire , sens trigo < 0
 *  G03,G30,G31	Interpolation circulaire , sens trigo > 0
 *  G04			commentaire
 *  G06			Interpolation parabolique
 *  G07			Interpolation cubique
 *  G10			interpolation lineaire ( echelle 10x )
 *  G11			interpolation lineaire ( echelle 0.1x )
 *  G12			interpolation lineaire ( echelle 0.01x )
 *  G52			plot symbole reference par Dnn code
 *  G53			plot symbole reference par Dnn ; symbole tourne de -90 degres
 *  G54			Selection d'outil
 *  G55			Mode exposition photo
 *  G56			plot symbole reference par Dnn A code
 *  G57			affiche le symbole reference sur la console
 *  G58			plot et affiche le symbole reference sur la console
 *  G60			interpolation lineaire ( echelle 100x )
 *  G70			Unites = Inches
 *  G71			Unites = Millimetres
 *  G74			supprime interpolation circulaire sur 360 degre, revient a G01
 *  G75			Active interpolation circulaire sur 360 degre
 *  G90			Mode Coordonnees absolues
 *  G91			Mode Coordonnees Relatives
 *
 *  Coordonnees X,Y
 *  X,Y sont suivies de + ou - et de m+n chiffres (non separes)
 *          m = partie entiere
 *          n = partie apres la virgule
 *           formats classiques : 	m = 2, n = 3 (format 2.3)
 *                                  m = 3, n = 4 (format 3.4)
 *  ex:
 *  G__ X00345Y-06123 D__*
 *
 *  Outils et D_CODES
 *  numero d'outil ( identification des formes )
 *  1 a 99 	(classique)
 *  1 a 999
 *  D_CODES:
 *
 *  D01 ... D9 = codes d'action:
 *  D01			= activation de lumiere (baisser de plume) lors du d�placement
 *  D02			= extinction de lumiere (lever de plume) lors du d�placement
 *  D03			= Flash
 *  D09			= VAPE Flash
 *  D51			= precede par G54 -> Select VAPE
 *
 *  D10 ... D255 = Indentification d'outils ( d'ouvertures )
 *              Ne sont pas tj dans l'ordre ( voir tableau dans PCBPLOT.H)
 */

// Type d'action du phototraceur:
#define GERB_ACTIVE_DRAW 1      // activation de lumiere ( baisser de plume)
#define GERB_STOP_DRAW   2      // extinction de lumiere ( lever de plume)
#define GERB_FLASH       3      // Flash

/* Variables locales : */
static wxPoint LastPosition;


/* Local Functions (are lower case since they are private to this source file) */


/**
 * Function fillCircularTRACK
 * initializes a given TRACK so that it can draw a circle which is not filled and
 * has a given pen width (\a aPenWidth ).
 *
 * @param aTrack The TRACK to fill in.
 * @param Dcode_index The DCODE value, like D14
 * @param aLayer The layer index to set into the TRACK
 * @param aPos The center point of the flash
 * @param aDiameter The diameter of the round flash
 * @param aPenWidth The width of the pen used to draw the circle's circumfrance.
 * @param isDark True if flash is positive and should use a drawing
 *   color other than the background color, else use the background color
 *   when drawing so that an erasure happens.
 */
static void fillCircularTRACK(  TRACK* aTrack, int Dcode_index, int aLayer,
         const wxPoint& aPos, int aDiameter, int aPenWidth, bool isDark )
{
    aTrack->m_Shape = S_CIRCLE;
    aTrack->m_Width = aPenWidth;

    aTrack->SetLayer( aLayer );
    aTrack->SetNet( Dcode_index );

    // When drawing a TRACK with shape S_CIRCLE, the hypotenuse (i.e. distance)
    // between the Start and End points gives the radius of the circle.
    aTrack->m_Start = aTrack->m_End = aPos;
    aTrack->m_End.x += max(0, (aDiameter + 1)/2);

    NEGATE( aTrack->m_Start.y );
    NEGATE( aTrack->m_End.y );

    if( !isDark )
    {
        aTrack->m_Flags |= DRAW_ERASED;
    }
}


/**
 * Function fillRoundFlashTRACK
 * initializes a given TRACK so that it can draw a circle which is filled and
 * has no pen border.
 *
 * @param aTrack The TRACK to fill in.
 * @param Dcode_index The DCODE value, like D14
 * @param aLayer The layer index to set into the TRACK
 * @param aPos The center point of the flash
 * @param aDiameter The diameter of the round flash
 * @param isDark True if flash is positive and should use a drawing
 *   color other than the background color, else use the background color
 *   when drawing so that an erasure happens.
 */
static void fillRoundFlashTRACK(  TRACK* aTrack, int Dcode_index, int aLayer,
         const wxPoint& aPos, int aDiameter, bool isDark )
{
    aTrack->SetLayer( aLayer );
    aTrack->m_Width = aDiameter;
    aTrack->m_Start = aTrack->m_End = aPos;
    NEGATE( aTrack->m_Start.y );
    NEGATE( aTrack->m_End.y );
    aTrack->SetNet( Dcode_index );
    aTrack->m_Shape   = S_SPOT_CIRCLE;

    if( !isDark )
    {
        aTrack->m_Flags |= DRAW_ERASED;
    }
}


/**
 * Function fillOvalOrRectFlashTRACK
 * initializes a given TRACK so that it can draw an oval or rectangular filled rectangle.
 *
 * @param aTrack The TRACK to fill in.
 * @param Dcode_index The DCODE value, like D14
 * @param aLayer The layer index to set into the TRACK
 * @param aPos The center point of the rectangle
 * @param aSize The size of the flash
 * @param aShape What type of flash, S_SPOT_OVALE or S_SPOT_RECT
 * @param isDark True if flash is positive and should use a drawing
 *   color other than the background color, else use the background color
 *   when drawing so that an erasure happens.
 */
static void fillOvalOrRectFlashTRACK(  TRACK* aTrack, int Dcode_index, int aLayer,
         const wxPoint& aPos, const wxSize& aSize, int aShape, bool isDark )
{
    int width = MIN( aSize.x, aSize.y );
    int len   = MAX( aSize.x, aSize.y ) - width;

    aTrack->SetLayer( aLayer );

    aTrack->m_Width = width;

    aTrack->m_Start = aTrack->m_End = aPos;
    NEGATE( aTrack->m_Start.y );
    NEGATE( aTrack->m_End.y );

    aTrack->SetNet( Dcode_index );

    aTrack->m_Shape = aShape;

    len >>= 1;
    if( aSize.x > aSize.y )  // oval or rectangle is horizontal
    {
        aTrack->m_Start.x -= len;
        aTrack->m_End.x   += len;
    }
    else    // oval or rectangle is vertical
    {
        aTrack->m_Start.y -= len;
        aTrack->m_End.y   += len;
    }

    if( !isDark )
    {
        aTrack->m_Flags |= DRAW_ERASED;
    }
}


/**
 * Function fillLineTRACK
 * initializes a given TRACK so that it can draw a linear D code.
 *
 * @param aTrack The TRACK to fill in.
 * @param Dcode_index The DCODE value, like D14
 * @param aLayer The layer index to set into the TRACK
 * @param aPos The center point of the flash
 * @param aDiameter The diameter of the round flash
 * @param isDark True if flash is positive and should use a drawing
 *   color other than the background color, else use the background color
 *   when drawing so that an erasure happens.
 */
static void fillLineTRACK(  TRACK* aTrack, int Dcode_index, int aLayer,
         const wxPoint& aStart, const wxPoint& aEnd, int aWidth, bool isDark )
{
    aTrack->SetLayer( aLayer );

    aTrack->m_Width = aWidth;

    aTrack->m_Start = aStart;
    NEGATE( aTrack->m_Start.y );

    aTrack->m_End = aEnd;
    NEGATE( aTrack->m_End.y );

    aTrack->SetNet( Dcode_index );

    if( !isDark )
    {
        aTrack->m_Flags |= DRAW_ERASED;
    }
}


/**
 * Function fillArcTRACK
 * initializes a given TRACK so that it can draw an arc G code.
 * <p>
 * if multiquadrant == true : arc can be 0 to 360 degres
 *   and \a rel_center is the center coordiante relative to startpoint.
 * <p>
 * if multiquadrant == false arc can be only 0 to 90 deg,
 *     and only in the same quadrant :
 * <ul>
 * <li> absolute angle 0 to 90 (quadrant 1) or
 * <li> absolute angle 90 to 180 (quadrant 2) or
 * <li> absolute angle 180 to 270 (quadrant 3) or
 * <li> absolute angle 270 to 0 (quadrant 4)
 * </ul><p>
 * @param aTrack is the TRACK to fill in.
 * @param Dcode_index is the DCODE value, like D14
 * @param aLayer is the layer index to set into the TRACK
 * @param aStart is the starting point
 * @param aEnd is the ending point
 * @param rel_center is the center coordiante relative to startpoint,
 *   given in ABSOLUE VALUE and the signe of values x et y de rel_center
 *   must be calculated from the previously given constraint: arc only in the same quadrant.
 * @param aDiameter The diameter of the round flash
 * @param aWidth is the pen width.
 * @param isDark True if flash is positive and should use a drawing
 *   color other than the background color, else use the background color
 *   when drawing so that an erasure happens.
 */
static void fillArcTRACK(  TRACK* aTrack, int Dcode_index, int aLayer,
                         const wxPoint& aStart, const wxPoint& aEnd,
                         const wxPoint& rel_center, int aWidth,
                         bool trigo_sens, bool multiquadrant, bool isDark )
{
    wxPoint center, delta;

    aTrack->m_Shape = S_ARC;
    aTrack->SetLayer( aLayer );
    aTrack->m_Width = aWidth;

    if( multiquadrant )
    {
        center.x = aStart.x + rel_center.x;
        center.y = aStart.y - rel_center.y;

        if( !trigo_sens )
        {
            aTrack->m_Start = aStart;
            aTrack->m_End   = aEnd;
        }
        else
        {
            aTrack->m_Start = aEnd;
            aTrack->m_End   = aStart;
        }
    }
    else
    {
        center  = rel_center;
        delta.x = aEnd.x - aStart.x;
        delta.y = aEnd.y - aStart.y;

        // il faut corriger de signe de rel_center.x et rel_center.y
        // selon le quadrant ou on se trouve
        if( (delta.x >= 0) && (delta.y >= 0) ) // 1er quadrant
        {
            center.x = -center.x;
        }
        else if( (delta.x < 0) && (delta.y >= 0) ) // 2eme quadrant
        {
            center.x = -center.x;
            center.y = -center.y;
        }
        else if( (delta.x < 0) && (delta.y < 0) )  // 3eme quadrant
        {
            center.y = -center.y;
        }
        else    // 4eme qadrant: les 2 coord sont >= 0!
        {
        }

        center.x += aStart.x;
        center.y  = aStart.y + center.y;

        if(  trigo_sens )
        {
            aTrack->m_Start = aStart;
            aTrack->m_End   = aEnd;
        }
        else
        {
            aTrack->m_Start = aEnd;
            aTrack->m_End   = aStart;
        }
    }

    aTrack->SetNet( Dcode_index );
    aTrack->m_Param        = center.x;
    aTrack->SetSubNet( center.y );

    NEGATE( aTrack->m_Start.y );
    NEGATE( aTrack->m_End.y );

    aTrack->SetSubNet( -aTrack->GetSubNet() );

    if( !isDark )
    {
        aTrack->m_Flags |= DRAW_ERASED;
    }
}


/**************************************************/
/* Routines utilis�es en lecture de ficher gerber */
/**************************************************/

/* ces routines lisent la chaine de texte point�e par Text.
 *  Apres appel, Text pointe le debut de la sequence non lue
 */

/***********************************************/
wxPoint GERBER::ReadXYCoord( char*& Text )
/***********************************************/

/* Retourne la coord courante pointee par Text (XnnnnYmmmm)
 */
{
    wxPoint pos = m_CurrentPos;
    int     type_coord = 0, current_coord, nbchar;
    bool    is_float   = false;
    char*   text;
    char    line[256];


    if( m_Relative )
        pos.x = pos.y = 0;
    else
        pos = m_CurrentPos;

    if( Text == NULL )
        return pos;

    text = line;
    while( *Text )
    {
        if( (*Text == 'X') || (*Text == 'Y') )
        {
            type_coord = *Text;
            Text++;
            text = line;
            nbchar = 0;
            while( IsNumber( *Text ) )
            {
                if( *Text == '.' )
                    is_float = true;
                *(text++) = *(Text++);
                if( (*Text >= '0') && (*Text <='9') )
                    nbchar++;
            }

            *text = 0;
            if( is_float )
            {
                if( m_GerbMetric )
                    current_coord = (int) round( atof( line ) / 0.00254 );
                else
                    current_coord = (int) round( atof( line ) * PCB_INTERNAL_UNIT );
            }
            else
            {
                int    fmt_scale = (type_coord == 'X') ? m_FmtScale.x : m_FmtScale.y;
                if( m_NoTrailingZeros )
                {
                    int min_digit = (type_coord == 'X') ? m_FmtLen.x : m_FmtLen.y;
                    while( nbchar < min_digit )
                    {
                        *(text++) = '0';
                        nbchar++;
                    }

                    *text = 0;
                }
                current_coord = atoi( line );
                double real_scale = 1.0;

                switch( fmt_scale )
                {
                case 0:
                    real_scale = 10000.0;
                    break;

                case 1:
                    real_scale = 1000.0;
                    break;

                case 2:
                    real_scale = 100.0;
                    break;

                case 3:
                    real_scale = 10.0;
                    break;

                case 4:
                    break;

                case 5:
                    real_scale = 0.1;
                    break;

                case 6:
                    real_scale = 0.01;
                    break;

                case 7:
                    real_scale = 0.001;
                    break;

                case 8:
                    real_scale = 0.0001;
                    break;

                case 9:
                    real_scale = 0.00001;
                    break;
                }

                if( m_GerbMetric )
                    real_scale = real_scale / 25.4;

                current_coord = (int) round( current_coord * real_scale );
            }

            if( type_coord == 'X' )
                pos.x = current_coord;
            else if( type_coord == 'Y' )
                pos.y = current_coord;

            continue;
        }
        else
            break;
    }

    if( m_Relative )
    {
        pos.x += m_CurrentPos.x;
        pos.y += m_CurrentPos.y;
    }

    m_CurrentPos = pos;
    return pos;
}


/************************************************/
wxPoint GERBER::ReadIJCoord( char*& Text )
/************************************************/

/* Retourne la coord type InnJnn courante pointee par Text (InnnnJmmmm)
 *  Ces coordonn�es sont relatives, donc si une coord est absente, sa valeur
 *  par defaut est 0
 */
{
    wxPoint pos( 0, 0 );

    int   type_coord = 0, current_coord, nbchar;
    bool  is_float   = false;
    char* text;
    char  line[256];

    if( Text == NULL )
        return pos;

    text = line;
    while( *Text )
    {
        if( (*Text == 'I') || (*Text == 'J') )
        {
            type_coord = *Text;
            Text++;
            text = line;
            nbchar = 0;
            while( IsNumber( *Text ) )
            {
                if( *Text == '.' )
                    is_float = true;
                *(text++) = *(Text++);
                if( (*Text >= '0') && (*Text <='9') )
                    nbchar++;
            }

            *text = 0;
            if( is_float )
            {
                if( m_GerbMetric )
                    current_coord = (int) round( atof( line ) / 0.00254 );
                else
                    current_coord = (int) round( atof( line ) * PCB_INTERNAL_UNIT );
            }
            else
            {
                int    fmt_scale = (type_coord == 'I') ? m_FmtScale.x : m_FmtScale.y;
                if( m_NoTrailingZeros )
                {
                    int min_digit = (type_coord == 'I') ? m_FmtLen.x : m_FmtLen.y;
                    while( nbchar < min_digit )
                    {
                        *(text++) = '0';
                        nbchar++;
                    }

                    *text = 0;
                }
                current_coord = atoi( line );
                double real_scale = 1.0;

                switch( fmt_scale )
                {
                case 0:
                    real_scale = 10000.0;
                    break;

                case 1:
                    real_scale = 1000.0;
                    break;

                case 2:
                    real_scale = 100.0;
                    break;

                case 3:
                    real_scale = 10.0;
                    break;

                case 4:
                    break;

                case 5:
                    real_scale = 0.1;
                    break;

                case 6:
                    real_scale = 0.01;
                    break;

                case 7:
                    real_scale = 0.001;
                    break;

                case 8:
                    real_scale = 0.0001;
                    break;

                case 9:
                    real_scale = 0.00001;
                    break;
                }

                if( m_GerbMetric )
                    real_scale = real_scale / 25.4;
                current_coord = (int) round( current_coord * real_scale );
            }
            if( type_coord == 'I' )
                pos.x = current_coord;
            else if( type_coord == 'J' )
                pos.y = current_coord;
            continue;
        }
        else
            break;
    }

    m_IJPos = pos;
    return pos;
}


/*****************************************************/
int GERBER::ReturnGCodeNumber( char*& Text )
/*****************************************************/

/* Lit la sequence Gnn et retourne la valeur nn
 */
{
    int   ii = 0;
    char* text;
    char  line[1024];

    if( Text == NULL )
        return 0;
    Text++;
    text = line;
    while( IsNumber( *Text ) )
    {
        *(text++) = *(Text++);
    }

    *text = 0;
    ii    = atoi( line );
    return ii;
}


/**************************************************/
int GERBER::ReturnDCodeNumber( char*& Text )
/**************************************************/

/* Lit la sequence Dnn et retourne la valeur nn
 */
{
    int   ii = 0;
    char* text;
    char  line[1024];

    if( Text == NULL )
        return 0;

    Text++;
    text = line;
    while( IsNumber( *Text ) )
        *(text++) = *(Text++);

    *text = 0;
    ii    = atoi( line );
    return ii;
}


/******************************************************************/
bool GERBER::Execute_G_Command( char*& text, int G_commande )
/******************************************************************/
{
    D(printf( "%22s: G_CODE<%d>\n", __func__, G_commande );)

    switch( G_commande )
    {
    case GC_PHOTO_MODE:                 // can starts a D03 flash command: redundant, can be safely ignored
        break;

    case GC_LINEAR_INTERPOL_1X:
        m_Iterpolation = GERB_INTERPOL_LINEAR_1X;
        break;

    case GC_CIRCLE_NEG_INTERPOL:
        m_Iterpolation = GERB_INTERPOL_ARC_NEG;
        break;

    case GC_CIRCLE_POS_INTERPOL:
        m_Iterpolation = GERB_INTERPOL_ARC_POS;
        break;

    case GC_COMMENT:
        text = NULL;
        break;

    case GC_LINEAR_INTERPOL_10X:
        m_Iterpolation = GERB_INTERPOL_LINEAR_10X;
        break;

    case GC_LINEAR_INTERPOL_0P1X:
        m_Iterpolation = GERB_INTERPOL_LINEAR_01X;
        break;

    case GC_LINEAR_INTERPOL_0P01X:
        m_Iterpolation = GERB_INTERPOL_LINEAR_001X;
        break;

    case GC_SELECT_TOOL:
    {
        int D_commande = ReturnDCodeNumber( text );
        if( D_commande < FIRST_DCODE )
            return false;
        if( D_commande > (MAX_TOOLS - 1) )
            D_commande = MAX_TOOLS - 1;
        m_Current_Tool = D_commande;
        D_CODE* pt_Dcode = GetDCODE( D_commande, false );
        if( pt_Dcode )
            pt_Dcode->m_InUse = true;
        break;
    }

    case GC_SPECIFY_INCHES:
        m_GerbMetric = false;           // false = Inches, true = metric
        break;

    case GC_SPECIFY_MILLIMETERS:
        m_GerbMetric = true;            // false = Inches, true = metric
        break;

    case GC_TURN_OFF_360_INTERPOL:
        m_360Arc_enbl = false;
        break;

    case GC_TURN_ON_360_INTERPOL:
        m_360Arc_enbl = true;
        break;

    case GC_SPECIFY_ABSOLUES_COORD:
        m_Relative = false;         // false = absolute Coord, RUE = relative Coord
        break;

    case GC_SPECIFY_RELATIVEES_COORD:
        m_Relative = true;          // false = absolute Coord, RUE = relative Coord
        break;

    case GC_TURN_ON_POLY_FILL:
        m_PolygonFillMode = true;
        break;

    case GC_TURN_OFF_POLY_FILL:
        m_PolygonFillMode      = false;
        m_PolygonFillModeState = 0;
        break;

    case GC_MOVE:       // Non existant
    default:
    {
        wxString msg; msg.Printf( wxT( "G%.2d command not handled" ), G_commande );
        DisplayError( NULL, msg );
        return false;
    }
    }


    return true;
}


/**
 * Function scale
 * converts a distance given in floating point to our deci-mils
 */
static int scale( double aCoord, bool isMetric )
{
    int ret;

    if( isMetric )
        ret = (int) round( aCoord / 0.00254 );
    else
        ret = (int) round( aCoord * PCB_INTERNAL_UNIT );

    return ret;
}


/**
 * Function mapPt
 * translates a point from the aperture macro coordinate system to our
 * deci-mils coordinate system.
 * @return wxPoint - The gerbview coordinate system vector.
 */
static wxPoint mapPt( double x, double y, bool isMetric )
{
    wxPoint ret(  scale( x, isMetric ),
                  scale( y, isMetric ) );

    return ret;
}


/**
 * Function mapExposure
 * translates the first parameter from an aperture macro into a current exposure
 * setting.
 * @param curExposure A dynamic setting which can change throughout the reading of the
 *         gerber file, and it indicates whether the current tool is lit or not.
 * @param isNegative A dynamic setting which can change throughout the reading of
 *    the gerber file, and it indicates whether the current D codes are to
 *    be interpreted as erasures or not.
 */
static bool mapExposure( int param1, bool curExposure, bool isNegative )
{
    bool    exposure;

    switch( param1 )
    {
    case 0:
        exposure = false;
        break;
    default:
    case 1:
        exposure = true;
        break;
    case 2:
        exposure = !curExposure;
    }

    return exposure ^ isNegative;
}


/*****************************************************************************/
bool GERBER::Execute_DCODE_Command( WinEDA_GerberFrame* frame, wxDC* DC,
                                          char*& text, int D_commande )
/*****************************************************************************/
{
    wxSize      size( 15, 15 );

    APERTURE_T  aperture = APT_CIRCLE;
    TRACK*      track;
    BOARD*		pcb = frame->GetBoard();

    int         activeLayer = frame->GetScreen()->m_Active_Layer;

    int         dcode = 0;
    D_CODE*     tool = NULL;
    wxString    msg;

    D(printf( "%22s: D_CODE<%d>\n", __func__, D_commande );)

    if( D_commande >= FIRST_DCODE )  // This is a "Set tool" command
    {
        if( D_commande > (MAX_TOOLS - 1) )
            D_commande = MAX_TOOLS - 1;

        // remember which tool is selected, nothing is done with it in this call
        m_Current_Tool = D_commande;

        D_CODE* pt_Dcode = GetDCODE( D_commande, false );
        if( pt_Dcode )
            pt_Dcode->m_InUse = true;

        return true;
    }
    else // D_commande = 0..9:	this is a pen command (usualy D1, D2 or D3)
    {
        m_Last_Pen_Command = D_commande;
    }

    if( m_PolygonFillMode )    // Enter a polygon description:
    {
        switch( D_commande )
        {
        case 1:     // code D01 Draw line, exposure ON
            m_Exposure = true;

            SEGZONE* edge_poly;
            edge_poly = new SEGZONE( pcb );
            pcb->m_Zone.Append( edge_poly );
            D(printf("R:%p\n", edge_poly );)

            edge_poly->SetLayer( activeLayer );
            edge_poly->m_Width = 1;

            edge_poly->m_Start = m_PreviousPos;
            NEGATE( edge_poly->m_Start.y );

            edge_poly->m_End = m_CurrentPos;
            NEGATE( edge_poly->m_End.y );

            edge_poly->SetNet( m_PolygonFillModeState );

            // the first track of each polygon has a netcode of zero, otherwise one.
            // set the erasure flag in that special track, if a negative polygon.
            if( !m_PolygonFillModeState )
            {
                if( m_LayerNegative ^ m_ImageNegative )
                    edge_poly->m_Flags |= DRAW_ERASED;
                D(printf("\nm_Flags=0x%08X\n", edge_poly->m_Flags );)
            }

            m_PreviousPos = m_CurrentPos;
            m_PolygonFillModeState = 1;
            break;

        case 2:     // code D2: exposure OFF (i.e. "move to")
            m_Exposure = false;
            m_PreviousPos = m_CurrentPos;
            m_PolygonFillModeState = 0;
            break;

        default:
            return false;
        }
    }
    else
    {
        switch( D_commande )
        {
        case 1:     // code D01 Draw line, exposure ON
            m_Exposure = true;

            tool = GetDCODE( m_Current_Tool, false );
            if( tool )
            {
                size     = tool->m_Size;
                dcode    = tool->m_Num_Dcode;
                aperture = tool->m_Shape;
            }

            switch( m_Iterpolation )
            {
            case GERB_INTERPOL_LINEAR_1X:
                track = new TRACK( pcb );
                pcb->m_Track.Append( track );
                D(printf("R:%p\n", track );)
                fillLineTRACK(  track, dcode, activeLayer,
                                m_PreviousPos, m_CurrentPos,
                                size.x, !(m_LayerNegative ^ m_ImageNegative) );
                break;

            case GERB_INTERPOL_LINEAR_01X:
            case GERB_INTERPOL_LINEAR_001X:
            case GERB_INTERPOL_LINEAR_10X:
                wxBell();
                break;

            case GERB_INTERPOL_ARC_NEG:
            case GERB_INTERPOL_ARC_POS:
                track = new TRACK( pcb );
                pcb->m_Track.Append( track );
                D(printf("R:%p\n", track );)
                fillArcTRACK(  track, dcode, activeLayer,
                     m_PreviousPos, m_CurrentPos, m_IJPos,
                     size.x, m_Iterpolation==GERB_INTERPOL_ARC_NEG ? false : true,
                     m_360Arc_enbl, !(m_LayerNegative ^ m_ImageNegative) );
                break;

            default:
                msg.Printf( wxT( "Execute_DCODE_Command: interpol error (type %X)" ),
                            m_Iterpolation );
                DisplayError( frame, msg );
                break;
            }

            m_PreviousPos = m_CurrentPos;
            break;

        case 2:     // code D2: exposure OFF (i.e. "move to")
            m_Exposure = false;
            m_PreviousPos = m_CurrentPos;
            break;

        case 3:     // code D3: flash aperture
            tool = GetDCODE( m_Current_Tool, false );
            if( tool )
            {
                size     = tool->m_Size;
                dcode    = tool->m_Num_Dcode;
                aperture = tool->m_Shape;
            }

            switch( aperture )
            {
            case APT_LINE:  // APT_LINE is not in the spec, don't know why it's here
            case APT_CIRCLE:
                track = new TRACK( pcb );
                pcb->m_Track.Append( track );
                D(printf("R:%p\n", track );)
                fillRoundFlashTRACK( track, dcode, activeLayer,
                                m_CurrentPos,
                                size.x, !(m_LayerNegative ^ m_ImageNegative) );
                break;

            case APT_OVAL:
            case APT_RECT:
                track = new TRACK( pcb );
                pcb->m_Track.Append( track );
                D(printf("R:%p\n", track );)
                fillOvalOrRectFlashTRACK( track, dcode, activeLayer,
                                m_CurrentPos, size,
                                aperture == APT_RECT ? S_SPOT_RECT : S_SPOT_OVALE,
                                !(m_LayerNegative ^ m_ImageNegative) );
                break;

            case APT_MACRO:
                {
                    APERTURE_MACRO* macro = tool->GetMacro();
                    wxASSERT( macro );

                    // split the macro primitives up into multiple normal TRACK elements
                    for( AM_PRIMITIVES::iterator p=macro->primitives.begin();  p!=macro->primitives.end();  ++p )
                    {
                        bool    exposure;
                        wxPoint curPos = m_CurrentPos;

                        switch( p->primitive_id )
                        {
                        case AMP_CIRCLE:
                            {
                                exposure = mapExposure( p->GetExposure(), m_Exposure, m_ImageNegative );
                                curPos += mapPt( p->params[2].GetValue( tool ), p->params[3].GetValue( tool ), m_GerbMetric );
                                int diameter = scale( p->params[1].GetValue( tool ), m_GerbMetric );

                                track = new TRACK( pcb );
                                pcb->m_Track.Append( track );
                                D(printf("R:%p\n", track );)
                                fillRoundFlashTRACK( track, dcode, activeLayer,
                                                m_CurrentPos,
                                                diameter, exposure );
                            }
                            break;

                        case AMP_LINE2:
                        case AMP_LINE20:
                            {
                                exposure = mapExposure( p->GetExposure(), m_Exposure, m_ImageNegative );
                                int width  = scale( p->params[1].GetValue( tool ), m_GerbMetric );
                                wxPoint start = mapPt( p->params[2].GetValue( tool ), p->params[3].GetValue( tool ), m_GerbMetric );
                                wxPoint end   = mapPt( p->params[4].GetValue( tool ), p->params[5].GetValue( tool ), m_GerbMetric );

                                if( start.x == end.x )
                                {
                                    size.x = width;
                                    size.y = ABS( end.y - start.y ) + 1;
                                }
                                else
                                {
                                    size.x = ABS( end.x - start.x ) + 1;
                                    size.y = width;
                                }

                                wxPoint midPoint( (start.x + end.x)/2, (start.y+end.y)/2 );
                                curPos += midPoint;
                                track = new TRACK( pcb );
                                pcb->m_Track.Append( track );
                                D(printf("R:%p\n", track );)
                                fillOvalOrRectFlashTRACK( track, dcode, activeLayer,
                                               curPos, size, S_SPOT_RECT,
                                               exposure );
                            }
                            break;

                        case AMP_LINE_CENTER:
                            {
                                exposure = mapExposure( p->GetExposure(), m_Exposure, m_ImageNegative );
                                wxPoint msize = mapPt( p->params[1].GetValue( tool ), p->params[2].GetValue( tool ), m_GerbMetric );
                                size.x = msize.x;
                                size.y = msize.y;
                                curPos += mapPt( p->params[3].GetValue( tool ), p->params[4].GetValue( tool ), m_GerbMetric );
                                track = new TRACK( pcb );
                                pcb->m_Track.Append( track );
                                D(printf("R:%p\n", track );)
                                fillOvalOrRectFlashTRACK( track, dcode, activeLayer,
                                               curPos, size, S_SPOT_RECT,
                                               exposure );
                            }
                            break;

                        case AMP_LINE_LOWER_LEFT:
                            {
                                exposure = mapExposure( p->GetExposure(), m_Exposure, m_ImageNegative );
                                wxPoint msize = mapPt( p->params[1].GetValue( tool ), p->params[2].GetValue( tool ), m_GerbMetric );
                                size.x = msize.x;
                                size.y = msize.y;
                                wxPoint lowerLeft = mapPt( p->params[3].GetValue( tool ), p->params[4].GetValue( tool ), m_GerbMetric );
                                curPos += lowerLeft;
                                // need the middle, so adjust from the lower left
                                curPos.y += size.y/2;
                                curPos.x += size.x/2;
                                track = new TRACK( pcb );
                                pcb->m_Track.Append( track );
                                D(printf("R:%p\n", track );)
                                fillOvalOrRectFlashTRACK( track, dcode, activeLayer,
                                               curPos, size, S_SPOT_RECT,
                                               exposure );
                            }
                            break;

                        case AMP_THERMAL:
                            {
                                int outerDiam = scale( p->params[2].GetValue(tool), m_GerbMetric );
                                int innerDiam = scale( p->params[3].GetValue(tool), m_GerbMetric );

                                curPos += mapPt( p->params[0].GetValue( tool ), p->params[1].GetValue( tool ), m_GerbMetric );

                                track = new TRACK( pcb );
                                pcb->m_Track.Append( track );
                                D(printf("R:%p\n", track );)
                                fillRoundFlashTRACK( track, dcode, activeLayer, curPos,
                                                outerDiam, !(m_LayerNegative ^ m_ImageNegative) );

                                track = new TRACK( pcb );
                                pcb->m_Track.Append( track );
                                D(printf("R:%p\n", track );)
                                fillRoundFlashTRACK( track, dcode, activeLayer, curPos,
                                                innerDiam, (m_LayerNegative ^ m_ImageNegative) );

                                // @todo: draw the cross hairs, see page 23 of rs274 spec.
                                // this might be done with two lines, thickness from params[4], and drawing
                                // darkness "(m_LayerNegative ^ m_ImageNegative)"
                            }
                            break;

                        case AMP_MOIRE:
                            {
                                curPos += mapPt( p->params[0].GetValue( tool ), p->params[1].GetValue( tool ), m_GerbMetric );

                                // e.g.: "6,0,0,0.125,.01,0.01,3,0.003,0.150,0"
                                int outerDiam = scale( p->params[2].GetValue(tool), m_GerbMetric );
                                int penThickness = scale( p->params[3].GetValue(tool), m_GerbMetric );
                                int gap = scale( p->params[4].GetValue(tool), m_GerbMetric );
                                int numCircles = p->params[5].GetValue(tool);
                                int crossHairThickness = scale( p->params[6].GetValue(tool), m_GerbMetric );
                                int crossHairLength = scale( p->params[7].GetValue(tool), m_GerbMetric );
                                // ignore rotation, not supported

                                int diamAdjust = 2 * (gap + penThickness);	// adjust outerDiam by this on each nested circle
                                for( int i=0; i<numCircles;  ++i, outerDiam -= diamAdjust )
                                {
                                    track = new TRACK( pcb );
                                    pcb->m_Track.Append( track );
                                    D(printf("R:%p\n", track );)
                                    fillCircularTRACK( track, dcode, activeLayer, curPos, outerDiam,
                                            penThickness, !(m_LayerNegative ^ m_ImageNegative) );
                                }

                                track = new TRACK( pcb );
                                pcb->m_Track.Append( track );
                                D(printf("R:%p\n", track );)
                                fillOvalOrRectFlashTRACK( track, dcode, activeLayer,
                                        curPos, wxSize(crossHairThickness,crossHairLength),
                                        S_SPOT_RECT, !(m_LayerNegative ^ m_ImageNegative) );

                                track = new TRACK( pcb );
                                pcb->m_Track.Append( track );
                                D(printf("R:%p\n", track );)

                                // swap x and y in wxSize() for this one
                                fillOvalOrRectFlashTRACK( track, dcode, activeLayer,
                                        curPos, wxSize(crossHairLength,crossHairThickness),
                                        S_SPOT_RECT, !(m_LayerNegative ^ m_ImageNegative) );
                            }
                            break;

                        case AMP_EOF:
                        case AMP_OUTLINE:
                        case AMP_POLYGON:
                        default:
                            // not yet supported, waiting for you.
                            break;
                        }
                    }
                }
                break;

            default:
                break;
            }

            m_PreviousPos = m_CurrentPos;
            break;

        default:
            return false;
        }
    }

    return true;
}

