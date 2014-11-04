#include <stdio.h>
#include <stdlib.h>

//Interface with Flex & Bison
extern FILE *yyin;              //from Flex
extern int yyparse(void);       //from Bison

int main(int argc, char** argv) {
	yyin = fopen(argv[1],"r");
	int bison_return_code = yyparse();
}

