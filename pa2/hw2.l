    /* calculator #1 */
%{
    #include "y.tab.h"
    #include <stdlib.h>
    void yyerror(char *);
%}

%%
"line" {
		return LINE;
	    }
[0-9]+      {
                yylval = atoi(yytext);
                return INT;
            }
">="	    {
		return GEQ;
	    }
"<="	    {
		return LEQ;
	    }
">"	    {
		return GT;
	    }
"<"	    {
	  	return LT;
	    }
"=="	    {
		return EQ;
	    }
"!="	    {
		return NEQ;
	    }
"+"	    {
		return ADD;
	    }
"-"	    {
		return SUB;
	    } 
"*"	    {
		return MULT;
	    }
"/"	    {
		return DIV;
	    } 	
"or"	    {
		return OR;
	    }	
"and"	    {
	   	return AND;
	    }
"not"	    {
		return NOT;
	    }
	    
"("	    {
		return LEFTP;
 	    }
")"	    {
		return RIGHTP;
	    }
[\n]      { return *yytext; }

[ \t]       ;       /* skip whitespace */

.           {yyerror("Unknown character");}

%%

int yywrap(void) {
     	
    return 1;
}
