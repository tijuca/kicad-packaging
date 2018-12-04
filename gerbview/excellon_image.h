/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2011-2016 Jean-Pierre Charras  jp.charras at wanadoo.fr
 * Copyright (C) 1992-2016 KiCad Developers, see AUTHORS.txt for contributors.
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

#ifndef EXCELLON_IMAGE_H
#define EXCELLON_IMAGE_H


enum drill_M_code_t {
    DRILL_M_UNKNOWN,
    DRILL_M_END,
    DRILL_M_TOOL_DOWN,      // tool down (starting a routed hole)
    DRILL_M_TOOL_UP,        // tool up (ending a routed hole)
    DRILL_M_ENDREWIND,
    DRILL_M_MESSAGE,
    DRILL_M_LONGMESSAGE,
    DRILL_M_HEADER,
    DRILL_M_ENDHEADER,
    DRILL_M_BEGINPATTERN,
    DRILL_M_ENDPATTERN,
    DRILL_M_CANNEDTEXT,
    DRILL_M_TIPCHECK,
    DRILL_M_METRIC,
    DRILL_M_IMPERIAL,
    DRILL_METRIC_HEADER,
    DRILL_IMPERIAL_HEADER,
    DRILL_DETECT_BROKEN,
    DRILL_INCREMENTALHEADER,
    DRILL_REWIND_STOP,
    DRILL_TOOL_CHANGE_STOP,
    DRILL_AUTOMATIC_SPEED,
    DRILL_AXIS_VERSION,
    DRILL_RESET_CMD,
    DRILL_AUTOMATIC_TOOL_CHANGE,
    DRILL_FMT,
    DRILL_SKIP,
    DRILL_TOOL_INFORMATION,
    DRILL_M_END_LIST                // not used: sentinel
};


enum drill_G_code_t {
    DRILL_G_UNKNOWN = DRILL_M_END_LIST+1,     // Use next available value
    DRILL_G_ABSOLUTE,
    DRILL_G_INCREMENTAL,
    DRILL_G_ZEROSET,
    DRILL_G_ROUT,
    DRILL_G_DRILL,
    DRILL_G_SLOT,
    DRILL_G_ZERO_SET,
    DRILL_G_LINEARMOVE,
    DRILL_G_CWMOVE,
    DRILL_G_CCWMOVE
};

// Helper struct to analyse Excellon commands
struct EXCELLON_CMD
{
    std::string m_Name;     // key string
    int    m_Code;          // internal code, used as id in functions
    int    m_asParams;      // 0 = no param, -1 = skip params, 1 = read params
};

// Helper struct to store Excellon points in routing mode
#define ROUTE_CCW 1
#define ROUTE_CW -1

struct EXCELLON_ROUTE_COORD
{
    int m_x;        // X coordinate
    int m_y;        // y coordinate
    int m_cx;       // center X coordinate in circular routing mode
                    // (when the IJ commad is used)
    int m_cy;       // center y coordinate in circular routing mode
                    // (when the IJ commad is used)
    int m_radius;   // radius in circular routing mode (when the A## command is used)
    int m_rmode;    // routing mode: 0 = circular, ROUTE_CCW (1) = ccw, ROUTE_CW (-1) = cw
    int m_arc_type_info; // arc using radius or center coordinates

    EXCELLON_ROUTE_COORD():
        m_x( 0 ),  m_y( 0 ), m_cx( 0 ), m_cy( 0 ), m_radius( 0 ),
        m_rmode( 0 ), m_arc_type_info( 0 )
    {}

    EXCELLON_ROUTE_COORD( const wxPoint& aPos ):
        m_x( aPos.x ),  m_y( aPos.y ),
        m_cx( 0 ), m_cy( 0 ), m_radius( 0 ), m_rmode( 0 ),
        m_arc_type_info( ARC_INFO_TYPE_NONE )
    {}

    EXCELLON_ROUTE_COORD( const wxPoint& aPos, const wxPoint& aCenter, int aMode ):
        m_x( aPos.x ),  m_y( aPos.y ),
        m_cx( aCenter.x ), m_cy( aCenter.y ), m_radius( 0 ),  m_rmode( aMode ),
        m_arc_type_info( ARC_INFO_TYPE_CENTER )
    {}

    EXCELLON_ROUTE_COORD( const wxPoint& aPos, int aRadius, int aMode ):
        m_x( aPos.x ),  m_y( aPos.y ),
        m_cx( 0 ), m_cy( 0 ), m_radius( aRadius ),  m_rmode( aMode ),
        m_arc_type_info( ARC_INFO_TYPE_RADIUS )
    {}

    wxPoint GetPos() { return wxPoint( m_x, m_y ); }
};

/* EXCELLON_IMAGE handle a drill image
 *  It is derived from GERBER_FILE_IMAGE because there is a lot of likeness
 *  between EXCELLON files and GERBER files
 *  DCode aperture are also similat to T Codes.
 *  So we can reuse GERBER_FILE_IMAGE to handle EXCELLON_IMAGE with very few new functions
 */

class EXCELLON_IMAGE : public GERBER_FILE_IMAGE
{
private:
    enum excellon_state {
        READ_HEADER_STATE,          // When we are in this state, we are reading header
        READ_PROGRAM_STATE          // When we are in this state, we are reading drill data
    };

    excellon_state m_State;         // state of excellon file analysis
    bool           m_SlotOn;        // true during an oblong drill definition
                                    // by G85 (canned slot) command
    bool           m_RouteModeOn;   // true during a route mode (for instance a oval hole) or a cutout
    std::vector<EXCELLON_ROUTE_COORD> m_RoutePositions;  // The list of points in a route mode

public: EXCELLON_IMAGE( int layer ) :
        GERBER_FILE_IMAGE( layer )
    {
        m_State  = READ_HEADER_STATE;
        m_SlotOn = false;
        m_RouteModeOn = false;
    }


    ~EXCELLON_IMAGE() {};

    virtual void ResetDefaultValues() override
    {
        GERBER_FILE_IMAGE::ResetDefaultValues();
        SelectUnits( false );
    }


    /**
     * Read and load a drill (EXCELLON format) file.
     * @param aFullFileName = the full filename of the Gerber file
     * when the file cannot be loaded
     * Warning and info messages are stored in m_Messages
     * @return bool if OK, false if the gerber file was not loaded
     */
    bool LoadFile( const wxString& aFullFileName );

private:
    bool Execute_HEADER_And_M_Command( char*& text );
    bool Select_Tool( char*& text );
    bool Execute_EXCELLON_G_Command( char*& text );
    bool Execute_Drill_Command( char*& text );

    /** Read a tool definition like T1C0.02 or T1F00S00C0.02 or T1C0.02F00S00
     * and enter params in TCODE list
     */
    bool readToolInformation( char*& aText );

    int TCodeNumber( char*& aText )
    {
        return DCodeNumber( aText );
    }


    void SelectUnits( bool aMetric );
};


/*
 *  EXCELLON commands are given here.
 *  Pcbnew uses only few excellon commands
 */

/*
 *  see http://www.excellon.com/manuals/program.htm
 */

/* coordintes units:
 *  Coordinates are measured either in inch or metric (millimeters).
 *  Inch coordinates are in six digits (00.0000) with increments as small as 0.0001 (1/10,000).
 *  Metric coordinates can be measured in microns (thousandths of a millimeter)
 *  in one of the following three ways:
 *   Five digit 10 micron resolution (000.00)
 *   Six digit 10 micron resolution (0000.00)
 *   Six digit micron resolution (000.000)
 *
 *  Leading and trailing zeros:
 *  Excellon (CNC-7) uses inches in six digits and metric in five or six digits.
 *  The zeros to the left of the coordinate are called leading zeros (LZ).
 *  The zeros to right of the coordinate are called trailing zeros (TZ).
 *  The CNC-7 uses leading zeros unless you specify otherwise through a part program.
 *  You can do so with the INCH/METRIC command.
 *  With leading zeros, the leading zeros must always be included.
 *  Trailing zeros are unneeded and may be left off.
 *  For trailing zeros, the reverse of the above is true.
 */

/*
 *  EXCELLON Commands Used in a Header
 *  The following table provides you with a list of commands which
 *  are the most used in a part program header.
 *  COMMAND         DESCRIPTION
 *  AFS	            Automatic Feeds and Speeds
 *  ATC             Automatic Tool Change
 *  BLKD            Delete all Blocks starting with a slash (/)
 *  CCW             Clockwise or Counter-clockwise Routing
 *  CP              Cutter Compensation
 *  DETECT          Broken Tool Detection
 *  DN              Down Limit Set
 *  DTMDIST         Maximum Rout Distance Before Toolchange
 *  EXDA            Extended Drill Area
 *  FMAT            Format 1 or 2
 *  FSB             Turns the Feed/Speed Buttons off
 *  HPCK            Home Pulse Check
 *  ICI             Incremental Input of Part Program Coordinates
 *  INCH            Measure Everything in Inches
 *  METRIC          Measure Everything in Metric
 *  M48             Beginning of Part Program Header
 *  M95             End of Header
 *  NCSL            NC Slope Enable/Disable
 *  OM48            Override Part Program Header
 *  OSTOP           Optional Stop Switch
 *  OTCLMP          Override Table Clamp
 *  PCKPARAM        Set up pecking tool,depth,infeed and retract parameters
 *  PF              Floating Pressure Foot Switch
 *  PPR             Programmable Plunge Rate Enable
 *  PVS             Pre-vacuum Shut-off Switch
 *  R,C             Reset Clocks
 *  R,CP            Reset Program Clocks
 *  R,CR            Reset Run Clocks
 *  R,D             Reset All Cutter Distances
 *  R,H             Reset All Hit Counters
 *  R,T             Reset Tool Data
 *  SBK             Single Block Mode Switch
 *  SG              Spindle Group Mode
 *  SIXM            Input From External Source
 *  T               Tool Information
 *  TCST            Tool Change Stop
 *  UP              Upper Limit Set
 *  VER             Selection of X and Y Axis Version
 *  Z               Zero Set
 *  ZA              Auxiliary Zero
 *  ZC              Zero Correction
 *  ZS              Zero Preset
 *  Z+# or Z-#      Set Depth Offset
 *  %               Rewind Stop
 *  #/#/#           Link Tool for Automatic Tool Change
 *  /               Clear Tool Linking
 */

/*
 *  Beyond The Header: The Part Program Body
 *  COMMAND         DESCRIPTION
 *  A#              Arc Radius
 *  B#              Retract Rate
 *  C#              Tool Diameter
 *  F#              Table Feed Rate;Z Axis Infeed Rate
 *  G00X#Y#         Route Mode; XY is the starting point
 *  G01X#Y#         Linear (Straight Line) Route Mode YX is the ending point
 *  G02X#Y#...      Circular CW Mode. Radius value (A#) or Center position (I#J#) follows
 *  G03X#Y#...      Circular CCW Mode. Radius value (A#) or Center position (I#J#) follows
 *  G04	X#          Variable Dwell
 *  G05             Drill Mode
 *  G07             Override current tool feed or speed
 *  G32X#Y#A#       Routed Circle Canned Cycle
 *  CW G33X#Y#A#    Routed Circle Canned Cycle
 *  CCW G34,#(,#)   Select Vision Tool
 *  G35(X#Y#)       Single Point Vision Offset (Relative to Work Zero)
 *  G36(X#Y#)       Multipoint Vision Translation (Relative to Work Zero)
 *  G37             Cancel Vision Translation or Offset (From G35 or G36)
 *  G38(X#Y#)       Vision Corrected Single Hole Drilling (Relative to Work Zero)
 *  G39(X#Y#)       Vision System Autocalibration
 *  G40             Cutter Compensation Off
 *  G41             Cutter Compensation Left
 *  G42             Cutter Compensation Right
 *  G45(X#Y#)       Single Point Vision Offset (Relative to G35 or G36)
 *  G46(X#Y#)       Multipoint Vision Translation (Relative to G35 or G36)
 *  G47             Cancel Vision Translation or Offset (From G45 or G46)
 *  G48(X#Y#)       Vision Corrected Single Hole Drilling (Relative to G35 or G36)
 *  G82(G81)        Dual In Line Package
 *  G83             Eight Pin L Pack
 *  G84             Circle
 *  G85             Slot
 *  G87             Routed Step Slot Canned Cycle
 *  G90             Absolute Mode
 *  G91             Incremental Input Mode
 *  G93X#Y#         Zero Set
 *  H#              Maximum hit count
 *  I#J#            Arc Center Offset
 *  M00(X#Y#)       End of Program - No Rewind
 *  M01             End of Pattern
 *  M02X#Y#         Repeat Pattern Offset
 *  M06(X#Y#)       Optional Stop
 *  M08             End of Step and Repeat
 *  M09(X#Y#)       Stop for Inspection
 *  M14             Z Axis Route Position With Depth Controlled Contouring
 *  M15             Z Axis Route Position
 *  M16             Retract With Clamping
 *  M17             Retract Without Clamping
 *  M18             Command tool tip check
 *  M25             Beginning of Pattern
 *  M30(X#Y#)       End of Program Rewind
 *  M45,long message\   Long Operator message on multiple\ part program lines
 *  M47,text        Operator Message
 *  M50,#           Vision Step and Repeat Pattern Start
 *  M51,#           Vision Step and Repeat Rewind
 *  M52(#)          Vision Step and Repeat Offset Counter Control
 *  M02XYM70        Swap Axes
 *  M60             Reference Scaling enable
 *  M61             Reference Scaling disable
 *  M62             Turn on peck drilling
 *  M63             Turn off peck drilling
 *  M71             Metric Measuring Mode
 *  M72             Inch Measuring Mode
 *  M02XYM80        Mirror Image X Axis
 *  M02XYM90        Mirror Image Y Axis
 *  M97,text        Canned Text
 *  M98,text        Canned Text
 *  M99,subprogram  User Defined Stored Pattern
 *  P#X#(Y#)        Repeat Stored Pattern
 *  R#M02X#Y#       Repeat Pattern (S&R)
 *  R#(X#Y#)        Repeat Hole
 *  S#              Spindle RPM
 *  T#              Tool Selection; Cutter Index
 *  Z+# or Z-#      Depth Offset
 *  %               Beginning of Pattern (see M25 command)
 *  /               Block Delete
 */

/*
 *  Example of a Header
 *  COMMAND         PURPOSE
 *  M48             The beginning of a header
 *  INCH,LZ         Use the inch measuring system with leading zeros
 *  VER,1           Use Version 1 X and Y axis layout
 *  FMAT,2          Use Format 2 commands
 *  1/2/3           Link tools 1, 2, and 3
 *  T1C.04F200S65   Set Tool 1 for 0.040" with infeed rate of 200 inch/min Speed of 65,000 RPM
 *  DETECT,ON       Detect broken tools
 *  M95             End of the header
 */

#endif  // EXCELLON_IMAGE_H
