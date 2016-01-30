	/********************************************/
	/* Definitions for the EESchema program:	*/
	/********************************************/

#ifndef PROGRAM_H
#define PROGRAM_H

#ifndef eda_global
#define eda_global extern
#endif

#include "macros.h"
#include "base_struct.h"

#include "component_class.h"

#define DRAWJUNCTION_SIZE 16		/* Rayon du symbole connexion */
#define DRAWMARKER_SIZE 16			/* Rayon du symbole marqueur */
#define DRAWNOCONNECT_SIZE 48		/* Rayon du symbole No Connexion */

#define HIGHLIGHT_COLOR WHITE


#define TEXT_NO_VISIBLE 1

/* flags pour BUS ENTRY (bus to bus ou wire to bus */
#define WIRE_TO_BUS 0
#define BUS_TO_BUS 1


typedef enum {		/* Type des Marqueurs */
	MARQ_UNSPEC,
	MARQ_ERC,
	MARQ_PCB,
	MARQ_SIMUL,
	MARQ_NMAX		 /* Derniere valeur: fin de tableau */
} TypeMarker;

/* Messages correspondants aux types des marqueurs */
#ifdef MAIN
const wxChar * NameMarqueurType[] =
	{
	wxT(""),
	wxT("ERC"),
	wxT("PCB"),
	wxT("SIMUL"),
	wxT("?????")
	};
#else
extern const wxChar * NameMarqueurType[];
#endif

/* Type des labels sur sheet (Labels sur hierarchie) et forme des Global-Labels*/
typedef enum {
	NET_INPUT,
	NET_OUTPUT,
	NET_BIDI,
	NET_TRISTATE,
	NET_UNSPECIFIED,
	NET_TMAX		/* Derniere valeur: fin de tableau */
} TypeSheetLabel;

/* Messages correspondants aux types ou forme des labels */
#ifdef MAIN
const char * SheetLabelType[] =
	{
	"Input",
	"Output",
	"BiDi",
	"3State",
	"UnSpc",
	"?????"
	};
#else
extern const char * SheetLabelType[];
#endif

/* Description du graphisme des icones associes aux types des Global_Labels */
#ifdef MAIN
int TemplateIN_HN[] = {6, 0,0, -1,-1, -2,-1, -2,1, -1,1, 0,0};
int TemplateIN_HI[] = {6, 0,0, 1,1, 2,1, 2,-1, 1,-1, 0,0};
int TemplateIN_BOTTOM[] = {6, 0,0, 1,-1, 1,-2, -1,-2, -1,-1, 0,0};
int TemplateIN_UP[] =	  {6, 0,0, 1,1, 1,2, -1,2, -1,1, 0,0};

int TemplateOUT_HN[] = {6, -2,0, -1,1, 0,1, 0,-1, -1,-1, -2,0};
int TemplateOUT_HI[] = {6, 2,0, 1,-1, 0,-1, 0,1, 1,1, 2,0};
int TemplateOUT_BOTTOM[] = {6, 0,-2, 1,-1, 1,0, -1,0, -1,-1, 0,-2};
int TemplateOUT_UP[] =		  {6, 0,2, 1,1, 1,0, -1,0, -1,1, 0,2};

int TemplateUNSPC_HN[] = {5, 0,-1, -2,-1, -2,1, 0,1, 0,-1};
int TemplateUNSPC_HI[] = {5, 0,-1, 2,-1, 2,1, 0,1, 0,-1};
int TemplateUNSPC_BOTTOM[] = {5, 1,0, 1,-2, -1,-2, -1,0, 1,0};
int TemplateUNSPC_UP[] =	 {5, 1,0, 1,2, -1,2, -1,0, 1,0};

int TemplateBIDI_HN[] = {5, 0,0, -1,-1, -2,0, -1,1, 0,0};
int TemplateBIDI_HI[] = {5, 0,0, 1,-1, 2,0, 1,1, 0,0};
int TemplateBIDI_BOTTOM[] = {5, 0,0, -1,-1, 0,-2, 1,-1, 0,0};
int TemplateBIDI_UP[] =		{5, 0,0, -1, 1, 0, 2, 1, 1, 0,0};

int Template3STATE_HN[] = {5, 0,0, -1,-1, -2,0, -1,1, 0,0};
int Template3STATE_HI[] = {5, 0,0, 1,-1, 2,0, 1,1, 0,0};
int Template3STATE_BOTTOM[] = {5, 0,0, -1,-1, 0,-2, 1,-1, 0,0};
int Template3STATE_UP[] = {5, 0,0, -1,1, 0,2, 1,1, 0,0};

int * TemplateShape[5][4] =
	{
	 {TemplateIN_HN,TemplateIN_UP,TemplateIN_HI,TemplateIN_BOTTOM},
	 {TemplateOUT_HN,TemplateOUT_UP,TemplateOUT_HI,TemplateOUT_BOTTOM},
	 {TemplateBIDI_HN,TemplateBIDI_UP,TemplateBIDI_HI,TemplateBIDI_BOTTOM},
	 {Template3STATE_HN,Template3STATE_UP,Template3STATE_HI,Template3STATE_BOTTOM},
	 {TemplateUNSPC_HN,TemplateUNSPC_UP,TemplateUNSPC_HI,TemplateUNSPC_BOTTOM}
	};
#else
extern int * TemplateShape[5][4];
#endif


/* Forward declarations */
class DrawSheetStruct;

class EDA_DrawLineStruct: public EDA_BaseLineStruct	/* Segment decription
						base class to describe items which have 2 end points (track, wire, draw line ...) */
{
public:
	bool m_StartIsDangling, m_EndIsDangling;	// TRUE si Start ou End not connected  (wires, tracks...)

public:
	EDA_DrawLineStruct(const wxPoint & pos, int layer );
	~EDA_DrawLineStruct(void) {}
	bool IsOneEndPointAt(const wxPoint & pos);
	EDA_DrawLineStruct * GenCopy(void);
	bool IsNull(void)
		{
			return (m_Start == m_End);
		}
};




class SCH_SCREEN: public BASE_SCREEN
{
public:
	DrawSheetStruct * m_RootSheet;	/* Chainage a la sheet mere dans une hierarchie */
									// Utile pour recadrer les affichages lors de la
									// navigation dans la hierarchie

public:
	SCH_SCREEN(EDA_BaseStruct * parent, WinEDA_DrawFrame * frame_source, int idtype);
	~SCH_SCREEN();
	SCH_SCREEN * Next(void) { return (SCH_SCREEN *) Pnext;}
	void ClearDrawList(void);	// Clear EESchema drawing list
	void Place(WinEDA_DrawFrame * frame, wxDC * DC) {};
	SCH_SCREEN * GenCopy(void);
};




class DrawMarkerStruct: public EDA_BaseStruct		/* marqueurs */
{
public:
	wxPoint m_Pos;					/* XY coordinates of marker. */
	TypeMarker m_Type;
	int m_MarkFlags;				// complements d'information
	wxString m_Comment;				/* Texte (commentaireassocie eventuel */

public:
	DrawMarkerStruct(const wxPoint & pos, const wxString & text);
	~DrawMarkerStruct(void);
	DrawMarkerStruct * GenCopy(void);
	wxString GetComment(void);
};

class DrawNoConnectStruct: public EDA_BaseStruct	/* Symboles de non connexion */
{
public:
	wxPoint m_Pos;						/* XY coordinates of NoConnect. */

public:
	DrawNoConnectStruct(const wxPoint & pos);
	~DrawNoConnectStruct(void) {}
	DrawNoConnectStruct * GenCopy(void);
};

class DrawBusEntryStruct: public EDA_BaseStruct  /* Struct de descr 1 raccord
									 a 45 degres de BUS ou WIRE */
{
public:
	int m_Layer;
	int m_Width;
	wxPoint m_Pos;
	wxSize m_Size;

public:
	DrawBusEntryStruct(const wxPoint & pos, int shape, int id );
	~DrawBusEntryStruct(void) {}
	DrawBusEntryStruct * GenCopy(void);
	wxPoint m_End(void);	// retourne la coord de fin du raccord
};

class DrawPolylineStruct: public EDA_BaseStruct	/* Polyligne (serie de segments) */
{
public:
	int m_Layer;
	int m_Width;
	int m_NumOfPoints;				/* Number of XY pairs in Points array. */
	int *m_Points;					/* XY pairs that forms the polyline. */

public:
	DrawPolylineStruct(int layer);
	~DrawPolylineStruct(void);
	DrawPolylineStruct * GenCopy(void);
};

class DrawJunctionStruct: public EDA_BaseStruct
{
public:
	int m_Layer;
	wxPoint m_Pos;					/* XY coordinates of connection. */

public:
	DrawJunctionStruct(const wxPoint & pos);
	~DrawJunctionStruct(void){}
	DrawJunctionStruct * GenCopy(void);
};

class DrawTextStruct: public EDA_BaseStruct, public EDA_TextStruct
{
public:
	int m_Shape;
	bool m_IsDangling;	// TRUE si non connecté

public:
	DrawTextStruct(const wxPoint & pos = wxPoint(0,0), const wxString & text = wxEmptyString);
	~DrawTextStruct(void) {}
	DrawTextStruct * GenCopy(void);
};

class DrawLabelStruct: public DrawTextStruct
{
public:
	DrawLabelStruct(const wxPoint & pos = wxPoint(0,0), const wxString & text = wxEmptyString);
	~DrawLabelStruct(void) {}
};

class DrawGlobalLabelStruct: public DrawTextStruct
{
public:
	DrawGlobalLabelStruct(const wxPoint & pos = wxPoint(0,0), const wxString & text = wxEmptyString);
	~DrawGlobalLabelStruct(void) {}
};

class DrawSheetLabelStruct: public EDA_BaseStruct, public EDA_TextStruct
{
public:
	int m_Edge, m_Shape;
	bool m_IsDangling;	// TRUE si non connecté

public:
	DrawSheetLabelStruct(DrawSheetStruct * parent,
			const wxPoint & pos = wxPoint(0,0), const wxString & text = wxEmptyString);
	~DrawSheetLabelStruct(void) {}
	DrawSheetLabelStruct * GenCopy(void);
	void Place(WinEDA_DrawFrame * frame, wxDC * DC);
};


class DrawSheetStruct: public DrawPartStruct	/* Gestion de la hierarchie */
{
public:
	DrawSheetLabelStruct *m_Label; 		/* Points de connection */
	int m_NbLabel;						/* Nombre de points de connexion */
	int m_Layer;
	wxPoint m_End;						/* Coord extremité sheet */

public:
	DrawSheetStruct(const wxPoint & pos = wxPoint(0,0) );
	~DrawSheetStruct(void);
	void Place(WinEDA_DrawFrame * frame, wxDC * DC);
	DrawSheetStruct * GenCopy(void);
	void Display_Infos(WinEDA_DrawFrame * frame);
	void CleanupSheet(WinEDA_SchematicFrame * frame, wxDC *DC);
};


#define MAX_LAYERS 44
class LayerStruct
{
public:
	char LayerNames[MAX_LAYERS+1][8];
	int  LayerColor[MAX_LAYERS+1];
	char LayerStatus[MAX_LAYERS+1];
	int  NumberOfLayers;
	int CurrentLayer;
	int CurrentWidth;
	int CommonColor;
	int Flags;
};


#endif /* PROGRAM_H */

