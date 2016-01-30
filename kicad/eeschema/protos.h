
#ifndef __PROTOS_H__
#define __PROTOS_H__


class EDA_BaseStruct;
class WinEDA_DrawPanel;
class WinEDA_SchematicFrame;
class LIB_COMPONENT;
class LIB_DRAW_ITEM;
class SCH_COMPONENT;
class BASE_SCREEN;
class SCH_SCREEN;
class SCH_ITEM;
class SCH_SHEET_PIN;
class PLOTTER;
class SCH_SHEET;
class LIB_PIN;


wxString ReturnDefaultFieldName( int aFieldNdx );


/****************/
/* DATABASE.CPP */
/****************/
void DisplayCmpDoc( wxString& Name );
wxString DataBaseGetName( WinEDA_DrawFrame* frame, wxString& Keys,
                          wxString& BufName );

/*********************/
/* DANGLING_ENDS.CPP */
/*********************/
bool SegmentIntersect( int Sx1, int Sy1, int Sx2, int Sy2, int Px1, int Py1 );

/****************/
/* BUS_WIRE_JUNCTION.CPP */
/****************/
void IncrementLabelMember( wxString& name );

/****************/
/* EDITPART.CPP */
/****************/
void InstallCmpeditFrame( WinEDA_SchematicFrame* parent, wxPoint& pos,
                          SCH_COMPONENT* m_Cmp );


bool MapAngles( int*      Angle1,
                int*      Angle2,
                const int TransMat[2][2] );


/**
 * Calculate new coordinate according to the transform matrix.
 *
 * @param aTransformMatrix = rotation, mirror .. matrix
 * @param aPosition = the position to transform
 * @return the new coordinate
 */
wxPoint        TransformCoordinate( const int      aTransformMatrix[2][2],
                                    const wxPoint& aPosition );

void           SnapLibItemPoint( int            OrigX,
                                 int            OrigY,
                                 int*           ClosestX,
                                 int*           ClosestY,
                                 SCH_COMPONENT* DrawLibItem );
bool           LibItemInBox( int x1, int y1, int x2, int y2,
                             SCH_COMPONENT* DrawLibItem );

/************/
/* BLOCK.CPP */
/************/
void      DeleteStruct( WinEDA_DrawPanel* panel,
                        wxDC*             DC,
                        SCH_ITEM*         DrawStruct );

// operations_on_item_lists.cpp
/** function DuplicateStruct
 *  Routine to create a new copy of given struct.
 * @param aDrawStruct = the SCH_ITEM to duplicate
 * @param aClone (defualt = true)
 *     if true duplicate also some parameters that must be unique
 *     (timestamp and sheet name)
 *      aClone must be false. use true only is undo/redo duplications
 */
SCH_ITEM* DuplicateStruct( SCH_ITEM* DrawStruct, bool aClone = false );

/*************/
/* LOCATE.CPP */
/*************/

SCH_COMPONENT*  LocateSmallestComponent( SCH_SCREEN* Screen );

/* Find the item within block selection. */
int             PickItemsInBlock( BLOCK_SELECTOR& aBlock,
                                  BASE_SCREEN*    screen );

/* function PickStruct:
 *   Search at location pos
 *
 *   SearchMask = (bitwise OR):
 *   LIBITEM
 *   WIREITEM
 *   BUSITEM
 *   RACCORDITEM
 *   JUNCTIONITEM
 *   DRAWITEM
 *   TEXTITEM
 *   LABELITEM
 *   SHEETITEM
 *   MARKERITEM
 *   NOCONNECTITEM
 *   SEARCH_PINITEM
 *   SHEETLABELITEM
 *   FIELDCMPITEM
 *
 *   if EXCLUDE_WIRE_BUS_ENDPOINTS is set, in wire ou bus search and locate,
 *   start and end points are not included in search
 *   if WIRE_BUS_ENDPOINTS_ONLY is set, in wire ou bus search and locate,
 *   only start and end points are included in search
 *
 *
 *   Return:
 *     Pointer to list of pointers to structures if several items are selected.
 *     Pointer to the structure if only 1 item is selected.
 *     NULL if no items are selects.
 */
SCH_ITEM*          PickStruct( const wxPoint& refpos,
                               BASE_SCREEN*   screen,
                               int            SearchMask );


SCH_SHEET_PIN* LocateSheetLabel( SCH_SHEET* Sheet, const wxPoint& pos );
LIB_PIN*                       LocateAnyPin( SCH_ITEM*       DrawList,
                                             const wxPoint&  RefPos,
                                             SCH_COMPONENT** libpart = NULL );

SCH_SHEET_PIN* LocateAnyPinSheet( const wxPoint& RefPos,
                                  SCH_ITEM*      DrawList );


/***************/
/* EEREDRAW.CPP */
/***************/
void DrawDanglingSymbol( WinEDA_DrawPanel* panel, wxDC* DC,
                         const wxPoint& pos, int Color );

void DrawStructsInGhost( WinEDA_DrawPanel* aPanel,
                         wxDC*             aDC,
                         SCH_ITEM*         aItem,
                         const wxPoint&    aOffset );
void SetHighLightStruct( SCH_ITEM* HighLight );
void RedrawActiveWindow( WinEDA_DrawPanel* panel, wxDC* DC );
void RedrawStructList( WinEDA_DrawPanel* panel,
                       wxDC*             DC,
                       SCH_ITEM*         Structs,
                       int               DrawMode,
                       int               Color = -1 );
void RedrawOneStruct( WinEDA_DrawPanel* panel,
                      wxDC*             DC,
                      SCH_ITEM*         Struct,
                      int               DrawMode,
                      int               Color = -1 );

/**************/
/* EELAYER.CPP */
/**************/
void       SeedLayers();
EDA_Colors ReturnLayerColor( int Layer );
void       DisplayColorSetupFrame( WinEDA_DrawFrame* parent,
                                   const wxPoint&    pos );

/*************/
/* EELOAD.CPP */
/*************/
int  CountCmpNumber();

/***************/
/* EECONFIG.CPP */
/***************/
bool Read_Hotkey_Config( WinEDA_DrawFrame* frame, bool verbose );


/**************/
/* NETLIST.CPP */
/**************/
int  IsBusLabel( const wxString& LabelDrawList );

/***************/
/* ANNOTATE.CPP */
/***************/
void ReAnnotatePowerSymbolsOnly();


/************/
/* PLOT.CPP */
/************/
void PlotDrawlist( PLOTTER* plotter, SCH_ITEM* drawlist );

/***************/
/* DELSHEET.CPP */
/***************/
void DeleteSubHierarchy( SCH_SHEET* Sheet, bool confirm_deletion );
bool ClearProjectDrawList( SCH_SCREEN* FirstWindow, bool confirm_deletion );

/* free the draw list screen->EEDrawList and the subhierarchies
 *   clear the screen datas (filenames ..)
 */

/*************/
/* DELETE.CPP */
/*************/

bool LocateAndDeleteItem( WinEDA_SchematicFrame* frame, wxDC* DC );
void EraseStruct( SCH_ITEM* DrawStruct, SCH_SCREEN* Window );
void DeleteAllMarkers( int type );


/**************/
/* PINEDIT.CPP */
/**************/
void InstallPineditFrame( WinEDA_LibeditFrame* parent,
                          wxDC*                DC,
                          const wxPoint&       pos );


/**************/
/* SELPART.CPP */
/**************/

/**
 * Function DisplayComponentsNamesInLib
 * Select component from list of components in this library
 *
 * If == NULL Library, selection of library REQUESTED
 * If only in research library
 *
 * Returns
 *   1 if selected component
 *   0 if canceled order
 */
int            DisplayComponentsNamesInLib( WinEDA_DrawFrame* frame,
                                            CMP_LIBRARY*      Library,
                                            wxString&         Buffer,
                                            wxString&         OldName );

/**
 * Function SelectLibraryFromList
 * displays a list of current loaded libraries, and allows the user to select
 * a library
 * This list is sorted, with the library cache always at end of the list
 */
CMP_LIBRARY* SelectLibraryFromList( WinEDA_DrawFrame* frame );

/**
 * Get the name component from a library to load.
 *
 * If no library specified, there will be demand for selection of a library.
 * Returns
 *   1 if selected component
 *   0 if canceled order
 * Place the name of the selected component list in BufName
 */
int            GetNameOfPartToLoad( WinEDA_DrawFrame* frame,
                                    CMP_LIBRARY*      Lib,
                                    wxString&         BufName );

/**************/
/* LIBARCH.CPP */
/**************/

bool LibArchive( wxWindow* frame, const wxString& ArchFullFileName );

/**************/
/* CLEANUP.CPP */
/**************/

void SchematicCleanUp( SCH_SCREEN* screen, wxDC* DC );

/* Routine de nettoyage:
 *    - regroupe les segments de fils (ou de bus) alignes en 1 seul segment
 *    - Detecte les objets identiques superposes
 */

void BreakSegmentOnJunction( SCH_SCREEN* Screen );

/* Break a segment ( BUS, WIRE ) int 2 segments at location aBreakpoint,
 * if aBreakpoint in on segment segment
 * ( excluding ends)
 */
void BreakSegment(SCH_SCREEN * aScreen, wxPoint aBreakpoint );

/**************/
/* EECLASS.CPP */
/**************/

void SetaParent( EDA_BaseStruct* Struct, BASE_SCREEN* Screen );

/***************/
/* OPTIONS.CPP */
/***************/
void DisplayOptionFrame( WinEDA_SchematicFrame* parent,
                         const wxPoint&         framepos );

/****************/
/* CONTROLE.CPP */
/****************/
void RemoteCommand( const char* cmdline );

#endif  /* __PROTOS_H__ */
