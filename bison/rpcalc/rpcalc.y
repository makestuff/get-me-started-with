/* Reverse polish notation calculator. */

%{
	#define YYSTYPE double
	#include <stdio.h>
	#include <math.h>
	extern int yylex(void);
	extern void yyerror(const char *);
%}

%token NUM

%% /* Grammar rules and actions follow. */

input:
  /* empty */
| input line
;

line:
  '\n'
  | exp '\n' { printf("%.10g\n", $1); }
;

exp:
    NUM     { $$ = $1;          }
  | exp exp '+' { $$ = $1 + $2;     }
  | exp exp '-' { $$ = $1 - $2;     }
  | exp exp '*' { $$ = $1 * $2;     }
  | exp exp '/' { $$ = $1 / $2;     }
  | exp exp '^' { $$ = pow($1, $2); }
  | exp     'n' { $$ = -$1;         }
;

%%
