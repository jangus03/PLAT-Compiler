/*File name: scanner.c
* Compiler: MS Visual Studio 2015 (x86)
* Author: Justin Angus - 040690837
* Course: CST 8152 – Compilers, Lab Section: 14
* Assignment: Assignment #2
* Date: November 5th, 2018
* Professor: Sv. Ranev
* Purpose: To implement a Lexical Analyzer by scanning the contents of a buffer and creating tokens using
* a token driver scanner and a Fitnite State Machine.
* Function list: int scanner_init(); Token malar_next_token(); int get_next_state(); int char_class();
* Token aa_func02(); Token aa_func03(); Token aa_func05(); Token aa_func08(); Token aa_func10(); Token aa_func11();
* int iskeyword();
*/

/* THERE WILL BE A "stdio.h(1): warning C4001:" BECAUSE THERE IS A SINGLE LINE COMMENT IN THAT FILE
* WHICH IS NOT ANSCII C COMPLIANT, THIS IS THE SOURCE OF THE WARNING!!!
*/

/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
* to suppress the warnings about using "unsafe" functions like fopen()
* and standard sting library functions defined in string.h.
* The define does not have any effect in Borland compiler projects.
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */

/*#define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */
#include "buffer.h"
#include "token.h"
#include "table.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG

/* Global objects - variables */
/* This buffer is used as a repository for string literals.
It is defined in platy_st.c */
extern Buffer * str_LTBL;	/*String literal table */
int line;					/* current line number of the source code */
extern int scerrnum;		/* defined in platy_st.c - run-time error number */

/* Local(file) global objects - variables */
static Buffer *lex_buf;		/*pointer to temporary lexeme buffer*/
static pBuffer sc_buf;		/*pointer to input source buffer*/
/* No other global variable declarations/definitiond are allowed

/* scanner.c static(local) function  prototypes */
static int char_class(char c);					/* character class function */
static int get_next_state(int, char, int *);	/* state machine function */
static int iskeyword(char * kw_lexeme); /*keywords lookup functuion */


/*Initializes scanner */
int scanner_init(Buffer * psc_buf) {
	if (b_isempty(psc_buf)) return EXIT_FAILURE;/*1*/
	/* in case the buffer has been read previously  */
	b_rewind(psc_buf);
	b_clear(str_LTBL);
	line = 1;
	sc_buf = psc_buf;
	return EXIT_SUCCESS;/*0*/
	/*   scerrnum = 0;  *//*no need - global ANSI C */
}

/* Purpose: It “reads” the lexeme from the input stream (in our case from the input buffer) one character at a time, and
* returns a token structure any time it finds a token pattern (as defined in the lexical grammar) which matches the lexeme
* found in the stream of input symbols
* Author: Justin Angus
* History/Versions: Version 1.0 November 8th, 2018
* Called functions: b_getc(), b_retract(), b_mark(), b_reset(), strcmp(), b_allocate(), b_compact(), get_next_state(), b_free()
* Parameters: void
* Return value: The token which was created by either the DFA or token driven scanner.
* Algorithm: This function uses a case statement for the token driven scanner, returning the tokens one by one. The DFA uses a do-while
* loop that iterates though the input stream until it hits an accepting state. The lex-start and lex-end determine the lexeme which is
* stored in the lex_buf then passed into one of the accepting state functions. The return value of the accepting functions is stored in
* Token t; then returned to be displayed.
*/
Token malar_next_token(void) {
	Token t = { 0 };		/* token to return after pattern recognition. Set all structure members to 0 */
	unsigned char c;		/* input symbol */
	int state = 0;			/* initial state of the FSM */
	short lexstart;			/* start offset of a lexeme in the input char buffer (array) */
	short lexend;			/* end offset of a lexeme in the input char buffer (array)*/
	int accept = NOAS;		/* type of state - initially not accepting */
	char test;				/* To test if any of the chars between and ! and \n are valid*/
	int error = 0;			/* Set to one if there is a comment error then return t*/
	char or [4];			/* Temp String to check for logical OR*/
	char and[5];			/* Temp String to check for logical AND*/

	while (1) { /* endless loop broken by token returns it will generate a warning */

		/*Get the next symbol from the input buffer*/
		c = b_getc(sc_buf);

		/*Part 1: Implementation of token driven scanner*/
		/*Every token is possessed by its own dedicated code*/
		switch (c) {
		case '+':
			t.code = ART_OP_T;
			t.attribute.arr_op = PLUS;
			return t;

		case '-':
			t.code = ART_OP_T;
			t.attribute.arr_op = MINUS;
			return t;

		case '*':
			t.code = ART_OP_T;
			t.attribute.arr_op = MULT;
			return t;

		case '/':
			t.code = ART_OP_T;
			t.attribute.arr_op = DIV;
			return t;

		case '!':
			/*Set test to the next char*/
			test = b_getc(sc_buf);

			/*Check to see if the next char is an error, if so set the error "flag"*/
			if (test != '!') {
				t.code = ERR_T;
				t.attribute.err_lex[0] = '!';
				t.attribute.err_lex[1] = test;
				error = COMMENT_ERROR;
			}
			/*If the next char is SEOF retract and return the code and attribute set above*/
			else if (test == SEOF_1 || test == SEOF_2) {
				b_retract(sc_buf);
				return t;
			}

			/*Loop and discard the rest of the comment (until new line), also check
			if there is an SEOF, set the appropriate attributes*/
			while (1) {
				test = b_getc(sc_buf);
				if (test == '\n') {
					++line;
					break;
				}
				else if (test == '\r') {
					if (b_getc(sc_buf) == '\n') {
						line++;
						break;
					}
					b_retract(sc_buf);
					++line;
				}
				else if (test == '\0' || test == 255) {
					b_retract(sc_buf);
					t.code = ERR_T;
					t.attribute.err_lex[0] = '!';
					t.attribute.err_lex[1] = '!';
					return t;
				}
			}

			/*If a comment error was set return t, if not continue*/
			if (error == COMMENT_ERROR) {
				return t;
			}
			continue;

		case '.':
			/*Set a mark where getc_offset is*/
			b_mark(sc_buf, b_getcoffset(sc_buf));

			/*Add the next 3 chars into an array*/
			for (int i = 0; i < 3; i++) {
				or [i] = b_getc(sc_buf);
			}
			or [3] = '\0';

			/*Check if the chars added to the array match OR. if so it is a logical OR statement*/
			if (strcmp(or , "OR.") == 0) {
				t.code = LOG_OP_T;
				t.attribute.log_op = OR;
				return t;
			}

			/*If not reset getc_offset to markc_offset*/
			b_reset(sc_buf);

			/*Add the next 4 chars into an array*/
			for (int i = 0; i < 4; i++) {
				and[i] = b_getc(sc_buf);
			}
			and[4] = '\0';

			/*Check if the chars added to the array match AND. if so it is a logical AND statement*/
			if (strcmp(and, "AND.") == 0) {
				t.code = LOG_OP_T;
				t.attribute.log_op = AND;
				return t;
			}

			/*If not reset getc_offset to markc_offset and set an error token with attribute*/
			b_reset(sc_buf);
			t.code = ERR_T;
			t.attribute.err_lex[0] = '.';
			return t;

		case '=':
			/*If the next char is another = it is EQ, if not it an assignment opperator and retract*/
			if (b_getc(sc_buf) == '=') {
				t.code = REL_OP_T;
				t.attribute.rel_op = EQ;
			}
			else {
				t.code = ASS_OP_T;
				b_retract(sc_buf);
			}
			return t;

		case '(':
			t.code = LPR_T;
			return t;

		case ')':
			t.code = RPR_T;
			return t;

		case '{':
			t.code = LBR_T;
			return t;

		case '}':
			t.code = RBR_T;
			return t;

		case '>':
			t.code = REL_OP_T;
			t.attribute.rel_op = GT;
			return t;

		case '<':
			/*If the next char is > it is not equals, if not it is less then and retract*/
			if (b_getc(sc_buf) == '>') {
				t.code = REL_OP_T;
				t.attribute.rel_op = NE;
			}
			else {
				t.code = REL_OP_T;
				t.attribute.rel_op = LT;
				b_retract(sc_buf);
			}
			return t;

		case ';':
			t.code = EOS_T;
			return t;

		case ',':
			t.code = COM_T;
			return t;

		case '#':
			t.code = SCC_OP_T;
			return t;

		case ' ':
			continue;

		case '\t':
			continue;

		case '\n':
			++line;
			continue;

		case '\v':
			continue;

		case '\f':
			continue;

		case '\r':
			if (b_getc(sc_buf) == '\n') {
				line++;
				break;
			}
			b_retract(sc_buf);
			++line;
			continue;

		case '\0':
			t.code = SEOF_T;
			t.attribute.seof = SEOF1;
			return t;

		case 255:
			t.code = SEOF_T;
			t.attribute.seof = SEOF2;
			return t;
		}

		/*Part 2: Implemenation of Finite state Machine (DFA) or Transition Table driven scanner*/
		/*Set the lexstart*/
		lexstart = b_mark(sc_buf, b_getcoffset(sc_buf) - 1);

		/*Loop until accepting state is reached*/
		do {
			state = get_next_state(state, c, &accept);
			/*If an accepting state is reached break and don't get the next char */
			if (accept != NOAS) {
				break;
			}
			c = b_getc(sc_buf);
		} while (accept == NOAS);

		/*Retract if it is ASWR*/
		if (accept == ASWR) {
			b_retract(sc_buf);
		}

		/*lexend is equal to the mark of getc_offset*/
		lexend = b_mark(sc_buf, b_getcoffset(sc_buf));

		/*Allocate space for the lex_buf*/
		lex_buf = b_allocate(lexend - lexstart + (short)1, FIXED, 'f');

		if (lex_buf == NULL) {
			scerrnum = SC_ERROR;
			t.code = RTE_T;
			strcpy(t.attribute.err_lex, "RUN TIME ERROR: ");
			return t;
		}

		/*Set markc_offset to lexstart with the appropriate function*/
		b_mark(sc_buf, lexstart);

		/*Set getc_offset to markc_offset which was just set to lexstart (so getc_offset is now at lexstart)*/
		b_reset(sc_buf);

		/*Loop until all the chars are added to the lex_buf*/
		while (b_getcoffset(sc_buf) < lexend) {
			b_addc(lex_buf, b_getc(sc_buf));
		}

		/*Compact and add \0*/
		b_compact(lex_buf, '\0');

		/*Call the accepting function of index state and pass in the lex_buf*/
		t = aa_table[state](b_location(lex_buf, 0));

		b_free(lex_buf);
		return t;

	}//end while(1)
}

int get_next_state(int state, char c, int *accept) {
	int col;
	int next;
	col = char_class(c);
	next = st_table[state][col];
#ifdef DEBUG
	printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
#endif
	assert(next != IS);
#ifdef DEBUG
	if (next == IS) {
		printf("Scanner Error: Illegal state:\n");
		printf("Input symbol: %c Row: %d Column: %d\n", c, state, col);
		exit(1);
	}
#endif
	*accept = as_table[next];
	return next;
}

/* Purpose: This function returns the column number in the transition table st_table for the input char c
* Author: Justin Angus
* History/Versions: Version 1.0 November 8th, 2018
* Called functions: N/A
* Parameters: char c; which is the character passed into char_class from the input stream
* Return value: This function returns the coulmn number in the transition table, this will determine
* if the state is an accepting state or not
* Algorithm: This function check the if the character is in a specific range or matches a specific symbol
* specified in our transition table, if the character is a match the function returns the column number.
*/
int char_class(char c) {
	/*the column number is stored in val*/
	int val;

	/*If c is between a-z or A-Z it is column 0*/
	if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') {
		val = 0;
	}
	/*If c is 0 it is column 1*/
	else if (c == '0') {
		val = 1;
	}
	/*If c is between 1-9 it is coulmn 2*/
	else if (c >= '1' && c <= '9') {
		val = 2;
	}
	/*If c '.' it is coulmn 3*/
	else if (c == '.') {
		val = 3;
	}
	/*If c is '$' it is coulmn 4*/
	else if (c == '$') {
		val = 4;
	}
	/*If c is " it is coulmn 5*/
	else if (c == '"') {
		val = 5;
	}
	/*If c is SEOF it is coulmn 6*/
	else if (c == '\0' || c == 255) {
		val = 6;
	}
	/*If c is anything else it is column 7*/
	else {
		val = 7;
	}
	/*Return the value of the column*/
	return val;
}

/* Purpose: This function checks to see if the lexeme passed in is either a KW_T or a VID
* Author: Justin Angus
* History/Versions: Version 1.0 November 8th, 2018
* Called functions: strlen(), isKeyword()
* Parameters: char* lexeme; which is passed in from the malar_next_token function
* Return value: This function returns Token t; which will either be a AVID_T or KW_T
* Algorithm: This function checks to see if the lexeme is a keyword using the isKeyword function,
* if the isKeyword function returns something other then NOT_A_KEYWORD the lexeme is processed as a keyword.
* This function then checks the length if the lexeme and if it is longer then VID_LEN if so,
* the function truncates it.
*/
Token aa_func02(char *lexeme) {
	Token t = { 0 };/* token to return after pattern recognition. Set all structure members to 0 */
	int charAmount = strlen(lexeme);/*Set charAmount to the lexeme's length*/

	/*Check if the lexeme is a keyword*/
	/*If the method call returns anything other then -1 it has found a match*/
	if (iskeyword(lexeme) != NOT_A_KEYWORD) {
		t.code = KW_T;
		/*Return the index where the match was aka. the keyword*/
		t.attribute.kwt_idx = iskeyword(lexeme);
		return t;
	}

	/*Set a AVID Token*/
	t.code = AVID_T;

	/*If lexeme's length is grearer then VID_LEN set charAmount to VID_LEN*/
	if (strlen(lexeme) > VID_LEN) {
		charAmount = VID_LEN;
	}

	/*Loop from 0-charAmount adding the char's from lexeme to vid_lex every iteration*/
	for (int i = 0; i < charAmount; i++) {
		t.attribute.vid_lex[i] = lexeme[i];
	}

	/*Add \0 to make C-Style string*/
	t.attribute.vid_lex[charAmount] = '\0';
	return t;
}

/* Purpose: This function check to see if the lexeme passed in is a SVID
* Author: Justin Angus
* History/Versions: Version 1.0 November 8th, 2018
* Called functions: strlen()
* Parameters: char* lexeme; which is passed in from the malar_next_token function
* Return value: This function returns Token t; which will be a SVID_T
* Algorithm: This function checks the length if the lexeme and if it is longer then VID_LEN if so,
* the function truncates it and adds $ on the end.
*/
Token aa_func03(char *lexeme) {
	Token t = { 0 };/* token to return after pattern recognition. Set all structure members to 0 */
	int charAmount = strlen(lexeme);/*Set charAmount to the lexeme's length*/

	/*Set SVID Token*/
	t.code = SVID_T;

	/*If lexeme's length is grearer then VID_LEN set charAmount to VID_LEN-1*/
	if (strlen(lexeme) > VID_LEN) {
		charAmount = VID_LEN - 1;
	}

	/*Loop from 0-charAmount adding the char's from lexeme to vid_lex every iteration*/
	for (int i = 0; i < charAmount; i++) {
		t.attribute.vid_lex[i] = lexeme[i];
	}

	/*If the SVID is greater then the VID_LENGTH append $ & \0 to the end of the name*/
	if (strlen(lexeme) > VID_LEN) {
		t.attribute.vid_lex[charAmount] = '$';
		t.attribute.vid_lex[charAmount + 1] = '\0';
		return t;
	}

	/*Add \0 to make C-Style string*/
	t.attribute.vid_lex[charAmount] = '\0';
	return t;
}

/* Purpose: The function checks to see if the lexeme passed in is a INL_T
* Author: Justin Angus
* History/Versions: Version 1.0 November 8th, 2018
* Called functions: atoi(), aa_func11()
* Parameters: char* lexeme; which is passed in from the malar_next_token function
* Return value: This function returns Token t; which will either be a INT_L or ERR_T
* Algorithm: This function converts the lexeme to a long and checks the lexeme's length,
* if the length is greater then SHORT_MAX_LENGTH or if the long is greater then SHRT_MAX
* or smaller then SHRT_MIN then it is invalid and processed by the error function. Otherwise the function
* proceeds .
*/
Token aa_func05(char *lexeme) {
	Token t = { 0 };/* token to return after pattern recognition. Set all structure members to 0 */
	int charAmount = strlen(lexeme);/*Set charAmount to the lexeme's length*/
	long l = atol(lexeme);//Hold initial value to check overflow
	short i;/*To hold the int value that will be returned*/

	/*Check if value is longer then 5 chars which would be bigger then SHRT_MAX and also check if it is less then 99999*/
	if (charAmount > INL_LEN || l > SHRT_MAX) {
		t = aa_func11(lexeme);
		return t;
	}
	/*Cast the long to an int*/
	i = (short)l;
	t.code = INL_T;
	t.attribute.int_value = i;
	return t;
}

/* Purpose: The function checks to see if the lexeme passed in is a FPL_T
* Author: Justin Angus
* History/Versions: Version 1.0 November 8th, 2018
* Called functions: atof(), aa_func11()
* Parameters: char* lexeme; which is passed in from the malar_next_token function
* Return value: This function returns Token t; which will either be a FPL_T or ERR_T
* Algorithm: This function convets the lexeme to a double and checks if the the value can
* properly be converted to a float, if it cannot is send the lexeme to the error function.
*/
Token aa_func08(char *lexeme) {
	Token t = { 0 };/* token to return after pattern recognition. Set all structure members to 0 */
	double d = atof(lexeme);/*To test the size of the return value of atof(lexeme) */
	float f;/*To hold the float value that will be returned*/

	/*MUST CHANGE LOGIC TO ERROR HANDLE Checking if "test" is in range of a 4-byte float*/
	if (d > FLT_MAX || d < FLT_MIN && d != 0) {
		t = aa_func11(lexeme);
		return t;
	}

	/*Cast the double to a float*/
	f = (float)d;

	t.code = FPL_T;
	t.attribute.flt_value = f;
	return t;
}

/* Purpose: The function checks to see if the lexeme passed in is SL
* Author: Justin Angus
* History/Versions: Version 1.0 November 8th, 2018
* Called functions: b_limit(), b_addc(), strlen()
* Parameters: char* lexeme; which is passed in from the malar_next_token function
* Return value: This function returns Token t; which will either be a STR_L
* Algorithm: This function adds the lexeme into the next available spot of the string literal table
* using b_limit() which is addc_offset (next spot) then using a loop and b_addc(), adds the lexeme to
* the string literal table.
*/
Token aa_func10(char *lexeme) {
	Token t = { 0 };/* token to return after pattern recognition. Set all structure members to 0 */

	/*The offset is where you can add the next String aka. addc_offset*/
	t.attribute.str_offset = b_limit(str_LTBL);

	/*Adding the lexeme to the SL table, omitting the quotes*/
	for (unsigned int i = 1; i < strlen(lexeme) - 1; i++) {
		b_addc(str_LTBL, lexeme[i]);
		if (lexeme[i] == '\n') {
			++line;
		}
		else if (lexeme[i] == '\r') {
			if (lexeme[i + 1] == '\n') {
				++i;
			}
			++line;
		}
	}

	/*Must add a terminator*/
	b_addc(str_LTBL, '\0');

	t.code = STR_T;
	return t;
}

/* Purpose: The function checks to see if the lexeme passed in is a ERR_T
* Author: Justin Angus
* History/Versions: Version 1.0 November 8th, 2018
* Called functions: strlen()
* Parameters: char* lexeme; which is passed in from the malar_next_token function
* Return value: This function returns Token t; which will be a ERR_T
* Algorithm: This function checks the length if the lexeme and if it is longer then ERR_LEN if so,
* the function truncates it and adds ... on the end.
*/
Token aa_func11(char *lexeme) {
	Token t = { 0 };/* token to return after pattern recognition. Set all structure members to 0 */
	int charAmount = strlen(lexeme);/*Set charAmount to the lexeme's length*/

	t.code = ERR_T;

	/*Check the WHOLE lexeme for new lines*/
	for (int i = 0; i < charAmount; i++) {
		if (lexeme[i] == '\n') {
			++line;
		}
		else if (lexeme[i] == '\r') {
			if (lexeme[i + 1] == '\n') {
				++i;
			}
			++line;
		}
	}

	/*If lexeme's length is grearer then ERR_LEN set charAmount to ERR_LEN-3*/
	if (strlen(lexeme) > ERR_LEN) {
		charAmount = ERR_LEN - 3;
	}

	for (int i = 0; i < charAmount; i++) {
		t.attribute.vid_lex[i] = lexeme[i];
	}

	/*If the ERR_T is greater then the VID_LENGTH append $ & \0 to the end of the name*/
	if (strlen(lexeme) > VID_LEN) {
		t.attribute.vid_lex[charAmount] = '.';
		t.attribute.vid_lex[charAmount + 1] = '.';
		t.attribute.vid_lex[charAmount + 2] = '.';
		t.attribute.vid_lex[charAmount + 3] = '\0';
		return t;
	}

	/*Add \0 to make C-Style string*/
	t.attribute.vid_lex[charAmount] = '\0';

	return t;
}

/* Purpose: The function checks to see if the lexeme passed in is a ERR_T
* Author: Justin Angus
* History/Versions: Version 1.0 November 8th, 2018
* Called functions: strlen()
* Parameters: char* lexeme; which is passed in from the malar_next_token function
* Return value: This function returns Token t; which will be a ERR_T
* Algorithm: This function checks the length if the lexeme and if it is longer then ERR_LEN if so,
* the function truncates it and adds ... on the end.
*/
Token aa_func12(char *lexeme) {
	Token t = { 0 };/* token to return after pattern recognition. Set all structure members to 0 */
	int charAmount = strlen(lexeme);/*Set charAmount to the lexeme's length*/

	t.code = ERR_T;

	/*Check the WHOLE lexeme for new lines*/
	for (int i = 0; i < charAmount; i++) {
		if (lexeme[i] == '\n') {
			++line;
		}
		else if (lexeme[i] == '\r') {
			if (lexeme[i + 1] == '\n') {
				++i;
			}
			++line;
		}
	}

	/*If lexeme's length is grearer then ERR_LEN set charAmount to ERR_LEN-3*/
	if (strlen(lexeme) > ERR_LEN) {
		charAmount = ERR_LEN - 3;
	}

	for (int i = 0; i < charAmount; i++) {
		t.attribute.vid_lex[i] = lexeme[i];
	}

	/*If the ERR_T is greater then the VID_LENGTH append $ & \0 to the end of the name*/
	if (strlen(lexeme) > VID_LEN) {
		t.attribute.vid_lex[charAmount] = '.';
		t.attribute.vid_lex[charAmount + 1] = '.';
		t.attribute.vid_lex[charAmount + 2] = '.';
		t.attribute.vid_lex[charAmount + 3] = '\0';
		return t;
	}

	/*Add \0 to make C-Style string*/
	t.attribute.vid_lex[charAmount] = '\0';

	return t;
}

/* Purpose: The function checks to see if the lexeme passed in is a KW_T
* Author: Justin Angus
* History/Versions: Version 1.0 November 8th, 2018
* Called functions: strcmp()
* Parameters: char* kw_lexeme; which is passed in from the aa_func02 function
* Return value: This function either returns the index of where the keyword was found or NOT_A_KEYWORD
* Algorithm: This function checks to see if the lemexe matches any of the key words in the kw_table,
* if there is a match the function returns the index of where it was found. Otherwise if there is no
* match it returns NOT_A_KEYWORD.
*/
int iskeyword(char * kw_lexeme) {
	/*Iterate through the kw_table seeing if any key words match kw_lexeme, if so, return that index of the array*/
	for (int i = 0; i < KWT_SIZE; i++) {
		if (strcmp(kw_lexeme, kw_table[i]) == 0) {
			return i;
		}
	}
	/*If is doesnt match return -1*/
	return NOT_A_KEYWORD;
}