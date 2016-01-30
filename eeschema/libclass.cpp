/**********************************************************/
/*	libclass.cpp										  */
/**********************************************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"

#include "protos.h"


	/*******************************************************/
	/* Methodes relatives a la manipulation des librairies */
	/*******************************************************/

/***************************************************************************************/
LibraryStruct::LibraryStruct(int type, const wxString & name, const wxString & fullname)
/***************************************************************************************/
{
	m_Type = type;					/* type indicator */
	m_Name = name;					/* Name of library loaded. */
	m_FullFileName = fullname;		/* Full File Name (with path) of library loaded. */
	m_NumOfParts = 0;				/* Number of parts this library has. */
	m_Entries = NULL;				/* Parts themselves are saved here. */
	m_Pnext = NULL;					/* Point on next lib in chain. */
	m_Modified = FALSE;				/* flag indicateur d'edition */
	m_TimeStamp = 0;
	m_Flags = 0;
	m_IsLibCache = FALSE;

}

/*****************************************/
void FreeLibraryEntry(LibCmpEntry * Entry)
/*****************************************/
/* Used by PQFreeFunc() to delete all entries
*/
{
	delete Entry;
}

/******************************/
LibraryStruct::~LibraryStruct()
/******************************/
{
	if ( m_Entries ) PQFreeFunc(m_Entries, (void(*)(void *))FreeLibraryEntry);
}


/*******************************************/
bool LibraryStruct::WriteHeader(FILE * file)
/*******************************************/
/* Ecrit l'entete du fichier librairie
*/
{
char BufLine[1024];
bool succes = TRUE;

    DateAndTime(BufLine);
    if ( fprintf(file,"%s %d.%d  Date: %s\n", LIBFILE_IDENT,
						LIB_VERSION_MAJOR, LIB_VERSION_MINOR,
									BufLine) != 5 )
		succes = FALSE;
#if 0
	if ( fprintf(file,"$HEADER\n") != 1 ) succes = FALSE;
	if ( fprintf(file,"TimeStamp %8.8lX\n", m_TimeStamp) != 2 ) succes = FALSE;
	if ( fprintf(file,"Parts %d\n", m_NumOfParts) != 2 ) succes = FALSE;
	if ( fprintf(file,"$ENDHEADER\n") != 1 ) succes = FALSE;
#endif
	return (succes);
}

/***********************************************************/
bool LibraryStruct::ReadHeader(FILE * libfile, int * LineNum)
/***********************************************************/
/* Ecrit l'entete du fichier librairie
*/
{
char Line[1024], * text, * data;

	while (GetLine(libfile, Line, LineNum, sizeof(Line)) )
	{
		text = strtok(Line, " \t\r\n");
		data = strtok(NULL, " \t\r\n");
		if ( stricmp(text, "TimeStamp") == 0 )
			m_TimeStamp = atol(data);
		if ( stricmp(text,"$ENDHEADER") == 0 ) return TRUE;
	}
	return FALSE;
}


	/*********************/
	/* class LibCmpEntry */
	/*********************/
/* Basic class for librarty oomponent description
	Not directly used
	Used to create the 2 derived classes :
		- EDA_LibCmpAliasStruct
		- EDA_LibComponentStruct
*/

/********************************************************************/
LibCmpEntry::LibCmpEntry(LibrEntryType CmpType, const wxChar * CmpName):
		EDA_BaseStruct(LIBCOMPONENT_STRUCT_TYPE)
/********************************************************************/
{
	Type = CmpType;
	m_Name.m_FieldId = VALUE;
	if ( CmpName ) m_Name.m_Text = CmpName;
}

/******************************/
LibCmpEntry::~LibCmpEntry(void)
/******************************/
{
}


	/*******************************/
	/* class EDA_LibCmpAliasStruct */
	/*******************************/

/* Class to define an alias of a component
	An alias uses the component defintion (graphic, pins...)
	but has its own name and documentation.
	Therefore, when the component is modified, alias of this component are modified.
	This is a simple method to create components with differs very few
	(like 74LS00, 74HC00 ... and many op amps )
*/

EDA_LibCmpAliasStruct:: EDA_LibCmpAliasStruct( const wxChar * CmpName,
				const wxChar * CmpRootName):
	LibCmpEntry(ALIAS, CmpName)
{
	if ( CmpRootName == NULL) m_RootName.Empty();
	else m_RootName = CmpRootName;
}

EDA_LibCmpAliasStruct::~EDA_LibCmpAliasStruct(void)
{
}


	/********************************/
	/* class EDA_LibComponentStruct */
	/********************************/

/* This is a standard component  (in library)
*/
EDA_LibComponentStruct:: EDA_LibComponentStruct( const wxChar * CmpName):
	LibCmpEntry(ROOT, CmpName)
{
	m_Drawings = NULL;
	m_LastDate = 0;
	m_UnitCount = 1;
	m_TextInside = 40;
	m_Options = ENTRY_NORMAL;
	m_UnitSelectionLocked = FALSE;
	m_DrawPinNum = m_DrawPinName = 1;

	Fields = NULL;
	m_Prefix.m_FieldId = REFERENCE;
}

/******************************************************/
EDA_LibComponentStruct::~EDA_LibComponentStruct( void )
/******************************************************/
{
LibEDA_BaseStruct * DrawItem, * NextDrawItem;
LibDrawField *TempField, *field;

	field = Fields; Fields = NULL;
	while (field)
	{
		TempField = field; field = (LibDrawField *) field->Pnext;
		delete TempField;
	}

	/* suppression des elements dependants */
	DrawItem = m_Drawings; m_Drawings = NULL;
	while(DrawItem)
	{
		NextDrawItem = DrawItem->Next();
		delete DrawItem;
		DrawItem = NextDrawItem;
	}
}


/**********************************************************************/
EDA_Rect EDA_LibComponentStruct::GetBoundaryBox( int Unit, int Convert)
/**********************************************************************/
/* Return the componenty boundary box ( in user coordinates )
	The unit Unit, and the shape Convert are considered.
	If Unit == 0, Unit is not used
	if Convert == 0 Convert is non used
**/
{
int xmin, xmax, ymin, ymax, x1, y1;
int * pt, ii;
LibEDA_BaseStruct * DrawEntry;
EDA_Rect BoundaryBox;

	DrawEntry = m_Drawings;
	if ( DrawEntry )
	{
		xmin = ymin = 0x7FFFFFFF; xmax = ymax = 0x80000000;
	}
	else
	{
		xmin = ymin = -50; xmax = ymax = 50;	// Min size in 1/1000 inch
	}

	for ( ;DrawEntry != NULL; DrawEntry = DrawEntry->Next() )
	{
		if ( DrawEntry->m_Unit > 0 ) // The item is non common to units
			if ( (m_UnitCount > 1 ) && (Unit > 0) && (Unit != DrawEntry->m_Unit) )
				continue;
		if ( DrawEntry->m_Convert > 0 )	//The item is not common to alls convert
			if ( (Convert > 0) && (Convert != DrawEntry->m_Convert) )
				continue;
				
		switch ( DrawEntry->m_StructType)
		{
			case COMPONENT_ARC_DRAW_TYPE:
			{
				// Arc is reduced to a line from m_Start to m_End.
				// TO DO better.
				LibDrawArc * Arc = (LibDrawArc *) DrawEntry;
				x1 = Arc->m_ArcStart.x;
				y1 = Arc->m_ArcStart.y;
				xmin = MIN(xmin, x1);
				ymin = MIN(ymin, y1);
				xmax = MAX(xmax, x1);
				ymax = MAX(ymax, y1);
				x1 = Arc->m_ArcEnd.x;
				y1 = Arc->m_ArcEnd.y;
				xmin = MIN(xmin, x1);
				ymin = MIN(ymin, y1);
				xmax = MAX(xmax, x1);
				ymax = MAX(ymax, y1);
			}
				break;

			case COMPONENT_CIRCLE_DRAW_TYPE:
			{
				LibDrawCircle * Circle = (LibDrawCircle *) DrawEntry;
				x1 = Circle->m_Pos.x - Circle->m_Rayon;
				y1 = Circle->m_Pos.y - Circle->m_Rayon;
				xmin = MIN(xmin, x1);
				ymin = MIN(ymin, y1);
				x1 = Circle->m_Pos.x + Circle->m_Rayon;
				y1 = Circle->m_Pos.y + Circle->m_Rayon;
				xmax = MAX(xmax, x1);
				ymax = MAX(ymax, y1);
			}
				break;

			case COMPONENT_RECT_DRAW_TYPE:
			{
				LibDrawSquare * Square = (LibDrawSquare *) DrawEntry;
				xmin = MIN(xmin, Square->m_Pos.x);
				xmin = MIN(xmin, Square->m_End.x);
				xmax = MAX(xmax, Square->m_Pos.x);
				xmax = MAX(xmax, Square->m_End.x);
				ymin = MIN(ymin, Square->m_Pos.y);
				ymin = MIN(ymin, Square->m_End.y);
				ymax = MAX(ymax, Square->m_Pos.y);
				ymax = MAX(ymax, Square->m_End.y);
			}
				break;

			case COMPONENT_PIN_DRAW_TYPE:
			{
				LibDrawPin * Pin = (LibDrawPin *) DrawEntry;
				x1 = Pin->m_Pos.x;
				y1 = Pin->m_Pos.y;
				xmin = MIN(xmin, x1);
				xmax = MAX(xmax, x1);
				ymin = MIN(ymin, y1);
				ymax = MAX(ymax, y1);
#if 0	// 0 pour englober le point origine de la pin, 1 pour englober toute la pin
				switch ( Pin->Orient )
				{
					case PIN_UP: y1 += Pin->Len; break;
					case PIN_DOWN: y1 -= Pin->Len; break;
					case PIN_LEFT: x1 -= Pin->Len; break;
					case PIN_RIGHT: x1 += Pin->Len; break;
				}
				xmin = MIN(xmin, x1);
				xmax = MAX(xmax, x1);
				ymin = MIN(ymin, y1);
				ymax = MAX(ymax, y1);
#endif
			}
				break;

			case COMPONENT_GRAPHIC_TEXT_DRAW_TYPE:
				break;

			case COMPONENT_POLYLINE_DRAW_TYPE:
			{
				LibDrawPolyline * polyline = (LibDrawPolyline *) DrawEntry;
				pt = polyline->PolyList;
				for( ii = 0; ii < polyline->n; ii++ )
				{
					if( xmin > *pt ) xmin = *pt;
					if( xmax < *pt ) xmax = *pt;
					pt++;
					if( ymin > *pt ) ymin = *pt;
					if( ymax < *pt ) ymax = *pt;
					pt++;
				}
			}
				break;
		}
	}

	// Update the BoundaryBox. Remenber the fact the screen Y axis is the reverse */
	ymax = - ymax; ymin = -ymin;	// Y is is screen axis sense
	// Ensure w and H > 0 (wxRect assume it)
	if ( xmax < xmin ) EXCHG( xmax, xmin );
	if ( ymax < ymin ) EXCHG( ymax, ymin );
	BoundaryBox.SetX(xmin); BoundaryBox.SetWidth(xmax-xmin);
	BoundaryBox.SetY(ymin); BoundaryBox.SetHeight(ymax-ymin);

	return BoundaryBox;
}


	/***************************/
	/* class LibraryFieldEntry */
	/***************************/
/* a Field is a string linked to a component.
	Unlike a pure graphic text, fields can be used in netlist generation
	and other things.

	4 fields have a special meaning:
		REFERENCE
		VALUE
		FOOTPRINT NAME
		SCHEMATIC LINK (reserved but not used in kicad)
*/
LibDrawField::LibDrawField(int idfield) : LibEDA_BaseStruct(COMPONENT_FIELD_DRAW_TYPE)
{
	m_FieldId = idfield;				/* 0 a 11, 0 = REFERENCE, 1 = VALUE*/
	if ( m_FieldId < 0 ) m_FieldId = 0;
	if ( m_FieldId >= NUMBER_OF_FIELDS ) m_FieldId = NUMBER_OF_FIELDS - 1;
	m_Size.x = m_Size.y = DEFAULT_SIZE_TEXT;
	m_Orient = 0;					/* Orientation */
	m_Attributs = 0;				/* Attributs = unvisible ... */
	m_Width = 0;
	m_HJustify = GR_TEXT_HJUSTIFY_CENTER;
	m_VJustify = GR_TEXT_VJUSTIFY_CENTER;	/* Horizontal and vertical text justification */
}

LibDrawField::~LibDrawField(void)
{
}

	// Creation et Duplication d'un field
LibDrawField * LibDrawField::GenCopy(void)
{
	LibDrawField * newfield = new LibDrawField(m_FieldId);
	Copy(newfield);

	return newfield;
}

	// copie du field dans le field Target
void LibDrawField::Copy(LibDrawField * Target)
{
	Target->m_Pos = m_Pos;
	Target->m_Size = m_Size;
	Target->m_Width = m_Width;
	Target->m_Orient = m_Orient;
	Target->m_Attributs = m_Attributs;
	Target->m_Text = m_Text;
	Target->m_Name = m_Name;
	Target->m_HJustify = m_HJustify;
	Target->m_VJustify = m_VJustify;
}

/* Elements Graphiques */
LibEDA_BaseStruct::LibEDA_BaseStruct(int struct_type):
	EDA_BaseStruct(struct_type)
{
	m_Unit = 0;		/* Unit identification (for multi part per package)
					0 if the item is common to all units */
	m_Convert = 0;	/* Shape identification (for parts which have a convert shape)
					0 if the item is common to all shapes */
	m_Width = 0;	/* Default value to draw lines or arc ... */
}

/***************************************************************/
LibDrawPin::LibDrawPin(void) : LibEDA_BaseStruct(COMPONENT_PIN_DRAW_TYPE)
/***************************************************************/
{
	m_PinLen = 300;				/* default Pin len */
	m_Orient = PIN_RIGHT;		/* Pin oprient: Up, Down, Left, Right */
	m_PinShape = NONE;			/* Bit a bit: Pin shape (voir enum prec) */
	m_PinType = PIN_UNSPECIFIED;	/* electrical type of pin */
	m_Attributs = 0;			 /* bit 0 != 0: pin invisible */
	m_PinNum = 0;				/*pin number ( i.e. 4 codes Ascii ) */
	m_PinNumSize = 50;
	m_PinNameSize = 50;			/* Default size for pin name and num */
	m_Width = 0;
//	m_PinNumWidth = m_PinNameWidth = 0;	// Unused
}


/******************************************/
wxPoint LibDrawPin::ReturnPinEndPoint(void)
/******************************************/
/* return the pin end position, for a component in normal orient
*/
{
wxPoint pos = m_Pos;
	switch ( m_Orient )
	{
		case PIN_UP: pos.y += m_PinLen; break;
		case PIN_DOWN: pos.y -= m_PinLen; break;
		case PIN_LEFT: pos.x -= m_PinLen; break;
		case PIN_RIGHT: pos.x += m_PinLen; break;
	}

	return pos;
}

/********************************************************/
int LibDrawPin::ReturnPinDrawOrient(int TransMat[2][2])
/********************************************************/
/* Return the pin real orientation (PIN_UP, PIN_DOWN, PIN_RIGHT, PIN_LEFT),
	according to its orientation,
	AND the matrix transform (rot, mirror) TransMat
*/
{
int orient;
int x1 = 0, y1 = 0;
int t1, t2;
	
	switch ( m_Orient )
	{
		case PIN_UP: y1 = 1; break;
		case PIN_DOWN: y1 = -1; break;
		case PIN_LEFT: x1 = -1; break;
		case PIN_RIGHT: x1 = 1; break;
	}
	t1 = TransMat[0][0] * x1 + TransMat[0][1] * y1;
	t2 = TransMat[1][0] * x1 + TransMat[1][1] * y1;
	orient = PIN_UP;
	if( t1 == 0 )
	{
		if ( t2 > 0 ) orient = PIN_DOWN;
	}
	else
	{
		orient = PIN_RIGHT;
		if ( t1 < 0 ) orient = PIN_LEFT;
	}

	return orient;
}


/****************************************************/
void LibDrawPin::ReturnPinStringNum(wxString & buffer)
/****************************************************/
/* fill the buffer with pin num as a wxString
	Pin num is coded as a long
	Used to print/draw the pin num
*/
{
char ascii_buf[5];
	
	strncpy(ascii_buf, (char*)&m_PinNum,4);
	ascii_buf[4]=0;

	buffer = CONV_FROM_UTF8(ascii_buf);
}
/****************************************************/
void LibDrawPin::SetPinNumFromString(wxString & buffer)
/****************************************************/
/* fill the buffer with pin num as a wxString
	Pin num is coded as a long
	Used to print/draw the pin num
*/
{
char ascii_buf[4];
unsigned ii, len = buffer.Len();
	
	ascii_buf[0] = ascii_buf[1] = ascii_buf[2] = ascii_buf[3] = 0;
	if ( len > 4 ) len = 4;
	for ( ii = 0; ii < len; ii ++ )
	{
		ascii_buf[ii] = buffer.GetChar(ii) & 0xFF;
	}
	strncpy((char*)&m_PinNum, ascii_buf, 4);
}

/*************************************/
LibDrawPin * LibDrawPin::GenCopy(void)
/*************************************/
{
LibDrawPin * newpin = new LibDrawPin();

	newpin->m_Pos = m_Pos;
	newpin->m_PinLen = m_PinLen;
	newpin->m_Orient = m_Orient;
	newpin->m_PinShape = m_PinShape;
	newpin->m_PinType = m_PinType;
	newpin->m_Attributs = m_Attributs;
	newpin->m_PinNum = m_PinNum;
	newpin->m_PinNumSize = m_PinNumSize;
	newpin->m_PinNameSize = m_PinNameSize;
	newpin->m_Unit = m_Unit;
	newpin->m_Convert = m_Convert;
	newpin->m_Flags = m_Flags;
	newpin->m_Width = m_Width;

	newpin->m_PinName = m_PinName;

	return newpin;
}

/**************************************************************/
LibDrawArc::LibDrawArc(void) : LibEDA_BaseStruct(COMPONENT_ARC_DRAW_TYPE)
/**************************************************************/
{
	m_Rayon = 0;
	t1 = t2 = 0;
	m_Width = 0;
	m_Fill = NO_FILL;
}


/************************************/
LibDrawArc * LibDrawArc::GenCopy(void)
/************************************/
{
LibDrawArc * newitem = new LibDrawArc();

	newitem->m_Pos = m_Pos;
	newitem->m_ArcStart = m_ArcStart;
	newitem->m_ArcEnd = m_ArcEnd;
	newitem->m_Rayon = m_Rayon;
	newitem->t1 = t1;
	newitem->t2 = t2;
	newitem->m_Width = m_Width;
	newitem->m_Unit = m_Unit;
	newitem->m_Convert = m_Convert;
	newitem->m_Flags = m_Flags;
	newitem->m_Fill = m_Fill;
	return newitem;
}


/**********************************************************************/
LibDrawCircle::LibDrawCircle(void) : LibEDA_BaseStruct(COMPONENT_CIRCLE_DRAW_TYPE)
/**********************************************************************/
{
	m_Rayon = 0;
	m_Fill = NO_FILL;
}

/*******************************************/
LibDrawCircle * LibDrawCircle::GenCopy(void)
/*******************************************/
{
LibDrawCircle * newitem = new LibDrawCircle();

	newitem->m_Pos = m_Pos;
	newitem->m_Rayon = m_Rayon;
	newitem->m_Width = m_Width;
	newitem->m_Unit = m_Unit;
	newitem->m_Convert = m_Convert;
	newitem->m_Flags = m_Flags;
	newitem->m_Fill = m_Fill;
	return newitem;
}


/*****************************************************************/
LibDrawText::LibDrawText(void) : LibEDA_BaseStruct(COMPONENT_GRAPHIC_TEXT_DRAW_TYPE)
/*****************************************************************/
{
	m_Horiz = TEXT_ORIENT_HORIZ;
	m_Size = wxSize(50,50);
	m_Type = 0;
	m_Width = 0;
}

/***************************************/
LibDrawText * LibDrawText::GenCopy(void)
/***************************************/
{
LibDrawText * newitem = new LibDrawText();

	newitem->m_Pos = m_Pos;
	newitem->m_Horiz = m_Horiz;
	newitem->m_Size = m_Size;
	newitem->m_Type = m_Type;
	newitem->m_Unit = m_Unit;
	newitem->m_Convert = m_Convert;
	newitem->m_Flags = m_Flags;
	newitem->m_Text = m_Text;
	newitem->m_Width = m_Width;
	return newitem;
}



LibDrawSquare::LibDrawSquare(void) : LibEDA_BaseStruct(COMPONENT_RECT_DRAW_TYPE)
{
	m_Width = 0;
	m_Fill = NO_FILL;
}

LibDrawSquare * LibDrawSquare::GenCopy(void)
{
LibDrawSquare * newitem = new LibDrawSquare();

	newitem->m_Pos = m_Pos;
	newitem->m_End = m_End;
	newitem->m_Width = m_Width;
	newitem->m_Unit = m_Unit;
	newitem->m_Convert = m_Convert;
	newitem->m_Flags = m_Flags;
	newitem->m_Fill = m_Fill;
	return newitem;
}

LibDrawSegment::LibDrawSegment(void) : LibEDA_BaseStruct(COMPONENT_LINE_DRAW_TYPE)
{
	m_Width = 0;
}

LibDrawSegment * LibDrawSegment::GenCopy(void)
{
LibDrawSegment * newitem = new LibDrawSegment();

	newitem->m_Pos = m_Pos;
	newitem->m_End = m_End;
	newitem->m_Width = m_Width;
	newitem->m_Unit = m_Unit;
	newitem->m_Convert = m_Convert;
	newitem->m_Flags = m_Flags;
	return newitem;
}


LibDrawPolyline::LibDrawPolyline(void) : LibEDA_BaseStruct(COMPONENT_POLYLINE_DRAW_TYPE)
{
	n = 0;
	PolyList = NULL;
	m_Fill = NO_FILL;
	m_Width = 0;
}

/************************************************/
LibDrawPolyline * LibDrawPolyline::GenCopy(void)
/************************************************/
{
LibDrawPolyline * newitem = new LibDrawPolyline();
int size;

	newitem->n = n;
	size = sizeof(int) * 2 * n;
	if( size )
	{
		newitem->PolyList = (int*)MyMalloc(size);
		memcpy(newitem->PolyList, PolyList, size);
	}
	newitem->m_Pos = m_Pos;
	newitem->m_Width = m_Width;
	newitem->m_Unit = m_Unit;
	newitem->m_Convert = m_Convert;
	newitem->m_Flags = m_Flags;
	newitem->m_Fill = m_Fill;
	return newitem;
}

/***************************************************/
void LibDrawPolyline::AddPoint(const wxPoint & point)
/***************************************************/
/* add a point to the polyline coordinate list, and realloc the memory
*/
{
int allocsize;

	n++;
	allocsize =  2 * sizeof(int) * n;
	if (PolyList == NULL)
		PolyList = (int*)MyMalloc( allocsize );
	else
		PolyList = (int*)realloc(PolyList, allocsize );

	PolyList[(n*2) - 2] = point.x;
	PolyList[(n*2) - 1] = - point.y;
}


