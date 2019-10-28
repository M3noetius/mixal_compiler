// bison -d parse.y && flex lex.l && gcc parse.tab.c lex.yy.c helper_funcs.c symbols.c -lfl

%{

#include <stdio.h>
#include <stdlib.h>
#include "helper_funcs.h"
#include "symbols.h"

//#define debug(message, x, y) (if (debug) printf("%s %\n", );)
%}

%union{
	node *ast_node;
	int number_int;
	char *id;
}

%right '=' add_equ sub_equ mul_equ div_equ mod_equ
%left or
%left and
%left equ not_equ 
%left '<' smaller_equ '>' bigger_equ
%left '+' '-'
%left '*' '/' '%'
%right '!'  


%token <number_int> NUM 
%token <id> id 

%token EOC VAR_START 
%token equ not_equ smaller_equ bigger_equ add_equ sub_equ mul_equ div_equ mod_equ and or
%token IF WHILE FOR CONTINUE BREAK ELSE

%token INT
%token print

%type <ast_node> PROGRAM EXP VARS DECL DECLS  STMTS STMT SIMP BLOCK CONTROL ELSE-BLC


%start PROGRAM

%%

PROGRAM: { $$=NULL; }
	| '{' DECLS STMTS '}'  {  $$ = newnode_2(PROGRAM, $2, $3); 
							  //print_tree($$, 0); 
							  //print_symbol_table(); 
							  generate_code($$);
						   }
	;


DECLS: { $$=NULL; }
	| DECLS DECL { $$ = newnode_2(DECLS, $1, $2); }
	;


DECL: VAR_START id VARS ':' TYPE EOC { $$ = newnode_symbol($3, NULL, $2);addSymbol($2); }
	;


VARS: { $$=NULL; }
	| ',' id VARS { $$ = newnode_symbol($3, NULL, $2); addSymbol($2); } 
	;


TYPE: INT
	;


STMTS: { $$=NULL; }
	| STMTS STMT { $$ = newnode_2(STATEMENTS, $1, $2); } 
	;


STMT: SIMP EOC { $$ = newnode_2(STATEMENT, $1, NULL); }
	| CONTROL
	| EOC { $$ = newnode_2(EOC_ONLY, NULL, NULL); }
	;

// a+= EXP == a = a + EXP so create an "addition" node which has left the symbol a and right the exp 
SIMP: id '=' EXP { $$ = newnode_2(EQU_VAR, newnode_symbol(NULL,NULL, $1), $3); }
	| id mul_equ EXP { $$ = newnode_2(MUL_EQU, newnode_symbol(NULL,NULL, $1), newnode_2(MUL, newnode_symbol(NULL, NULL, $1), $3)); }
	| id div_equ EXP { $$ = newnode_2(DIV_EQU, newnode_symbol(NULL,NULL, $1), newnode_2(DIV, newnode_symbol(NULL, NULL, $1), $3)); }
	| id add_equ EXP { $$ = newnode_2(ADD_EQU, newnode_symbol(NULL,NULL, $1), newnode_2(ADD, newnode_symbol(NULL, NULL, $1), $3)); }
	| id sub_equ EXP { $$ = newnode_2(SUB_EQU, newnode_symbol(NULL,NULL, $1), newnode_2(SUB, newnode_symbol(NULL, NULL, $1), $3)); }
	| id mod_equ EXP { $$ = newnode_2(MOD_EQU, newnode_symbol(NULL,NULL, $1), newnode_2(MOD, newnode_symbol(NULL, NULL, $1), $3)); }
	| print EXP { $$ = newnode_2(PRINT, NULL, $2); }
	;

CONTROL: IF '(' EXP ')' BLOCK ELSE-BLC { $$ = newnode_3(_IF ,$3, $5, $6); }
	| WHILE '(' EXP ')' BLOCK { $$ = newnode_2(_WHILE, $3, $5); }
	| FOR '(' SIMP EOC EXP EOC SIMP ')' BLOCK { $$ = newnode_4(_FOR ,$3, $5, $7, $9); }
	| BREAK EOC { $$ = newnode_2(_BREAK, NULL, NULL); }
	| CONTINUE EOC { $$ = newnode_2(_CONTINUE, NULL, NULL); }
	;

ELSE-BLC: { $$=NULL; }
	| ELSE BLOCK { $$ = newnode_2(ELSE_BLC, $2, NULL); }
	;

BLOCK: STMT { $$ = newnode_2(BLOCK, $1, NULL); }
	| '{' STMTS '}' { $$ = newnode_2(BLOCK, $2, NULL); }
	;

//Simple Math staff
EXP: '(' EXP ')' { $$ = $2; }
	| EXP '+' EXP { $$ = newnode_2(ADD, $1, $3); }
	| EXP '-' EXP { $$ = newnode_2(SUB, $1, $3); }
	| EXP '/' EXP { $$ = newnode_2(DIV, $1, $3); }
	| EXP '*' EXP { $$ = newnode_2(MUL, $1, $3); }
	| EXP '%' EXP { $$ = newnode_2(MOD, $1, $3); }
	// Hack negative with zero - num
	| '-' EXP { $$ = newnode_2(NEGATIVE, newnode_num(0), $2); }
	| EXP '<' EXP { $$ = newnode_2(SMALLER, $1, $3); }
	| EXP '>' EXP { $$ = newnode_2(BIGGER, $1, $3); }
	| EXP smaller_equ EXP { $$ = newnode_2(SMALLER_EQU, $1, $3); }
	| EXP bigger_equ EXP  { $$ = newnode_2(BIGGER_EQU, $1, $3); }
	| EXP equ EXP         { $$ = newnode_2(EQU, $1, $3); }
	| EXP not_equ EXP     { $$ = newnode_2(NOT_EQU, $1, $3); }
	
	| '!' EXP { $$ = newnode_2(NOT, NULL, $2); }
	| EXP and EXP 		  { $$ = newnode_2(AND, $1, $3); }
	| EXP or EXP 		  { $$ = newnode_2(OR, $1, $3); }
	
	| id  { $$ = newnode_symbol(NULL, NULL, $1); }
	| NUM { $$ = newnode_num($1); }
	;


%%

main(int argc, char **argv)
{
	//Initialize symbol table
	initiallize_symbols();
	yyparse();
}


yyerror(char *s)
{
	fprintf(stderr, "error: %s\n", s);
}


