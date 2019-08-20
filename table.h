/*File name: table.h
* Compiler: MS Visual Studio 2015 (x86)
* Author: Justin Angus - 040690837
* Course: CST 8152 – Compilers, Lab Section: 14
* Assignment: Assignment #2
* Date: November 5th, 2018
* Professor: Sv. Ranev
* Purpose: Preprocessor directives, type declarations and prototypes necessary for finite state machine implementation
* as required for CST8152-Assignment #2.
*/
#ifndef  TABLE_H_
#define  TABLE_H_ 

#ifndef BUFFER_H_
#include "buffer.h"
#endif

#ifndef NULL
#include <_null.h> /* NULL pointer constant is defined there */
#endif

/*   Source end-of-file (SEOF) sentinel symbol
*    '\0' or one of 255,0xFF,EOF
*/

/*  Special case tokens processed separately one by one
*  in the token-driven part of the scanner
*  '=' , ' ' , '(' , ')' , '{' , '}' , == , <> , '>' , '<' , ';',
*  white space
*  !!comment , ',' , ';' , '-' , '+' , '*' , '/', # ,
*  .AND., .OR. , SEOF, 'illegal symbol',
*/

/*Other defines*/
#define SC_ERROR 99
#define FIXED 0
#define COMMENT_ERROR 1
#define SEOF_1 '\0'
#define SEOF_2 255

/*Define error state #*/
#define ES  11	/* Error state  with no retract */
#define ER  12	/* Error state  with retract */
#define IS -1	/* Inavalid state */

/* State transition table definition */
#define TABLE_COLUMNS 8
/*transition table - type of states defined in separate table */
int  st_table[][TABLE_COLUMNS] = {
	{ 1,6,4,ES,ES,9,IS,ES },		/* State 0 */
	{ 1,1,1,2,3,2,2,2 },			/* State 1 */
	{ IS,IS,IS,IS,IS,IS,IS,IS },	/* State 2 */
	{ IS,IS,IS,IS,IS,IS,IS,IS },	/* State 3 */
	{ ES,4,4,7,5,ES,5,5 },			/* State 4 */
	{ IS,IS,IS,IS,IS,IS,IS,IS },	/* State 5 */
	{ ES,6,ES,7,ES,ES,5,5 },		/* State 6 */
	{ 8,7,7,8,8,ES,8,8 },			/* State 7 */
	{ IS,IS,IS,IS,IS,IS,IS,IS },	/* State 8 */
	{ 9,9,9,9,9,10,ER,9 },			/* State 9 */
	{ IS,IS,IS,IS,IS,IS,IS,IS },	/* State 10 */
	{ IS,IS,IS,IS,IS,IS,IS,IS },	/* State 11 */
	{ IS,IS,IS,IS,IS,IS,IS,IS },	/* State 12 */
};

/* Accepting state table definition */
#define ASWR	20	/* accepting state with retract */
#define ASNR	21  /* accepting state with no retract */
#define NOAS	22  /* not accepting state */
int as_table[] = { NOAS,NOAS,ASWR,ASNR,NOAS,ASWR,NOAS,NOAS,ASWR,NOAS,ASNR,ASNR,ASWR };

/* Accepting action function declarations */
Token aa_func02(char *lexeme);	/* State 2 */
Token aa_func03(char *lexeme);	/* State 3 */
Token aa_func05(char *lexeme);	/* State 5 */
Token aa_func08(char *lexeme);	/* State 8 */
Token aa_func10(char *lexeme);	/* State 10 */
Token aa_func11(char *lexeme);	/* State 11 */
Token aa_func12(char *lexeme);	/* State 12 */

/*defining a new type: pointer to function (of one char * argument) returning Token*/
typedef Token(*PTR_AAF)(char *lexeme);


/* Accepting function (action) callback table (array) definition */
/* If you do not want to use the typedef, the equvalent declaration is:
* Token (*aa_table[])(char lexeme[]) = {
*/
PTR_AAF aa_table[] = {//set not accepting to null
	NULL,			/* State 0 */
	NULL,			/* State 1 */
	aa_func02,		/* State 2 */
	aa_func03,		/* State 3 */
	NULL,			/* State 4 */
	aa_func05,		/* State 5 */
	NULL,			/* State 6 */
	NULL,			/* State 7 */
	aa_func08,		/* State 8 */
	NULL,			/* State 9 */
	aa_func10,		/* State 10 */
	aa_func11,		/* State 11 */
	aa_func12,		/* State 12 */
};

/* Keyword lookup table (.AND. and .OR. are not keywords) */
#define KWT_SIZE  10
#define NOT_A_KEYWORD -1
char * kw_table[] = {
	"ELSE",
	"FALSE",
	"IF",
	"PLATYPUS",
	"READ",
	"REPEAT",
	"THEN",
	"TRUE",
	"WHILE",
	"WRITE"
};

#endif
