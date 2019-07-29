%{
#define YYERROR_VERBOSE
#include<stdio.h>
#include<string.h>
int lineNum = 1;
int errorNum = 0;

%}
%union {
int integer;
char *s;
}

%token <int> INT
%token <int> CHAR
%token <int> AND OR
%token <int> IF THEN ELSE
%token <int> FOR
%token <int> PROCEDURE
%token <int> READ
%token <int> TO
%token <int> WHILE
%token <int> WRITE
%token <int> BY
%token <int> ADD
%token <int> MINUS
%token <int> MUL
%token <int> DIV
%token <int> LT
%token <int> LE
%token <int> EQ
%token <int> GT
%token <int> GE
%token <int> NE
%token <int> OPEN_CB
%token <int> CLOSE_CB
%token <int> COLON
%token <int> SEMI_COLON
%token <int> OPEN_SQUARE_BRACKETS
%token <int> CLOSE_SQUARE_BRACKETS
%token <int> OPEN_ROUND_BRACKETS
%token <int> CLOSE_ROUND_BRACKETS
%token <int> EQUAL
%token <int> COMMA
%token <int> NAME
%token <int> NUMBER
%token <int> CHARCONST
%token <int> NOT
%token <int> ERRORT


%type <int> Procedure
%type <int> Decls
%type <int> Decl
%type <int> Type
%type <int> SpecList
%type <int> Spec

%type <int> Exprs
%type <int> Expr
%type <int> Term
%type <int> Bounds
%type <int> Bound
%type <int> Stmts
%type <int> Stmt
%type <int> AndTerm
%type <int> Bool
%type <int> OrTerm
%type <int> Factor
%type <int> RelExpr
%type <int> Reference
%type <int> WithElse

%start Procedure

%%

Type: INT|CHAR;
Reference: NAME | NAME OPEN_SQUARE_BRACKETS Exprs CLOSE_SQUARE_BRACKETS;

Expr: Expr ADD Term | Expr MINUS Term | Term;
Exprs:Expr COMMA Exprs | Expr;
Term: Term MUL Factor | Term DIV Factor | Factor;
Stmt: Reference EQUAL Expr SEMI_COLON
	| OPEN_CB Stmts CLOSE_CB
	| WHILE OPEN_ROUND_BRACKETS Bool CLOSE_ROUND_BRACKETS OPEN_CB Stmts CLOSE_CB
	| READ Reference SEMI_COLON
	| WRITE Expr SEMI_COLON
	| FOR NAME EQUAL Expr TO Expr BY Expr OPEN_CB Stmts CLOSE_CB
	| IF OPEN_ROUND_BRACKETS Bool CLOSE_ROUND_BRACKETS THEN Stmt
	| IF OPEN_ROUND_BRACKETS Bool CLOSE_ROUND_BRACKETS THEN WithElse ELSE Stmt
	| OPEN_CB CLOSE_CB {yyerror("Empty Statement");}
	| OPEN_CB SEMI_COLON CLOSE_CB {yyerror("Empty Statement");}
	| error SEMI_COLON{yyerrok;};

WithElse: IF OPEN_ROUND_BRACKETS Bool CLOSE_ROUND_BRACKETS THEN WithElse ELSE WithElse
	| Reference EQUAL Expr SEMI_COLON
	| OPEN_CB Stmts CLOSE_CB
	| WHILE OPEN_ROUND_BRACKETS Bool CLOSE_ROUND_BRACKETS OPEN_CB Stmts CLOSE_CB
        | READ Reference SEMI_COLON
        | WRITE Expr SEMI_COLON
        | FOR NAME EQUAL Expr TO Expr BY Expr OPEN_CB Stmts CLOSE_CB;

RelExpr: Expr | RelExpr LT Expr | RelExpr LE Expr | RelExpr EQ Expr | RelExpr NE Expr | RelExpr GE Expr | RelExpr GT Expr;

Bool: NOT OrTerm | OrTerm;
OrTerm: OrTerm OR OrTerm | AndTerm;
AndTerm: AndTerm AND RelExpr | RelExpr; 

Factor: OPEN_ROUND_BRACKETS Expr CLOSE_ROUND_BRACKETS | Reference | NUMBER | CHARCONST | ERRORT {yyerror("char just expects to have one character.");};

Spec: NAME
	| NAME OPEN_SQUARE_BRACKETS Bounds CLOSE_SQUARE_BRACKETS;

Bound: NUMBER COLON NUMBER
Bounds: Bounds COMMA Bound
	| Bound;

SpecList:Spec
	| SpecList COMMA Spec;

Decl: Type SpecList;

Decls: Decl SEMI_COLON 
	| Decls Decl SEMI_COLON
	| error SEMI_COLON{yyerrok;};	
	
Stmts: Stmt
	| Stmts Stmt
	
Procedure: PROCEDURE NAME OPEN_CB Decls Stmts CLOSE_CB;


%%

int yyerror(const char*s)
{
	errorNum += 1;
	printf("Line %d: %s\n", lineNum, s);
}

int main(int argc, char* argv[])
{
	extern FILE *yyin;
	if (argc == 2) {
		if (!strcmp("-h", argv[1])) {
			printf("-h : ./demo [file name] will execute the compile with that file.\n");
			printf("The compile result will present through stdout.\n");
			printf("If you do not add file name after the compiler, it will accept stdin instead.\n");
			return 0;
		} else {
			FILE *input;
			input=fopen(argv[1], "r");
			yyin = input;
		}
	} else {
		yyin = stdin;
	}
	
	errorNum = 0;
	yyparse();
	if (errorNum == 0){
		printf("parse successfully\n");
	}
	else{
		printf("parse fail and total errors: %d\n", errorNum);
	}
	
	return 0;
}
