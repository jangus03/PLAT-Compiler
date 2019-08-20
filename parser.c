/*File name: parser.c
* Compiler: MS Visual Studio 2015 (x86)
* Author: Justin Angus - 040690837
* Course: CST-8152 Compilers, Lab Section: 14
* Assignment: Assignment #3
* Date: Decemebr 6th, 2018
* Professor: Sv. Ranev
* Purpose: The purpose is to create a RDPP building on top of the scanner. (Warning because of the single line comment in stdio.h)
* Function list: void program(void); Token malar_next_token(void);void parser(void);
* void match(int pr_token_code, int pr_token_attribute);void syn_eh(int sync_token_code);void syn_printe();
* void gen_incode(char* string);void opt_statements(void);void statements(void);void statements_p(void);
* void statement(void);void assignment_statement(void);void assignment_expression(void);
* void selection_statement(void);void iteration_statement(void);void precondition(void);
* void input_statement(void);void variable_list(void);void variable_list_p(void);void variable_identifier(void);
* void output_statement(void);void output_list(void);void arithmetic_expression(void);void unary_arithmetic_expression(void);
* void additive_arithmetic_expression(void);void additive_arithmetic_expression_p(void);
* void multiplicative_arithmetic_expression(void);void multiplicative_arithmetic_expression_p(void);
* void primary_arithmetic_expression(void);void string_expression(void);void string_expression_p(void);
* void primary_string_expression(void);void conditional_expression(void);void logical_or_expression(void);
* void logical_or_expression_p(void);void logical_and_expression(void);void logical_and_expression_p(void);
* void relational_expression(void);void relational_operator(void);void primary_a_relational_expression(void);
* void primary_s_relational_expression(void);
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

/* project header files */
#include "parser.h"

extern int line;			/*defined in scanner.c*/
extern Buffer * str_LTBL;	/*defined in platy_st.c*/
extern char * kw_table[];	/*defined in table.h*/

/* Purpose: To match the grammar and make sure the lookahead code is the code that the parser wants.
* Author: Justin Angus
* History/Versions: Version 1.0 Decemeber 6th, 2018
* Called functions: malar_next_token(); syn_eh(pr_token_code); syn_printe();
* Parameters: int: pr_token_code, int: pr_token_attribute
* Return value: void
* Algorithm: If the pr_token_code is KW_T or LOG_OP_T or ART_OP_T or REL_OP_T, try to match both code and attribute. If now
* try to just match code. If it there is no match print error and return, if there is check for SEOF and ERR_T.
*/
void match(int pr_token_code, int pr_token_attribute) {
	/*Create a "boolean"*/
	int match = NO_MATCH;

	/*If the pr_token_code is one of the four below match both code and attribute*/
	if (pr_token_code == KW_T || pr_token_code == LOG_OP_T || pr_token_code == ART_OP_T || pr_token_code == REL_OP_T) {
		if (lookahead.code == pr_token_code && lookahead.attribute.get_int == pr_token_attribute)
			match = MATCH;
	}
	/*Otherwise just match the code*/
	else {
		if (lookahead.code == pr_token_code)
			match = MATCH;
	}

	/*If there isnt a match it will print a error and just leave the function*/
	if (match == NO_MATCH) {
		syn_eh(pr_token_code);
		return;
	}

	if (lookahead.code == SEOF_T)
		return;

	lookahead = malar_next_token();

	/*If there is an error do the follow and call the next token because you don't want it in the Paser*/
	if (lookahead.code == ERR_T) {
		syn_printe();
		lookahead = malar_next_token();
		synerrno++;
	}
}

/* Purpose: This function acts as our "error recovery"
* Author: Justin Angus
* History/Versions: Version 1.0 Decemeber 6th, 2018
* Called functions: syn_printe(); exit(synerrno);  malar_next_token();
* Parameters: int: sync_token_code
* Return value: void
* Algorithm: While the lookahead code is not the sync_token_code get the next token (aka find a safe place).
* Also check for SEOF so we don't overrun the buffer.
*/
void syn_eh(int sync_token_code) {
	/*Call syn_printe and increment the error counter*/
	syn_printe();
	++synerrno;

	/*Keep advancing until lookahead matches the token code required by the parser*/
	while (lookahead.code != sync_token_code) {
		/*If the required token is not SEOF and SEOF is found*/
		if (lookahead.code == SEOF_T && sync_token_code != SEOF_T) {
			exit(synerrno);
		}
		lookahead = malar_next_token();
	}

	/*If the match is found and the required token code is not SEOF, advance and return*/
	if (sync_token_code != SEOF_T) {
		lookahead = malar_next_token();
		return;
	}

	/*If it is SEOF, just return*/
	return;
}

/* error printing function for Assignment 3 (Parser), F18 */
void syn_printe() {
	Token t = lookahead;

	printf("PLATY: Syntax error:  Line:%3d\n", line);
	printf("*****  Token code:%3d Attribute: ", t.code);
	switch (t.code) {
	case  ERR_T: /* ERR_T     0   Error token */
		printf("%s\n", t.attribute.err_lex);
		break;
	case  SEOF_T: /*SEOF_T    1   Source end-of-file token */
		printf("SEOF_T\t\t%d\t\n", t.attribute.seof);
		break;
	case  AVID_T: /* AVID_T    2   Arithmetic Variable identifier token */
	case  SVID_T:/* SVID_T    3  String Variable identifier token */
		printf("%s\n", t.attribute.vid_lex);
		break;
	case  FPL_T: /* FPL_T     4  Floating point literal token */
		printf("%5.1f\n", t.attribute.flt_value);
		break;
	case INL_T: /* INL_T      5   Integer literal token */
		printf("%d\n", t.attribute.get_int);
		break;
	case STR_T:/* STR_T     6   String literal token */
		printf("%s\n", b_location(str_LTBL, t.attribute.str_offset));
		break;
	case SCC_OP_T: /* 7   String concatenation operator token */
		printf("NA\n");
		break;
	case  ASS_OP_T:/* ASS_OP_T  8   Assignment operator token */
		printf("NA\n");
		break;
	case  ART_OP_T:/* ART_OP_T  9   Arithmetic operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  REL_OP_T: /*REL_OP_T  10   Relational operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  LOG_OP_T:/*LOG_OP_T 11  Logical operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  LPR_T: /*LPR_T    12  Left parenthesis token */
		printf("NA\n");
		break;
	case  RPR_T: /*RPR_T    13  Right parenthesis token */
		printf("NA\n");
		break;
	case LBR_T: /*    14   Left brace token */
		printf("NA\n");
		break;
	case RBR_T: /*    15  Right brace token */
		printf("NA\n");
		break;
	case KW_T: /*     16   Keyword token */
		printf("%s\n", kw_table[t.attribute.get_int]);
		break;
	case COM_T: /* 17   Comma token */
		printf("NA\n");
		break;
	case EOS_T: /*    18  End of statement *(semi - colon) */
		printf("NA\n");
		break;
	default:
		printf("PLATY: Scanner error: invalid token code: %d\n", t.code);
	}/*end switch*/
}/* end syn_printe()*/

 /* Purpose: This function takes a string, "output", and prints it.
 * Author: Justin Angus
 * History/Versions: Version 1.0 Decemeber 6th, 2018
 * Called functions: N/A
 * Parameters: int: sync_token_code
 * Return value: void
 */
void gen_incode(char* output) {
	printf("%s\n", output);
}

void parser(void) {
	/*FIRST set: {KW_T(PLATYPUS)}*/
	lookahead = malar_next_token();
	program(); match(SEOF_T, NO_ATTR);
	gen_incode("PLATY: Source file parsed");
}

void program(void) {
	/*FIRST set: {KW_T(PLATYPUS)}*/
	match(KW_T, PLATYPUS); match(LBR_T, NO_ATTR); opt_statements();
	match(RBR_T, NO_ATTR);
	gen_incode("PLATY: Program parsed");
}

void opt_statements() {
	/* FIRST set: {AVID_T,SVID_T,KW_T(but not ? see above),e} */
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T: statements(); break;
	case KW_T:
		/* check for PLATYPUS, ELSE, THEN, REPEAT, TRUE, FALSE here
		and in statements_p()*/
		if (lookahead.attribute.get_int != PLATYPUS
			&& lookahead.attribute.get_int != ELSE
			&& lookahead.attribute.get_int != THEN
			&& lookahead.attribute.get_int != REPEAT
			&& lookahead.attribute.get_int != TRUE
			&& lookahead.attribute.get_int != FALSE) {
			statements();
			break;
		}
	default: /*empty string ? optional statements*/;
		gen_incode("PLATY: Opt_statements parsed");
	}
}

void statements(void) {
	/*FIRST set: {AVID, SVID, KW_T(IF),  KW_T(WHILE), KW_T(READ), KW_T(WRITE)}*/
	statement();
	statements_p();
}

void statements_p(void) {
	/*FIRST set: {AVID, SVID, KW_T(IF), KW_T(WHILE), KW_T(READ), KW_T(WRITE), e}*/
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T: statement(); statements_p(); break;
	case KW_T:
		if (lookahead.attribute.get_int != PLATYPUS
			&& lookahead.attribute.get_int != ELSE
			&& lookahead.attribute.get_int != THEN
			&& lookahead.attribute.get_int != REPEAT
			&& lookahead.attribute.get_int != TRUE
			&& lookahead.attribute.get_int != FALSE) {
			statement(); statements_p();
			break;
		}
	}
}

void statement(void) {
	/*FIRST set: {AVID,SVID, KW_T(IF), KW_T(WHILE), KW_T(READ), KW_T(WRITE)}*/
	if (lookahead.code == AVID_T || lookahead.code == SVID_T) {
		assignment_statement();
	}
	else if (lookahead.code == KW_T && lookahead.attribute.get_int == IF) {
		selection_statement();
	}
	else if (lookahead.code == KW_T && lookahead.attribute.get_int == WHILE) {
		iteration_statement();
	}
	else if (lookahead.code == KW_T && lookahead.attribute.get_int == READ) {
		input_statement();
	}
	else if (lookahead.code == KW_T && lookahead.attribute.get_int == WRITE) {
		output_statement();
	}
	else {/*empty string ? optional statements*/
		syn_printe();
	}
}

void assignment_statement(void) {
	/*FIRST set: {AVID,SVID}*/
	switch (lookahead.code) {
	case SVID_T:
	case AVID_T:assignment_expression(); match(EOS_T, NO_ATTR); break;
	default: /*empty string ? optional statements*/;
		syn_printe();
	}
	gen_incode("PLATY: Assignment statement parsed");
}

void assignment_expression(void) {
	/*FIRST set: {AVID,SVID}*/
	switch (lookahead.code) {
	case AVID_T:
		match(AVID_T, NO_ATTR); match(ASS_OP_T, NO_ATTR); arithmetic_expression();
		gen_incode("PLATY: Assignment expression (arithmetic) parsed"); break;
	case SVID_T:
		match(SVID_T, NO_ATTR); match(ASS_OP_T, NO_ATTR); string_expression();
		gen_incode("PLATY: Assignment expression (string) parsed"); break;
	default: /*empty string ? optional statements*/
		syn_printe();
	}
}

void selection_statement(void) {
	/*FIRST set: {KW_T(IF)}*/
	match(KW_T, IF); precondition(); match(LPR_T, NO_ATTR);
	conditional_expression(); match(RPR_T, NO_ATTR); match(KW_T, THEN);
	match(LBR_T, NO_ATTR); opt_statements(); match(RBR_T, NO_ATTR);
	match(KW_T, ELSE); match(LBR_T, NO_ATTR); opt_statements();
	match(RBR_T, NO_ATTR); match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Selection statement parsed");
}

void iteration_statement(void) {
	/*FIRST set: {KW_T(WHILE)}*/
	match(KW_T, WHILE); precondition(); match(LPR_T, NO_ATTR);
	conditional_expression(); match(RPR_T, NO_ATTR); match(KW_T, REPEAT);
	match(LBR_T, NO_ATTR); statements(); match(RBR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Iteration statement parsed");
}

void precondition(void) {
	/*FIRST set: {KW_T(TRUE), KW_T(FALSE)}*/
	switch (lookahead.code) {
	case KW_T:
		if (lookahead.attribute.get_int == TRUE) {
			match(KW_T, TRUE); break;
		}
		else if (lookahead.attribute.get_int == FALSE) {
			match(KW_T, FALSE); break;
		}
	default: /*empty string ? optional statements*/
		syn_printe();
	}
}

void input_statement(void) {
	/*FIRST set: {KW_T(READ)}*/
	match(KW_T, READ); match(LPR_T, NO_ATTR); variable_list();
	match(RPR_T, NO_ATTR); match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Input statement parsed");
}

void variable_list(void) {
	/*FIRST set: {AVID_T, SVID_T}*/
	variable_identifier();
	variable_list_p();
	gen_incode("PLATY: Variable list parsed");
}

void variable_list_p(void) {
	/*FIRST set: {COM_T, e}*/
	switch (lookahead.code) {
	case COM_T: match(COM_T, NO_ATTR); variable_identifier(); variable_list_p();
	}
}

void variable_identifier(void) {
	/*FIRST set: {AVID_T, SVID_T}*/
	switch (lookahead.code) {
	case AVID_T: match(AVID_T, NO_ATTR); break;
	case SVID_T: match(SVID_T, NO_ATTR); break;
	default: syn_printe();
	}
}

void output_statement(void) {
	/*FIRST set: {KW_T(WRITE)}*/
	match(KW_T, WRITE); match(LPR_T, NO_ATTR);
	output_list(); match(RPR_T, NO_ATTR); match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Output statement parsed");
}

void output_list(void) {
	/*FIRST set: {AVID_T, SVID_T, STR_L, e}*/
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T: variable_list(); break;
	case STR_T: match(STR_T, NO_ATTR); gen_incode("PLATY: Output list (string literal) parsed"); break;
	default:
		/*empty string ? optional statements*/
		gen_incode("PLATY: Output list (empty) parsed"); break;
	}
}

void arithmetic_expression(void) {
	/*FIRST set: {ART_OP_T(PLUS), ART_OP_T(MINUS), AVID_T, FPL_T, INL_T, LPR_T}*/
	switch (lookahead.code) {
	case ART_OP_T:
		if (lookahead.attribute.get_int == PLUS || lookahead.attribute.get_int == MINUS) {
			unary_arithmetic_expression(); break;
		}
	case AVID_T:
	case FPL_T:
	case INL_T:
	case LPR_T: additive_arithmetic_expression(); break;
	default: /*empty string ? optional statements*/
		syn_printe();
	}
	gen_incode("PLATY: Arithmetic expression parsed");
}

void unary_arithmetic_expression(void) {
	/*FIRST set: {ART_OP_T(PLUS), ART_OP_T(MINUS)}*/
	switch (lookahead.attribute.get_int) {
	case PLUS: match(ART_OP_T, PLUS); primary_arithmetic_expression(); break;
	case MINUS: match(ART_OP_T, MINUS); primary_arithmetic_expression(); break;
	default: /*empty string ? optional statements*/
		syn_printe();
	}
	gen_incode("PLATY: Unary arithmetic expression parsed");
}

void additive_arithmetic_expression(void) {
	/*FIRST set: {AVID_T, FPL_T, INL_T, LPR_T}*/
	multiplicative_arithmetic_expression();
	additive_arithmetic_expression_p();
}

void additive_arithmetic_expression_p(void) {
	/*FIRST set: {ART_OP_T(PLUS), ART_OP_T(MINUS), e}*/
	switch (lookahead.code) {
	case ART_OP_T:
		if (lookahead.attribute.get_int == PLUS) {/*here it was a "+"*/
			match(ART_OP_T, PLUS); /*here it is a "."*/
			multiplicative_arithmetic_expression();
			additive_arithmetic_expression_p();
			gen_incode("PLATY: Additive arithmetic expression parsed");
			break;
		}
		else if (lookahead.attribute.get_int == MINUS) {
			match(ART_OP_T, MINUS); multiplicative_arithmetic_expression();
			additive_arithmetic_expression_p();
			gen_incode("PLATY: Additive arithmetic expression parsed");
			break;
		}
	}
}

void multiplicative_arithmetic_expression(void) {
	/*FIRST set: {AVID_T, FPL_T, INL_T, LPR_T}*/
	primary_arithmetic_expression();
	multiplicative_arithmetic_expression_p();
}

void multiplicative_arithmetic_expression_p(void) {
	/*FIRST set: {ART_OP_T(MULT), ART_OP_T(DIV), e}*/
	switch (lookahead.code) {
	case ART_OP_T:
		if (lookahead.attribute.get_int == MULT) {
			match(ART_OP_T, MULT); primary_arithmetic_expression();
			multiplicative_arithmetic_expression_p();
			gen_incode("PLATY: Multiplicative arithmetic expression parsed");
			break;

		}
		else if (lookahead.attribute.get_int == DIV) {
			match(ART_OP_T, DIV); primary_arithmetic_expression();
			multiplicative_arithmetic_expression_p();
			gen_incode("PLATY: Multiplicative arithmetic expression parsed");
			break;
		}
	}
}

void primary_arithmetic_expression(void) {
	/*FIRST set: {AVID_T, FPL_T, INL_T, LPR_T}*/
	switch (lookahead.code) {
	case AVID_T: match(AVID_T, NO_ATTR); break;
	case FPL_T: match(FPL_T, NO_ATTR); break;
	case INL_T: match(INL_T, NO_ATTR); break;
	case LPR_T: match(LPR_T, NO_ATTR); arithmetic_expression(); match(RPR_T, NO_ATTR); break;
	default: /*empty string ? optional statements*/
		syn_printe();
	}
	gen_incode("PLATY: Primary arithmetic expression parsed");
}

void string_expression(void) {
	/*FIRST set: {SVID_T, STR_T}*/
	primary_string_expression();
	string_expression_p();
	gen_incode("PLATY: String expression parsed");
}

void string_expression_p(void) {
	/*FIRST set: {SCC_OP_T, e}*/
	switch (lookahead.code) {
	case SCC_OP_T: match(SCC_OP_T, NO_ATTR); primary_string_expression(); string_expression_p(); break;
	}
}

void primary_string_expression(void) {
	/*FIRST set: {SVID_T, STR_T}*/
	switch (lookahead.code) {
	case SVID_T: match(SVID_T, NO_ATTR); break;
	case STR_T: match(STR_T, NO_ATTR); break;
	default: /*empty string ? optional statements*/
		syn_printe();
	}
	gen_incode("PLATY: Primary string expression parsed");
}

void conditional_expression(void) {
	logical_or_expression();
	gen_incode("PLATY: Conditional expression parsed");
}

void logical_or_expression(void) {
	/*FIRST set: {AVID_T, FPL_T, INL_T, SVID_T, STR_T}*/
	logical_and_expression();
	logical_or_expression_p();
}

void logical_or_expression_p(void) {
	/*FIRST set: {REL_OP_T(OR), e}*/
	switch (lookahead.code) {
	case LOG_OP_T:
		if (lookahead.attribute.get_int == OR) {
			match(LOG_OP_T, OR); logical_and_expression();
			logical_or_expression_p();
			gen_incode("PLATY: Logical OR expression parsed");
			break;
		}
	}
}

void logical_and_expression(void) {
	/*FIRST set: {AVID_T, FPL_T, INL_T, SVID_T, STR_T}*/
	relational_expression();
	logical_and_expression_p();
}

void logical_and_expression_p(void) {
	/*FIRST set: {REL_OP_T(AND), e}*/
	switch (lookahead.code) {
	case LOG_OP_T:
		if (lookahead.attribute.get_int == AND) {
			match(LOG_OP_T, AND); relational_expression();
			logical_and_expression_p();
			gen_incode("PLATY: Logical AND expression parsed");
			break;
		}
	}
}

void relational_expression(void) {
	/*FIRST set: {AVID_T, FPL_T, INL_T, SVID_T, STR_T}*/
	switch (lookahead.code) {
	case AVID_T:
	case FPL_T:
	case INL_T: primary_a_relational_expression(); relational_operator(); primary_a_relational_expression(); break;
	case SVID_T:
	case STR_T: primary_s_relational_expression(); relational_operator(); primary_s_relational_expression(); break;
	default: /*empty string ? optional statements*/
		syn_printe();
	}
	gen_incode("PLATY: Relational expression parsed");
}

void relational_operator(void) {
	/*FIRST set: {==, <>, >, < }*/
	switch (lookahead.code) {
	case REL_OP_T:
		if (lookahead.attribute.get_int == EQ) {
			match(REL_OP_T, EQ);
		}
		else if (lookahead.attribute.get_int == NE) {
			match(REL_OP_T, NE);
		}
		else if (lookahead.attribute.get_int == GT) {
			match(REL_OP_T, GT);
		}
		else if (lookahead.attribute.get_int == LT) {
			match(REL_OP_T, LT);
		}
		else {
			syn_printe();
		}
	}
}

void primary_a_relational_expression(void) {
	/*FIRST set: {AVID_T, FPL_T, INL_T}*/
	switch (lookahead.code) {
	case AVID_T: match(AVID_T, NO_ATTR); break;
	case FPL_T: match(FPL_T, NO_ATTR); break;
	case INL_T: match(INL_T, NO_ATTR);  break;
	default: /*empty string ? optional statements*/
		syn_printe();
	}
	gen_incode("PLATY: Primary a_relational expression parsed");
}

void primary_s_relational_expression(void) {
	/*FIRST set: {SVID_T, STR_T}*/
	switch (lookahead.code) {
	case SVID_T:
	case STR_T: primary_string_expression(); break;
	default: /*empty string ? optional statements*/
		syn_printe();
	}
	gen_incode("PLATY: Primary s_relational expression parsed");
}
