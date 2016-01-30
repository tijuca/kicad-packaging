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
#include "class_screen.h"

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
	virtual void Draw(WinEDA_DrawPanel * panel, wxDC * DC, const wxPoint & offset, int draw_mode, int Color = -1);
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
	virtual void Draw(WinEDA_DrawPanel * panel, wxDC * DC, const wxPoint & offset, int draw_mode, int Color = -1);
};

class DrawNoConnectStruct: public EDA_BaseStruct	/* Symboles de non connexion */
{
public:
	wxPoint m_Pos;						/* XY coordinates of NoConnect. */

public:
	DrawNoConnectStruct(const wxPoint & pos);
	~DrawNoConnectStruct(void) {}
	DrawNoConnectStruct * GenCopy(void);
	virtual void Draw(WinEDA_DrawPanel * panel, wxDC * DC, const wxPoint & offset, int draw_mode, int Color = -1);
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
	virtual void Draw(WinEDA_DrawPanel * panel, wxDC * DC, const wxPoint & offset, int draw_mode, int Color = -1);
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
	virtual void Draw(WinEDA_DrawPanel * panel, wxDC * DC, const wxPoint & offset, int draw_mode, int Color = -1);
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
	virtual void Draw(WinEDA_DrawPanel * panel, wxDC * DC, const wxPoint & offset, int draw_mode, int Color = -1);
};

class DrawTextStruct: public EDA_BaseStruct, public EDA_TextStruct
{
public:
	int m_Shape;
	bool m_IsDangling;	// TRUE si non connect�

public:
	DrawTextStruct(const wxPoint & pos = wxPoint(0,0), const wxString & text = wxEmptyString);
	~DrawTextStruct(void) {}
	DrawTextStruct * GenCopy(void);
	virtual void Draw(WinEDA_DrawPanel * panel, wxDC * DC, const wxPoint & offset, int draw_mode, int Color = -1);
	void SwapData(DrawTextStruct * copyitem);
	virtual void Place(WinEDA_DrawFrame * frame, wxDC * DC);
private:
	void DrawAsText(WinEDA_DrawPanel * panel, wxDC * DC, const wxPoint & offset, int draw_mode, int Color);
	void DrawAsLabel(WinEDA_DrawPanel * panel, wxDC * DC, const wxPoint & offset, int draw_mode, int Color);
	void DrawAsGlobalLabel(WinEDA_DrawPanel * panel, wxDC * DC, const wxPoint & offset, int draw_mode, int Color);
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

