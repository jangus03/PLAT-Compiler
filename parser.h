/*File name: parser.h
* Compiler: MS Visual Studio 2015 (x86)
* Author: Justin Angus - 040690837
* Course: CST 8152 – Compilers, Lab Section: 14
* Assignment: Assignment #3
* Date: December 6th, 2018
* Professor: Sv. Ranev
* Purpose: Preprocessor directives, type declarations and prototypes necessary for RDPP implementation
* as required for CST8152-Assignment #3.
*/
#ifndef  PARSER_H_
#define  PARSER_H_ 

#ifndef TOKEN_H_
#include "token.h"
#endif

#ifndef BUFFER_H_
#include "buffer.h"
#endif

#define NO_ATTR 99
#define MATCH 1
#define NO_MATCH 0

/* globals variables */
static Token lookahead;
int synerrno;

/*Keywords by index*/
enum KeyWordByIndex { ELSE, FALSE, IF, PLATYPUS, READ, REPEAT, THEN, TRUE, WHILE, WRITE };

/* function declarations */
Token malar_next_token(void);
void parser(void);
void match(int pr_token_code, int pr_token_attribute);
void syn_eh(int sync_token_code);
void syn_printe();
void gen_incode(char* string);

/*Grammar functions*/
void program(void);
void opt_statements(void);
void statements(void);
void statements_p(void);
void statement(void);
void assignment_statement(void);
void assignment_expression(void);
void selection_statement(void);
void iteration_statement(void);
void precondition(void);
void input_statement(void);
void variable_list(void);
void variable_list_p(void);
void variable_identifier(void);
void output_statement(void);
void output_list(void);
void arithmetic_expression(void);
void unary_arithmetic_expression(void);
void additive_arithmetic_expression(void);
void additive_arithmetic_expression_p(void);
void multiplicative_arithmetic_expression(void);
void multiplicative_arithmetic_expression_p(void);
void primary_arithmetic_expression(void);
void string_expression(void);
void string_expression_p(void);
void primary_string_expression(void);
void conditional_expression(void);
void logical_or_expression(void);
void logical_or_expression_p(void);
void logical_and_expression(void);
void logical_and_expression_p(void);
void relational_expression(void);
void relational_operator(void);
void primary_a_relational_expression(void);
void primary_s_relational_expression(void);
#endif
