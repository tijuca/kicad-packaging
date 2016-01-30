/***********************************************************/
/*                      wxPcbStruct.h                      */
/***********************************************************/

#ifndef  WXPCB_STRUCT_H
#define  WXPCB_STRUCT_H


#include "wxstruct.h"
#include "base_struct.h"

#ifndef PCB_INTERNAL_UNIT
#define PCB_INTERNAL_UNIT 10000
#endif


/*  Forward declarations of classes. */
class PCB_SCREEN;
class WinEDA_Toolbar;
class WinEDA_ModuleEditFrame;
class BOARD;
class TEXTE_PCB;
class MODULE;
class TRACK;
class SEGZONE;
class SEGVIA;
class D_PAD;
class TEXTE_MODULE;
class MIREPCB;
class COTATION;
class EDGE_MODULE;
class WinEDA3D_DrawFrame;
class DRC;
class ZONE_CONTAINER;
class DRAWSEGMENT;
class GENERAL_COLLECTOR;
class GENERAL_COLLECTORS_GUIDE;
class PCB_LAYER_WIDGET;


/**
 * @info see also class WinEDA_BasePcbFrame: Basic class for pcbnew and
 *gerbview
 */


/*****************************************************/
/* class WinEDA_PcbFrame: the main frame for Pcbnew  */
/*****************************************************/
class WinEDA_PcbFrame : public WinEDA_BasePcbFrame
{
    friend class PCB_LAYER_WIDGET;

protected:

    PCB_LAYER_WIDGET* m_Layers;

    DRC* m_drc;                     ///< the DRC controller, see drc.cpp

    // we'll use lower case function names for private member functions.
    void createPopUpMenuForZones( ZONE_CONTAINER* edge_zone, wxMenu* aPopMenu );
    void createPopUpMenuForFootprints( MODULE* aModule, wxMenu* aPopMenu );
    void createPopUpMenuForFpTexts( TEXTE_MODULE* aText, wxMenu* aPopMenu );
    void createPopUpMenuForFpPads( D_PAD* aPad, wxMenu* aPopMenu );
    void createPopupMenuForTracks( TRACK* aTrack, wxMenu* aPopMenu );
    void createPopUpMenuForTexts( TEXTE_PCB* Text, wxMenu* menu );
    void createPopUpBlockMenu( wxMenu* menu );
    void createPopUpMenuForMarkers( MARKER_PCB* aMarker, wxMenu* aPopMenu );

    /**
     * Function setActiveLayer
     * will change the currently active layer to \a aLayer and also
     * update the PCB_LAYER_WIDGET.
     */
    void setActiveLayer( int aLayer, bool doLayerWidgetUpdate = true )
    {
        ( (PCB_SCREEN*) GetScreen() )->m_Active_Layer = aLayer;

        if( doLayerWidgetUpdate )
            syncLayerWidget();
    }

    /**
     * Function getActiveLayer
     * returns the active layer
     */
    int getActiveLayer()
    {
        return ( (PCB_SCREEN*) GetScreen() )->m_Active_Layer;
    }

    /**
     * Function syncLayerWidget
     * updates the currently "selected" layer within the PCB_LAYER_WIDGET.
     * The currently active layer is defined by the return value of getActiveLayer().
     * <p>
     * This function cannot be inline without including layer_widget.h in
     * here and we do not want to do that.
     */
    void syncLayerWidget( );

    /**
     * Function syncLayerBox
     * updates the currently "selected" layer within m_SelLayerBox
     * The currently active layer, as defined by the return value of
     * getActiveLayer().  And updates the colored icon in the toolbar.
     */
    void syncLayerBox();


public:
    WinEDAChoiceBox* m_SelLayerBox;         // a combo box to display and
                                            // select active layer
    WinEDAChoiceBox* m_SelTrackWidthBox;    // a combo box to display and
                                            // select current track width
    WinEDAChoiceBox* m_SelViaSizeBox;       // a combo box to display and
                                            // select current via diameter
    wxTextCtrl*      m_ClearanceBox;        // a text ctrl to display the
                                            // current tracks and vias
                                            // clearance
    wxTextCtrl*      m_NetClassSelectedBox; // a text ctrl to display the
                                            // current NetClass
    bool             m_TrackAndViasSizesList_Changed;

    bool             m_show_microwave_tools;
    bool             m_show_layer_manager_tools;


public:
    WinEDA_PcbFrame( wxWindow* father, const wxString& title,
                     const wxPoint& pos, const wxSize& size,
                     long style = KICAD_DEFAULT_DRAWFRAME_STYLE );

    ~WinEDA_PcbFrame();

    void             OnQuit( wxCommandEvent & WXUNUSED(event) );

    /** Function ToPlotter
     * Open a dialog frame to create plot and drill files
     * relative to the current board
     */
    void             ToPlotter( wxCommandEvent& event );

    /** function ToPrinter
     * Install the print dialog
     */
    void             ToPrinter( wxCommandEvent& event );

    void             GetKicadAbout( wxCommandEvent& event );

    /** Function IsGridVisible() , virtual
     * @return true if the grid must be shown
     */
    virtual bool     IsGridVisible();

    /** Function SetGridVisibility() , virtual
     * It may be overloaded by derived classes
     * if you want to store/retrieve the grid visiblity in configuration.
     * @param aVisible = true if the grid must be shown
     */
    virtual void     SetGridVisibility(bool aVisible);

    /** Function GetGridColor() , virtual
     * @return the color of the grid
     */
    virtual int     GetGridColor();

    /** Function SetGridColor() , virtual
     * @param aColor = the new color of the grid
     */
    virtual void     SetGridColor(int aColor);

    // Configurations:
    void             InstallConfigFrame( const wxPoint& pos );
    void             Process_Config( wxCommandEvent& event );
    void             Update_config( wxWindow* displayframe );

    /** Function Read_Config
     * Read the project configuration file
     * @param projectFileName = the config filename
     *  if not found use kicad.pro
     *  if not found : initialize default values
     * @return true if the current config is modified, false if no change
     */
    bool             Read_Config( const wxString& projectFileName );

    void             OnHotKey( wxDC*           DC,
                               int             hotkey,
                               EDA_BaseStruct* DrawStruct );
    bool             OnHotkeyDeleteItem( wxDC* DC, EDA_BaseStruct* DrawStruct );

    void             OnCloseWindow( wxCloseEvent& Event );
    void             Process_Special_Functions( wxCommandEvent& event );
    void             Tracks_and_Vias_Size_Event( wxCommandEvent& event );

    void             ProcessMuWaveFunctions( wxCommandEvent& event );
    void             MuWaveCommand( wxDC* DC, const wxPoint& MousePos );

    void             RedrawActiveWindow( wxDC* DC, bool EraseBg );
    void             ReCreateHToolbar();
    void             ReCreateAuxiliaryToolbar();
    void             ReCreateVToolbar();
    void             ReCreateAuxVToolbar();
    void             ReCreateOptToolbar();
    void             ReCreateMenuBar();
    WinEDAChoiceBox* ReCreateLayerBox( WinEDA_Toolbar* parent );

    /**
     * Function IsElementVisible
     * tests whether a given element category is visible. Keep this as an
     * inline function.
     * @param aPCB_VISIBLE is from the enum by the same name
     * @return bool - true if the element is visible.
     * @see enum PCB_VISIBLE
     */
    bool IsElementVisible( int aPCB_VISIBLE )
    {
        return GetBoard()->IsElementVisible( aPCB_VISIBLE );
    }

    /**
     * Function SetElementVisibility
     * changes the visibility of an element category
     * @param aPCB_VISIBLE is from the enum by the same name
     * @param aNewState = The new visibility state of the element category
     * @see enum PCB_VISIBLE
     */
    void SetElementVisibility( int aPCB_VISIBLE, bool aNewState );

    /**
     * Function SetVisibleAlls
     * Set the status of all visible element categories and layers to VISIBLE
     */
    void SetVisibleAlls( );

    /**
     * Function ReFillLayerWidget
     * changes out all the layers in m_Layers and may be called upon
     * loading a new BOARD.
     */
    void             ReFillLayerWidget();

    void             Show3D_Frame( wxCommandEvent& event );
    void             GeneralControle( wxDC* DC, wxPoint Mouse );

    /** function ShowDesignRulesEditor
     * Display the Design Rules Editor.
     */
    void             ShowDesignRulesEditor( wxCommandEvent& event );

    /* toolbars update UI functions: */

    void             PrepareLayerIndicator();

    /**
     * Function AuxiliaryToolBar_Update_UI
     * update the displayed values on auxiliary horizontal toolbar
     * (track width, via sizes, clearance ...
     */
    void             AuxiliaryToolBar_Update_UI();

    /**
     * Function AuxiliaryToolBar_DesignRules_Update_UI
     * update the displayed values: track width, via sizes, clearance
     * used when a new netclass is selected
     */
    void             AuxiliaryToolBar_DesignRules_Update_UI();

    /* mouse functions events: */
    void             OnLeftClick( wxDC* DC, const wxPoint& MousePos );
    void             OnLeftDClick( wxDC* DC, const wxPoint& MousePos );

    /**
     * Function OnRightClick
     * populates a popup menu with the choices appropriate for the current
     *context.
     * The caller will add the ZOOM menu choices afterwards.
     * @param aMousePos The current mouse position
     * @param aPopMenu The menu to add to.
     */
    bool             OnRightClick( const wxPoint& aMousePos, wxMenu* aPopMenu );

    void             OnSelectOptionToolbar( wxCommandEvent& event );
    void             ToolOnRightClick( wxCommandEvent& event );

    /** Function SaveCopyInUndoList.
     * Creates a new entry in undo list of commands.
     * add a picker to handle aItemToCopy
     * @param aItemToCopy = the board item modified by the command to undo
     * @param aTypeCommand = command type (see enum UndoRedoOpType)
     * @param aTransformPoint = the reference point of the transformation, for
     *commands like move
     */
    virtual void     SaveCopyInUndoList( BOARD_ITEM* aItemToCopy,
                                        UndoRedoOpType aTypeCommand,
                                        const wxPoint& aTransformPoint =
                                            wxPoint( 0, 0 ) );

    /** Function SaveCopyInUndoList (overloaded).
     * Creates a new entry in undo list of commands.
     * add a list of pickers to handle a list of items
     * @param aItemsList = the list of items modified by the command to undo
     * @param aTypeCommand = command type (see enum UndoRedoOpType)
     * @param aTransformPoint = the reference point of the transformation, for
     *commands like move
     */
    virtual void SaveCopyInUndoList( PICKED_ITEMS_LIST& aItemsList,
                                    UndoRedoOpType aTypeCommand,
                                    const wxPoint& aTransformPoint =
                                        wxPoint( 0, 0 ) );

    /** Function PutDataInPreviousState()
     * Used in undo or redo command.
     * Put data pointed by List in the previous state, i.e. the state memorized
     * by List
     * @param aList = a PICKED_ITEMS_LIST pointer to the list of items to
     *                undo/redo
     * @param aRedoCommand = a bool: true for redo, false for undo
     * @param aRebuildRatsnet = a bool: true to rebuild ratsnet (normal use),
     *                          false
     * to just retrieve las state (used in abort commands that do not need to
     * rebuild ratsnest)
     */
    void PutDataInPreviousState( PICKED_ITEMS_LIST* aList,
                                 bool               aRedoCommand,
                                 bool               aRebuildRatsnet = true );

    /** Function GetBoardFromRedoList
     *  Redo the last edition:
     *  - Save the current board in Undo list
     *  - Get an old version of the board from Redo list
     *  @return none
     */
    void GetBoardFromRedoList( wxCommandEvent& event );

    /** Function GetBoardFromUndoList
     *  Undo the last edition:
     *  - Save the current board in Redo list
     *  - Get an old version of the board from Undo list
     *  @return none
     */
    void GetBoardFromUndoList( wxCommandEvent& event );

    /* Block operations: */

    int  ReturnBlockCommand( int key );
    void HandleBlockPlace( wxDC* DC );
    int  HandleBlockEnd( wxDC* DC );

    /**
     * Function Block_SelectItems
     * Uses  GetScreen()->m_BlockLocate
     * select items within the selected block.
     * selected items are put in the pick list
     * @param none
     */
    void Block_SelectItems();

    /**
     * Function Block_Delete
     * deletes all items within the selected block.
     * @param none
     */
    void Block_Delete();

    /**
     * Function Block_Rotate
     * Rotate all items within the selected block.
     * The rotation center is the center of the block
     * @param none
     */
    void Block_Rotate();

    /**
     * Function Block_Flip
     * Flip items within the selected block.
     * The flip center is the center of the block
     * @param none
     */
    void Block_Flip();

    /**
     * Function Block_Move
     * move all items within the selected block.
     * New location is determined by the current offset from the selected
     *block's original location.
     * @param none
     */
    void Block_Move();

    /**
     * Function Block_Mirror_X
     * mirrors all items within the currently selected block in the X axis.
     * @param none
     */
    void Block_Mirror_X();

    /**
     * Function Block_Duplicate
     * Duplicate all items within the selected block.
     * New location is determined by the current offset from the selected
     * block's original location.
     * @param none
     */
    void Block_Duplicate();


    void SetToolbars();
    void Process_Settings( wxCommandEvent& event );
    void InstallPcbOptionsFrame( int id );
    void InstallDisplayOptionsDialog( wxCommandEvent& aEvent );
    void InstallPcbGlobalDeleteFrame( const wxPoint& pos );

    void GenModulesPosition( wxCommandEvent& event );
    void GenModuleReport( wxCommandEvent& event );
    void InstallDrillFrame( wxCommandEvent& event );
    void ToPostProcess( wxCommandEvent& event );

    void OnFileHistory( wxCommandEvent& event );
    void Files_io( wxCommandEvent& event );
    bool LoadOnePcbFile( const wxString& FileName, bool Append );


    /**
     * Function ReadPcbFile
     * reads a board file  <file>.brd
     * @param Append if 0: a previously loaded board is deleted before loading
     *               the file else all items of the board file are added to the
     *               existing board
     */
    int  ReadPcbFile( FILE* File, bool Append );

    bool SavePcbFile( const wxString& FileName );
    int  SavePcbFormatAscii( FILE* File );
    bool WriteGeneralDescrPcb( FILE* File );

    // BOARD handling

    /** function Clear_Pcb()
     * delete all and reinitialize the current board
     * @param aQuery = true to prompt user for confirmation, false to
     *                 initialize silently
     */
    bool Clear_Pcb( bool aQuery );

    // Drc control

    /* function GetDrcController
     * @return the DRC controller
     */
    DRC* GetDrcController() { return m_drc; }

    /**
     * Function RecreateBOMFileFromBoard
     * Recreates a .cmp file from the current loaded board
     * this is the same as created by cvpcb.
     * can be used if this file is lost
     */
    void       RecreateCmpFileFromBoard( wxCommandEvent& aEvent );

    /**
     * Function RecreateBOMFileFromBoard
     * Creates a BOM file from the current loaded board
     */
    void       RecreateBOMFileFromBoard( wxCommandEvent& aEvent );

    void       ExportToGenCAD( wxCommandEvent& event );

    /**
     * Function ExporttoSPECCTRA
     * will export the current BOARD to a specctra dsn file.  See
     * See http://www.autotraxeda.com/docs/SPECCTRA/SPECCTRA.pdf for the
     * specification.
     */
    void       ExportToSpecctra( wxCommandEvent& event );

    /**
     * Function ImportSpecctraSession
     * will import a specctra *.ses file and use it to relocate MODULEs and
     * to replace all vias and tracks in an existing and loaded BOARD.
     * See http://www.autotraxeda.com/docs/SPECCTRA/SPECCTRA.pdf for the
     * specification.
     */
    void       ImportSpecctraSession( wxCommandEvent& event );

    /**
     * Function ImportSpecctraDesign
     * will import a specctra *.dsn file and use it to replace an entire BOARD.
     * The new board will not have any graphics, only components, tracks and
     * vias.
     * See http://www.autotraxeda.com/docs/SPECCTRA/SPECCTRA.pdf for the
     * specification.
     */
    void       ImportSpecctraDesign( wxCommandEvent& event );

    /**
     * Function Access_to_External_Tool
     * Run an external tool (like freeroute )
     */
    void       Access_to_External_Tool( wxCommandEvent& event );

    MODULE*    ListAndSelectModuleName();

    /** Function ListNetsAndSelect
     * called by a command event
     * displays the sorted list of nets in a dialog frame
     * If a net is selected, it is highlighted
     */
    void       ListNetsAndSelect( wxCommandEvent& event );

    void       Swap_Layers( wxCommandEvent& event );
    void       Install_Test_DRC_Frame( wxDC* DC );

    // Handling texts on the board
    void       Rotate_Texte_Pcb( TEXTE_PCB* TextePcb, wxDC* DC );
    TEXTE_PCB* Create_Texte_Pcb( wxDC* DC );
    void       Delete_Texte_Pcb( TEXTE_PCB* TextePcb, wxDC* DC );
    void       StartMoveTextePcb( TEXTE_PCB* TextePcb, wxDC* DC );
    void       Place_Texte_Pcb( TEXTE_PCB* TextePcb, wxDC* DC );
    void       InstallTextPCBOptionsFrame( TEXTE_PCB* TextPCB, wxDC* DC );

    // Graphic Segments type DRAWSEGMENT
    void       Start_Move_DrawItem( DRAWSEGMENT* drawitem, wxDC* DC );
    void       Place_DrawItem( DRAWSEGMENT* drawitem, wxDC* DC );
    void       InstallGraphicItemPropertiesDialog( DRAWSEGMENT* aItem,
                                                   wxDC*        aDC );

    // Footprint edition (see also WinEDA_BasePcbFrame)
    void       InstallModuleOptionsFrame( MODULE* Module, wxDC* DC );
    void       StartMove_Module( MODULE* module, wxDC* DC );
    bool       Delete_Module( MODULE* module,
                              wxDC*   DC,
                              bool    aAskBeforeDeleting );
    void       Change_Side_Module( MODULE* Module, wxDC* DC );

    void       InstallExchangeModuleFrame( MODULE* ExchangeModuleModule );

    /** function Exchange_Module
     * Replaces OldModule by NewModule, using OldModule settings:
     * position, orientation, pad netnames ...)
     * OldModule is deleted or put in undo list.
     * @param aOldModule = footprint to replace
     * @param aNewModule = footprint to put
     * @param aUndoPickList = the undo list used to save  OldModule. If null,
     *                        OldModule is deleted
     */
    void       Exchange_Module( MODULE*            aOldModule,
                                MODULE*            aNewModule,
                                PICKED_ITEMS_LIST* aUndoPickList );

    // loading modules: see WinEDA_BasePcbFrame

    // Board handling
    void   RemoveStruct( BOARD_ITEM* Item, wxDC* DC );

    // Highlight functions:
    int    Select_High_Light( wxDC* DC );
    void   High_Light( wxDC* DC );

    // Track and via edition:
    void   Via_Edit_Control( wxCommandEvent& event );

    /* Return true if a microvia can be put on board
     * A microvia is a small via restricted to 2 near neighbor layers
     * because its is hole is made by laser which can penetrate only one layer
     * It is mainly used to connect BGA to the first inner layer
     * And it is allowed from an external layer to the first inner layer
     */
    bool IsMicroViaAcceptable( void );

    /**
     * Function Other_Layer_Route
     * operates in one of two ways.  If argument track is NULL, then swap the
     * active layer between m_Route_Layer_TOP and m_Route_Layer_BOTTOM.  If a
     * track is in progress (track is not NULL), and if DRC allows it, place
     * a via on the end of the current track, and then swap the current active
     * layer and start a new segment on the new layer.
     * @param track A TRACK* to append the via to or NULL.
     * @param DC A device context to draw on.
     * @return bool - true if the operation was successful, else false such as
     *                the case where DRC would not allow a via.
     */
    bool   Other_Layer_Route( TRACK* track, wxDC* DC );
    void   Affiche_PadsNoConnect( wxDC* DC );
    void   Affiche_Status_Net( wxDC* DC );
    TRACK* Delete_Segment( wxDC* DC, TRACK* Track );
    void   Delete_Track( wxDC* DC, TRACK* Track );
    void   Delete_net( wxDC* DC, TRACK* Track );
    void   Remove_One_Track( wxDC* DC, TRACK* pt_segm );

    /** function Reset_All_Tracks_And_Vias_To_Netclass_Values
     * Reset all tracks width and/or vias diameters and drill
     * to their default Netclass value
     * @param aTrack : bool true to modify tracks
     * @param aVia : bool true to modify vias
     */
    bool   Reset_All_Tracks_And_Vias_To_Netclass_Values( bool aTrack,
                                                         bool aVia );

    /** function Change_Net_Tracks_And_Vias_Sizes
     * Reset all tracks width and vias diameters and drill
     * to their default Netclass value or current values
     * @param aNetcode : the netcode of the net to edit
     * @param aUseNetclassValue : bool. True to use netclass values, false to
     *                            use current values
     */
    bool   Change_Net_Tracks_And_Vias_Sizes( int  aNetcode,
                                             bool aUseNetclassValue );

    /** Function Edit_Track_Width
     * Modify a full track width (using DRC control).
     * a full track is the set of track segments between 2 ends: pads or a
     * point that has more than 2 segments ends connected
     * @param  DC = the curred device context (can be NULL)
     * @param aTrackSegment = a segment or via on the track to change
     */
    void   Edit_Track_Width( wxDC* DC, TRACK* Track );

    /** Function Edit_TrackSegm_Width
     *  Modify one track segment width or one via diameter (using DRC control).
     * @param  DC = the current device context (can be NULL)
     * @param aTrackItem = the track segment or via to modify
     */
    void   Edit_TrackSegm_Width( wxDC* DC, TRACK* segm );
    TRACK* Begin_Route( TRACK* track, wxDC* DC );
    void   End_Route( TRACK* track, wxDC* DC );
    void   ExChange_Track_Layer( TRACK* pt_segm, wxDC* DC );
    void   Attribut_Segment( TRACK* track, wxDC* DC, bool Flag_On );
    void   Attribut_Track( TRACK* track, wxDC* DC, bool Flag_On );
    void   Attribut_net( wxDC* DC, int net_code, bool Flag_On );
    void   Start_MoveOneNodeOrSegment( TRACK* track, wxDC* DC, int command );
    bool   PlaceDraggedOrMovedTrackSegment( TRACK* Track, wxDC* DC );
    bool   MergeCollinearTracks( TRACK* track, wxDC* DC, int end );
    void   Start_DragTrackSegmentAndKeepSlope( TRACK* track, wxDC* DC );
    void   SwitchLayer( wxDC* DC, int layer );
    bool   Add_45_degrees_Segment( wxDC* DC );
    bool   Genere_Pad_Connexion( wxDC* DC, int layer );

    /** function EraseRedundantTrack
     * Called after creating a track
     * Remove (if exists) the old track that have the same starting and the
     * same ending point as the new created track
     * (this is the redunding track)
     * @param aDC = the current device context (can be NULL)
     * @param aNewTrack = the new created track (a pointer to a segment of the
     *                    track list)
     * @param aNewTrackSegmentsCount = number of segments in this new track
     * @param aItemsListPicker = the list picker to use for an undo command
     *                           (can be NULL)
     */
    int    EraseRedundantTrack( wxDC*              aDC,
                                TRACK*             aNewTrack,
                                int                aNewTrackSegmentsCount,
                                PICKED_ITEMS_LIST* aItemsListPicker );

    /** Function SetTrackSegmentWidth
     *  Modify one track segment width or one via diameter (using DRC control).
     *  Basic routine used by other routines when editing tracks or vias
     * @param aTrackItem = the track segment or via to modify
     * @param aItemsListPicker = the list picker to use for an undo command
     *                           (can be NULL)
     * @param aUseNetclassValue = true to use NetClass value, false to use
     *                            current designSettings value
     * @return  true if done, false if no not change (because DRC error)
     */
    bool SetTrackSegmentWidth( TRACK*             aTrackItem,
                               PICKED_ITEMS_LIST* aItemsListPicker,
                               bool               aUseNetclassValue );


    // zone handling

    /** Function Delete_Zone_Fill
     * Remove the zone filling which include the segment aZone, or the zone
     * which have the given time stamp.  A zone is a group of segments which
     * have the same TimeStamp
     * @param aZone = zone segment within the zone to delete. Can be NULL
     * @param aTimestamp = Timestamp for the zone to delete, used if aZone ==
     *                     NULL
     */
    void Delete_Zone_Fill( SEGZONE* Track, long aTimestamp = 0 );


    /** Function Delete_LastCreatedCorner
     * Used only while creating a new zone outline
     * Remove and delete the current outline segment in progress
     * @return 0 if no corner in list, or corner number
     */
    int  Delete_LastCreatedCorner( wxDC* DC );

    /**
     * Function Begin_Zone
     * initiates a zone edge creation process,
     * or terminates the current zone edge and creates a new zone edge stub
     */
    int  Begin_Zone( wxDC* DC );

    /**
     * Function End_Zone
     * terminates (if no DRC error ) the zone edge creation process
     * @param DC = current Device Context
     * @return true if Ok, false if DRC error
     */
    bool End_Zone( wxDC* DC );

    /** Function Fill_Zone()
     *  Calculate the zone filling for the outline zone_container
     *  The zone outline is a frontier, and can be complex (with holes)
     *  The filling starts from starting points like pads, tracks.
     * If exists the old filling is removed
     * @param zone_container = zone to fill
     * @param verbose = true to show error messages
     * @return error level (0 = no error)
     */
    int  Fill_Zone( ZONE_CONTAINER* zone_container, bool verbose = TRUE );

    /** Function Fill_All_Zones()
     *  Fill all zones on the board
     * The old fillings are removed
     * @param verbose = true to show error messages
     * @return error level (0 = no error)
     */
    int  Fill_All_Zones( bool verbose = TRUE );


    /**
     * Function Add_Zone_Cutout
     * Add a cutout zone to a given zone outline
     * @param DC = current Device Context
     * @param zone_container = parent zone outline
     */
    void Add_Zone_Cutout( wxDC* DC, ZONE_CONTAINER* zone_container );

    /**
     * Function Add_Similar_Zone
     * Add a zone to a given zone outline.
     * if the zones are overlapping they will be merged
     * @param DC = current Device Context
     * @param zone_container = parent zone outline
     */
    void Add_Similar_Zone( wxDC* DC, ZONE_CONTAINER* zone_container );

    /**
     * Function Edit_Zone_Params
     * Edit params (layer, clearance, ...) for a zone outline
     */
    void Edit_Zone_Params( wxDC* DC, ZONE_CONTAINER* zone_container );

    /**
     * Function Start_Move_Zone_Corner
     * Prepares a move corner in a zone outline,
     * called from a move corner command (IsNewCorner = false),
     * or a create new cornet command (IsNewCorner = true )
     */
    void Start_Move_Zone_Corner( wxDC*           DC,
                                 ZONE_CONTAINER* zone_container,
                                 int             corner_id,
                                 bool            IsNewCorner );

    /**
     * Function Start_Move_Zone_Corner
     * Prepares a drag edge in an existing zone outline,
     */
    void Start_Move_Zone_Drag_Outline_Edge(
        wxDC* DC,
        ZONE_CONTAINER*
              zone_container,
        int   corner_id );

    /**
     * Function End_Move_Zone_Corner_Or_Outlines
     * Terminates a move corner in a zone outline, or a move zone outlines
     * @param DC = current Device Context (can be NULL)
     * @param zone_container: the given zone
     */
    void End_Move_Zone_Corner_Or_Outlines(
        wxDC* DC,
        ZONE_CONTAINER*
              zone_container );

    /**
     * Function End_Move_Zone_Corner_Or_Outlines
     * Remove the currently selected corner in a zone outline
     * the .m_CornerSelection is used as corner selection
     */
    void         Remove_Zone_Corner( wxDC* DC, ZONE_CONTAINER* zone_container );

    /** Function Delete_Zone
     * Remove the zone which include the segment aZone, or the zone which have
     * the given time stamp.  A zone is a group of segments which have the
     * same TimeStamp
     * @param DC = current Device Context (can be NULL)
     * @param zone_container = zone to modify
     *  the member .m_CornerSelection is used to find the outline to remove.
     * if the outline is the main outline, all the zone is removed
     * otherwise, the hole is deleted
     */
    void         Delete_Zone_Contour( wxDC* DC, ZONE_CONTAINER* zone_container );

    /**
     * Function Start_Move_Zone_Outlines
     * Initialize parameters to move an existing zone outlines.
     * @param DC = current Device Context (can be NULL)
     * @param zone_container: the given zone to move
     */
    void         Start_Move_Zone_Outlines( wxDC*           DC,
                                           ZONE_CONTAINER* zone_container );

    // Target handling
    MIREPCB*     Create_Mire( wxDC* DC );
    void         Delete_Mire( MIREPCB* MirePcb, wxDC* DC );
    void         StartMove_Mire( MIREPCB* MirePcb, wxDC* DC );
    void         Place_Mire( MIREPCB* MirePcb, wxDC* DC );
    void         InstallMireOptionsFrame( MIREPCB*       MirePcb,
                                          wxDC*          DC,
                                          const wxPoint& pos );

    // Graphic segments type DRAWSEGMENT handling:
    DRAWSEGMENT* Begin_DrawSegment( DRAWSEGMENT* Segment, int shape, wxDC* DC );
    void         End_Edge( DRAWSEGMENT* Segment, wxDC* DC );
    void         Delete_Segment_Edge( DRAWSEGMENT* Segment, wxDC* DC );
    void         Delete_Drawings_All_Layer( int aLayer );

    // Dimension handling:
    void         Install_Edit_Cotation( COTATION*      Cotation,
                                        wxDC*          DC,
                                        const wxPoint& pos );
    COTATION*    Begin_Cotation( COTATION* Cotation, wxDC* DC );
    void         Delete_Cotation( COTATION* Cotation, wxDC* DC );


    // netlist  handling:
    void         InstallNetlistFrame( wxDC* DC, const wxPoint& pos );

    // Autoplacement:
    void         AutoPlace( wxCommandEvent& event );

    /** function OnOrientFootprints
     * install the dialog box for the common Orient Footprints
     */
    void         OnOrientFootprints( void );

    /** function ReOrientModules
     * Set the orientation of footprints
     * @param ModuleMask = mask (wildcard allowed) selection
     * @param Orient = new orientation
     * @param include_fixe = true to orient locked footprints
     */
    void         ReOrientModules( const wxString& ModuleMask, int Orient,
                                  bool include_fixe );
    void         FixeModule( MODULE* Module, bool Fixe );
    void         AutoMoveModulesOnPcb( bool PlaceModulesHorsPcb );
    bool         SetBoardBoundaryBoxFromEdgesOnly();
    void         AutoPlaceModule( MODULE* Module, int place_mode, wxDC* DC );
    int          RecherchePlacementModule( MODULE* Module, wxDC* DC );
    void         GenModuleOnBoard( MODULE* Module );
    float        Compute_Ratsnest_PlaceModule( wxDC* DC );
    int          GenPlaceBoard();
    void         DrawInfoPlace( wxDC* DC );

    // Autorouting:
    int          Solve( wxDC* DC, int two_sides );
    void         Reset_Noroutable( wxDC* DC );
    void         Autoroute( wxDC* DC, int mode );
    void         ReadAutoroutedTracks( wxDC* DC );
    void         GlobalRoute( wxDC* DC );

    void         Show_1_Ratsnest( EDA_BaseStruct* item, wxDC* DC );
    void         Clean_Pcb( wxDC* DC );

    void         InstallFindFrame( const wxPoint& pos, wxDC* DC );

    /**
     * Function SendMessageToEESCHEMA
     * sends a message to the schematic editor so that it may move its cursor
     * to a part with the same reference as the objectToSync
     * @param objectToSync The object whose reference is used to synchronize
     *                     eeschema.
     */
    void         SendMessageToEESCHEMA( BOARD_ITEM* objectToSync );

    /* Micro waves functions */
    void         Edit_Gap( wxDC* DC, MODULE* Module );
    MODULE*      Create_MuWaveBasicShape( const wxString& name, int pad_count );
    MODULE*      Create_MuWaveComponent( int shape_type );
    MODULE*      Create_MuWavePolygonShape();
    void         Begin_Self( wxDC* DC );
    MODULE*      Genere_Self( wxDC* DC );

    /**
     * Load applications settings specific to the PCBNew.
     *
     * This overrides the base class WinEDA_BasePcbFrame::LoadSettings() to
     * handle settings specific common to the PCB layout application.  It
     * calls down to the base class to load settings common to all PCB type
     * drawing frames.  Please put your application settings for PCBNew here
     * to avoid having application settings loaded all over the place.
     */
    virtual void LoadSettings();

    /**
     * Save applications settings common to PCB draw frame objects.
     *
     * This overrides the base class WinEDA_BasePcbFrame::SaveSettings() to
     * save settings specific to the PCB layout application main window.  It
     * calls down to the base class to save settings common to all PCB type
     * drawing frames.  Please put your application settings for PCBNew here
     * to avoid having application settings saved all over the place.
     */
    virtual void SaveSettings();

    /** function SetLanguage
     * called on a language menu selection
     */
    virtual void SetLanguage( wxCommandEvent& event );
    

    DECLARE_EVENT_TABLE()
};


/*********************************************************/
/* class WinEDA_ModuleEditFrame: public WinEDA_DrawFrame */
/* Class for the footprint editor                        */
/*********************************************************/

class WinEDA_ModuleEditFrame : public WinEDA_BasePcbFrame
{
public:
    MODULE*  CurrentModule;
    wxString m_CurrentLib;

public:
    WinEDA_ModuleEditFrame( wxWindow* father,
                            const wxString& title,
                            const wxPoint& pos, const wxSize& size,
                            long style = KICAD_DEFAULT_DRAWFRAME_STYLE );

    ~WinEDA_ModuleEditFrame();

    void         InstallOptionsFrame( const wxPoint& pos );

    void         OnCloseWindow( wxCloseEvent& Event );
    void         Process_Special_Functions( wxCommandEvent& event );
    void         RedrawActiveWindow( wxDC* DC, bool EraseBg );
    void         ReCreateHToolbar();
    void         ReCreateVToolbar();
    void         ReCreateOptToolbar();
    void         ReCreateAuxiliaryToolbar();
    void         OnLeftClick( wxDC* DC, const wxPoint& MousePos );
    void         OnLeftDClick( wxDC* DC, const wxPoint& MousePos );
    bool         OnRightClick( const wxPoint& MousePos, wxMenu* PopMenu );
    void         SetToolbars();
    void         ReCreateMenuBar();
    void         ToolOnRightClick( wxCommandEvent& event );
    void         OnSelectOptionToolbar( wxCommandEvent& event );
    void         OnHotKey( wxDC* DC, int hotkey, EDA_BaseStruct* DrawStruct );
    void         Show3D_Frame( wxCommandEvent& event );
    void         GeneralControle( wxDC* DC, wxPoint Mouse );
    void         LoadModuleFromBoard( wxCommandEvent& event );

    /** function ToPrinter
     * Install the print dialog
     */
    void         ToPrinter( wxCommandEvent& event );

    // BOARD handling

    /** function Clear_Pcb()
     * delete all and reinitialize the current board
     * @param aQuery = true to prompt user for confirmation, false to
     *                 initialize silently
     */
    bool         Clear_Pcb( bool aQuery );

    /* handlers for block commands */
    int          ReturnBlockCommand( int key );
    virtual void HandleBlockPlace( wxDC* DC );
    virtual int  HandleBlockEnd( wxDC* DC );

    BOARD_ITEM*  ModeditLocateAndDisplay( int aHotKeyCode = 0 );

    /* Undo and redo functions */
public:

    /** Function SaveCopyInUndoList.
     * Creates a new entry in undo list of commands.
     * add a picker to handle aItemToCopy
     * @param aItem = the board item modified by the command to undo
     * @param aTypeCommand = command type (see enum UndoRedoOpType)
     * @param aTransformPoint = the reference point of the transformation, for
     *                          commands like move
     */
    virtual void SaveCopyInUndoList( BOARD_ITEM* aItem,
                                    UndoRedoOpType aTypeCommand,
                                    const wxPoint& aTransformPoint =
                                        wxPoint( 0, 0 ) );

    /** Function SaveCopyInUndoList (overloaded).
     * Creates a new entry in undo list of commands.
     * add a list of pickers to handle a list of items
     * @param aItemsList = the list of items modified by the command to undo
     * @param aTypeCommand = command type (see enum UndoRedoOpType)
     * @param aTransformPoint = the reference point of the transformation, for
     *                          commands like move
     */
    virtual void SaveCopyInUndoList( PICKED_ITEMS_LIST& aItemsList,
                                    UndoRedoOpType aTypeCommand,
                                    const wxPoint& aTransformPoint =
                                        wxPoint( 0, 0 ) );

private:
    void         GetComponentFromUndoList( wxCommandEvent& event );
    void         GetComponentFromRedoList( wxCommandEvent& event );

public:

    // Footprint edition
    void         Place_Ancre( MODULE* module );
    void         RemoveStruct( EDA_BaseStruct* Item );
    void         Transform( MODULE* module, int transform );

    // loading Footprint
    MODULE*      Import_Module( wxDC* DC );
    void         Export_Module( MODULE* ptmod, bool createlib );
    void         Load_Module_From_BOARD( MODULE* Module );

    // functions to edit footprint edges

    /**
     * Function Edit_Edge_Width
     * changes the width of module perimeter lines, EDGE_MODULEs.
     * @param ModuleSegmentWidth (global) = new width
     * @param Edge = edge to edit, or NULL.  If Edge == NULL change
     *               the width of all the footprint's edges
     * @param DC = current Device Context
     */
    void         Edit_Edge_Width( EDGE_MODULE* Edge );
    void         Edit_Edge_Layer( EDGE_MODULE* Edge );
    void         Delete_Edge_Module( EDGE_MODULE* Edge );
    EDGE_MODULE* Begin_Edge_Module( EDGE_MODULE* Edge, wxDC* DC, int type_edge );
    void         End_Edge_Module( EDGE_MODULE* Edge, wxDC* DC );
    void         Enter_Edge_Width( EDGE_MODULE* Edge, wxDC* DC );
    void         Start_Move_EdgeMod( EDGE_MODULE* drawitem, wxDC* DC );
    void         Place_EdgeMod( EDGE_MODULE* drawitem, wxDC* DC );

    // handlers for libraries:
    void         Delete_Module_In_Library( const wxString& libname );
    int          Create_Librairie( const wxString& LibName );
    void         Select_Active_Library();

    DECLARE_EVENT_TABLE()
};

#endif  /* WXPCB_STRUCT_H */