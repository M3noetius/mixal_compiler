%{
#include "helper_funcs.h"
#include "parse.tab.h"
%}

%%

"(" |
")" |
"{" |
"}" { return yytext[0]; }

"var" { return VAR_START; }
"," |
":"   { return yytext[0]; }
"int" { return INT; }

"print"    { return print; }
"if" 	   { return IF; }
"for" 	   { return FOR; }
"while"    { return WHILE; }
"continue" { return CONTINUE; }
"break"    { return BREAK; }
"else"     { return ELSE; }


"="  { return yytext[0]; }
"+=" { return add_equ; }
"-=" { return sub_equ; }
"*=" { return mul_equ; }
"/=" { return div_equ; }
"%=" { return mod_equ; }

"+" |
"-" |
"*" |
"/" |
"%" |
"<" |
">" { return yytext[0]; }

">=" { return bigger_equ; }
"<=" { return smaller_equ; }
"==" { return equ; }
"!=" { return not_equ; }

"!" { return yytext[0]; }

"&&" { return and; }
"||" { return or; }



";" {return EOC;}

[0-9]+   { yylval.number_int = atoi(yytext); return NUM;}
[a-zA-Z][a-zA-Z]* { char *s = malloc(sizeof(yytext)+1);
					strcpy(s, yytext);
					s[sizeof(yytext)] = '\0';
					yylval.id = s; 
					return id;}

. {;}
%%