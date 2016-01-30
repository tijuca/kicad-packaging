/****************************************************************/
/*	Headers fo library definition and lib component definitions */
/****************************************************************/

#ifndef LIBCMP_H
#define LIBCMP_H

#ifndef eda_global
#define eda_global extern
#endif

#include "priorque.h"

#define LIB_VERSION_MAJOR 2
#define LIB_VERSION_MINOR 3
#define LIBFILE_IDENT "EESchema-LIBRARY Version"  /* Must be at the lib file start. */
#define DOCFILE_IDENT "EESchema-DOCLIB  Version 2.0"  /* Must be at the doc file start. */
#define DOC_EXT wxT(".dcm")		/* extension des fichiers de documentation */

#define TARGET_PIN_DIAM 12		/* Diam cercle des extremites des pins */

#define DEFAULT_TEXT_SIZE 50	/* Default size for field texts */
#define PART_NAME_LEN	15		/* Maximum length of part name. */
#define PREFIX_NAME_LEN	5		/* Maximum length of prefix (IC, R, SW etc.). */
#define PIN_WIDTH	100			/* Width between 2 pins in internal units. */
#define PIN_LENGTH	300			/* Default Length of each pin to be drawn. */

#define INVERT_PIN_RADIUS 35			/* Radius of inverted pin circle. */
#define CLOCK_PIN_DIM 40				/* Dim of clock pin symbol. */
#define IEEE_SYMBOL_PIN_DIM 40			/* Dim of special pin symbol. */

#define NO_FILL 0				// Poly, Squar, Circle, Arc = option No Fill
#define FILLED_SHAPE 1			// Poly, Squar, Circle, Arc = option Fill with current color
#define FILLED_WITH_BG_BODYCOLOR 2	// Poly, Squar, Circle, Arc = option Fill
									// with background body color

//Offsets used in editing library component, for handle aliad dats
#define ALIAS_NAME 0
#define ALIAS_DOC 1
#define ALIAS_KEYWORD 2
#define ALIAS_DOC_FILENAME 3
#define ALIAS_NEXT 4


typedef enum {
	LOCATE_COMPONENT_ARC_DRAW_TYPE = 1,
	LOCATE_COMPONENT_CIRCLE_DRAW_TYPE = 2,
	LOCATE_COMPONENT_GRAPHIC_TEXT_DRAW_TYPE = 4,
	LOCATE_COMPONENT_RECT_DRAW_TYPE = 8,
	LOCATE_LINE_DRAW_TYPE = 0x10,
	LOCATE_COMPONENT_POLYLINE_DRAW_TYPE = 0x20,
	LOCATE_COMPONENT_LINE_DRAW_TYPE = 0x40
} LocateDrawStructType;

#define LOCATE_ALL_DRAW_ITEM 0xFFFFFFFF

/* flags utilises dans FindLibPart() : */
#define FIND_ROOT 0		/* indique la recherche du composant racine si
							meme si le composant specifie est un alias */
#define FIND_ALIAS 1	/* indique la recherche du composant specifie
							(alias ou racine) */

/* definition des types des structures d'elements de librairie */
typedef enum {
	ROOT,		/* Structure est a standard EDA_LibComponentStruct */
	ALIAS		/* Structure is an alias */
} LibrEntryType;

/* valeur du membre .m_Options */
typedef enum {
	ENTRY_NORMAL,	// Libentry is standard
	ENTRY_POWER		// Libentry is a power symbol
} LibrEntryOptions;

/* Definitions des Pins */

typedef enum {		/* Type des Pins. si modif: modifier tableau des mgs suivant */
	PIN_INPUT,
	PIN_OUTPUT,
	PIN_BIDI,
	PIN_TRISTATE,
	PIN_PASSIVE,
	PIN_UNSPECIFIED,
	PIN_POWER_IN,
	PIN_POWER_OUT,
	PIN_OPENCOLLECTOR,
	PIN_OPENEMITTER,
	PIN_NC,				/* No connect */
	PIN_NMAX			/* Valeur limite ( utilisee comme limite de tableaux) */
} ElectricPinType;

/* Messages d'affichage du type electrique */
eda_global wxChar * MsgPinElectricType[]
#ifdef MAIN
	= {
	wxT("input"),
	wxT("output"),
	wxT("BiDi"),
	wxT("3state"),
	wxT("passive"),
	wxT("unspc"),
	wxT("power_in"),
	wxT("power_out"),
	wxT("openCol"),
	wxT("openEm"),
	wxT("?????")
	}
#endif
	;

/* Autres bits: bits du membre .Flag des Pins */
#define PINNOTDRAW 1		/* si 1: pin invisible */

typedef enum {  	  		/* Forme des Pins */
	NONE = 0,
	INVERT = 1,
	CLOCK = 2,
	LOWLEVEL_IN = 4,
	LOWLEVEL_OUT = 8
	} DrawPinShape;

typedef enum {				/* Orientation des Pins */
	PIN_RIGHT = 'R',
	PIN_LEFT = 'L',
	PIN_UP = 'U',
	PIN_DOWN = 'D',
	} DrawPinOrient;

/*************************************/
/* Classe representant une librairie */
/*************************************/

class LibraryStruct
{
public:
	int m_Type;						/* type indicator */
	wxString m_Name;				/* Name of library loaded. */
	wxString m_FullFileName;		/* Full File Name (with path) of library loaded. */
	wxString m_Header;				/* first line of library loaded. */
	int m_NumOfParts;				/* Number of parts this library has. */
	PriorQue * m_Entries;			/* Parts themselves are saved here. */
	LibraryStruct * m_Pnext;		/* Point on next lib in chain. */
	int m_Modified;					/* flag indicateur d'edition */
	int m_Size;						// Size in bytes (for statistics)
	long m_TimeStamp;				// Signature temporelle
	int m_Flags;					// variable used in some functions
	bool m_IsLibCache;				// False for the "standard" libraries,
									// True for the library cache

public:
	LibraryStruct(int type, const wxString & name, const wxString & fullname);
	~LibraryStruct();
	bool WriteHeader(FILE * file);
	bool ReadHeader(FILE * file, int * LineNum);
};


/*******************************************************************/
/* Classes representant les el�ments d'un composant d'une libraire */
/*******************************************************************/


/* class LibEDA_BaseStruct : Basic class for items used in a library component
	(graphic shapes, texts, fields, pins)
*/

class LibEDA_BaseStruct : public EDA_BaseStruct
{
public:
	int m_Unit;					/* Unit identification (for multi part per parkage)
								0 if the item is common to all units */
	int m_Convert;				/* Shape identification (for parts which have a convert shape)
									0 if the item is common to all shapes */

public:
	LibEDA_BaseStruct * Next(void) {return (LibEDA_BaseStruct *) Pnext;}
	LibEDA_BaseStruct(int struct_type);
	virtual ~LibEDA_BaseStruct(void){}
	void Display_Infos_DrawEntry(WinEDA_DrawFrame * frame);
};

class LibDrawPin : public LibEDA_BaseStruct
{
public:
	wxPoint m_Pos;			/* Pin position */
	short m_PinLen;				/* Pin lenght */
	short m_Orient;			/* Pin orientation (Up, Down, Left, Right) */
	short m_PinShape;			/* Bitwise ORed: Pin shape (see enum DrawPinShape) */
	char m_PinType;			/* Electrical pin properties */
	char m_Attributs;		/* bit 0 != 0: pin invisible */
	long m_PinNum;			/* Pin number: 4 Ascii code like
							"12" or "anod" or "G6" 
							"12" is really "12\0\0"*/
	wxString m_PinName;
	short m_SizeNum, m_SizeName;	/* Pin num and Pin name sizes */

public:
	LibDrawPin(void);
	~LibDrawPin(void) {}
	LibDrawPin * GenCopy(void);
    bool WriteDescr( FILE * File );
	void Display_Infos(WinEDA_DrawFrame * frame);
	wxPoint ReturnPinEndPoint(void);
	int ReturnPinDrawOrient(int TransMat[2][2]);
	void ReturnPinStringNum(wxString & buffer);
	void SetPinNumFromString(wxString & buffer);
	void DrawPinTexts(WinEDA_DrawPanel * panel, wxDC * DC,
				wxPoint & pin_pos, int orient,
				int TextInside, bool DrawPinNum, bool DrawPinName,
				int Color, int DrawMode);
	void PlotPinTexts(wxPoint & pin_pos, int orient,
				int TextInside, bool DrawPinNum, bool DrawPinName);
};


class LibDrawArc : public LibEDA_BaseStruct
{
public:
	wxPoint m_Pos;			/* Position du point de reference (Centre)*/
	int m_Rayon;
	int m_Width;
	int m_Fill;
	int t1, t2;				/* position des 2 extremites de l'arc en 0,1 degres */
	wxPoint m_Start, m_End;	/* position des 2 extremites de l'arc en coord reelles*/

public:
	LibDrawArc(void);
	~LibDrawArc(void){}
	LibDrawArc * GenCopy(void);
    bool WriteDescr( FILE * File );
};

class LibDrawCircle : public LibEDA_BaseStruct
{
public:
	wxPoint m_Pos;			/* Position du point de reference */
	int m_Rayon;
	int m_Width;
	int m_Fill;

public:
	LibDrawCircle(void);
	~LibDrawCircle(void){}
	LibDrawCircle * GenCopy(void);
    bool WriteDescr( FILE * File );
};

class LibDrawText : public LibEDA_BaseStruct
{
public:
	wxPoint m_Pos;			/* Position du point de reference */
	int m_Horiz;
	wxSize m_Size;
	int m_Type;
	wxString m_Text;

public:
	LibDrawText(void);
	~LibDrawText(void){}
	LibDrawText * GenCopy(void);
    bool WriteDescr( FILE * File );
};

class LibDrawSquare : public LibEDA_BaseStruct
{
public:
	wxPoint m_Start;
	wxPoint m_End;
	int m_Width;
	int m_Fill;

public:
	LibDrawSquare(void);
	~LibDrawSquare(void){}
	LibDrawSquare * GenCopy(void);
    bool WriteDescr( FILE * File );
};

class LibDrawSegment : public LibEDA_BaseStruct
{
public:
	wxPoint m_Start;
	wxPoint m_End;
	int m_Width;

public:
	LibDrawSegment(void);
	~LibDrawSegment(void){}
	LibDrawSegment * GenCopy(void);
    bool WriteDescr( FILE * File );
};

class LibDrawPolyline : public LibEDA_BaseStruct
{
public:
	int n, *PolyList;
	int m_Width;
	int m_Fill;

public:
	LibDrawPolyline(void);
	~LibDrawPolyline(void){ if ( PolyList ) free(PolyList);}
	LibDrawPolyline * GenCopy(void);
	void AddPoint(const wxPoint & point);
    bool WriteDescr( FILE * File );
};

/* Fields identiques aux fields des composants, pouvant etre predefinis en lib
2 Fields sont toujours presents : Prefix (U, IC..) et Name (74LS00..)*/
class LibDrawField :  public LibEDA_BaseStruct
{
public:
	int m_FieldId;				// 0 a 11
								// 0 = Name 1 = Valeur 2 .. 11 autres fields
	wxPoint m_Pos;
	wxSize m_Size;
	int m_Orient;				/* Orientation */
	int m_Attributs;			/* Attributs (Non visible ...) */
	int m_HJustify, m_VJustify;	/* Justifications Horiz et Vert du texte */
	wxString m_Text;			/* Field Data */
	wxString m_Name;			/* Field Name */

public:
	LibDrawField(int idfield = 2);
	~LibDrawField(void);

	LibDrawField * GenCopy(void);
	void Copy(LibDrawField * Target);
    bool WriteDescr( FILE * File );
};


/* classe de base de description des composants en librairie */
class LibCmpEntry : public EDA_BaseStruct
{
public:
	LibrEntryType Type;			/* Type = ROOT;
										= ALIAS pour struct LibraryAliasType */
	LibDrawField m_Name;		// name	(74LS00 ..) in lib ( = VALUE )
	wxString m_Doc;				/* ligne de documentation */
	wxString m_KeyWord;			/* liste des mots cles */
	wxString m_DocFile;			/* nom du fichier Doc Associe */
	LibrEntryOptions m_Options;	// special features (i.e. Entry is a POWER)

public:
	LibCmpEntry(LibrEntryType CmpType, const wxChar * CmpName);
	virtual ~LibCmpEntry(void);
    bool WriteDescr( FILE * File );
};

class EDA_LibComponentStruct: public LibCmpEntry		/* composant "racine" */
{
public:
	LibDrawField m_Prefix;			/* Prefix ( U, IC ... ) = REFERENCE */
	wxArrayString m_AliasList;		/* ALIAS list for the component */
	int m_UnitCount;				/* Units (or sections) per package */
	bool m_UnitSelectionLocked;		// True if units are differents and their selection is locked
									// (i.e. if part A cannot be automatically changed in part B
	int m_TextInside;				/* if 0: pin name drawn on the pin itself
									if > 0 pin name drawn inside the component,
									with a distance of m_TextInside in mils */
	bool m_DrawPinNum;
	bool m_DrawPinName;
	LibDrawField *Fields;			/* Auxiliairy Field list (id = 2 a 11*/
	LibEDA_BaseStruct * m_Drawings;	/* How to draw this part */
	long m_LastDate;				// Last change Date

public:
	EDA_LibComponentStruct( const wxChar * CmpName);
	EDA_Rect GetBoundaryBox( int Unit, int Convert);	/* return Box around the part. */
	~EDA_LibComponentStruct( void );
	void SortDrawItems(void);
};

class EDA_LibCmpAliasStruct: public LibCmpEntry
{
public:
	wxString m_RootName;		/* Part name pour le composant de reference */

public:
	EDA_LibCmpAliasStruct( const wxChar * CmpName, const wxChar * CmpRootName);
	~EDA_LibCmpAliasStruct(void);
};

/* Variables */
extern LibraryStruct *LibraryList;		   /* All part libs are saved here. */

/* Variables Utiles pour les editions de composants en librairie */
eda_global LibEDA_BaseStruct * LibItemToRepeat; /* pointeur sur l'�l�ment que l'on
												peut r�p�ter (Pin..;) */
eda_global LibraryStruct *CurrentLib;			/* Pointeur sur la librairie du
											composant en cours d'edition */
eda_global EDA_LibComponentStruct *CurrentLibEntry;	/* pointeur sur le composant en
											cours d'edition */
eda_global LibEDA_BaseStruct * CurrentDrawItem;	/* pointeur sur les
									elements de dessin du comp. en edition */

eda_global wxString CurrentAliasName;			// Nom de l'alias selectionn�
eda_global bool g_AsDeMorgan;		// Pour libedit:
eda_global int CurrentUnit
#ifdef MAIN
	= 1
#endif
;
eda_global int CurrentConvert		/* Convert = 1 .. 255 */
#ifdef MAIN
	= 1
#endif
;


eda_global wxString FindLibName;		/* nom de la librairie ou a ete trouve le
							dernier composant recherche par FindLibPart() */

#endif  //  LIBCMP_H

