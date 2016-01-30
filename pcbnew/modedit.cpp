	/****************/
	/* modedit.cpp  */
	/****************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "fctsys.h"
#include "common.h"

#include "pcbnew.h"
#include "trigo.h"
#include "3d_viewer.h"

#include "bitmaps.h"
#include "protos.h"
#include "id.h"


/*********************************************************************/
EDA_BaseStruct * WinEDA_ModuleEditFrame::ModeditLocateAndDisplay(void)
/*********************************************************************/
{
EDA_BaseStruct * DrawStruct = GetScreen()->m_CurrentItem;
MODULE * Module = m_Pcb->m_Modules;

	if ( Module == NULL ) return NULL;

	DrawStruct = Locate_Edge_Module(Module, CURSEUR_OFF_GRILLE);
	if ( DrawStruct )
		{
		Affiche_Infos_Segment_Module(this, Module,(EDGE_MODULE*) DrawStruct);
		}
	else DrawStruct = Locate( CURSEUR_OFF_GRILLE );

	return DrawStruct;
}


/****************************************************************************/
void WinEDA_ModuleEditFrame::Process_Special_Functions(wxCommandEvent& event)
/****************************************************************************/
/* Traite les selections d'outils et les commandes appelees du menu POPUP
*/
{
int id = event.GetId();
wxPoint pos;
wxClientDC dc(DrawPanel);

	GetScreen()->CursorOff(DrawPanel, &dc);
	DrawPanel->PrepareGraphicContext(&dc);

	wxGetMousePosition(&pos.x, &pos.y);

	pos.y += 20;

	switch ( id )	// Arret eventuel de la commande de d�placement en cours
		{
		case wxID_CUT:
		case wxID_COPY:
		case ID_TOOLBARH_PCB_SELECT_LAYER:
		case ID_MODEDIT_PAD_SETTINGS:
		case ID_PCB_USER_GRID_SETUP:
		case ID_POPUP_PCB_ROTATE_TEXTEPCB:
		case ID_POPUP_PCB_EDIT_TEXTEPCB:
		case ID_POPUP_PCB_ROTATE_TEXTMODULE:
		case ID_POPUP_PCB_ROTATE_MODULE_CLOCKWISE:
		case ID_POPUP_PCB_ROTATE_MODULE_COUNTERCLOCKWISE:
 		case ID_POPUP_PCB_EDIT_TEXTMODULE:
		case ID_POPUP_PCB_IMPORT_PAD_SETTINGS:
		case ID_POPUP_PCB_EXPORT_PAD_SETTINGS:
		case ID_POPUP_PCB_GLOBAL_IMPORT_PAD_SETTINGS:
		case ID_POPUP_PCB_STOP_CURRENT_DRAWING:
		case ID_POPUP_PCB_EDIT_EDGE:
		case ID_POPUP_PCB_EDIT_WIDTH_CURRENT_EDGE:
		case ID_POPUP_PCB_EDIT_WIDTH_ALL_EDGE:
		case ID_POPUP_PCB_EDIT_LAYER_CURRENT_EDGE:
		case ID_POPUP_PCB_EDIT_LAYER_ALL_EDGE:
		case ID_POPUP_PCB_ENTER_EDGE_WIDTH:
		case ID_POPUP_DELETE_BLOCK:
		case ID_POPUP_PLACE_BLOCK:
		case ID_POPUP_ZOOM_BLOCK:
		case ID_POPUP_MIRROR_Y_BLOCK:
		case ID_POPUP_MIRROR_X_BLOCK:
		case ID_POPUP_ROTATE_BLOCK:
		case ID_POPUP_COPY_BLOCK:
			break;

		case ID_POPUP_CANCEL_CURRENT_COMMAND:
			if( GetScreen()->ManageCurseur &&
				GetScreen()->ForceCloseManageCurseur )
				{
				GetScreen()->ForceCloseManageCurseur(this, &dc);
				}
			break;

		default:	// Arret dea commande de d�placement en cours
			if( GetScreen()->ManageCurseur &&
				GetScreen()->ForceCloseManageCurseur )
				{
				GetScreen()->ForceCloseManageCurseur(this, &dc);
				}
			SetToolID( 0, wxCURSOR_ARROW, wxEmptyString);
			break;
		}

	switch ( id )	// Traitement des commandes
		{
		case ID_EXIT :
			Close(TRUE);
			break;

		case ID_LIBEDIT_SELECT_CURRENT_LIB:
			Select_Active_Library();
			break;

		case ID_LIBEDIT_DELETE_PART:
			{
			wxString Line;
			Line = MakeFileName(g_RealLibDirBuffer, m_CurrentLib, LibExtBuffer);
			Delete_Module_In_Library(Line);
			}
			break;

		case ID_MODEDIT_NEW_MODULE:
			Clear_Pcb(&dc, TRUE);
			GetScreen()->ClearUndoRedoList();
			GetScreen()->m_CurrentItem = NULL;
			GetScreen()->m_Curseur = wxPoint(0,0);
			Create_1_Module(& dc, wxEmptyString);
			if ( m_Pcb->m_Modules ) m_Pcb->m_Modules->m_Flags = 0;
			Zoom_Automatique(TRUE);
			break;

		case ID_MODEDIT_SAVE_LIBMODULE:
			{
			wxString Line;
			Line = MakeFileName(g_RealLibDirBuffer, m_CurrentLib.GetData(), LibExtBuffer);
			Save_1_Module(Line, m_Pcb->m_Modules, TRUE, TRUE);
			GetScreen()->ClrModify();
			}
			break;

		case ID_MODEDIT_LOAD_MODULE_FROM_BOARD:
			GetScreen()->ClearUndoRedoList();
			Load_Module_Module_From_BOARD(NULL);
			GetScreen()->ClrModify();
			if ( m_Draw3DFrame )
				m_Draw3DFrame->NewDisplay();
			break;

		case ID_MODEDIT_SAVE_MODULE_IN_BOARD:
			{
			WinEDA_PcbFrame * pcbframe = m_Parent->m_PcbFrame;
			BOARD * mainpcb = pcbframe->m_Pcb;
			MODULE * oldmodule = NULL;
			MODULE * module_in_edit = m_Pcb->m_Modules;
			// creation du nouveau module sur le PCB en cours
			MODULE * newmodule = new MODULE(mainpcb);
			newmodule->Copy(module_in_edit);
            newmodule->m_Parent = mainpcb;  // modifie par la copie
			newmodule->m_Link = 0;
			// Recherche de l'ancien module correspondant
			//(qui a pu changer ou disparaitre a la suite d'�ditions)
			if ( module_in_edit->m_Link )
				{
				oldmodule = mainpcb->m_Modules;
				for(  ; oldmodule != NULL ; oldmodule = (MODULE *) oldmodule->Pnext )
					{
					if( module_in_edit->m_Link == oldmodule->m_TimeStamp )
						break;
					}
				}

				// Placement du module dans la liste des modules du PCB.
			newmodule->Pnext = mainpcb->m_Modules;
			mainpcb->m_Modules = newmodule;
			newmodule->Pback = mainpcb;
			if ( newmodule->Pnext ) newmodule->Pnext->Pback = newmodule;

			if ( oldmodule )
				{
				newmodule = pcbframe->Exchange_Module(this,
					oldmodule, newmodule);
				newmodule->m_TimeStamp = module_in_edit->m_Link;
				}
			else
				{
				pcbframe->Place_Module(newmodule, NULL);
				newmodule->m_TimeStamp = GetTimeStamp();
				}

			newmodule->m_Flags = 0;
			GetScreen()->ClrModify();
			pcbframe->GetScreen()->m_CurrentItem = NULL;
			mainpcb->m_Status_Pcb = 0;
			}
			break;

		case ID_LIBEDIT_IMPORT_PART:
			GetScreen()->ClearUndoRedoList();
			GetScreen()->m_CurrentItem = NULL;
			Clear_Pcb(&dc, TRUE);
			GetScreen()->m_Curseur = wxPoint(0,0);
			Import_Module(&dc);
			if ( m_Pcb->m_Modules ) m_Pcb->m_Modules->m_Flags = 0;
			GetScreen()->ClrModify();
			Zoom_Automatique(TRUE);
			if ( m_Draw3DFrame )
				m_Draw3DFrame->NewDisplay();
			break;

		case ID_LIBEDIT_EXPORT_PART:
			if ( m_Pcb->m_Modules )
				Export_Module(m_Pcb->m_Modules, FALSE);
			break;

		case ID_LIBEDIT_CREATE_NEW_LIB_AND_SAVE_CURRENT_PART:
			if ( m_Pcb->m_Modules )
				Export_Module(m_Pcb->m_Modules, TRUE);
			break;

		case ID_MODEDIT_SHEET_SET:
			break;

		case ID_MODEDIT_LOAD_MODULE:
			GetScreen()->ClearUndoRedoList();
			GetScreen()->m_CurrentItem = NULL;
			Clear_Pcb(&dc, TRUE);
			GetScreen()->m_Curseur = wxPoint(0,0);
			Load_Module_From_Library(m_CurrentLib, &dc);
			if ( m_Pcb->m_Modules ) m_Pcb->m_Modules->m_Flags = 0;
			GetScreen()->ClrModify();
			Zoom_Automatique(TRUE);
			if ( m_Draw3DFrame )
				m_Draw3DFrame->NewDisplay();
			break;


		case ID_MODEDIT_PAD_SETTINGS:
			InstallPadOptionsFrame(NULL, NULL, wxPoint(-1, -1) );
			break;

		case ID_MODEDIT_CHECK:
			break;

		case ID_MODEDIT_EDIT_MODULE_PROPERTIES:
			if ( m_Pcb->m_Modules )
				{
				GetScreen()->m_CurrentItem = m_Pcb->m_Modules;
				InstallModuleOptionsFrame((MODULE *)GetScreen()->m_CurrentItem,
					&dc, pos);
				GetScreen()->m_CurrentItem->m_Flags = 0;
				}
			break;

		case ID_MODEDIT_ADD_PAD:
			if ( m_Pcb->m_Modules )
				SetToolID( id, wxCURSOR_PENCIL, _("Add Pad"));
			else
				{
				SetToolID( id, wxCURSOR_ARROW, _("Pad Settings"));
				InstallPadOptionsFrame(NULL, NULL, wxPoint(-1, -1) );
				SetToolID( 0, wxCURSOR_ARROW, wxEmptyString);
				}
			break;

		case ID_LINE_COMMENT_BUTT:
		case ID_PCB_ARC_BUTT:
		case ID_PCB_CIRCLE_BUTT:
		case ID_TEXT_COMMENT_BUTT:
			SetToolID( id, wxCURSOR_PENCIL, _("Add Drawing"));
			break;

		case ID_MODEDIT_PLACE_ANCHOR:
			SetToolID( id, wxCURSOR_PENCIL, _("Place anchor"));
			break;

		case ID_NO_SELECT_BUTT:
			SetToolID( 0, wxCURSOR_ARROW, wxEmptyString);
			break;

		case ID_POPUP_CLOSE_CURRENT_TOOL:
			break;

		case ID_POPUP_CANCEL_CURRENT_COMMAND:
			break;

		case ID_MODEDIT_DELETE_ITEM_BUTT:
			SetToolID( id, wxCURSOR_BULLSEYE, _("Delete item"));
			break;

		case ID_POPUP_PCB_ROTATE_MODULE_COUNTERCLOCKWISE:
			DrawPanel->MouseToCursorSchema();
			Rotate_Module(&dc, (MODULE*)GetScreen()->m_CurrentItem, 900, TRUE);
			break;

		case ID_POPUP_PCB_ROTATE_MODULE_CLOCKWISE:
			DrawPanel->MouseToCursorSchema();
			Rotate_Module(&dc, (MODULE*)GetScreen()->m_CurrentItem, -900, TRUE);
			break;

		case ID_POPUP_PCB_EDIT_MODULE:
			InstallModuleOptionsFrame((MODULE *)GetScreen()->m_CurrentItem,
					&dc, pos);
			GetScreen()->m_CurrentItem->m_Flags = 0;
			DrawPanel->MouseToCursorSchema();
			break;

		case ID_POPUP_PCB_MOVE_PAD_REQUEST:
			DrawPanel->MouseToCursorSchema();
			StartMovePad((D_PAD *)GetScreen()->m_CurrentItem, &dc);
			break;

		case ID_POPUP_PCB_EDIT_PAD:
			InstallPadOptionsFrame((D_PAD *)GetScreen()->m_CurrentItem,
					&dc, pos);
			DrawPanel->MouseToCursorSchema();
			break;

		case ID_POPUP_PCB_DELETE_PAD:
			SaveCopyInUndoList();
			DeletePad((D_PAD *)GetScreen()->m_CurrentItem, &dc);
			GetScreen()->m_CurrentItem = NULL;
			DrawPanel->MouseToCursorSchema();
			break;

		case ID_POPUP_PCB_IMPORT_PAD_SETTINGS:
			SaveCopyInUndoList();
			DrawPanel->MouseToCursorSchema();
			Import_Pad_Settings((D_PAD *)GetScreen()->m_CurrentItem, &dc);
			break;

		case ID_POPUP_PCB_GLOBAL_IMPORT_PAD_SETTINGS:
			SaveCopyInUndoList();
			Global_Import_Pad_Settings((D_PAD *)GetScreen()->m_CurrentItem, &dc);
			DrawPanel->MouseToCursorSchema();
			break;

		case ID_POPUP_PCB_EXPORT_PAD_SETTINGS:
			DrawPanel->MouseToCursorSchema();
			Export_Pad_Settings((D_PAD *)GetScreen()->m_CurrentItem);
			break;

		case ID_POPUP_PCB_EDIT_TEXTMODULE:
			InstallTextModOptionsFrame((TEXTE_MODULE *)GetScreen()->m_CurrentItem,
					&dc, pos);
			DrawPanel->MouseToCursorSchema();
			break;

		case ID_POPUP_PCB_MOVE_TEXTMODULE_REQUEST:
			DrawPanel->MouseToCursorSchema();
			StartMoveTexteModule( (TEXTE_MODULE *) GetScreen()->m_CurrentItem,
					&dc);
			break;

		case ID_POPUP_PCB_ROTATE_TEXTMODULE:
			RotateTextModule((TEXTE_MODULE *)GetScreen()->m_CurrentItem,
					&dc);
			DrawPanel->MouseToCursorSchema();
			break;

		case ID_POPUP_PCB_DELETE_TEXTMODULE:
			SaveCopyInUndoList();
			DeleteTextModule((TEXTE_MODULE *)GetScreen()->m_CurrentItem,
					&dc);
			GetScreen()->m_CurrentItem = NULL;
			DrawPanel->MouseToCursorSchema();
			break;

		case ID_POPUP_PCB_MOVE_EDGE:
			Start_Move_EdgeMod((EDGE_MODULE *)GetScreen()->m_CurrentItem, &dc);
			DrawPanel->MouseToCursorSchema();
			break;

		case ID_POPUP_PCB_STOP_CURRENT_DRAWING:
			DrawPanel->MouseToCursorSchema();
			if ( (GetScreen()->m_CurrentItem->m_Flags & IS_NEW) )
				{
				End_Edge_Module((EDGE_MODULE *) GetScreen()->m_CurrentItem, &dc);
				GetScreen()->m_CurrentItem = NULL;
				}
			break;

		case ID_POPUP_PCB_ENTER_EDGE_WIDTH:
			{
			EDGE_MODULE * edge = NULL;
			if ( GetScreen()->m_CurrentItem &&
				( GetScreen()->m_CurrentItem->m_Flags & IS_NEW) &&
				(GetScreen()->m_CurrentItem->m_StructType == TYPEEDGEMODULE) )
				{
				edge = (EDGE_MODULE *) GetScreen()->m_CurrentItem;
				}
			Enter_Edge_Width(edge, &dc);
			DrawPanel->MouseToCursorSchema();
			}
			break;

		case ID_POPUP_PCB_EDIT_WIDTH_CURRENT_EDGE:
			DrawPanel->MouseToCursorSchema();
			Edit_Edge_Width((EDGE_MODULE *) GetScreen()->m_CurrentItem, &dc);
			break;

		case ID_POPUP_PCB_EDIT_WIDTH_ALL_EDGE:
			DrawPanel->MouseToCursorSchema();
			Edit_Edge_Width(NULL, &dc);
			break;

		case ID_POPUP_PCB_EDIT_LAYER_CURRENT_EDGE:
			DrawPanel->MouseToCursorSchema();
			Edit_Edge_Layer((EDGE_MODULE *) GetScreen()->m_CurrentItem, &dc);
			break;

		case ID_POPUP_PCB_EDIT_LAYER_ALL_EDGE:
			DrawPanel->MouseToCursorSchema();
			Edit_Edge_Layer(NULL, &dc);
			break;

		case ID_POPUP_PCB_DELETE_EDGE:
			SaveCopyInUndoList();
			DrawPanel->MouseToCursorSchema();
			RemoveStruct(GetScreen()->m_CurrentItem, &dc);
			GetScreen()->m_CurrentItem = NULL;
			break;

		case ID_MODEDIT_MODULE_ROTATE:
		case ID_MODEDIT_MODULE_MIRROR:
		case ID_MODEDIT_MODULE_SCALE:
		case ID_MODEDIT_MODULE_SCALEX:
		case ID_MODEDIT_MODULE_SCALEY:
			SaveCopyInUndoList();
			Transform( (MODULE*) GetScreen()->m_CurrentItem, &dc, id);
			break;

		case ID_PCB_DRAWINGS_WIDTHS_SETUP:
			InstallOptionsFrame(pos);
			break;

		case ID_PCB_PAD_SETUP:
			InstallPadOptionsFrame((D_PAD *)GetScreen()->m_CurrentItem,
					&dc, pos);
			break;

		case ID_PCB_USER_GRID_SETUP:
			InstallGridFrame(pos);
			break;
		
		case ID_MODEDIT_UNDO:
			GetComponentFromUndoList();
			DrawPanel->Refresh(TRUE);
			break;

		case ID_MODEDIT_REDO:
			GetComponentFromRedoList();
			DrawPanel->Refresh(TRUE);
			break;

		case ID_POPUP_PLACE_BLOCK:
			GetScreen()->BlockLocate.m_Command = BLOCK_MOVE;
			DrawPanel->m_AutoPAN_Request = FALSE;
			HandleBlockPlace(&dc);
			break;

		case ID_POPUP_COPY_BLOCK:
			GetScreen()->BlockLocate.m_Command = BLOCK_COPY;
			m_CurrentScreen->BlockLocate.SetMessageBlock(this);
			DrawPanel->m_AutoPAN_Request = FALSE;
			HandleBlockPlace(&dc);
			break;

		case ID_POPUP_ZOOM_BLOCK:
			GetScreen()->BlockLocate.m_Command = BLOCK_ZOOM;
			m_CurrentScreen->BlockLocate.SetMessageBlock(this);
			m_CurrentScreen->BlockLocate.SetMessageBlock(this);
			HandleBlockEnd(&dc);
			break;

		case ID_POPUP_DELETE_BLOCK:
			GetScreen()->BlockLocate.m_Command = BLOCK_DELETE;
			m_CurrentScreen->BlockLocate.SetMessageBlock(this);
			HandleBlockEnd(&dc);
			break;

		case ID_POPUP_ROTATE_BLOCK:
			GetScreen()->BlockLocate.m_Command = BLOCK_ROTATE;
			m_CurrentScreen->BlockLocate.SetMessageBlock(this);
			HandleBlockEnd(&dc);
			break;

		case ID_POPUP_MIRROR_X_BLOCK:
		case ID_POPUP_MIRROR_Y_BLOCK:
		case ID_POPUP_INVERT_BLOCK:
			GetScreen()->BlockLocate.m_Command = BLOCK_INVERT;
			m_CurrentScreen->BlockLocate.SetMessageBlock(this);
			HandleBlockEnd(&dc);
			break;
		default:
			DisplayError(this,
				wxT("WinEDA_ModuleEditFrame::Process_Special_Functions error"));
			break;
		}

	SetToolbars();
	GetScreen()->CursorOn(DrawPanel, &dc);
}


/******************************************************************************/
void WinEDA_ModuleEditFrame::Transform(MODULE* module, wxDC * DC, int transform)
/******************************************************************************/
/* Execute les transformations de la repr�sentation des modules.
	le module, apres transformation est toujours en position de reference:
	position 0,0
	orientation 0, cot� composant.
*/
{
D_PAD * pad = module->m_Pads;
EDA_BaseStruct * PtStruct = module->m_Drawings;
TEXTE_MODULE * textmod;
EDGE_MODULE* edgemod;
int angle = 900;	// NECESSAIREMENT +- 900 (+- 90 degres) )

	switch ( transform )
	{
		case ID_MODEDIT_MODULE_ROTATE:
			module->SetOrientation(angle);

			for (; pad != NULL; pad = (D_PAD*) pad->Pnext )
			{
				pad->m_Pos0 = pad->m_Pos;
				pad->m_Orient -= angle;
				RotatePoint(&pad->m_Offset.x, &pad->m_Offset.y, angle);
				EXCHG(pad->m_Size.x, pad->m_Size.y);
				RotatePoint(&pad->m_DeltaSize.x, &pad->m_DeltaSize.y, - angle);
			}

			module->m_Reference->m_Pos0 = module->m_Reference->m_Pos;
			module->m_Reference->m_Orient += angle;
			if ( module->m_Reference->m_Orient >= 1800 )
				module->m_Reference->m_Orient -= 1800;
			module->m_Value->m_Pos0 = module->m_Value->m_Pos;
			module->m_Value->m_Orient += angle;
			if ( module->m_Value->m_Orient >= 1800 )
				module->m_Value->m_Orient -= 1800;

			/* Rectification des contours et textes de l'empreinte : */
			for(; PtStruct != NULL; PtStruct = PtStruct->Pnext )
			{
			if( PtStruct->m_StructType == TYPEEDGEMODULE )
				{
				edgemod = (EDGE_MODULE*) PtStruct ;
				edgemod->m_Start0 = edgemod->m_Start;
				edgemod->m_End0 = edgemod->m_End;
				}
			if( PtStruct->m_StructType == TYPETEXTEMODULE )
				{
				/* deplacement des inscriptions : */
				textmod = (TEXTE_MODULE*) PtStruct;
				textmod->m_Pos0 = textmod->m_Pos;
				}
			}

			module->m_Orient = 0;
			break;

		case ID_MODEDIT_MODULE_MIRROR:
			for ( ; pad != NULL; pad = (D_PAD*) pad->Pnext )
			{
				pad->m_Pos.y = -pad->m_Pos.y;
				pad->m_Pos0.y = -pad->m_Pos0.y;
				pad->m_Offset.y = -pad->m_Offset.y;
				pad->m_DeltaSize.y = -pad->m_DeltaSize.y;
				if(pad->m_Orient) pad->m_Orient = 3600 - pad->m_Orient;
			}

			/* Inversion miroir de la Reference */
			textmod = module->m_Reference;
			textmod->m_Pos.y = -textmod->m_Pos.y;
			textmod->m_Pos0.y = textmod->m_Pos0.y;
			if(textmod->m_Orient) textmod->m_Orient = 3600 - textmod->m_Orient;

			/* Inversion miroir de la Valeur  */
			textmod = module->m_Value;
			textmod->m_Pos.y = -textmod->m_Pos.y;
			textmod->m_Pos0.y = textmod->m_Pos0.y;
			if(textmod->m_Orient) textmod->m_Orient = 3600 - textmod->m_Orient;

			/* Inversion miroir des dessins de l'empreinte : */
			PtStruct = module->m_Drawings;
			for( ; PtStruct != NULL; PtStruct = PtStruct->Pnext)
			{
				switch( PtStruct->m_StructType)
				{
					case TYPEEDGEMODULE:
						edgemod = (EDGE_MODULE *) PtStruct;
						edgemod->m_Start.y = -edgemod->m_Start.y ;
						edgemod->m_End.y = -edgemod->m_End.y;
						/* inversion des coords locales */
						edgemod->m_Start0.y = -edgemod->m_Start0.y;
						edgemod->m_End0.y = -edgemod->m_End0.y;
						break;

					case TYPETEXTEMODULE:
						/* Inversion miroir de la position et mise en miroir : */
						textmod = (TEXTE_MODULE*)PtStruct;
						textmod->m_Pos.y = -textmod->m_Pos.y;
						textmod->m_Pos0.y = textmod->m_Pos0.y;
						if(textmod->m_Orient)
							textmod->m_Orient = 3600 - textmod->m_Orient;
						break;

					default: DisplayError(this, wxT("Type Draw Indefini"));
						break;
				}
			}
			break;

		case ID_MODEDIT_MODULE_SCALE:
		case ID_MODEDIT_MODULE_SCALEX:
		case ID_MODEDIT_MODULE_SCALEY:
			DisplayInfo(this, wxT("Not availlable"));
			break;
		}
	module->Set_Rectangle_Encadrement();
	DrawPanel->ReDraw(DC);
}
