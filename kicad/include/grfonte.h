	/* grfonte.h : codage de la fonte de caracteres utilisee en trace de
		textes graphiques */
/*
formes des caracteres : definies sur matrice 10 x 13 sour forme de vecteurs
d'origine le debut du caractere (poSH_CODE matrice 0,0) sous forme d'une sequence
ascii .les lignes au dessous de 0,0 sont notees a,b,c.
conventions :
	coord 0...9 : ascii '0'...'9' (2 octets successifs = 1 coord)
	'U'		montee de plume
	'D'		descente de plume
	'X'		fin de trace

la matrice totale du caractere est ici 12x9. et l'espacement est de 13 points
	-2..0 = jambage au dessous de la ligne
	0..9 = cadre matrice
Version actuelle:
	en X : dim 0..6 a ramener en 0..9 ( espacement = 13 )
Matrice:

	9  ----------
	8  ----------
	7  ----------
	6  ----------
	5  ----------
	4  ----------
	3  ----------
	2  ----------
	1  ----------
___ 0  ----------
	-1 ----------
	-2 ----------
	-3 ----------
	   0123456789---

	dans les descr ci dessous:
		X = fin de descr
		U = plume Up
		D = plume Down
		n, n = 2 nombres = coord X et Y ( ici = -3 ... +9 )
*/

#ifndef _GRFONTE_H_
#define _GRFONTE_H_

#define SH_CODE signed char
extern const SH_CODE *graphic_fonte_shape[256];

#if defined(wxUSE_UNICODE) && defined(KICAD_CYRILLIC)
extern const int utf8_to_ascii[64];
#endif

#ifdef EDA_DRAWBASE

#define Up (SH_CODE)'U'
#define X (SH_CODE)'X'
#define Dn (SH_CODE)'D'

const SH_CODE noshape[] = {X};		//code non inscriptible
const SH_CODE char_shape_space[] = {Up,X};			/* space */
const SH_CODE char_shape_ipoint[] = {Up,0,4,Dn,1,4,Up,4,4,Dn,9,4,Up,X};			/*!*/
const SH_CODE char_shape_openacc[] = {Up,8,3,Dn,9,4,Up,8,5,Dn,9,6,Up,X};			/*{*/
const SH_CODE char_shape_dieze[] = {Up,3,1,Dn,3,7,Up,5,2,Dn,5,8,Up,1,3,Dn,7,4,Up,1,5,Dn,7,6,Up,X};		/*#*/
const SH_CODE char_shape_dollar[] = {Up,0,4,Dn,9,4,Up,0,5,Dn,9,5,Up,0,2,Dn,0,7,1,8,3,8,4,7,4,2,5,1,7,1,8,2,8,7,Up,X};	/*$*/
const SH_CODE char_shape_percent[] = {Up,0,1,Dn,9,8,Up,9,1,Dn,7,1,7,3,9,3,9,1,Up,0,6,Dn,0,8,2,8,2,6,0,6,Up,X};	/*%*/
const SH_CODE char_shape_and[] = {Up,0,7,Dn,7,2,8,2,9,4,8,6,7,6,2,1,1,1,0,3,0,4,1,6,2,7,Up,X};			/*&*/
const SH_CODE char_shape_el[] = {Up,9,4,Dn,7,3,Up,X};				/*'*/
const SH_CODE char_shape_openpar[] = {Up,0,4,Dn,2,3,7,3,9,4,Up,X};				/*(*/
const SH_CODE char_shape_closepar[] = {Up,0,4,Dn,2,5,7,5,9,4,Up,X};				/*)*/
const SH_CODE char_shape_star[] = {Up,1,4,Dn,7,4,Up,4,1,Dn,4,7,Up,2,2,Dn,6,6,Up,6,2,Dn,2,6,Up,X};		/***/
const SH_CODE char_shape_plus[] = {Up,4,1,Dn,4,7,Up,1,4,Dn,7,4,Up,X};					/*+*/
const SH_CODE char_shape_comma[] = {Up,1,4,Dn,0,4,-3,3,Up,X};							/*,*/
const SH_CODE char_shape_minus[] = {Up,4,2,Dn,4,7,Up,X};							/*-*/
const SH_CODE char_shape_point[] = {Up,0,4,Dn,1,4,Up,X};							/*.*/
const SH_CODE char_shape_slash[] = {Up,0,2,Dn,9,6,Up,X};							/* / */

const SH_CODE char_shape_0[] = {Up,0,1,Dn,9,8,Up,1,1,Dn,8,1,9,2,9,7,8,8,1,8,0,7,0,2,1,1,Up,X};		/*0*/
const SH_CODE char_shape_1[] = {Up,0,3,Dn,0,7,Up,0,5,Dn,9,5,Up,8,5,Dn,6,3,Up,X};				/*1*/
const SH_CODE char_shape_2[] = {Up,0,8,Dn,0,1,3,1,4,2,4,7,5,8,8,8,9,7,9,2,8,1,Up,X};			/*2*/
const SH_CODE char_shape_3[] = {Up,9,1,Dn,9,8,8,8,5,4,5,7,4,8,1,8,0,7,0,2,1,1,Up,X};			/*3*/
const SH_CODE char_shape_4[] = {Up,0,6,Dn,9,6,2,1,2,8,Up,X};						/*4*/
const SH_CODE char_shape_5[] = {Up,2,1,Dn,1,1,0,2,0,7,1,8,4,8,6,6,6,1,9,1,9,8,Up,X};			/*5*/
const SH_CODE char_shape_6[] = {Up,8,8,Dn,9,7,9,2,8,1,1,1,0,2,0,7,1,8,4,8,5,7,5,2,4,1,Up,X};		/*6*/
const SH_CODE char_shape_7[] = {Up,9,1,Dn,9,8,0,1,Up,X};						/*7*/
const SH_CODE char_shape_8[] = {Up,0,2,Dn,1,1,4,1,5,2,5,7,6,8,8,8,9,7,9,2,8,1,6,1,5,2,Up,5,7,Dn,4,8,1,8,0,7,0,2,Up,X}; /*8*/
const SH_CODE char_shape_9[] = {Up,0,2,Dn,0,7,1,8,8,8,9,7,9,2,8,1,5,1,4,2,4,7,5,8,Up,X};		/*9*/

const SH_CODE char_shape_dbpoint[] = {Up,6,4,Dn,5,4,Up,3,4,Dn,2,4,Up,X};			/*:*/
const SH_CODE char_shape_vpoint[] = {Up,5,4,Dn,4,4,Up,2,4,Dn,1,4,-2,3,Up,X};			/*;*/
const SH_CODE char_shape_less[] = {Up,0,7,Dn,4,2,9,7,Up,X};				/*<*/
const SH_CODE char_shape_egal[] = {Up,2,2,Dn,2,7,Up,5,2,Dn,5,7,Up,X};			/*=*/
const SH_CODE char_shape_more[] = {Up,0,2,Dn,4,7,9,2,Up,X};				/*>*/
const SH_CODE char_shape_intpoint[] = {Up,0,4,Dn,1,4,Up,3,4,Dn,4,4,5,6,6,7,8,7,9,6,9,2,8,1,Up,X};		/*?*/
const SH_CODE char_shape_arond[] = {Up,0,8,Dn,0,3,2,1,6,1,8,3,8,6,6,8,3,8,3,5,4,4,5,5,5,8,Up,X};		/*@*/

const SH_CODE char_shape_A[] = {Up,0,1,Dn,7,1,9,3,9,6,7,8,0,8,Up,5,1,Dn,5,8,Up,X};		/*A*/
const SH_CODE char_shape_B[] = {Up,0,1,Dn,9,1,9,7,8,8,6,8,5,7,5,1,Up,5,7,Dn,3,8,1,8,0,7,0,1,Up,X};	/*B*/
const SH_CODE char_shape_C[] = {Up,1,8,Dn,0,7,0,2,1,1,8,1,9,2,9,7,8,8,Up,X};			/*C*/
const SH_CODE char_shape_D[] = {Up,0,2,Dn,9,2,Up,9,1,Dn,9,7,8,8,1,8,0,7,0,1,Up,X};		/*D*/
const SH_CODE char_shape_E[] = {Up,0,8,Dn,0,1,9,1,9,8,Up,5,1,Dn,5,5,Up,X};			/*E*/
const SH_CODE char_shape_F[] = {Up,0,1,Dn,9,1,9,8,Up,5,1,Dn,5,5,Up,X};				/*F*/
const SH_CODE char_shape_G[] = {Up,8,7,Dn,9,7,9,2,8,1,1,1,0,2,0,7,1,8,4,8,4,5,Up,X};		/*G*/
const SH_CODE char_shape_H[] = {Up,0,1,Dn,9,1,Up,0,8,Dn,9,8,Up,4,1,Dn,4,8,Up,X};			/*H*/
const SH_CODE char_shape_I[] = {Up,0,2,Dn,0,6,Up,0,4,Dn,9,4,Up,9,2,Dn,9,6,Up,X};			/*I*/
const SH_CODE char_shape_J[] = {Up,2,1,Dn,0,2,0,4,1,5,9,5,Up,9,3,Dn,9,7,Up,X};			/*J*/
const SH_CODE char_shape_K[] = {Up,0,1,Dn,9,1,Up,9,7,Dn,5,1,0,8,Up,X};			/*K*/
const SH_CODE char_shape_L[] = {Up,9,1,Dn,0,1,0,9,Up,X};				/*L*/
const SH_CODE char_shape_M[] = {Up,0,1,Dn,9,1,5,5,9,9,0,9,Up,X};			/*M*/
const SH_CODE char_shape_N[] = {Up,0,1,Dn,9,1,0,8,9,8,Up,X};				/*N*/
const SH_CODE char_shape_O[] = {Up,0,2,Dn,1,1,8,1,9,2,9,7,8,8,1,8,0,7,0,2,Up,X};		/*O*/
const SH_CODE char_shape_P[] = {Up,0,1,Dn,9,1,9,7,8,8,5,8,4,7,4,1,Up,X};			/*P*/
const SH_CODE char_shape_Q[] = {Up,1,1,Dn,8,1,9,2,9,7,8,8,1,8,0,7,0,2,1,1,Up,4,6,Dn,0,8,Up,X};		/*Q*/
const SH_CODE char_shape_R[] = {Up,0,1,Dn,9,1,9,7,8,8,5,8,4,7,4,1,Up,4,4,Dn,0,8,Up,X};		/*R*/
const SH_CODE char_shape_S[] = {Up,1,1,Dn,0,2,0,7,1,8,4,8,5,7,5,2,6,1,8,1,9,2,9,7,8,8,Up,X};		/*S*/
const SH_CODE char_shape_T[] = {Up,9,1,Dn,9,9,Up,9,5,Dn,0,5,Up,X};			/*T*/
const SH_CODE char_shape_U[] = {Up,9,1,Dn,1,1,0,2,0,7,1,8,9,8,Up,X};			/*Up,*/
const SH_CODE char_shape_V[] = {Up,9,1,Dn,0,5,9,9,Up,X};				/*V*/
const SH_CODE char_shape_W[] = {Up,9,1,Dn,0,3,3,5,0,7,9,9,Up,X};			/*W*/
const SH_CODE char_shape_X[] = {Up,0,1,Dn,9,8,Up,9,1,Dn,0,8,Up,X};			/*X*/
const SH_CODE char_shape_Y[] = {Up,9,1,Dn,4,5,9,9,Up,4,5,Dn,0,5,Up,X};			/*Y*/
const SH_CODE char_shape_Z[] = {Up,9,1,Dn,9,8,0,1,0,8,Up,X};				/*Z*/

const SH_CODE char_shape_opencr[] = {Up,0,5,Dn,0,3,9,3,9,5,Up,X};				/*[*/
const SH_CODE char_shape_backslash[] = {Up,9,1,Dn,0,7,Up,X};				/*\*/
const SH_CODE char_shape_closecr[] = {Up,0,3,Dn,0,5,9,5,9,3,Up,X};				/*]*/
const SH_CODE char_shape_xor[] = {Up,7,1,Dn,9,5,7,8,Up,X};				/*^*/
const SH_CODE char_shape_underscore[] = {Up,0,1,Dn,0,8,Up,X};				/*_*/
const SH_CODE char_shape_altel[] = {Up,9,4,Dn,7,5,Up,X};				/*`*/

const SH_CODE char_shape_a[] = {Up,6,1,Dn,6,7,5,8,0,8,0,2,1,1,2,1,3,2,3,8,Up,X};		/*a*/
const SH_CODE char_shape_b[] = {Up,9,1,Dn,0,1,0,8,4,8,5,7,5,1,Up,X};		/*b*/
const SH_CODE char_shape_c[] = {Up,1,8,Dn,0,7,0,2,1,1,5,1,6,2,6,7,5,8,Up,X};			/*c*/
const SH_CODE char_shape_d[] = {Up,9,8,Dn,0,8,0,2,1,1,5,1,6,2,6,8,Up,X};		/*d*/
const SH_CODE char_shape_e[] = {Up,3,1,Dn,3,8,5,8,6,7,6,2,5,1,1,1,0,2,0,7,1,8,Up,X};		/*e*/
const SH_CODE char_shape_f[] = {Up,0,2,Dn,8,2,9,3,9,7,8,8,Up,5,1,Dn,5,4,Up,X};			/*f*/
const SH_CODE char_shape_g[] = {Up,-2,1,Dn,-3,2,-3,7,-2,8,5,8,6,7,6,2,5,1,1,1,0,2,0,7,1,8,Up,X};		/*g*/
const SH_CODE char_shape_h[] = {Up,0,1,Dn,9,1,Up,5,1,Dn,6,2,6,7,5,8,0,8,Up,X};			/*h*/
const SH_CODE char_shape_i[] = {Up,9,4,Dn,8,4,Up,5,4,Dn,1,4,0,5,0,6,Up,X};			/*i*/
const SH_CODE char_shape_j[] = {Up,9,4,Dn,8,4,Up,5,4,Dn,-2,4,-3,3,-2,2,Up,X};			/*j*/
const SH_CODE char_shape_k[] = {Up,0,2,Dn,9,2,Up,3,2,Dn,3,3,6,8,Up,3,3,Dn,0,8,Up,X};		/*k*/
const SH_CODE char_shape_l[] = {Up,9,3,Dn,9,4,0,4,Up,0,3,Dn,0,5,Up,X};			/*l*/
const SH_CODE char_shape_m[] = {Up,0,1,Dn,6,1,Up,5,1,Dn,6,2,6,4,5,5,0,5,Up,5,5,Dn,6,6,6,8,5,9,0,9,Up,X};	/*m*/
const SH_CODE char_shape_n[] = {Up,0,1,Dn,6,1,Up,5,1,Dn,6,2,6,6,5,7,0,7,Up,X};			/*n*/
const SH_CODE char_shape_o[] = {Up,0,2,Dn,1,1,5,1,6,2,6,7,5,8,1,8,0,7,0,2,Up,X};		/*o*/
const SH_CODE char_shape_p[] = {Up,-3,1,Dn,6,1,Up,5,1,Dn,6,2,6,7,5,8,1,8,0,7,0,2,1,1,Up,X};		/*p*/
const SH_CODE char_shape_q[] = {Up,-3,8,Dn,6,8,Up,5,8,Dn,6,7,6,2,5,1,1,1,0,2,0,7,1,8,Up,X};		/*q*/
const SH_CODE char_shape_r[] = {Up,0,1,Dn,6,1,Up,4,1,Dn,6,3,6,6,4,8,Up,X};			/*r*/
const SH_CODE char_shape_s[] = {Up,1,1,Dn,0,2,0,7,1,8,2,8,3,7,3,2,4,1,5,1,6,2,6,7,5,8,Up,X};		/*s*/
const SH_CODE char_shape_t[] = {Up,1,7,Dn,0,6,0,3,1,2,9,2,Up,6,2,Dn,6,5,Up,X};			/*t*/
const SH_CODE char_shape_u[] = {Up,6,1,Dn,2,1,0,3,0,6,2,8,6,8,Up,X};			/*u*/
const SH_CODE char_shape_v[] = {Up,6,1,Dn,3,1,0,4,3,7,6,7,Up,X};			/*v*/
const SH_CODE char_shape_w[] = {Up,6,1,Dn,2,1,0,3,2,5,0,7,2,9,6,9,Up,X};			/*w*/
const SH_CODE char_shape_x[] = {Up,0,1,Dn,6,8,Up,6,1,Dn,0,8,Up,X};			/*x*/
const SH_CODE char_shape_y[] = {Up,6,1,Dn,2,1,0,3,0,6,2,8,Up,6,8,Dn,-2,8,-3,7,-3,2,-2,1,Up,X}; /*y*/
const SH_CODE char_shape_z[] = {Up,6,1,Dn,6,8,0,1,0,8,Up,X}; /*z*/

const SH_CODE char_shape_opbrack[] = {Up,9,5,Dn,8,4,6,4,5,3,4,3,3,4,1,4,0,5,Up,X}; /*{*/
const SH_CODE char_shape_or[] = {Up,9,4,Dn,0,4,Up,X};				/*|*/
const SH_CODE char_shape_closebr[] = {Up,9,3,Dn,8,4,6,4,5,5,4,5,3,4,1,4,0,3,Up,X}; /*}*/
const SH_CODE char_shape_tilda[] = {Up,8,1,Dn,9,2,9,3,7,5,7,6,8,7,Up,X}; /*~*/
const SH_CODE char_shape_del[] = {X}; /*<del>*/

/* codes utiles >= 128 */
const SH_CODE char_shape_C_Cedille[] = {Up,1,8,Dn,0,7,0,2,1,1,8,1,9,2,9,7,8,8,Up,X}; /* C cedille*/
const SH_CODE char_shape_c_cedille[] = {Up,1,8,Dn,0,7,0,2,1,1,5,1,6,2,6,7,5,8,Up,0,4,Dn,-3,2,Up,X}; /* � */
const SH_CODE char_shape_a_grave[] = {Up,6,1,Dn,6,7,5,8,0,8,0,2,1,1,2,1,3,2,3,8,Up,7,4,Dn,9,2,Up,X};
const SH_CODE char_shape_a_aigu[] = {Up,6,1,Dn,6,7,5,8,0,8,0,2,1,1,2,1,3,2,3,8,Up,7,2,Dn,9,4,Up,X};
const SH_CODE char_shape_a_circ[] = {Up,6,1,Dn,6,7,5,8,0,8,0,2,1,1,2,1,3,2,3,8,Up,7,1,Dn,9,4,7,7,Up,X};
const SH_CODE char_shape_a_trema[] = {Up,6,1,Dn,6,7,5,8,0,8,0,2,1,1,2,1,3,2,3,8,Up,9,2,Dn,9,3,Up,9,5,Dn,9,6,Up,X};
const SH_CODE char_shape_e_grave[] = {Up,3,1,Dn,3,8,5,8,6,7,6,2,5,1,1,1,0,2,0,7,1,8,Up,7,4,Dn,9,2,Up,X};
const SH_CODE char_shape_e_aigu[] = {Up,3,1,Dn,3,8,5,8,6,7,6,2,5,1,1,1,0,2,0,7,1,8,Up,7,3,Dn,9,5,Up,X};
const SH_CODE char_shape_e_circ[] = {Up,3,1,Dn,3,8,5,8,6,7,6,2,5,1,1,1,0,2,0,7,1,8,Up,7,1,Dn,9,4,7,7,Up,X};
const SH_CODE char_shape_e_trema[] = {Up,3,1,Dn,3,8,5,8,6,7,6,2,5,1,1,1,0,2,0,7,1,8,Up,9,2,Dn,9,3,Up,9,5,Dn,9,6,Up,X};
const SH_CODE char_shape_i_trema[] = {Up,5,4,Dn,1,4,0,5,0,6,Up,9,2,Dn,9,3,Up,9,5,Dn,9,6,Up,X};
const SH_CODE char_shape_i_circ[] = {Up,5,4,Dn,1,4,0,5,0,6,Up,7,1,Dn,9,4,7,7,Up,X};
const SH_CODE char_shape_u_grave[] = {Up,6,1,Dn,2,1,0,3,0,6,2,8,6,8,Up,7,4,Dn,9,2,Up,X};
const SH_CODE char_shape_o_trema[] = {Up,0,2,Dn,1,1,5,1,6,2,6,7,5,8,1,8,0,7,0,2,Up,9,2,Dn,9,3,Up,9,5,Dn,9,6,Up,X};
const SH_CODE char_shape_o_circ[] = {Up,0,2,Dn,1,1,5,1,6,2,6,7,5,8,1,8,0,7,0,2,Up,7,2,Dn,9,4,7,7,Up,X};
const SH_CODE char_shape_u_circ[] = {Up,6,1,Dn,2,1,0,3,0,6,2,8,6,8,Up,7,1,Dn,9,4,7,7,Up,X};
const SH_CODE char_shape_u_trema[] = {Up,6,1,Dn,2,1,0,3,0,6,2,8,6,8,Up,9,2,Dn,9,3,Up,9,5,Dn,9,6,Up,X};

#if defined(KICAD_CYRILLIC)
const SH_CODE char_shape_yu[] = {Up,6,0,Dn,0,0,Up,3,0,Dn,3,2,Up,0,3,Dn,1,2,5,2,6,3,6,7,5,8,1,8,0,7,0,3,Up,X}; // ю
const SH_CODE char_shape_YU[] = {Up,9,0,Dn,0,0,Up,4,0,Dn,4,2,Up,0,3,Dn,1,2,8,2,9,3,9,7,8,8,1,8,0,7,0,3,Up,X}; // Ю
const SH_CODE char_shape_be[] = {Up,6,7,Dn,6,2,5,1,1,1,0,2,0,7,1,8,5,8,6,7,9,1,9,8,Up,X}; // б
const SH_CODE char_shape_BE[] = {Up,9,8,Dn,9,1,0,1,0,7,1,8,4,8,5,7,5,1,Up,X}; // Б
const SH_CODE char_shape_tse[] = {Up,6,1,Dn,1,1,0,2,0,8,6,8,Up,0,8,Dn,0,9,-2,9,Up,X}; // ц
const SH_CODE char_shape_TSE[] = {Up,9,1,Dn,0,1,0,9,-2,9,Up,9,8,Dn,0,8,Up,X}; // Ц
const SH_CODE char_shape_de[] = {Up,9,3,Dn,9,7,8,8,1,8,0,7,0,2,1,1,5,1,6,2,6,8,Up,X}; // д
const SH_CODE char_shape_DE[] = {Up,0,2,Dn,9,3,9,7,0,7,Up,-1,1,Dn,0,1,0,8,-1,8,Up,X}; // Д
const SH_CODE char_shape_ee[] = {Up,3,1,Dn,3,8,5,8,6,7,6,2,5,1,1,1,0,2,0,8,Up,X}; //e
const SH_CODE char_shape_EE[] = {Up,9,8,Dn,9,1,0,1,0,8,Up,5,1,Dn,5,7,Up,X}; //E
const SH_CODE char_shape_ef[] = {Up,7,4,Dn,-3,4,Up,5,1,Dn,6,2,6,6,5,7,1,7,0,6,0,2,1,1,5,1,Up,X}; // ф
const SH_CODE char_shape_EF[] = {Up,9,4,Dn,0,4,Up,7,0,Dn,8,1,8,7,7,8,3,8,2,7,2,1,3,0,7,0,Up,X}; // Ф
const SH_CODE char_shape_ghe[] = {Up,5,1,Dn,6,2,6,7,5,8,4,8,3,7,3,2,2,1,1,1,0,2,0,7,1,8,Up,X}; // г
const SH_CODE char_shape_GHE[] = {Up,0,1,Dn,9,1,9,8,Up,X}; // Г
const SH_CODE char_shape_xe[] = {Up,0,1,Dn,6,8,Up,6,1,Dn,0,8,Up,X}; // x
const SH_CODE char_shape_XE[] = {Up,0,1,Dn,9,8,Up,9,1,Dn,0,8,Up,X}; // X
const SH_CODE char_shape_ii[] = {Up,6,1,Dn,1,1,0,2,0,3,1,8,Up,0,8,Dn,6,8,Up,X}; // и
const SH_CODE char_shape_II[] = {Up,0,1,Dn,9,1,Up,1,1,Dn,8,8,Up,9,8,Dn,0,8,Up,X}; // И
const SH_CODE char_shape_shorti[] = {Up,6,1,Dn,1,1,0,2,0,3,1,8,Up,0,8,Dn,6,8,Up,8,3,Dn,8,6,Up,X}; // й
const SH_CODE char_shape_SHORTI[] = {Up,9,1,Dn,0,1,Up,1,1,Dn,8,8,Up,9,8,Dn,0,8,Up,9,3,Dn,9,6,Up,X}; // Й
const SH_CODE char_shape_ka[] = {Up,6,1,Dn,0,1,Up,3,1,Dn,6,8,Up,4,3,Dn,0,8,Up,X}; //к
const SH_CODE char_shape_KA[] = {Up,9,1,Dn,0,1,Up,4,1,Dn,9,8,Up,5,3,Dn,0,8,Up,X}; //K
const SH_CODE char_shape_letterel[] = {Up,0,1,Dn,6,5,6,8,0,8,Up,X}; // л
const SH_CODE char_shape_EL[] = {Up,0,1,Dn,9,5,9,8,0,8,Up,X}; // Л
const SH_CODE char_shape_em[] = {Up,0,0,Dn,6,0,3,4,6,8,0,8,Up,X}; // м
//const SH_CODE char_shape_M[] = {X}; // М
const SH_CODE char_shape_en[] = {Up,6,1,Dn,0,1,Up,6,8,Dn,0,8,Up,3,1,Dn,3,8,Up,X}; // н
const SH_CODE char_shape_EN[] = {Up,9,1,Dn,0,1,Up,9,8,Dn,0,8,Up,5,1,Dn,5,8,Up,X}; // H
const SH_CODE char_shape_pe[] = {Up,0,1,Dn,6,1,6,7,5,8,0,8,Up,X}; // п
const SH_CODE char_shape_PE[] = {Up,0,1,Dn,9,1,9,8,0,8,Up,X}; // П
const SH_CODE char_shape_ya[] = {Up,0,8,Dn,6,8,6,2,5,1,4,1,3,2,3,8,Up,3,3,Dn,0,1,Up,X}; // я
const SH_CODE char_shape_YA[] = {Up,0,8,Dn,9,8,9,2,8,1,5,1,4,2,4,8,Up,4,3,Dn,0,1,Up,X}; // Я
const SH_CODE char_shape_te[] = {Up,0,1,Dn,6,1,6,6,5,7,0,7,Up,6,4,Dn,0,4,Up,X}; // т == m
const SH_CODE char_shape_TE[] = {Up,0,4,Dn,9,4,Up,9,1,Dn,9,7,Up,X}; // Т
const SH_CODE char_shape_uu[] = {Up,6,1,Dn,1,1,0,2,0,8,6,8,Up,0,8,Dn,-2,8,-3,7,-3,1,Up,X}; // у - latin y
const SH_CODE char_shape_UU[] = {Up,9,1,Dn,5,1,4,2,4,8,Up,9,8,Dn,1,8,0,7,0,1,Up,X}; // У
const SH_CODE char_shape_zhe[] = {Up,0,0,Dn,6,8,Up,6,0,Dn,0,8,Up,6,4,Dn,0,4,Up,X}; // ж
const SH_CODE char_shape_ZHE[] = {Up,0,0,Dn,9,8,Up,9,0,Dn,0,8,Up,9,4,Dn,0,4,Up,X}; // Ж
const SH_CODE char_shape_ve[] = {Up,5,1,Dn,6,2,6,7,5,8,1,8,0,7,0,2,1,1,8,1,9,2,9,4,8,5,7,5,6,4,Up,X}; // в
const SH_CODE char_shape_VE[] = {Up,5,1,Dn,5,7,4,8,1,8,0,7,0,1,9,1,9,7,8,8,6,8,5,7,Up,X}; // В
const SH_CODE char_shape_softsign[] = {Up,6,1,Dn,0,1,0,7,1,8,2,8,3,7,3,1,Up,X}; // ь
const SH_CODE char_shape_SOFTSIGN[] = {Up,9,1,Dn,0,1,0,7,1,8,4,8,5,7,5,1,Up,X}; // Ь
const SH_CODE char_shape_yeru[] = {Up,6,1,Dn,0,1,0,6,1,7,2,7,3,6,3,1,Up,6,8,Dn,0,8,Up,X}; // ы
const SH_CODE char_shape_YERU[] = {Up,9,1,Dn,0,1,0,6,1,7,3,7,4,6,4,1,Up,9,8,Dn,0,8,Up,X}; // Ы
const SH_CODE char_shape_ze[] = {Up,5,1,Dn,6,2,6,7,5,8,4,8,3,7,3,5,Up,3,7,Dn,2,8,1,8,0,7,0,2,1,1,Up,X}; // з
const SH_CODE char_shape_ZE[] = {Up,8,1,Dn,9,2,9,7,8,8,6,8,5,7,5,4,Up,5,7,Dn,4,8,1,8,0,7,0,2,1,1,Up,X}; // З
const SH_CODE char_shape_sha[] = {Up,6,0,Dn,1,0,0,1,0,8,6,8,Up,6,4,Dn,0,4,Up,X}; // ш
const SH_CODE char_shape_SHA[] = {Up,9,0,Dn,0,0,0,8,9,8,Up,9,4,Dn,0,4,Up,X}; // Ш
const SH_CODE char_shape_ye[] = {Up,0,1,Dn,0,7,1,8,5,8,6,7,6,1,Up,3,4,Dn,3,8,Up,X}; // э
const SH_CODE char_shape_YE[] = {Up,0,1,Dn,0,7,1,8,8,8,9,7,9,1,Up,5,4,Dn,5,8,Up,X}; // Э
const SH_CODE char_shape_shcha[] = {Up,6,0,Dn,1,0,0,1,0,8,6,8,Up,6,4,Dn,0,4,Up,0,8,Dn,0,9,-2,9,Up,X}; // щ
const SH_CODE char_shape_SHCHA[] = {Up,9,0,Dn,0,0,0,8,9,8,Up,9,4,Dn,0,4,Up,0,8,Dn,0,9,-2,9,Up,X}; // Щ
const SH_CODE char_shape_che[] = {Up,6,1,Dn,4,1,3,2,3,8,Up,6,8,Dn,0,8,Up,X}; // ч
const SH_CODE char_shape_CHE[] = {Up,9,1,Dn,5,1,4,2,4,8,Up,9,8,Dn,0,8,Up,X}; // Ч
const SH_CODE char_shape_hardsign[] = {Up,6,0,Dn,6,1,0,1,0,7,1,8,2,8,3,7,3,1,Up,X}; // ъ
const SH_CODE char_shape_HARDSIGN[] = {Up,9,0,Dn,9,1,0,1,0,7,1,8,4,8,5,7,5,1,Up,X}; // Ъ
#endif

const SH_CODE *graphic_fonte_shape[256] =
	{
	// codes 0..31:
	noshape, noshape, noshape, noshape, noshape, noshape, noshape, noshape,
	noshape, noshape, noshape, noshape, noshape, noshape, noshape, noshape,
	noshape, noshape, noshape, noshape, noshape, noshape, noshape, noshape,
	noshape, noshape, noshape, noshape, noshape, noshape, noshape, noshape,
	// codes 32 .. 127:
	char_shape_space,					/* space */
	char_shape_ipoint,				/*!*/
	char_shape_openacc,
	char_shape_dieze,
	char_shape_dollar,
	char_shape_percent,
	char_shape_and,
	char_shape_el,
	char_shape_openpar,
	char_shape_closepar,
	char_shape_star,
	char_shape_plus,
	char_shape_comma,
	char_shape_minus,
	char_shape_point,
	char_shape_slash,
	char_shape_0, char_shape_1, char_shape_2, char_shape_3, char_shape_4, char_shape_5, char_shape_6, char_shape_7, char_shape_8, char_shape_9,
	char_shape_dbpoint,
	char_shape_vpoint,
	char_shape_less,
	char_shape_egal,
	char_shape_more,
	char_shape_intpoint,
	char_shape_arond,

	char_shape_A, char_shape_B, char_shape_C, char_shape_D, char_shape_E, char_shape_F, char_shape_G, char_shape_H, char_shape_I,
	char_shape_J, char_shape_K, char_shape_L, char_shape_M, char_shape_N, char_shape_O, char_shape_P, char_shape_Q, char_shape_R,
	char_shape_S, char_shape_T, char_shape_U, char_shape_V, char_shape_W, char_shape_X, char_shape_Y, char_shape_Z,

	char_shape_opencr,
	char_shape_backslash,
	char_shape_closecr,
	char_shape_xor,
	char_shape_underscore,
	char_shape_altel,

	char_shape_a, char_shape_b, char_shape_c, char_shape_d, char_shape_e, char_shape_f, char_shape_g, char_shape_h, char_shape_i,
	char_shape_j, char_shape_k, char_shape_l, char_shape_m, char_shape_n, char_shape_o, char_shape_p, char_shape_q, char_shape_r,
	char_shape_s, char_shape_t, char_shape_u, char_shape_v, char_shape_w, char_shape_x, char_shape_y, char_shape_z,

	char_shape_opbrack,
	char_shape_or,
	char_shape_closebr,
	char_shape_tilda,
	char_shape_del,
	// codes >= 128:
	noshape, noshape, noshape, noshape,		// 128..131
	noshape, noshape, noshape, noshape,	// 132..135
	noshape, noshape, noshape, noshape,	//136..139
	noshape, noshape, noshape, noshape,	//140..143
	noshape, noshape, noshape, noshape,	//144..147
	noshape, noshape, noshape, noshape,	//148..151
	noshape, noshape, noshape, noshape,	//152..155
	noshape, noshape, noshape, noshape,	//156..159

	noshape, noshape, noshape, noshape,	//160..163
	noshape, noshape, noshape, noshape,	//164..167
	noshape, noshape, noshape, noshape,	// 168..171
	noshape, noshape, noshape, noshape,	//172..175
	noshape, noshape, noshape, noshape,	//176..179
	noshape, noshape, noshape, noshape,	//180..183
	noshape, noshape, noshape, noshape,	//184..187
	noshape, noshape, noshape, noshape,	//188..191

#if defined(KICAD_CYRILLIC)
	char_shape_yu, char_shape_a, char_shape_be, char_shape_tse,
	char_shape_de, char_shape_ee, char_shape_ef, char_shape_ghe,    //196..199
	char_shape_xe, char_shape_ii, char_shape_shorti, char_shape_ka, // 200..203
	char_shape_letterel, char_shape_em, char_shape_en, char_shape_o,        //204..207
	char_shape_pe, char_shape_ya, char_shape_p, char_shape_c,       //208..211
	char_shape_te, char_shape_uu, char_shape_zhe, char_shape_ve,    //212..215
	char_shape_softsign, char_shape_yeru, char_shape_ze, char_shape_sha,    //216..219
	char_shape_ye, char_shape_shcha, char_shape_che, char_shape_hardsign,   //220..223
	char_shape_YU, char_shape_A, char_shape_BE, char_shape_TSE,             //0xE0..0xE3 224..227
	char_shape_DE, char_shape_EE, char_shape_EF, char_shape_GHE,            //228..231
	char_shape_XE, char_shape_II, char_shape_SHORTI, char_shape_KA,         //232..235
	char_shape_EL, char_shape_M, char_shape_EN, char_shape_O,               //236..239
	char_shape_PE, char_shape_YA, char_shape_P, char_shape_C,               //240..243
	char_shape_TE, char_shape_UU, char_shape_ZHE, char_shape_VE,            //244..247
	char_shape_SOFTSIGN, char_shape_YERU, char_shape_ZE, char_shape_SHA,    //248..251
	char_shape_YE, char_shape_SHCHA, char_shape_CHE, char_shape_HARDSIGN,   //252..255
#else
	noshape, noshape, noshape, noshape,	//192..195
	noshape, noshape, noshape, char_shape_C_Cedille,	//196..199
	noshape, noshape, noshape, noshape,	// 200..203
	noshape, noshape, noshape, noshape,	//204..207
	noshape, noshape, noshape, noshape,	//208..211
	noshape, noshape, noshape, noshape,	//212..215
	noshape, noshape, noshape, noshape,	//216..219
	noshape, noshape, noshape, noshape,	//220..223

	char_shape_a_grave, char_shape_a_aigu, char_shape_a_circ, noshape,	//224..227
	char_shape_a_trema, noshape, noshape, char_shape_c_cedille,	//228..231
	char_shape_e_grave, char_shape_e_aigu, char_shape_e_circ, char_shape_e_trema,	//232..235
	noshape, noshape, char_shape_i_circ, char_shape_i_trema,	//236..239
	noshape, noshape, noshape, noshape,	//240..243
	char_shape_o_circ, noshape, char_shape_o_trema, noshape,	//244..247
	noshape, char_shape_u_grave, noshape, char_shape_u_circ,	//248..251
	char_shape_u_trema, noshape, noshape, noshape,	//252..255
#endif

	} ;
#if defined(wxUSE_UNICODE) && defined(KICAD_CYRILLIC)
const int utf8_to_ascii[] =
	{
	0xE1, 0xE2, 0xF7, 0xE7, 0xE4, 0xE5, 0xF6, 0xFA,
	0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0,
	0xF2, 0xF3, 0xF4, 0xF5, 0xE6, 0xE8, 0xE3, 0xFE,
	0xFB, 0xFD, 0xFF, 0xF9, 0xF8, 0xFC, 0xE0, 0xF1,
	0xC1, 0xC2, 0xD7, 0xC7, 0xC4, 0xC5, 0xD6, 0xDA,
	0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0,
	0xD2, 0xD3, 0xD4, 0xD5, 0xC6, 0xC8, 0xC3, 0xDE,
	0xDB, 0xDD, 0xDF, 0xD9, 0xD8, 0xDC, 0xC0, 0xD1
	};
#endif
#endif
#endif	// ifndef _GRFONTE_H_

