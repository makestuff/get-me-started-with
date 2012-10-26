#include <stdio.h>
#include <ctype.h>
#define YYSTYPE double
#include "rpcalc.tab.h"

int yyparse(void);

int main(void) {
	return yyparse();
}

/* Lexer function: read characters, generate tokens */
int yylex(void) {
	int ch;

	/* Skip white space. */
	do {
		ch = getchar();
	} while ( ch == ' ' || ch == '\t' );

	/* Process numbers. */
	if ( ch == '.' || isdigit(ch) ) {
		ungetc(ch, stdin);
		scanf("%lf", &yylval);
		return NUM;
	}

	/* Return end-of-input. */
	if ( ch == EOF ) {
		return 0;
	}

	/* Return a single char. */
	return ch;
}

/* Called by yyparse on error. */
void yyerror(const char *s) {
	fprintf(stderr, "%s\n", s);
}
