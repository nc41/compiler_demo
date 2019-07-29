%{
#include <string.h>
#include <stdio.h>
#include "att.h"
#include "parser.tab.h"
#define YY_SKIP_YYWRAP
extern int lineNum;
%}

%%

"int"	return INT;
"char"	return CHAR;
'([^'\\\n]|\\.)' {yylval.variable.c = yytext; return CHARCONST;}
'.*' {return ERRORT;}
[0-9]+ {yylval.variable.num = atoi(yytext); return NUMBER;}
procedure return PROCEDURE;

"{"	return OPEN_CB;
"}"	return CLOSE_CB;
"("	return OPEN_ROUND_BRACKETS;
")"	return CLOSE_ROUND_BRACKETS;
"["	return OPEN_SQUARE_BRACKETS;
"]"	return CLOSE_SQUARE_BRACKETS;
":"	return COLON;

[ \t]+	;
[\n]	{lineNum++;}
"and"	return AND;
"or"	return OR;
"if"	return IF;
"then"	return THEN;
"while" return WHILE;
"write" return WRITE;
"<"	return LT;
"<="	return LE;
"!="	return NE;
">"	return GT;
">="	return GE;
"read"	return READ;
"to"	return TO;
"+"	return ADD;
"-"	{return MINUS;}
"*"	return MUL;
"/"	return DIVIDE;
"not"	return NOT;
"for"	return FOR;
"else"	return ELSE;

"by"	return BY;
"=" 	return EQUAL;
"=="	return EQ;
";"	return SEMI_COLON;
","	return COMMA;
"//"[^\n]* ;
[a-zA-Z][a-zA-Z0-9]* { yylval.variable.c = (char*) malloc(strlen(yytext) + 5); strcpy(yylval.variable.c, yytext); return NAME;}


%%


int yywrap()
{
	return 1;
}





