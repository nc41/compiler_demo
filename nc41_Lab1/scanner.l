%{
#include<stdio.h>
#include "y.tab.h"
extern int lineNum;
%}

%%

"int"	return INT;
"char"	return CHAR;
'([^'\\\n]|\\.)' {return CHARCONST;}
'.*' {return ERRORT;}
[0-9]+ return NUMBER;
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
"/"	return DIV;
"not"	return NOT;
"for"	return FOR;
"else"	return ELSE;

"by"	return BY;
"=" 	return EQUAL;
"=="	return EQ;
";"	return SEMI_COLON;
","	return COMMA;
"//"[^\n]* ;
[a-zA-Z][a-zA-Z0-9]* {return NAME;}


%%


int yywrap()
{
	return 1;
}





