%{

#include "representation.h" /* used in parser.ypp */
#include <string.h>	/* for strcmp, strdup & friends */
#include <stdio.h>	/* for strcmp, strdup & friends */
#include <stdlib.h>	/* for atoi() */

#include "parser.h"	/* (TK_) symbols from parser.y */

extern int line_number;	/* defined in parser.ypp */

%}

%x line_comment
%x comment

%option noyywrap
%option nounput

%%
[ \t\r]		    	    	break;		/* ignore white space */ 
"\n" 			    		++line_number;

"("                         return LPAREN;
")"                         return RPAREN;
"{"                         return LBRACE;
"}"                         return RBRACE;

"piece"			    		return PIECE;
":="                        return DEF;
"||"                	    return SEQ;
","                 	    return SEP;
":"                 	    return COL;

"Name :"                    return H_NAME;
"Description :"             return H_DESCR;

"start"                     return START;
"stop"                      return STOP;
"trapezoid"                 return TRAPEZOID;
"link"                      return LINK;
"split"                     return SPLIT;

"eof"                       return END_OF_FILE;


"//"                        BEGIN(line_comment);   //
<line_comment>[^\n"eof"]    break;                 //
<line_comment>\n            {                      // Line comments handling
                              BEGIN(INITIAL);      //
			      			  line_number++;       //
                            }                      //
<line_comment>"eof"         return END_OF_FILE;	   //


"/*"                        BEGIN(comment);        //
<comment>[^"*\""eof""\n"]   break;                 //
<comment>"\*"               BEGIN(INITIAL);        // Comments handling
<comment>"eof"       	    return END_OF_FILE;	   //


<comment>\n		    		line_number++;

-?[0-9]+                      { 
                              yylval.integer = atoi(yytext); 
                              return INT; 
                            }

[A-Za-z_]([A-Za-z0-9_])*    {
                              yylval.name = strdup(yytext);
                              return NAME;
                            }


\"[^\n]*\"                  {
                              yylval.name = strdup(yytext);
                              return DESCR ; 
                            }

.			    			{ 
							  fprintf(stderr,"line %d: error: illegal character %s\n", line_number, strdup(yytext)) ;
                              exit(1);
                            }

