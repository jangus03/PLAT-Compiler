/*File name: buffer.h
* Compiler: MS Visual Studio 2015 (x86)
* Author: Justin Angus - 040690837
* Course: CST 8152 – Compilers, Lab Section: 14
* Assignment: Assignment #2
* Date: November 5th, 2018
* Professor: Sv. Ranev
* Purpose: Preprocessor directives, type declarations and prototypes necessary for buffer implementation
* as required for CST8152-Assignment #1.
*/
#ifndef BUFFER_H_
#define BUFFER_H_

/*#pragma warning(1:4001) *//*to enforce C89 type comments  - to make //comments an warning */

/*#pragma warning(error:4001)*//* to enforce C89 comments - to make // comments an error */

/* standard header files */
#include <stdio.h>  /* standard input/output */
#include <malloc.h> /* for dynamic memory allocation*/
#include <limits.h> /* implementation-defined data type ranges and limits */


														   /*constants*/
#define MODE_FAIL 10			/* fail mode return value*/
#define RT_FAIL_1 -1			/* fail return value */
#define RT_FAIL_2 -2			/* fail return value */
#define LOAD_FAIL -2			/* load fail return value */
#define BUFFER_FULL 1			/* full buffer return value */
#define BUFFER_NOT_FULL 0		/* not full buffer return value */
#define BUFFER_IS_EMPTY 1		/* empty buffer return value */
#define BUFFER_IS_NOT_EMPTY 0	/* not empty return value */
#define RT_FAIL_HEX 0x100		/* fail value in hex */
#define EOB_RETURN_VAL 0		/* eob return value */
#define REWIND_PASS 0			/* passed rewind reture value */
#define CLEAR_PASS 0			/* passed clear return value */
#define FIXED 0					/* fixed value */
#define ADDITIVE 1				/* additive value */
#define MULTIPLICATIVE -1		/* multiplicative value */
#define SET_EOB_BIT 1			/* Value of a set EOB bit */
#define SHRT_MAX_N_1 SHRT_MAX -1

#define DEFAULT_FLAGS 0xFFFC		/*default flags value*/
#define SET_EOB  0x01				/*set eob mask*/
#define RESET_EOB 0xFFFE			/*reset eob mask*/
#define CHECK_EOB 0x01				/*check eob mask*/
#define SET_R_FLAG  0x02			/*set r_flag mask*/
#define RESET_R_FLAG 0xFFFD			/*reset r_flag mask*/
#define CHECK_R_FLAG 0X02			/*check r_flag mask*/


typedef struct BufferDescriptor {
	char *cb_head;			/* pointer to the beginning of character array (character buffer) */
	short capacity;			/* current dynamic memory size (in bytes) allocated to character buffer */
	short addc_offset;		/* the offset (in chars) to the add-character location */
	short getc_offset;		/* the offset (in chars) to the get-character location */
	short markc_offset;		/* the offset (in chars) to the mark location */
	char  inc_factor;		/* character array increment factor */
	char  mode;				/* operational mode indicator */
	unsigned short flags;	/* contains character array reallocation flag and end-of-buffer flag */
} Buffer, *pBuffer;


/* function declarations */
Buffer * b_allocate(short init_capacity, char inc_factor, char o_mode);
pBuffer b_addc(pBuffer const pBD, char symbol);
int b_clear(Buffer * const pBD);
void b_free(Buffer * const pBD);
int b_isfull(Buffer * const pBD);
short b_limit(Buffer * const pBD);
short b_capacity(Buffer * const pBD);
short b_mark(pBuffer const pBD, short mark);
int b_mode(Buffer * const pBD);
size_t b_incfactor(Buffer * const pBD);
int b_load(FILE * const fi, Buffer * const pBD);
int b_isempty(Buffer * const pBD);
char b_getc(Buffer * const pBD);
int b_eob(Buffer * const pBD);
int b_print(Buffer * const pBD);
Buffer * b_compact(Buffer * const pBD, char symbol);
char b_rflag(Buffer * const pBD);
short b_retract(Buffer * const pBD);
short b_reset(Buffer * const pBD);
short b_getcoffset(Buffer * const pBD);
int b_rewind(Buffer * const pBD);
char * b_location(Buffer * const pBD, short loc_offset);

#endif

