/***********/
/* protos.h */
/***********/

#ifndef PROTO_H
#define PROTO_H


#include <vector>


class COMMAND;

/** Function SwapData
 * Used in undo / redo command:
 *  swap data between Item and a copy
 *  swapped data is data modified by edition, so NOT ALL values are swapped
 * @param aItem = the item
 * @param aImage = a copy of the item
 */
void SwapData( BOARD_ITEM* aItem, BOARD_ITEM* aImage );


/* install function for DialogNonCopperZonesEditor dialog frame :*/
bool InstallDialogNonCopperZonesEditor( WinEDA_PcbFrame* aParent,
                                        ZONE_CONTAINER*  aZone );

/*******************/
/* PAD_CONNECT.CPP */
/*******************/

class D_PAD;

/**
 * Function CreateSortedPadListByXCoord
 * first empties then fills the vector with all pads and sorts them by
 * increasing x coordinate.  The vector only holds pointers to the pads and
 * those pointers are only references to pads which are owned by the BOARD
 * through other links.
 * @param aBoard Which board to gather pads from.
 * @param aVector Where to put the pad pointers.
 */
void CreateSortedPadListByXCoord( BOARD* aBoard, std::vector<D_PAD*>* aVector );


/* Create a sorted list of pointers to pads.
 * This list is sorted by X coordinate value.
 * The list must be freed bu user
 */

/**************/
/* PCBCFG.CPP */
/**************/
bool Read_Hotkey_Config( WinEDA_DrawFrame* frame, bool verbose );


/***************/
/* TRPISTE.CPP */
/***************/

/* Routine trace of n consecutive segments in memory.
 * Useful for mounting a track record for being the segments that
 * Tracks are contiguous in memory
 * Parameters:
 * Pt_start_piste = starting address of the list of segments
 * Nbsegment = number of segments was traced
 * Mode_color = mode (GrXOR, Gror ..)
 * CAUTION:
 * The starting point of a track following MUST exist: may be
 * Then put a 0 before calling a routine if the track is the last draw
 */
void Trace_Une_Piste( WinEDA_DrawPanel* panel,
                      wxDC*             DC,
                      TRACK*            pt_start_piste,
                      int               nbsegment,
                      int               mode_color );


/****************/
/* LOCATE.CPP : */
/****************/

/* Find a pad by it's name om the module. */
TRACK*       Locate_Via( BOARD* Pcb, const wxPoint& pos, int layer = -1 );

/**
 * Function Locate_Via_Area
 * finds the first SEGVIA which covers the given aPos with a matching layer.
 * @param aStart The starting TRACK or SEGVIA in the BOARD's list.
 * @param aPos The wxPoint to HitTest() against.
 * @param aLayer The layer to match, pass -1 for a don't care.
 * @return TRACK* - actually a SEGVIA* if found, else NULL.
 */
TRACK*       Locate_Via_Area( TRACK*         aStart,
                              const wxPoint& aPos,
                              int            aLayer = -1 );

/* Locates the center through the point x, y, on layer data
 * by masquelayer.
 * Search is done to address start_adr has end_adr (not included)
 */
TRACK*       Fast_Locate_Via( TRACK* start_adr, TRACK* end_adr,
                              const wxPoint& pos, int masquelayer );

/* Locates the center through the point x, y, on layer data
 * by masquelayer.
 * Search is done to address start_adr has end_adr (not included)
 */
TRACK*       Fast_Locate_Piste( TRACK* start_adr, TRACK* end_adr,
                                const wxPoint& ref_pos, int masquelayer );

/* Search for segment connected to the segment edge by
 * Ptr_piste:
 * If int = START, the point of beginning of the segment is used
 * If int = END, the end point of the segment is used
 * The search is done only on the ends of segments
 * The search is limited to the area [... pt_base] pt_lim.
 */
TRACK*       Locate_Piste_Connectee( TRACK* ptr_piste, TRACK* pt_base,
                                     TRACK* pt_lim, int extr );

/*
 * 1 - Locate segment of track leading from the mouse.
 * 2 - Locate segment of track point by point.
 * Ref_pX, ref_pY.
 *
 * If layer <0 the layer is not tested.
 *
 * The search begins to address start_adresse
 */
TRACK*       Locate_Pistes( BOARD* aPcb, TRACK* start_adresse, int layer, int typeloc );
TRACK*       Locate_Pistes( BOARD* aPcb, TRACK* start_adresse,
                            const wxPoint& ref_pos, int layer );

/* Locate pad connected to the beginning or end of a segment
 * Input: pointer to the segment, and flag = START or END
 * Returns:
 * A pointer to the description of the patch if pad was found.
 * NULL pointer if pad was not found.
 */
D_PAD*       Locate_Pad_Connecte( BOARD* aPcb, TRACK* ptr_segment, int extr );


/*
 * Locate pad pointed to by the coordinate ref_pX,, ref_pY or the current
 * cursor position, search done on all tracks.
 * Entry:
 * - Mouse coord (Curseur_X and Curseur_Y)
 * Or ref_pX, ref_pY
 * Returns:
 * Pointer to the pad if found
 * NULL pointer if pad not found
 */
D_PAD*       Locate_Any_Pad( BOARD* aPcb,
                             int    typeloc,
                             bool   OnlyCurrentLayer = FALSE );
D_PAD*       Locate_Any_Pad( BOARD*         aPcb,
                             const wxPoint& ref_pos,
                             bool           OnlyCurrentLayer = FALSE );

/* Locate pad pointed to by the coordinate ref_pX,, ref_pY or the cursor
 * position of the current footprint.
 * Input:
 * - The module to search.
 * - Layer to search or -1 to search all layers.
 * Returns:
 * A pointer to the pad if found otherwise NULL.
 */
D_PAD*       Locate_Pads( MODULE* Module, int layer, int typeloc );
D_PAD*       Locate_Pads( MODULE* Module, const wxPoint& ref_pos, int layer );

/* Locate a footprint by its bounding rectangle. */
MODULE*      Locate_Prefered_Module( BOARD* Pcb, int typeloc );

/* Locate a pad pointed to by the cursor on the footprint.
 * Module.
 * Input:
 * - Module to search.
 * Returns:
 * A pointer to the pad if found otherwise NULL.
 */
D_PAD*       Locate_Pads( MODULE* Module, int typeloc );

/* Locate a trace segment at the current cursor position.
 * The search begins to address start_adresse.
 */
TRACK*       Locate_Pistes( TRACK* start_adresse, int typeloc );

DRAWSEGMENT* Locate_Segment_Pcb( BOARD* Pcb, int LayerSearch, int typeloc );

/* Locate pad containing the point px, py, layer on layer.
 *
 * The list of pads must already exist.
 *
 * Returns:
 * Pointer to the pad if found, otherwise NULL.
 */
D_PAD*       Fast_Locate_Pad_Connecte( BOARD*         Pcb,
                                       const wxPoint& ref_pos,
                                       int            layer );

/*
 * 1 - Locate trace segment at the current cursor position.
 * 2 - Locate trace segment at the given coordinates ref_pos.
 *
 * If layer == -1, check all layers.
 *
 * The search begins to address start_adresse
 */
TRACK*       Locate_Zone( TRACK* start_adresse, int layer, int typeloc );
TRACK*       Locate_Zone( TRACK*         start_adresse,
                          const wxPoint& ref_pos,
                          int            layer );


/*************/
/* MODULES.C */
/*************/
int  ChangeSideNumLayer( int oldlayer );
void DrawModuleOutlines( WinEDA_DrawPanel* panel, wxDC* DC, MODULE* module );
void Montre_Position_Empreinte( WinEDA_DrawPanel* panel,
                                wxDC*             DC,
                                bool              erase );


/* LOADCMP.C : */
MODULE* Load_Module_From_Library( WinEDA_DrawFrame* frame, wxDC* DC );


/****************/
/* EDITRACK.C : */
/****************/

TRACK* LocateIntrusion( TRACK* listStart, TRACK* aTrack );

void   ShowNewTrackWhenMovingCursor( WinEDA_DrawPanel* panel,
                                     wxDC* DC, bool erase );

/* Determine coordinate for a segment direction of 0, 90 or 45 degrees,
 * depending on it's position from the origin (ox, oy) and the current
 * cursor position.
 */
void   Calcule_Coord_Extremite_45( int ox, int oy, int* fx, int* fy );


/*****************/
/* TRACK.CPP : */
/*****************/

/**
 * Function Marque_Une_Piste
 * marks a chain of track segments, connected to aTrackList.
 * Each segment is marked by setting the BUSY bit into m_Flags.  Electrical
 * continuity is detected by walking each segment, and finally the segments
 * are rearranged into a contiguous chain within the given list.
 *
 * @param aPcb = the board to analyze
 * @param aStartSegm - The first interesting segment within a list of track
 *                     segment of aPcb
 * @param aSegmCount = a pointer to an integer where to return the number of
 *                     interesting segments
 * @param aTrackLen = a pointer to an integer where to return the lenght of the
 *                    track
 * @param aReorder = true for reorder the interesting segments (useful for
 *                   track edition/deletion) in this case the flag BUSY is
 *                   set (the user is responsible of flag clearing). False
 *                   for no reorder : useful when we want just calculate the
 *                   track length in this case, flags are reset
 * @return TRACK* the first in the chain of interesting segments.
 */
TRACK* Marque_Une_Piste( BOARD* aPcb,
                         TRACK* aStartSegm,
                         int*   aSegmCount,
                         int*   aTrackLen,
                         bool   aReorder );

/* Calculate end  coordinate of a trace.
 * Returns 1 if OK, 0 if trace looped back on itself.
 * The coord are returned StartTrack-> ox, oy
 * And EndTrack-> fx, fy if OK
 * The segments are drawn consecutively.
 */
int  ReturnEndsTrack( TRACK* RefTrack, int NbSegm,
                      TRACK** StartTrack, TRACK** EndTrack );

/* Update the state of a list of structures. */
void ListSetState( EDA_BaseStruct* Start, int Nbitem, int State, int onoff );


/**************/
/* CLEAN.CPP : */
/**************/

/* Remove segments connected incorrectly.
 */
int Netliste_Controle_piste( WinEDA_PcbFrame* frame, wxDC* DC, int affiche );


/************/
/* ZONES.CPP */
/************/
int Propagation( WinEDA_PcbFrame* frame );

/****************/
/* ATTRIBUT.CPP */
/****************/

/* Compute the attributes that are 0 (masque_clr) and put a 1
 * (Masque_set), depending on the options attribute.
 *
 * These attributes are normally the member flags of the structure TRACK
 * Pointers NULLs are accepted.
 */
void MasqueAttributs( int* masque_set, int* masque_clr );


/***************/
/* DUPLTRAC.CPP */
/***************/

/* Routine to find the point "attachment" at the end of a trace.
 * This may be a PAD or another trace segment.
 * Returns:
 * - Pointer to the PAD or:
 * - Pointer to the segment or:
 * - NULL
 * Parameters:
 * - aPos - coordinate point test
 * ALayerMask of mask layers to be tested
 */
BOARD_ITEM* LocateLockPoint( BOARD* aPcb, wxPoint aPos, int aLayerMask );

/* Create an intermediate point on a segment
 * aSegm segment is broken into 2 segments connecting point pX, pY
 * After insertion:
 *   The new segment starts from  to new point, and ends to initial aSegm ending point
 *   the old segment aSegm ends to new point
 * Returns:
 *   NULL if no new point (ie if aRefPoint already corresponded at one end of aSegm
 * or
 *   Pointer to the segment created
 *   Returns the exact value of aRefPoint
 * If aSegm points to a via:
 *   Returns the exact value of aRefPoint and a pointer to the via,
 *   But does not create extra point
 */
TRACK*      CreateLockPoint( BOARD* aPcb,
                             wxPoint&           aRefPoint,
                             TRACK*             aSegm,
                             PICKED_ITEMS_LIST* aItemsListPicker );


/****************/
/* CONTROLE.CPP */
/****************/
void RemoteCommand( const char* cmdline );
bool Project( wxPoint* res, wxPoint on_grid, const TRACK* track );


/***************/
/* AUTOROUT.CPP */
/***************/
void DisplayBoard( WinEDA_DrawPanel* panel, wxDC* DC );    /* for Debugging */


/**************/
/* NETLIST.CPP */
/**************/

/* List the names of the modules of PCB
 * Returns a pointer to the module selected or NULL if no selection.
 */
MODULE* ListAndSelectModuleName( COMMAND* Cmd );


/***************************/
/* DIALOG_LAYERS_SETUP.CPP */
/***************************/

void DisplayDialogLayerSetup( WinEDA_PcbFrame* parent );

#endif  /* #define PROTO_H */
