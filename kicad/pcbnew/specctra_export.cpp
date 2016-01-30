/*
 * This program source code file is part of KICAD, a free EDA CAD application.
 *
 * Copyright (C) 2007-2008 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
 * Copyright (C) 2007 Kicad Developers, see change_log.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */


/*  This source is a complement to specctra.cpp and implements the export to
    specctra dsn file format.  The specification for the grammar of the specctra
    dsn file used to develop this code is given here:
    http://tech.groups.yahoo.com/group/kicad-users/files/  then file "specctra.pdf"

    Also see the comments at the top of the specctra.cpp file itself.
*/


#include "specctra.h"
#include "collectors.h"
#include "wxPcbStruct.h"        // Change_Side_Module()
#include "pcbstruct.h"          // HISTORY_NUMBER
#include "autorout.h"           // NET_CODES_OK

#include "trigo.h"              // RotatePoint()
#include <set>                  // std::set
#include <map>                  // std::map

#include <boost/utility.hpp>    // boost::addressof()

using namespace DSN;


// see wxPcbStruct.h
void WinEDA_PcbFrame::ExportToSpecctra( wxCommandEvent& event )
{
    wxString        fullFileName = GetScreen()->m_FileName;
    wxString        path;
    wxString        name;
    wxString        ext;

    wxString        dsn_ext = wxT( ".dsn" );
    wxString        mask    = wxT( "*" ) + dsn_ext;

    wxFileName::SplitPath( fullFileName, &path, &name, &ext );
    name += dsn_ext;

    fullFileName = EDA_FileSelector( _( "Specctra DSN file:" ),
                                     path,
                                     name,      // name.ext without path!
                                     dsn_ext,
                                     mask,
                                     this,
                                     wxFD_SAVE,
                                     FALSE
                                     );
    if( fullFileName == wxEmptyString )
        return;

    SPECCTRA_DB     db;
    bool            ok = true;
    wxString        errorText;

    BASE_SCREEN*    screen = GetScreen();
    bool            wasModified = screen->IsModify() && !screen->IsSave();

    db.SetPCB( SPECCTRA_DB::MakePCB() );

    SetLocaleTo_C_standard( );    // Switch the locale to standard C

    //  DSN Images (=Kicad MODULES and pads) must be presented from the
    //  top view.  So we temporarily flip any modules which are on the back
    //  side of the board to the front, and record this in the MODULE's flag field.
    db.FlipMODULEs( m_Pcb );

    try
    {
        db.FromBOARD( m_Pcb );
        db.ExportPCB(  fullFileName, true );

        // if an exception is thrown by FromBOARD or ExportPCB(), then
        // ~SPECCTRA_DB() will close the file.
    }
    catch( IOError ioe )
    {
        ok = false;

        // copy the error string to safe place, ioe is in this scope only.
        errorText = ioe.errorText;
    }

    SetLocaleTo_Default( );      // revert to the current locale

    // done assuredly, even if an exception was thrown and caught.
    db.RevertMODULEs( m_Pcb );


    // The two calls below to BOARD::Change_Side_Module(), both set the
    // modified flag, yet their actions cancel each other out, so it should
    // be ok to clear the modify flag.
    if( !wasModified )
        screen->ClrModify();

    if( ok )
    {
        Affiche_Message( wxString( _("BOARD exported OK.")) );
    }
    else
    {
        errorText += '\n';
        errorText += _("Unable to export, please fix and try again.");
        DisplayError( this, errorText );
    }
}


namespace DSN {


const KICAD_T SPECCTRA_DB::scanPADs[] = { TYPEPAD, EOT };


/**
 * Function scale
 * converts a distance from kicad units to our reported specctra dsn units:
 * 1/10000 inches (deci-mils) to mils.  So the factor of 10 comes in.
 */
static inline double scale( int kicadDist )
{
    return kicadDist/10.0;
}

static inline double mapX( int x )
{
    return scale(x);
}

static inline double mapY( int y )
{
    return -scale(y);      // make y negative, since it is increasing going down.
}


/**
 * Function mapPt
 * converts a Kicad point into a DSN file point.  Kicad's BOARD coordinates
 * are in deci-mils  (i.e. 1/10,000th of an inch) and we are exporting in units
 * of mils, so we have to divide by 10.
 */
static POINT mapPt( const wxPoint& pt )
{
    POINT ret;
    ret.x = mapX( pt.x );
    ret.y = mapY( pt.y );
    ret.FixNegativeZero();
    return ret;
}


/**
 * Function findPoint
 * searches for a DRAWSEGMENT with an end point or start point of aPoint, and
 * if found, removes it from the TYPE_COLLECTOR and returns it, else returns NULL.
 * @param aPoint The starting or ending point to search for.
 * @return DRAWSEGMENT* - The first DRAWSEGMENT that has a start or end point matching
 *   aPoint, otherwise NULL if none.
 */
static DRAWSEGMENT* findPoint( const wxPoint& aPoint, TYPE_COLLECTOR* items )
{
    for( int i=0;  i<items->GetCount();  ++i )
    {
        DRAWSEGMENT* graphic = (DRAWSEGMENT*) (*items)[i];

        wxASSERT( graphic->Type() == TYPEDRAWSEGMENT );

        if( aPoint == graphic->GetStart() || aPoint == graphic->GetEnd() )
        {
            items->Remove(i);
            return graphic;
        }
    }


#if defined(DEBUG)
    printf("Unable to find segment matching point (%d,%d)\n",
                         aPoint.x, aPoint.y );

    for( int i=0;  i<items->GetCount();  ++i )
    {
        DRAWSEGMENT* graphic = (DRAWSEGMENT*) (*items)[i];

        printf( "type=%s, GetStart()=%d,%d  GetEnd()=%d,%d\n",
                CONV_TO_UTF8( BOARD_ITEM::ShowShape( (Track_Shapes)graphic->m_Shape ) ),
                graphic->GetStart().x,
                graphic->GetStart().y,
                graphic->GetEnd().x,
                graphic->GetEnd().y );
    }
#endif

    return NULL;
}


/**
 * Function isRoundKeepout
 * decides if the pad is a copper-less through hole which needs to be made into
 * a round keepout.
 */
static bool isRoundKeepout( D_PAD* aPad )
{
    if( aPad->m_PadShape==PAD_CIRCLE )
    {
        if( aPad->m_Drill.x >= aPad->m_Size.x )
            return true;

        if( (aPad->m_Masque_Layer & ALL_CU_LAYERS) == 0 )
            return true;
    }

    return false;
}


/**
 * Function makePath
 * creates a PATH element with a single straight line, a pair of vertices.
 */
static PATH* makePath( const POINT& aStart, const POINT& aEnd, const std::string& aLayerName )
{
    PATH*   path = new PATH( 0, T_path );

    path->AppendPoint( aStart );
    path->AppendPoint( aEnd );
    path->SetLayerId( aLayerName.c_str() );
    return path;
}


/**
 * Struct wxString_less_than
 * is used by the std:set<> and std::map<> instantiations below.
 * See STRINGSET typedef and PINMAP typedef below.
 */
struct wxString_less_than
{
    // a "less than" test on two wxStrings
    bool operator()( const wxString& s1, const wxString& s2) const
    {
        return s1.Cmp( s2 ) < 0;  // case specific wxString compare
    }
};


/**
 * Function makePADSTACK
 * creates a PADSTACK which matches the given pad.  Only pads which do not
 * satisfy the function isKeepout() should be passed to this function.
 * @param aPad The D_PAD which needs to be made into a PADSTACK.
 * @return PADSTACK* - The created padstack, including its padstack_id.
 */
PADSTACK* SPECCTRA_DB::makePADSTACK( BOARD* aBoard, D_PAD* aPad )
{
    char        name[80];                   // padstack name builder
    std::string uniqifier;

    // caller must do these checks before calling here.
    wxASSERT( !isRoundKeepout( aPad ) );

    PADSTACK*   padstack = new PADSTACK();

    int         reportedLayers = 0;              // how many in reported padstack
    const char* layerName[NB_COPPER_LAYERS];

    if( aPad->m_Attribut==PAD_SMD || aPad->m_Attribut==PAD_CONN )
    {
        // PAD_SMD and PAD_CONN are reported on each layer for which
        // they are present.
        uniqifier = '[';

        if( aPad->IsOnLayer( LAYER_CMP_N ) )
        {
            layerName[reportedLayers++] = layerIds[0].c_str();
            uniqifier += 'T';   // T for top, could have used a layer index here alternatively
        }
        if( aPad->IsOnLayer( COPPER_LAYER_N ) )
        {
            int pcbLayerNdx = kicadLayer2pcb[COPPER_LAYER_N];
            layerName[reportedLayers++] = layerIds[ pcbLayerNdx ].c_str();
            uniqifier += 'B';   // B for bottom
        }

        uniqifier += ']';
    }

    else        // through hole pad
    {
        uniqifier = '[';

        bool onAllCopperLayers = false;
        if( (aPad->m_Masque_Layer & ALL_CU_LAYERS) == ALL_CU_LAYERS )
        {
            onAllCopperLayers = true;
            uniqifier += 'A';               // A for all layers
        }

        const int copperCount = aBoard->GetCopperLayerCount();
        for( int layer=0;  layer<copperCount;  ++layer )
        {
            int kilayer = pcbLayer2kicad[layer];

            if( onAllCopperLayers || aPad->IsOnLayer( kilayer ) )
            {
                layerName[reportedLayers++] = layerIds[layer].c_str();

                if( !onAllCopperLayers )
                {
                    if( layer == 0 )
                        uniqifier += 'T';
                    else if( layer == copperCount-1 )
                        uniqifier += 'B';
                    else
                        uniqifier += char('0' + layer);  // layer index char
                }
            }
        }

        uniqifier += ']';
    }

    POINT   dsnOffset;

    if( aPad->m_Offset.x || aPad->m_Offset.y )
    {
        char offsetTxt[32];

        wxPoint offset( aPad->m_Offset.x, aPad->m_Offset.y );

        dsnOffset = mapPt( offset );

        // using '(' or ')' would cause padstack name to be quote wrapped,
        // so use other brackets, and {} locks freerouter.
        sprintf( offsetTxt, "[%.6g,%.6g]", dsnOffset.x, dsnOffset.y );

        uniqifier += offsetTxt;
    }

    switch( aPad->m_PadShape )
    {
    default:
    case PAD_CIRCLE:
        {
            double  diameter = scale(aPad->m_Size.x);

            for( int ndx=0;  ndx<reportedLayers;  ++ndx )
            {
                SHAPE*      shape = new SHAPE( padstack );
                padstack->Append( shape );

                CIRCLE*     circle = new CIRCLE( shape );
                shape->SetShape( circle );

                circle->SetLayerId( layerName[ndx] );
                circle->SetDiameter( diameter );
                circle->SetVertex( dsnOffset );
            }

            snprintf( name, sizeof(name), "Round%sPad_%.6g_mil",
                     uniqifier.c_str(), scale(aPad->m_Size.x) );
            name[ sizeof(name)-1 ] = 0;

            padstack->SetPadstackId( name );
        }
        break;

    case PAD_RECT:
        {
            double dx = scale( aPad->m_Size.x ) / 2.0;
            double dy = scale( aPad->m_Size.y ) / 2.0;

            POINT   lowerLeft( -dx, -dy );
            POINT   upperRight( dx, dy );

            lowerLeft  += dsnOffset;
            upperRight += dsnOffset;

            for( int ndx=0;  ndx<reportedLayers;  ++ndx )
            {
                SHAPE*      shape = new SHAPE( padstack );
                padstack->Append( shape );

                RECTANGLE*  rect = new RECTANGLE( shape );
                shape->SetShape( rect );

                rect->SetLayerId( layerName[ndx] );
                rect->SetCorners( lowerLeft, upperRight );
            }

            snprintf( name, sizeof(name),  "Rect%sPad_%.6gx%.6g_mil",
                     uniqifier.c_str(), scale(aPad->m_Size.x), scale(aPad->m_Size.y)  );
            name[ sizeof(name)-1 ] = 0;

            padstack->SetPadstackId( name );
        }
        break;

    case PAD_OVAL:
        {
            double  dx = scale( aPad->m_Size.x ) / 2.0;
            double  dy = scale( aPad->m_Size.y ) / 2.0;
            double  dr = dx - dy;
            double  radius;
            POINT   start;
            POINT   stop;

            if( dr >= 0 )       // oval is horizontal
            {
                radius = dy;

                start = POINT( -dr, 0.0 );
                stop  = POINT(  dr, 0.0 );
            }
            else        // oval is vertical
            {
                radius = dx;
                dr = -dr;

                start = POINT( 0.0, -dr );
                stop  = POINT( 0.0,  dr );
            }

            start += dsnOffset;
            stop  += dsnOffset;

            for( int ndx=0;  ndx<reportedLayers;  ++ndx )
            {
                SHAPE*  shape;
                PATH*   path;
                // see http://www.freerouting.net/usren/viewtopic.php?f=3&t=317#p408
                shape = new SHAPE( padstack );
                padstack->Append( shape );
                path = makePath( start, stop, layerName[ndx] );
                shape->SetShape( path );
                path->aperture_width = 2.0 * radius;
            }

            snprintf( name, sizeof(name),  "Oval%sPad_%.6gx%.6g_mil",
                     uniqifier.c_str(), scale(aPad->m_Size.x), scale(aPad->m_Size.y)  );
            name[ sizeof(name)-1 ] = 0;

            padstack->SetPadstackId( name );
        }
        break;

/*
    case PAD_TRAPEZOID:
        break;
*/
    }

    return padstack;
}


/// data type used to ensure unique-ness of pin names, holding (wxString and int)
typedef std::map<wxString, int, wxString_less_than> PINMAP;


IMAGE* SPECCTRA_DB::makeIMAGE( BOARD* aBoard, MODULE* aModule )
{
    PINMAP          pinmap;
    TYPE_COLLECTOR  moduleItems;
    wxString        padName;


    // get all the MODULE's pads.
    moduleItems.Collect( aModule, scanPADs );

    IMAGE*  image = new IMAGE(0);

    image->image_id = CONV_TO_UTF8( aModule->m_LibRef );

    // from the pads, and make an IMAGE using collated padstacks.
    for( int p=0;  p<moduleItems.GetCount();  ++p )
    {
        D_PAD* pad = (D_PAD*) moduleItems[p];

        // see if this pad is a through hole with no copper on its perimeter
        if( isRoundKeepout( pad ) )
        {
            double  diameter = scale( pad->m_Drill.x );
            POINT   vertex   = mapPt( pad->m_Pos0 );

            int layerCount = aBoard->GetCopperLayerCount();
            for( int layer=0;  layer<layerCount;  ++layer )
            {
                KEEPOUT* keepout = new KEEPOUT(image, T_keepout);
                image->keepouts.push_back( keepout );

                CIRCLE*  circle = new CIRCLE( keepout );
                keepout->SetShape( circle );

                circle->SetDiameter( diameter );
                circle->SetVertex( vertex );
                circle->SetLayerId( layerIds[layer].c_str() );
            }
        }

        // else if() could there be a square keepout here?

        else
        {
            PADSTACK*   padstack = makePADSTACK( aBoard, pad );

            PADSTACKSET::iterator iter = padstackset.find( *padstack );
            if( iter != padstackset.end() )
            {
                // padstack is a duplicate, delete it and use the original
                delete padstack;
                padstack = (PADSTACK*) *iter.base();    // folklore, be careful here
            }
            else
            {
                padstackset.insert( padstack );
            }

            PIN*    pin = new PIN(image);

            padName = pad->ReturnStringPadName();
            pin->pin_id  = CONV_TO_UTF8( padName );

            if( padName!=wxEmptyString && pinmap.find( padName )==pinmap.end() )
            {
                pinmap[ padName ] = 0;
            }
            else    // pad name is a duplicate within this module
            {
                char buf[32];

                int duplicates = ++pinmap[ padName ];

                sprintf( buf, "@%d", duplicates );

                pin->pin_id += buf;      // append "@1" or "@2", etc. to pin name
            }

            pin->kiNetCode = pad->GetNet();

            image->pins.push_back( pin );

            pin->padstack_id = padstack->padstack_id;

            int angle = pad->m_Orient - aModule->m_Orient;   // tenths of degrees
            if( angle )
            {
                NORMALIZE_ANGLE_POS(angle);
                pin->SetRotation( angle / 10.0 );
            }

            wxPoint pos( pad->m_Pos0 );

            pin->SetVertex( mapPt( pos )  );
        }
    }

#if 1   // enable image (outline) scopes.
    static const KICAD_T scanEDGEs[] = { TYPEEDGEMODULE, EOT };

    // get all the MODULE's EDGE_MODULEs and convert those to DSN outlines.
    moduleItems.Collect( aModule, scanEDGEs );

    for( int i=0;  i<moduleItems.GetCount();  ++i )
    {
        EDGE_MODULE*    graphic = (EDGE_MODULE*) moduleItems[i];
        SHAPE*          outline;
        PATH*           path;

        switch( graphic->m_Shape )
        {
        case S_SEGMENT:
            outline = new SHAPE( image, T_outline );
            image->Append( outline );
            path = new PATH( outline );
            outline->SetShape( path );
            path->SetAperture( scale( graphic->m_Width ) );
            path->SetLayerId( "signal" );
            path->AppendPoint( mapPt( graphic->m_Start0 ) );
            path->AppendPoint( mapPt( graphic->m_End0 ) );
            break;

        case S_CIRCLE:
            {
                // this is best done by 4 QARC's but freerouter does not yet support QARCs.
                // for now, support by using line segments.

                outline = new SHAPE( image, T_outline );
                image->Append( outline );
                path = new PATH( outline );
                outline->SetShape( path );
                path->SetAperture( scale( graphic->m_Width ) );
                path->SetLayerId( "signal" );

                // Do the math using Kicad units, that way we stay out of the
                // scientific notation range of floating point numbers in the
                // DSN file.   We do not parse scientific notation in our own
                // lexer/beautifier, and the spec is not clear that this is
                // required.  Fixed point floats are all that should be needed.

                double  radius = hypot( double( graphic->m_Start.x - graphic->m_End.x ),
                                        double( graphic->m_Start.y - graphic->m_End.y ) );

                // better if evenly divisible into 360
                const int DEGREE_INTERVAL = 18;         // 18 means 20 line segments

                for( double radians = 0.0;  radians < 2*M_PI;  radians += DEGREE_INTERVAL * M_PI / 180.0 )
                {
                    wxPoint   point( int( radius * cos( radians ) ),
                                     int( radius * sin( radians ) ) );

                    point += graphic->m_Start0;     // an offset

                    path->AppendPoint( mapPt(point) );
                }
            }
            break;

        case S_RECT:
        case S_ARC:
        default:
            D( printf("makeIMAGE(): unsupported shape %s\n",
                      CONV_TO_UTF8( BOARD_ITEM::ShowShape( (Track_Shapes)graphic->m_Shape))  );)
            continue;
        }
    }
#endif

    return image;
}


PADSTACK* SPECCTRA_DB::makeVia( int aCopperDiameter, int aDrillDiameter,
                               int aTopLayer, int aBotLayer )
{
    char        name[48];
    PADSTACK*   padstack = new PADSTACK();

    double      dsnDiameter = scale(aCopperDiameter);

    for( int layer=aTopLayer;  layer<=aBotLayer;  ++layer )
    {
        SHAPE* shape = new SHAPE( padstack );
        padstack->Append( shape );

        CIRCLE* circle = new CIRCLE( shape );
        shape->SetShape( circle );

        circle->SetDiameter( dsnDiameter );
        circle->SetLayerId( layerIds[layer].c_str() );
    }

    snprintf( name, sizeof(name),  "Via[%d-%d]_%.6g:%.6g_mil",
             aTopLayer, aBotLayer, dsnDiameter,
             // encode the drill value into the name for later import
             scale( aDrillDiameter )
             );
    name[ sizeof(name)-1 ] = 0;
    padstack->SetPadstackId( name );

    return padstack;
}


PADSTACK* SPECCTRA_DB::makeVia( const SEGVIA* aVia )
{
    int     topLayer;
    int     botLayer;

    aVia->ReturnLayerPair( &topLayer, &botLayer );

    topLayer = kicadLayer2pcb[topLayer];
    botLayer = kicadLayer2pcb[botLayer];

    if( topLayer > botLayer )
        EXCHG( topLayer, botLayer );

    return makeVia( aVia->m_Width, aVia->GetDrillValue(), topLayer, botLayer );
}


void SPECCTRA_DB::fillBOUNDARY( BOARD* aBoard, BOUNDARY* boundary ) throw( IOError )
{
    TYPE_COLLECTOR          items;

    // get all the DRAWSEGMENTS into 'items', then look for layer == EDGE_N,
    // and those segments comprise the board's perimeter.

    static const KICAD_T  scanDRAWSEGMENTS[] = { TYPEDRAWSEGMENT, EOT };

    items.Collect( aBoard, scanDRAWSEGMENTS );

    bool haveEdges = false;

    for( int i=0;  i<items.GetCount();  )
    {
        DRAWSEGMENT* item = (DRAWSEGMENT*) items[i];

        wxASSERT( item->Type() == TYPEDRAWSEGMENT );

        if( item->GetLayer() != EDGE_N )
        {
            items.Remove( i );
        }
        else
        {
            haveEdges = true;
            ++i;
            //D( item->Show( 0, std::cout );)
        }
    }

    if( haveEdges )
    {
        PATH*  path = new PATH( boundary );
        boundary->paths.push_back( path );
        path->layer_id = "pcb";

        wxPoint         prevPt;

        DRAWSEGMENT*    graphic = (DRAWSEGMENT*) items[0];

        // the first DRAWSEGMENT is in 'graphic*', ok to remove it from 'items'
        items.Remove( 0 );

        prevPt = graphic->GetEnd();
        path->AppendPoint( mapPt( graphic->GetEnd() ) );

        // do not append the other end point yet, this first edge item might be an arc

        for(;;)
        {
            switch( graphic->m_Shape )
            {
            case S_ARC:
                // freerouter does not yet understand arcs, so approximate
                // an arc with a series of short lines and put those
                // line segments into the !same! PATH.
                {
                    const int STEPS =  9;      // in an arc of 90 degrees

                    wxPoint start  = graphic->GetStart();
                    wxPoint end    = graphic->GetEnd();
                    wxPoint center = graphic->m_Start;
                    int     angle  = -graphic->m_Angle;

                    if( prevPt != start )
                    {
                        wxASSERT( prevPt == graphic->GetEnd() );

                        angle = -angle;
                        EXCHG( start, end );
                    }

                    wxPoint nextPt;

                    for( int step=1;  step<=STEPS;  ++step )
                    {
                        int rotation = ( angle * step )/STEPS;

                        nextPt = start;

                        RotatePoint( &nextPt.x, &nextPt.y, center.x, center.y, rotation );

                        path->AppendPoint( mapPt( nextPt ) );
                    }

                    prevPt = nextPt;
                }
                break;

            case S_CIRCLE:
                // do not output a circle, freerouter does not understand it.
                // this might be a mounting hole or something, ignore it without error
                break;

            default:
                {
                    wxString error;

                    error.Printf( _("Unsupported DRAWSEGMENT type %s"),
                      BOARD_ITEM::ShowShape( (Track_Shapes) graphic->m_Shape ).GetData() );

                    ThrowIOError( error );
                }
                break;

            case S_SEGMENT:
                {
                    POINT  nextPt;

                    if( prevPt != graphic->GetStart() )
                    {
                        wxASSERT( prevPt == graphic->GetEnd() );
                        nextPt = mapPt( graphic->GetStart() );
                        prevPt = graphic->GetStart();
                    }
                    else
                    {
                        nextPt = mapPt( graphic->GetStart() );
                        prevPt = graphic->GetEnd();
                    }

                    path->AppendPoint( nextPt );
                 }
            }

            if( items.GetCount() == 0 )
                break;

            graphic = findPoint( prevPt, &items );
            if( !graphic )
            {
                wxString error;

                error << _("Unable to find the next segment with an endpoint of ");
                error << prevPt;
                error << wxChar('\n');
                error << _("Edit Edges_Pcb segments, making them contiguous.");
                ThrowIOError( error );
            }
        }
    }
    else
    {
        aBoard->ComputeBoundaryBox();

        RECTANGLE*  rect = new RECTANGLE( boundary );
        boundary->rectangle = rect;

        rect->layer_id = "pcb";

        // opposite corners
        wxPoint bottomRight;
        bottomRight.x = aBoard->m_BoundaryBox.GetRight();
        bottomRight.y = aBoard->m_BoundaryBox.GetBottom();

        rect->SetCorners( mapPt( aBoard->m_BoundaryBox.GetOrigin() ),
                          mapPt( bottomRight ) );
    }
}



typedef std::set<std::string>  STRINGSET;
typedef std::pair<STRINGSET::iterator, bool> STRINGSET_PAIR;


void SPECCTRA_DB::FromBOARD( BOARD* aBoard ) throw( IOError )
{
    TYPE_COLLECTOR          items;

    static const KICAD_T    scanMODULEs[] = { TYPEMODULE, EOT };

    // Not all boards are exportable.  Check that all reference Ids are unique.
    // Unless they are unique, we cannot import the session file which comes
    // back to us later from the router.
    {
        TYPE_COLLECTOR  padItems;

        items.Collect( aBoard, scanMODULEs );

        STRINGSET       refs;       // holds module reference designators

        for( int i=0;  i<items.GetCount();  ++i )
        {
            MODULE* module = (MODULE*) items[i];

            if( module->GetReference() == wxEmptyString )
            {
                ThrowIOError( _("Component with value of \"%s\" has empty reference id."),
                                module->GetValue().GetData() );
            }

            // if we cannot insert OK, that means the reference has been seen before.
            STRINGSET_PAIR refpair = refs.insert( CONV_TO_UTF8( module->GetReference() ) );
            if( !refpair.second )      // insert failed
            {
                ThrowIOError( _("Multiple components have identical reference IDs of \"%s\"."),
                      module->GetReference().GetData() );
            }
        }
    }

    if( !pcb )
        pcb = SPECCTRA_DB::MakePCB();

    //-----<layer_descriptor>-----------------------------------------------
    {
        // specctra wants top physical layer first, then going down to the
        // bottom most physical layer in physical sequence.
        // @question : why does Kicad not display layers in that order?

        buildLayerMaps( aBoard );

        int layerCount = aBoard->GetCopperLayerCount();

        for( int pcbNdx=0;  pcbNdx<layerCount; ++pcbNdx )
        {
            LAYER*      layer = new LAYER( pcb->structure );
            pcb->structure->layers.push_back( layer );

            layer->name = layerIds[pcbNdx];

            DSN_T layerType;
            switch( aBoard->GetLayerType( pcbLayer2kicad[pcbNdx] ) )
            {
            default:
            case LT_SIGNAL:     layerType = T_signal;       break;
            case LT_POWER:      layerType = T_power;        break;
            case LT_MIXED:      layerType = T_mixed;        break;
            case LT_JUMPER:     layerType = T_jumper;       break;
            }

            layer->layer_type = layerType;

            layer->properties.push_back( PROPERTY() );
            PROPERTY* property = &layer->properties.back();
            property->name = "index";
            char temp[32];
            sprintf( temp, "%d", pcbNdx );
            property->value = temp;
        }
    }

    // a space in a quoted token is NOT a terminator, true establishes this.
    pcb->parser->space_in_quoted_tokens = true;

    //-----<unit_descriptor> & <resolution_descriptor>--------------------
    {
        pcb->unit->units = T_mil;
        pcb->resolution->units = T_mil;

        // Kicad only supports 1/10th of mil internal coordinates.  So to avoid
        // having the router give us back 1/100th of mil coordinates which we
        // will have to round and thereby cause error, we declare our maximum
        // resolution precisely at 1/10th for now.  For more on this, see:
        // http://www.freerouting.net/usren/viewtopic.php?f=3&t=354
        pcb->resolution->value = 10;
    }


    //-----<boundary_descriptor>------------------------------------------
    {
        // Because fillBOUNDARY() can throw an exception, we link in an
        // empty boundary so the BOUNDARY does not get lost in the event of
        // of an exception.
        BOUNDARY* boundary = new BOUNDARY(0);
        pcb->structure->SetBOUNDARY( boundary );
        fillBOUNDARY( aBoard, boundary );
    }


    //-----<rules>--------------------------------------------------------
    {
        // put out these rules, the user can then edit them with a text editor
        char    rule[80];

        int     curTrackWidth = aBoard->m_BoardSettings->m_CurrentTrackWidth;
        int     curTrackClear = aBoard->m_BoardSettings->m_TrackClearence;

        // The +5 is to give freerouter a little extra room, this is 0.5 mils.
        // If we export without this, then on import freerouter violates our
        // DRC checks with track to via spacing, although this could be a
        // result of > testing vs. >= testing in PCBNEW's DRC.
        double  clearance = scale(curTrackClear+5);

        STRINGS& rules = pcb->structure->rules->rules;

        sprintf( rule, "(width %.6g)", scale( curTrackWidth ) );
        rules.push_back( rule );

        sprintf( rule, "(clearance %.6g)", clearance );
        rules.push_back( rule );

        /* see: http://www.freerouting.net/usren/viewtopic.php?f=5&t=339#p474
        sprintf( rule, "(clearance %.6g (type pad_to_turn_gap))", clearance );
        rules.push_back( rule );

        sprintf( rule, "(clearance %.6g (type smd_to_turn_gap))", clearance );
        rules.push_back( rule );

        sprintf( rule, "(clearance %.6g (type via_via))", clearance );
        rules.push_back( rule );

        sprintf( rule, "(clearance %.6g (type via_smd))", clearance );
        rules.push_back( rule );

        sprintf( rule, "(clearance %.6g (type via_pin))", clearance );
        rules.push_back( rule );

        sprintf( rule, "(clearance %.6g (type pin_pin))", clearance );
        rules.push_back( rule );

        sprintf( rule, "(clearance %.6g (type smd_pin))", clearance );
        rules.push_back( rule );
        */

        // well, the user is going to text edit these in the DSN file anyway,
        // at least until we have an export dialog.
        clearance = scale(curTrackClear)/4;

        sprintf( rule, "(clearance %.6g (type smd_smd))", clearance );
        rules.push_back( rule );
    }


    //-----<zone containers become planes>--------------------------------
    {
        static const KICAD_T  scanZONEs[] = { TYPEZONE_CONTAINER, EOT };
        items.Collect( aBoard, scanZONEs );

        for( int i=0;  i<items.GetCount();  ++i )
        {
            ZONE_CONTAINER* item = (ZONE_CONTAINER*) items[i];

            COPPER_PLANE*   plane = new COPPER_PLANE( pcb->structure );
            pcb->structure->planes.push_back( plane );

            PATH*           polygon = new PATH( plane, T_polygon );
            plane->SetShape( polygon );

            plane->name = CONV_TO_UTF8( item->m_Netname );

            polygon->layer_id = layerIds[ kicadLayer2pcb[ item->GetLayer() ] ];

            int count = item->m_Poly->corner.size();
            for( int j=0; j<count; ++j )
            {
                wxPoint   point( item->m_Poly->corner[j].x,
                                 item->m_Poly->corner[j].y );
                polygon->AppendPoint( mapPt(point) );
            }
        }
    }

    // keepouts could go here, there are none in Kicad at this time.

    //-----<build the images, components, and netlist>-----------------------
    {
        PIN_REF     empty( pcb->network );
        std::string componentId;

        // find the highest numbered netCode within the board.
        int highestNetCode = -1;
        for( EQUIPOT* equipot = aBoard->m_Equipots;  equipot;  equipot = equipot->Next() )
            highestNetCode = MAX( highestNetCode, equipot->GetNet() );

        deleteNETs();

        // expand the net vector to highestNetCode+1, setting empty to NULL
        nets.resize( highestNetCode+1, NULL );

        // skip netcode = 0
        for( unsigned i=1;  i<nets.size();  ++i )
            nets[i] = new NET( pcb->network );

        for( EQUIPOT* equipot = aBoard->m_Equipots;  equipot;  equipot = equipot->Next() )
        {
            int netcode = equipot->GetNet();
            if( netcode > 0 )
                nets[ netcode ]->net_id = CONV_TO_UTF8( equipot->m_Netname );
        }

        items.Collect( aBoard, scanMODULEs );

        padstackset.clear();

        for( int m=0;  m<items.GetCount();  ++m )
        {
            MODULE* module = (MODULE*) items[m];

            IMAGE*  image  = makeIMAGE( aBoard, module );

            componentId = CONV_TO_UTF8( module->GetReference() );

            // create a net list entry for all the actual pins in the image
            // for the current module.  location of this code is critical
            // because we fabricated some pin names to ensure unique-ness
            // of pin names within a module, do not move this code because
            // the life of this 'IMAGE* image' is not necessarily long.  The
            // exported netlist will have some fabricated pin names in it.
            // If you don't like fabricated pin names, then make sure all pads
            // within your MODULEs are uniquely named!

            for( unsigned p=0;  p<image->pins.size();  ++p )
            {
                PIN* pin = &image->pins[p];

                int netcode = pin->kiNetCode;
                if( netcode > 0 )
                {
                    NET* net = nets[netcode];

                    net->pins.push_back( empty );

                    PIN_REF& pin_ref = net->pins.back();

                    pin_ref.component_id = componentId;
                    pin_ref.pin_id = pin->pin_id;
                }
            }


            IMAGE*  registered = pcb->library->LookupIMAGE( image );
            if( registered != image )
            {
                // If our new 'image' is not a unique IMAGE, delete it.
                // and use the registered one, known as 'image' after this.
                delete image;
                image = registered;
            }

            COMPONENT* comp = pcb->placement->LookupCOMPONENT( image->GetImageId() );

            PLACE* place = new PLACE( comp );
            comp->places.push_back( place );

            place->SetRotation( module->m_Orient/10.0 );
            place->SetVertex( mapPt( module->m_Pos ) );
            place->component_id = componentId;
            place->part_number  = CONV_TO_UTF8( module->GetValue() );

            // module is flipped from bottom side, set side to T_back
            if( module->flag )
            {
                int angle = 1800 - module->m_Orient;
                NORMALIZE_ANGLE_POS(angle);
                place->SetRotation( angle/10.0 );

                place->side = T_back;
            }
        }

        // copy the SPECCTRA_DB::padstackset to the LIBRARY.  Since we are
        // removing, do not increment the iterator
        for( PADSTACKSET::iterator i=padstackset.begin();  i!=padstackset.end();
                                   i=padstackset.begin() )
        {
            PADSTACKSET::auto_type ps = padstackset.release( i );
            PADSTACK* padstack = ps.release();

            pcb->library->AddPadstack( padstack );
        }

        // copy our SPECCTRA_DB::nets to the pcb->network
        for( unsigned n=1;  n<nets.size();  ++n )
        {
            NET* net = nets[n];
            if( net->pins.size() )
            {
                // give ownership to pcb->network
                pcb->network->nets.push_back( net );
                nets[n] = 0;
            }
        }
    }


    //-----< output the vias >-----------------------------------------------
    {
        // ASSUME:  unique pads are now in the padstack list!  i.e. this code
        // must follow the initial padstack construction code.
        // Next we add the via's which may be used.

        int defaultViaSize = aBoard->m_BoardSettings->m_CurrentViaSize;

        /* I need at least one via for the (class...) scope below
        if( defaultViaSize )
        */
        {
            PADSTACK*   padstack = makeVia( defaultViaSize, g_DesignSettings.m_ViaDrill,
                                           0, aBoard->GetCopperLayerCount()-1 );
            pcb->library->AddPadstack( padstack );

            // remember this index, it is the default via and also the start of the
            // vias within the padstack list.  Before this index are the pads.
            // At this index and later are the vias.
            pcb->library->SetViaStartIndex( pcb->library->padstacks.size()-1 );
        }

        for( int i=0;  i<HISTORY_NUMBER;  ++i )
        {
            int viaSize = aBoard->m_BoardSettings->m_ViaSizeHistory[i];
            if( !viaSize )
                break;

            if( viaSize == defaultViaSize )
                continue;

            PADSTACK*   padstack = makeVia( viaSize, g_DesignSettings.m_ViaDrill,
                                           0, aBoard->GetCopperLayerCount()-1 );
            pcb->library->AddPadstack( padstack );
        }
    }


#if 1  // do existing wires and vias

    //-----<create the wires from tracks>-----------------------------------
    {
        // export all of them for now, later we'll decide what controls we need
        // on this.
        static const KICAD_T scanTRACKs[] = { TYPETRACK, EOT };

        items.Collect( aBoard, scanTRACKs );

        std::string netname;
        WIRING*     wiring = pcb->wiring;
        PATH*       path = 0;

        int old_netcode = -1;
        int old_width = -1;
        int old_layer = -1;

        for( int i=0;  i<items.GetCount();  ++i )
        {
            TRACK*  track = (TRACK*) items[i];

            int     netcode = track->GetNet();

            if( netcode == 0 )
                continue;

            if( old_netcode != netcode
            ||  old_width   != track->m_Width
            ||  old_layer   != track->GetLayer()
            ||  (path && path->points.back() != mapPt(track->m_Start) )
              )
            {
                old_width   = track->m_Width;
                old_layer   = track->GetLayer();

                if( old_netcode != netcode )
                {
                    old_netcode = netcode;
                    EQUIPOT* equipot = aBoard->FindNet( netcode );
                    wxASSERT( equipot );
                    netname = CONV_TO_UTF8( equipot->m_Netname );
                }

                WIRE* wire = new WIRE( wiring );
                wiring->wires.push_back( wire );
                wire->net_id = netname;

                wire->wire_type = T_protect;  // @todo, this should be configurable

                int kiLayer  = track->GetLayer();
                int pcbLayer = kicadLayer2pcb[kiLayer];

                path = new PATH( wire );
                wire->SetShape( path );

                path->layer_id = layerIds[pcbLayer];
                path->aperture_width = scale( old_width );

                path->AppendPoint( mapPt( track->m_Start ) );
            }

            path->AppendPoint( mapPt( track->m_End ) );
        }
    }


    //-----<export the existing real BOARD instantiated vias>-----------------
    {
        // export all of them for now, later we'll decide what controls we need
        // on this.
        static const KICAD_T scanVIAs[] = { TYPEVIA, EOT };

        items.Collect( aBoard, scanVIAs );

        for( int i=0;  i<items.GetCount();  ++i )
        {
            SEGVIA* via = (SEGVIA*) items[i];
            wxASSERT( via->Type() == TYPEVIA );

            int netcode = via->GetNet();
            if( netcode == 0 )
                continue;

            PADSTACK* padstack = makeVia( via );
            PADSTACK* registered = pcb->library->LookupVia( padstack );
            if( padstack != registered )
            {
                delete padstack;
            }

            WIRE_VIA* dsnVia = new WIRE_VIA( pcb->wiring );
            pcb->wiring->wire_vias.push_back( dsnVia );

            dsnVia->padstack_id = registered->padstack_id;
            dsnVia->vertexes.push_back( mapPt( via->GetPosition() ) );

            EQUIPOT* equipot = aBoard->FindNet( netcode );
            wxASSERT( equipot );

            dsnVia->net_id = CONV_TO_UTF8( equipot->m_Netname );

            dsnVia->via_type = T_protect;     // @todo, this should be configurable
        }
    }

#endif  // do existing wires and vias

    //-----<via_descriptor>-------------------------------------------------
    {
        // Output the vias in the padstack list here, by name.  This must
        // be done after exporting existing vias as WIRE_VIAs.
        VIA*        vias = pcb->structure->via;
        PADSTACKS&  padstacks = pcb->library->padstacks;
        int         viaNdx = pcb->library->via_start_index;

        if( viaNdx != -1 )
        {
#if 1
            for(  ; viaNdx < (int)padstacks.size();  ++viaNdx )
            {
                vias->AppendVia( padstacks[viaNdx].padstack_id.c_str() );
            }
#else
            // output only the default via.   Then use class_descriptors to
            // override the default.  No, this causes free router not to
            // output the unmentioned vias into the session file.
            vias->AppendVia( padstacks[viaNdx].padstack_id.c_str() );
#endif
        }
    }


    //-----<output a default class with all nets and the via and track size>--
    {
        char        text[80];
        STRINGSET   netIds;       // sort the net names in here

        CLASS*  clazz = new CLASS( pcb->network );
        pcb->network->classes.push_back( clazz );

        // freerouter creates a class named 'default' anyway, and if we
        // try and use that, we end up with two 'default' via rules so use
        // something else as the name of our default class.   Someday we may support
        // additional classes.  Until then the user can text edit the exported
        // DSN file and use this class as a template, copying it and giving the
        // copy a different class_id and splitting out some of the nets.
        clazz->class_id = "kicad_default";

        // Insert all the net_ids into the set.  They are unique, but even if
        // they were not the duplicated name is not our error, but the BOARD's.
        // A duplicate would be removed here.
        NETS& nets = pcb->network->nets;
        for( NETS::iterator i=nets.begin();  i!=nets.end();  ++i )
            netIds.insert( i->net_id );

        // netIds is now sorted, put them into clazz->net_ids
        for( STRINGSET::iterator i=netIds.begin();  i!=netIds.end();  ++i )
            clazz->net_ids.push_back( *i );

        // output the via and track dimensions, the whole reason for this scope.
        int     curTrackWidth = aBoard->m_BoardSettings->m_CurrentTrackWidth;

        clazz->rules = new RULE( clazz, T_rule );

        sprintf( text, "(width %.6g)", scale( curTrackWidth ) );
        clazz->rules->rules.push_back( text );

        int         viaNdx = pcb->library->via_start_index;

        sprintf( text, "(use_via %s)", pcb->library->padstacks[viaNdx].padstack_id.c_str() );
        clazz->circuit.push_back( text );
    }
}


void SPECCTRA_DB::FlipMODULEs( BOARD* aBoard )
{
    for( MODULE* module = aBoard->m_Modules;  module;  module = module->Next() )
    {
        module->flag = 0;
        if( module->GetLayer() == COPPER_LAYER_N )
        {
            aBoard->Change_Side_Module( module, NULL );
            module->flag = 1;
        }
    }

    modulesAreFlipped = true;
}


void SPECCTRA_DB::RevertMODULEs( BOARD* aBoard )
{
    if( !modulesAreFlipped )
        return;

    //  DSN Images (=Kicad MODULES and pads) must be presented from the
    //  top view.  Restore those that were flipped.
    for( MODULE* module = aBoard->m_Modules;  module;  module = module->Next() )
    {
        if( module->flag )
        {
            aBoard->Change_Side_Module( module, NULL );
            module->flag = 0;
        }
    }

    modulesAreFlipped = false;
}


}       // namespace DSN
