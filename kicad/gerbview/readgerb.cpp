/********************************************************/
/**** Routine de lecture et visu d'un fichier GERBER ****/
/********************************************************/

#include "fctsys.h"

#include "common.h"
#include "gerbview.h"
#include "pcbplot.h"

#include "protos.h"

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


/* Variables locales : */

/* Routines Locales */


/* Routine de Lecture d'un fichier de D Codes.
 *  Accepte format standard ou ALSPCB
 *      un ';' demarre un commentaire.
 *
 *  Format Standard:
 *  tool,	 Horiz,		  Vert,	  drill, vitesse, acc. ,Type ; [DCODE (commentaire)]
 *  ex:	   1,		  12,		12,		0,		  0,	 0,	  3 ; D10
 *
 *  Format ALSPCB:
 *  Ver  ,  Hor , Type , Tool [,Drill]
 *  ex:	0.012, 0.012,  L   , D10
 *
 *  Classe les caract en buf_tmp sous forme de tableau de structures D_CODE.
 *  Retourne:
 *      < 0 si erreur:
 *          -1 = Fichier non trouve
 *          -2 = Erreur lecture fichier
 *      Rang de D_code maxi lu ( nbr de dcodes )
 *
 *
 *  Representation interne:
 *
 *  Les lignes sont repr�sent�es par des TRACKS standards
 *  Les Flash  sont repr�sent�es par des DRAWSEGMENTS
 *      - ronds ou ovales: DRAWSEGMENTS
 *      - rectangles: DRAWSEGMENTS
 *  la reference aux D-CODES est plac�e dans le membre GetNet()
 */


/********************************************************/
bool WinEDA_GerberFrame::Read_GERBER_File( wxDC*           DC,
                                           const wxString& GERBER_FullFileName,
                                           const wxString& D_Code_FullFileName )
/********************************************************/

/* Read a gerber file (RS274D or RS274X format).
 *  Normal format:
 *      Imperial
 *      Absolute
 *      end of block = *
 *      CrLf after each command
 *      G codes repetes
 */
{
    int           G_commande = 0,
                  D_commande = 0;   /* command number for G et D commands (like G04 or D02) */
    char          Line[4000];       /* Buffer to read 1 line of the gerber file
                                     * warning: some files can have very long lines, so the buffer must be large
                                     */
    wxString      msg;
    char*         text;
    int           layer;    /* current layer used in gerbview */
    GERBER_Descr* gerber_layer;
    wxPoint       pos;
    int           error = 0;

    layer = GetScreen()->m_Active_Layer;

    if( g_GERBER_Descr_List[layer] == NULL )
    {
        g_GERBER_Descr_List[layer] = new GERBER_Descr( layer );
    }

    gerber_layer = g_GERBER_Descr_List[layer];

    /* Set the gerber scale: */
    gerber_layer->ResetDefaultValues();

    /* Read the gerber file */
    gerber_layer->m_Current_File = wxFopen( GERBER_FullFileName, wxT( "rt" ) );
    if( gerber_layer->m_Current_File == 0 )
    {
        msg = _( "File " ) + GERBER_FullFileName + _( " not found" );
        DisplayError( this, msg, 10 );
        return FALSE;
    }

    gerber_layer->m_FileName = GERBER_FullFileName;

    wxString path = wxPathOnly( GERBER_FullFileName );
    if( path != wxEmptyString )
        wxSetWorkingDirectory( path );

    wxBusyCursor show_wait;
    SetLocaleTo_C_standard( );

    while( TRUE )
    {
        if( fgets( Line, sizeof(Line) - 1, gerber_layer->m_Current_File ) == NULL ) // E.O.F
        {
            if( gerber_layer->m_FilesPtr == 0 )
                break;

            fclose( gerber_layer->m_Current_File );

            gerber_layer->m_FilesPtr--;
            gerber_layer->m_Current_File =
                gerber_layer->m_FilesList[gerber_layer->m_FilesPtr];

            continue;
        }

        text = StrPurge( Line );

        while( text && *text )
        {
            switch( *text )
            {
            case ' ':
            case '\r':
            case '\n':
                text++;
                break;

            case '*':       // End commande
                gerber_layer->m_CommandState = END_BLOCK;
                text++;
                break;

            case 'M':       // End file
                gerber_layer->m_CommandState = CMD_IDLE;
                while( *text )
                    text++;

                break;

            case 'G':    /* Line type Gxx : command */
                G_commande = gerber_layer->ReturnGCodeNumber( text );
                gerber_layer->Execute_G_Command( text, G_commande );
                break;

            case 'D':       /* Line type Dxx : Tool selection (xx > 0) or command if xx = 0..9*/
                D_commande = gerber_layer->ReturnDCodeNumber( text );
                gerber_layer->Execute_DCODE_Command( this, DC,
                                                     text, D_commande );
                break;

            case 'X':
            case 'Y':                   /* Move or draw command */
                pos = gerber_layer->ReadXYCoord( text );
                if( *text == '*' )      // command like X12550Y19250*
                {
                    gerber_layer->Execute_DCODE_Command( this, DC, text,
                                                         gerber_layer->m_Last_Pen_Command );
                }
                break;

            case 'I':
            case 'J':       /* Auxiliary Move command */
                pos = gerber_layer->ReadIJCoord( text );
                break;

            case '%':
                if( gerber_layer->m_CommandState != ENTER_RS274X_CMD )
                {
                    gerber_layer->m_CommandState = ENTER_RS274X_CMD;

                    if( !gerber_layer->ReadRS274XCommand( this, DC, Line, text ) )
                    {
                        error++;
                    }
                }
                else        //Error
                {
                    error++;
                    gerber_layer->m_CommandState = CMD_IDLE;
                    text++;
                }
                break;

            default:
                text++;
                error++;
                break;
            }
        }
    }

    if( error )
    {
        msg.Printf( _( "%d errors while reading Gerber file [%s]" ),
                   error, GERBER_FullFileName.GetData() );
        DisplayError( this, msg );
    }
    fclose( gerber_layer->m_Current_File );

    SetLocaleTo_Default( );

    /* Init  DCodes list and perhaps read a DCODES file,
     * if the gerber file is only a RS274D file (without any aperture information)
     */
    if( !gerber_layer->m_As_DCode )
    {
        wxString DCodeFileName;
        if( D_Code_FullFileName.IsEmpty() )
        {
            wxString mask;
            DCodeFileName = GERBER_FullFileName;
            ChangeFileNameExt( DCodeFileName, g_PenFilenameExt );
            mask = wxT( "*" ) + g_PenFilenameExt;
            DCodeFileName = EDA_FileSelector( _( "D codes files:" ),
                                              wxEmptyString,    /* Chemin par defaut */
                                              DCodeFileName,    /* nom fichier par defaut */
                                              g_PenFilenameExt, /* extension par defaut */
                                              mask,             /* Masque d'affichage */
                                              this,
                                              0,
                                              TRUE
                                              );
        }
        else
            DCodeFileName = D_Code_FullFileName;

        if( !DCodeFileName.IsEmpty() )
        {
            Read_D_Code_File( DCodeFileName );
            CopyDCodesSizeToItems();
        }
    }

    return TRUE;
}
