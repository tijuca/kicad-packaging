	/****************************************************************/
	/* EESchema: find.cpp (functions for seraching a schematic item */
	/****************************************************************/
/*
	Search a text (text, value, reference) withing e composent or
	search a composant in libraries, a marker ...,
	in current sheet or whole the project
*/
#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"

/* Variables Locales */
static int s_ItemsCount, s_MarkerCount;
static wxString s_OldStringFound;

#include "dialog_find.cpp"


#include "protos.h"



/**************************************************************/
void InstallFindFrame(WinEDA_SchematicFrame *parent, wxPoint & pos)
/**************************************************************/
{
	parent->DrawPanel->m_IgnoreMouseEvents = TRUE;
	WinEDA_FindFrame * frame = new WinEDA_FindFrame(parent);
	frame->ShowModal(); frame->Destroy();
	parent->DrawPanel->m_IgnoreMouseEvents = FALSE;
}


/**************************************************************/
void WinEDA_FindFrame::FindMarker(wxCommandEvent& event)
/**************************************************************/
/*  Search de markers in whole the hierarchy.
	Mouse cursor is put on the marker
	search the first marker, or next marker
*/
{
int id = event.GetId();
	if( id != FIND_NEXT_MARKER ) m_Parent->FindMarker(0);
	else m_Parent->FindMarker(1);

	Close();
	
}
/*****************************************************************/
EDA_BaseStruct * WinEDA_SchematicFrame::FindMarker(int SearchType)
/*****************************************************************/
/* Search markers in whole the hierarchy.
	Mouse cursor is put on the marker
	SearchType = 0: search the first marker, else search next marker
*/
{
SCH_SCREEN * Screen, * FirstScreen = NULL;
EDA_BaseStruct *DrawList, *FirstStruct = NULL, *Struct = NULL;
DrawMarkerStruct * Marker = NULL;
int StartCount;
bool NotFound;
wxPoint firstpos, pos;
wxSize size = DrawPanel->GetClientSize();
wxPoint curpos, old_cursor_position;
bool force_recadre = FALSE;
wxString msg, WildText;
	
	g_LastSearchIsMarker = TRUE;
	/* Set s_MarkerCount to 0 if we are look for the first marker */
	if( SearchType == 0 ) s_MarkerCount = 0;

	EDA_ScreenList ScreenList(NULL);
	NotFound = TRUE; StartCount = 0;
	/* Search for s_MarkerCount markers */
	for ( Screen = ScreenList.GetFirst(); Screen != NULL; Screen = ScreenList.GetNext() )
	{
		DrawList = Screen->EEDrawList;
		while ( DrawList && NotFound )
		{
			if(DrawList->m_StructType == DRAW_MARKER_STRUCT_TYPE )
			{
			Marker = (DrawMarkerStruct *) DrawList;
				NotFound = FALSE;
				pos = Marker->m_Pos;
				if ( FirstScreen == NULL )	/* First item found */
				{
					FirstScreen = Screen; firstpos = pos;
					FirstStruct = DrawList;
				}
	
				StartCount++;
				if( s_MarkerCount >= StartCount )
				{
					NotFound = TRUE;	/* Search for other markers */
				}
				else	/* We have found s_MarkerCount markers -> Ok */
				{
					Struct = DrawList; s_MarkerCount++; break ;
				}
			}
			DrawList = DrawList->Pnext;
		}
		if( NotFound == FALSE ) break;
	}
	
	if( NotFound && FirstScreen )	// markers are found, but we have reach the last marker */
	{	// After the last marker, the first marker is used */
		NotFound = FALSE; Screen = FirstScreen;
		Struct = FirstStruct;
		pos = firstpos; s_MarkerCount = 1;
	}

	if( NotFound == FALSE)
	{
		if ( Screen != GetScreen() )
		{
			Screen->SetZoom(GetScreen()->GetZoom() );
			m_CurrentScreen = ActiveScreen = Screen;
			force_recadre = TRUE;
		}

		old_cursor_position = Screen->m_Curseur;
		Screen->m_Curseur = pos;
		curpos = DrawPanel->CursorScreenPosition();
		// calcul des coord curseur avec origine = screen
		DrawPanel->GetViewStart(&m_CurrentScreen->m_StartVisu.x,
								&m_CurrentScreen->m_StartVisu.y);
		curpos.x -= m_CurrentScreen->m_StartVisu.x;
		curpos.y -= m_CurrentScreen->m_StartVisu.y;

		/* Il y a peut-etre necessite de recadrer le dessin: */
		if( (curpos.x <= 0) || (curpos.x >= size.x-1) ||
			(curpos.y <= 0) || (curpos.y >= size.y) || force_recadre )
		{
			Recadre_Trace(TRUE);
		}
		else
		{
			wxClientDC dc(DrawPanel);
			DrawPanel->PrepareGraphicContext(&dc);
			EXCHG(old_cursor_position, Screen->m_Curseur);
			DrawPanel->CursorOff(&dc);
 			GRMouseWarp(DrawPanel, curpos );
			EXCHG(old_cursor_position, Screen->m_Curseur);
			DrawPanel->CursorOn(&dc);
 		}

		msg.Printf( _("Marker %d found in %s"), s_MarkerCount, Screen->m_FileName.GetData());
		Affiche_Message(msg);
	}

	else
	{
		Affiche_Message(wxEmptyString);
		msg = _("Marker Not Found");
		DisplayError(this,msg, 10);
	}
	
	return Marker;
}


/**************************************************************/
void WinEDA_FindFrame::FindSchematicItem(wxCommandEvent& event)
/**************************************************************/
/* Find a string in schematic.
	Call to WinEDA_SchematicFrame::FindSchematicItem()
*/
{
int id = event.GetId();

	if( id == FIND_SHEET )
		m_Parent->FindSchematicItem(m_NewTextCtrl->GetValue(), 0);
	else if( id == FIND_HIERARCHY )
		m_Parent->FindSchematicItem(m_NewTextCtrl->GetValue(), 1);
	else if( id == FIND_NEXT )
		m_Parent->FindSchematicItem(wxEmptyString, 2);

	Close();
}

/************************************************************************/
EDA_BaseStruct * WinEDA_SchematicFrame::FindSchematicItem(
			const wxString & pattern, int SearchType)
/************************************************************************/
/* Find a string in schematic.
	Search is made in current sheet (SearchType = 0),
	or the whole hierarchy (SearchType = 1),
	or for the next item  (SearchType = 2).
	Mouse cursor is put on item
*/
{
SCH_SCREEN * Screen, * FirstScreen = NULL;
EDA_BaseStruct *DrawList = NULL, *FirstStruct = NULL, *Struct = NULL;
int StartCount, ii, jj;
bool NotFound;
wxPoint firstpos, pos, old_cursor_position;
static int Find_in_hierarchy;
wxSize size = DrawPanel->GetClientSize();
wxPoint curpos;
bool force_recadre = FALSE;
wxString msg, WildText;
	
	g_LastSearchIsMarker = FALSE;
	
	if( SearchType == 0 )
	{
		s_OldStringFound = pattern;
		Find_in_hierarchy = FALSE;
	}
	
	if( SearchType == 1 )
	{
		s_OldStringFound = pattern;
		Find_in_hierarchy = TRUE;
	}

	if(  SearchType != 2  ) s_ItemsCount = 0;

	WildText = s_OldStringFound;
	NotFound = TRUE; StartCount = 0;
	
	EDA_ScreenList ScreenList(NULL);
	Screen = ScreenList.GetFirst();
	if ( ! Find_in_hierarchy )  Screen = (SCH_SCREEN*) m_CurrentScreen; 

	for ( ; Screen != NULL; Screen = ScreenList.GetNext() )
	{
		DrawList = Screen->EEDrawList;
		while ( DrawList )
		{
			switch (DrawList->m_StructType)
			{
				case DRAW_LIB_ITEM_STRUCT_TYPE :
					#undef STRUCT
					#define STRUCT ((EDA_SchComponentStruct*)DrawList)
					if( WildCompareString( WildText, STRUCT->m_Field[REFERENCE].m_Text, FALSE ) )
					{
						NotFound = FALSE;
						pos = STRUCT->m_Field[REFERENCE].m_Pos;
						break;
					}
					if( WildCompareString( WildText, STRUCT->m_Field[VALUE].m_Text, FALSE ) )
					{
						NotFound = FALSE;
						pos = STRUCT->m_Field[VALUE].m_Pos;
					}
					break;

				case DRAW_LABEL_STRUCT_TYPE :
				case DRAW_GLOBAL_LABEL_STRUCT_TYPE :
				case DRAW_TEXT_STRUCT_TYPE :
					#undef STRUCT
					#define STRUCT ((DrawTextStruct*)DrawList)
					if( WildCompareString( WildText, STRUCT->m_Text, FALSE ) )
					{
						NotFound = FALSE;
						pos = STRUCT->m_Pos;
					}
					break;

				default:
					break;
			}

			if(NotFound == FALSE)	/* Element trouve */
			{
				if ( FirstScreen == NULL )	/* 1er element trouve */
				{
					FirstScreen = Screen; firstpos = pos;
					FirstStruct = DrawList;
				}

				StartCount++;
				if( s_ItemsCount >= StartCount )
				{
					NotFound = TRUE;	/* Continue recherche de l'element suivant */
				}
				else
				{
					Struct = DrawList; s_ItemsCount++; break ;
				}
			}
			if( NotFound == FALSE ) break;
			DrawList = DrawList->Pnext;
		}
		if( NotFound == FALSE ) break;
		if( Find_in_hierarchy == FALSE ) break;
	}

	if( NotFound && FirstScreen )
	{
		NotFound = FALSE; Screen = FirstScreen; Struct = FirstStruct;
		pos = firstpos; s_ItemsCount = 1;
	}

	if( NotFound == FALSE)
	{
		if ( Screen != GetScreen() )
		{
			Screen->SetZoom(GetScreen()->GetZoom() );
			m_CurrentScreen = ActiveScreen = Screen;
			force_recadre = TRUE;
		}

		/* Si la struct localisee est du type DRAW_LIB_ITEM_STRUCT_TYPE,
			Les coordonnes sont a recalculer en fonction de la matrice
			d'orientation */
		if( Struct->m_StructType == DRAW_LIB_ITEM_STRUCT_TYPE )
		{
			#undef STRUCT
			#define STRUCT ((EDA_SchComponentStruct*)Struct)
			pos.x -= STRUCT->m_Pos.x; pos.y -= STRUCT->m_Pos.y;
			ii = STRUCT->m_Transform[0][0] * pos.x + STRUCT->m_Transform[0][1] * pos.y;
			jj = STRUCT->m_Transform[1][0] * pos.x + STRUCT->m_Transform[1][1] * pos.y;
			pos.x = ii + STRUCT->m_Pos.x; pos.y = jj + STRUCT->m_Pos.y;
		}

		old_cursor_position = Screen->m_Curseur;
		Screen->m_Curseur = pos;
		curpos = DrawPanel->CursorScreenPosition();
		DrawPanel->GetViewStart(&m_CurrentScreen->m_StartVisu.x,
								&m_CurrentScreen->m_StartVisu.y);

		// calcul des coord curseur avec origine = screen
		curpos.x -= m_CurrentScreen->m_StartVisu.x;
		curpos.y -= m_CurrentScreen->m_StartVisu.y;

		/* Il y a peut-etre necessite de recadrer le dessin: */
		if( (curpos.x <= 0) || (curpos.x >= size.x-1) ||
			(curpos.y <= 0) || (curpos.y >= size.y) || force_recadre )
		{
			Recadre_Trace(TRUE);
		}
		else
		{
			wxClientDC dc(DrawPanel);
			DrawPanel->PrepareGraphicContext(&dc);
			EXCHG(old_cursor_position, Screen->m_Curseur);
			DrawPanel->CursorOff(&dc);
 			GRMouseWarp(DrawPanel, curpos );
			EXCHG(old_cursor_position, Screen->m_Curseur);
			DrawPanel->CursorOn(&dc);
		}

		msg = WildText + _(" Found in ") + Screen->m_FileName;
		Affiche_Message(msg);
	}

	else
	{
		Affiche_Message(wxEmptyString);
		msg = WildText + _(" Not Found");
		DisplayError(this,msg, 10);
	}
	
	return DrawList;
}


/*************************************************************/
void WinEDA_FindFrame::LocatePartInLibs(wxCommandEvent& event)
/*************************************************************/
/* Recherche exhaustive d'un composant en librairies, meme non chargees
*/
{
wxString Text, FindList;
const wxChar ** ListNames;
LibraryStruct *Lib = NULL;
EDA_LibComponentStruct * LibEntry;
bool FoundInLib = FALSE;	// True si reference trouvee ailleurs qu'en cache
	
	Text = m_NewTextCtrl->GetValue();
	if ( Text.IsEmpty() )
	{
		Close(); return;
	}
	s_OldStringFound = Text;

	int ii, nbitems, NumOfLibs = NumOfLibraries();
	if (NumOfLibs == 0)
		{
		DisplayError(this, _("No libraries are loaded"));
		Close(); return;
		}

	ListNames = GetLibNames();
		
	nbitems = 0;
	for (ii = 0; ii < NumOfLibs; ii++ )	/* Recherche de la librairie */
	{
	bool IsLibCache;
		Lib = FindLibrary(ListNames[ii]);
		if ( Lib == NULL ) break;
		if ( Lib->m_Name.Contains( wxT(".cache")) ) IsLibCache = TRUE;
		else IsLibCache = FALSE;
		LibEntry = (EDA_LibComponentStruct *) PQFirst(&Lib->m_Entries, FALSE);
		while( LibEntry )
		{
			if( WildCompareString(Text, LibEntry->m_Name.m_Text, FALSE) )
			{
				nbitems ++;
				if ( ! IsLibCache ) FoundInLib = TRUE;
				if ( ! FindList.IsEmpty() ) FindList += wxT("\n");
				FindList << _("Found ")
						+ LibEntry->m_Name.m_Text
						+ _(" in lib ") + Lib->m_Name;
			}
		LibEntry = (EDA_LibComponentStruct *) PQNext(Lib->m_Entries, LibEntry, NULL);
		}
	}

	free (ListNames);
	
	if ( ! FoundInLib )
	{
		if ( nbitems ) FindList = wxT("\n") + Text + _(" found only in cache");
		else FindList = Text + _(" not found");
		FindList += _("\nExplore All Libraries?");
		if ( IsOK(this, FindList) )
		{
			FindList.Empty();
			ExploreAllLibraries(Text, FindList);
			if ( FindList.IsEmpty() ) DisplayInfo(this, _("Nothing found") );
			else DisplayInfo(this, FindList);
		}
	}
	else DisplayInfo(this, FindList);
	
	Close();
}


/***************************************************************************************/
int WinEDA_FindFrame::ExploreAllLibraries(const wxString & wildmask, wxString & FindList)
/***************************************************************************************/
{
wxString FullFileName;
FILE * file;
int nbitems = 0, LineNum = 0;
char Line[2048], *name;
	
	FullFileName = MakeFileName(g_RealLibDirBuffer, wxT("*"), g_LibExtBuffer);
	
	FullFileName = wxFindFirstFile(FullFileName);
	while ( ! FullFileName.IsEmpty() )
	{
		file = wxFopen(FullFileName, wxT("rt"));
		if (file == NULL) continue;
 
		while (GetLine(file, Line, &LineNum, sizeof(Line)) )
		{
			if (strnicmp(Line, "DEF", 3) == 0)
			{ /* Read one DEF part from library: DEF 74LS00 U 0 30 Y Y 4 0 N */
				strtok(Line, " \t\r\n");
				name = strtok(NULL, " \t\r\n");
				wxString st_name = CONV_FROM_UTF8(name);
				if( WildCompareString(wildmask, st_name, FALSE) )
				{
					nbitems ++;
					if ( ! FindList.IsEmpty() ) FindList += wxT("\n");
					FindList << _("Found ") << CONV_FROM_UTF8(name)
							<< _(" in lib ") << FullFileName;
				}
			}
			else if (strnicmp(Line, "ALIAS", 5) == 0)
			{ /* Read one ALIAS part from library: ALIAS 74HC00 74HCT00 7400 74LS37 */
				strtok(Line, " \t\r\n");
				while ( (name = strtok(NULL, " \t\r\n")) != NULL )
				{
					wxString st_name = CONV_FROM_UTF8(name);
					if( WildCompareString( wildmask, st_name, FALSE) )
					{
						nbitems ++;
						if ( ! FindList.IsEmpty() ) FindList += wxT("\n");
						FindList << _("Found ") << CONV_FROM_UTF8(name)
								<< _(" in lib ") << FullFileName;
					}
				}
			}
		}
		fclose(file);
		FullFileName = wxFindNextFile();
	}

	return nbitems;
}

